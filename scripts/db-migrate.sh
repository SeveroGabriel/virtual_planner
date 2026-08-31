#!/usr/bin/env bash
# Mantém o comando local/CI; o migrador pertence ao contexto do backend.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec bash "${SCRIPT_DIR}/../back-end/migrations/db-migrate.sh" "$@"
