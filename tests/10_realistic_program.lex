// Long realistic OAP program -- 15 statements. Resembles a real CompileCC.oap
// excerpt: a block of NewFU declarations (three FUs of different types),
// followed by var-inits, atr offset, vector literal, attribute-block,
// attribute-of-attribute paren, FU.MK, ALE expression, FU.MK with inline
// string, and a closing brace-subgraph using the declared Vars.
NewFU={Mnemo="L1" FUType=FUListNew}
NewFU={Mnemo="L2" FUType=FUListNew}
NewFU={Mnemo="Cnt" FUType=FUIntAluNew}
A=1
B=2
Offset*-99
V#10,20,30
Atr={X=1 Y=2}
NewObj(Atr)
Atr=Bus.MakeFU
Atr=1+2
Cons.OutLn="hi"
Atr={P=A Q=B}
Done=42
