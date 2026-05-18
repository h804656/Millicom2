// Error: AtrAtr3 (after `Mnemo(Mnemo(`) expects FU/Mk/Atr; another `(` triggers else.
//   Foo(           -> Root Mnemo -> MnemoAnalysis (
//   Bar            -> AtrAtr Mnemo
//   (              -> AtrAtr2 (
//   (              -> AtrAtr3 else -> SYNTAX ERROR
Foo(Bar((
