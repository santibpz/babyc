// Lexer for baby c

/*

State table

c = [a-zA-Z]
b = \t\n
d = [0-9]

final states = [3...10] // 3 to 10

    c   (   )   {   }   b   d   ;   ~   -  other
0   1   6   7   8   9   0   2   10  11  3   14
1   1   4   4   4   4   4   1   4   4   4   4
2   1   5   5   5   5   5   2   5   5   5   5
3   12  12  12  12  12  12  12  12  12  13  12

final states

4 = identifier found
5 = digit found
...
12 = "-" found
13 = "--" found
14 = token not identified
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
#include <regex>
#include "helper.h"
#include "LexicalError.h"
#include "SyntaxErrorException.h"
#include "AST.h"
#include "TackyGenerator.h"
#include "AssemblyASTGenerator.h"


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


// vector<vector<int>>stateTable = {
//     {1,5,6,7,8,0,2,9,10,11},
//     {1,3,3,3,3,3,1,3,3,3},
//     {1,4,4,4,4,4,2,4,4,4}
// };

vector<vector<int>> stateTable = {
    {1, 6, 7, 8, 9, 0, 2, 10, 11, 3, 14},
    {1, 4, 4, 4, 4, 4, 1, 4, 4, 4, 4},
    {1, 5, 5, 5, 5, 5, 2, 5, 5, 5, 5},
    {12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 12}
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
    {"semicolon", "SEMI_COLON"},
    {"tilde", "TILDE"},
    {"negation", "NEGATION_OP"},
    {"decrement", "DECREMENT_OP"}
};

vector<Token>tokenList;
int current = -1;

void tokenizer(string line) {
    int state = 0;
    int col;
    char curr;
    string currentTokenStr;
    string tokenType;
    stringstream err;
    bool tokenFound = false;
    bool reset = false;
    int i = 0;
    while(i < line.size()) {
        curr = line[i]; // current char being processed

        // check eol
        if(curr == '$') break;

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
        } else if(curr == '~') {
            col = 8;
        } else if(curr == '-') {
            col = 9;
        } else {
            col = 10;
        }

        // update state
        state = stateTable[state][col];

        // Check if final state of current token was reached
        if(state == 4) {
            state = 0;
            // identifer token found
            if(currentTokenStr == "int") {
                tokenType = TokenTypeMap[currentTokenStr];
            } else if(currentTokenStr == "return") {
                tokenType = TokenTypeMap[currentTokenStr];
            } else {
                /*
                    regex check
                    identifiers can NOT begin with digits.
                */ 
               regex pattern("^\\d+\\w+");
               if (regex_match(currentTokenStr, pattern)) {
                err << "Lexical Error: Invalid identifier -> " << currentTokenStr;
                LEXICAL_ERROR(err.str());
            }
                tokenType = TokenTypeMap["identifier"];
            }
            tokenFound = true;
            reset = true;
            i-=1;
        } else if(state == 5) {
            // digit token found
            state = 0;
            tokenType = TokenTypeMap["number"];
            tokenFound = true;
            reset = true;
            i-=1;
           
        } else if(state == 6) {
            // openParen token found
            state = 0;
            tokenType = TokenTypeMap["openParen"];
            tokenFound = true;
            currentTokenStr = curr;
            reset = true;
        } else if(state == 7) {
            // closeParen token found
             state = 0;
            tokenType = TokenTypeMap["closeParen"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 8) {
            // openBrace token found
             state = 0;
            tokenType = TokenTypeMap["openBrace"];
            tokenFound = true;
            currentTokenStr = curr;
            reset = true;
        } else if(state == 9) {
            // closeBrace token found
             state = 0;
            tokenType = TokenTypeMap["closeBrace"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 10) {
            // semicolon token found
            state = 0;
            tokenType = TokenTypeMap["semicolon"];
            tokenFound = true;
            currentTokenStr = curr;
        } else if(state == 11) {
            // tilde token found
            state = 0;
            tokenType = TokenTypeMap["tilde"];
            tokenFound = true;
            currentTokenStr = curr;
            reset = true;
        } else if(state == 12){
            // negation op token found
            state = 0;
            tokenType = TokenTypeMap["negation"];
            tokenFound = true;
            // currentTokenStr = curr;
            reset = true;
            i-=1;
        } else if(state == 13) {
            // decrement op found
            state = 0;
            tokenType = TokenTypeMap["decrement"];
            tokenFound = true;
            currentTokenStr += curr;
            reset = true;
        }
        else if(state == 14) {
            // token not identified
            currentTokenStr = curr;
            err << "Lexical Error: Token not identified -> " << currentTokenStr;
            LEXICAL_ERROR(err.str());
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

UnaryOperator parseUnop(string unopToken) {
    if(unopToken == "~") {
        return UnaryOperator::Complement;
    } else if(unopToken == "-") return UnaryOperator::Negate;
}

Exp* parseExp() {
    Token tkn = next();
    Exp* e;
        if (tkn.tokenType == "NUM" ) { 
            //<int>
            Exp* constant = AST::Constant(tkn.tokenVal);
            e = constant;
        } else if(tkn.tokenType == "TILDE" || tkn.tokenType == "NEGATION_OP") {
            //<unop> <exp>
            UnaryOperator op = parseUnop(tkn.tokenValue);
            Exp* operand = parseExp();
            Exp* unary = Unary(op, operand);
            e = unary;
        } else if(tkn.tokenType == "OPEN_PAREN") {
            //(<exp>)
            Exp* exp = parseExp();
            e = exp;
            // Check at the end CLOSE_PAREN is present
            tkn = next();
            if(tkn.tokenType != "CLOSE_PAREN") {
                ss_msg << "Syntax Error: Expected token \")\" but got \"" << tkn.tokenValue << "\"" << endl;
                SYNTAX_ERROR(ss_msg.str());
            }
        } else {
            // fail
            ss_msg << "Syntax Error: Unexpected token -> " << tkn.tokenValue << endl;
            SYNTAX_ERROR(ss_msg.str());
        }
        return e; // return final expression
}

Statement* parseStatement() {
    Token tkn = next();
    Statement* statement;
        if(tkn.tokenType == "RETURN") { 
            Exp* exp = parseExp(); // this will throw an exception if there is a syntax err
            Statement* returnStmt = Return(exp);
            tkn = next();
            if(tkn.tokenType != "SEMI_COLON") {
                // fail
                ss_msg << "Syntax Error: Expected token -> " << tkn.tokenValue << endl;
                SYNTAX_ERROR(ss_msg.str());
            }
            statement = returnStmt;
        } else {
             ss_msg << "Syntax Error: Malformed statement -> " << endl;
            SYNTAX_ERROR(ss_msg.str());
        }
        return statement;
}

AST::Function* parseFunction() {
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

        Statement* statement = parseStatement(); // function statement

        tkn = next();

        if(tkn.tokenType != "CLOSE_BRACE") {
            // fail
            ss_msg = message(tkn.tokenValue);
            SYNTAX_ERROR(ss_msg.str());
        }

        AST::Function* function = Func(id, statement);
        return function;
}

AST::Program* parseProgram() {
        AST::Function* function = parseFunction();
        AST::Program* program = Prog(function);
        return program;
}

void printExp(Exp* exp) {
    if(exp->type==ExpressionType::Unary) {
        if(exp->op == UnaryOperator::Complement) cout << "Complement" << endl;
        else if(exp->op == UnaryOperator::Negate)cout << "Negate" << endl;
        cout << "\t";
        printExp(exp->operand);
    } else if(exp->type==ExpressionType::Constant) {
        cout << "\tConstant " << exp->value << endl;
    }
}

void printStatement(Statement* stmt) {
    if(stmt->type==StatementType::Return) {
        cout << "Return Statement" << endl;
        printExp(stmt->exp);
    }
    return;
}

void printFunction(AST::Function* fn) {
    cout << "Function " << fn->id << endl;
    printStatement(fn->statement);
}

void printProgram(AST::Program* program) {
    cout << "Program " <<endl;
    printFunction(program->function);
}


int main()
{
    string line;
    fstream fs("./test.txt");
    try {
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
            // Parser
            // Exp* exp = parseExp();
            // printExp(exp);
            // Statement* stmt = parseStatement();
            // printStatement(stmt);
            AST::Function* fn = parseFunction();
            printFunction(fn);
            // AST::Program* prog = parseProgram();
            // printProgram(prog);

            // Generate TAC 
            TAC::Function* fun = emitFunction(fn);
            // TAC::Program* programTAC = emitProgram(prog);

            // Generate Assembly AST
            ASM_AST::Function* asmFn = emitFunction(fun);

            // Replace Pseudo registers  with Stack offset
            replacePseudoRegisters(asmFn);

        }
    } catch (const LexicalError& e) {
        cerr << e.what() << endl;
        return 1;
    } catch (const SyntaxErrorException& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
