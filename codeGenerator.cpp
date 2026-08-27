#include "codeGenerator.h"

ofstream output("output.s");

string operandEmitter(ASM_AST::Operand* operand) {
    string resolvedOperand;
    switch(operand->type) {
        case OperandType::Imm:
            resolvedOperand = "$" + to_string(operand->integer);
            break;
        case OperandType::Stack:
            resolvedOperand = to_string(operand->offset) + "(%rbp)";
            break;
        case OperandType::Reg:
            string reg = operand->reg == Register::AX ? "%eax" : "%r10d";
            resolvedOperand = reg;
            break;
    }
    return resolvedOperand;
}

string opEmitter(UnaryOp op) {
    string unaryOp;
    switch(op) {
        case UnaryOp::Neg:
            unaryOp = "negl";
            break;
        case UnaryOp::Not:
            unaryOp = "notl";
            break;
    }
    return unaryOp;
}

void instructionsEmitter(ASM_AST::Instruction* head) {
    ASM_AST::Instruction* instruction = head;
    while(instruction!=nullptr) {
        switch(instruction->type) {
            case AsmInstructionType::AllocateStack:
                // int offset = instruction->offset;
                output << "\tsubq" << "\t$" << -(instruction->offset) << ", " << "%rsp" << endl;
                break;
            case AsmInstructionType::Mov:
                // string src = operandEmitter(instruction->src);
                // string dst = operandEmitter(instruction->dst);
                output << "\tmovl" << "\t" << operandEmitter(instruction->src) << ", " << operandEmitter(instruction->dst) << endl;
                break;
            case AsmInstructionType::Unary:
                // string op = opEmitter(instruction->op);
                // string operand = operandEmitter(instruction->operand);
                output << "\t" << opEmitter(instruction->op) << "\t" << operandEmitter(instruction->operand) << endl;
                break;
            case AsmInstructionType::Ret:
                output << "\tmovq" << "\t%rbp, %rsp" << endl;
                output << "\tpopq" << "\t%rbp" << endl;
                output << "\tret" << endl;
                break;

        }
        instruction = instruction->next;
    }

}

void functionEmitter(ASM_AST::Function* function) {
    string id = function->identifier;
    output << "\t.globl " << id << endl;
    output << id << ": " << endl;
    output << "\tpushq" << "\t%rbp" << endl;
    output << "\tmovq" << "\t%rsp, %rbp" << endl;
    instructionsEmitter(function->head);
}

void programEmitter(ASM_AST::Program* program) {
    functionEmitter(program->function);
    output << ".section .note.GNU-stack,\"\",@progbits" << endl;
}
