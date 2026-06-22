#pragma once
#include <exception>
#include <string>
#include <utility>

class InvalidSellException: public std::exception {
private:
    std::string message;

public:
    explicit InvalidSellException(const std::string& message): message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
    ~InvalidSellException() override {}
};
