// Long real-OAP-shaped program touching every major construct:
//   NewFU declaration, Var setup, Atr offset, Vector with Var elements,
//   Brace subgraph, Mnemo(Atr) paren chain, ALE expressions,
//   FU.MK with string and ALE arg, EqualAfter FU full chain.
NewFU={Mnemo="L" FUType=FUListNew}
A=1
B=2
C=3
D=4
Offset*-7
V#A,B,C
Atr={X=1 Y=2}
NewObj(Atr)
Atr=A+B*C-D
Atr=(A+B)*(C-D)
Cons.OutLn="run"
Cons.OutLn=A+B*C
Atr=Bus.MakeFU
Atr={P=A Q=B}
Cons.OutLn="hi"
Done=42
