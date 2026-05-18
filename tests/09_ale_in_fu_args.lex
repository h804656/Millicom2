// ALE as the RHS of an FU.MK assignment. The LHS first walks through
// FUAfter . / MkWait / AtrBrack / EqualWait before reaching EqualAfter where
// the ALE push fires.
A=1
B=2
C=3
D=4
Cons.OutLn="hello"
Cons.OutLn=A+B
Cons.OutLn=A+B*C-D
Cons.OutLn=(A+B)*C
Done=42
