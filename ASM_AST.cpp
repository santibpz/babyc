#include "ASM_AST.h"

using namespace ASM_AST;

/* Operand Factory */

Operand* ASM_AST::Imm(int integer) {
    Operand* imm = new Operand;
    imm->type = OperandType::Imm;
    imm->integer = integer;
    return imm;
}

Operand* ASM_AST::Reg(Register Register) {
    Operand* reg = new Operand;
    reg->type = OperandType::Reg;
    reg->reg = Register;
    return reg;
}

Operand* ASM_AST::Pseudo(string identifier) {
    Operand* pseudo = new Operand;
    pseudo -> type = OperandType::Pseudo;
    pseudo -> identifier = identifier;
    return pseudo;
}

Operand* ASM_AST::Stack(int offset) {
    Operand* stack = new Operand;
    stack->type = OperandType::Stack;
    stack->offset = offset;
    return stack;
}

/* Instruction Factory */

Instruction* ASM_AST::Mov(Operand* src, Operand* dst) {
    Instruction* mov = new Instruction;
    mov->type = AsmInstructionType::Mov;
    mov->src = src;
    mov->dst = dst;
    return mov;
}

Instruction* ASM_AST::Unary(UnaryOp op, Operand* operand) {
    Instruction* unary = new Instruction;
    unary -> type = AsmInstructionType::Unary;
    unary->op = op;
    unary->operand = operand;
    return unary;

}

Instruction* ASM_AST::AllocateStack(int offset) {
    Instruction* allocate = new Instruction;
    allocate->type = AsmInstructionType::AllocateStack;
    allocate->offset= offset;
    return allocate;
}

Instruction* ASM_AST::Ret() {
    Instruction* ret = new Instruction;
    ret->type = AsmInstructionType::Ret;
    return ret;
}

/*Function Factory*/


Function* ASM_AST::Func(string identifier, Instruction* head) {
    Function* function = new Function;
    function->identifier= identifier;
    function->head = head;
    return function;
}

/*Program Factory*/

Program* ASM_AST::Prog(Function* function) {
    Program* program = new Program;
    program->function=function;
    return program;
}
