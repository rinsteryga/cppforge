#!/bin/bash
cd "$(dirname "$0")" || exit

export PGPASSWORD=cppforge

echo "Dropping database cppforge..."
psql -U postgres -c "DROP DATABASE IF EXISTS cppforge;"

echo "Creating database cppforge..."
psql -U postgres -c "CREATE DATABASE cppforge;"

echo "Running schema migration..."
psql -U postgres -d cppforge -f schema.sql

echo "Running seed migration..."
psql -U postgres -d cppforge -f seed.sql

echo "Database reset complete!"
