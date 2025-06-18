#pragma once
#include <string>
#include <vector>

namespace lms {

class Database; // Forward declaration for Database class

class Book {
private:
    std::string bookID              = "";
    std::string name                = "";
    std::string author              = "";
    std::string year                = "";
    std::string currentUser         = "";
    std::vector<std::string> tags   = {};           // Added: tags for categorization
    bool isAvailable                = true;         // Added: availability status

public:
    // Use default values for all parameters, so a single constructor can be used for all cases
    Book(const std::string& name = "", const std::string& author = "", const std::string& year = "");
    
    // Getters
    std::string getBookID() const;
    std::string getBookName() const;
    std::string getAuthor() const;
    std::string getCurrentUser() const;
    std::string getPublicationYear() const;
    std::vector<std::string> getTags() const;
    bool available() const;

    // Setters
    void setBookID(const std::string& _bookID);
    void setBookName(const std::string& _name);
    void setAuthor(const std::string& _author);
    void setCurrentUser(const std::string& _currentUser);
    void setPublicationYear(const std::string& _year);
    void addTag(const std::string& _tag);
    void setTags(const std::vector<std::string>& _tags);
    void setAvailable(bool _status);

    // Generators
    std::string generateID() const;
    std::string generateTagString() const;

    // Update this book's data in the database
    bool updateInDB(Database& db) const;
};
}
