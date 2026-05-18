// Error: ALE entered (after a binary operator) with no operand to follow.
//   Atr=A          -> Root Mk/Atr -> EqualWait = -> EqualAfter Const/Var/Vect
//   +              -> EqVar Operator -> ALE
//   ;              -> ALE else -> SYNTAX ERROR
A=1
Atr=A+
