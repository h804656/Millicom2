// Vector declarations and the constant-marker '#' family. Cycles through the
// VectWait <-> VectNext4 <-> VectNext5 chain multiple times.
// Transitions exercised:
//   MnemoAnalysis # -> ConstWait -> VectWait
//   VectWait , -> VectNext4 element -> VectNext5 , -> VectWait ...  (x4)
//   VectWait else -> Root  (terminator)
//   plus a second, shorter vector to verify ListHead reset between statements.
V#1,2,3,4,5
Vec2#10,20
