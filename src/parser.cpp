#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include <map>
#include <cstring>
#include <iostream>
#define mp make_pair
using std :: string;
using std :: vector;
using std :: pair;

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;


Expr Syntax :: parse(Assoc &env) {
    return (*this)->parse(env);
}

Expr Number :: parse(Assoc &env) {
    return Expr(new Fixnum(n));
}
ExprType Number :: gettype() {
    return E_FIXNUM;
}
Expr Identifier :: parse(Assoc &env) {
    return Expr(new Var(s));
}
ExprType Identifier :: gettype() {
    if(primitives[s])
        return primitives[s];
    if(reserved_words[s])
        return reserved_words[s];
    if(s == "let")
        return E_LET;
    if(s == ".")
        return E_DOT;
    return E_VAR;
}

Expr TrueSyntax :: parse(Assoc &env) {
    return Expr(new True());
}
ExprType TrueSyntax :: gettype() {
    return E_TRUE;
}

Expr FalseSyntax :: parse(Assoc &env) {
    return Expr(new False());
}
ExprType FalseSyntax :: gettype() {
    return E_FALSE;
}
Expr List :: parse(Assoc &env) {
    if(!stxs.size())
        throw RuntimeError("empty");
    ExprType type = stxs[0]->gettype();
    /*stxs[0]->show(std::cout);
    stxs[1]->show(std::cout);
    stxs[2]->show(std::cout);*/
    if(type == E_PLUS) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Plus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_MINUS) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Minus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_MUL) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Mult(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_EXIT) {
        if(stxs.size() != 1)
            throw RuntimeError("RE");
        return Expr(new Exit());
    }else if(type == E_BOOLQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsBoolean(stxs[1]->parse(env)));
    }else if(type == E_VOID) {
        if(stxs.size() != 1)
            throw RuntimeError("RE");
        return Expr(new MakeVoid());
    }else if(type == E_CONS) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Cons(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_QUOTE) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new Quote(stxs[1]));
    }else if(type == E_CAR) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new Car(stxs[1]->parse(env)));
    }else if(type == E_CDR) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new Cdr(stxs[1]->parse(env)));
    }else if(type == E_BEGIN) {
        vector<Expr> task;
        for (int i = 1; i <= stxs.size(); i++)
            task.push_back(stxs[i]->parse(env));
        return Expr(new Begin(task));
    }
    
}
ExprType List :: gettype() {
    return E_LIST;
}

#endif