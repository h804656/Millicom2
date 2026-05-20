// Round-trip self-runnable .ind: NewFU={Mnemo="X" FUType=Y} emits as a
// `1001 I:<fuType>` Bus.MakeFU IP into CapsIps (not the old
// CreateNewFU.FindOr atr 14228), and user-FU MK addresses get rebased
// from compile-time FU indices (~47) to reload-time indices (2, 3, ...).
// This test is structural: the actual round-trip is exercised by the
// fixture oap2/tiny.oap + oap2/tiny_compiled.ind (committed alongside)
// and re-validated here by re-running tiny.oap through the compiler.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Cons2" FUType=FUConsNew}
Cons.OutLn="cons one"
Cons2.OutLn="cons two"
Cons.OutLn="cons one again"
