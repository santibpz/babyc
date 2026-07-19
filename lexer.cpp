// Lexer for baby c

/*

State table

c = [a-zA-Z]
b = \t\n
d = [0-9]

final states = [3...10] // 3 to 10

    c   (   )   {   }   b   d   ;   other
0   1   5   6   7   8   0   2   9   10
1   1   3   3   3   3   3   3   3   3 
2   4   4   4   4   4   4   2   4   4


*/

#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <map>
#include <format>
#include <sstream>
#include <vector>
#include <memory>
#include "helper.h"
#include "SyntaxErrorException.h"
#include "AST.h"


using namespace std;

stringstream ss_msg;
ofstream output("output.s");


class Token {
    public:
    string tokenValue;
    int tokenVal;
    string tokenType;
    Token(){}
    Token(string _token, string _type) {
        tokenValue = _token;
        tokenType = _type;
    }
    Token(int _token, string _type) {
        tokenVal = _token;
        tokenType = _type;
    }
};


vector<vector<int>>stateTable = {
    {1,5,6,7,8,0,2,9,10},
    {1,3,3,3,3,3,3,3,3},
    {4,4,4,4,4,4,2,4,4}
};

map<string, string>TokenTypeMap = {
    {"int", "INT"},
    {"return", "RETURN"},
    {"identifier", "IDENTIFIER"},
    {"number", "NUM"},
    {"openParen", "OPEN_PAREN"},
    {"closeParen", "CLOSE_PAREN"},
    {"openBrace", "OPEN_BRACE"},
    {"closeBrace", "CLOSE_BRACE"},
    {"semicolon", "SEMI_COLON"}
};

vector<Token>tokenList;
int current = -1;

void tokenizer(string line) {
    int state = 0;
    int col;
    char curr;
    string currentTokenStr;
    string tokenType;
    bool tokenFound = false;
    bool reset = false;
    int i = 0;
    while(i < line.size()) {
        curr = line[i]; // current char being processed

        if(reset) {
            currentTokenStr = "";
            reset = false;
        }

        // evaluate what column in the state table should this be set based on type of current char
        if(isalpha(curr)) {
            col = 0;
        } else if(isblank(curr)) {
            col = 5;
        } else if(isdigit(curr)) {
            col = 6;
        } else if(curr == '(') {
            col = 1;
        } else if(curr == ')') {
            col = 2;
        } else if(curr == '{') {
            col = 3;
        } else if(curr == '}') {
            col = 4;
        } else if(curr == ';') {
            col = 7;
        } else {
            col = 8;
        }

        // update state
        state = stateTable[state][col];

        // Check if final state of current token was reached
        if(state == 3) {
            state = 0;
            // identifer token found
            if(currentTokenStr == "int") {
                tokenType = TokenTypeMap[currentTokenStr];
            } else if(currentTokenStr == "return") {
                tokenType = TokenTypeMap[currentTokenStr];
            } else {
                tokenType = TokenTypeMap["identifier"];
            }
            tokenFound = true;
            reset = true;
            i-=1;
        } else if(state == 4) {
            // digit token found
            state = 0;
            tokenType = TokenTypeMap["number"];
            tokenFound = true;
            reset = true;
            i-=1;
           
        } else if(state == 5) {
            // openParen token found
            state = 0;
            tokenType = TokenTypeMap["openParen"];
            tokenFound = true;
            currentTokenStr = curr;
            reset = true;
        } else if(state == 6) {
            // closeParen token found
             state = 0;
            tokenType = TokenTypeMap["closeParen"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 7) {
            // openBrace token found
             state = 0;
            tokenType = TokenTypeMap["openBrace"];
            tokenFound = true;
            currentTokenStr = curr;
            reset = true;
        } else if(state == 8) {
            // closeBrace token found
             state = 0;
            tokenType = TokenTypeMap["closeBrace"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 9) {
            // semicolon token found
            state = 0;
            tokenType = TokenTypeMap["semicolon"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 10) {
            // token not identified
        }

        if(state != 0) {
            // no final state has been found
            currentTokenStr+=curr;
        }

        if(tokenFound) {
            tokenFound = false;
            Token token;
            if(tokenType=="NUM") {
                token = Token(stoi(currentTokenStr), tokenType);
                 cout << token.tokenVal << " -> " << token.tokenType << endl;
            } else {
                token = Token(currentTokenStr, tokenType);
                 cout << token.tokenValue << " -> " << token.tokenType << endl;
            } 

            // cout << token.tokenValue << " -> " << token.tokenType << endl;
            tokenList.push_back(token);
        }

        // move to next char
        i++;
    }
}

Token next() {
    current++;
    return tokenList[current];
}


Exp parseExp() {
    Token tkn = next();
        if (tkn.tokenType != "NUM" ) {
            // fail
            ss_msg << "Syntax Error: Unexpected token -> " << tkn.tokenValue << endl;
            SYNTAX_ERROR(ss_msg.str());
        }
        Exp exp(tkn.tokenVal);
        return exp;
}

Statement parseStatement() {
    Token tkn = next();
        if(tkn.tokenType != "RETURN") { 
        // fail
            ss_msg << "Syntax Error: Unexpected token -> " << tkn.tokenValue << endl;
            SYNTAX_ERROR(ss_msg.str());
        }
        Exp exp = parseExp(); // this will throw an exception if there is a syntax err

        tkn = next();
        if(tkn.tokenType != "SEMI_COLON") {
            // fail
            ss_msg << "Syntax Error: Expected token -> " << tkn.tokenValue << endl;
            SYNTAX_ERROR(ss_msg.str());
        }
        Statement statement(exp);
        return statement;
    
}

Function parseFunction() {
    Token tkn = next();
        if(tkn.tokenType != "INT") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }
        tkn = next();
        if(tkn.tokenType != "IDENTIFIER") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }
        string id = tkn.tokenValue;
        tkn = next();
        if(tkn.tokenType != "OPEN_PAREN") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }
        tkn = next();
        // arguments TODO

        if(tkn.tokenType != "CLOSE_PAREN") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }
        tkn = next();
        if(tkn.tokenType != "OPEN_BRACE") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }

        Statement statement = parseStatement(); // function statement

        tkn = next();

        if(tkn.tokenType != "CLOSE_BRACE") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }

        Function function(id, statement);
        return function;

}

Program parseProgram() {
    try {
        Function function = parseFunction();
        Program program(function);
        return program;
    } catch(const SyntaxErrorException e) {
        cerr << e.what() << endl;
        exit(1);
    }
    
}

void printExpression(Exp exp) {
    cout << exp.getNodeType() << " ";
    cout << exp.getInt();
    return;
}

void printStatement(Statement stmt) {
    cout << stmt.getNodeType() << endl;
    cout << "\t\t";
    printExpression(stmt.getExp());
    return;
}

void printFunction(Function fn) {
    cout << fn.getNodeType() << endl;
    cout << "\t";
    cout << fn.getId() << endl;
    cout << "\t";
    printStatement(fn.getStmt());
}

void printProgram(Program program) {
    cout << program.getNodeType() << endl;
    cout << "\t";
    printFunction(program.getFunction());
}

enum class OperandType { Imm, Register };

struct Operand {
    OperandType type;
    Operand () {};
    Operand(OperandType type) : type(type) {}
};

struct Imm : Operand {
    int value;
    Imm() {};
    Imm(int val) : Operand(OperandType::Imm), value(val) {};
};

enum class RegisterName { EAX };

string getRegisterName(RegisterName registerName) {
        switch (registerName) {
            case RegisterName::EAX: return "eax";
        }
        return "should not fall here";
    }

struct Register : Operand {
    RegisterName registerName;
    Register(RegisterName regName) : Operand(OperandType::Register), registerName(regName) {}
};

enum class InstructionType {
    Mov,
    Ret
};

struct Instruction {
    InstructionType type;
    Instruction(InstructionType type) : type(type) {}
    virtual ~Instruction() = default;
};

struct Mov : Instruction {
    Imm src;
    Register dst;

     Mov(const Imm& s, const Register& d)
        : Instruction(InstructionType::Mov), src(s), dst(d) {}
};

struct Ret : Instruction {
    Ret() : Instruction(InstructionType::Ret) {}
};

struct Func {
    string identifier;
    vector<unique_ptr<Instruction>> instructions;
};

struct Prog {
    Func fn;
};

Imm genExp(const Exp& exp) {
    // Imm imm;
    // if(exp.getNodeType() == "EXPRESSION") {
        Imm imm(exp.getInt());
        // imm.value = exp.getInt();
    // }
    return imm;
}

vector<unique_ptr<Instruction>> genStmt(const Statement& stmt) {
    // statement = statement ::= "return" <exp> ";"
    vector<unique_ptr<Instruction>> instructions;
    if(stmt.getNodeType() == "STATEMENT") {
        Imm imm = genExp(stmt.getExp());
        Register reg(RegisterName::EAX);
        instructions.push_back(make_unique<Mov>(imm, reg));
        instructions.push_back(make_unique<Ret>());
    }
    return instructions;
}


Func genFunc(const Function& function) {
    // <function> ::= "int" <id> "(" ")" "{" <statement> "}"
    Func fn;
    if(function.getNodeType() == "FUNCTION") {
        fn.identifier = function.getId();
        fn.instructions = genStmt(function.getStmt());
    }
    return fn;
}

Prog genProg(const Program& program) {
    // <program> ::= <function>
    Prog prog;
    if(program.getNodeType() == "PROGRAM") {
        Func fn = genFunc(program.getFunction());
        prog.fn = std::move(fn);
    }
    return prog;
}

// Emit Code

void emitMov(const Mov& mov) {
    output << "\t" << "movl\t";
    output << "$" << mov.src.value << ", ";
    output << "%" << getRegisterName(mov.dst.registerName) << endl;

}

void emitRet() {
    output << "\t" << "ret" << endl;
}

void emitInstructions(const vector<unique_ptr<Instruction>>& instructions) {
    for(const auto& instruction: instructions) {
        // Get instruction type
        InstructionType type = instruction->type;
        if(type == InstructionType::Mov) {
            const Mov* mov = static_cast<const Mov*>(instruction.get());
            emitMov(*mov);
        } else if(type == InstructionType::Ret) {
            emitRet();
        }
    }

}

void emitFunc(const Func& fn) {
    output << "\t" << ".globl " << fn.identifier << endl;
    output << fn.identifier << ":" << endl;
    emitInstructions(fn.instructions);
}

void emit(const Prog& prog) {
    emitFunc(prog.fn);
    output << "\t" << ".section\t" ".note.GNU-stack,\"\",@progbits" << endl;
}


int main()
{
    string line;
    fstream fs("./test.txt");
    if (!fs.is_open())
    {

        cout << "Failed to open file";
    }
    else
    {
        while (!fs.eof())
        {
            // process each line with the tokenizer
            getline(fs >> ws, line);
            line.push_back('$');
            tokenizer(line);
        }

        // AST
        Program program = parseProgram(); 
    
        printProgram(program);

        //assemby AST
        Prog p = genProg(program);
        cout << endl;
        emit(p);
    }
    return 0;
}
