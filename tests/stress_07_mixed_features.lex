// Mix of features:
//   - NewFU declares 1 Cons, 2 ALUs, 1 List
//   - compile-time ALE on vars (X,Y,Z) drives both string output and ALU input
//   - stdin reads a single dynamic value via Cons.InputMk into B
//   - cross-FU transfer A.Out=B.Add
//   - multi-IP row registered on the List (suppressed at parse time)
//   - final compile-time print after the row
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" FUType=FUIntAluNew}
NewFU={Mnemo="B" FUType=FUIntAluNew}
NewFU={Mnemo="L" FUType=FUListNew}
X=4
Y=5
Z=2
A.Set=X*Y+Z
Cons.InputMk=B.Set
A.OutMk=B.Add
L.Set=>{Mnemo="row" Cons.OutLn="must-not-row-1" Cons.OutLn="must-not-row-2"}
B.OutMk=Cons.OutLn
Cons.OutLn="done"
