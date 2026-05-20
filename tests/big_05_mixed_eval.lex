// Mix compile-time ALE with runtime ALU + stdin in one program.
//   - vars X,Y,Z,W (compile-time constants in ALE)
//   - two ALE prints with parens + precedence
//   - one stdin value into Alu, mutated through several ALU ops
//   - cross-FU transfer (Cnt.Out from a second ALU into Alu.Add)
// With stdin "100": Alu=100, +X(4)=104, *W(2)=208, -Y(7)=201,
// +Cnt(50)=251, /Z(3)=83.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Alu" FUType=FUIntAluNew}
NewFU={Mnemo="Cnt" FUType=FUIntAluNew}
X=4
Y=7
Z=3
W=2
Cnt.Set=50
Cons.OutLn="-- compile-time --"
Cons.OutLn=X*Y+Z
Cons.OutLn=(X+Y)*W-Z
Cons.OutLn="-- runtime --"
Cons.InputMk=Alu.Set
Alu.Add=X
Alu.Mul=W
Alu.Sub=Y
Cnt.OutMk=Alu.Add
Alu.Div=Z
Alu.OutMk=Cons.OutLn
Cons.OutLn="-- done --"
