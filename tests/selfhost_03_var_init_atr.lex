// VarIniWait Atr row: `MyVar=Name` where Name is an atr-constant
// (either a built-in like `Mnemo` or a `Name*-N` define) binds MyVar
// to the atr's int value. Before the fix this errored with
// "VarIniWait: value (Const, Var, Vect or '[') expected after '='".
NewFU={Mnemo="Cons" FUType=FUConsNew}
// User-defined `Name*-N` constant on the RHS of a var init.
MagicCode*-321
LoadVar=MagicCode
Cons.OutLn=LoadVar
// Built-in atr (`Mnemo` resolves to -2) bound to a var, then read.
M=Mnemo
Cons.OutLn=M
// Multiple atr-typed inits to confirm the row doesn't break statefully.
A=Mnemo
B=Mnemo
C=Mnemo
Cons.OutLn=A
Cons.OutLn=B
Cons.OutLn=C
Cons.OutLn="done"
