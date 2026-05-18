// '>'/'<' list separators in every context + MnemoLoad's four exits.
// Transitions exercised:
//   MnemoAnalysis > < -> ListAfter
//   ListAfter Mk/Atr/Const/Var, ListAfter > < self-loop, ListAfter Mnemo
//   ListAfterMnemo { -> Root subgraph
//   ListAfterMnemo else (Tail after Top parks, the next `=` bounces back to Root)
//   Root > < self-loop (final `> > Foo`)
//   EqualAfter Mnemo -> MnemoLoad with all four exits:
//     MnemoLoad (, MnemoLoad {, MnemoLoad > <, MnemoLoad else->Root
//   LoadVar Const/Var/Vect -> BrackLoadVar )
A=1
List1 > 1
List2 > > 2
ListN > Item{X=1}
ListE > Top Tail=99
Atr=Helper(A)
Atr=NewObj{X=1}
Atr=Wrapper>z{Inner=1}
Atr=Done
Final=42
> > Foo
