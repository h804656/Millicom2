// Multi-FU method dispatch -- exercises every FU.MK token-type path through
// Root and EqualAfter, plus the cross-FU interleaving you see in real OAP
// programs (var setup, three Cons.OutLn flavours, an FU.MK via EqualAfter,
// then a final var).
//   A=1, B=2                     VarIniWait Const x2
//   Cons.OutLn="start"           Root FU/FUType, FUAfter ., MkWait MkForFU,
//                                AtrBrack else->EqualWait, EqualWait =,
//                                EqualAfter Mnemo, MnemoLoad else->Root
//   Cons.OutLn=A                 same chain ending in EqualAfter Const/Var/Vect,
//                                EqVar else -> Root
//   Cons.Out=B                   same again (different Mk on Cons)
//   Atr=Bus.MakeFU               EqualAfter FU -> FUAfter2 . -> MkWait2
//                                MkForFU -> LoadAfter else -> Root
//   Cons.OutLn="end"             one more method call
//   Done=42                      verifies Root is clean at end
A=1
B=2
Cons.OutLn="start"
Cons.OutLn=A
Cons.Out=B
Atr=Bus.MakeFU
Cons.OutLn="end"
Done=42
