#ifndef AST_H
#define AST_H

#include <string>
using namespace std;

class Node {
public:
    string _type;

    // Node();
    Node(string type);
    string getNodeType() const;
};

class Exp : public Node {
public:
    int _integer;

    // Exp();
    Exp(int integer);
    void setInt(int integer);
    int getInt() const;
};

class Statement : public Node {
public:
    Exp exp;

    // Statement();
    Statement(Exp exp);
    Exp getExp() const;
};

class Function : public Node {
public:
    string _id;
    Statement _stmt;

    // Function();
    Function(string id, Statement stmt);
    string getId() const;
    Statement getStmt() const;
};

class Program : public Node {
public:
    Function _function;

    // Program();
    Program(Function function);
    Function getFunction() const;
};

#endif
