// main.cpp
#include <iostream>
#include <algorithm>
#include <cctype>
#include "../include/lms/Database.h"
#include "../include/lms/Book.h"
#include "../include/lms/User.h"
using namespace lms;

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) ++start;
    auto end = s.end();
    do { --end; } while (end != start && std::isspace(*end));
    return std::string(start, end + 1);
}

void listUsers(Database& db) {
    auto users = db.getAllUsers();
    std::cout << "\nUsers in system:\n";
    for (const auto& user : users) {
        std::cout << "- " << user.getName() << " (" << user.getUserID() << ")\n";
    }
}

void listBooks(Database& db) {
    auto books = db.getAllBooks();
    std::cout << "\nBooks in system:\n";
    for (const auto& book : books) {
        std::cout << "- " << book.getBookName() << " by " << book.getAuthor() << " (" << book.getBookID() << ")\n";
    }
}

int main() {
    Database db("test.db");
    if (!db.connect()) {
        std::cerr << "Failed to connect to database!" << std::endl;
        return 1;
    }
    std::cout << "Library Management System Started!\n";
    int choice;
    do {
        std::cout << "\nMenu:\n1. List Users\n2. List Books\n3. Add User\n4. Add Book\n0. Exit\nChoice: ";
        std::cin >> choice;
        switch (choice) {
            case 1: listUsers(db); break;
            case 2: listBooks(db); break;
            case 3: {
                std::cin.ignore(); // flush newline
                std::string name, email, dob, address;
                std::cout << "Enter user name: ";
                std::getline(std::cin, name);
                std::cout << "Enter user email: ";
                std::getline(std::cin, email);
                std::cout << "Enter user date of birth: ";
                std::getline(std::cin, dob);
                std::cout << "Enter user address: ";
                std::getline(std::cin, address);
                name = trim(name);
                email = trim(email);
                dob = trim(dob);
                address = trim(address);
                if (name.empty() || email.empty() || dob.empty() || address.empty()) {
                    std::cout << "[Warning] All fields must be non-empty! User not added.\n";
                } else {
                    User newUser(name, email, dob, address);
                    newUser.setUserID(newUser.generateID()); // Generate unique ID
                    if (db.addUser(newUser)) {
                        std::cout << "User added successfully!\n";
                    } else {
                        std::cout << "Failed to add user.\n";
                    }
                }
                break;
            }
            case 4: {
                std::cin.ignore(); // flush newline
                std::string name, author, year;
                std::cout << "Enter book name: ";
                std::getline(std::cin, name);
                std::cout << "Enter book author: ";
                std::getline(std::cin, author);
                std::cout << "Enter publication year: ";
                std::getline(std::cin, year);
                name = trim(name);
                author = trim(author);
                year = trim(year);
                if (name.empty() || author.empty() || year.empty()) {
                    std::cout << "[Warning] All fields must be non-empty! Book not added.\n";
                } else {
                    Book newBook(name, author, year);
                    newBook.setBookID(newBook.generateID()); // Generate unique ID
                    newBook.setAvailable(true); // Set book as available
                    if (db.addBook(newBook)) {
                        std::cout << "Book added successfully!\n";
                    } else {
                        std::cout << "Failed to add book.\n";
                    }
                }
                break;
            }
            case 0: std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    db.disconnect();
    return 0;
}
