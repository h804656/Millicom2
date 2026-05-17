// ALE expression as the RHS of FU.MK assignment. The dispatch chain is much
// longer than `Atr=expr` because the LHS walks through FUAfter . / MkWait /
// AtrBrack / EqualWait before reaching EqualAfter, where the ALE push fires.
//   Cons.OutLn="hello"        plain string arg (Const branch)
//   Cons.OutLn=A+B            ALE binary chain
//   Cons.OutLn=A+B*C-D        three-operator ALE
//   Cons.OutLn=(A+B)*C        paren-first ALE in FU arg
A=1
B=2
C=3
D=4
Cons.OutLn="hello"
Cons.OutLn=A+B
Cons.OutLn=A+B*C-D
Cons.OutLn=(A+B)*C
Done=42
