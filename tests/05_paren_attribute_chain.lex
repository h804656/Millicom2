// AtrAtr family. Each statement ends with the parser back at Root.
//   Baz(Atr)         MnemoAnalysis ( -> AtrAtr Mk/Atr -> AtrBrack ) -> Root
//   Foo(Bar(Atr))    nested -- inner AtrAtr3 chain returns to Root via inner ')'
//                    The outer ')' falls into Root's else (unexpected) but the
//                    inner chain has already fired its trace lines. We accept
//                    the trailing 'Root: unexpected token' as harmless.
A=1
Baz(Atr)
Foo(Bar(Atr))
