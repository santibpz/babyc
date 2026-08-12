#ifndef ASM_AST_H
#define ASM_AST_H

#include <string>
#include <vector>
#include "Types.h"

using namespace std;

namespace ASM_AST {

/*operand = Imm(int) | Reg(reg) | Pseudo(identifier) | Stack(int)*/

struct Operand {
    OperandType type;

    // Imm: immediate value
    int integer;

    // Register
    Register reg;

    // Pseudo
    string identifier;

    // Stack
    int offset;
};

Operand* Imm(int integer);
Operand* Reg(Register Register);
Operand* Pseudo(string identifier);
Operand* Stack(int offset);

/*
instruction = Mov(operand src, operand dst)
            | Unary (unary_operator, operand)
            | AllocateStack(int)
            | Ret
*/


struct Instruction {
    AsmInstructionType type;

    // Mov
    Operand* src;
    Operand* dst;

    // Unary
    UnaryOp op;
    Operand* operand;

    // AllocateStack
    int offset;

    // Ret

};

Instruction* Mov(Operand* src, Operand* dst);
Instruction* Unary(UnaryOp op, Operand* operand);
Instruction* AllocateStack(int offset);
Instruction* Ret();

/*function_definition = Function(identifier name, instruction* instructions)*/

struct Function {
    string identifier;
    vector<Instruction*> instructions;
};

Function* Func(string identifier, vector<Instruction*> instructions);

/*program = Program(function_definition)*/

struct Program {
    Function* function;
};

Program* Prog(Function* function);

}

#endif