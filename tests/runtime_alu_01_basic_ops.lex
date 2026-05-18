// Runtime IntAlu FU operations dispatched live during compile.
// IntAlu's accumulator is stateful across statements, so ALE compile-time
// shunting can't fold these into a single literal. The result is computed
// by the actual IntAlu instance at runtime, then surfaced via OutMk.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Alu" FUType=FUIntAluNew}
Alu.Set=10
Alu.Add=5
Alu.OutMk=Cons.OutLn
Alu.Mul=2
Alu.OutMk=Cons.OutLn
Alu.Sub=4
Alu.OutMk=Cons.OutLn
Alu.Div=2
Alu.OutMk=Cons.OutLn
Alu.Mod=3
Alu.OutMk=Cons.OutLn
