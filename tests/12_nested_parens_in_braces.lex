// Deeply nested mix of brace subgraphs, paren attribute-of-attribute
// expressions, var-inits, and AtrAtr2->AtrAtr3 chains.
//   A=1                                  Const branch
//   Foo{ Bar(Atr) Baz=A Inner{Deep=2 Nested(Mnemo)} }
//     -> MnemoAnalysis { (pushes outer)
//        Bar(Atr)   -> MnemoAnalysis ( -> AtrAtr Mk/Atr -> AtrBrack )
//        Baz=A      -> VarIniWait Var
//        Inner{...} -> MnemoAnalysis { (pushes inner)
//          Deep=2   -> VarIniWait Const
//          Nested(Mnemo)
//                   -> MnemoAnalysis ( -> AtrAtr Mk/Atr -> AtrBrack )
//        }          -> Root }
//     }             -> Root }
A=1
Foo{Bar(Atr) Baz=A Inner{Deep=2 Nested(Mnemo)}}
