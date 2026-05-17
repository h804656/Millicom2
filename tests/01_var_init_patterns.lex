// All three VarIniWait value rows in one program, plus an Atr offset and an
// "unknown mnemonic creates an Atr entry via MnemoAnalysis else".
// Transitions exercised:
//   Root Mnemo -> MnemoAnalysis = -> VarIniWait Const  (First=1)
//   Root Mnemo -> MnemoAnalysis = -> VarIniWait Const  (Second=2)
//   Root Mnemo -> MnemoAnalysis = -> VarIniWait Var    (Alias=First)
//   Root Mnemo -> MnemoAnalysis * -> ConstWait2 Const  (Offset*-99)
//   Root Mnemo -> MnemoAnalysis else (Atr)             (BareIdent ;)
First=1
Second=2
Alias=First
Offset*-99
BareIdent
