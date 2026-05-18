// Brace subgraphs + paren attribute chains (AtrAtr family).
// Transitions exercised:
//   MnemoAnalysis { -> Root subgraph, Root } pop
//   Empty subgraph Foo{} -- MnemoAnalysis { immediately followed by Root }
//   Four-deep nested { } with statements at each level
//   MnemoAnalysis ( -> AtrAtr Mk/Atr -> AtrBrack )
//   AtrAtr Mnemo -> AtrAtr2 ( -> AtrAtr3 Mk/Atr -> AtrBrack )
//   Mnemo(Mnemo) ends in AtrAtr2 -- next token bounces back to Root
//   Interleaved braces and parens (Outer{Foo(Atr) Inner{Bar(Atr)}})
A=1
Foo{}
Baz(Atr)
Foo(Bar(Atr))
Foo(Bar)
B{C{D{X=1 Y=2} Z=3}}
Outer{Bar(Atr) Sub=A Inner{Deep=2 Nested(Mnemo)}}
A{B{C{D{X=1}}}}
Done=42
