// ALE: chained binary ops, paren-first, paren-mid, all 5 operators,
// paren-wrap single operand, double-paren wrap, unary +/- at ALE entry.
// Verifies ALE <-> ALEAfter ping-pong through long chains and nested groups
// (Stack of `(` markers + receiver-Stack discipline).
A=1
B=2
C=3
D=4
E=5
F=6
G=7
H=8
I=9
J=10
Atr=A+B-C*D/E+F-G*H+I-J
Atr=A+B*C
Atr=(A+B)*C
Atr=A-(B-C)
Atr=A+B*C-D/E%F
Atr=(A)
Atr=((A))
Atr=(-A)
Atr=(+A)
Atr=(A+B)+(A+B)
Done=42
