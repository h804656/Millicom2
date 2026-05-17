// All EqualAfter destinations from a Mk/Atr LHS in one fixture. Statements
// chain through Root cleanly between each.
//   Atr={X=1}         EqualAfter { -> Root subgraph
//   Atr=>1            EqualAfter > <  -> ListAfter Mk/Atr/Const/Var
//   Atr=Bus.MakeFU    EqualAfter FU -> FUAfter2 . -> MkWait2 MkForFU
//                     -> LoadAfter else -> Root  (full FU.MK resolution)
//   Atr=1+2 X=99      EqVar Operator -> ALE; ALE consumes 1+2; non-arith X
//                     pops ALE, Atr=99 resumes at Root.
A=1
Atr={X=1}
Atr=>1
Atr=Bus.MakeFU
Atr=1+2 X=99
