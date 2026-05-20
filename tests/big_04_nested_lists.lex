// Recursive dispatch chain across 3 user-declared Lists. OUTER row body
// calls L2.FindAnd, which fires the MIDDLE body, which calls L3.FindAnd,
// which fires the INNER body. Each level then prints its own "fired"
// line, so the output reveals the natural unwind order:
//   inner fires first (deepest), then middle, then outer.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L1" FUType=FUListNew}
NewFU={Mnemo="L2" FUType=FUListNew}
NewFU={Mnemo="L3" FUType=FUListNew}
L1.MkModeSet=1
L2.MkModeSet=1
L3.MkModeSet=1
L3.Set=>{Mnemo="INNER"  Cons.OutLn="inner-fired"}
L2.Set=>{Mnemo="MIDDLE" L3.FindAnd={Mnemo="INNER"}  Cons.OutLn="middle-fired"}
L1.Set=>{Mnemo="OUTER"  L2.FindAnd={Mnemo="MIDDLE"} Cons.OutLn="outer-fired"}
Cons.OutLn="-- begin --"
L1.FindAnd={Mnemo="OUTER"}
Cons.OutLn="-- end --"
