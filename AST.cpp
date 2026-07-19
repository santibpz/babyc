#include "AST.h"

// Node::Node() : _type("NODE") {}

Node::Node(string type) : _type(type) {}

string Node::getNodeType() const {
    return this->_type;
}

// Exp::Exp() : Node("EXPRESSION"), _integer(0) {}

Exp::Exp(int integer) : Node("EXPRESSION"), _integer(integer) {}

void Exp::setInt(int integer) {
    _integer = integer;
}

int Exp::getInt() const {
    return this->_integer;
}

// Statement::Statement() : Node("STATEMENT") {}

Statement::Statement(Exp exp) : Node("STATEMENT"), exp(exp) {}

Exp Statement::getExp() const {
    return this->exp;
}

// Function::Function() : Node("FUNCTION") {}

Function::Function(string id, Statement stmt) : Node("FUNCTION"), _id(id), _stmt(stmt) {}

string Function::getId() const {
    return this->_id;
}

Statement Function::getStmt() const {
    return this->_stmt;
}

// Program::Program() : Node("PROGRAM") {}

Program::Program(Function function) : Node("PROGRAM"), _function(function) {}

Function Program::getFunction() const {
    return this->_function;
}
