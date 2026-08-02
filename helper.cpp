#include "helper.h"

stringstream message(string message) {
    stringstream ss_msg;
    ss_msg << "Syntax Error: Unexpected token -> " << message << endl;
    return ss_msg;
}

string makeTemp(int tmp) {
    return  "tmp." + to_string(tmp);
}