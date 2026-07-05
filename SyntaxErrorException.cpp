#include "SyntaxErrorException.h"

SyntaxErrorException::SyntaxErrorException(
    const std::string& message,
    const std::string& file,
    int line,
    const std::string& function)
{
    msg = file + ":" + std::to_string(line)
        + " in " + function
        + ": " + message;
}

const char* SyntaxErrorException::what() const noexcept {
    return msg.c_str();
}