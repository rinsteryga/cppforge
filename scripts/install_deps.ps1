param (
    [string]$InstallDir = "C:\cppforge"
)

$ErrorActionPreference = "Stop"

$PG_USER = "app"
$PG_PASSWORD = "secret_password_123"
$PG_DB = "app"
$PG_SERVICE_NAME = "cppforge-postgres"
$PG_PORT = 5432

if (-not (Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Path $InstallDir -ErrorAction SilentlyContinue | Out-Null
}
Start-Transcript -Path "$InstallDir\install_deps.log" -Append

$EnvPath = "$InstallDir\.env"
if (Test-Path $EnvPath) {
    Remove-Item $EnvPath -Force -ErrorAction SilentlyContinue
}

$SkipDbConfig = $false
$IsInstalled = $false
$PgBinDir = "C:\Program Files\PostgreSQL\16\bin"

# Поиск существующего psql.exe
$PsqlSearch = Get-ChildItem -Path "C:\Program Files\PostgreSQL\*\bin\psql.exe" -ErrorAction SilentlyContinue | Sort-Object FullName -Descending | Select-Object -First 1
if ($PsqlSearch) {
    $PgBinDir = $PsqlSearch.DirectoryName
    $IsInstalled = $true
} else {
    $portCheck = Get-NetTCPConnection -LocalPort $PG_PORT -ErrorAction SilentlyContinue
    if ($portCheck) {
        $PsqlCmd = Get-Command "psql" -ErrorAction SilentlyContinue
        if ($PsqlCmd) {
            $PgBinDir = Split-Path $PsqlCmd.Source
            $IsInstalled = $true
            Write-Host "Found custom PostgreSQL running on port $PG_PORT"
        }
    }
}

# Если не установлен, проверяем порт 5432, чтобы не было конфликтов
if (-not $IsInstalled) {
    $portCheck = Get-NetTCPConnection -LocalPort $PG_PORT -ErrorAction SilentlyContinue
    if ($portCheck) {
        $PG_PORT = 5433
        Write-Host "Port 5432 is taken, using port 5433 for new PostgreSQL installation."
    }
}

if ($IsInstalled) {
    Write-Host "Found existing PostgreSQL installation at $PgBinDir."
    
    $Service = Get-Service -Name $PG_SERVICE_NAME -ErrorAction SilentlyContinue
    if ($Service -and $Service.Status -ne 'Running') {
        Write-Host "PostgreSQL service ($PG_SERVICE_NAME) is stopped. Attempting to start..."
        Start-Service -Name $PG_SERVICE_NAME -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 5
    }

    $env:PGPASSWORD = $PG_PASSWORD
    
    $OldErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $DbReady = $false
    for ($i = 0; $i -lt 15; $i++) {
        & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -c "SELECT 1;" 2>&1 | Out-Null
        if ($LASTEXITCODE -eq 0) {
            $DbReady = $true
            break
        }
        Start-Sleep -Seconds 2
    }
    $ErrorActionPreference = $OldErrorAction
    
    if (-not $DbReady) {
        Write-Warning "Default password did not work, or server is unreachable. We will try to continue, but DB setup might fail."
        $SkipDbConfig = $true
    } else {
        Write-Host "Successfully connected to PostgreSQL."
        $SkipDbConfig = $false
    }
} else {
    
    Write-Host "Installing Visual C++ Redistributable (Required for PostgreSQL)..."
    $VcRedistPath = Join-Path $InstallDir "scripts\vc_redist.x64.exe"
    
    if (Test-Path $VcRedistPath) {
        Start-Process -FilePath $VcRedistPath -ArgumentList "/install /quiet /norestart" -Wait -NoNewWindow
    } else {
        Write-Warning "VC++ Redistributable not found in package at $VcRedistPath! Skipping..."
    }
    
    $InstallerPath = Join-Path $InstallDir "scripts\postgresql-16.4-1-windows-x64.exe"

    if (-not (Test-Path $InstallerPath)) {
        throw "Bundled PostgreSQL installer not found at $InstallerPath!"
    }

    Write-Host "Installing PostgreSQL (progress window will be hidden, please wait 1-3 minutes)..."
    $InstallArgs = @(
        "--mode", "unattended",
        "--unattendedmodeui", "none",
        "--servicename", $PG_SERVICE_NAME,
        "--superpassword", $PG_PASSWORD,
        "--serverport", $PG_PORT,
        "--disable-components", "pgAdmin,stackbuilder"
    )
    $process = Start-Process -FilePath $InstallerPath -ArgumentList $InstallArgs -Wait -NoNewWindow -PassThru

    if ($process.ExitCode -ne 0) {
        throw "PostgreSQL installation failed with Exit Code $($process.ExitCode)."
    }

    if (!(Test-Path $PgBinDir)) {
        throw "Cannot find expected PostgreSQL bin directory at $PgBinDir"
    }


    $env:PGPASSWORD = $PG_PASSWORD
    $env:PGCLIENTENCODING = "utf8"

    Write-Host "Waiting for service to become responsive (this might take up to 2 minutes on new systems)..."
    $OldErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $DbReady = $false
    $LastPsqlError = ""
    for ($i = 0; $i -lt 45; $i++) {
        $LastPsqlError = & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -c "SELECT 1;" 2>&1
        if ($LASTEXITCODE -eq 0) {
            $DbReady = $true
            break
        }
        Start-Sleep -Seconds 2
    }
    $ErrorActionPreference = $OldErrorAction

    if (-not $DbReady) {
        Write-Warning "PostgreSQL service took too long to start. Last error: $LastPsqlError"
        Write-Warning "We will skip database setup."
        $SkipDbConfig = $true
    }
}

if (-not $SkipDbConfig) {
    Write-Host "Configuring database..."
    $RoleExistsRaw = & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -tAc "SELECT 1 FROM pg_catalog.pg_roles WHERE rolname='$PG_USER'" 2>&1
    $RoleExists = ($RoleExistsRaw -join "").Trim()
    if (-not $RoleExists.Contains("1")) {
        Write-Host "Creating role $PG_USER..."
        "CREATE ROLE $PG_USER LOGIN PASSWORD '$PG_PASSWORD';" | & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT 2>&1 | Out-Null
    } else {
        Write-Host "Role $PG_USER already exists."
    }
    
    $DbExistsRaw = & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT -tAc "SELECT 1 FROM pg_database WHERE datname='$PG_DB'" 2>&1
    $DbExists = ($DbExistsRaw -join "").Trim()
    if (-not $DbExists.Contains("1")) {
        Write-Host "Creating database $PG_DB..."
        "CREATE DATABASE $PG_DB OWNER $PG_USER;" | & "$PgBinDir\psql.exe" -U postgres -d postgres -p $PG_PORT 2>&1 | Out-Null
    } else {
        Write-Host "Database $PG_DB already exists."
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

    $ModulesDir = "$InstallDir\data\migrations\modules"
    if (Test-Path $ModulesDir) {
        $ModuleFiles = Get-ChildItem -Path $ModulesDir -Filter "*.sql" | Sort-Object Name
        foreach ($ModFile in $ModuleFiles) {
            & "$PgBinDir\psql.exe" -U $PG_USER -d $PG_DB -p $PG_PORT -f $ModFile.FullName
        }
    }
} else {
    Write-Warning "Skipped database setup because PostgreSQL connection failed."
}

Write-Host "Creating .env file..."
$EnvConfig = @"
PG_HOST=127.0.0.1
PG_PORT=$PG_PORT
PG_DB=$PG_DB
PG_USER=$PG_USER
PG_PASSWORD=$PG_PASSWORD
"@

$EnvPath = "$InstallDir\.env"
Set-Content -Path $EnvPath -Value $EnvConfig

Write-Host "PostgreSQL setup complete!"

$ToolchainBin = Join-Path $InstallDir "bin"

if (Test-Path $ToolchainBin) {
    Write-Host "Configuring system PATH for GCC toolchain..."
    try {
        $MachinePath = [Environment]::GetEnvironmentVariable("Path", "Machine")
        
        $Paths = $MachinePath -split ";" | Where-Object { $_ -match '\S' }
        
        $PathExists = $false
        foreach ($p in $Paths) {
            if ($p.TrimEnd('\') -ieq $ToolchainBin.TrimEnd('\')) {
                $PathExists = $true
                break
            }
        }

        if (-not $PathExists) {
            Write-Host "Adding $ToolchainBin to Machine PATH..."
            $NewPath = ($Paths + $ToolchainBin) -join ";"
            
            [Environment]::SetEnvironmentVariable("Path", $NewPath, "Machine")
            
            $env:Path += ";$ToolchainBin"
            
            Write-Host "Toolchain path successfully added."
        } else {
            Write-Host "Toolchain path already exists in Machine PATH. Skipping."
        }
    } catch {
        Write-Warning "Failed to add GCC to PATH. Ensure the installer is running with Administrator privileges."
        Write-Warning $_.Exception.Message
    }
} else {
    Write-Warning "Toolchain bin directory not found at $ToolchainBin. Skipping PATH configuration."
}

Stop-Transcript