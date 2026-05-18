"""
Byte-level patch for CompileNew6.oap: add the IpBuf* MK names (250-259)
to Bus's MkList. This is idempotent -- safe to re-run after edits.
"""
import sys

PATH = r"E:\miemp\Millicom\Millicom\CompileNew6.oap"
BAK  = r"E:\miemp\Millicom\Millicom\CompileNew6.oap.bak"

# Always patch from the pristine backup so re-runs don't accumulate.
with open(BAK, "rb") as f:
    data = f.read()

ANCHOR = b'\r\n  Obj={Mnemo="FU index"'
INSERT = (
    b'\r\n  Obj={Mnemo="IpBufWrite" Mk=253 Hint="Serialize buffer to .ind"}'
    b'\r\n  Obj={Mnemo="IpBufSetAtr" Mk=256 Hint="Buffer atr for next emit"}'
    b'\r\n  Obj={Mnemo="IpBufEmit" Mk=257 Hint="Emit IP from buffered atr+Load"}'
    b'\r\n  Obj={Mnemo="IpBufStashLoad" Mk=258 Hint="Stash Load for stashed emit"}'
    b'\r\n  Obj={Mnemo="IpBufEmitStashed" Mk=259 Hint="Emit IP using stashed Load"}'
    b'\r\n  Obj={Mnemo="AleReset" Mk=270 Hint="Reset ALE evaluator"}'
    b'\r\n  Obj={Mnemo="AleOpSet" Mk=271 Hint="Set pending ALE operator (1=Add..5=Mod)"}'
    b'\r\n  Obj={Mnemo="AleApply" Mk=272 Hint="Apply pending ALE op to operand value"}'
    b'\r\n  Obj={Mnemo="AleParenIn" Mk=273 Hint="ALE: enter paren scope"}'
    b'\r\n  Obj={Mnemo="AleParenOut" Mk=274 Hint="ALE: exit paren scope and combine"}'
    b'\r\n  Obj={Mnemo="AleEmit" Mk=275 Hint="Emit IP with ALE accumulator value"}'
    b'\r\n  Obj={Mnemo="IpBufPendingMakeFU" Mk=281 Hint="Mark next stashed emit as MakeFU"}'
    b'\r\n  Obj={Mnemo="PendingFuNameModeSet" Mk=286 Hint="Capture next stashed string as user-FU name"}'
)

idx = data.find(ANCHOR)
if idx < 0:
    print("anchor not found")
    sys.exit(1)

new = data[:idx] + INSERT + data[idx:]
with open(PATH, "wb") as f:
    f.write(new)
print(f"patched: {len(INSERT)} bytes at offset {idx}")
print(f"new size: {len(new)} (bak: {len(data)})")
