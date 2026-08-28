ALTER TABLE goals
    ADD COLUMN reference_date DATE;

UPDATE goals
-- created_at preserva a data histórica de criação dos registros existentes.
SET reference_date = created_at::date
WHERE reference_date IS NULL;

ALTER TABLE goals
    ALTER COLUMN reference_date SET NOT NULL;
