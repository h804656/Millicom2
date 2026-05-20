// `//` comments on their own line AND trailing inline. Both should be
// silently consumed; the code on each line should run.
NewFU={Mnemo="Cons" FUType=FUConsNew}
Cons.OutLn="alpha" // trailing comment after statement
// standalone comment line
Cons.OutLn="bravo"
// another standalone
Cons.OutLn="charlie" // and one more inline
Cons.OutLn="delta"
