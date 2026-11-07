#!/usr/bin/env sh
set -eu

docker compose up -d db

ready=0
attempt=0
while [ "$attempt" -lt 30 ]; do
    if docker compose exec -T db sh -ec 'pg_isready -U "$POSTGRES_USER" -d "$POSTGRES_DB"' >/dev/null 2>&1; then
        ready=1
        break
    fi
    attempt=$((attempt + 1))
    sleep 2
done
if [ "$ready" -ne 1 ]; then
    echo 'Compose PostgreSQL did not become ready.' >&2
    exit 1
fi

# Safe to re-run: this file only creates cppforge_test when it does not exist.
docker compose exec -T db sh -ec \
    'psql -U "$POSTGRES_USER" -d postgres -v ON_ERROR_STOP=1 -f /docker-entrypoint-initdb.d/10_test_db.sql'

# schema.sql drops tables; apply it only to this dedicated test database.
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d cppforge_test -v ON_ERROR_STOP=1 -f /migrations/schema.sql'
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d cppforge_test -v ON_ERROR_STOP=1 -f /migrations/seed.sql'

echo 'cppforge_test is ready with the repository schema and seed data.'
