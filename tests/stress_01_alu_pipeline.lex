// 3-stage runtime ALU pipeline driven entirely by stdin.
// Stage layout:
//   Cons.InputMk reads N -> A.Set
//   Cons.InputMk reads M -> A.Add  (A = N+M)
//   A.OutMk -> B.Set                 (B = N+M)
//   Cons.InputMk reads K -> B.Mul    (B = (N+M)*K)
//   B.OutMk -> C.Set                 (C = (N+M)*K)
//   Cons.InputMk reads L -> C.Sub    (C = (N+M)*K - L)
//   C.OutMk -> Cons.OutLn            print final
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" FUType=FUIntAluNew}
NewFU={Mnemo="B" FUType=FUIntAluNew}
NewFU={Mnemo="C" FUType=FUIntAluNew}
Cons.InputMk=A.Set
Cons.InputMk=A.Add
A.OutMk=B.Set
Cons.InputMk=B.Mul
B.OutMk=C.Set
Cons.InputMk=C.Sub
C.OutMk=Cons.OutLn
