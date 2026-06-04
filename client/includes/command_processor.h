#pragma once

class CommandProcessor {
public:
    explicit CommandProcessor(int argc, char* argv[]);
    int run() const;

private:
    bool processSignup() const;
    bool processLogin() const;
    int launchGame() const;
    void printHelp() const;

    static constexpr int MIN_ARGS = 2;
    static constexpr int BASE_ARG = 3;
    static constexpr int SIGNUP_ARG = 10;  // host port --signup user pass race clase head body
    static constexpr int LOGIN_ARG = 6;    // host port --login user password
    static constexpr int MAX_HEAD_ID = 9;
    static constexpr int MAX_BODY_ID = 9;

    int argc_;
    char** argv_;
};
