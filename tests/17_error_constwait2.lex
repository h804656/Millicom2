// Error: ConstWait2 (after `Mnemo*`) requires an integer Const; a Mnemo triggers else.
//   Off            -> Root Mnemo
//   *              -> MnemoAnalysis *
//   Foo            -> ConstWait2 else -> SYNTAX ERROR
Off*Foo
