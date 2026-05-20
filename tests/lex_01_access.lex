// Lex is now a user-visible FU mnemo (injected at .ind-load time, same
// path as MnemoTable). User code can call Lex.<MK>= and reach the
// bootstrap Lex without syntax error. This test exercises a few inert
// settings (SepSet adds a literal to the separator set but only matters
// if the rest of the input contains it; we don't, so behavior is
// observable only as "no syntax error + framing prints both fire").
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="-- pre --"
Lex.SepSet="@"
Lex.SepSet="$"
Lex.SepSet="!"
Cons.OutLn="-- mid --"
Lex.SepErase="@"
Cons.OutLn="-- post --"
