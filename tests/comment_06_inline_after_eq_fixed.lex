// `FU.MK= // comment` followed by a real value on the next line now
// works -- the previous limitation (comment consumed as RHS) is fixed
// by making Lex state 11 / 12 silent. Tests:
//   - Cons.OutLn= // inline comment
//     "actual-value"            <-- value on next line
//   - L.Set= // comment
//     >{Mnemo="A" Cons.OutLn="row-A"}  <-- list rows on next lines
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
L.MkModeSet=1
Cons.OutLn= // inline comment 1
"first-value"
L.Set= // inline comment 2
>{Mnemo="A" Cons.OutLn="row-A-body"}
L.FindAnd={Mnemo="A"}
Cons.OutLn="done"
