import z3
from adapter import Input, PathCond, Adapter

# EXAMPLE 1: Guolong's example

# def f(i, j, k):
#     if(0 < i < 10):
#         if(i < 5):
#             o = 5 / (i - 2)   # err1
#         else:
#             o = 6 / (i - 3)   # err2
#     else:
#         o = 2*i


def ex1a():
    # path condition leading to err1: 0 < i < 10 & i < 5 & i = 2
    myi, myj, myk = z3.Ints('i j k')
    inp = Input([myi, myj, myk], [2, 100, 1000])
    bad_pathconds = [PathCond([0 < myi, myi < 10, myi < 5, myi == 2])]

    return inp, bad_pathconds


# inp, bad_pathconds = ex1a()
# Adapter(inp, bad_pathconds).doit()


def ex1b():
    """
    violated path conditions to err2: 0 < i < 10 & i >= 5 & i = 3
    can be fixed by changing i to #3
    """
    myi, myj, myk = z3.Ints('i j k')
    inp = Input([myi, myj, myk], [3, 100, 1000])
    bad_pathconds = [PathCond([0 < myi, myi < 10, myi < 5, myi == 3])]

    return inp, bad_pathconds


# inp, bad_pathconds = ex1b()
# Adapter(inp, bad_pathconds).doit()


def ex1c():
    """
    violated path conditions to err1 + a fake one:
    - 0 < i < 10 & i >= 5 & i = 3
    - j >= 0
    can be fixed by changing i to !3 and j to < 0
    """
    myi, myj, myk = z3.Ints('i j k')
    inp = Input([myi, myj, myk], [3, 100, 1000])
    bad_pathconds = [
        PathCond([0 < myi, myi < 10, myi < 5, myi == 3]),
        PathCond([myj >= 0])
    ]

    return inp, bad_pathconds


inp, bad_pathconds = ex1c()
# Adapter(inp, bad_pathconds).doit()


# EXAMPLE 2: from PFA proposal

# int vandi ( char* str )
# {
# char id [3];
# assert ( *str != '_' ) ;
# if ( isvalid(str)) {
#   int i = 0;
#   while ( *str != null )
#     id [ i++] = *str++;
#     return intern ( id ) ;
# } else
#   // default behavior
# return 1;
# }

def ex2a():
    a0, a1, a2, a3, a4 = z3.Ints('a0 a1 a2 a3 a4')
    inp = Input([a0, a1, a2, a3, a4], [0, 1, 2, 3, -1])
    bad_pathconds = [
        PathCond([a0 != -1, a1 != -1, a2 != -1, a3 != -1])
    ]
    return inp, bad_pathconds


inp, bad_pathconds = ex2a()
# Adapter(inp, bad_pathconds).doit()

# EXAMPLE 3: Didier's

# $input int y[5];
# int * derivative(int y[], int size){
#     int h = 1;
#     int diff[size];
#     diff[0]=0;
#     for (int i = 1; i < size-1; i++) {
#         diff[i] = 2*h/(-y[i-1]+y[i+1]); //possible div by zero
#         $pathCondition();
#     }
#     diff[size-1]=0;
#     return diff;
# }
# int main() {
#   derivative(y,5);
#   return 0;
# }


def ex3a():
    y0, y1, y2, y3, y4, y5 = z3.Ints('y0 y1 y2 y3 y4 y5')
    inp = Input([y0, y1, y2, y3, y4, y5], [0, 1, 2, 5, 4, 5])
    bad_pathconds = [
        PathCond([y0 == y2]),
        PathCond([y0 != y2, y1 == y3]),
        PathCond([y0 != y2, y1 != y3, y2 == y4]),
        PathCond([y0 != y2, y1 != y3, y2 != y4, y3 == y5])
    ]

    return inp, bad_pathconds


inp, bad_pathconds = ex3a()
Adapter(inp, bad_pathconds).doit()

# MISCS

# a0 = z3.Int('a0')
# a1 = z3.Int('a1')
# a2 = z3.Int('a2')
# a3 = z3.Int('a3')
# a4 = z3.Int('a4')
# inp = Input([a0, a1, a2, a3], [0, 1, 2, 3])
# print(inp.create_constraint(idxs={5}))

# inp.create_constraints_k(2)
