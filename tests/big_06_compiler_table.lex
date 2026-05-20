// Larger "compiler table"-shaped fixture: declare a State List with
// several keyword rows, each with multi-statement bodies that include
// a literal print AND a counter increment. After all keywords have
// been triggered, ask the counter ALU how many fired and print a final
// status line. Tests the integration of:
//   - multi-row LHS=>{r1}>{r2}>{r3} in a user-declared List
//   - L.MkModeSet=1 + L.FindAnd fires multi-IP row body
//   - cross-FU transfer (Counter.Add=1 inside row body)
//   - final compile-time print after runtime updates
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="State" FUType=FUListNew}
NewFU={Mnemo="Counter" FUType=FUIntAluNew}
State.MkModeSet=1
Counter.Set=0
State.Set=>{Mnemo="OPEN"  Cons.OutLn="state-open"  Counter.Add=1}>{Mnemo="MOVE" Cons.OutLn="state-move" Counter.Add=1}>{Mnemo="CLOSE" Cons.OutLn="state-close" Counter.Add=1}
Cons.OutLn="-- run --"
State.FindAnd={Mnemo="OPEN"}
State.FindAnd={Mnemo="MOVE"}
State.FindAnd={Mnemo="MOVE"}
State.FindAnd={Mnemo="CLOSE"}
Counter.OutMk=Cons.OutLn
Cons.OutLn="-- end --"
