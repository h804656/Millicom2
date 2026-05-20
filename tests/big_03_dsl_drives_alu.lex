// DSL where each keyword mutates an ALU accumulator. Combines:
//   - MnemoTable extension with multi-IP rows that include positive-atr
//     FU.MK= IPs (cross-FU dispatch from inside a row body)
//   - Two ALUs tracked in parallel (Sum, Cnt)
//   - keyword dispatch via Lex.UnicMk=MnemoTable.FindAnd
// Final REPORT keyword reads both ALUs and prints them.
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="Sum" FUType=FUIntAluNew}
NewFU={Mnemo="Cnt" FUType=FUIntAluNew}
MnemoTable.MkModeSet=1
Sum.Set=0
Cnt.Set=0
MnemoTable.LineCopyAdd={Mnemo="ADD5"  Sum.Add=5  Cnt.Add=1}
MnemoTable.LineCopyAdd={Mnemo="ADD10" Sum.Add=10 Cnt.Add=1}
MnemoTable.LineCopyAdd={Mnemo="REPORT" Sum.OutMk=Cons.OutLn Cnt.OutMk=Cons.OutLn Cons.OutLn="-end-"}
Cons.OutLn="-- start --"
ADD5
ADD10
ADD5
ADD10
ADD5
REPORT
