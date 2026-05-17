// Every '>'/'<' transition, designed so each statement leaves the parser at
// Root. Statements that don't end at Root come last.
//   List1 > 1            MnemoAnalysis > -> ListAfter Mk/Atr/Const/Var -> Root
//   List2 > > 2          MnemoAnalysis > -> ListAfter > self -> Mk/Atr/Const/Var -> Root
//   ListN > Item{X=1}    ListAfter Mnemo -> ListAfterMnemo { -> Root subgraph
//   > > Foo              Root > self-loop x2; then the trailing Mnemo parks in
//                        MnemoAnalysis (acceptable for the LAST statement)
List1 > 1
List2 > > 2
ListN > Item{X=1}
> > Foo
