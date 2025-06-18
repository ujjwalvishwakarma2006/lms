# LMS Project Documentation

## Features (As of Latest commit)
- Add, list, and manage users and books
- Persistent storage using SQLite
- In-memory and database-synced operations
- Modern CMake build system

## Future Improvements
- User authentication and roles
- Book reservation and due date management
- Advanced search and filtering
- GUI frontend
- REST API or web interface
- Automated tests

## Bugs
- Do not enter empty fields while adding users and books

## Build & Usage
See the main `README.md` in the project root for build and usage instructions.

## Project Structure (detailed)
- `src/` - All C++ source files
- `include/lms/` - All public headers
- `lib/sqlite3/` - SQLite amalgamation source
- `utils/` - Utility headers (e.g., picosha2)
- `scripts/` - Build scripts (Bash, batch)
- `docs/` - Documentation (this file, design notes, etc.)
