#ifndef TACKY_GEN_H
#define TACKY_GEN_H

#include "TAC.h"
#include "AST.h"
#include "helper.h"

using namespace AST;
using namespace TAC;

Val* emitExpression(Exp* exp, vector<Instruction*>& instructions, int& nextTmp);
Instruction* emitStatement(Statement* stat, vector<Instruction*>& instructions, int& nextTmp);
TAC::Function* emitFunction(AST::Function* function);
TAC::Program* emitProgram(AST::Program* program);

#endif