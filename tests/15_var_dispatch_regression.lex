// Var-dispatch regression guard. Earlier rounds had a real bug where a
// previously-declared Var (X=1 ...) would dispatch as Const-atr when
// referenced later -- the MnemoTable entry's Const/Var bit wasn't being
// toggled correctly through the LineCopyAddPrevLoadSet path.
//
// The fix landed via the List.cpp empty-vector guards on MK 163/159 and the
// Stack.LineAdd -> Stack.LineCopyAdd swap. This fixture asserts Var arrives
// as a Var-atr token at every receiver that distinguishes it from Const:
//   - VarIniWait Var row    (First=A, Second=B, inside nested {})
//   - VarIniWait Var inside Mk/Atr = { ... } subgraph (Atr={P=A Q=B})
//   - EqualAfter Const/Var/Vect (Var as FU.MK arg: Cons.OutLn=A)
A=1
B=2
First=A
Second=B
Outer{Sub=A Mid{Deep=B Leaf=A}}
Atr={P=A Q=B}
Cons.OutLn=A
