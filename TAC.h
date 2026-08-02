#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include "Types.h"

using namespace std;

namespace TAC {

/*
val = Constant(int) | Var(identifier)
*/

struct Val {
    ValType type;

    // Constant
    int value;

    // Var
    string identifier;
};

Val* Constant(int value);
Val* Var(string identifier);

/*
instruction = Return(val) | Unary(unary_operator, val src, val dst)
*/

struct Instruction {
    InstructionType type;

    // Return
    Val* val;

    // Unary
    UnaryOperator op;
    Val* src; 
    Val* dst; // should always be of typer Var
};

Instruction* Return(Val* val);
Instruction* Unary(UnaryOperator op, Val* src, Val* dst);

/*function_definition = Function(identifier, instruction* body)*/

struct Function {
    string id;
    vector<Instruction*> instructions; // body
};

Function* Func(string id, vector<Instruction*> instructions);

/*program = Program (function _definition)*/

struct Program {
    Function* function;
};

Program* Prog(Function* function);

} // namespace tac

#endif