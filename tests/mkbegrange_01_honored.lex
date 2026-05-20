// MkBegRange=N in a NewFU body sets the new FU's FUMkGlobalAdr to N.
// Verified here by using Bus.LastFUMkRangeOutMk to print the most-
// recently-created FU's MK range; without the MkBegRange honor it would
// print the default index*1000 value (smaller, since the FU is somewhere
// in the 40-50 index range).
NewFU={Mnemo="Cons" FUType=FUConsNew}
NewFU={Mnemo="A" MkBegRange=99000 FUType=FUIntAluNew}
Bus.LastFUMkRangeOutMk=Cons.OutLn
