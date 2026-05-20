// A user-program "compiler setup": touches Lex, MnemoTable, and an
// IntAlu all by user-visible name. Demonstrates that the three core
// runtime FUs (the lexer, the global mnemo table, and a fresh accumulator)
// are reachable without any privileged C++ hookup -- just OAP code.
//   - Lex.SepSet adds two extra separator literals (inert here, but
//     proves Lex.<MK>= dispatches into the bootstrap Lex by name).
//   - MnemoTable.MkModeSet=1 enables row-body dispatch on Find success.
//   - Three keywords (INIT, ADD, SHOW) drive an Acc accumulator.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Acc" FUType=FUIntAluNew}
Lex.SepSet="$"
Lex.SepSet="@"
MnemoTable.MkModeSet=1
Acc.Set=0
MnemoTable.LineCopyAdd={Mnemo="INIT" Acc.Set=100 Cons.OutLn="initialized"}
MnemoTable.LineCopyAdd={Mnemo="ADD"  Acc.Add=7   Cons.OutLn="added"}
MnemoTable.LineCopyAdd={Mnemo="SHOW" Acc.OutMk=Cons.OutLn}
Cons.OutLn="-- start --"
INIT
ADD
ADD
ADD
SHOW
Cons.OutLn="-- done --"
