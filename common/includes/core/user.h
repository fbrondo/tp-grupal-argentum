#ifndef USER_H
#define USER_H
#include <string>

struct User {
    std::string username;
    std::string password;

    User() = default;
    User(const User&) = default;
    User& operator=(const User&) = default;
    User(User&&) = default;
    User& operator=(User&&) = default;
    User(std::string user, std::string pass): username(user), password(pass) {}
};

#endif
