// Verifies the live-dispatch fix: after `L.Set=>{...}` at parse-time,
// L must actually hold the row content (not empty). We probe with
// `L.FullExec={...}` (run if list non-empty) and `L.EmptyProgExec={...}`
// (run if list empty). Before the fix, L.Set was live-dispatched with
// Load=0 so L stayed empty and "L-empty" would fire instead.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
Cons.OutLn="before-set"
L.Set=>{Mnemo="A" Cons.OutLn="must-not-row-body"}
Cons.OutLn="after-set"
L.FullExec={Cons.OutLn="L-non-empty"}
L.EmptyProgExec={Cons.OutLn="L-was-empty"}
Cons.OutLn="done"
