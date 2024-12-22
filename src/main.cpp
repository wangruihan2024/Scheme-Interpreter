#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "RE.hpp"
#include <sstream>
#include <iostream>
#include <map>


extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

void REPL()
{
    // read - evaluation - print loop
    Assoc global_env = empty();
    Expr exp = Expr();
    std::vector<std::string> parameter_ = {"x", "y"};
    exp = new Mult(new Var("x"), new Var("y"));
    global_env = extend("*", ClosureV(parameter_, exp, global_env), global_env);
    // *
    exp = new Minus(new Var("x"), new Var("y"));
    global_env = extend("-", ClosureV(parameter_, exp, global_env), global_env);
    // -
    exp = new Plus(new Var("x"), new Var("y"));
    global_env = extend("+", ClosureV(parameter_, exp, global_env), global_env);
    // +
    exp = new Greater(new Var("x"), new Var("y"));
    global_env = extend(">", ClosureV(parameter_, exp, global_env), global_env);
    // >
    exp = new GreaterEq(new Var("x"), new Var("y"));
    global_env = extend(">=", ClosureV(parameter_, exp, global_env), global_env);
    // >=
    exp = new Less(new Var("x"), new Var("y"));
    global_env = extend("<", ClosureV(parameter_, exp, global_env), global_env);
    // <
    exp = new LessEq(new Var("x"), new Var("y"));
    global_env = extend("<=", ClosureV(parameter_, exp, global_env), global_env);
    // <=
    exp = new Equal(new Var("x"), new Var("y"));
    global_env = extend("=", ClosureV(parameter_, exp, global_env), global_env);
    // =
    exp = new IsEq(new Var("x"), new Var("y"));
    global_env = extend("eq?", ClosureV(parameter_, exp, global_env), global_env);
    // =?
    exp = new Cons(new Var("x"), new Var("y"));
    global_env = extend("cons", ClosureV(parameter_, exp, global_env), global_env);
    // cons

    parameter_ = {"x"};
    exp = new IsBoolean(new Var("x"));
    global_env = extend("boolean?", ClosureV(parameter_, exp, global_env), global_env);
    // boolean?
    exp = new IsFixnum(new Var("x"));
    global_env = extend("fixnum?", ClosureV(parameter_, exp, global_env), global_env);
    // fixnum?
    exp = new IsNull(new Var("x"));
    global_env = extend("null?", ClosureV(parameter_, exp, global_env), global_env);
    // null?
    exp = new IsPair(new Var("x"));
    global_env = extend("pair?", ClosureV(parameter_, exp, global_env), global_env);
    // pair?
    exp = new IsProcedure(new Var("x"));
    global_env = extend("procedure?", ClosureV(parameter_, exp, global_env), global_env);
    // procedure?
    exp = new IsSymbol(new Var("x"));
    global_env = extend("symbol?", ClosureV(parameter_, exp, global_env), global_env);
    // symbol
    exp = new Not(new Var("x"));
    global_env = extend("not", ClosureV(parameter_, exp, global_env), global_env);
    // not
    exp = new Car(new Var("x"));
    global_env = extend("car", ClosureV(parameter_, exp, global_env), global_env);
    // car
    exp = new Cdr(new Var("x"));
    global_env = extend("cdr", ClosureV(parameter_, exp, global_env), global_env);
    // cdr

    parameter_ = {};
    exp = new MakeVoid();
    global_env = extend("void", ClosureV(parameter_, exp, global_env), global_env);
    // void
    exp = new Exit();
    global_env = extend("exit", ClosureV(parameter_, exp, global_env), global_env);
    // exit
    while (1)
    {
        #ifndef ONLINE_JUDGE
            std::cout << "scm> ";
        #endif
        Syntax stx = readSyntax(std :: cin); // read
        try
        {
            Expr expr = stx -> parse(global_env); // parse
            // stx -> show(std :: cout); // syntax print
            Value val = expr -> eval(global_env);
            if (val -> v_type == V_TERMINATE)
                break;
            val -> show(std :: cout); // value print
        }
        catch (const RuntimeError &RE)
        {
            // std :: cout << RE.message();
            std :: cout << "RuntimeError";
        }
        puts("");
    }
}


int main(int argc, char *argv[]) {
    initPrimitives();
    initReservedWords();
    REPL();
    return 0;
}
