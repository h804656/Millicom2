// Edge case: `LHS= // comment` (inline comment IMMEDIATELY after `=`,
// no real RHS on the same line). The comment is silently consumed by
// Lex state 11, so VarIniWait/EqualAfter never see a value. The next
// statement's first token then arrives at the still-active value-state
// and (correctly) trips the "value expected" error. This is the right
// failure mode for what is, after all, a malformed program -- it's
// just that the malformedness is now visible instead of being silently
// miscompiled into A=<comment-text>.
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="before"
A= // comment that should NOT become A's value
B=2
Cons.OutLn="never-reaches-here"
