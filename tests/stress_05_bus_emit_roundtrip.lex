// Bus.IpBufWrite serializes the current CapsIps buffer to a .ind file.
// This exercises:
//   - the MakeFU IPs (NewFU lines emit two MakeFU + body sub-caps)
//   - a runtime ALU operation result emitted as a literal IP
//   - the IpBufWrite call itself emitting a 1253 S:... IP
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" FUType=FUIntAluNew}
A.Set=100
A.Sub=58
A.OutMk=Cons.OutLn
Bus.IpBufWrite="C:/Users/hacker/Downloads/millicom/tests/.stress05_emit.ind"
Cons.OutLn="emit-done"
