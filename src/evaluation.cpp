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

Value Let::eval(Assoc &env) {} // let expression

Value Lambda::eval(Assoc &env) {} // lambda expression

Value Apply::eval(Assoc &e) {} // for function calling

Value Letrec::eval(Assoc &env) {} // letrec expression

Value Var::eval(Assoc &e) {} // evaluation of variable

Value Fixnum::eval(Assoc &e) {
    return IntegerV(n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    
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
        if(s->gettype()==E_LIST){
        List *p = dynamic_cast<List*>(s.get());
        if(!(p->stxs.size())) return NullV();
        Value v = PairV(Quote(p -> stxs[p -> stxs.size() - 1]).eval(e), NullV());
        Value val = PairV(Quote(p -> stxs[p -> stxs.size()-1]).eval(e), NullV());
        for(int i = p->stxs.size()-2 ; i >= 0 ; i--){
            val = PairV(Quote(p->stxs[i]).eval(e), v);
            v = val;
        }
        return val;
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

Value Less::evalRator(const Value &rand1, const Value &rand2) {} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {} // eq?

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

} // symbol?

Value IsNull::evalRator(const Value &rand) {} // null?

Value IsPair::evalRator(const Value &rand) {} // pair?

Value IsProcedure::evalRator(const Value &rand) {} // procedure?

Value Not::evalRator(const Value &rand) {} // not

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
