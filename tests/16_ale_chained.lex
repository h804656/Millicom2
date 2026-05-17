// 10-operand binary chain. 10 var decls, 9 binary operators, no parens.
// Verifies the ALE <-> ALEAfter ping-pong survives a deeply repeated cycle
// (the previously-stuck ALE state used to swallow everything after the
// first push -- this would have hung forever).
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
Done=42
