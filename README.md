# cppforge

We are working on this project now. Dev branch will be merged to main when it`s ready.
Devs: @rinsteryga, @Sernock

## Build Instructions (Ubuntu / Debian)

1. **Clone the repository:**

   ```bash
   git clone https://github.com/rinsteryga/cppforge.git
   cd cppforge
   ```

2. **Install dependencies:**

   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake qtbase5-dev qt5-qmake qtdeclarative5-dev qttools5-dev postgresql postgresql-contrib
   ```

3. **Database Setup (PostgreSQL):**

   ```bash
   sudo -u postgres psql -c "CREATE DATABASE cppforge;"
   ```

   Since the `.env` file is in `.gitignore`, you need to create it manually from the example:

   ```bash
   cp .env.example .env
   nano .env # Or use any other text editor
   ```

   *Make sure to fill in your actual PostgreSQL credentials (`PG_USER` and `PG_PASSWORD`) inside the `.env` file before proceeding.*

   ```bash
   psql -U postgres -d cppforge -f src/data/migrations/schema.sql
   psql -U postgres -d cppforge -f src/data/migrations/seed.sql
   ```

4. **Build the project:**

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

5. **Run the application:**

   ```bash
   ./cppforge
   ```
