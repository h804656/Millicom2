// Realistic mixed program: declares FUs, defines a state table, runs
// compile-time math, runs runtime ALU on stdin, prints a result.
//   - 1 Cons, 3 ALUs, 1 List
//   - Compile-time vars used as ALU operands
//   - Atr declaration
//   - Multi-IP row body
//   - Cons.InputMk feeds ALU
//   - Multiple cross-FU transfers
//   - Final compile-time print of ALE expression after runtime work
PhaseAtr*-2200
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Sum" FUType=FUIntAluNew}
NewFU={Mnemo="Tmp" FUType=FUIntAluNew}
NewFU={Mnemo="Scale" FUType=FUIntAluNew}
NewFU={Mnemo="States" FUType=FUListNew}
Base=10
Multiplier=3
Adjust=2
Scale.Set=Multiplier+Adjust
States.Set=>{Mnemo="init" Cons.OutLn="state-init" Cons.OutLn="state-init-2"}
Cons.OutLn="boot"
Cons.InputMk=Tmp.Set
Tmp.OutMk=Sum.Set
Tmp.OutMk=Sum.Add
Scale.OutMk=Sum.Mul
Sum.Sub=Base
Sum.OutMk=Cons.OutLn
Cons.OutLn=Base*Multiplier+Adjust
Cons.OutLn="done"
