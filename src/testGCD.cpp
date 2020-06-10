#include <iostream>
#include <z3++.h>
#include "adapter.h"

using namespace std;
using namespace z3;

int main() {
    context c;
    // Define variables/ constants
    expr a = c.int_const("a");
    expr b = c.int_const("b");
    expr_vector vars = expr_vector(c);
    vars.push_back(a);
    vars.push_back(b);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(4));
    vals.push_back(c.int_val(4));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    // Define bad conditions
    vector<PathCond> badPathConds;
    expr_vector conjs = expr_vector(c);
    conjs.push_back(a > c.int_val(0));
    conjs.push_back(a < c.int_val(5));
    conjs.push_back(b > c.int_val(0));
    badPathConds.push_back(PathCond(conjs, &c));

    // Execute
    Adapter adap = Adapter(inp, badPathConds, &c);
    adap.doit();
    return 0;
}
