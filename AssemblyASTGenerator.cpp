
#include "AssemblyASTGenerator.h"
#include "TAC.h"
#include <map>

void emitReturn(TAC::Instruction* tacReturnInstruction, vector<ASM_AST::Instruction*>& asmInstructions) {
    Instruction* mov;
    
    Operand* src;
    Operand* dst;
    
    TAC::Val* returnVal = tacReturnInstruction->val;
    
    if(returnVal->type == ValType::Constant) {
        // it means return instructions is something like: return 2;
        src = Imm(returnVal->value);
    } else if(returnVal->type == ValType::Var) {
        // it means return instructions is something like: return tmp.i;
        src = Pseudo(returnVal->identifier);
    }
    
    dst = Reg(Register::AX);
    mov = Mov(src, dst);
    
    // add mov instruction to asmInstructions
    asmInstructions.push_back(mov);
    
    // call Ret
    Instruction* ret = Ret();
    
    // add ret instruction to asmInstructions
    asmInstructions.push_back(ret);
    
    return;
}

UnaryOp parseOp(UnaryOperator unop) {
    if(unop == UnaryOperator::Complement) {
        return UnaryOp::Not;
    } else if(unop == UnaryOperator::Negate) return UnaryOp::Neg;
}

void emitUnary(TAC::Instruction* tacUnaryInstruction, vector<ASM_AST::Instruction*>& asmInstructions) {
    Instruction* mov;

    Operand* src;
    Operand* dst; // always ValType Var

    if(tacUnaryInstruction->src->type == ValType::Constant) {
        src = Imm(tacUnaryInstruction->src->value);
    } else if(tacUnaryInstruction->src->type == ValType::Var) {
        src = Pseudo(tacUnaryInstruction->src->identifier);
    }

    dst = Pseudo(tacUnaryInstruction->dst->identifier); // always Var

    // create mov instruction
    mov = Mov(src, dst);

    // add mov instruction to asmInstructions
    asmInstructions.push_back(mov);

    Instruction* unaryInstruction = Unary(parseOp(tacUnaryInstruction->op), dst);

     // add unaryInstruction to asmInstructions
     asmInstructions.push_back(unaryInstruction);

    return;

}

vector<ASM_AST::Instruction*> emitInstructions(vector<TAC::Instruction*>instructions) {
    vector<ASM_AST::Instruction*>asmInstructions;
    for(TAC::Instruction* tacInstruction : instructions) {
        // i'd like this to be a switch stmt
        if(tacInstruction->type == InstructionType::Return) {
            // ASM_AST::Instruction* returnInstruction = emitReturn(tac_instruction);
            emitReturn(tacInstruction, asmInstructions);
        } else if(tacInstruction->type == InstructionType::Unary) {
            emitUnary(tacInstruction, asmInstructions);
        }
    }
    return asmInstructions;
}

ASM_AST::Function* emitFunction(TAC::Function* function) {
    string identifier = function->id;
    vector<ASM_AST::Instruction*>instructions = emitInstructions(function->instructions);
    // call Function to crate a func
    return Func(identifier, instructions);
}

/* compiler pass: Replace Pseudo variables with Stack */

/*
@param operand: represents src and/or dst of Mov instruction
*/
void replacePseudoRegister(Operand*& operand, map<string, int>& offsetMap, int& currentOffset) {
    int offset;
    string tempVar = operand->identifier;
    auto it = offsetMap.find(tempVar);
    if (it != offsetMap.end()) {
        // pseudo variable exists in offsetMap so we already know its offset
        offset = it->second;
    } else {
        // variable does not exist
        currentOffset-=4;
        offset = currentOffset;
        offsetMap.insert({tempVar, offset});
    }
    // replace pseudo register with a stack location by specifying the offset
    operand = Stack(offset);
}

void doReplacePseudoRegisters(Instruction*& asmInstruction, map<string, int>& offsetMap, int& currentOffset) {
    // in Mov, src could be Imm or Pseudo and dst is always Pseudo
    // in Unary src is always UnaryOp
    // if(asmInstruction->type == AsmInstructionType::Mov) {
        if(asmInstruction->src->type==OperandType::Pseudo) {
            replacePseudoRegister(asmInstruction->src, offsetMap, currentOffset);
        }
    // }
    // dst operand SHOULD always be Pseudo
    replacePseudoRegister(asmInstruction->dst, offsetMap, currentOffset);
}

void replacePseudoRegisters(ASM_AST::Function* function) {
    map<string, int>offsetMap;
    int currentOffset = 0;
    for(Instruction* asmInstruction : function->instructions) {
        // if(asmInstruction->type==AsmInstructionType::Mov || asmInstruction->type==AsmInstructionType::Unary) {
        if(asmInstruction->type==AsmInstructionType::Mov ) {
            doReplacePseudoRegisters(asmInstruction, offsetMap, currentOffset);
        // } else if(asmInstruction->type==AsmInstructionType::Unary) {
        //    // TODO
        // }
        }
    }
}