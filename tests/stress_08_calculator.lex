// Tiny calculator: reads four ints (a, b, c, d) and computes
//   ((a + b) * c - d) / b
// All four reads are interleaved with ALU dispatches so each value
// participates directly with no compile-time folding.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Acc" FUType=FUIntAluNew}
NewFU={Mnemo="Mem" FUType=FUIntAluNew}
Cons.InputMk=Acc.Set
Cons.InputMk=Mem.Set
Mem.OutMk=Acc.Add
Cons.InputMk=Acc.Mul
Cons.InputMk=Acc.Sub
Mem.OutMk=Acc.Div
Acc.OutMk=Cons.OutLn
