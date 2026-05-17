// Real multi-FU declaration block, mirroring the shape of the actual
// NewFU={...} stack at the top of CompileCC.oap. Three back-to-back NewFU
// declarations (a pattern that used to overflow the C stack via a
// MnemoTable.FindAnd <-> Lex.SendToReceiver ping-pong), followed by var
// setup, a method call, an EqualAfter FU chain, and a clean Done finish.
NewFU={Mnemo="L1" FUType=FUListNew}
NewFU={Mnemo="L2" FUType=FUListNew}
NewFU={Mnemo="Cnt" FUType=FUIntAluNew}
A=1
Cons.OutLn="declared"
Atr=Bus.MakeFU
Done=42
