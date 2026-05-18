// Models the shape of a CompileCC.oap state table: a List holding a
// grammar row whose body has multiple discrete effects. Adds three
// such rows across two Lists. Stress-tests:
//   - addUserFuMnemoRow for List type across multiple declarations
//   - LineCopyAdd={...} (FU.MK={sub-cap} form) for adding rows individually
//   - row body with a nested FU.MK={...} sub-cap (mimicking what
//     `Stack.LineCopyAdd={MkExec}` etc. compile to)
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Root" FUType=FUListNew}
NewFU={Mnemo="EqualWait" FUType=FUListNew}
NewFU={Mnemo="Marker" FUType=FUListNew}
Root.LineCopyAdd={Mnemo="Mk" Cons.OutLn="must-not-mk-1" Marker.LineCopyAdd={Sep="."} Cons.OutLn="must-not-mk-2"}
Root.LineCopyAdd={Mnemo="Mnemo" Cons.OutLn="must-not-mnemo"}
EqualWait.LineCopyAdd={Mnemo="EqSep" Marker.LineCopyAdd={Sep="="} Cons.OutLn="must-not-eq"}
Cons.OutLn="three rows registered"
