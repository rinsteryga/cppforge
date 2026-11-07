$ErrorActionPreference = 'Stop'

docker compose up -d db
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to start the Compose PostgreSQL service.'
}

$ready = $false
for ($attempt = 0; $attempt -lt 30; $attempt++) {
    docker compose exec -T db sh -ec 'pg_isready -U "$POSTGRES_USER" -d "$POSTGRES_DB"' *> $null
    if ($LASTEXITCODE -eq 0) {
        $ready = $true
        break
    }
    Start-Sleep -Seconds 2
}
if (-not $ready) {
    throw 'Compose PostgreSQL did not become ready.'
}

# Safe to re-run: this file only creates cppforge_test when it does not exist.
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d postgres -v ON_ERROR_STOP=1 -f /docker-entrypoint-initdb.d/10_test_db.sql'
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to create cppforge_test.'
}

# schema.sql drops tables; apply it only to this dedicated test database.
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d cppforge_test -v ON_ERROR_STOP=1 -f /migrations/schema.sql'
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to apply the schema to cppforge_test.'
}
docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d cppforge_test -v ON_ERROR_STOP=1 -f /migrations/seed.sql'
if ($LASTEXITCODE -ne 0) {
    throw 'Failed to seed cppforge_test.'
}

Write-Host 'cppforge_test is ready with the repository schema and seed data.'
