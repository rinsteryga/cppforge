 #!/bin/bash

INSTALL_DIR=${1:-"/opt/cppforge"}
PG_USER="app"
PG_PASSWORD="secret_password_123"
PG_DB="app"
PG_PORT=5432

if [ "$(id -u)" -ne 0 ]; then
  echo "Please run as root (sudo)"
  exit 1
fi

mkdir -p "$INSTALL_DIR"
LOG_FILE="$INSTALL_DIR/install_deps.log"
echo "Starting installation..." > "$LOG_FILE"

echo "Checking for PostgreSQL..."
PSQL_CMD=$(command -v psql)
if [ -z "$PSQL_CMD" ]; then
    PSQL_CMD=$(find /usr/bin /usr/lib/postgresql /opt /usr/local -name psql -type f -executable 2>/dev/null | head -n 1)
fi

if [ -z "$PSQL_CMD" ] && ! id -u postgres > /dev/null 2>&1; then
    echo "ERROR: PostgreSQL not found!"
    exit 0
fi

if command -v systemctl > /dev/null 2>&1; then
    systemctl start postgresql
    systemctl enable postgresql
fi

echo "Waiting for PostgreSQL to start..."
for i in {1..30}; do
    if su - postgres -c "psql -c 'SELECT 1;'" > /dev/null 2>&1; then
        break
    fi
    sleep 1
done

echo "Configuring database..."
if [ "$(su - postgres -c "psql -tAc \"SELECT 1 FROM pg_roles WHERE rolname='$PG_USER'\"")" != "1" ]; then
    su - postgres -c "psql -c \"CREATE ROLE $PG_USER LOGIN PASSWORD '$PG_PASSWORD';\""
fi

if [ "$(su - postgres -c "psql -tAc \"SELECT 1 FROM pg_database WHERE datname='$PG_DB'\"")" != "1" ]; then
    su - postgres -c "psql -c \"CREATE DATABASE $PG_DB OWNER $PG_USER;\""
fi

echo "Applying migrations..."
MIGRATION_FILE="$INSTALL_DIR/share/cppforge/data/migrations/schema.sql"
if [ -f "$MIGRATION_FILE" ]; then
    PGPASSWORD=$PG_PASSWORD psql -h localhost -U $PG_USER -d $PG_DB -p $PG_PORT -f "$MIGRATION_FILE"
fi

SEED_FILE="$INSTALL_DIR/share/cppforge/data/migrations/seed.sql"
if [ -f "$SEED_FILE" ]; then
    PGPASSWORD=$PG_PASSWORD psql -h localhost -U $PG_USER -d $PG_DB -p $PG_PORT -f "$SEED_FILE"
fi

MODULES_DIR="$INSTALL_DIR/share/cppforge/data/migrations/modules"
if [ -d "$MODULES_DIR" ]; then
    for f in "$MODULES_DIR"/*.sql; do
        if [ -f "$f" ]; then
            PGPASSWORD=$PG_PASSWORD psql -h localhost -U $PG_USER -d $PG_DB -p $PG_PORT -f "$f"
        fi
    done
fi

echo "Creating .env file..."
mkdir -p /etc/cppforge
cat <<EOF > "/etc/cppforge/.env"
PG_HOST=127.0.0.1
PG_PORT=$PG_PORT
PG_DB=$PG_DB
PG_USER=$PG_USER
PG_PASSWORD=$PG_PASSWORD
EOF

chmod 644 "/etc/cppforge/.env"
echo "PostgreSQL setup complete!"
