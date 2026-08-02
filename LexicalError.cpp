#include "LexicalError.h"

LexicalError::LexicalError(
    const std::string& message,
    const std::string& file,
    int line,
    const std::string& function)
{
    msg = file + ":" + std::to_string(line)
        + " in " + function
        + ": " + message;
}

const char* LexicalError::what() const noexcept {
    return msg.c_str();
}
