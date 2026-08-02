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

#endif