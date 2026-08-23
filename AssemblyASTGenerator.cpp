
#include "AssemblyASTGenerator.h"
#include "TAC.h"
#include <iostream>
#include <map>


// helper print instructions

void printInstructions(ASM_AST::Instruction* head) {
    ASM_AST::Instruction* aux = head;
    while(aux != nullptr) {

        if(aux->type == AsmInstructionType::AllocateStack) {
            cout << "AllocateStack" << endl;
            cout << "offset: " << aux->offset << endl;
        }

         else if(aux->type == AsmInstructionType::Mov) {
            cout << "MOV" << endl;
            if(aux->src->type == OperandType::Imm) {
                cout << "src: " << aux->src->integer << endl;
            } else if(aux->src->type == OperandType::Pseudo) {
                cout << "src: " << aux->src->identifier << endl;;
            } else if(aux->src->type == OperandType::Stack) {
                cout << "src: " << "stack(" << aux->src->offset << ")" << endl;
            } else if(aux->src->type == OperandType::Reg) {
                string reg = aux->src->reg == Register::AX ? "AX" : "R10";
                cout << "src: " << reg << endl;
            }
            // cout << endl;

            if(aux->dst->type == OperandType::Pseudo) {
                cout << "dst: " << aux->dst->identifier << endl;
            } else if(aux->dst->type == OperandType::Reg) {
                string reg = aux->dst->reg == Register::AX ? "AX" : "R10";
                cout << "dst: " << reg << endl;
            } else if(aux->dst->type == OperandType::Stack) {
                cout << "dst: " << "stack(" << aux->dst->offset << ")" << endl;;
            }

        } 
        
        else if(aux->type == AsmInstructionType::Unary) {
            cout << "Unary" << endl;
            if(aux->op == UnaryOp::Neg) {
                cout << "op: " << "NEG " << endl;
            } else if(aux->op == UnaryOp::Not) {
                cout << "op: " << "NOT" << endl;
            }


            if(aux->operand->type == OperandType::Pseudo) {
                cout << "operand: " << aux->operand->identifier << endl;
            } else if(aux->operand->type == OperandType::Stack) {
                cout << "operand: " << "stack(" << aux->operand->offset << ")" << endl;
            }
        }

        else if(aux->type == AsmInstructionType::Ret) {
            cout << "RET" << endl;
        }
        cout << endl;
        aux = aux ->next;
    }
}


void appendInstruction( ASM_AST::Instruction*& head,  ASM_AST::Instruction*& tail,  ASM_AST::Instruction* instruction) {
    // set the head of the linked list for ASM Instructions
    if (head == nullptr) {
        head = instruction;
    }

    // append the new instruction
     if(tail != nullptr) {
        tail->next = instruction;
    }

    // update reference to last instruction (tail)
    tail = instruction;

    return;
}

Operand* toOperand(TAC::Val* val) {
    Operand* operand;
    switch(val->type) {
        case ValType::Constant:
            operand = Imm(val->value);
            break;
        case ValType::Var:
            operand = Pseudo(val->identifier);
            break;
    }
    return operand;
}

UnaryOp parseOp(UnaryOperator unop) {
    if(unop == UnaryOperator::Complement) {
        return UnaryOp::Not;
    } else if(unop == UnaryOperator::Negate) return UnaryOp::Neg;
}

void emitReturn(TAC::Instruction* tacReturnInstruction, ASM_AST::Instruction*& head, ASM_AST::Instruction*& tail) {
 
    Operand* src = toOperand(tacReturnInstruction->val);
    Operand* dst = Reg(Register::AX);
   
    appendInstruction(
         head,
         tail,
         Mov(src, dst)
    );

    appendInstruction(
         head,
         tail,
         Ret()
    );

    return;
}

void emitUnary(TAC::Instruction* tacUnaryInstruction, ASM_AST::Instruction*& head, ASM_AST::Instruction*& tail) {

    Operand* src = toOperand(tacUnaryInstruction->src);
    Operand* dst = Pseudo(tacUnaryInstruction->dst->identifier); // always Var

    appendInstruction(
        head, 
        tail, 
        Mov(src, dst));

    appendInstruction(
        head, 
        tail,  
        Unary(parseOp(tacUnaryInstruction->op), dst));

    return;

}

ASM_AST::Instruction* emitInstructions(vector<TAC::Instruction*>instructions) {
    Instruction* head = nullptr;
    Instruction* tail = nullptr;
    for(TAC::Instruction* tacInstruction : instructions) {
        // i'd like this to be a switch stmt
        if(tacInstruction->type == InstructionType::Return) {
            emitReturn(tacInstruction, head, tail);
        } else if(tacInstruction->type == InstructionType::Unary) {
            emitUnary(tacInstruction, head, tail);
        }
    }

    return head;
}

ASM_AST::Function* emitFunction(TAC::Function* function) {
    string identifier = function->id;
    Instruction* head = emitInstructions(function->instructions);

    cout << "Printing instructions: " << endl;
    printInstructions(head);

    return Func(identifier, head);
}

ASM_AST::Program* emitProgram(TAC::Program* program) {
    ASM_AST::Function* function = emitFunction(program->function);
    return Prog(function);
}



/* compiler pass: Replace Pseudo variables with Stack */

/*
@param operand: represents src and/or dst of Mov instruction
*/
void doReplacePseudoRegister(Operand*& operand, map<string, int>& offsetMap, int& currentOffset) {
    if (operand->type == OperandType::Pseudo) {
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
}

void doReplacePseudoRegisters(Instruction*& asmInstruction, map<string, int>& offsetMap, int& currentOffset) {
    // in Mov, src could be Imm or Pseudo and dst is always Pseudo
    // in Unary src is always UnaryOp
    if(asmInstruction->type == AsmInstructionType::Mov) {
        doReplacePseudoRegister(asmInstruction->src, offsetMap, currentOffset);
        doReplacePseudoRegister(asmInstruction->dst, offsetMap, currentOffset);
    } else if(asmInstruction->type == AsmInstructionType::Unary) {
        doReplacePseudoRegister(asmInstruction->operand, offsetMap, currentOffset);
    }
}

int replacePseudoRegisters(ASM_AST::Program*& program) {
    map<string, int>offsetMap;
    int stackOffset = 0;
    ASM_AST::Function* function = program->function; 
    ASM_AST::Instruction* currInstruction = function->head;
    while(currInstruction->next!=nullptr) {
        doReplacePseudoRegisters(currInstruction, offsetMap, stackOffset);
        currInstruction = currInstruction->next;
    }

    cout << endl;
    cout << "Printing tree after replacing pseudo register with proper stack Operand" << endl;

    printInstructions(function->head);

    return stackOffset;
}

/*
Compiler pass: Fix instructions
*/

void addAllocateStack(ASM_AST::Function*& function, int stackOffset) {
    // We need to update function's head (root of instructions) with new AllocateStack Instruction
    Instruction* allocateStackInstruction =  AllocateStack(stackOffset);
    allocateStackInstruction -> next = function -> head;
   function -> head = allocateStackInstruction;
   return;
}

ASM_AST::Instruction* fixInstruction(ASM_AST::Instruction* movInstruction) {

    Operand* src = movInstruction->src;
    Operand* dst = movInstruction->dst;
    Operand* addr = Reg(Register::R10);

    ASM_AST::Instruction* load = Mov(src, addr);
    ASM_AST::Instruction* store = Mov(addr, dst);

    load -> next = store;
    store->next = movInstruction->next;

    return load;

}

bool shouldFix(ASM_AST::Instruction* instr) {
    return (instr->type==AsmInstructionType::Mov) && (instr->src->type == OperandType::Stack && instr->dst->type == OperandType::Stack);
}

void fixInvalidInstructions(ASM_AST::Instruction*& head) {
    ASM_AST::Instruction*  prev = nullptr;
    ASM_AST::Instruction* curr = head;
    while(curr != nullptr) {
        if(shouldFix(curr)) {
            ASM_AST::Instruction* next = curr->next;
            ASM_AST::Instruction* loadInstruction = fixInstruction(curr);
            if(prev == nullptr) {
                head = loadInstruction;
            } else {
                prev->next = loadInstruction;
            }
            prev = loadInstruction->next;
            curr = next;

        } else {
            prev = curr;
            curr = curr->next;  
        }
    }
}

void fixInstructions(ASM_AST::Program*& program, int stackOffset) {
    ASM_AST::Function* function = program->function;
    addAllocateStack(function, stackOffset);
    fixInvalidInstructions(function->head);
    printInstructions(function->head);
}