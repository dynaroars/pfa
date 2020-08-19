#include <iostream>
#include <z3++.h>
#include "adapter.h"

using namespace std;
using namespace z3;

int main() {
    context c;
    // Define variables/ constants
    expr c0 = c.int_const("c0");
    expr c1 = c.int_const("c1");
    expr c2 = c.int_const("c2");
    expr_vector vars = expr_vector(c);
    vars.push_back(c0);
    vars.push_back(c1);
    vars.push_back(c2);

    // Define values
    expr_vector vals = expr_vector(c);
    vals.push_back(c.int_val(2));
    vals.push_back(c.int_val(4));
    vals.push_back(c.int_val(4));
    
    // Define Input
    Input inp = Input(vars, vals, &c);

    // Define bad conditions
    vector<PathCond> badPathConds;
    expr_vector conjs = expr_vector(c);
    conjs.push_back((c1 % c.int_val(2)) == c.int_val(0));
    badPathConds.push_back(PathCond(conjs, &c));

    // Execute
    Adapter adap = Adapter(inp, badPathConds, &c);
    adap.doit();
    return 0;
}
