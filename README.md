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
   sudo -u postgres psql -d cppforge -f src/data/migrations/schema.sql
   sudo -u postgres psql -d cppforge -f src/data/migrations/seed.sql
   ```

   **Database Reset / Update:**
   If you need to reset the database (drop, recreate, and seed) during development, use the provided cross-platform scripts in `src/data/migrations/`.
   - On Windows: Run `reset_db.bat`.
   - On Linux/macOS: Run `bash reset_db.sh`.

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

## Installation (Release Packages)

If you prefer not to build from source, you can install the pre-compiled packages.

### Windows (.exe installer)
1. Download the `cppforge-*-win64.exe` installer from the releases page.
2. Run the installer. It will automatically download, install, and configure PostgreSQL if it's not already set up.
3. Launch `cppforge` from the Start menu.

### Linux (.deb package for Ubuntu/Debian)
1. Download the `cppforge-*-Linux.deb` package.
2. Install it using `apt` so that dependencies (like `libqt5sql5-psql` and PostgreSQL) are automatically resolved and installed:
   ```bash
   sudo apt install ./cppforge-1.0.2-Linux.deb
   ```
3. The installation script will configure the PostgreSQL database and environment automatically.

### Linux (.tar.gz archive)
1. Download the `cppforge-*-Linux.tar.gz` archive.
2. Extract the archive:
   ```bash
   tar -xzf cppforge-1.0.2-Linux.tar.gz
   cd cppforge-1.0.2-Linux
   ```
3. Run the installation script with root privileges to install dependencies and set up the database:
   ```bash
   sudo sh ./share/cppforge/scripts/install_deps.sh
   ```
4. Run the application:
   ```bash
   ./bin/cppforge
   ```

### macOS (.dmg)
1. Download the `cppforge-*-Darwin.dmg` image.
2. Open the `.dmg` file and drag `cppforge` to your Applications folder.
3. *(Note: You may need to manually configure the PostgreSQL database or .env file for macOS at this time)*
