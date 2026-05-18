// 12 vars, complex ALE expressions chained. All operands compile-time
// foldable so the result is a single int per line. Exercises the
// shunting-yard with mixed precedences and parens.
NewFU={Mnemo="Cons" FUType=FUConsNew}
A=2
B=3
C=5
D=7
E=11
F=13
G=17
H=19
I=23
J=29
K=31
L=37
Cons.OutLn=A+B*C-D
Cons.OutLn=(A+B)*(C-D)
Cons.OutLn=A*B*C*D
Cons.OutLn=A+B+C+D+E+F+G+H
Cons.OutLn=L-K+J-I+H-G
Cons.OutLn=(A+B)*(C+D)-(E+F)
Cons.OutLn=A*A+B*B+C*C
Cons.OutLn=L*K/J
Cons.OutLn=A+B*C-D+E*F-G+H*I-J
