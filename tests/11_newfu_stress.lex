// Stress test for the recent multi-NewFU fix. Five consecutive NewFU
// declarations -- the same shape as the FU-declaration block at the top of
// CompileCC.oap, which used to overflow the C stack via a
// MnemoTable.FindAnd <-> Lex.SendToReceiver recursion before the fix in
// Lex.cpp's SendToReceiver case. Each NewFU should leave the parser in a
// clean Root state for the next one.
NewFU={Mnemo="A1" FUType=FUListNew}
NewFU={Mnemo="A2" FUType=FUListNew}
NewFU={Mnemo="A3" FUType=FUListNew}
NewFU={Mnemo="A4" FUType=FUListNew}
NewFU={Mnemo="A5" FUType=FUListNew}
Done=42
