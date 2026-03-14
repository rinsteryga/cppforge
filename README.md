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
   sudo apt install -y build-essential cmake qtbase5-dev qt5-qmake qtdeclarative5-dev qttools5-dev
   ```

3. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

4. **Run the application:**
   ```bash
   ./cppforge
   ```