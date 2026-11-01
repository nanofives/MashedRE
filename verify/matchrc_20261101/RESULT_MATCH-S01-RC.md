# RESULT MATCH-S01-RC / MATCH-S03-RC / MATCH-S04-RC (2026-11-01): C4 route match under L-68, tracker text only

Branch `orch/MATCH-S01-RC` off `orch/integration` @ 0e3d71a75, worktree `.worktrees/MATCH-S01-RC`. One worker for the
three ledger rows. **TEXT-ONLY**: no source, swap, build input or `.ps1`/`.bat` changed, so the default gates are
untouched. No game run, no Ghidra.

## Verdict

| row | swapped RVAs in scope | C4 by `match` | exe-callers=0 (stay, L-68 note) | exe caller but held below C4 |
|---|---|---|---|---|
| MATCH-S01-RC | 17 (18 in `swaps.tsv` minus 0x00408a50, HARNESS lead) | **8** (5 promoted C3->C4, 3 already C4 gain the route) | **8** | 1 (0x00408ad0, U-4200) |
| MATCH-S03-RC | 8 | **2** (C3->C4) | **5** | 1 (0x00414180, S-3669) |
| MATCH-S04-RC | 22 (swapped in both targets) | **3** (C3->C4) | **19** | 0 |

Not in scope and untouched: 0x00407600 (not swapped, waits on L-69), 0x00408a50 (HARNESS lead), S03's
0x00414060 / 0x00418860 / 0x004148b0 (matched only, not swapped), S04's 31 asi-only swaps.

## Method (all three conditions of `re/CONFIDENCE.md` "C4 routes by byte match")

1. **Byte identity + relocheck, re-run at this tip.** `MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/match.py <src> --bytes`
   (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87) and `re/tools/match/relocheck.py <src>` on every
   source of the 47 RVAs: all BYTE-IDENTICAL over the whole symbol, all relocheck OK
   (`match_rerun.txt`, `match_callerless.txt`). 0x00404ee0 scores 89.3% by instruction but is BYTE-IDENTICAL over the
   99 B symbol (Ghidra's size is short), as in MATCH-S01. The S03 sources declare globals as plain `extern DAT_<va8>`
   (no `MGLOBAL`), so stock relocheck lists them unchecked (0x0041ae20: `_DAT_005f3304`; 0x0041b520: `_DAT_0063c8d0`;
   0x00414180: 8 DAT names). `relocheck_dat.py` (stock relocheck plus one rule: `DAT_<va8>` = `MGLOBAL(DAT_<va8>,
   0x<va8>)`, the address the port binds) checks each: 0x0041ae20 OK 1/0, 0x0041b520 OK 3/0, 0x00414180 OK 11 checked,
   only `__ftol` x2 unchecked (CRT, by name). Every S03 and S04 line in `match_callerless.txt` comes from it.
2. **One body per RVA, both targets.** `scripts/rva_body_scan.py` (`scan_all` / `rva_to_tus`): each of the 47 RVAs has
   exactly one body TU in the exe and the same TU in the .asi (table below). `py -3.12 scripts/lint_rva_bodies.py`:
   `48 known, NEW=0`; none of the C4 RVAs is in `re/tools/dual_copy_allowlist.txt`.
3. **Exe callers (L-68), static.** `exe_callers.py` walks every TU of `mashedmod/exe_sources.rsp` and its quoted
   includes, strips comments and strings, evaluates `#if/#ifdef/#ifndef` with `MASHED_STANDALONE` defined plus the
   TU's own `#define`s, and lists each active reference to the body's names (port symbol, install entry, `FUN_<va8>`
   alias). Output `exe_callers_hits.txt`. Each hit was then read by hand: definitions, prototypes, `#define`,
   `#pragma comment(linker, /EXPORT)` and `RH_ScopedInstall` are not callers. `raw_refs.py` lists raw-address
   references and names in string literals (`raw_refs.txt`): raw `reinterpret_cast<...>(0x0041xxxxu)` calls in the exe
   target the original image address, not the swapped body (the exe installs RVA thunks only at 0x0042b8b0,
   0x0042b8c0, 0x0042d5a0, 0x00472f40, 0x004730b0, 0x00472c60, `exe_main.cpp:9651-9656`), so they are not callers.
   For every caller found, the enclosing function's own exe caller was traced until a known-live root (WinMain,
   `TrackRenderer::UpdateCar` from `exe_main.cpp:3894`, the race render path). A caller whose own caller count is 0
   (dead chain) does not count.
4. **Gates.** Unchanged at the merge of each slice (`roadmap.ps1 get`): MATCH-S01 train 20261031_160356, MATCH-S03
   train 20261031_142553, MATCH-S04 train 20261031_150246, all G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3,
   equal to `merge_train.json` refs today. This branch changes no build input, so the gates still hold.
5. **re-classify checks.** Open STUBS rows with the RVA as *caller*, open UNCERTAINTIES rows whose Blocks names the RVA.

## Table: RVA -> exe caller -> verdict

Port body = the one TU in both targets. Paths under `mashedmod/src/mashed_re/`.

### MATCH-S01-RC

| rva | port body | exe caller (file:line, chain) | verdict |
|---|---|---|---|
| 0x00402f50 | Match/MatchS01.cpp | exe_main.cpp:9225 `BootDefaultParamsInit()` in ExecuteFrontendBootChain <- WinMain exe_main.cpp:9538 | **C3->C4 match** |
| 0x00404e00 | Vehicle/CarMaterialTables.cpp | LibRw/RwCarFx.cpp:34 CarDirtBodyName <- D3d9Render/TrackRenderer.cpp:4185 | **C3->C4 match** |
| 0x00404e20 | Vehicle/CarMaterialTables.cpp | LibRw/RwCarFx.cpp:40 CarDirtWheelName <- TrackRenderer.cpp:4186 | **C3->C4 match** |
| 0x00404e50 | Save/GameSave.cpp | Race/GameFlow.cpp:205 ReadSaveImage <- GameFlow.cpp:283/454 (SaveProgress, Campaign_LoadProgress <- exe_main.cpp:9979) | **C4 kept, + match route** |
| 0x00404f50 | Save/GameSave.cpp | Race/GameFlow.cpp:200 WriteSaveImage <- GameFlow.cpp:295/314 (SaveProgress) | **C4 kept, + match route** |
| 0x00404f80 | Save/GameSave.cpp | Race/GameFlow.cpp:204 ReadSaveImage (as above) | **C4 kept, + match route** |
| 0x00405890 | Match/MatchS01.cpp | Gameplay/PickupCollect_naked.inc:25 `call Pred405890` in PK_458920 <- Gameplay/PickupTick.cpp:327/402 (Pickups_Callback <- Race/WorldRegistry.cpp:79, PickupCb_MaybeReplay <- TrackRenderer.cpp:3254) | **C3->C4 match** |
| 0x00409290 | Match/MatchS01.cpp | Race/RoundGrid.cpp:116/214/307 (RoundGrid_Anchor <- TrackRenderer.cpp:7529, GridDir_Anchor <- TrackRenderer.cpp:4959, RoundGrid_MaybeReplay <- TrackRenderer.cpp:3251) | **C3->C4 match** |
| 0x00408ad0 | Match/MatchS01.cpp | D3d9Render/TrackRenderer.cpp:7815 (RacePct) and :8242 (FillRaceCamCars) | **held at C2**: open U-4200 (structural, Blocks: `C3 of 0x00408ad0`) |
| 0x00401340 | Match/MatchS01.cpp | none | exe-callers=0, stays C3 |
| 0x00401570 | Match/MatchS01.cpp | only from 0x00401ee0's body (re/match/s01/00401570.c:34), itself callerless; Frontend/MenuLeaves_af4.cpp:43 is a raw-address call | exe-callers=0, stays C3 |
| 0x00401ee0 | Match/MatchS01.cpp | none | exe-callers=0, stays C3 |
| 0x004026d0 | Match/MatchS01.cpp | none | exe-callers=0, stays C4 on the frida route only |
| 0x00402f80 | Match/MatchS01.cpp | none | exe-callers=0, stays C3 |
| 0x00404e80 | Save/GameSaveBuffer.cpp | none (build.bat:195: nothing calls Serialize/DeserializeFromBuffer by name) | exe-callers=0, stays C4 on the frida route only |
| 0x00404ee0 | Save/GameSaveBuffer.cpp | none | exe-callers=0, stays C4 on the frida route only |
| 0x00407a60 | Match/MatchS01.cpp | none | exe-callers=0, stays C3 |

### MATCH-S03-RC

| rva | port body | exe caller | verdict |
|---|---|---|---|
| 0x0041b520 | Race/SubStateCallees.cpp | Race/SubStateMachine.cpp:157 `Update41b520()` in SubStateEnter4 <- TrackRenderer.cpp:5306 (UpdateCar) | **C3->C4 match** |
| 0x0041ae20 | Race/SubStateCallees.cpp | re/match/s03/h1_0041ae20.c:32, inside 0x0041b520's body (same TU, live chain above) | **C3->C4 match** |
| 0x00414180 | Race/SubStateMachine.cpp | TrackRenderer.cpp:5260 `SubStateRollCountdown414180()` -> SubStateMachine.cpp:384 | **held at C3**: open stub S-3669 has 0x00414180 as caller; the .asi binds `FUN_00472650` to a raw call of 0x00472650 (SubStateMachine.cpp `#else` arm), the exe to `::Ai::Ai_RandRange` |
| 0x00417730 | Vehicle/VehicleState.cpp | none | exe-callers=0, stays C3 |
| 0x0041ad60 | Render/ParticleEmitterCtors.cpp | none (naked entry ParticleEmitterCtorA has no exe caller) | exe-callers=0, stays C3 |
| 0x00415020 | HUD/ScenarioWriters_sa2s2.cpp | none | exe-callers=0, stays C3 |
| 0x00413bc0 | HUD/ScenarioLeaves_sa1.cpp | none (naked entry HudUvRect413bc0 has no exe caller) | exe-callers=0, stays C3 |
| 0x00414120 | Frontend/SmallLeaves_t1.cpp | none | exe-callers=0, stays C3 |

### MATCH-S04-RC

| rva | port body | exe caller | verdict |
|---|---|---|---|
| 0x0041d820 | Match/MatchS04_0041d820.cpp | exe_main.cpp:9274 `TimerSlotClear()` in ExecuteFrontendBootChain <- WinMain | **C3->C4 match** |
| 0x0041d910 | Match/MatchS04_0041d910.cpp | Race/SubStateMachine.cpp:125 `Clear63d584Pair()` in SubStateEnter4 <- TrackRenderer.cpp:5306 | **C3->C4 match** |
| 0x0041da90 | Match/MatchS04_0041da90.cpp | Race/SubStateMachine.cpp:245 `DeltaTimeOutGet()` in SubStateHeldCountdown5_C <- TrackRenderer.cpp:5308 | **C3->C4 match** |
| 0x0041c2d0 | Match/MatchS04_0041c2d0.cpp | HUD/HudDispatch.cpp:243 `call FUN_0041c2d0` in HudDispatchMode10, which has no exe caller; 0x0041c300's body (callerless) | exe-callers=0, stays C4 on the frida route only |
| 0x0041b770, 0x0041c010 | Match/MatchS04_0041b770.cpp | none (0x0041b770 only from 0x0041c010's body) | exe-callers=0, stay C3 |
| 0x0041bc50, 0x0041c0c0 | Match/MatchS04_0041bc50.cpp | none (MatchS02_0040dfc0.cpp:34 is a raw-address call) | exe-callers=0, stay C3 |
| 0x0041c300 | Match/MatchS04_0041c2d0.cpp | none (MatchS02_0040dfc0.cpp:37 raw address) | exe-callers=0, stays C3 |
| 0x0041c320, 0x0041cb10 | Match/MatchS04_0041c320.cpp | none | exe-callers=0, stay C3 / C2 |
| 0x0041c9a0, 0x0041ccc0 | Match/MatchS04_0041c9a0.cpp | none (MatchS02_0040dfc0.cpp:40 raw address) | exe-callers=0, stay C2 / C3 |
| 0x0041cbc0 | Match/MatchS04_0041cbc0.cpp | none | exe-callers=0, stays C3 |
| 0x0041cd20, 0x0041d6e0 | Match/MatchS04_0041cd20.cpp | none | exe-callers=0, stay C3 / C2 |
| 0x0041e080 | Match/MatchS04_0041e080.cpp | none (SubStateMachine.cpp:131 is `#else`, .asi only; the exe arm is `ExeOnce`) | exe-callers=0, stays C3 |
| 0x0041e130, 0x0041eda0, 0x0041f000 | Match/MatchS04_<rva>.cpp | none | exe-callers=0, stay C3 |
| 0x0041f090, 0x0041f1c0 | Match/MatchS04_<rva>.cpp | none | exe-callers=0, stay C3 |

## Notes on the C4 rows

- Callees in the C4 bodies bind to port functions in the exe: 0x00404e50/f50/f80 to `gFileRead` / `gFileWrite` /
  `gFileExists` / `SaveStatusClear` (Save/GameSave.cpp:170-193, 250-253), 0x00409290 to `GetDat0066d6e0` /
  `GetTableEntry0066d658` (Frontend/GlobalGetters_s5.cpp). The others have no callees.
- x87: `Match/MatchS01.cpp` and `MatchS04_0041d910` are in `x87_tus.txt`. `Race/SubStateCallees.cpp` (0x0041ae20 /
  0x0041b520) and the save, car-table and other S04 TUs are not physics TUs. 0x0041ae20's only float operation is a
  doubling of a stored float.
- `hooks.csv` `file` and `exe_file` of all 47 rows now name the port body TU above.
- STUBS rows that name a C4 RVA as the stubbed *callee* in another function (S-3580/S-3581 in 0x0040cea0, S-1861 in
  0x00410510, S-3652/S-3649 in 0x0040dbd0, S-2605 in 0x004103a0) concern those callers' bodies and were not touched.

## Unblock paths for the two held rows (not done here: they change source or need a ruling)

- **0x00414180** (S-3669): bind the .asi arm of `FUN_00472650` to the same port RandRange the exe uses, or close
  S-3669 if the lead rules that a raw call to the original inside MASHED.exe is not a stub. Exact change, in
  `mashedmod/src/mashed_re/Race/SubStateMachine.cpp`, `#else` arm of the MATCH-S03 block:
  `-#define FUN_00472650 (reinterpret_cast<float(__cdecl*)(float, float)>(0x00472650u))`
  `+#define FUN_00472650 ::Ai::Ai_RandRange` (needs `Ai/AiStandalone.h` in the .asi build and a gate run; the .asi
  side then no longer calls the original PRNG, so `hook_diff` evidence for 0x00414180 must be re-checked).
- **0x00408ad0** (U-4200): U-4200 asks what the float at DAT_008a96ec+idx*0x30c means. Byte identity does not answer
  it. Either resolve U-4200 (`uncertainty_resolve.py`) or a rubric ruling that a `match` C4 does not need structural
  U-rows that block C3 to be closed.

## Files

`exe_callers.py.txt`, `raw_refs.py.txt`, `relocheck_dat.py.txt` (measurement tools; stored as `.txt` so the merge
train's text-only check does not count them as build inputs; copy to `.py` and run from the repo root:
`py -3.12 exe_callers.py . <names>`, `py -3.12 raw_refs.py . rvas.txt <names file>`,
`py -3.12 relocheck_dat.py re/tools/match <src.c>`),
`exe_callers_hits.txt`, `raw_refs.txt`, `match_rerun.txt` (C4 candidates), `match_callerless.txt` (the rest),
`rvas.txt` (the 47 RVAs).
