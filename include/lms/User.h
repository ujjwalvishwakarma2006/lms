#pragma once
#include <string>
#include <vector>
#include "../utils/picosha2.h"

namespace lms {

class Database; // Forward declaration for Database class

class User {
private:
    std::string userID                      = ""; 
    std::string name                        = "";  
    std::string email                       = ""; 
    std::string dob                         = ""; 
    std::string address                     = ""; 
    std::vector<std::string> borrowedBooks  = {}; // List of bookIDs
    bool isActive                           = true;

public:
    User(const User&) = default; // Copy constructor
    User& operator=(const User&) = default; // Copy assignment operator
    User(const std::string& name = "", const std::string& email = "", const std::string& dob = "", const std::string& address = "");

    // Getters
    std::string getUserID() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getDOB() const;
    std::string getAddress() const;
    std::vector<std::string> getBorrowedBooks() const;
    bool active() const;

    // Setters
    void setUserID(const std::string& _userID);
    void setName(const std::string& _name);
    void setEmail(const std::string& _email);
    void setDOB(const std::string& _dob);
    void setAddress(const std::string& _address);
    void setBorrowedBooks(const std::vector<std::string>& books);
    void setActive(bool status);

    // Book management (in-memory only)
    void addBorrowedBook(const std::string& bookID);
    void removeBorrowedBook(const std::string& bookID);

    // Book management (with database sync)
    bool borrowBookDB(const std::string& bookID, Database& db);
    bool returnBookDB(const std::string& bookID, Database& db);

    // ID generation
    std::string generateID() const;

    // Update this user's data in the database
    bool updateInDB(Database& db) const;
};
}
