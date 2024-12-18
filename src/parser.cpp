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
    return E_VAR;
}

Expr TrueSyntax :: parse(Assoc &env) {}
ExprType TrueSyntax :: gettype() {
    return E_TRUE;
}

Expr FalseSyntax :: parse(Assoc &env) {}
ExprType FalseSyntax :: gettype() {
    return E_FALSE;
}
Expr List :: parse(Assoc &env) {
    if(!stxs.size())
        throw RuntimeError("empty");
    ExprType type = stxs[0]->gettype();

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
    }
    
}
ExprType List :: gettype() {
    return E_LIST;
}

#endif