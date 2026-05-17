// Right-deep paren nesting: A+(B+(C+(D+E)))
//   Five operands, four binary ops, three nested right-leaning paren groups.
//   Inverse of test 21: Stack pushes 3 markers, all 3 pop after the innermost
//   operand. Different ALE/ALEAfter cycle pattern than left-deep.
A=1
B=2
C=3
D=4
E=5
Atr=A+(B+(C+(D+E)))
Done=42
