// The "hello-world" version of a full CompileCC.oap program: one NewFU
// declaration, then a sequence of FU.MK calls (Cons.OutLn variants) with
// different arg shapes (string literal, Var, no arg), interleaved with an
// EqualAfter FU chain. Exercises the multi-FU dispatch loop end-to-end with
// the new MkWait MkForFU fix in play.
NewFU={Mnemo="L" FUType=FUListNew}
A=1
Cons.OutLn="step1"
Cons.OutLn=A
Atr=Bus.MakeFU
Cons.OutLn="step2"
Cons.Out="x"
Cons.OutLn="step3"
Final=42
