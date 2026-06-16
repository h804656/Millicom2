// Per-FU method resolution: the name "OutMk" exists in TWO different method tables
// with DIFFERENT mks -- IntAlu's OutMk = mk21 (emit the accumulator) and List's
// OutMk = mk3 (dump the rows). The SubDown resolution must search each FU's OWN
// -21 table, so A.OutMk dispatches IntAlu's mk21 (prints 42) while L.OutMk
// dispatches List's mk3 (prints the rows r1/r2) -- NOT a single global/shared mk.
// (Declaring L before using A would expose a global-resolution bug; A still -> 42.)
NewFU={Mnemo="A" FUType=FUIntAluNew}
NewFU={Mnemo="L" FUType=FUListNew}
A.Set=42
Cons.OutLn="A-OutMk"
A.OutMk=Cons.OutLn
Cons.OutLn="L-OutMk"
L.Set=>{Mnemo="r1"}>{Mnemo="r2"}
L.OutMk=Cons.OutLn
