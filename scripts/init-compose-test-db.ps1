$ErrorActionPreference = 'Stop'

$testDb = $env:TEST_PG_DB
if ([string]::IsNullOrWhiteSpace($testDb)) {
    $testDb = 'cppforge_test'
}
if ($testDb -notmatch '^[A-Za-z0-9_]+$') {
    throw 'TEST_PG_DB must be a simple PostgreSQL identifier (letters, digits, underscores).'
}

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

docker compose exec -T db sh -ec 'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -tAc "select 1"' *> $null
if ($LASTEXITCODE -ne 0) {
    throw @'
The PostgreSQL volume exists, but the current PG_USER/PG_DB cannot use it.
The volume keeps the POSTGRES_USER and POSTGRES_DB used at first initialization.
Do not change PG_USER/PG_DB for an existing volume. Either:
  1. use the original credentials, or
  2. recreate the volume with `docker compose down -v` (this deletes all data in it), or
  3. use a separate Compose project/volume for cppforge_test.
No database was modified by this helper.
'@
}

# Create the test database when it does not exist. The identifier is validated above.
$createDatabaseSql = @'
SELECT format('CREATE DATABASE %I', :'dbname')
WHERE NOT EXISTS (SELECT 1 FROM pg_database WHERE datname = :'dbname') \gexec
'@
$createDatabaseSql | docker compose exec -T db psql -U "$POSTGRES_USER" -d postgres -v ON_ERROR_STOP=1 -v dbname="$testDb"
if ($LASTEXITCODE -ne 0) {
    throw "Failed to create $testDb."
}

# schema.sql drops tables; apply it only to the dedicated test database.
docker compose exec -T db psql -U "$POSTGRES_USER" -d $testDb -v ON_ERROR_STOP=1 -f /migrations/schema.sql
if ($LASTEXITCODE -ne 0) {
    throw "Failed to apply the schema to $testDb."
}
docker compose exec -T db psql -U "$POSTGRES_USER" -d $testDb -v ON_ERROR_STOP=1 -f /migrations/seed.sql
if ($LASTEXITCODE -ne 0) {
    throw "Failed to seed $testDb."
}

Write-Host "$testDb is ready with the repository schema and seed data."
