#!/usr/bin/env sh
set -eu

test_db="${TEST_PG_DB:-cppforge_test}"
case "$test_db" in
    "" | *[!A-Za-z0-9_]*)
        echo "TEST_PG_DB must be a simple PostgreSQL identifier (letters, digits, underscores)." >&2
        exit 1
        ;;
esac

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

if ! docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -tAc "select 1"' >/dev/null 2>&1; then
    cat >&2 <<'EOF'
The PostgreSQL volume exists, but the current PG_USER/PG_DB cannot use it.
The volume keeps the POSTGRES_USER and POSTGRES_DB used at first initialization.
Do not change PG_USER/PG_DB for an existing volume. Either:
  1. use the original credentials, or
  2. recreate the volume with `docker compose down -v` (this deletes all data in it), or
  3. use a separate Compose project/volume for cppforge_test.
No database was modified by this helper.
EOF
    exit 1
fi

# Create the test database when it does not exist. The identifier is validated above.
docker compose exec -T db sh -ec \
    'psql -U "$POSTGRES_USER" -d postgres -v ON_ERROR_STOP=1 -v dbname="$1"' sh "$test_db" <<'SQL'
SELECT format('CREATE DATABASE %I', :'dbname')
WHERE NOT EXISTS (SELECT 1 FROM pg_database WHERE datname = :'dbname') \gexec
SQL

# schema.sql drops tables; apply it only to the dedicated test database.
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d "$1" -v ON_ERROR_STOP=1 -f /migrations/schema.sql' sh "$test_db"
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d "$1" -v ON_ERROR_STOP=1 -f /migrations/seed.sql' sh "$test_db"

echo "$test_db is ready with the repository schema and seed data."
