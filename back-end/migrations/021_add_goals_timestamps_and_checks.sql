-- Fixes a mismatch between PostgresGoalRepository::update() (which sets
-- updated_at=CURRENT_TIMESTAMP) and the 020 schema, which never created that
-- column. Applying this against a database that already has 020 is safe:
-- ADD COLUMN ... DEFAULT backfills existing rows, and the CHECK constraints
-- only reject values that never come from domain::to_string() in the first
-- place.

ALTER TABLE goals
    ADD COLUMN created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP;

ALTER TABLE goals
    ADD COLUMN updated_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP;

-- Enum-like columns are stored as the exact strings produced by
-- domain::to_string() (see src/domain/enums/*.cpp). Constrain them so bad
-- data cannot be inserted outside the C++ layer.
ALTER TABLE goals
    ADD CONSTRAINT goals_category_check
    CHECK (category IN (
        'College',
        'Work',
        'Health',
        'Leisure',
        'PersonalProjects',
        'Study'
    ));

ALTER TABLE goals
    ADD CONSTRAINT goals_status_check
    CHECK (status IN (
        'In Progress',
        'Completed',
        'Partially Completed',
        'Failed'
    ));

ALTER TABLE goals
    ADD CONSTRAINT goals_period_check
    CHECK (period IN (
        'Weekly',
        'Monthly',
        'Yearly'
    ));

CREATE INDEX IF NOT EXISTS idx_goals_status ON goals (status);
