// `\**\Name.Set=` block prefix (CompileCC.oap style) is lexed as a
// no-op; the rest reads as `Name.Set=` and the multi-row body that
// follows registers normally in the user-declared List.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
\**\L.Set=
>{Mnemo="A" Cons.OutLn="row-a"}>{Mnemo="B" Cons.OutLn="row-b"}
Cons.OutLn="block parsed"
L.MkModeSet=1
L.FindAnd={Mnemo="B"}
Cons.OutLn="done"
