// Mini DSL: extend MnemoTable with several user keywords. Each keyword's
// row body has multiple effect IPs (compile-time consts + Cons.OutLn).
// Exercises: MnemoTable injection, multi-IP row bodies, MkAtrExec firing
// positive-atr IPs in registration order, MnemoTable.FindAnd via Lex's
// UnicMk dispatch path on each keyword token.
NewFU={Mnemo="Cons" FUType=FUConsNew}
MnemoTable.MkModeSet=1
MnemoTable.LineCopyAdd={Mnemo="HELLO" Cons.OutLn="hello-handler-1" Cons.OutLn="hello-handler-2"}
MnemoTable.LineCopyAdd={Mnemo="GOODBYE" Cons.OutLn="goodbye-handler"}
MnemoTable.LineCopyAdd={Mnemo="STATUS" Cons.OutLn="status-line-A" Cons.OutLn="status-line-B" Cons.OutLn="status-line-C"}
MnemoTable.LineCopyAdd={Mnemo="VERSION" Cons.OutLn="v1.0.0"}
Cons.OutLn="-- dsl boot --"
HELLO
STATUS
VERSION
HELLO
GOODBYE
Cons.OutLn="-- dsl done --"
