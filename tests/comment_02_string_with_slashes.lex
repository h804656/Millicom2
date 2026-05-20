// `//` inside a string literal is content, not a comment marker. Lex state 4
// (string accumulation) handles `"..."` separately from state 5/11. So a
// literal string containing `//` must print verbatim.
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="path/to//file"
Cons.OutLn="//ratio"
Cons.OutLn="trailing//"
Cons.OutLn="x"
// Then a real comment after, also fine.
Cons.OutLn="done"
