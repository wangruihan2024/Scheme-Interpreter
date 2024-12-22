#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {
    Assoc e = empty();
    for (auto it = env; it.get() != nullptr; it = it->next)
        e = extend(it->x, it->v, e);
    for (int i = 0; i < bind.size(); i++) {
        bool if_change = false;
        for (auto j = e; j.get() != nullptr; j = j->next) {
            if(j->x == bind[i].first) {
                if_change = true;
                modify(bind[i].first, bind[i].second->eval(env), e);
                break;
            }
        }
        if(!if_change)
            e = extend(bind[i].first, bind[i].second->eval(env), e);
    }
    return body->eval(e);
} // let expression

Value Lambda::eval(Assoc &env) {
    // std::cout << "this is closureV" << std::endl;
    // env->show(std::cout);
    return ClosureV(x, e, env);
} // lambda expression

Value Apply::eval(Assoc &e) {
    Value v_temp = rator->eval(e);
    if(v_temp->v_type != V_PROC)
        throw RuntimeError("RE");
    Closure *clo = dynamic_cast<Closure *>(v_temp.get());
    if(clo->parameters.size() != rand.size())
        throw RuntimeError("RE");
    Assoc upd_e = empty();
    for (auto i = e; i.get() != nullptr; i = i->next) 
        upd_e = extend(i->x, i->v, upd_e);
    for (auto it = clo->env; it.get() != nullptr; it = it->next) {
        bool if_change = false;
        for (auto it2 = upd_e; it2.get() != nullptr; it2 = it2->next) {
            if(it->x == it2->x) {
                if_change = true;
                modify(it2->x, it2->v, upd_e);
                break;
            }
        }
        if(!if_change)
            extend(it->x, it->v, upd_e);
    }
    for (int i = 0; i < rand.size(); i++) {
        Value v = rand[i]->eval(e);
        bool if_change = 0;
        for (auto it = upd_e; it.get() != nullptr; it = it->next) {
            if(clo->parameters[i] == it->x) {
                modify(clo->parameters[i], v, upd_e);
                if_change = true;
                break;
            }
        }
        if(!if_change)
            upd_e = extend(clo->parameters[i], v, upd_e);
    }
    return clo->e->eval(upd_e);
} // for function calling

Value Letrec::eval(Assoc &env) {
    Assoc e1 = empty(), e2 = empty();
    for (auto it = env; it.get() != nullptr; it = it->next) 
        e1 = extend(it->x, it->v, e1);
    for (int i = 0; i < bind.size(); i++) {
        bool if_change = false;
        for (auto j = e1; j.get() != nullptr; j = j->next) {
            if(bind[i].first == j->x) {
                modify(j->x, Value(nullptr), e1);
                if_change = true;
                break;
            }
        }
        if(!if_change) 
            e1 = extend(bind[i].first, Value(nullptr), e1);
    }
    for (auto it = e1; it.get() != nullptr; it = it->next)
        e2 = extend(it->x, it->v, e2);
    for (int i = 0; i < bind.size(); i++)
        modify(bind[i].first, bind[i].second->eval(e2), e1);
    return body->eval(e1);
} // letrec expression

Value Var::eval(Assoc &e) {
    // std::cout << "here is identifier-var" << std::endl;
    // for (auto it = e; it.get() != nullptr; it = it->next){
    //     std::cout << it->x << std::endl;
    // }
    Value tmp = find(x, e);
    if(tmp.get() == nullptr)
        throw RuntimeError("RE");
    return tmp;
    
} // evaluation of variable

Value Fixnum::eval(Assoc &e) {
    return IntegerV(n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    Value p = cond->eval(e);
    Boolean *temp = dynamic_cast<Boolean*>(p.get());
    if(temp == nullptr)
        return conseq->eval(e);
    else if(temp->b)
        return conseq->eval(e);
    return alter->eval(e);
} // if expression

Value True::eval(Assoc &e) {
    return BooleanV(true);
} // evaluation of #t

Value False::eval(Assoc &e) {
    return BooleanV(false);
} // evaluation of #f

Value Begin::eval(Assoc &e) {
    if(es.size() == 0)
        return NullV();
    for (auto it = 0; it != es.size() - 1; it++)
        es[it]->eval(e);
    return es[es.size() - 1]->eval(e);
} // begin expression

Value Quote::eval(Assoc &e) {
    ExprType typ = s->gettype();
    if(typ == E_FIXNUM) {
        Number* p = dynamic_cast<Number*>(s.get());
        return IntegerV(p->n);
    }else if(typ == E_FALSE) {
        return BooleanV(false);
    }else if(typ == E_TRUE) {
        return BooleanV(true);
    }else if(typ == E_LIST) {
        List *p = dynamic_cast<List*>(s.get());
        /*p->stxs[0]->show(std::cout);
        std::cout << std::endl;
        p->stxs[1]->show(std::cout);
        std::cout << std::endl;
        p->stxs[2]->show(std::cout);
        std::cout << std::endl;*/
        if(!(p->stxs.size())) return NullV();
        if(p->stxs.size() >= 3 && p -> stxs[p -> stxs.size() - 2] -> gettype() == E_DOT){
            Value v = Quote(p -> stxs[p -> stxs.size() - 1]).eval(e);
            for (int i = p->stxs.size() - 3; i >= 0; i--)
                v = PairV(Quote(p->stxs[i]).eval(e), v);
            return v;
        }else {
            Value v = NullV();
            for(int i = p->stxs.size()-1 ; i >= 0 ; i--)
                v = PairV(Quote(p->stxs[i]).eval(e), v);
            return v;
        }
    }else{
        Identifier *p = dynamic_cast<Identifier*>(s.get());
        return SymbolV(p->s);
    }
} // quote expression

Value MakeVoid::eval(Assoc &e) {
    return VoidV();
} // (void)

Value Exit::eval(Assoc &e) {
    return TerminateV();
} // (exit)

Value Binary::eval(Assoc &e) {
    Value a = rand1->eval(e), b = rand2->eval(e);
    if(e_type == E_PLUS) {
        Plus *p = dynamic_cast<Plus*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_MINUS) {
        Minus *p = dynamic_cast<Minus*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_MUL) {
        Mult *p = dynamic_cast<Mult*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_CONS) {
        Cons *p = dynamic_cast<Cons*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_LT) {
        Less *p = dynamic_cast<Less*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_LE) {
        LessEq *p = dynamic_cast<LessEq*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_EQ) {
        Equal *p = dynamic_cast<Equal*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_GE) {
        GreaterEq *p = dynamic_cast<GreaterEq*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_GT) {
        Greater *p = dynamic_cast<Greater*>(this);
        return p->evalRator(a, b);
    }
    if(e_type == E_EQQ) {
        IsEq *p = dynamic_cast<IsEq *>(this);
        return p->evalRator(a, b);
    }

} // evaluation of two-operators primitive

Value Unary::eval(Assoc &e) {
    Value v = rand->eval(e);
    if(e_type == E_BOOLQ) {
        IsBoolean *p = dynamic_cast<IsBoolean *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_CAR) {
        Car *p = dynamic_cast<Car *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_CDR) {
        Cdr *p = dynamic_cast<Cdr *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_NOT) {
        Not *p = dynamic_cast<Not *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_INTQ) {
        IsFixnum *p = dynamic_cast<IsFixnum *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_BOOLQ) {
        IsBoolean *p = dynamic_cast<IsBoolean *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_NULLQ) {
        IsNull *p = dynamic_cast<IsNull *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_PAIRQ) {
        IsPair *p = dynamic_cast<IsPair *>(this);
        return p->evalRator(v);
    }
    if(e_type == E_SYMBOLQ) {
        IsSymbol *p = dynamic_cast<IsSymbol *>(this);
        return p->evalRator(v);
    }
} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return IntegerV((a->n) * (b->n));
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return IntegerV(a->n + b->n);
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return IntegerV((a->n) - (b->n));
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return BooleanV(a->n < b->n);
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return BooleanV(a->n <= b->n);
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return BooleanV(a->n == b->n);
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return BooleanV(a->n >= b->n);
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("mistaken type");
    Integer *a = dynamic_cast<Integer *>(rand1.get());
    Integer *b = dynamic_cast<Integer *>(rand2.get());
    return BooleanV(a->n > b->n);
} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {
    if(rand1->v_type == rand2->v_type) {
        ValueType vt = rand1->v_type;
        if(vt == V_INT) {
            Integer *p1 = dynamic_cast<Integer *>(rand1.get());
            Integer *p2 = dynamic_cast<Integer *>(rand2.get());
            return BooleanV(p1->n == p2->n);
        }else if(vt == V_BOOL) {
            Boolean *p1 = dynamic_cast<Boolean *>(rand1.get());
            Boolean *p2 = dynamic_cast<Boolean *>(rand2.get());
            return BooleanV(p1->b == p2->b);
        }else if(vt == V_SYM) {
            Symbol *p1 = dynamic_cast<Symbol *>(rand1.get());
            Symbol *p2 = dynamic_cast<Symbol *>(rand2.get());
            return BooleanV(p1->s == p2->s);
        }else if(vt == V_NULL || vt == V_VOID) {
            return BooleanV(true);
        }else {
            return BooleanV(rand1.get() == rand2.get());
        }
    }
    return BooleanV(false);
} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
} // cons

Value IsBoolean::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_BOOL);
} // boolean?

Value IsFixnum::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_INT);
} // fixnum?

Value IsSymbol::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_SYM);
} // symbol?

Value IsNull::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_NULL);
} // null?

Value IsPair::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_PAIR);
} // pair?

Value IsProcedure::evalRator(const Value &rand) {
    return BooleanV(rand->v_type == V_PROC);
} // procedure?

Value Not::evalRator(const Value &rand) {
    if(rand->v_type == V_BOOL) {
        Boolean *p = dynamic_cast<Boolean *>(rand.get());
        if(p->b)
            return BooleanV(false);
        return BooleanV(true);
    }
    return BooleanV(false);
} // not

Value Car::evalRator(const Value &rand) {
    if(rand->v_type != V_PAIR)
        throw RuntimeError("RE");
    Pair *p = dynamic_cast<Pair*>(rand.get());
    return p->car;
} // car

Value Cdr::evalRator(const Value &rand) {
    if(rand->v_type != V_PAIR)
        throw RuntimeError("RE");
    Pair *p = dynamic_cast<Pair*>(rand.get());
    return p->cdr;
} // cdr
