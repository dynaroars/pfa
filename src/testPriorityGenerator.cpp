#include <iostream>
#include <z3++.h>
#include "PriorityGenerator.h"

using namespace std;
using namespace z3;

int main() {
    context con;
    expr_vector validConds = expr_vector(con);
    expr a = con.int_const("a");
    expr b = con.int_const("b");
    expr c = con.int_const("c");
    expr d = con.int_const("d");
    validConds.push_back((con.int_val(1) <= a));
    validConds.push_back(a < b);
    validConds.push_back(d > con.int_val(3));
    validConds.push_back(d <= con.int_val(5));
    validConds.push_back(b <= c);
    validConds.push_back(con.int_val(4) >= b);
    validConds.push_back(c < con.int_val(10));
    PriorityGenerator priorityGen = PriorityGenerator(validConds, &con);
    priorityGen.printGraph();
    vector<VarInfo> priList = priorityGen.generatePriorityList();
    cout << "Edit order: ";
    for(VarInfo x : priList)
        cout << x.varStr << " "; cout << endl;
    for(VarInfo x : priList) {
        cout << "Variable: " << x.varStr << endl;
        cout << "--- Left bound: " << (x.leftLim.hasVal ? to_string(x.leftLim.val) : "NULL") << endl;
        cout << "--- Right bound: " << (x.rightLim.hasVal ? to_string(x.rightLim.val) : "NULL") << endl;
    }
    return 0;
}