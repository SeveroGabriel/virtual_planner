-- Faixa 020-029 (`Goal`).
--
-- A migration base 001 declara a regra de isolamento do projeto:
--
--     as tabelas de domínio nascem com
--     user_id BIGINT NOT NULL REFERENCES users(id)
--
-- A tabela `reminders` cumpre a regra (migration 040). A `goals` nasceu sem a
-- coluna, então o isolamento estava aplicado pela metade — o que é pior que não
-- estar aplicado, porque dá a impressão de que existe.
--
-- O DEFAULT 1 existe só para o backfill das linhas já gravadas: aponta para o
-- usuário único semeado pela 001, coerente com a ADR-002. Ele é removido logo
-- em seguida, para que nenhuma inserção futura herde um dono por acidente —
-- quem insere passa a ser obrigado a dizer de quem é a meta.
--
-- Reaplicar é seguro: a coluna só é adicionada se ainda não existir, e o índice
-- usa IF NOT EXISTS.

ALTER TABLE goals
    ADD COLUMN IF NOT EXISTS user_id BIGINT NOT NULL DEFAULT 1
    REFERENCES users(id);

ALTER TABLE goals
    ALTER COLUMN user_id DROP DEFAULT;

-- Toda query de leitura passa a filtrar por user_id: sem índice, a listagem por
-- período faria varredura sequencial na tabela inteira.
CREATE INDEX IF NOT EXISTS goals_user_id_reference_date_idx
    ON goals (user_id, reference_date);
