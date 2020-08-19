from z3 import *

a = Int('a')
b = Int('b')
print((a > b).children())