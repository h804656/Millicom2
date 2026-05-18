// Five ALUs holding distinct values. Transfer data between them via
// .Out=.Set / .Add. Final answer is a chained reduction:
//   A=10  B=20  C=30  D=40  E=50
//   A.Out=Acc.Set        -> Acc=10
//   B.Out=Acc.Add        -> Acc=30
//   C.Out=Acc.Sub        -> Acc=0
//   D.Out=Acc.Add        -> Acc=40
//   E.Out=Acc.Add        -> Acc=90
//   Acc.Div=2            -> Acc=45
//   Acc.OutMk=Cons.OutLn
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" FUType=FUIntAluNew}
NewFU={Mnemo="B" FUType=FUIntAluNew}
NewFU={Mnemo="C" FUType=FUIntAluNew}
NewFU={Mnemo="D" FUType=FUIntAluNew}
NewFU={Mnemo="E" FUType=FUIntAluNew}
NewFU={Mnemo="Acc" FUType=FUIntAluNew}
A.Set=10
B.Set=20
C.Set=30
D.Set=40
E.Set=50
A.OutMk=Acc.Set
B.OutMk=Acc.Add
C.OutMk=Acc.Sub
D.OutMk=Acc.Add
E.OutMk=Acc.Add
Acc.Div=2
Acc.OutMk=Cons.OutLn
