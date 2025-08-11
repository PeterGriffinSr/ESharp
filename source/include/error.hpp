#pragma once
#include <exception>
#include <string>

class LexerError : public std::exception {
public:
    LexerError(const std::string& msg, int line, int col, const std::string& sourceLine = "");

    const char* what() const noexcept override;

    int getLine() const;
    int getCol() const;

private:
    std::string message;
    int line;
    int col;
    std::string sourceLine;
    std::string formattedMessage;

    std::string formatMessage() const;
};