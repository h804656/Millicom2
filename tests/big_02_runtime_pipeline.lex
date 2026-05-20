// 4-stdin runtime pipeline across 3 ALUs.
// Layout:
//   A = stdin[0], B = stdin[1], C = stdin[2]
//   A += B  (cross-FU transfer via .OutMk)
//   A *= stdin[3]
//   A -= C  (cross-FU transfer)
//   A /= 2  (compile-time literal)
//   print A
// With stdin 4,5,3,6: A=4, +5=9, *6=54, -3=51, /2=25.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" FUType=FUIntAluNew}
NewFU={Mnemo="B" FUType=FUIntAluNew}
NewFU={Mnemo="C" FUType=FUIntAluNew}
Cons.OutLn="-- pipeline start --"
Cons.InputMk=A.Set
Cons.InputMk=B.Set
Cons.InputMk=C.Set
B.OutMk=A.Add
Cons.InputMk=A.Mul
C.OutMk=A.Sub
A.Div=2
A.OutMk=Cons.OutLn
Cons.OutLn="-- pipeline done --"
