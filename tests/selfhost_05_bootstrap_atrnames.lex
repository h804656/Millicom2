// BootstrapCC.oap pre-load: with the bootstrap file fed first, built-in
// atr names like `Mk`, `Const`, `ProgStop`, `MkBegRange` resolve in row
// bodies and as RHS values. Before BootstrapCC.oap, these names errored
// (`Mk=N` would create a global `Mk` var; subsequent uses cross-talked).
// See oap2/BootstrapCC.oap for the source of truth; the `.bootstrap`
// sidecar file tells the harness to feed it first.
NewFU={Mnemo="Cons" FUType=FUConsNew}
// Atr-typed bootstrap names on RHS of var-init -- each should bind to
// its negative atr code.
mk_code=Mk
Cons.OutLn=mk_code
const_code=Const
Cons.OutLn=const_code
constint_code=ConstInt
Cons.OutLn=constint_code
constfloat_code=ConstFloat
Cons.OutLn=constfloat_code
fu_typelist_code=FUTypeList
Cons.OutLn=fu_typelist_code
mklist_code=MkList
Cons.OutLn=mklist_code
// Bare-name MK table using `Mk=N` field semantics inside row bodies --
// the original failure case from CompileCC.oap:96 (CommonMkTable et al).
SomeTable
	>{Mnemo="r1" Mk=995 Hint="row a"}
	>{Mnemo="r2" Mk=999 Hint="row b"}
Cons.OutLn="table OK"
Cons.OutLn="done"
