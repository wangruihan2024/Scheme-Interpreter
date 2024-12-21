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

Value IsProcedure::evalRator(const Value &rand) {} // procedure?

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
