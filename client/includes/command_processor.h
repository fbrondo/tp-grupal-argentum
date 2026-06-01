#pragma once

class CommandProcessor {
public:
    explicit CommandProcessor(int argc, char* argv[]);
    int run() const;

private:
    bool processSignup() const;
    bool processLogin() const;
    bool processCreateCharacter() const;
    int launchGame() const;

    static constexpr int BASE_ARG = 3;
    static constexpr int SIGNUP_ARG = 6;  // host port --signup user password
    static constexpr int LOGIN_ARG = 6;   // host port --login user password
    static constexpr int CREATE_CHARACTER_ARG =
            9;  // host port --create-character user password name race class

    int argc_;
    char** argv_;
};
