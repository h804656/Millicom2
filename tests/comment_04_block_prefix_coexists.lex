// `\**\` block prefix should still strip independently of `//` comments.
// Mix them in one file to verify they don't interfere with each other.
// NOTE: inline `//` directly after `=` is a known limitation -- the
// StrAtr that Lex state 11/12 emits for the comment arrives at the
// post-`=` state and is consumed as an RHS value. So we put the comment
// on its own line instead.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
L.MkModeSet=1
// block-prefix declaration below; following rows define the list rows
\**\L.Set=
>{Mnemo="A" Cons.OutLn="row-A-body"}
// standalone comment between definition and use
Cons.OutLn="setup-done"
L.FindAnd={Mnemo="A"} // trigger A
Cons.OutLn="final"
