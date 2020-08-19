#include <iostream>
#include <z3++.h>
#include "adapter.h"

using namespace std;
using namespace z3;

int main() {
    context c;
    // Define variables/ constants
    expr x = c.int_const("x");
    expr y = c.int_const("y");
    expr temp = c.int_const("temp");
    expr_vector vars = expr_vector(c);
    vars.push_back(x);
    vars.push_back(y);
    vars.push_back(temp);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(-1));
    vals.push_back(c.int_val(1));
    vals.push_back(c.int_val(2));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    expr tmp = (c.int_val(2) >= y);
    func_decl tmpp = tmp.decl();
    cout << tmp.to_string() << " " << tmp.arg(0) << " " << tmp.arg(1) << endl;
    cout << tmpp.name().str() << endl;
    cout << (c.int_val(3)).is_numeral();

    // // Define bad conditions
    // vector<PathCond> badPathConds;
    // expr_vector conjs = expr_vector(c);     // Cond 1
    // conjs.push_back(y <= x);
    // badPathConds.push_back(PathCond(conjs, &c));
    // conjs = expr_vector(c);                 // Cond 2
    // conjs.push_back(x < c.int_val(2));
    // badPathConds.push_back(PathCond(conjs, &c));
    // conjs = expr_vector(c);                 // Cond 3
    // conjs.push_back(x >= c.int_val(4));
    // badPathConds.push_back(PathCond(conjs, &c));
    // conjs = expr_vector(c);                 // Cond 2
    // conjs.push_back(y < c.int_val(2));
    // badPathConds.push_back(PathCond(conjs, &c));
    // conjs = expr_vector(c);                 // Cond 3
    // conjs.push_back(y >= c.int_val(4));
    // badPathConds.push_back(PathCond(conjs, &c));

    // // Execute
    // Adapter adap = Adapter(inp, badPathConds, &c);
    // adap.doit();
    return 0;
}
