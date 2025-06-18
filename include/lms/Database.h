#pragma once
#include <string>
#include <vector>
#include "../lib/sqlite3/sqlite3.h"
#include "Book.h"
#include "User.h"

namespace lms {
    class Database {
    private:
        std::string dbPath;
        sqlite3* db = nullptr;
        bool connected = false;

    public:
        Database(const std::string& dbPath);
        ~Database();
        
        bool connect();
        void disconnect();
        bool isConnected() const;
        
        // Book operations
        bool addBook(const Book& book);
        bool removeBook(const std::string& bookID);
        bool updateBook(const Book& book);
        Book getBook(const std::string& bookID) const;
        std::vector<Book> getAllBooks() const;
        
        // User operations
        bool addUser(const User& user);
        bool removeUser(const std::string& userID);
        bool updateUser(const User& user);
        User getUser(const std::string& userID) const;
        std::vector<User> getAllUsers() const;
    };
}
