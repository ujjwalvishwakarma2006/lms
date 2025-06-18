#include "../include/lms/User.h"
#include "../include/lms/Book.h"
#include "../include/lms/Database.h"
#include <algorithm>

namespace lms {
    Book::Book(const std::string& name, const std::string& author, const std::string& year)
        : name(name), author(author), year(year), isAvailable(true) {}

    std::string Book::getBookID() const { return bookID; }
    std::string Book::getBookName() const { return name; }
    std::string Book::getAuthor() const { return author; }
    std::string Book::getCurrentUser() const { return currentUser; }
    std::string Book::getPublicationYear() const { return year; }
    std::vector<std::string> Book::getTags() const { return tags; }
    bool Book::available() const { return isAvailable; }

    void Book::setBookID(const std::string& _bookID) { bookID = _bookID; }
    void Book::setBookName(const std::string& _name) { name = _name; }
    void Book::setAuthor(const std::string& _author) { author = _author; }
    void Book::setCurrentUser(const std::string& _currentUser) { currentUser = _currentUser; }
    void Book::setPublicationYear(const std::string& _year) { year = _year; }
    void Book::addTag(const std::string& _tag) { tags.push_back(_tag); }
    void Book::setTags(const std::vector<std::string>& _tags) { tags = _tags; }
    void Book::setAvailable(bool _status) { isAvailable = _status; }

    bool Book::updateInDB(Database& db) const {
        return db.updateBook(*this);
    }

    std::string Book::generateID() const {
        std::string data = name + author + year;
        std::string hash = picosha2::hash256_hex_string(data);
        return hash.substr(0, 32); // 16 bytes (32 hex chars)
    }
} // namespace lms
