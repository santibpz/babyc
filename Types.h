#ifndef TYPES_H
#define TYPES_H

enum class ExpressionType {
    Constant,
    Unary
};

enum class UnaryOperator {
    Complement, // ~
    Negate // -
};

enum class StatementType {
    Return
};

/*TAC Types*/

enum class ValType {
    Constant,
    Var
};

enum class InstructionType {
    Return,
    Unary
};

/*Assemby AST Types*/

enum class OperandType {
    Imm,
    Reg,
    Pseudo,
    Stack
};

enum class Register {
    AX,
    R10
};

enum class UnaryOp {
    Neg,
    Not
};

enum class AsmInstructionType {
    Mov,
    Unary,
    AllocateStack,
    Ret
};

#endif