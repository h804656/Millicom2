// Multi-FU method dispatch + Var-as-arg regression coverage.
// Exercises every FU.MK token-type path:
//   Root FU/FUType, FUAfter ., MkWait MkForFU, AtrBrack else->EqualWait
//   EqualWait =, EqualAfter Const/Var/Vect, EqVar else -> Root
//   EqualAfter FU -> FUAfter2 . -> MkWait2 MkForFU -> LoadAfter else->Root
//   Var dispatched into MnemoTable then re-emitted as Var atr in three contexts:
//     VarIniWait Var (First=A, Second=B)
//     Nested VarIniWait Var inside { } subgraphs
//     EqualAfter Const/Var/Vect (Cons.OutLn=A as Var arg)
A=1
B=2
Cons.OutLn="start"
Cons.OutLn=A
Cons.Out=B
Atr=Bus.MakeFU
First=A
Second=B
Outer{Sub=A Mid{Deep=B Leaf=A}}
Atr={P=A Q=B}
Cons.OutLn="end"
Done=42
