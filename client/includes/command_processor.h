#pragma once

#include <string>

class CommandProcessor {
public:
    explicit CommandProcessor(int argc, char* argv[]);
    int run() const;

private:
    int processSignup() const;
    int processLogin() const;
    int launchGame(const std::string& user, const std::string& pass) const;
    void printHelp() const;

    int argc_;
    char** argv_;
};
