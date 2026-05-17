// Left-deep paren nesting: ((((A+B)+C)+D)+E)
//   Five operands, four binary ops, four nested left-leaning paren groups.
//   The Stack accumulates 4 `(` markers, then unwinds them one by one.
//   Verifies the Stack discipline under deeply left-recursive grouping.
A=1
B=2
C=3
D=4
E=5
Atr=((((A+B)+C)+D)+E)
Done=42
