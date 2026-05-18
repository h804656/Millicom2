// Variable init (Const, Var, Vect) + ConstWait2 offset + bare-Atr + bare-Var
// at Root + single-element vector + MnemoAnalysis else (Atr).
// Transitions exercised:
//   Root Mnemo -> MnemoAnalysis = -> VarIniWait {Const,Var,Vect}
//   Root Mnemo -> MnemoAnalysis * -> ConstWait2 Const
//   Root Mnemo -> MnemoAnalysis # -> ConstWait -> VectWait <-> VectNext4 <-> VectNext5
//   VectWait else->Root (single-element vector V#1)
//   var-as-vector-element (X,Y,Z dispatch as Var atr after prior X=10/Y=20/Z=30)
//   Root Mk/Atr -> EqualWait else->Root (bare Atr, next token bounces back)
//   Root Var -> ALE (bare X at Root re-sent from EqualWait else)
//   MnemoAnalysis else (Atr) (BareIdent at end, harness `;` triggers else)
First=1
Second=2
Alias=First
Offset*-99
V#1
V2#1,2,3,4,5
Vec3#10,20
X=10
Y=20
Z=30
V4#X,Y,Z
W=99
Atr
X
BareIdent
