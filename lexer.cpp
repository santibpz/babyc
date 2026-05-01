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
#include <regex>
#include <cctype>
#include <map>

using namespace std;

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
            // cout << "entered state 3 -> " << " " << currentTokenStr;
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
            cout << currentTokenStr << " -> " << tokenType << endl;
        }

        // move to next char
        i++;
    }
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
    }
    return 0;
}
