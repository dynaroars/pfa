import z3
from adapter import Input, PathCond, Adapter

def ex():
    x, y, temp = z3.Ints("x y temp")
    inp = Input([x, y, temp], [-1, 1, 2])
    bad_pathconds = [
        PathCond([x < 2]),
        PathCond([y < 2]),
        PathCond([x >= 4]),
        PathCond([y >= 4]),
        PathCond([y <= x])
    ]
    return inp, bad_pathconds

inp, bad_pathconds = ex()
Adapter(inp, bad_pathconds).doit()