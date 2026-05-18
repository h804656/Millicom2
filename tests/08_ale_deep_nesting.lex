// Deeply nested balanced parens, left-deep & right-deep groupings.
// Tests Stack push/pop discipline of `(` markers and receiver-Stack across
// many opens/closes.
A=1
B=2
C=3
D=4
E=5
F=6
G=7
H=8
I=9
Atr=(((A+B)+(C+D))*(E-F))/((G+H)-I)
Atr=((((A+B)+C)+D)+E)
Atr=A+(B+(C+(D+E)))
Done=42
