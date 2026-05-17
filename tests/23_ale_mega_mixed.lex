// Mega-mix: a long real-OAP-shaped program that exercises every major
// parser construct alongside multiple ALE expressions.
//   NewFU declaration
//   Var setup (4 vars)
//   Atr offset declaration
//   Vector literal with Var elements
//   Two ALE expressions (chain + paren-first)
//   FU.MK string literal call
//   FU.MK with ALE arg
//   Atr=Bus.MakeFU full chain
//   Closing var-init
NewFU={Mnemo="L" FUType=FUListNew}
A=1
B=2
C=3
D=4
Offset*-7
V#A,B,C
Atr=A+B*C-D
Atr=(A+B)*(C-D)
Cons.OutLn="run"
Cons.OutLn=A+B*C
Atr=Bus.MakeFU
Done=42
