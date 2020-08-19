/**
 * Generate a list of variables ordered by their priorites.
 * Priority definition: A is before B in list --> A should be edited before B (if necessary)
 * 
 * INPUT: an expr_vector containing valid conditions
 *      Assume a valid condition is given as (A <= B) or similar
 * OUTPUT: a vector of variables and their information (range...)
 */

#include <stdexcept>
#include <z3++.h>
#include <algorithm>
#include <map>
#include <vector>
#include <queue>

using namespace std;
using namespace z3;

class Limit {
public:
    int val;
    bool hasVal;

    Limit() { hasVal = false; }
    Limit(int val) { 
        this->val = val; 
        this->hasVal = true; 
    }
};

class VarInfo {
private:
    context* c;
public:
    expr* var;
    string varStr;
    Limit leftLim, rightLim;

    VarInfo(expr& var, context* c) {
        if (c == NULL)
            throw invalid_argument("Context cannot be passed as NULL");
        this->c = c;
        this->var = new expr(var);
        this->varStr = var.to_string();
    }

    bool operator< (const VarInfo& x) const { return (varStr < x.varStr); }
    bool operator== (const VarInfo& x) const { return (varStr == x.varStr); }
};

class EdgeTo {
public:
    int des;
    bool canEqual;
    EdgeTo(int des, bool canEqual) { this->des = des; this->canEqual = canEqual; }
};

class PriorityGenerator {
private:
    context* c;
    expr_vector* validConds;
    vector<VarInfo> vars;
    vector<vector<EdgeTo>> graph;
    int* inDeg;

public:
    PriorityGenerator(expr_vector& validConds, context* c) {
        if (c == NULL) 
            throw invalid_argument("Context cannot be passed as NULL");
        this->c = c;
        this->validConds = new expr_vector(*c);
        for(int i = 0; i < validConds.size(); i++)
            this->validConds->push_back(validConds[i]);
        processValidConds();
    }

    // TODO: improve checking if the variable exists
    int findVar(VarInfo* var, int* varNum) {
        for(int i = 0; i < vars.size(); i++)
            if (vars[i] == *var)
                return i;
        // cout << var->varStr << endl;
        vars.push_back(*var);
        graph.push_back(vector<EdgeTo>());
        return (*varNum)++;
    }

    void processValidConds() {
        int varNum = 0;
        for(int i = 0; i < validConds->size(); i++) {
            expr cond = (*validConds)[i];
            expr leftSide = cond.arg(0), rightSide = cond.arg(1);
            if (cond.decl().name().str()[0] == '>')
                swap(leftSide, rightSide);
            bool canEqual = cond.decl().name().str().size() > 1;
            int idLeft = -1, idRight = -1;
            if (!leftSide.is_numeral()) {
                VarInfo var = VarInfo(leftSide, c);
                idLeft = findVar(&var, &varNum);
                // cout << "Left" << var.varStr << " " << idLeft << ' ' << varNum << endl;
            }
            if (!rightSide.is_numeral()) {
                VarInfo var = VarInfo(rightSide, c);
                idRight = findVar(&var, &varNum);
                // cout << "Right" << var.varStr << " " << idRight << ' ' << varNum << endl;
            }
            if (idLeft == -1 && idRight != -1)
                vars[idRight].leftLim = Limit(leftSide.get_numeral_int() + !canEqual);
            if (idRight == -1 && idLeft != -1)
                vars[idLeft].rightLim = Limit(rightSide.get_numeral_int() - !canEqual);
            if (idLeft != -1 && idRight != -1)
                graph[idLeft].push_back(EdgeTo(idRight, canEqual));
        }
        inDeg = new int[varNum];
        fill(inDeg, inDeg + varNum, 0);
        for(int i = 0; i < varNum; i++) 
            for(int j = 0; j < graph[i].size(); j++)
                ++inDeg[graph[i][j].des];
    }

    void printGraph() {
        cout << "Graph: " << endl;
        for(int i = 0; i < vars.size(); i++) 
            for(int j = 0; j < graph[i].size(); j++) 
                cout << "   " << vars[i].varStr << (graph[i][j].canEqual ? " <= " : " < ") << vars[graph[i][j].des].varStr << endl;
    }

    vector<VarInfo> generatePriorityList() {
        // TODO: check SCC --> determine if we can edit variables
        vector<VarInfo> ret;
        queue<int> que;
        for(int i = 0; i < vars.size(); i++)
            if (inDeg[i] == 0)
                que.push(i);
        while (!que.empty()) {
            int u = que.front(); que.pop();
            ret.push_back(vars[u]);
            for(EdgeTo v : graph[u])
                if (--inDeg[v.des] == 0)
                    que.push(v.des);
        }
        if (ret.size() != vars.size()) {
            cout << "The graph may have circuit! Check the dependencies of variables again";
            throw invalid_argument("The graph may have circuit! Check the dependencies of variables again");
        }
        return ret;
    }
};