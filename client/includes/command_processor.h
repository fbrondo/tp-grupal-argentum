#pragma once

class CommandProcessor {
public:
    explicit CommandProcessor(int argc, char* argv[]);
    int run() const;

private:
    bool processSignup() const;
    bool processLogin() const;
    int launchGame() const;

    static constexpr int BASE_ARG = 3;
    static constexpr int SIGNUP_ARG =
            10;                          // host port --signup user pass race clase head_id body_id
    static constexpr int LOGIN_ARG = 6;  // host port --login user password

    int argc_;
    char** argv_;
};
