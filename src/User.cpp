#include "../include/lms/User.h"
#include "../include/lms/Book.h"
#include "../include/lms/Database.h"
#include <algorithm>

namespace lms {
    User::User(const std::string& name, const std::string& email, const std::string& dob, const std::string& address)
        : name(name), email(email), dob(dob), address(address), isActive(true) {}

    std::string User::getUserID() const { return userID; }
    std::string User::getName() const { return name; } 
    std::string User::getEmail() const { return email; }
    std::string User::getDOB() const { return dob; }
    std::string User::getAddress() const { return address; }
    std::vector<std::string> User::getBorrowedBooks() const { return borrowedBooks; }
    bool User::active() const { return isActive; }

    void User::setUserID(const std::string& _userID) { userID = _userID; }
    void User::setName(const std::string& _name) { name = _name; }
    void User::setEmail(const std::string& _email) { email = _email; }
    void User::setDOB(const std::string& _dob) { dob = _dob; }
    void User::setAddress(const std::string& _address) { address = _address; }
    void User::setBorrowedBooks(const std::vector<std::string>& books) { borrowedBooks = books; }
    void User::setActive(bool status) { isActive = status; }

    void User::addBorrowedBook(const std::string& bookID) {
        if (std::find(borrowedBooks.begin(), borrowedBooks.end(), bookID) == borrowedBooks.end())
            borrowedBooks.push_back(bookID);
    }

    void User::removeBorrowedBook(const std::string& bookID) {
        auto it = std::remove(borrowedBooks.begin(), borrowedBooks.end(), bookID);
        if (it != borrowedBooks.end())
            borrowedBooks.erase(it, borrowedBooks.end());
    }

    bool User::borrowBookDB(const std::string& bookID, Database& db) {
        // Update in-memory state
        if (std::find(borrowedBooks.begin(), borrowedBooks.end(), bookID) == borrowedBooks.end())
            borrowedBooks.push_back(bookID);
        else
            return false; // Already borrowed

        // Update user in DB
        if (!db.updateUser(*this)) return false;

        // Update book's currentUser in DB
        Book book = db.getBook(bookID);
        if (book.getBookID().empty()) return false; // Book not found
        book.setCurrentUser(userID);
        book.setAvailable(false);
        return db.updateBook(book);
    }

    bool User::returnBookDB(const std::string& bookID, Database& db) {
        // Update in-memory state
        auto it = std::remove(borrowedBooks.begin(), borrowedBooks.end(), bookID);
        if (it == borrowedBooks.end()) return false; // Not borrowed
        borrowedBooks.erase(it, borrowedBooks.end());

        // Update user in DB
        if (!db.updateUser(*this)) return false;

        // Update book's currentUser in DB
        Book book = db.getBook(bookID);
        if (book.getBookID().empty()) return false; // Book not found
        book.setCurrentUser("");
        book.setAvailable(true);
        return db.updateBook(book);
    }

    std::string User::generateID() const {
        std::string data = name + email + dob + address;
        std::string hash = picosha2::hash256_hex_string(data);
        return hash.substr(0, 32); // 16 bytes (32 hex chars)
    }

    bool User::updateInDB(Database& db) const {
        return db.updateUser(*this);
    }
}
