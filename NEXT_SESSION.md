# Next-session handoff (2026-05-21)

## Current state

**Tests: 60/60 passing.** The C++ executor still hosts `InjectBuiltinMnemos`
(Bus.cpp:762) but the OAP-side bootstrap (`oap2/BootstrapCC.oap`) is now
prepended via `--lex-file` to every C++ invocation that takes `.oap` input.

## The bootstrap-prepend rule (settled 2026-05-21)

**Whenever `Millicom.exe` is invoked with `.oap` input, prepend
`oap2/BootstrapCC.oap` as the first `--lex-file`.** Always. No opt-out.

Scripts already wired this way:
- `oap2/run.bat`
- `tests/run_tests.ps1` (the `.no-bootstrap` opt-out was removed)
- `tests/run_ale.ps1`

**Do NOT** try to compile `BootstrapCC.oap` to a `BootstrapCC.ind` and load
it alongside `CompileCC.ind`. That path was investigated and is a dead end —
Delphi's `--compile-to` captures only `NewFU` records from a flat
declaration file, never the `MnemoTable.LineAdd` rows. See "Dead-ends" below
for the specifics so future sessions don't repeat the experiments.

## Removing InjectBuiltinMnemos — where the arc actually stands

The shim survives because there's no equivalent OAP-side mechanism that
runs at `.ind` load. The prepend rule fixes the same effect at *runtime*
(every C++ invocation has the bootstrap text fed in), but the bare
`CompileCC.ind` loaded without `--lex-file` still lacks the
MnemoTable/Lex/atr-name rows.

Two routes to a true removal:

**A) Self-host.** Teach `CompileCC.oap` to emit `.ind` capsules so it
becomes its own compiler. Then Delphi's 5-`MkTable.Set=Y!` cap is gone and
the missing peer rows can be added directly to `CompileCC.oap`. Substantial
work — `CompileCC.oap` is a lexer/state-machine today, not a capsule emitter.
Also has a chicken-and-egg for the first .ind.

**B) Hand-craft BootstrapCC.ind.** Write a Python or C++ generator that
emits the `.ind` text format directly, bypassing Delphi. Then modify
`Millicom.cpp` to accept multiple `.ind` files and load them in order.
Lower-uncertainty than (A), still meaningful work. The `.ind` text format
is documented (informally) in `IndFileConvertOld.cpp`; `oap2/CompileCC.ind`
(1430 records) is the exemplar.

Neither route is the next session's job unless explicitly asked — the
prepend rule keeps tests green and the shim is well-isolated.

## Dead-ends (DO NOT re-litigate)

These were tried 2026-05-20/21 against `BootstrapCC.oap` for the
multi-ind path. All produced a 43-byte / 2-record `.ind` (NewFU only) or
no `.ind` at all.

| Attempt | Result |
|---|---|
| `MnemoTable.LineAdd={...}` flat top-level | 2 records (NewFU only) |
| `Bus={MnemoTable.LineAdd={...}}` wrapping | 2 records |
| `MnemoTable.Set=>{...}>{...}` block | AV in Delphi unless HashAtrSet preconfig added |
| `\**\MnemoTable.Set=` (capture marker) | 2 records |
| `MnemoTable.HashAtrSet=Mnemo` prelude + Set= | No AV, but still 2 records |
| `MainBus.FUTypeCorrectSet=-96` | No effect on capture |
| `MainBus.ModeSet=2` (full IndexMode) | Crash, exit -1 |
| `--run BootstrapCC.oap --compile-to ...` (no --bootstrap) | No .ind |
| `--bootstrap CompileCC.oap --run BootstrapCC.oap --compile-to ...` | No .ind |

Root cause: Delphi's capture pipeline (`CapsManager.IpIcIdOutMk=MainBus.IpIcIdSet`
in Processor.pas) only tees MainBus calls into the IndexVector. Direct-to-FU
calls (MnemoTable.X, Lex.X) bypass it. CompileNew6.oap produces a ~1MB
Lexica.ind anyway via some grammar-driven emit path inside its lexer
state-graph that we couldn't isolate in this investigation.

## Key file/line references

- `Bus.cpp:762-789` — `InjectBuiltinMnemos` (the surviving shim)
- `Bus.cpp:738-758` — comment on the Delphi 5-`MkTable.Set=Y!` cap
- `Millicom.cpp:130` — call site for `InjectBuiltinMnemos`
- `Millicom.cpp:62` — comment sketching the self-host bootstrap (aspirational, not implemented)
- `oap2/CompileCC.oap:15-16` — `NewFU MnemoTable` / `NewFU Lex` declarations
- `oap2/CompileCC.oap:233-237` — the 5 peer rows at Delphi's cap
- `oap2/BootstrapCC.oap` — the prepended preamble (NewFU + 33 LineAdds)
- `oap2/run.bat` — uses prepend in the compile step
- `tests/run_tests.ps1` — prepends BootstrapCC unconditionally
- `tests/run_ale.ps1` — prepends BootstrapCC in compile step
- `oap2/run_old.bat` — Delphi-side compile+run helper (no prepend; Delphi has its own MnemoTable)
- `IndFileConvertOld.cpp` — .ind format reference

## Build & test commands

```powershell
# Build C++ executor
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
    Millicom.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal

# Recompile CompileCC.oap → CompileCC.ind via Delphi
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
.\tests\run_ale.ps1                    # end-to-end ALE tests
```
