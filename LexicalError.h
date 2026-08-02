#ifndef LEXICAL_ERROR_EXCEPTION_H
#define LEXICAL_ERROR_EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class LexicalError : public exception {
private:
    string msg;

public:
    LexicalError(const string& message,
                 const string& file,
                 int line,
                 const string& function);

    const char* what() const noexcept override;
};

#define LEXICAL_ERROR(msg) throw LexicalError(msg, __FILE__, __LINE__, __func__)

#endif
