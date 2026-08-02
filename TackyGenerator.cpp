#include "TackyGenerator.h"
#include "helper.h"
#include <format>

Val* emitExpression(Exp* exp, vector<Instruction*>& instructions, int& nextTmp) {
    Val* dst;
    if(exp->type==ExpressionType::Constant) {
        return TAC::Constant(exp->value);
    } else if(exp->type == ExpressionType::Unary) {
        Val* src = emitExpression(exp->operand, instructions, nextTmp);
        dst = Var(makeTemp(++nextTmp));
        instructions.push_back(TAC::Unary(exp->op, src, dst));
    }
    return dst;
}

Instruction* emitStatement(Statement* statement, vector<Instruction*>& instructions, int& nextTmp) {
    // TODO: later on we should differentiate different types of instructions/statements
    Val* exp = emitExpression(statement->exp, instructions, nextTmp);
    return Return(exp);
}

 TAC::Function* emitFunction(AST::Function* function) {
    vector<Instruction*>instructions;
    int nextTmp = -1;
    string id = function->id;
    Instruction* returnInstruction = emitStatement(function->statement, instructions, nextTmp);
    instructions.push_back(returnInstruction);
    return Func(id, instructions);
 }

TAC::Program* emitProgram(AST::Program* program) {
     TAC::Function* function = emitFunction(program->function);
     return Prog(function);
}

