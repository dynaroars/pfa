### PFA Progress
- Implemented ```adapter.py``` in C++ --> tested manually with quixbugs benchmark
- Idea: topological sorting could be used to generate order of variables to change to satisfy a program's path condition --> the idea is implemented in ```PriorityGenerator.h```

### PAC-related Progress
- Note: PAC ignores details within CFG --> a problem: PAC doesn't distinguish between the complexity of several types of loops (infinite loops, loops' bound...)
-- Recursion is also not considered in PAC (due to intra-procedure approarch)
- Thought: extract details from CFG (variable assignment, conditions...) and apply symbolic execution tools to manage the variables' changes when calculating complexity
-- Some tools might be helpful: CIVL (to generate symbolic execution tree in C++, might indirectly), SPF (generate symbolic execution tree in Java), clang-llvm (generate CFG)

### Goals
- Think of better approarch to calculate complexity (could resolve the mentioned problems)
- "Think in background": How to statically edit source code to avoid bugs (infinite loops & arrays)