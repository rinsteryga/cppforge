SELECT 'CREATE DATABASE cppforge_test'
WHERE NOT EXISTS (SELECT 1 FROM pg_database WHERE datname = 'cppforge_test')\gexec

\connect cppforge_test

CREATE TABLE IF NOT EXISTS modules
(
    id BIGINT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT
);

INSERT INTO modules (id, name, description)
VALUES (1, 'Intro', 'Start here')
ON CONFLICT (id) DO UPDATE
SET name = EXCLUDED.name, description = EXCLUDED.description;
