#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
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
    // std::cout << "here is identifier-var" << std::endl;
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
    // std::cout << "the following is stxs[0]" << std::endl;
    // stxs[0]->show(std::cout);
    if(!stxs.size())
        throw RuntimeError("empty");
    ExprType type = stxs[0]->gettype();
    /*stxs[0]->show(std::cout);
    stxs[1]->show(std::cout);
    stxs[2]->show(std::cout);*/
    Identifier *id = dynamic_cast<Identifier*>(stxs[0].get());
    if(id != nullptr) {
        bool f = 0;
        for (auto i = env; i.get() != nullptr; i = i->next)
            if(i->x == id->s) {
                f = 1;
                break;
            }
        if(f) {
            vector<Expr> task;
            for (int i = 1; i < stxs.size(); i++)
                task.push_back(stxs[i]->parse(env));
            return Expr(new Apply(id->parse(env), task));
        }
    }
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
        for (int i = 1; i < stxs.size(); i++) 
            task.push_back(stxs[i]->parse(env));
        return Expr(new Begin(task));
    }else if(type == E_IF) {
        if(stxs.size() != 4)
            throw RuntimeError("RE");
        return Expr(new If(stxs[1]->parse(env), stxs[2]->parse(env), stxs[3]->parse(env)));
    }else if(type == E_LT) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Less(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_LE) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new LessEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_EQ) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Equal(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_GE) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new GreaterEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_GT) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new Greater(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_NOT) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new Not(stxs[1]->parse(env)));
    }else if(type == E_INTQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsFixnum(stxs[1]->parse(env)));
    }else if(type == E_BOOLQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsBoolean(stxs[1]->parse(env)));
    }else if(type == E_NULLQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsNull(stxs[1]->parse(env)));
    }else if(type == E_PAIRQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsPair(stxs[1]->parse(env)));
    }else if(type == E_SYMBOLQ) {
        if(stxs.size() != 2)
            throw RuntimeError("RE");
        return Expr(new IsSymbol(stxs[1]->parse(env)));
    }else if(type == E_EQQ) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        return Expr(new IsEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }else if(type == E_LAMBDA) {
        // std::cout << "lambda" << std::endl;
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        if(stxs[1]->gettype() != E_LIST)
            throw RuntimeError("RE");
        Assoc e = empty();
        for (auto it = env; it.get() != nullptr; it = it->next) {
            e = extend(it->x, it->v, e);
        }
        vector<string> task;
        List* tasks = dynamic_cast<List*>(stxs[1].get());
        for (int i = 0; i < tasks->stxs.size(); i++) {
            Identifier *temp = dynamic_cast<Identifier *>(tasks->stxs[i].get());
            if(temp == nullptr)
                throw RuntimeError("RE");
            task.push_back(temp->s);
            e = extend(temp->s, NullV(), e);
        }
        return Expr(new Lambda(task, stxs[2]->parse(e)));
    }else if(type == E_LET || type == E_LETREC) {
        if(stxs.size() != 3)
            throw RuntimeError("RE");
        if(stxs[1]->gettype() != E_LIST)
            throw RuntimeError("RE");
        vector<pair<string, Expr>> bind;
        List *bindlist = dynamic_cast<List *>(stxs[1].get());
        Assoc e = empty();
        for (auto i = env; i.get() != nullptr; i = i->next)
            e = extend(i->x, i->v, e);
        for (int i = 0; i < bindlist->stxs.size(); i++)
        {
            if (bindlist->stxs[i]->gettype() != E_LIST)
                throw RuntimeError("RE");
            List *bindsublist = dynamic_cast<List *>(bindlist->stxs[i].get());
            if (bindsublist->stxs.size() != 2)
                throw RuntimeError("RE");
            Identifier *subpair = dynamic_cast<Identifier *>(bindsublist->stxs[0].get());
            if (subpair == nullptr)
                throw RuntimeError("RE");
            bind.push_back(mp(subpair->s, bindsublist->stxs[1]->parse(env)));
            e = extend(subpair->s, NullV(), e);
        }
        if(type == E_LET)
            return Expr(new Let(bind, stxs[2]->parse(env)));
        if(type == E_LETREC)
            return Expr(new Letrec(bind, stxs[2]->parse(env)));
    }
    // std::cout << "NEWapply" << std::endl;
    vector<Expr> task;
    for (int i = 1; i < stxs.size(); i++)
        task.push_back(stxs[i]->parse(env));
    return Expr(new Apply(stxs[0]->parse(env) , task));
    
    
}
ExprType List :: gettype() {
    return E_LIST;
}

#endif