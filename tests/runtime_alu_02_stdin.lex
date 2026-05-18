// Cons.InputMk reads stdin and dispatches the parsed value via its Load MK.
// This proves runtime values (not known at compile time) flow into IntAlu.
// Stdin (from runtime_alu_02_stdin.stdin): 10, 5, 2
// Expected: (10 + 5) * 2 = 30
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Alu" FUType=FUIntAluNew}
Cons.InputMk=Alu.Set
Cons.InputMk=Alu.Add
Cons.InputMk=Alu.Mul
Alu.OutMk=Cons.OutLn
