#pragma once
#include <exception>
#include <string>
#include <utility>

class InvalidBuyException: public std::exception {
private:
    std::string message;

public:
    explicit InvalidBuyException(const std::string& message): message(message) {}
    const char* what() const noexcept override { return message.c_str(); }
    ~InvalidBuyException() override {}
};
