#include "TAC.h"
using namespace TAC;

/*Val Factory*/

Val* TAC::Constant(int value) {
    Val* constant = new Val;
    constant->type=ValType::Constant;
    constant->value = value;
    return constant;
}

Val* TAC::Var(string identifier) {
    Val* var = new Val;
    var-> type = ValType::Var;
    var->identifier = identifier;
    return var;
}

/*Instruction Factory*/

Instruction* TAC::Return(Val* val) {
    Instruction* instruction = new Instruction;
    instruction->type=InstructionType::Return;
    instruction->val=val;
    return instruction;
}

Instruction* TAC::Unary(UnaryOperator op, Val* src, Val* dst) {
    Instruction* unary = new Instruction;
    unary->type = InstructionType::Unary;
    unary->op = op;
    unary->src = src;
    unary->dst = dst;
    return unary;
}

/*Function Factory*/

Function* TAC::Func(string id, vector<Instruction*>instructions) {
    Function* function = new Function;
    function->id=id;
    function->instructions=instructions;
    return function;
}

/* Program Factory*/

Program* TAC::Prog(Function* function) {
    Program* program = new Program;
    program->function = function;
    return program;
}