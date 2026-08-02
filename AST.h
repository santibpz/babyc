#ifndef AST_H
#define AST_H

#include <string>
#include "Types.h"
using namespace std;

namespace AST {

class Node {
public:
    string _type;

    Node(string type);
    string getNodeType() const;
};

// exp = Constant(int) | Unary(unary_operator, exp)

struct Exp {
    ExpressionType type;
    // Constant
    int value;
    // Unary
    UnaryOperator op;
    Exp* operand;
};

// Factory functions
Exp* Constant(int value);
Exp* Unary(UnaryOperator op, Exp* operand);

// statement = Return (exp)

struct Statement {
    StatementType type;
    // Return Statement
    Exp* exp;
};

Statement* Return(Exp* exp);

// function_definition = Function(identifier name, statement body)

struct Function {
    string id;
    Statement* statement;
};

Function* Func(string id, Statement* statement);

// program = Program(function _definition)

struct Program {
    Function* function;
};

Program* Prog(Function* function);

} // namespace ast

#endif
