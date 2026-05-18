// Five consecutive NewFU={...} declarations -- the same shape as the FU block
// at the top of CompileCC.oap. Each NewFU must leave the parser in clean Root
// state for the next. (Previously overflowed the C stack via a
// MnemoTable.FindAnd <-> Lex.SendToReceiver ping-pong; fixed in Lex.cpp.)
// Trailing var-init + FU.MK + Var arg verifies state stays clean.
NewFU={Mnemo="L1" FUType=FUListNew}
NewFU={Mnemo="L2" FUType=FUListNew}
NewFU={Mnemo="L3" FUType=FUListNew}
NewFU={Mnemo="Cnt" FUType=FUIntAluNew}
NewFU={Mnemo="L5" FUType=FUListNew}
A=1
Cons.OutLn="declared"
Atr=Bus.MakeFU
Done=42
