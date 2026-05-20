// MnemoTable is now user-visible (injected from C++ at .ind-load time
// to sidestep Delphi's "duplicate MkTable.Set=X! per row" limit). User
// code can extend the runtime mnemonic table with `MnemoTable.LineCopyAdd`.
// Typing the newly-registered name fires the row body via MnemoTable's
// existing MkAtrExec-on-match path.
NewFU={Mnemo="Cons" FUType=FUConsNew}
MnemoTable.LineCopyAdd={Mnemo="Hello" Cons.OutLn="hello-from-row"}
MnemoTable.LineCopyAdd={Mnemo="World" Cons.OutLn="world-from-row"}
Cons.OutLn="before"
Hello
World
Cons.OutLn="after"
