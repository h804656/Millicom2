// 4 levels of FU.MK={...} nesting in a single statement. Each `{` opens
// a sub-cap, each `}` closes it. Body IPs must all be SUPPRESSED at
// parse-time (SubCapDepth>0). Outer Cons.OutLn outside the construct
// must still live-dispatch.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
NewFU={Mnemo="M" FUType=FUListNew}
NewFU={Mnemo="N" FUType=FUListNew}
Cons.OutLn="pre"
L.LineCopyAdd={Mnemo="lvl1" Cons.OutLn="must-not-fire-lvl1" M.LineCopyAdd={Mnemo="lvl2" Cons.OutLn="must-not-fire-lvl2" N.LineCopyAdd={Mnemo="lvl3" Cons.OutLn="must-not-fire-lvl3"}}}
Cons.OutLn="post"
