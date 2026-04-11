param (
    [string]$InstallDir = "C:\cppforge"
)

$ErrorActionPreference = "Stop"

$PG_USER = "app"
$PG_PASSWORD = "secret_password_123"
$PG_DB = "app"
$PG_SERVICE_NAME = "PostgreSQL"
$PG_PORT = 5432

$PgBinDir = "C:\Program Files\PostgreSQL\16\bin"
$IsInstalled = Test-Path "$PgBinDir\psql.exe"

if ($IsInstalled) {
    Write-Host "Found existing PostgreSQL installation at $PgBinDir."
    $env:PGPASSWORD = $PG_PASSWORD
    & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -c "SELECT 1;" 2>&1 | Out-Null
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Default password did not work for 'postgres' user."
        $PG_PASSWORD = Read-Host "Please enter the actual password for the 'postgres' user"
        $env:PGPASSWORD = $PG_PASSWORD
        
        & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -c "SELECT 1;" 2>&1 | Out-Null
        if ($LASTEXITCODE -ne 0) {
            throw "Failed to connect to PostgreSQL with the provided password!"
        }
    }
    Write-Host "Successfully connected to PostgreSQL."
} else {
    $PostgresInstallerUrl = "https://get.enterprisedb.com/postgresql/postgresql-16.4-1-windows-x64.exe"
    $InstallerPath = "$env:TEMP\postgresql-installer.exe"

    Write-Host "Downloading PostgreSQL installer (can take a few minutes)..."
    if (!(Test-Path $InstallerPath) -or ((Get-Item $InstallerPath).Length -lt 50MB)) {
        Invoke-WebRequest -Uri $PostgresInstallerUrl -OutFile $InstallerPath -UseBasicParsing
        
        if ((Get-Item $InstallerPath).Length -lt 50MB) {
            throw "Download failed! File is too small. URL might be broken."
        }
    } else {
        Write-Host "Using cached PostgreSQL installer."
    }

    Write-Host "Installing PostgreSQL (progress window will appear)..."
    $InstallArgs = @(
        "--mode", "unattended",
        "--unattendedmodeui", "minimal",
        "--servicename", $PG_SERVICE_NAME,
        "--superpassword", $PG_PASSWORD,
        "--serverport", $PG_PORT
    )
    $process = Start-Process -FilePath $InstallerPath -ArgumentList $InstallArgs -Wait -NoNewWindow -PassThru

    if ($process.ExitCode -ne 0) {
        throw "PostgreSQL installation failed with Exit Code $($process.ExitCode). You may need to install Microsoft Visual C++ Redistributable."
    }

    if (!(Test-Path $PgBinDir)) {
        throw "Cannot find expected PostgreSQL bin directory at $PgBinDir"
    }

    $env:PGPASSWORD = $PG_PASSWORD

    Write-Host "Waiting for service to become responsive..."
    Start-Sleep -Seconds 10
}

Write-Host "Configuring database..."
$CreateDbSql = @"
CREATE USER $PG_USER WITH PASSWORD '$PG_PASSWORD';
CREATE DATABASE $PG_DB OWNER $PG_USER;
"@
$CreateDbSql | & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT

Write-Host "Applying database migrations..."
$MigrationFile = "$InstallDir\data\migrations\schema.sql"
if (Test-Path $MigrationFile) {
    & "$PgBinDir\psql.exe" -U $PG_USER -d $PG_DB -p $PG_PORT -f $MigrationFile
} else {
    Write-Host "WARNING: Migration file not found at $MigrationFile"
}

Write-Host "Applying database seed..."
$SeedFile = "$InstallDir\data\migrations\seed.sql"
if (Test-Path $SeedFile) {
    & "$PgBinDir\psql.exe" -U $PG_USER -d $PG_DB -p $PG_PORT -f $SeedFile
} else {
    Write-Host "WARNING: Seed file not found at $SeedFile"
}

Write-Host "Creating .env file..."
$EnvConfig = @"
PG_HOST=localhost
PG_PORT=$PG_PORT
PG_DB=$PG_DB
PG_USER=$PG_USER
PG_PASSWORD=$PG_PASSWORD
"@

$EnvPath = "$InstallDir\.env"
Set-Content -Path $EnvPath -Value $EnvConfig
Write-Host "PostgreSQL setup complete!"
