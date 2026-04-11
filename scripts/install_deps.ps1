param (
    [string]$InstallDir = "C:\cppforge"
)

$ErrorActionPreference = "Stop"

$PG_USER = "app"
$PG_PASSWORD = "secret_password_123"
$PG_DB = "app"
$PG_SERVICE_NAME = "cppforge-postgres"
$PG_PORT = 5432

$SkipDbConfig = $false
$IsInstalled = $false
$PgBinDir = "C:\Program Files\PostgreSQL\16\bin"

# Check standard program files paths
$PsqlSearch = Get-ChildItem -Path "C:\Program Files\PostgreSQL\*\bin\psql.exe" -ErrorAction SilentlyContinue | Sort-Object FullName -Descending | Select-Object -First 1
if ($PsqlSearch) {
    $PgBinDir = $PsqlSearch.DirectoryName
    $IsInstalled = $true
}

# Check if available in PATH
if (-not $IsInstalled) {
    $PsqlCmd = Get-Command "psql" -ErrorAction SilentlyContinue
    if ($PsqlCmd) {
        $PgBinDir = Split-Path $PsqlCmd.Source
        $IsInstalled = $true
    }
}

if (-not $IsInstalled) {
    # Test if port is already taken by some other process
    $portCheck = Get-NetTCPConnection -LocalPort $PG_PORT -ErrorAction SilentlyContinue
    if ($portCheck) {
        $PG_PORT = 5433 # fallback
        Write-Host "Port 5432 is taken, using port 5433 for new PostgreSQL installation."
    }
}

if ($IsInstalled) {
    Write-Host "Found existing PostgreSQL installation at $PgBinDir."
    $env:PGPASSWORD = $PG_PASSWORD
    & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -c "SELECT 1;" 2>&1 | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Default password did not work for 'postgres' user. We will try to continue, but DB setup might fail."
        $SkipDbConfig = $true
    } else {
        Write-Host "Successfully connected to PostgreSQL."
        $SkipDbConfig = $false
    }
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

if (-not $SkipDbConfig) {
    Write-Host "Configuring database..."
    $CreateDbSql = @"
    DO `$do` BEGIN
      IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = '$PG_USER') THEN
        CREATE ROLE $PG_USER LOGIN PASSWORD '$PG_PASSWORD';
      END IF;
    END `$do`;
"@
    $CreateDbSql | & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT
    
    $DbExists = & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -tAc "SELECT 1 FROM pg_database WHERE datname='$PG_DB'"
    if ($DbExists -ne "1") {
        "CREATE DATABASE $PG_DB OWNER $PG_USER;" | & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT
    }
    
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
} else {
    Write-Warning "Skipped database and table creation because PostgreSQL connection failed."
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
