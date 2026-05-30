# Handoff: migrate the C++ `IpBuf*` emission layer to OAP-only

**Goal.** Make the compiler emit its `.ind` output using only inbuilt OAP FUs,
keeping **one** native millicommand (`IpBufWrite`, the serializer). Remove the
six bespoke emission mks ChatGPT added to `Bus.cpp`:

| Mk | Mnemo | Role |
|----|-------|------|
| 256 | `IpBufSetAtr` | set pending atr for next emit (`CapsPendingAtr=Load`) |
| 257 | `IpBufEmit` / `CapsEmit` | append `{atr,Load}` to `CapsEntries`; sub-cap open/close + MakeFU-on-close |
| 258 | `IpBufStashLoad` / `CapsStashLoad` | stash a typed value for next stashed emit; also captures FU name when `PendingFuNameMode` |
| 259 | `IpBufEmitStashed` / `CapsEmitStashed` | append with stashed value; live-dispatch iff `SubCapDepth==0`; honors `PendingMakeFU` |
| 281 | `IpBufPendingMakeFU` | flag: next stashed-emit also emits a MakeFU |
| 286 | `PendingFuNameModeSet` | flag: next stashed string is the user-FU name |

**Keep:** `IpBufWrite` (Mk 253; invoked from `Millicom.cpp` as `Bus.ProgFU(1253,{Cstring,&outFile})`).

This is the compiler's **core output path** — *every* test depends on it
(not just ALE). It must stay **60/60 grammar + 9/10 ALE** (`ale_09` is a
pre-existing sub-capsule bug; see below) throughout. Do it **parallel → verify
→ flip → delete**, test-gated.

---

## What lives where (grep — line numbers drift)

- `Bus.cpp`: cases `253,256,257,258,259,281,286` (search `case 253:` etc.). The
  serializer body (253) does sub-capsule grouping, DInd pointer encoding, and
  **atr-rebasing**.
- `Bus.h`: `struct CapsEntry { long atr; LoadPoint load; bool isMarker,
  isNewFuParent; long newFuType; }`, `vector<CapsEntry> CapsEntries`,
  `CapsPendingAtr`, `CapsStashLoad`, `SubCapOpenAtr=-1000`, `SubCapCloseAtr=-1001`,
  `SubCapDepth`, `PendingFuNameMode`/`PendingFuName`, `PendingMakeFU`/
  `PendingMakeFUType`, `UserFuRanges`/`NextReloadFuIdx`/`rebaseAtr()`.
- `oap2/CompileCC.oap`: emit sites use `Main_Bus.IpBufSetAtr`,
  `Main_Bus.IpBufStashLoad`, `Main_Bus.IpBufEmitStashed`, `Main_Bus.IpBufEmit`,
  `Main_Bus.IpBufPendingMakeFU`, `Main_Bus.PendingFuNameModeSet`. The BusMkList
  defs for these are in the `BusMkList` block; the IntAlu/List/Cons MkTables for
  *compiled programs* are `IntAluMkTable`/`ListMkTable`/`ConsMkTable`.

## The `.ind` text format (decoded — verified)

First line = IP count `N`. Then one line per IP: `atr TYPE:value f1 f2 f3 f4`.
- `atr` = rebased MK/atr code. `TYPE:value` ∈ `I:int`, `S:str"`, `B:T|F`, `D:dbl`.
- `f1` = trailing/sub pointer = `(targetIdx<<2)+flag`; **flag 2 = leaf** (points
  to self), **flag 0 = has sub-cap** (points to sub-cap's first IP).
- `f2` = `11` (constant IP kind).
- `f3` = next sibling = `(idx<<2)` or `-1`. `f4` = prev sibling = `(idx<<2)` or `-1`.
- A **MakeFU** IP serializes as `1001 I:<fuType>` (from `isNewFuParent`).

Examples (real output):
```
Cons.OutLn=5  ->   2
                   1001 I:1 2 11 4 -1      # NewFU Cons (type 1)
                   2002 I:5 6 11 -1 0      # Cons.OutLn(5), Cons@2000 + OutLn Mk2

Cons.OutLn={Cons.OutLn="x"}  ->  3
                   1001 I:1 2 11 4 -1
                   2002 I:0 8 11 -1 0      # parent; f1=8=(2<<2)+0 -> sub-cap at IP2
                   2002 S:x" 10 11 -1 -1   # sub-cap body (single IP)
```

## Data model (for the serializer)

- `Consts.h`: `class ip { long atr; LoadPoint Load; }`; `IC_type = vector<ip>*`;
  `ICVect = vector<vector<ip>*>`.
- `List.h`: `class List` has **`vector<IC_type> ListHead;` (public)** — its lines
  (each line is an IC = `vector<ip>`). Sub-capsules are `ip`s whose `Load` is an
  IC pointer (`IcPointType`).
- The compiler's `OAList` (FUListNew) is **scratch** — it only builds `>{...}`/
  sub-graph fragments (`LineAdd` used 3× in list/mnemo contexts) and is **never
  serialized**. The authoritative output is `CapsEntries`. So you must build a
  **new, full accumulator** in OAP; don't assume `OAList` already has it.

## The design (no new mks)

1. **OAP accumulator `CapsList`** — a dedicated `FUListNew`. Build it as a *flat*
   list of lines that mirror `CapsEntries` 1:1, so the existing serializer is
   reused unchanged:
   - each emitted IP → `CapsList.LineAdd` then set the line's atr
     (`MkCalc.OutMk=CapsList.LastAtrSet`) and load (`<value>.OutMk=CapsList.LastLoadSet`).
   - sub-cap open → a line with atr `-1000`; close → atr `-1001`.
   - MakeFU → a line with atr `1001` and load = FU type (so rebuild sets
     `isNewFuParent=true, newFuType=load`).
   - **sentinel**: bootstrap writes a first line with atr `-9999` so the C++ can
     find `CapsList` by scanning FUs (no registration mk needed).
2. **`IpBufWrite` (253)** — at the top, find the `CapsList` (scan `FUs` for a
   `List`* whose `ListHead[0][0].atr == -9999`), **rebuild `CapsEntries`** from
   its lines (skip the sentinel; `isMarker = atr∈{-1000,-1001}`,
   `isNewFuParent = atr==1001`), then run the **existing** serialization verbatim.
   (Needs `#include "List.h"` in `Bus.cpp`.)
3. **Side-effects move to OAP:**
   - **MakeFU**: call `Main_Bus.MakeFU` directly at the `NewFU` close (Mk 1 exists).
   - **FU-name capture** (replaces 286): capture the `Mnemo="..."` string into the
     OAList/MnemoTable row directly in OAP.
   - **live-dispatch** (the `SubCapDepth==0` branch of 259 that makes
     `Cons.OutLn=5` print at compile-time): re-dispatch the IP through the bus in
     OAP when not inside a `{...}` body.

## Ordered, test-gated steps

1. Add `NewFU={Mnemo="CapsList" FUType=FUListNew}` + bootstrap sentinel line
   (atr `-9999`). Add `findCapsList()` + the guarded rebuild in `IpBufWrite`
   (**guard: only use CapsList if it has >1 line**, else old path). Rebuild C++
   + recompile `CompileCC.ind`; confirm **still 60/60** (CapsList empty → no-op).
2. Migrate emit sites in `CompileCC.oap` to **also** populate `CapsList` (in
   parallel; `IpBuf*` still authoritative). Cover: simple assign (`EqVar` else),
   ALE result (`ALEAfter` end / `]`), sub-cap open/close (`EqualAfter {`,
   `MnemoLoad {`, list `>`), `NewFU` (MakeFU marker), and the `Atr=`/`Mk=` paths.
3. Flip `IpBufWrite` to read `CapsList`; **diff** its `.ind` vs the `IpBuf` one
   for every test until byte-identical (`run_tests.ps1` checks traces;
   `run_ale.ps1` checks runtime values — both must hold).
4. Move MakeFU / FU-name / live-dispatch to OAP; remove `Bus.cpp` cases
   256/257/258/259/281/286 + their `BusMkList` defs; rebuild; confirm green.

## Hard parts / gotchas

- **NewFU sub-cap structure.** `NewFU={Mnemo="X" FUType=Y}` emits `1001 I:Y` and
  *skips* the body fields. In `CapsEntries` this is `isNewFuParent` on a parent +
  a `-1000`/body/`-1001` group that the serializer's pre-pass (the `skip[]` loop)
  drops. Your `CapsList` must reproduce that exact shape, or move the skip logic.
- **atr-rebasing** (`UserFuRanges`, `rebaseAtr`): user FUs live-dispatch at high
  compile-time indices but reload at 2,3,…; the serializer rewrites their atrs.
  This is tied to MakeFU emission order — keep it in the serializer (it can
  recompute from the `1001` markers' order), or carry the ranges into OAP.
- **live-dispatch timing**: `Cons.OutLn=5` must still print during compile (the
  `--lex-file` runtime mirror). That's the `SubCapDepth==0` ProgFU in case 259.
- **`SubCapDepth` / marker discipline**: `-1000`/`-1001` lines must nest exactly
  as today, or sub-caps mis-group.

## Pre-existing bug (do NOT attribute to this work)

`ale_09_subic_mixed` fails identically on the committed HEAD compiler: (1) a bare
`Atr=99` inside a `Cons.OutLn={...}` sub-IC leaks a spurious `-8 # 99`, and
(2) a sub-IC nested inside an `Atr={...}` subgraph loses its body
(`boxed-in-subgraph`). It's a sub-capsule / `{`-path-distinction bug, unrelated
to emission-layer migration.

## Build & test

```powershell
# rebuild C++ executor (after Bus.cpp/.h changes)
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
    Millicom.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal
# recompile CompileCC.oap -> CompileCC.ind (after CompileCC.oap changes)
Push-Location "E:\miemp\Millicom\Millicom"
Start-Process -FilePath "E:\miemp\Millicom\Millicom\millicom.exe" `
  -ArgumentList @('--bootstrap','C:\Users\hacker\Downloads\millicom\oap2\CompileCC.oap',
    '--run','C:\Users\hacker\Downloads\millicom\tests\empty.oap',
    '--compile-to','C:\Users\hacker\Downloads\millicom\oap2\CompileCC.ind',
    '--exit-delay','9000','--exit') `
  -WorkingDirectory "E:\miemp\Millicom\Millicom" -WindowStyle Hidden -Wait
Pop-Location
# tests
.\tests\run_tests.ps1          # grammar/trace, expect 60/60
.\tests\run_ale.ps1            # ALE runtime values, expect 9/10 (ale_09 pre-existing)
```

_Context: the ALE evaluator was already migrated to pure OAP this session
(two-accumulator + `AleResult`/`AleOp`/`AleTmp` IntAlus, parens via IntAlu
`Push`/`Pop` + `ReceiverPush`/`Pop` + synthetic `AleParenVal` token). All
`Ale*` mks (Bus.cpp 270–275) are gone. The `IpBuf*` suite is the last native
emission machinery._
