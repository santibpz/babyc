#include "ASM_AST.h"
#include "TAC.h"

using namespace ASM_AST;

ASM_AST::Function* emitFunction(TAC::Function* function);
vector<ASM_AST::Instruction*> emitInstructions(vector<TAC::Instruction*>instructions);

void replacePseudoRegisters(ASM_AST::Function* function); // TODO: Update this to take program as param