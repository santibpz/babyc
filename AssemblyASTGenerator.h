#include "ASM_AST.h"
#include "TAC.h"

using namespace ASM_AST;

ASM_AST::Instruction* emitInstructions(vector<TAC::Instruction*>instructions);
ASM_AST::Function* emitFunction(TAC::Function* function);
ASM_AST::Program* emitProgram(TAC::Program* program);

int replacePseudoRegisters(ASM_AST::Program*& program); 

void fixInstructions(ASM_AST::Program*& program, int stackOffset);