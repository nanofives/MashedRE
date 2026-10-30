# RESULT D4-DC-77b0-WRITERS (static, writer side of VALSRC rows 20 and 23)

Verdict: GREEN. Two writer tables, every row cited by RVA, and one verdict per VALSRC row on whether the .asi body
(A = `Ai/AiPreTick.cpp`) and the exe `AiPreTickRubberBand` (E = `Ai/AiStandalone.cpp:1682`) can see different values.
Both verdicts are "yes, on named frame classes", each with the runtime evidence that would close it. Static only: no build,
no game run, no test slot, no Frida. No `mashedmod/` change, no `.ps1` change, no tracker change.

Base: `orch/integration` @ `009a74b2d`. Parent rows: `verify/d4dc77b0valsrc_20261030/RESULT_D4-DC-77b0-VALSRC.md` rows 20
(p4 type, 0x0040e470) and 23 (p5 sub mode, 0x0040e350).

## Sources of evidence

- `re/tools/decomp_pc.py` against pool clone `Mashed_pool0` (read-only, acquired and released by the tool): decomp +
  callers of the 20 functions in both tables, `--callers` on 0x004177b0 / 0x00425a40 / 0x00410d10 / 0x0040fc00 /
  0x00492d30 / 0x0042c960 / 0x00430290 / 0x0043d2a0 / 0x004030d0.
- capstone linear disassembly of `original/MASHED.exe.unpatched` (SHA-256
  `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, the version anchor): a byte scan of `.text` for every
  `E8`/`E9` rel32 whose target is 0x0040e480 or 0x0040e360 (41 + 5 sites, each confirmed by an aligned decode that lands
  on the call), the argument pushes before each, and the context of every 0x0063ba8c store.
- `.data` reads of the anchor image: 0x005f2770 = 0x005f2728, 0x005f275c..0x005f2768 = 0,0,0,0 (raw `.data`, no
  initialiser other than 0), 0x005f29c0 = 1, 0x0063ba8c lies in BSS (no file bytes, so 0 at load).
- Port side read at `009a74b2d`: `Ai/AiPreTick.cpp:73-74`, `Ai/AiStandalone.cpp:2087-2129`,
  `D3d9Render/TrackRenderer.cpp:141-197, 597, 636-644, 5319-5360, 5467, 7733, 8970`, `Race/SubStateMachine.cpp:133-158,
  175-180, 198-200, 280`, `Race/RuleEngine.cpp:148`, `Race/ScoringHooks.cpp:544-548`, `Core/FaithDefaults.cpp:51, 66`.

Raw dumps (not committed), `D:/MashedWT/D4-DC-77b0-WRITERS_data/`:

| file | SHA-256 |
|---|---|
| scan_calls.py | cac4d2dcb0a4e5803287f490f442001c801a6cbf03bb30bb26bfc9eda93d8680 |
| scan_calls.txt | db6b5cbd287a756a5830592b40a9e36d0e1e445fd77a3c3b3bdd89e595407476 |
| wdis.py | 3d365b01bdf972701176f76b9b3f521f82740adc42f0d3cac880defd5a6126ea |
| dis_411170.txt | e935b299fa2c5b6d26787e4a114888db385bd81e9f767fd3380b0e4c7657b433 |
| dis_ctx.txt | 7319e5b7b3f3c03a13318c63d2bb779e0567ccb5ca5d59a127b55d97722c4a7a |
| decomp_writers.txt | 7a01f2c26ff36ceca7e165a71948f590b01341d8ef4fd9d9ec1293027084af7f |
| callers2.txt | aac53ae3da81a82ea74d2fd6030742aa79d84fd5a45b57ab044cfdf874f7f74c |

## Port-side premise (changed since VALSRC)

VALSRC rows 20 and 23 describe both port knobs as default OFF. At `009a74b2d` both are default ON in the exe:
`Core/FaithDefaults.cpp:51` seeds `MASHED_SLOT_TYPE_LIVE=1` (fflip 2026-10-21) and `:66` seeds `MASHED_SUBSTATE=1`
(D3-SUBSTATE-FLIP 2026-10-29). So in the default exe:

- E's type read is `aib_veh_type` (`TrackRenderer.cpp:149-155`) = the live cell `*(*(0x005f2770) + 0x34 + 4v)`, the same
  cell A reads through 0x0040e470 (`AiPreTick.cpp:73`). The exe seeds 0x005f2770 = 0x005f2728 (`TrackRenderer.cpp:597`).
- E's sub-mode read is `aib_game_sub_mode` (`TrackRenderer.cpp:191-197`): 7 while `s_roundEnd7` is up, else the exe's own
  0x0063ba8c when non-zero, else 6. A reads the original's live 0x0063ba8c through 0x0040e350 (`AiPreTick.cpp:74`).

## Where FUN_004177b0 runs in the original (needed for "race path")

Static call chain, from the decomp and callers lists:

- FUN_004111c0 is a `switch (DAT_0063ba8c)` over cases 1..0xb (decomp of 0x004111c0; Ghidra's body includes the
  non-contiguous case blocks at 0x0040dd80.., 0x0040fe90.., 0x004100c4.., 0x0041017c.., 0x00410ad0..). Its callers are
  FUN_0042c960 and FUN_00492d30. The C1 plate on 0x004111c0 calls it the "top-level race state-machine tick". That it runs
  once per frame is plate text, not shown here: [UNCERTAIN] missing a per-frame call count of 0x004111c0.
- Case bodies: 1 = setup then `= 2` (0x004100dd). 2 = setup tail then `= 3` / `= 4`. 3 = FUN_004102f0 + FUN_0040fc00.
  4 = FUN_0040dbd0 + FUN_0040fc00. 5 = FUN_004103a0 + FUN_0040fc00. 6 = FUN_00411170 + FUN_0040fc00. 7 = FUN_00410860 +
  FUN_0040fc00. 8 = FUN_00414220, FUN_00426c70, FUN_0045bed0, `= 2` (0x0040dda3). 9 = FUN_0040ddb0, `= 0xa` (0x0040ddf5).
  0xa = FUN_004117b0 thunk + FUN_0040fc00. 0xb = FUN_0040fc00, then the timer test that may store 9 (0x00410b02).
- FUN_004177b0's only caller is FUN_00418860. FUN_00418860's callers are FUN_004103a0 (case 5) and FUN_00425a40.
  FUN_00425a40's only caller is FUN_0040fc00, which calls it when `DAT_005f29c0 != 0` (0x005f29c0 is 1 in the image,
  toggled by FUN_00496930(6)/(5) in FUN_0040fc00). FUN_00425a40 returns before FUN_00418860 only when
  `DAT_007f1a50 == 1` and (`DAT_007f1a54 != 4` or `DAT_007f1a74 == 0 && DAT_007f1a70 == 0`).
- So FUN_004177b0 is statically reachable in cases 3, 4, 5, 6, 7, 0xa, 0xb (through FUN_0040fc00) and a second time per
  case-5 call (through FUN_004103a0's own FUN_00418860 call). It is not reachable in cases 1, 2, 8, 9.
- The sub-mode it reads is the value after that frame's case handler: case 3 can hand it 4 (0x00410387), case 4 always
  hands it 5 (all four exits of FUN_0040dbd0 store 5), case 5 hands its FUN_0040fc00 call 6 once 0x004104e3 fires (its own
  inner call, earlier in FUN_004103a0, still sees 5), case 6 can hand it 7 (0x004111a6) or 0xb (0x00410645), case 7 can
  hand it 9 (0x00410a5e) or 8 (0x00410a6e). Case 0xb stores 9 after its FUN_0040fc00 call.

"Race path" below = reached from FUN_004111c0's cases. "Frontend" = reached only from the menu side (FUN_00492d30 ->
FUN_0043dfd0 / FUN_0043d7c0, FUN_004030d0 -> FUN_0043df00, FUN_0043d2a0 -> FUN_00432800). hooks.csv `subsystem` column
is cited for each function where it has a row.

## Table 1: VALSRC row 20, writer FUN_0040e480 (`*(*(0x005f2770) + 0x34 + 4*a1) = a2`)

cdecl, so the first push is a2 and the second a1. The decomp column gives what Ghidra resolved for register arguments.

| caller (hooks.csv) | call sites | (a1, a2) stored | path |
|---|---|---|---|
| FUN_004111c0 (util, C2) | 0x0040fec3 / 0x0040fecc / 0x0040fed5 / 0x0040fedd | (0,2) (1,2) (2,2) (3,0): pushes `6a02 53`, `6a02 6a01`, `6a02 6a02`, `53 6a03` with EBX = 0 from `xor ebx,ebx` at 0x0040feb9. Guarded by `FUN_0042f6a0() == 0xb` (0x0040feb4 call, 0x0040febb `cmp eax,0xb`, 0x0040febe `jne`) | race path, case 1 only (one dispatcher call per race start) |
| FUN_00418860 (ai, C2) | 0x004188c0 / 0x004188db / 0x004188f6 | (1,2) (2,2) (3,2), each only when `FUN_0046c7b0(v) == 1` (0x004188b0 / 0x004188ca / 0x004188e5). At 0x004188bf a1 is `push eax` with EAX == 1 from the `cmp eax,1 / jne` at 0x004188b8. Outer gate: `DAT_00801ca0 > 3` and (`FUN_0042f6a0()` in {3,4,5,10} or `DAT_007f0fd0` in {4,8,9,10}) | race path, every FUN_004177b0 call: the three stores precede the FUN_004177b0 call in the same function |
| FUN_0042b960 (frontend, C3, CarSlotInit1P) | 0x0042b9b0 / 0x0042b9b9 / 0x0042b9c2 / 0x0042b9cb | (0,1) (1,0) (2,0) (3,0): pushes `6a01 6a00` at 0x0042b98d/0x0042b98f, then `6a00 6a01`, `6a00 6a02`, `6a00 6a03` | frontend (sole caller FUN_0043dfd0) |
| FUN_0042b9e0 (frontend, C3) | 0x0042b9f3 (loop), 0x0042bafa (loop) | (v,0) for v = 0..3 (`push 0 / push esi`, ESI from `xor esi,esi` 0x0042b9e8, loop); (local_4,1) at 0x0042bafa: a1 = ECX, the loop slot index (decomp `FUN_0040e480(local_4,1)`), value 1 | frontend (sole caller FUN_0043dfd0) |
| FUN_00432800 (frontend, C2) | 0x004328f7 (loop), 0x00432932 (loop) | (v, EBP) for v = 0..3 (EDI from `xor edi,edi` 0x004328f3 / 0x00432927). Decomp shows `FUN_0040e480(iVar2,0)` at both, EBP stored to DAT_007f0fe8 at 0x004328ed where the decomp has `DAT_007f0fe8 = 0`: value 0 | frontend (callers FUN_0043d2a0, FUN_0043dfd0) |
| FUN_0043df00 (frontend, C2) | 0x0043df74 / 0x0043df7d / 0x0043df86 / 0x0043df8e | (0,2) (1,2) (2,2) (3,ESI): decomp `FUN_0040e480(3,0)`, value 0 | frontend (sole caller FUN_004030d0) |
| FUN_0043dfd0 (frontend, C2) | 22 sites, 0x0043e8ac..0x0043f8dd | 3 blocks of (0,1) (1,0) (2,0) (3,0) (0x0043e8ac..c4, 0x0043ea0d..25, 0x0043ea8b..a3; decomp lines 419-422, 467-470, 485-488 of its body); (i,2) at 0x0043eb6f, 0x0043ec91, 0x0043ee1f, 0x0043ef73, 0x0043f0a4, 0x0043f678 (`push 2` each, a1 a loop register); (EBP,ESI) at 0x0043f807 = decomp `(iVar8,1)`; (EBP,0) at 0x0043f826; (EBP,2) at 0x0043f89a; (EBP,1) at 0x0043f8dd. Every a2 is in {0,1,2}. a1 at the loop sites is a loop index: [UNCERTAIN] per-site slot, missing a trace of the menu path taken | frontend (sole caller FUN_00492d30) |

No other writer exists: `--datarefs` on 0x005f275c lists 0 writes (VALSRC `datarefs2.txt`), and 0x005f2770 has no write
reference, so every store into 0x005f275c..0x005f2768 goes through FUN_0040e480.

Port writers of the same cells (default exe): `TrackRenderer.cpp:640-643` (0,1) (1,0) (2,0) (3,0) at race load, a bridge
standing in for FUN_0042b960; `Ai_CountdownAlivePoke` (`AiStandalone.cpp:2090-2101`, called at `TrackRenderer.cpp:5467`)
(v,2) for v = 1..3 when `VehicleSlotGetter(v) == 1`; `Ai_Standalone_Tick` (`AiStandalone.cpp:2121-2126`) (v,2) for
v = 1..3 when `s_host.car_alive(v) == 1`, before the `AiPreTickRubberBand` call at `:2129`. No port counterpart of the
case-1 block at 0x0040fec3..0x0040fedd was found in a grep of `mashedmod/src` for `0x0040fec` / `0x0040feb`.

**Verdict row 20: yes, for car 0 only.**
- v1..v3: no, on any AiPreTick call after the first alive-poke. Both sides store (v,2) for each alive AI car ahead of the
  pre-tick in the same function (original 0x004188b0..0x004188f6 before its FUN_004177b0 call; exe `:2121-2129`), and no
  race-path writer stores any other value to v1..v3 (Table 1). [UNCERTAIN] for the frames before each side's first poke:
  E's in-tick poke tests `g_aib.alive`, which is 0 pre-GO, and relies on `Ai_CountdownAlivePoke` there. Missing: a per-call
  dump of `*(0x005f2728 + 0x38..0x40)` at FUN_004177b0 entry (original) and at `AiPreTickRubberBand` entry (exe) through
  the countdown.
- v0: yes. No race-path writer stores v0 except case 1 under `FUN_0042f6a0() == 0xb` (stores 2, 0x0040fec3), which has
  no port counterpart. Otherwise v0 keeps the last frontend store: 1 from FUN_0042b960 or the three FUN_0043dfd0 blocks,
  0 from the (v,0) loops of FUN_0042b9e0 / FUN_00432800, 2 from FUN_0043df00's (0,2). The exe always
  starts the race with v0 = 1. The original was measured at 1 on 581/672 and 0 on 91/672 samples
  (`verify/d3_gatefire_20261008/RESULT_E470.md:16`, transitions not characterised). Missing: a read of 0x005f275c at
  every FUN_004177b0 entry in the canonical scenario, with the round type (`DAT_0067e9fc`, FUN_0042f6a0) logged.

## Table 2: VALSRC row 23, writers of 0x0063ba8c

19 immediate stores plus the store inside the setter FUN_0040e360 (0x0040e364 `mov [0x0063ba8c],eax`) = the 20 WRITE
references of `--datarefs` (VALSRC `datarefs.txt`). "Per frame" = the store sits in a case handler that runs on every
dispatcher call while the state holds; the store itself is conditional as shown.

| # | store | value | in function | reached from | runs per frame in a race? |
|---|---|---|---|---|---|
| 1 | 0x0040d3e7 | 1 | FUN_0040d270 (track, C2) | caller FUN_0040d440 | no case handler. [UNCERTAIN] frequency: FUN_0040d440's callers not traced |
| 2 | 0x0040dbf5 | 5 | FUN_0040dbd0 (util, C3) | case 4 | once: every exit of FUN_0040dbd0 stores 5, so state 4 lasts one dispatcher call |
| 3 | 0x0040dc17 | 5 | FUN_0040dbd0 | case 4 | as #2 |
| 4 | 0x0040dc30 | 5 | FUN_0040dbd0 | case 4 | as #2 |
| 5 | 0x0040dc40 | 5 | FUN_0040dbd0 | case 4 | as #2 |
| 6 | 0x0040dda3 | 2 | FUN_004111c0 case 8 block | case 8 | once (unconditional end of case 8) |
| 7 | 0x0040ddf5 | 0xa | FUN_004111c0 case 9 block | case 9 | once (unconditional end of case 9) |
| 8 | 0x0040de1a | 1 | FUN_0040de10 (util, C2) | FUN_0043d7c0 (3 calls) <- FUN_00492d30 | no: frontend |
| 9 | 0x0040e364 | arg | FUN_0040e360 (util, C3, setter) | 5 call sites, below | see 9a..9e |
| 9a | call 0x0040e57c | 7 (`6a07` 0x0040e57a) | FUN_0040e560 | FUN_004103a0 (case 5) when `FUN_00429860() != 0`, `FUN_00430820() == 0`, `FUN_0042f6a0() == 2` | per frame of state 5, gated on round type 2 |
| 9b | call 0x00429537 | 7 (`6a07` 0x00429535) | FUN_00429310 (util, C2) | FUN_0040fc00, every case that calls it | per frame, stores only when `DAT_008991bc == 0xb && FUN_0040e350() == 6` |
| 9c | call 0x0043dd66 | 1 (ESI = 1, 0x0043dd5a) | FUN_0043d7c0 | FUN_00492d30 | no: frontend |
| 9d | call 0x0043dd93 | 1 (`push esi` 0x0043dd8c; the decomp shows both FUN_0043d7c0 calls as `FUN_0040e360(1)`, body lines 187 and 299) | FUN_0043d7c0 | FUN_00492d30 | no: frontend |
| 9e | call 0x0043f982 | 1 (ESI = 1, 0x0043f976) | FUN_0043dfd0 | FUN_00492d30 | no: frontend |
| 10 | 0x004100dd | 2 | FUN_004111c0 case 1 block | case 1 | once (unconditional end of case 1) |
| 11 | 0x00410279 | 3 | FUN_004111c0 case 2 tail | case 2 | once, when `DAT_005f29b8 == 100000` (0x0041026d) |
| 12 | 0x00410287 | 4 | FUN_004111c0 case 2 tail | case 2 | once, the else arm (DAT_005f29b8 = 12000 follows) |
| 13 | 0x004102ac | 4 | FUN_004111c0 case 2 tail | case 2 | once, when `DAT_007f0fd0 == 6` (0x004102a3) |
| 14 | 0x00410387 | 4 | FUN_004102f0 (util, C2) | case 3 | per frame of state 3, stores when `FUN_004430b0() == 0` (0x0041037e) on the exit test |
| 15 | 0x004104e3 | 6 | FUN_004103a0 (util, C3) | case 5 | per frame of state 5, stores when `FUN_0041da90` result `>= _DAT_005ccdf4` |
| 16 | 0x00410645 | 0xb | FUN_00410510 (save, C2) | FUN_00411170 (case 6) at 0x00411198, only after `FUN_00410d10() != 0` (0x0041118f / 0x00411196) | per frame of state 6, stores on the LAB_0041062a conclusion |
| 17 | 0x00410a5e | 9 | FUN_00410860 (util, C2) | case 7 | per frame of state 7, stores when `199 < DAT_005f29b8`, the end test passes, and the points scan gives a slot |
| 18 | 0x00410a6e | 8 | FUN_00410860 | case 7 | as #17 with no slot |
| 19 | 0x00410b02 | 9 | FUN_004111c0 case 0xb block | case 0xb | per frame of state 0xb, stores when `_DAT_0063b91c >= _DAT_005cc574` and the slot byte test at 0x00410af8 passes |
| 20 | 0x004111a6 | 7 | FUN_00411170 (vehicle, C3) | case 6 | per frame of state 6, stores when `FUN_00410d10() != 0` and `FUN_00410510() == 0` (0x0041119f `je`); `FUN_00410d10() == 0` returns at 0x004111b5 with the state left at 6 |

Every stored value is in 1..0xb. No original writer stores 0.

Port writers of the exe's 0x0063ba8c (default exe): `TrackRenderer.cpp:7733` stores 0 at every round start (a value no
original writer stores); the countdown block `TrackRenderer.cpp:5319-5360` stores 3 / 4 (0x00410279 / 0x00410287 /
0x00410367 analogues); `SubStateMachine.cpp:133-158` stores 5 (FUN_0040dbd0); `:280` stores 6 (0x004104e3);
`SubStateMachine.cpp:175-180` calls the image's 0x0040e560 when `RoundSubMode() == 2`, which stores 7 through 0x0040e364.
The exe stores no 8, 9, 0xa or 0xb: `RuleEngine.cpp:148` leaves `OnConclude` empty in the exe, and the 0xb store at
`ScoringHooks.cpp:548` belongs to the .asi's `LiveInputs`. Round end 7 is a flag, not a store: `TrackRenderer.cpp:8970`
raises `s_roundEnd7`, which `aib_game_sub_mode` returns as 7.

**Verdict row 23: yes, on two frame classes. The phase-5 gate only tests `== 6` (0x00417aec), so only 6 vs non-6 matters.**
- Agree (both non-6, or both 6): countdown states 3/4/5 and the 5 -> 6 handover (exe stores the same values at the same
  transitions); a non-match round end (original 7 at 0x004111a6, then 8 / 9 in case 7; exe returns 7 from the flag).
  The values differ there (7 vs 8 / 9) but both skip phase 5.
- Differ, case A, match end: the original goes 6 -> 0xb (0x00410645) -> 9 (0x00410b02) -> 0xa (0x0040ddf5), and
  FUN_004177b0 runs in states 0xb and 0xa through FUN_0040fc00 with phase 5 off. The exe never stores those values and the
  flag is raised only on the rule engine's r == 0 round end (`TrackRenderer.cpp:159-163`), so an exe pre-tick after a
  match end reads 6 and runs phase 5. [UNCERTAIN] whether the exe calls `AiPreTickRubberBand` after a match end. Missing:
  a per-call log of `aib_game_sub_mode()` at `AiPreTickRubberBand` entry and of 0x0063ba8c at FUN_004177b0 entry across
  one match end of the canonical scenario.
- Differ, case B, round start: the exe stores 0 (`TrackRenderer.cpp:7733`), which `aib_game_sub_mode` maps to 6. The
  original passes through 8 -> 2 there, and FUN_004177b0 is not reachable in cases 2 and 8. [UNCERTAIN] whether any exe
  pre-tick runs while the cell is 0, i.e. before the countdown block's `ba8c == 0` arm (`TrackRenderer.cpp:5321`) on the
  round's first frame. Missing: the same per-call log on the first frames of round 2.
- Not covered by either side's store set: time-trial 9b (FUN_00429310, needs `DAT_008991bc == 0xb`) and the FUN_0040d270
  store of 1 (#1). Missing: whether the canonical scenario reaches either (a write watch on 0x0063ba8c,
  `--watch-write`, over one canonical race).

## Side finding (not asked, recorded for the next step)

In state 5 the original calls FUN_004177b0 twice per dispatcher call when FUN_00425a40 does not return early: once from
FUN_004103a0's own FUN_00418860 call and once from case 5's FUN_0040fc00 -> FUN_00425a40 -> FUN_00418860. The exe's state-5
emulation calls the AI tick once at 0x0041040e (`SubStateMachine.cpp:198-200`). Whether the exe also runs the FUN_0040fc00
leg in state 5 was not checked: [UNCERTAIN], missing a per-frame count of `AiPreTickRubberBand` calls in state 5.

## Not done

No tracker change: neither row closes an `UNCERTAINTIES.md` row. Both verdicts need the runtime logs named above, which
this static step does not produce.
