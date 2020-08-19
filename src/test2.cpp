#include <iostream>
#include <z3++.h>
#include "adapter.h"

using namespace std;
using namespace z3;

int main() {
    context c;
    // Define variables/ constants
    expr a1 = c.int_const("a1");
    expr a2 = c.int_const("a2");
    expr_vector vars = expr_vector(c);
    vars.push_back(a1);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(5));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    // Define bad conditions
    vector<PathCond> badPathConds;
    expr_vector conjs = expr_vector(c);
    conjs.push_back(a1 < c.int_val(1));
    badPathConds.push_back(PathCond(conjs, &c));
    conjs = expr_vector(c);
    conjs.push_back(a1 == c.int_val(5));
    badPathConds.push_back(PathCond(conjs, &c));

    // Execute
    Adapter adap = Adapter(inp, badPathConds, &c);
    adap.doit();
    return 0;
}
