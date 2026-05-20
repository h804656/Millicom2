// MnemoTable.FindAnd should match a row we just added via LineCopyAdd.
// We enable MkModeSet so the matched row's body runs (same mechanism the
// parser already uses for built-in mnemos like "Cons"/"NewFU").
NewFU={Mnemo="Cons" FUType=FUConsNew}
MnemoTable.MkModeSet=1
MnemoTable.LineCopyAdd={Mnemo="Foo" Cons.OutLn="found-foo"}
MnemoTable.LineCopyAdd={Mnemo="Bar" Cons.OutLn="found-bar"}
MnemoTable.FindAnd={Mnemo="Bar"}
Cons.OutLn="after"
