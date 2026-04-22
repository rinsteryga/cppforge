 #!/bin/bash

INSTALL_DIR=${1:-"/opt/cppforge"}
PG_USER="app"
PG_PASSWORD="secret_password_123"
PG_DB="app"
PG_PORT=5432

if [ "$EUID" -ne 0 ]; then
  echo "Please run as root (sudo)"
  exit 1
fi

mkdir -p "$INSTALL_DIR"
exec > >(tee -a "$INSTALL_DIR/install_deps.log") 2>&1

echo "Checking for PostgreSQL..."
if ! command -v psql &> /dev/null; then
    echo "PostgreSQL not found. Attempting installation..."
    if command -v apt-get &> /dev/null; then
        apt-get update
        apt-get install -y postgresql postgresql-contrib
    elif command -v brew &> /dev/null; then
        sudo -u $(logname) brew install postgresql
        sudo -u $(logname) brew services start postgresql
    else
        echo "Could not find a supported package manager (apt or brew). Please install PostgreSQL manually."
        exit 1
    fi
fi

if command -v systemctl &> /dev/null; then
    systemctl start postgresql
    systemctl enable postgresql
fi

echo "Configuring database..."
sudo -u postgres psql -c "SELECT 1 FROM pg_roles WHERE rolname='$PG_USER'" | grep -q 1 || \
sudo -u postgres psql -c "CREATE ROLE $PG_USER LOGIN PASSWORD '$PG_PASSWORD';"

sudo -u postgres psql -c "SELECT 1 FROM pg_database WHERE datname='$PG_DB'" | grep -q 1 || \
sudo -u postgres psql -c "CREATE DATABASE $PG_DB OWNER $PG_USER;"

echo "Applying migrations..."
MIGRATION_FILE="$INSTALL_DIR/data/migrations/schema.sql"
if [ -f "$MIGRATION_FILE" ]; then
    PGPASSWORD=$PG_PASSWORD psql -h localhost -U $PG_USER -d $PG_DB -p $PG_PORT -f "$MIGRATION_FILE"
fi

SEED_FILE="$INSTALL_DIR/data/migrations/seed.sql"
if [ -f "$SEED_FILE" ]; then
    PGPASSWORD=$PG_PASSWORD psql -h localhost -U $PG_USER -d $PG_DB -p $PG_PORT -f "$SEED_FILE"
fi

echo "Creating .env file..."
cat <<EOF > "$INSTALL_DIR/.env"
PG_HOST=127.0.0.1
PG_PORT=$PG_PORT
PG_DB=$PG_DB
PG_USER=$PG_USER
PG_PASSWORD=$PG_PASSWORD
EOF

chmod 600 "$INSTALL_DIR/.env"
echo "PostgreSQL setup complete!"
