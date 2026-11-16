# RESULT T2-RACEPATH-CENSUS: which functions the original runs, and what the top-down route has for each

## Verdict: PARTIAL (boot, audio-init, menu and race-load measured; race-tick pending, see section 7)

Every census run up to c8 dies early in the race. c8 is the one clean run up to the race anchor, with no harness
intervention, and it is the source of every count below. Its anchor is at N=3633 with dn_warp=108, the same as ref1
(N=3632, dn_warp=108). The raw files are listed at the end.

## 1. Method (measured, not inferred)

**Recipe.**
- The ref1 argv of `verify/ro_20261116/REFERENCE_RO-REF-NAME.json`, unchanged, through `census_arms.py` (a copy of
  `refname_arms.py` with this row's root, data folder and lock label).
- Root: `D:\MashedWT\T2-RACEPATH-CENSUS_root`, a robocopy of `RO-REF-FIXED-LOAD_root`. `original/MASHED.exe` sha256 is
  `b9977dab...` (the ref1 exe).
- Each launch ran under test-slot, with `mashed_lock.py run --label T2-RACEPATH-CENSUS` inside it.

**Counters.**
- `detorig.py --prov-count` puts a native counter (Interceptor + CModule, no JS per call, all threads) on each listed
  entry of `re/console/pc_funcs.csv` (6541 entries). The counters go live at the PE entry.
- The c8 list has **6445** entries:
  - 34 are excluded by `census_list.py` (`not_instrumented.tsv`):
    - 12 have a `ret` before 5 bytes.
    - 2 have a `jmp` before 5 bytes.
    - 13 have a direct branch target inside the patch window. Jump-table entries are scanned too: the image has no
      base relocations.
    - 7 entries are the return address of a call.
  - 68 CRT functions that use the x87 unit are excluded by `census_crtfpu.py` (`not_instrumented_crtfpu.tsv`).
  - Section 6 says why the last two rules exist.
- `0x00497310` gets no counter: the install fails with "wrong type" because detorig already hooks it.
- These 96 functions plus `0x00497310` are **not instrumented**. They never appear in the counts.

**Phase split.**
- Counter snapshots at tick 1, the warp and the race anchor come from `detorig.js`.
- `--prov-snap` (new) adds the first enter or leave of named RVAs.
- c8 values (all main thread, all tick 0): `0x004669b0` enter at n=12, `0x005aa3d0` enter at n=14 (where both exe builds
  stop today, RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970), `0x0045d460` enter at n=18, `0x004669b0` leave at n=46.

| phase | from | to |
|---|---|---|
| boot | the PE entry | tick 1, minus the audio init |
| audio-init | `0x004669b0` enter | `0x004669b0` leave |
| menu | tick 1 | the warp (tick 300) |
| race-load | the warp | the race anchor |
| race-tick | the anchor | the end |

**Status join** (`census_join.py`).
- RE status: `hooks.csv` confidence plus the `c4=` route, `ADMITTED.tsv`, `PROVISIONAL.tsv` (status and score), and
  `PROVISIONAL_RWC.tsv` (score).
- Binding on the top-down route, in this order of precedence:
  1. `verify/portorigtop_20261113/tools/plan.json` (audio tree: bind match / admitted / provisional / replay, reuse, or stop).
  2. `verify/porttop_20261108/BINDINGS.tsv`.
  3. `ROUTED.tsv` rows routed under MASHED_RWC_CORE.
  4. Static reach of today's exe: `matched` if C4 by match or libid, else `exe-body`.
  5. Anything else: `unbound`, meaning no exe body on the route.
- Bindings `rw-lib`, `crt-lib` and `shim` are librw, exe-CRT and shim bindings.

**Reach.**
- `reached_today`: the original ran the function before its first call of `0x005aa3d0`. [UNCERTAIN] This assumes the exe
  follows the original's call order up to that stop. It is not measured on the exe. The claim is in the register.
- `reach_static` (`census_reach.py`, `reach_top_static.tsv`): the linked exe's call graph walked from `_Orig_00492370@16`.
  - Tool: `scripts/backfill_port.py` build_graph (rel32 calls + base relocations).
  - Exe and map: the PORT-TOP-AUDIOINIT-SPLIT-5A9970 checkpoint-4 build (5cae85f7f, in orch/integration at 7a5e5e777).
  - Result: 17,871 exe functions reached, 1448 original RVAs named.

## 2. Counts (c8, from `census_counts.json` by `census_tables.py`)

The tables are in **`census_tables.md`**, generated from `census_counts.json` with no hand-copied numbers:
- Distinct functions executed up to the race anchor: **2199**. 1026 of them ran before the exe's stop at `0x005aa3d0`.

**Phase x RE status**

| phase | C4:match | C4:libid | C4:frida-replay | C4 (no route tag) | C3 | C2 | C1 | not in hooks.csv | total |
|---|---|---|---|---|---|---|---|---|---|
| boot | 940 | 297 | 7 | 6 | 25 | 268 | 43 | 22 | 1608 |
| audio-init | 233 | 22 | 7 | 0 | 21 | 63 | 12 | 10 | 368 |
| menu | 184 | 44 | 5 | 6 | 15 | 21 | 4 | 2 | 281 |
| race-load | 713 | 133 | 11 | 14 | 39 | 220 | 35 | 14 | 1179 |
| race-tick | pending | | | | | | | | |

Byte-matched (`c4=match`, `c4=libid`) and replay-verified (`c4=frida-replay`) counts stay apart, per L-128.

**Phase x top-down binding**

| phase | matched | replay | admitted | provisional | stand-in | under-stand-in | hand-port | exe-body | shim | rw-lib | crt-lib | missing | stop | unbound |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| boot | 422 | 4 | 1 | 8 | 41 | 7 | 18 | 60 | 19 | 53 | 17 | 5 | 7 | 946 |
| audio-init | 133 | 3 | 8 | 11 | 6 | 0 | 2 | 11 | 2 | 8 | 8 | 1 | 12 | 163 |
| menu | 116 | 4 | 1 | 2 | 5 | 6 | 9 | 19 | 0 | 6 | 3 | 0 | 3 | 107 |
| race-load | 215 | 4 | 1 | 4 | 9 | 1 | 11 | 32 | 9 | 33 | 11 | 1 | 9 | 839 |

**Functions first run in the race load: 364.** By binding: unbound 337, matched 18, exe-body 3, and 1 each of
stand-in, under-stand-in, hand-port, shim, rw-lib and stop.

**Race-load unbound: 839.**
- By status: C4:match 448, C2 187, C4:libid 122, C1 29, C3 26, not in hooks.csv 13, C4 (no route tag) 11,
  C4:frida-replay 3.
- By subsystem: renderware 163, render 142, lua-4.0 130, RenderWare-Physics-3.7 76, util 49, msvc-crt-main 45,
  track 41, audio 40, directshow 22, frontend 20, libpng 19, gameplay 16.

## 3. Race load: provisional, stand-in, missing and stop functions, ranked by race-load calls

| binding | rva, name and race-load calls |
|---|---|
| provisional (4) | `0x005a6f30` 104, `0x005a6ea0` 104, `0x005aa0c0` AudioTreeWalkPredicateSearch 72, `0x005ae800` AudioPoolBlockAlloc 36 |
| stand-in (9) | `0x0055deb0` RwpWorldSolverHandle_RegAbi 650, `0x00556e90` 208, `0x004967e0` 105, `0x00429290` 104, `0x004d8480` RwErrSlotWrite 78, `0x004963e0` ConfigLogError 7, `0x004b4080` 5, `0x004950b0` QpcTimeScaledTo3Mhz 3, `0x005c4ad0` 1 |
| under-stand-in (1) | `0x0042f510` Vehicle0HandleGet 3 |
| missing (1) | `0x00428450` 104 |
| stop (9) | `0x004ccef0` 5647, `0x004a34b0` _strncpy 349, `0x0045dd60` 104, `0x0045d7a0` 104, `0x005a75b0` 104, `0x0057c210` RwpBodyTableLookup 72, `0x005aead0` 4, `0x005a7b60` 1, `0x00551460` 1 |
| hand-port, shim, rw-lib, exe-body (11 / 9 / 33 / 32) | in `census_tables.md` |

**Top unbound by calls:**
- `0x0053e6d0` C1 renderware 53004
- `0x004df8a0` C4:match renderware 28080
- `0x004d54f0` C4:match render 26832
- `0x004d7480` C4:match render 26832 (prov_rwc 96.2)
- `0x004b5380` C4:match render 22563
- `0x004b6cd0` not in hooks.csv 22480
- `0x005a6670` C4:match audio 20800

## 4. Audio init: what remains before 0x0045d460

- **94 functions** are first called after the exe's stop (`0x005aa3d0`) and up to the first call of `0x0045d460`. The
  set is the same in c2, c4 and c8.
- **Stops remaining before 0x0045d460: 65.**

| group | count |
|---|---|
| plan.json stop | 8 |
| unbound, C4 by match or libid (binding only) | 29 |
| unbound, no C4 body | 28 |
| already bound: matched / replay / provisional | 25 / 2 / 2 |

**`audio_window_65.tsv`** has one row per remaining stop: class, first_call_order (rank in the 94, measured with
`--prov-snap` on all 94 in run c4), calls up to `0x0045d460`, calls in the whole audio init (c4), status and
first_call_thread.
- 62 are first called on the main thread.
- 3 are first called on the `__beginthread` worker (tid 21164 in c4): `0x005c2dd9`, `0x005aef70`, `0x005a84d0`.
- The first five in call order: `0x005aa3d0` (1), `0x005b36b0` (2), `0x005a6340` (3), `0x005aee20` (4, no C4 body),
  `0x005aa200` (5).

## 5. Run-to-run comparison (is the harness changing the counts?)

**c2 against c7, up to the warp.**
- c7 carried `--crash-fpmask`, whose handler count is 0 at the warp: it did nothing before the warp.
- The distinct sets are **equal** in every pre-warp phase: 1811 functions to tick 1, 1841 to the warp.
- The **audio-init counts are equal**: 0 differences.
- 111 functions differ before the audio init, by +1 to +84 calls (one by -3). The boot's wall-clock polling ran longer
  in c7: bootlen tgt 11 against 13, rw_waits 1 against 2.
- 25 functions differ in the menu: the `0x0049c7xx`-`0x0049dxxx` per-frame cluster at 154 against 168, `0x00499820` at
  23 against 20, `0x0049cc40` at 2 against 1.
- These are run-to-run differences in real-time pollers. The distinct sets do not depend on them.

**c7 against c8, the race load.**
- c7 had the fpmask handler active, with 30 masked traps. c8 had no fpmask.
- Set: equal except `0x004a842d` (CRT), 0 calls in c7 against 30 in c8.
- Counts: 25 differ, all frame-pump or CRT code. 22 are the `0x0049c7xx`-`0x004a21a0` cluster at 92 against 56/57. The
  other three are `0x004a2be9` (1060 against 1090), `0x004a842d` and `0x004a4170` (29 against 3).
- So **fpmask did change counts** in the race load. c7 is diagnostic only, and no count in this RESULT comes from it.
- Both runs reach the anchor at dn_warp=108.

**Instrumentation against ref1.** The counters shift the boot by 1 or 2 clock calls: bootlen N 233 to 235 in the census
runs, against ref1's 234. So the census race is not guaranteed to be the ref1 race tick for tick. The race anchor lands at
the same dn_warp, 108.

## 6. The race crashes (c1..c8) and the exclusions they led to

| run | what changed | outcome |
|---|---|---|
| c1, c2, c3 | 6514 counters | `0xC000008F` (STATUS_FLOAT_INEXACT_RESULT) in the race load, N about 3569 |
| c4 | 94 window snaps | same; used for the window order |
| c5 | `--crash-exit` | the exit comes from mainCRTStartup's `__except`: `0x004a4d75` (`entry`), then `__exit` `0x004a333c`, then `0x004a3258`, then `___crtExitProcess` `0x004a31b1`, then ExitProcess(0xC000008F), on the main thread. Frida's handler never saw the exception |
| c6 | a JS hook on the filter `0x004aba86` | killed the process with no exit call |
| c7 | `--crash-fpmask` (diagnostic native VEH that masks PE and continues) | 30 traps, all at `0x00478542`. That is the first x87 instruction after `FUN_004783f0`'s call of CRT acos `0x004a3384` (at `0x0047853d`), with FCW `0x004f` (precision and underflow unmasked) |
| c8 | 6445 counters | passes the race load with no `0xC000008F`; dies about 11 clock calls after the anchor |

**Why the two new exclusion rules exist (from c7).**
- acos (`0x004a3384`, 9 bytes) is `lea edx,[esp+4]; call __fload_withFB 0x004a70d5`.
- pc_funcs splits a separate "entry" at its return address, `0x004a338d`, which runs `push edx; wait; fnstcw`.
- A counter there runs between `__fload_withFB`'s `ret` and the control-word save.
- Hence: return-site entries are not counted, and neither are the 68 CRT x87 functions.

**c8's death.** An access violation at EIP `0xf327cfe9` with ebp=9 (the same EIP as in c7), with return addresses inside
Frida trampoline pages on the stack (`0x04340a0f`, `0x0456240f`). [UNCERTAIN] The candidate is a counted function whose
patch window holds a `call`: relocated, it pushes a trampoline return address, and a callee that reads its return address
reads trampoline bytes. Test: c9 (section 7).

## 7. Race-tick: pending

c9 drops 233 more entries from the c8 list (`not_instrumented_c9_callwin.tsv`). Those are the entries whose patch window
holds a call and that c8 did not run up to the anchor. The 118 call-window entries that did run up to the anchor keep
their counters, since they are shown safe there. The c9 list has 6212 entries.

## Raw files (not committed)

| file | sha256 |
|---|---|
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c8/prov_anchor.json (all counts) | f714de6cf1a8e54f3a050042b96885d5bb4fe80955d08d34c68e3f1265a67181 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c8/prov_warp.json | 64efea5cae53f67696959d4ed3afcdeb17faab4262e7e51a3868fe7aafa1e0f6 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c8/meta.json | 20a05166fb74747e85fc6234139f75b276b5a0ab475e13dbff283a6069d327ba |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c8.log | f66e1a11acb43ea1c7a6f235f9d86e381c08a92587c5a03598a0296069c7e027 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c8/crash.jsonl | 4b3576787491e328be7ad729634b6dd0b431ca14279d11a11eb54107c668907e |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c7/prov_anchor.json (fpmask, diagnostic) | d1775922aa8244bd9362f9c43aeeb17992773daa4e72981dfcef6ae5ef2a5004 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c7/prov_warp.json | 83224e2befc36cd3a873bad04c86e35b9871906364fb0399a1de45e3eb9eca30 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c7.log | 648e1fafc4f21af8fd51833e63b638dfd1afcb792241e964a1d8a520009852e3 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c7/crash.jsonl | d186576736677a09d62b76e485903dd53eafbf53f17208672f4c4a387dc23898 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c5/crash.jsonl | 97f1acb85ea6253a8f806a9eae5ebace9aa62a63f5c938a63c6dedd4cecdf72c |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c5.log | 71c42fcdb45604b894b94cf790e62ba37128bd2231091cf54c62f8f0ca0d7fd7 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c6.log | 7a48f98c653916ea4d3b747f9ee5bab0617f353e8c9afe88afea9686bbe7a65e |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c4/prov_warp.json (window order) | e8307e2d8042b896af8ab4aeeeded0c4d1aa91ac91ba8128825f9fed59148dd7 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c4.log | ca9e7fb9fde3b74da2624d7dddf0695a94004d5bc98486c2f839907501784655 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c3.log | 53a5059238ba3d236ee76a9d83a6239f25368946966d579db4b0a23f3c19ca25 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c3/crash.jsonl | 642f2dedba35047a33ee434dea1be724d13e9853367e8e2ac4bace35c77f0bc2 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c2/prov_warp.json | b7dc4e935504d93aceeefdb61626304063c1a9608b49e1d0954fe004a130a647 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c2.log | 406b4200ba23cb6e3b91e75c08fa0c3c52006a9f72d96e1be20195e7b2e13510 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c1.log | b8c8b770510d38fdbbe006faab4b6e2eaddb97daae7a173d33ad3bb9175310ca |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/count_list8.txt (c8) | c35a34f606f3eb278b348fb7212c404b24347be45127bb816576cb9bbe92226a |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/count_list9.txt (c9) | cea3694aeadc6980f1f6bb11b212489cfb59205dbfc6209e0814ae98944f0c68 |
| ../PORT-TOP-AUDIOINIT-SPLIT-5A9970/mashedmod/build/mashed_re.exe (reach) | 7a5af1b34a28c0b6e44038090f1b2aa033c2e5cca98818362879f6d6c92a0df8 |
| ../PORT-TOP-AUDIOINIT-SPLIT-5A9970/mashedmod/build/mashed_re.map (reach) | 0b055c33cc393b620bf582428e4f4e851b9ac07164fe8b8b6848d5fc4c895ed4 |

## Files

| file | content |
|---|---|
| `census_T2-RACEPATH-CENSUS.tsv` | one row per rva x phase: calls, status, binding, binding_src, reached_today, reach_static, subsystem, first_phase |
| `census_counts.json`, `census_tables.md` | the counts and their tables |
| `audio_window_65.tsv` | the 65 stops before `0x0045d460` |
| `not_instrumented*.tsv` | the excluded entries, with reasons |
| `reach_top_static.tsv` | the static reach of today's exe |
| `census_list.py`, `census_crtfpu.py`, `census_join.py`, `census_reach.py`, `census_tables.py`, `audio_window.py`, `census_arms.py` | the scripts |

Harness: `re/frida/detorig.py` and `js/detorig.js`.
- `--prov-snap` (with seq and tid per snap) and partial prov reports at the warp and the anchor.
- Crash diagnostics: `--crash-system`, which skips OutputDebugString; `--crash-exit`; and `--crash-fpmask`, which is
  diagnostic only and was not used for any count.
