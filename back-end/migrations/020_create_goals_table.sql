CREATE TABLE goals
(
    id BIGSERIAL PRIMARY KEY,

    description TEXT NOT NULL,

    category VARCHAR(50) NOT NULL,

    status VARCHAR(50) NOT NULL,

    period VARCHAR(50) NOT NULL

);