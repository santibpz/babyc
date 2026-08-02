#include "AST.h"
using namespace AST;

Exp* AST::Constant(int value) {
    Exp* constant = new Exp;
    constant->type= ExpressionType::Constant;
    constant->value = value;
    constant->operand = nullptr;    
    return constant;
}

Exp* AST::Unary(UnaryOperator op, Exp* operand) {
    Exp* unary = new Exp;
    unary->type = ExpressionType::Unary;
    unary->op = op;
    unary->operand = operand;
    return unary;
}

Statement* AST::Return(Exp* exp) {
    Statement* statement = new Statement;
    statement->type=StatementType::Return;
    statement->exp = exp;
    return statement;
}

Function* AST::Func(string id, Statement* statement) {
    Function* func = new Function;
    func->id=id;
    func->statement=statement;
    return func;
}

Program* AST::Prog(Function* function) {
    Program* program = new Program;
    program->function = function;
    return program;
}
