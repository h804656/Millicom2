// Deeply nested balanced parens: 9 operands, mixed groupings.
//   (((A+B)+(C+D))*(E-F))/((G+H)-I)
// Verifies the Stack-of-parens tracking and the ALE/ALEAfter cycle through
// many opens and closes without leaking entries.
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
Done=42
