// Regression: `MnemoTable.Set=>{Mnemo="FUType" Lex.SendToReceiver={Atr=FUType}}`
// (line ~197 of CompileCC.oap) used to crash because the existing FUType
// MnemoTable row fires `Main_Bus.IpBufPendingMakeFU=0` as a side effect
// whenever "FUType" is looked up. When that lookup happens inside an
// unrelated row body (here the multi-row Set= rebuilding FUType's own row),
// the flag still fired, the C++ deferred-MakeFU path then ran at the
// surrounding row's close, and crashed dispatching MakeFU with garbage
// state. Fix: gate the IpBufPendingMakeFU -> PendingMakeFU transition on
// SubCapDepth == 1 (directly inside a NewFU={...} body), not deeper.
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="before"
MnemoTable.Set=
>{Mnemo="Atr"    Lex.SendToReceiver={Atr=Atr}}
>{Mnemo="Mnemo"  Lex.SendToReceiver={Atr=Mnemo}}
>{Mnemo="FUType" Lex.SendToReceiver={Atr=FUType}}
Cons.OutLn="after"
