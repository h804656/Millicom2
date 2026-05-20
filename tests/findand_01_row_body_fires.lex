// FindAnd on a user-declared List fires the matched row's body.
// Setup:
//   - L.MkModeSet=1   tells Searcher to ProgExec the row body on match
//   - L.Set=>{...}>{...} populates L with two rows (multi-row form)
// Then L.FindAnd={Mnemo="A"} matches row A by template IP {atr=-2 Load="A"}
// and the body's positive-atr IPs (Cons.OutLn="fired-A") execute.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="L" FUType=FUListNew}
L.MkModeSet=1
L.Set=>{Mnemo="A" Cons.OutLn="fired-A"}>{Mnemo="B" Cons.OutLn="fired-B"}
Cons.OutLn="before-find"
L.FindAnd={Mnemo="A"}
Cons.OutLn="middle"
L.FindAnd={Mnemo="B"}
Cons.OutLn="after-find"
