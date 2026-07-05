 #ifndef SYNTAX_ERROR_EXCEPTION_H
#define SYNTAX_ERROR_EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class SyntaxErrorException : public exception {
private:
    string msg;

public:
    SyntaxErrorException(const string& message,
                         const string& file,
                         int line,
                         const string& function);

    const char* what() const noexcept override;
};

#define SYNTAX_ERROR(msg) throw SyntaxErrorException(msg, __FILE__, __LINE__, __func__)

#endif