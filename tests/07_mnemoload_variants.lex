// EqualAfter --[Mnemo]--> MnemoLoad has four exits: '(', '{', '>/<', else.
// All four in one program. The else-row needs the next token to NOT be (, {,
// or >/< -- a fresh Mk/Atr triggers MnemoLoad else and re-dispatches.
//   Atr=Helper(A)      MnemoLoad ( -> LoadVar Const/Var/Vect -> BrackLoadVar )
//   Atr=NewObj{X=1}    MnemoLoad { -> subgraph
//   Atr=Wrapper>z      MnemoLoad > <  -> ListAfter Mnemo
//   Atr=Done           MnemoLoad parked; next 'Atr=' triggers else -> Root
A=1
Atr=Helper(A)
Atr=NewObj{X=1}
Atr=Wrapper>z
Atr=Done
Final=42
