SELECT 'CREATE DATABASE cppforge_test'
WHERE NOT EXISTS (SELECT 1 FROM pg_database WHERE datname = 'cppforge_test')\gexec
