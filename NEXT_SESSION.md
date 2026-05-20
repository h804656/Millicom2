# Next-session handoff (2026-05-19)

## Where we left off

**Goal**: replace `BusFU::InjectBuiltinMnemos` (Bus.cpp:762) — the C++ shim that
hand-injects MnemoTable + Lex mnemo rows after `.ind` load — with the equivalent
declaration done in OAP, matching CompileNew6.oap's pattern
(`NewFU={Mnemo="MnemoTable" FUType=FUList}` at line 11).

**Decision** (chosen via AskUserQuestion this session):
- "Move NewFU declarations to BootstrapCC; CompileCC.oap stops declaring them"
- Sub-step picked: "Tiny first step" (add scaffolding only, don't refactor load
  order yet)

## What changed this session

1. **`oap2/BootstrapCC.oap`** — added two NewFU scaffolding rows at the top:
   ```
   NewFU={Mnemo="MnemoTable" FUType=FUListNew Hint="Таблица мнемоник"}
   NewFU={Mnemo="Lex"        FUType=FULexNew  Hint="Лексический анализатор"}
   ```
   Plus the user added (intentionally) a destructive `MnemoTable.Set=` block at
   lines 26-31. That block REPLACES the live MnemoTable with 5 rows
   (Atr/Mnemo/FUType/Hint/pi) — wiping every other mnemonic the loaded
   compiler depends on.

2. **`tests/run_tests.ps1`** — auto-prepends `--lex-file oap2\BootstrapCC.oap`
   for every test. A `<name>.no-bootstrap` marker file opts a test out.
   `<name>.bootstrap` sidecars still add EXTRA files after the default.

3. **Removed** `tests/selfhost_05_bootstrap_atrnames.bootstrap` — now redundant
   since auto-preload covers it.

## Current test status

**1 / 60 PASS.** The 59 failures all stem from the destructive
`MnemoTable.Set=` in BootstrapCC.oap:26-31 — after it runs, the loaded
compiler has no rows for `Console`, `Lex`, `Mk`, `Atr`, any state name, etc.

**Pending unanswered question** (an AskUserQuestion the user interrupted):
how to handle those 5 destructive rows? Options were:
- Convert `MnemoTable.Set=` → 5 `MnemoTable.LineAdd=` rows (append, don't
  replace). Tests should go back to ~60 green.
- Delete the 5-row block entirely.
- Leave it — investigate the collapse intentionally.

**Start next session by asking the user which of these they want**, since the
user explicitly added the Set= block — it may be a probe they want to keep.

## The bigger architectural arc (not started)

To actually remove `InjectBuiltinMnemos`, beyond the scaffolding above:

| Step | What |
|---|---|
| a | Compile BootstrapCC.oap → BootstrapCC.ind (via Delphi, working dir = `E:\miemp\Millicom\Millicom`) |
| b | Modify `Millicom.cpp` to accept multiple `.ind` files and load them in order |
| c | Remove `NewFU={Mnemo="MnemoTable"...}` and `NewFU={Mnemo="Lex"...}` from CompileCC.oap (lines 15-16) |
| d | Rebase the hard-coded `Lex.SendToReceiver={FU=N}` base addresses in CompileCC.oap (lines 253-257 etc.) — they'll shift if BootstrapCC.ind pre-creates FUs |
| e | Recompile CompileCC.oap → CompileCC.ind (via Delphi) |
| f | Verify 60 tests pass |
| g | Delete `InjectBuiltinMnemos` from Bus.cpp/Bus.h + the call site in Millicom.cpp |

Step (d) is the subtle one — `Lex.SendToReceiver={FU=4000}` is the runtime
address Stack lives at, and the address only stays correct if Stack is still
the 4th-created FU. Bootstrap-side creations shift everything.

## Key file/line references

- `Bus.cpp:762-789` — `InjectBuiltinMnemos` (the thing we're trying to remove)
- `Bus.cpp:642-731` — `addUserFuMnemoRow` (the runtime peer-cloning we'd
  rely on instead, called from the deferred-MakeFU dispatch at Bus.cpp:341)
- `Bus.cpp:738-758` — comment explaining WHY InjectBuiltinMnemos exists
  (Delphi 5-forward-ref truncation limit)
- `Millicom.cpp:130` — the call that needs removing in the end state
- `oap2/CompileCC.oap:15-16` — NewFU declarations to eventually remove
- `oap2/CompileCC.oap:253-257` — the 5 peer rows (Bus, Cons, IntAluPeer,
  ListPeer, LexPeer) that sit at Delphi's truncation limit
- `oap2/CompileCC.oap:222` — `FUType` mnemo row with the
  `Main_Bus.IpBufPendingMakeFU=0` side effect (the depth-gate fix from
  the prior session lives at Bus.cpp case 259 ~line 470)

## Build & test commands

```powershell
# Build C++ executor (when Bus.cpp/Lex.cpp/etc. change)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
    Millicom.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal

# Recompile CompileCC.oap → CompileCC.ind (when oap2/CompileCC.oap changes)
Start-Process -FilePath "E:\miemp\Millicom\Millicom\millicom.exe" `
  -ArgumentList @(
    '--bootstrap',  'C:\Users\hacker\Downloads\millicom\oap2\CompileCC.oap',
    '--run',        'C:\Users\hacker\Downloads\millicom\tests\empty.oap',
    '--compile-to', 'C:\Users\hacker\Downloads\millicom\oap2\CompileCC.ind',
    '--exit-delay', '8000', '--exit'
  ) `
  -WorkingDirectory "E:\miemp\Millicom\Millicom" `
  -WindowStyle Hidden -Wait

# Run tests
.\tests\run_tests.ps1                  # all
.\tests\run_tests.ps1 -Filter 04_*     # subset
.\tests\run_tests.ps1 -ShowOutput      # dump stdout per test
```
