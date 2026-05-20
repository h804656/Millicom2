// CompileCC.oap-style block declaration in a single shot:
//   - `\**\` block prefix (stripped by lexer)
//   - `MkBegRange=N` field inside a NewFU body
//   - `Mnemo!` load-by-reference postfix (stripped after the Mnemo)
// All three forms must lex without syntax error and the rest of the
// program proceeds normally.
NewFU={Mnemo="MainBus2" MkBegRange=1000 FUType=FUBusNew}
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
\**\L.Set=
>{Mnemo="A" Cons.OutLn="row-A-fired"}>{Mnemo="B" Cons.OutLn="row-B-fired"}
Cons.OutLn="block parsed"
L.MkModeSet=1
Cons.OutLn=ConsMkTable!
L.FindAnd={Mnemo="A"}
Cons.OutLn="end"
