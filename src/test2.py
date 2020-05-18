import z3
import math
import sympy as sp
from adapter import Input, PathCond, Adapter


def ex5b():
    a1, a2 = z3.Ints('a1 a2')
    inp = Input([a1], [5])
    bad_pathconds = [
        PathCond([a1 < 1]),
        PathCond([a1 == 5])
        # PathCond([a2 < 1]),
        # PathCond([a2 == 5]),
        # PathCond([a3 < 1]),
        # PathCond([a3 == 5])
    ]
    return inp, bad_pathconds


inp, bad_pathconds = ex5b()
Adapter(inp, bad_pathconds).doit()
