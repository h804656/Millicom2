// Phase 4c: row body can contain a nested `FU.MK={...}` sub-capsule
// alongside other effects. The structure is two levels of nesting:
//   List IC -> row body -> FU.MK sub-cap.
// All inner IPs must be suppressed at parse time (no live-dispatch).
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
NewFU={Mnemo="M" FUType=FUListNew}
Cons.OutLn="pre"
L.Set=>{Mnemo="A" Cons.OutLn="must-not-1" M.LineCopyAdd={Mnemo="nested-must-not-fire"} Cons.OutLn="must-not-2"}
Cons.OutLn="post"
