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
    expr z = c.int_const("z");
    expr_vector vars = expr_vector(c);
    vars.push_back(x);
    vars.push_back(y);
    vars.push_back(z);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(5));
    vals.push_back(c.int_val(6));
    vals.push_back(c.int_val(3));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    // Define bad conditions
    vector<PathCond> badPathConds;
    expr_vector conjs = expr_vector(c);
    conjs.push_back(x <= y);
    conjs.push_back(z < y);
    conjs.push_back(z < x);
    badPathConds.push_back(PathCond(conjs, &c));

    // Execute
    Adapter adap = Adapter(inp, badPathConds, &c);
    adap.doit();
    return 0;
}
