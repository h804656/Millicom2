// Three back-to-back ALE expressions, each a different shape. Tests that
// the receiver stack and the Stack-of-parens are released cleanly between
// expressions.
//   Atr=A+B*C        chain      (EqVar Operator push, two ALE-cycles)
//   Atr=(A+B)*C      paren-first (EqualAfter ( push, two cycles + outer op)
//   Atr=A-(B-C)      paren-mid   (push ALEAfter, then ALE inner paren)
A=1
B=2
C=3
Atr=A+B*C
Atr=(A+B)*C
Atr=A-(B-C)
Done=42
