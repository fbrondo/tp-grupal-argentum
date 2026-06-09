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

    static constexpr int MIN_ARGS = 2;
    static constexpr int BASE_ARG = 3;
    static constexpr int GAME_ARG = 5;     // host port user pass
    static constexpr int SIGNUP_ARG = 10;  // host port --signup user pass race clase head body
    static constexpr int LOGIN_ARG = 6;    // host port --login user pass
    static constexpr int MAX_HEAD_ID = 9;
    static constexpr int MAX_BODY_ID = 9;

    int argc_;
    char** argv_;
};
