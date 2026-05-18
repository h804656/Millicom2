// Phase 4c: multi-IP `>{Mnemo="A" effect1 effect2}` row body emits as a
// single sub-capsule attached to the row's LineAtr (-6). Verified here by:
//   - the parser accepts the multi-effect body (no syntax error)
//   - the row body IPs are SUPPRESSED at parse time (SubCapDepth>0 gates
//     live-dispatch) — they belong to the deferred row body, not the outer
//     program flow. So "must-not-fire-*" must not appear in stdout.
//   - statements outside the row continue to live-dispatch normally.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
Cons.OutLn="before-row"
L.Set=>{Mnemo="A" Cons.OutLn="must-not-fire-1" Cons.OutLn="must-not-fire-2"}
Cons.OutLn="after-row"
