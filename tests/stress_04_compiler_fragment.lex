// Mini grammar-fragment lifted in spirit from CompileCC.oap's own state
// definitions: declare a List as a state table, add multi-effect rows
// to it (each row models one transition). Tests:
//   - multi-IP row bodies with multiple effect kinds
//   - row body with a nested FU.MK={...} sub-cap
//   - Atr declaration in same file
//   - subsequent live-dispatch after all the row definitions
StateAtr*-2100
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="State" FUType=FUListNew}
NewFU={Mnemo="Marker" FUType=FUListNew}
State.Set=>{Mnemo="foo" Cons.OutLn="must-not-foo-1" Marker.LineCopyAdd={Mnemo="from-foo"} Cons.OutLn="must-not-foo-2"}
Cons.OutLn="grammar defined"
