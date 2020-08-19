#include <iostream>
#include <z3++.h>
#include "adapter.h"

using namespace std;
using namespace z3;

int main() {
    context c;
    // Define variables/ constants
    expr n = c.int_const("n");
    expr unrealVar = c.int_const("unreal");
    expr_vector vars = expr_vector(c);
    vars.push_back(n);
    vars.push_back(unrealVar);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(2));
    vals.push_back(c.int_val(0));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    // Define bad conditions
    vector<PathCond> badPathConds;
    expr_vector conjs = expr_vector(c);
    conjs.push_back(n > c.int_val(0));
    badPathConds.push_back(PathCond(conjs, &c));

    // Execute
    Adapter adap = Adapter(inp, badPathConds, &c);
    adap.doit();
    return 0;
}
