#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <z3++.h>

using namespace std;
using namespace z3;

/*
Manages methods to help C++ with Python features
*/
class AdapterHelper {
public:
    /**
     * Create possible k-of-n combinations of a range [0, n)
     * Each combination is stored in a set<int>
     */
    static vector<set<int>> combinations(int n, int k) {
        cout << "!!! Combination: " << n << ' ' << k << endl;
        vector<set<int>> ret;
        string bit(k, 1);
        bit.resize(n, 0);
        do {
            set<int> curPermu;
            cout << "!!! ";
            for(int i = 0; i < n; i++)
                if (bit[i]) {
                    curPermu.insert(i);
                    cout << i << ' ';
                }
            ret.push_back(curPermu);
            cout << endl;
        } while (prev_permutation(bit.begin(), bit.end()));
        return ret;
    }
};

/*
Manages bad conditions leading to program crash
*/
class PathCond {
private:
    context* c;
    expr_vector *conjs;

public:
    PathCond() { this->conjs = NULL; }

    PathCond(expr_vector& conjs, context* c) {
        if (c == NULL) 
            cout << "Context must not be NULL" << endl;
        if (conjs.size() == 0) {
            cout << "Input for PathCond constructor is empty" << endl;
            exit(0);
        }
        this->c = c;
        this->conjs = new expr_vector(conjs);
    }
    
    expr constraint() {
        return (conjs->size() > 1 ? mk_and(*conjs) : (*conjs)[0]);
    }

    PathCond get_new(int idx = -1) {
        expr_vector newConjs = expr_vector(*c);
        if (idx == -1)
            idx = (int)conjs->size() - 1;
        for(int i = 0; i < conjs->size(); i++) {
            if (i == idx)
                newConjs.push_back(!((*conjs)[i]));
            else 
                newConjs.push_back((*conjs)[i]);
        }
        return PathCond(newConjs, c);
    }

    expr_vector* getConjs() { return this->conjs; }

    friend ostream& operator<< (ostream& out, const PathCond& x) {
        for(int i = 0; i < (int)(x.conjs->size()) - 1; i++)
            out << (*(x.conjs))[i] << " && ";
        out << x.conjs->back();
        return out;
    }
};

/* 
Manages variables & their values 
*/
class Input {
private:
    context* c;
    expr_vector *zvars, *zvals;

public:
    Input() {}

    Input(expr_vector& zvars, expr_vector& zvals, context* c) {
        if (c == NULL)
            cout << "Context must not be null" << endl;
        if (zvars.size() != zvals.size())
            cout << "Number of variables and values are not equal" << endl;
        this->c = c;
        this->zvars = new expr_vector(zvars);
        this->zvals = new expr_vector(zvals);
    }

    expr constraint(set<int> idxs = set<int>()) {
        expr_vector conjs = expr_vector(*c);
        for(int i = 0; i < zvars->size(); i++) 
            if (!idxs.count(i))
                conjs.push_back((*zvars)[i] == (*zvals)[i]);
        if (conjs.size() == 0)
            cout << "Set of input constraints generated by Input::constraint() is empty" << endl;
        return (conjs.size() > 1 ? mk_and(conjs) : conjs[0]);
    }

    expr create_constraints_k(int k) {
        expr_vector disjs = expr_vector(*c);
        vector<set<int>> combin = (AdapterHelper::combinations(zvars->size(), k));
        for(set<int> idxs : combin)
            disjs.push_back(constraint(idxs));
        if (disjs.size() == 0)
            cout << "Data generated by Input::create_constraints_k(k) is empty" << endl;
        return (disjs.size() > 1 ? mk_or(disjs) : disjs[0]);
    }

    Input* gen(expr_vector hardConstrs, expr softConstr) {
        cout << "hard: " << hardConstrs << endl;
        cout << "soft: " << softConstr << endl;
        cout << "end of constraint" << endl << endl;

        solver sol = solver(*c);

        hardConstrs.push_back(softConstr);
        expr constrs = mk_and(hardConstrs);
        sol.add(constrs);
        if (sol.check() != sat)
            return NULL;
        
        model m = sol.get_model();
        expr_vector zvars = expr_vector(*c);
        expr_vector zvals = expr_vector(*c);
        for(int i = 0; i < m.size(); i++) {
            zvars.push_back(m[i]());
            zvals.push_back(m.get_const_interp(m[i]));
        }
        return new Input(zvars, zvals, c);
    }

    int check(vector<PathCond> badPathConds) {
        solver sol = solver(*c);
        for(int i = 0; i < badPathConds.size(); i++) {
            cout << "checking" << endl;
            cout << badPathConds[i] << endl;
            // cout << constraint() << endl;
            // cout << badPathConds[i].constraint() << endl;
            expr f = !(implies(constraint(), badPathConds[i].constraint()));
            cout << constraint() << endl;
            sol.reset();
            sol.add(f);
            cout << sol << endl;
            check_result isStat = sol.check();
            cout << isStat << endl;
            if (isStat == unsat) {
                cout << i << endl;
                return i;
            }
        }
        return -1; // replaces None
    }

    expr_vector* getZVars() { return this->zvars; }
    
    expr_vector* getZVals() { return this->zvals; }

    friend ostream& operator<< (ostream& out, const Input& x) {
        for(int i = 0; i < (x.zvars)->size() - 1; i++) 
            out << (*x.zvars)[i] << "=" << (*x.zvals)[i] << ", ";
        out << (x.zvars)->back() << "=" << (x.zvals)->back();
        return out;
    }
};

/*
Manages methods adjusting inputs to avoid program crash
*/
class Adapter {
private:
    context* c;
    Input inp;
    vector<PathCond> badPathConds;

public:
    Adapter(Input& inp, vector<PathCond>& badPathConds, context* c) {
        if (c == NULL)
            cout << "Context must not be NULL" << endl;
        this->c = c;
        this->inp = inp;
        this->badPathConds = badPathConds;
    }

    void doit() {
        vector<PathCond> remains = badPathConds;
        pair<Input*, expr> p = pair<Input*, expr>(NULL, expr(*c)); 
        p.first = new Input(*(this->inp.getZVars()), *(this->inp.getZVals()), c);
        int iteration = 0;
        expr_vector satConstrs = expr_vector(*c);
        cout << remains[0] << endl;
        while (++iteration) {
            cout << "** iteration " << iteration << " **" << endl;
            cout << "checking inp: " << *p.first << " against " << remains.size() << " bad conds" << endl;
            int badCondIdx = p.first->check(remains);
            if (badCondIdx == -1) {
                cout << "avoided all bad path conds." << endl << "Done!" << endl;
                break;
            }
            PathCond badCond = remains[badCondIdx];
            cout << "found one bad cond: " << badCond << endl;
            p = generate_new_input(badCond, satConstrs);
            satConstrs.push_back(p.second);
            cout << "Constraint" << endl << satConstrs << endl;
            if (p.first == NULL) {
                cout << "cannot modify input to avoid bad path" << endl;
                break;
            }
            cout << "found inp: " << *p.first << endl;
            remains.erase(remains.begin() + badCondIdx);    // Double-check where badCondIdx starts?
        }
        if (p.first != NULL)
            cout << "resulting inp: " << (*p.first) << endl; 
    }

    pair<Input*, expr> generate_new_input(PathCond badPathCond, expr_vector satConstrs) {
        expr hardConstr = badPathCond.get_new().constraint();
        expr_vector hardConstrs = expr_vector(satConstrs);
        hardConstrs.push_back(hardConstr);
        int oriInpLen = (this->inp.getZVars())->size();
        for(int k = 1; k < oriInpLen; k++) {
            cout << "attempting inp that is " << 100.0 * (oriInpLen - k) / oriInpLen;
            cout << " (" << oriInpLen - k << "/" << oriInpLen << ") similar to orig" << endl;
            expr softConstr = this->inp.create_constraints_k(k);
            Input* inp = (this->inp.gen(hardConstrs, softConstr));
            cout << *inp << endl;
            if (inp) 
                return pair<Input*, expr>(inp, hardConstr);
        }
        return pair<Input*, expr>(NULL, hardConstr);
    }
};