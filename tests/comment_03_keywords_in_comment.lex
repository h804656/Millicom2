// Tokens that LOOK like grammar inside a comment must not affect parsing.
// State 11 consumes the line as raw chars; nothing inside emits a token.
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="before"
// NewFU={Mnemo="Fake" FUType=FUConsNew}  Cons.OutLn="ghost-1"
// Cons.OutLn="ghost-2"  // a comment in a comment
// 42 + "string in comment" SHOULD NOT FIRE
Cons.OutLn="after"
