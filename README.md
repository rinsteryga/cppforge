# cppforge

We are working on this project now. Dev branch will be merged to main when it`s ready.
Devs: @rinsteryga, @Sernock

## Build Instructions (Ubuntu / Debian)

1. **Clone the repository:**

   ```bash
   git clone https://github.com/rinsteryga/cppforge.git
   cd cppforge
   ```

2. **Install dependencies (or use vcpkg):**

   The system dependencies of this build are PostgreSQL and a C++ toolchain.
   Qt and all other third-party libraries are provided by vcpkg:

   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake ninja-build git pkg-config curl zip unzip tar \
     postgresql postgresql-contrib \
     autoconf automake autoconf-archive bison flex gperf libtool python3 \
     libgl1-mesa-dev libglu1-mesa-dev \
     libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxrandr-dev \
     libxcb1-dev libxcb-cursor-dev libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev \
     libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-shape0-dev \
     libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev \
     libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev \
     libfontconfig1-dev libfreetype6-dev libharfbuzz-dev libssl-dev
   ```

   Set up vcpkg (see <https://learn.microsoft.com/vcpkg/get_started/overview>) and make
   sure the `VCPKG_ROOT` environment variable points to its checkout.

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
   cmake --preset linux-x64-vcpkg
   cmake --build --preset linux-x64-vcpkg
   ctest --test-dir build/linux-x64-vcpkg --output-on-failure
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
2. Install it using `apt` so that PostgreSQL is automatically resolved and installed:
   ```bash
   sudo apt install ./cppforge-1.0.3-Linux.deb
   ```
3. The installation script will configure the PostgreSQL database and environment automatically.

### Linux (.tar.gz archive)
1. Download the `cppforge-*-Linux.tar.gz` archive.
2. Extract the archive:
   ```bash
   tar -xzf cppforge-1.0.3-Linux.tar.gz
   cd cppforge-1.0.3-Linux
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

## Backend API development

The Crow server exposes the first versioned API slice under `/api/v1`. To start the local
PostgreSQL instance, copy `.env.example` to `.env` if needed, set the `PG_*` values, then run:

```bash
docker compose up -d db
docker compose ps
```

The Compose service publishes PostgreSQL only on `127.0.0.1`, includes a `pg_isready`
healthcheck, and initializes the separate `cppforge_test` database on a fresh volume. The
destructive `schema.sql` is **not** applied automatically. To initialize the desktop/server
development database manually, use the mounted migration directory:

```bash
docker compose exec db sh -c 'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -f /migrations/schema.sql'
docker compose exec db sh -c 'psql -U "$POSTGRES_USER" -d "$POSTGRES_DB" -f /migrations/seed.sql'
```

Run the server with `SERVER_HOST`, `SERVER_PORT`, and `LOG_LEVEL` from `.env`. The integration
test does not load `.env`; opt in by setting `TEST_PG_HOST`, `TEST_PG_PORT`, `TEST_PG_DB`,
`TEST_PG_USER`, and `TEST_PG_PASSWORD` in the process environment. If `cppforge_test` is
unavailable or those settings are absent, the HTTP integration test skips. Unit tests do not
require PostgreSQL, Docker, or network access.

The API contract and examples are in `contracts/openapi.yaml` and `contracts/fixtures/`.
Planned backend stages are tracked in `docs/backend-roadmap.md`. React and QML clients are not
part of this slice.
