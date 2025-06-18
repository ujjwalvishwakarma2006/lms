#include "../include/lms/Database.h"
#include <iostream>
#include <sstream>

namespace lms {
    Database::Database(const std::string& dbPath) : dbPath(dbPath) {}

    Database::~Database() {
        disconnect();
    }

    bool Database::connect() {
        if (connected) return true;
        int rc = sqlite3_open(dbPath.c_str(), &db);
        connected = (rc == SQLITE_OK);
        if (!connected) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }
        // Create users table if it doesn't exist
        const char* userTableSQL =
            "CREATE TABLE IF NOT EXISTS users ("
            "id TEXT PRIMARY KEY, "
            "name TEXT, "
            "email TEXT, "
            "dob TEXT, "
            "address TEXT, "
            "borrowed_books TEXT, "
            "is_active INTEGER);";
        char* errMsg = nullptr;
        rc = sqlite3_exec(db, userTableSQL, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating users table: " << (errMsg ? errMsg : "unknown") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        // Create books table if it doesn't exist
        const char* bookTableSQL =
            "CREATE TABLE IF NOT EXISTS books ("
            "id TEXT PRIMARY KEY, "
            "name TEXT, "
            "author TEXT, "
            "year TEXT, "
            "currentUser TEXT, "
            "tags TEXT);";
        rc = sqlite3_exec(db, bookTableSQL, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Error creating books table: " << (errMsg ? errMsg : "unknown") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    void Database::disconnect() {
        if (connected && db) {
            sqlite3_close(db);
            db = nullptr;
            connected = false;
        }
    }

    bool Database::isConnected() const {
        return connected;
    }




    // Book operations
    bool Database::addBook(const Book& book) {
        if (!connected) return false;
        const char* sql = "INSERT INTO books (id, name, author, year, currentUser, tags) VALUES (?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, book.getBookID().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, book.getBookName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, book.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, book.getPublicationYear().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, book.getCurrentUser().c_str(), -1, SQLITE_TRANSIENT);
        // Assume book.getTags() returns a vector<string> of tags, store as comma-separated string
        std::string tagsStr;
        const auto& tags = book.getTags();
        for (size_t i = 0; i < tags.size(); ++i) {
            tagsStr += tags[i];
            if (i + 1 < tags.size()) tagsStr += ",";
        }
        sqlite3_bind_text(stmt, 6, tagsStr.c_str(), -1, SQLITE_TRANSIENT);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool Database::removeBook(const std::string& bookID) {
        if (!connected) return false;
        const char* sql = "DELETE FROM books WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, bookID.c_str(), -1, SQLITE_TRANSIENT);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool Database::updateBook(const Book& book) {
        if (!connected) return false;
        const char* sql = "UPDATE books SET name = ?, author = ?, year = ?, currentUser = ?, tags = ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, book.getBookName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, book.getAuthor().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, book.getPublicationYear().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, book.getCurrentUser().c_str(), -1, SQLITE_TRANSIENT);
        // Convert tags vector to comma-separated string
        std::string tagsStr;
        const auto& tags = book.getTags();
        for (size_t i = 0; i < tags.size(); ++i) {
            tagsStr += tags[i];
            if (i + 1 < tags.size()) tagsStr += ",";
        }
        sqlite3_bind_text(stmt, 5, tagsStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, book.getBookID().c_str(), -1, SQLITE_TRANSIENT);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    Book Database::getBook(const std::string& bookID) const {
        if (!connected) return Book("", "", "");
        const char* sql = "SELECT id, name, author, year, currentUser, tags FROM books WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return Book("", "", "");
        sqlite3_bind_text(stmt, 1, bookID.c_str(), -1, SQLITE_TRANSIENT);
        Book result("", "", "");
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result.setBookID(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            result.setBookName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            result.setAuthor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            result.setPublicationYear(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            result.setCurrentUser(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            // Parse tags from comma-separated string
            std::string tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            std::vector<std::string> tags;
            size_t start = 0, end = 0;
            while ((end = tagsStr.find(',', start)) != std::string::npos) {
                tags.push_back(tagsStr.substr(start, end - start));
                start = end + 1;
            }
            if (!tagsStr.empty() && start < tagsStr.size())
                tags.push_back(tagsStr.substr(start));
            result.setTags(tags);
        }
        sqlite3_finalize(stmt);
        return result;
    }

    std::vector<Book> Database::getAllBooks() const {
        std::vector<Book> books;
        if (!connected) return books;
        const char* sql = "SELECT id, name, author, year, currentUser, tags FROM books;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return books;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Book b("", "", "");
            b.setBookID(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            b.setBookName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            b.setAuthor(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            b.setPublicationYear(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            b.setCurrentUser(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            // Parse tags from comma-separated string
            std::string tagsStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            std::vector<std::string> tags;
            size_t start = 0, end = 0;
            while ((end = tagsStr.find(',', start)) != std::string::npos) {
                tags.push_back(tagsStr.substr(start, end - start));
                start = end + 1;
            }
            if (!tagsStr.empty() && start < tagsStr.size())
                tags.push_back(tagsStr.substr(start));
            b.setTags(tags);
            books.push_back(b);
        }
        sqlite3_finalize(stmt);
        return books;
    }




    // User operations
    bool Database::addUser(const User& user) {
        if (!connected) return false;
        // Serialize borrowedBooks as comma-separated string
        std::ostringstream oss;
        for (size_t i = 0; i < user.getBorrowedBooks().size(); ++i) {
            if (i > 0) oss << ",";
            oss << user.getBorrowedBooks()[i];
        }
        std::string borrowedBooksStr = oss.str();
        const char* sql = "INSERT INTO users (id, name, email, dob, address, borrowed_books, is_active) VALUES (?, ?, ?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, user.getUserID().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, user.getName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, user.getEmail().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, user.getDOB().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, user.getAddress().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, borrowedBooksStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, user.active() ? 1 : 0);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool Database::removeUser(const std::string& userID) {
        if (!connected) return false;
        const char* sql = "DELETE FROM users WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_TRANSIENT);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool Database::updateUser(const User& user) {
        if (!connected) return false;
        std::ostringstream oss;
        for (size_t i = 0; i < user.getBorrowedBooks().size(); ++i) {
            if (i > 0) oss << ",";
            oss << user.getBorrowedBooks()[i];
        }
        std::string borrowedBooksStr = oss.str();
        const char* sql = "UPDATE users SET name = ?, email = ?, dob = ?, address = ?, borrowed_books = ?, is_active = ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, user.getName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, user.getEmail().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, user.getDOB().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, user.getAddress().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, borrowedBooksStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 6, user.active() ? 1 : 0);
        sqlite3_bind_text(stmt, 7, user.getUserID().c_str(), -1, SQLITE_TRANSIENT);
        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    User Database::getUser(const std::string& userID) const {
        if (!connected) return User("", "");
        const char* sql = "SELECT id, name, email, dob, address, borrowed_books, is_active FROM users WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return User("", "");
        sqlite3_bind_text(stmt, 1, userID.c_str(), -1, SQLITE_TRANSIENT);
        User result("", "");
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result.setUserID(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            result.setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            result.setEmail(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            result.setDOB(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            result.setAddress(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            // Parse borrowedBooks from comma-separated string
            std::string borrowedBooksStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            std::vector<std::string> borrowedBooks;
            std::istringstream iss(borrowedBooksStr);
            std::string token;
            while (std::getline(iss, token, ',')) {
                if (!token.empty()) borrowedBooks.push_back(token);
            }
            result.setBorrowedBooks(borrowedBooks);
            result.setActive(sqlite3_column_int(stmt, 6) != 0);
        }
        sqlite3_finalize(stmt);
        return result;
    }

    std::vector<User> Database::getAllUsers() const {
        std::vector<User> users;
        if (!connected) return users;
        const char* sql = "SELECT id, name, email, dob, address, borrowed_books, is_active FROM users;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return users;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            User u("", "");
            u.setUserID(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            u.setName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            u.setEmail(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            u.setDOB(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            u.setAddress(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            // Parse borrowedBooks from comma-separated string
            std::string borrowedBooksStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            std::vector<std::string> borrowedBooks;
            std::istringstream iss(borrowedBooksStr);
            std::string token;
            while (std::getline(iss, token, ',')) {
                if (!token.empty()) borrowedBooks.push_back(token);
            }
            u.setBorrowedBooks(borrowedBooks);
            u.setActive(sqlite3_column_int(stmt, 6) != 0);
            users.push_back(u);
        }
        sqlite3_finalize(stmt);
        return users;
    }
} // namespace lms
