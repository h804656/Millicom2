// Multi-row list: `L.Set=>{r1}>{r2}` must register BOTH rows in L.
// After both rows are added, L.FullExec should fire (L non-empty), and
// L.EmptyProgExec should NOT. The row bodies themselves stay deferred
// (no live-dispatch) because SubCapDepth>0 while they're parsed.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
Cons.OutLn="before"
L.Set=>{Mnemo="A" Cons.OutLn="must-not-row-A"}>{Mnemo="B" Cons.OutLn="must-not-row-B"}
Cons.OutLn="after"
L.FullExec={Cons.OutLn="L-has-rows"}
L.EmptyProgExec={Cons.OutLn="L-empty"}
Cons.OutLn="done"
