-- Migration base (faixa 001-019, responsável: Arquitetura).
--
-- ADR-002 (docs/architecture.md, P-22A): o sistema é single-tenant. Existe
-- exatamente um usuário, sem cadastro, login, senha ou sessão. Ainda assim as
-- tabelas de domínio nascem com
--     user_id BIGINT NOT NULL REFERENCES users(id)
-- apontando para a linha semeada aqui.
--
-- Escopo desta migration é só a identidade do usuário. Colunas de perfil (nome,
-- e-mail) ficam na faixa 050-059 (P-26.4), conforme back-end/migrations/README.md.
-- Sem coluna de senha e sem coluna de credencial: não há autenticação no projeto.
--
-- Reaplicar este arquivo é seguro: CREATE TABLE IF NOT EXISTS e ON CONFLICT DO
-- NOTHING não duplicam a linha nem sobrescrevem dados.

CREATE TABLE IF NOT EXISTS users
(
    id BIGSERIAL PRIMARY KEY
);

INSERT INTO users (id)
VALUES (1)
ON CONFLICT (id) DO NOTHING;

-- O INSERT acima grava um id explícito e não avança a sequence do BIGSERIAL.
-- Sem este setval, o primeiro INSERT sem id (P-26.4) pegaria nextval = 1 e
-- violaria a primary key. Alinhar a sequence com o maior id existente resolve
-- isso e é idempotente em reaplicação.
SELECT setval(pg_get_serial_sequence('users', 'id'), (SELECT MAX(id) FROM users));
