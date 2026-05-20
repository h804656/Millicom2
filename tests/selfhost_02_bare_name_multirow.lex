// MnemoAnalysis `>` bare-name multi-row: `Name >{r1} >{r2}` where Name
// is NOT preceded by `=`. Before the fix the second `>{...}` row hit
// "Root: unexpected token -4 # {" because Root's `>` self-loop accepts
// `>` but has no `{` handler -- the multi-row continuation never
// engaged after the first row's `}`. Direct excerpt shape from
// CompileCC.oap:96-101 (CommonMkTable et al.).
//
// Row body contents are restricted to Cons.OutLn=<literal> so we test
// JUST the multi-row continuation, not Delphi-side field-assignment
// semantics (e.g. `Mk=1` inside a row body has special meaning in
// Delphi that we don't replicate -- separate gap).
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="before"
SomeTable
	>{Cons.OutLn="row a"}
	>{Cons.OutLn="row b"}
	>{Cons.OutLn="row c"}
Cons.OutLn="after"
