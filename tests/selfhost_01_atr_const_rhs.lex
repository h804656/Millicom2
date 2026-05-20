// EqualAfter Atr row: `<FU>.<MK>=<AtrConstant>` -- the RHS is the name
// of an atr-constant (built-in like Mnemo/FUType, or `Name*-N` define).
// Before the fix this errored with "EqualAfter: value expected after '='".
// Direct excerpt-shape from CompileCC.oap:89: `Lex.UnicAtrSet=Mnemo`.
NewFU={Mnemo="Cons" FUType=FUConsNew}
MyAtr*-77
MyAtr2*-88
// Built-in atr on RHS: should print Atr's resolved value.
Cons.OutLn=Mnemo
// User-defined `Name*-N` constant on RHS.
Cons.OutLn=MyAtr
Cons.OutLn=MyAtr2
// Combined LHS chain hitting EqualAfter:
Lex.UnicAtrSet=Mnemo
Cons.OutLn="done"
