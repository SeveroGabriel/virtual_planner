#!/usr/bin/env bash
#
# db-migrate.sh - aplica as migrations SQL de back-end/migrations/ no banco
# PostgreSQL configurado por variáveis de ambiente.
#
# Uso:
#   ./scripts/db-migrate.sh
#
# Variáveis de ambiente (defaults compatíveis com back-end/.env.example e
# docker-compose.yml):
#   POSTGRES_HOST     (default: localhost)
#   POSTGRES_PORT     (default: 5432)
#   POSTGRES_DB       (default: virtual_planner)
#   POSTGRES_USER     (default: virtual_planner)
#   POSTGRES_PASSWORD (OBRIGATÓRIA, sem default: o script aborta sem ela)
#   POSTGRES_SSLMODE  (default: disable)
#   MIGRATIONS_DIR    (default: back-end/migrations relativo a este script)
#
# Regras:
#   - As migrations são ordenadas numericamente pelo prefixo do arquivo
#     (NNN_nome.sql), não lexicograficamente, então 010 não fica antes de 002.
#   - Idempotente: migrations já registradas na tabela schema_migrations são
#     puladas. Rodar o script duas vezes seguidas é seguro.
#   - Cada migration roda dentro de uma transação (BEGIN/COMMIT). Se ela
#     falhar, o script para imediatamente e nada é registrado como aplicado
#     (a transação nunca é comitada e o PostgreSQL desfaz o que não foi
#     comitado ao encerrar a conexão).
#   - Nunca imprime a senha nem a connection string completa.
#   - Nunca faz DROP nem apaga dados: apenas executa os arquivos .sql como
#     estão e registra o que foi aplicado.
#
# Requer o cliente `psql` no PATH. Usamos `psql` puro (em vez de
# `docker compose exec`) porque este script também roda no CI do GitHub
# Actions, onde o PostgreSQL é um service container acessível via rede, sem
# `docker compose` disponível.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

POSTGRES_HOST="${POSTGRES_HOST:-localhost}"
POSTGRES_PORT="${POSTGRES_PORT:-5432}"
POSTGRES_DB="${POSTGRES_DB:-virtual_planner}"
POSTGRES_USER="${POSTGRES_USER:-virtual_planner}"
POSTGRES_SSLMODE="${POSTGRES_SSLMODE:-disable}"
MIGRATIONS_DIR="${MIGRATIONS_DIR:-${REPO_ROOT}/back-end/migrations}"

# Sem default de propósito. Um placeholder que funciona é pior que nenhum:
# ele conecta, o script segue, e a senha publicada neste repositório vira a
# credencial real do banco sem ninguém notar.
if [ -z "${POSTGRES_PASSWORD:-}" ]; then
  echo "ERRO: POSTGRES_PASSWORD não definida." >&2
  echo "      Defina-a no ambiente ou copie .env.example para .env e ajuste." >&2
  exit 1
fi

if ! command -v psql >/dev/null 2>&1; then
  echo "ERRO: psql não encontrado no PATH. Instale o cliente PostgreSQL (ex.: postgresql-client / libpq)." >&2
  exit 1
fi

if [ ! -d "${MIGRATIONS_DIR}" ]; then
  echo "ERRO: diretório de migrations não encontrado: ${MIGRATIONS_DIR}" >&2
  exit 1
fi

# A senha nunca é impressa: fica apenas em variável de ambiente para o psql.
export PGPASSWORD="${POSTGRES_PASSWORD}"
export PGSSLMODE="${POSTGRES_SSLMODE}"

PSQL_BASE_ARGS=(-h "${POSTGRES_HOST}" -p "${POSTGRES_PORT}" -U "${POSTGRES_USER}" -d "${POSTGRES_DB}" -v ON_ERROR_STOP=1 --no-psqlrc)

echo "Conectando em ${POSTGRES_USER}@${POSTGRES_HOST}:${POSTGRES_PORT}/${POSTGRES_DB} (senha omitida)."

# Cria a tabela de controle se ainda não existir. Não destrutivo.
psql "${PSQL_BASE_ARGS[@]}" -c \
  "CREATE TABLE IF NOT EXISTS schema_migrations (
     version TEXT PRIMARY KEY,
     filename TEXT NOT NULL,
     applied_at TIMESTAMPTZ NOT NULL DEFAULT now()
   );" >/dev/null

shopt -s nullglob
sql_files=("${MIGRATIONS_DIR}"/*.sql)
shopt -u nullglob

if [ "${#sql_files[@]}" -eq 0 ]; then
  echo "Nenhum arquivo .sql encontrado em ${MIGRATIONS_DIR}. Nada a fazer."
  exit 0
fi

migration_manifest="$(mktemp)"
sorted_manifest="$(mktemp)"
wrapper_sql="$(mktemp)"
trap 'rm -f "${migration_manifest}" "${sorted_manifest}" "${wrapper_sql}"' EXIT

# Valida o prefixo numérico de cada migration e monta o manifesto
# "versao<TAB>arquivo<TAB>caminho_absoluto".
for f in "${sql_files[@]}"; do
  base="$(basename "${f}")"
  version="${base%%_*}"
  if ! [[ "${version}" =~ ^[0-9]+$ ]]; then
    echo "ERRO: migration '${base}' não segue o padrão NNN_nome.sql (prefixo numérico obrigatório)." >&2
    exit 1
  fi
  printf '%s\t%s\t%s\n' "${version}" "${base}" "${f}" >> "${migration_manifest}"
done

# Falha cedo se duas migrations usarem o mesmo prefixo: a ordem de aplicação
# ficaria indefinida e a versão é PRIMARY KEY em schema_migrations.
# A regra de resolução está em back-end/migrations/README.md.
duplicated_versions="$(cut -f1 "${migration_manifest}" | sort | uniq -d)"
if [ -n "${duplicated_versions}" ]; then
  echo "ERRO: prefixo de migration duplicado: ${duplicated_versions//$'\n'/, }" >&2
  echo "Renomeie uma delas para o próximo número livre da faixa do seu domínio (veja back-end/migrations/README.md)." >&2
  exit 1
fi

# Ordena numericamente pelo prefixo (evita quebra lexicográfica em 010+).
sort -t $'\t' -k1,1n "${migration_manifest}" > "${sorted_manifest}"

# Versões já aplicadas, para não reaplicar.
applied_versions="$(psql "${PSQL_BASE_ARGS[@]}" -tAc "SELECT version FROM schema_migrations ORDER BY version;")"

is_applied() {
  local v="$1"
  printf '%s\n' "${applied_versions}" | grep -qx "${v}"
}

applied_count=0
skipped_count=0

while IFS=$'\t' read -r version filename filepath; do
  if is_applied "${version}"; then
    echo "Pulando: ${filename} (já aplicada anteriormente)"
    skipped_count=$((skipped_count + 1))
    continue
  fi

  echo "Aplicando: ${filename}..."

  {
    echo "BEGIN;"
    echo "\\i ${filepath}"
    printf "INSERT INTO schema_migrations (version, filename) VALUES ('%s', '%s');\n" "${version}" "${filename}"
    echo "COMMIT;"
  } > "${wrapper_sql}"

  if ! psql "${PSQL_BASE_ARGS[@]}" -f "${wrapper_sql}"; then
    echo "ERRO: falha ao aplicar ${filename}. A transação não foi comitada, nada foi registrado." >&2
    exit 1
  fi

  echo "OK: ${filename} aplicada."
  applied_count=$((applied_count + 1))
done < "${sorted_manifest}"

echo "Concluído: ${applied_count} migration(ns) aplicada(s), ${skipped_count} pulada(s)."
