import z3
import itertools
import pdb

DBG = pdb.set_trace


class PathCond:
    def __init__(self, conjs):
        assert isinstance(conjs, list), conjs
        assert conjs, conjs

        self.conjs = conjs

    def __str__(self):
        return ' && '.join(map(str, self.conjs))

    @property
    def constraint(self):
        return z3.And(self.conjs) if len(self.conjs) >= 2 else self.conjs[0]

    def get_new(self, idx=None):
        if idx is None:
            idx = -1

        conjs = list(self.conjs)
        conjs[idx] = z3.Not(conjs[-1])
        return self.__class__(conjs)


class Input:
    def __init__(self, zvars, zvals):
        assert len(zvars) == len(zvals)
        self.zvars = zvars
        self.zvals = zvals

    def __str__(self):
        return ', '.join("{}={}".format(x, y) for x, y in zip(self.zvars, self.zvals))

    @property
    def constraint(self):
        return self.create_constraint()

    def create_constraint(self, idxs=set()):
        """
        create an input constraint but ignore those in idxs
        """
        conjs = [var == val for i, (var, val) in
                 enumerate(zip(self.zvars, self.zvals))
                 if i not in idxs]
        assert conjs
        return z3.And(conjs) if len(conjs) >= 2 else conjs[0]

    def create_constraints_k(self, k):
        #assert k >= 1, k
        disjs = []
        for idxs in itertools.combinations(range(len(self.zvars)), k):
            disjs.append(self.create_constraint(idxs))

        assert disjs
        return z3.Or(disjs) if len(disjs) >= 2 else disjs[0]

    def gen(self, hard_constrs, soft_constr):
        print('hard: {}'.format(hard_constrs))
        print('soft: {}'.format(soft_constr))
        solver = z3.Solver()

        constrs = hard_constrs + [soft_constr]
        constrs = z3.And(constrs)
        print(constrs)
        solver.add(constrs)
        stat = solver.check()
        if stat != z3.sat:
            return None
        m = solver.model()
        zvars = []
        zvals = []
        for v in m:
            zvars.append(v)
            zvals.append(m[v])
        inp = self.__class__(zvars, zvals)
        return inp

    def check(self, bad_pathconds):
        """
        return an index to the hard constraint that input_constraint violates
        """
        assert isinstance(bad_pathconds, list), bad_pathconds
        assert all(isinstance(pc, PathCond)
                   for pc in bad_pathconds), bad_pathcond

        solver = z3.Solver()
        for i, bpc in enumerate(bad_pathconds):
            f = z3.Not(z3.Implies(self.constraint, bpc.constraint))
            solver.reset()
            solver.add(f)
            stat = solver.check(f)
            if stat == z3.unsat:
                return i

        return None


class Adapter:
    def __init__(self, inp, bad_pathconds):
        assert isinstance(inp, Input), inp
        assert isinstance(bad_pathconds, list) and bad_pathconds, bad_pathconds
        assert all(isinstance(pc, PathCond)
                   for pc in bad_pathconds), bad_pathconds

        self.inp = inp
        self.bad_pathconds = bad_pathconds

    def doit(self):
        """
        Given
        - an input constraint: the original input, e.g., x==3,  or a_0==1 & a_1==3
        - hard constraints: a set of conjunctive formulas that lead to failure
        Adapter new input such that
        - passes the hard constraints
        - as similar to the input constraint as possible
        """
        remains = list(self.bad_pathconds)
        inp = self.inp
        iteration = 0
        sat_constrs = []
        while True:
            iteration += 1
            print("** iteration {} **".format(iteration))
            print("checking inp: {} against {} bad conds".format(inp, len(remains)))
            bad_cond_idx = inp.check(remains)
            if bad_cond_idx is None:  # pass all remains
                print("avoided all bad path conds.\nDone!")
                break
            bad_cond = remains[bad_cond_idx]
            print("found one bad cond: {}".format(bad_cond))
            inp, sat_constr = self.generate_new_input(bad_cond, sat_constrs)
            sat_constrs.append(sat_constr)
            if inp is None:
                print("cannot modify input to avoid bad path")
                break
            print("found inp: {}".format(inp))
            remains = remains[:bad_cond_idx] + remains[bad_cond_idx+1:]

        if inp is not None:
            print("resulting inp: {}".format(inp))

    def generate_new_input(self, bad_pathcond, sat_constrs):
        assert isinstance(bad_pathcond, PathCond), bad_pathcond
        assert isinstance(sat_constrs, list), sat_constrs
        assert all(z3.is_expr(c) for c in sat_constrs), sat_constrs

        hard_constr = bad_pathcond.get_new().constraint
        hard_constrs = sat_constrs + [hard_constr]
        print("testing hard: {}".format(hard_constrs))
        print("testing sat_constr: {}".format(sat_constrs))
        orig_inp_len = len(self.inp.zvars)
        for k in range(1, orig_inp_len):
            print("attempting inp that is {} ({}/{}) similar to orig".format(
                (orig_inp_len - k) * 100. / float(orig_inp_len),
                orig_inp_len - k, orig_inp_len))
            soft_constr = self.inp.create_constraints_k(k)
            inp = self.inp.gen(hard_constrs, soft_constr)
            if inp:
                return inp, hard_constr

        return None, hard_constr
