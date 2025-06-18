# Library Management System (LMS)

A modular C++ Library Management System with SQLite database integration.

For features and future improvements see docs

## Project Structure
- `src/` - Source files
- `include/lms/` - Header files
- `lib/sqlite3/` - SQLite amalgamation
- `utils/` - Utility headers (e.g., picosha2)
- `scripts/` - Build scripts

### Prerequisites
- CMake (3.10+)
- C++17 compiler (GCC, Clang, or MSVC)
- (Windows) Use Git Bash, MSYS2, or WSL for `.sh` scripts, or use CMake from the command line

### Build Steps
1. Create a build directory:
   ```sh
   mkdir build
   cd build
   ```
2. Run CMake and build:
   ```sh
   cmake ..
   cmake --build . --config Release
   ```
   Or use the provided script:
   ```sh
   ../scripts/build.sh
   ```

## Usage
- Run the executable from the `build` directory:
  ```sh
  ./lms   # or lms.exe on Windows
  ```
- Follow the menu to add/list users and books.

## Notes
- The database file (`test.db` or `library.db`) will be created in the build directory.
- Currently it creates with name `test.db`.
- Tables are auto-created if they do not exist.
- For Windows, use CMake's native build tools or a Unix-like shell for scripts.
