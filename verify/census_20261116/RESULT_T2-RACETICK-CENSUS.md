# RESULT T2-RACETICK-CENSUS: the race-tick part of the census, and the cause of the race-tick crash

## Verdict: GREEN (race tick measured; crash cause found and removed)

- **The cause of the race-tick crash is one census entry, `0x0040e430`.** pc_funcs lists it as a function
  (`switchdataD_0040e430`), but it is the 6-dword jump table that `FUN_0040e3a0` indexes at `0x0040e3a9`
  (`jmp dword ptr [eax*4 + 0x40e430]`).
  - The counter's 5-byte `E9 rel32` patch overwrites table[0] and the first byte of table[1].
  - The first race-tick call of `0x0040e3a0` (from `0x0040eea6`, car index 0) then jumps to `0x????cfe9`. The low byte
    `e9` is the patch's opcode.
  - This is the AV of c7..c10 and of r2..r4 below.
- **r5 is r2 minus that one entry.** It runs all 600 ticks with rc=0, where r2 died 9 clock calls after the anchor.
- **Race tick: 1731 distinct functions executed. 1178 of them run for the first time in the race tick.**
- The pre-anchor phases of r5 equal c8's in every table cell: boot 1608, audio-init 368, menu 281, race-load 1179, and
  2199 distinct functions up to the anchor. So the change touched no earlier phase.
- 8 runs were allowed. 5 were used (r1..r5).

## 1. Recipe (unchanged from T2-RACEPATH-CENSUS)

- **Launcher:** `racetick_arms.py`, a copy of `census_arms.py` with this row's root, data folder and lock label. It
  takes the ref1 argv of `verify/ro_20261116/REFERENCE_RO-REF-NAME.json`.
- **Root:** `D:\MashedWT\T2-RACETICK-CENSUS_root`, a robocopy of `T2-RACEPATH-CENSUS_root`. `original/MASHED.exe`
  sha256 is `b9977dab70607720cf58bc7832937c203b1e0b33ff5896da586baf56aa79fe87`, the ref1 exe.
- **Launch:** test-slot (`PROYECTOS_TEST_MEM_GB=4`) outside, `mashed_lock.py run --label T2-RACETICK-CENSUS` inside.
- **Flags on every run:** `--crash-catch 60 --crash-system --crash-exit` and
  `--prov-snap 0x004669b0,0x005aa3d0,0x0045d460,0x004669b0:leave`, the phase snaps of c8.
- **Harness additions** (`re/frida/detorig.py`, `js/detorig.js`):
  - `fault-counts`: at the first fault, the handler writes every counter that moved since the anchor snapshot (rva,
    calls now, calls at the anchor), plus the seq / n / tid of every `--prov-snap` first entry.
  - `--prov-probe` (r4 only): attaches each counter as a probe (the bare `onEnter` pointer) instead of the CModule
    object.
- **Race tick:** from the anchor (N=3634, tick 300, dn_warp=108) to the end of the run (N=33753). In that window the
  per-frame function `0x00492e90` ran 2725 times.

## 2. The runs

| run | counters | outcome |
|---|---|---|
| r1 | 0 (an empty `--prov-count` file; the snaps and crash flags are on) | anchor N=3632 dn_warp=108, 600 ticks, rc=0. **The harness flags alone survive the race tick.** |
| r2 | 6445 (the c8 list) | AV at EIP `0xf327cfe9`, N=3643, esp `0x1afdac`, ebp 9. `fault-counts`: 719 counters moved after the anchor, 345 of them with 0 calls at the anchor |
| r3 | 6094 (the c10 list), plus snaps on the 321 of those 345 that are in it | same AV, N=3644. All 321 snaps were taken. The last first-entered: `0x0047ea60` (seq 323), then `0x0040e3a0` (seq 324) |
| r4 | 6445, probe form | same AV (EIP `0xf327cfe9`, N=3642) |
| r5 | 6444 = the c8 list minus `0x0040e430` | anchor N=3634 dn_warp=108, 600 ticks, rc=0. 3377 distinct executed, install_fail 1 (`0x00497310`, as in c8) |

**How the stack located the call.**
- In c10, r2 and r3, every return slot of the main thread's call chain held a trampoline-page value (`0x04xxx40f` /
  `0x04xxxa0f`). An example is the slot of `call 0x004111c0` from `0x00492d85`, at esp+0x88.
- In r4 (probe form) the same slots hold the real return addresses: `0x00492d8a`, `0x0041016c`, and at [esp]
  `0x0040eeab`.
- `0x0040eeab` follows `call 0x0040e3a0` at `0x0040eea6`. In r2, r3 and r4, `0x0040e3a0` has exactly 1 call at the
  fault.
- So the CModule-object form replaces return addresses and the probe form does not. That replacement is not the
  cause: r4 dies the same way.

**Why the fault is the jump table.**
- `FUN_0040e3a0` is 4 instructions before its table jump: `mov eax,[esp+4]; cmp eax,5; ja 0x40e427;
  jmp [eax*4+0x40e430]`.
- Its table on disk (unpatched bytes) is `0x40e3b0 0x40e3c4 0x40e3d8 0x40e3ec 0x40e400 0x40e414`.
- r4's [esp+4] at the fault is 0 (car index 0), so the jump reads table[0], the dword the `0x0040e430` counter
  rewrites.
- The causal evidence is r5 against r2: they differ by that one entry. The mechanism above is consistent with both runs.

**The new exclusion rule.**
- `census_jumptable.py` sweeps `.text` for `jmp`/`mov dword ptr [reg*4 + imm32]` and `movzx byte ptr [reg + imm32]`
  tables: 312 dword tables and 33 byte tables.
- It excludes every entry whose 5-byte window overlaps one. In the c8 list that is `0x0040e430` only
  (`not_instrumented_jumptable.tsv`).
- c9 and c10's call-window exclusions are **not** needed. r5 counts those 353 entries.

**Why the call-window hypothesis was wrong.** The patch was never on `0x0040e3a0`'s code. It was on its data.
`0x0040e3a0` is first called in the race tick, so no run failed earlier.

## 3. Race-tick counts (r5, from `census_counts_racetick.json` by `census_tables.py`)

All tables are in `census_tables_racetick.md`. It has the same rows as the previous RESULT plus the race-tick row.

**Phase x RE status** (distinct RVAs)

| phase | C4:match | C4:libid | C4:frida-replay | C4 | C3 | C2 | C1 | none | total |
|---|---|---|---|---|---|---|---|---|---|
| boot | 940 | 297 | 7 | 6 | 25 | 268 | 43 | 22 | 1608 |
| audio-init | 233 | 22 | 7 | 0 | 21 | 63 | 12 | 10 | 368 |
| menu | 184 | 44 | 5 | 6 | 15 | 21 | 4 | 2 | 281 |
| race-load | 713 | 133 | 11 | 14 | 39 | 220 | 35 | 14 | 1179 |
| **race-tick** | 1064 | 117 | 77 | 75 | 69 | 271 | 20 | 38 | **1731** |

Byte-matched (`c4=match`, `c4=libid`) and replay-verified (`c4=frida-replay`) stay apart, per L-128.

**Race tick: status x binding** (`racetick_status_x_binding.md`, from `census_T2-RACETICK-CENSUS.tsv`)

| status | matched | replay | admitted | provisional | stand-in | under-stand-in | hand-port | exe-body | shim | rw-lib | crt-lib | stop | unbound | total |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| C4:match | 297 | 0 | 0 | 0 | 6 | 10 | 16 | 0 | 0 | 14 | 0 | 0 | 721 | 1064 |
| C4:libid | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 10 | 0 | 107 | 117 |
| C4:frida-replay | 0 | 8 | 0 | 0 | 0 | 0 | 0 | 6 | 0 | 0 | 0 | 0 | 63 | 77 |
| C4 | 0 | 0 | 0 | 0 | 1 | 1 | 0 | 3 | 0 | 0 | 0 | 4 | 66 | 75 |
| C3 | 0 | 0 | 1 | 3 | 0 | 0 | 0 | 14 | 0 | 0 | 0 | 1 | 50 | 69 |
| C2 | 0 | 0 | 1 | 4 | 2 | 0 | 1 | 10 | 0 | 1 | 0 | 8 | 244 | 271 |
| C1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 4 | 0 | 0 | 0 | 1 | 15 | 20 |
| none | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 3 | 1 | 0 | 0 | 1 | 33 | 38 |
| total | 297 | 8 | 2 | 7 | 9 | 11 | 17 | 40 | 1 | 15 | 10 | 15 | 1299 | 1731 |

No race-tick function has binding `missing`.

**Functions first run in the race tick: 1178.** By binding: unbound 1014, matched 123, under-stand-in 10, hand-port 9,
exe-body 11, stop 6, replay 2, stand-in 2, admitted 1.

**Race-tick unbound: 1299.**
- By status: C4:match 721, C2 244, C4:libid 107, C4 66, C4:frida-replay 63, C3 50, none 33, C1 15.
- By subsystem: render 210, audio 146, RenderWare-Physics-3.7 139, gameplay 137, qhull-2002.1 107, util 98,
  renderware 83, vehicle 64, particle 48, ai 37, msvc-crt-main 36, none 33.

## 4. Race tick: provisional, stand-in, missing and stop functions, ranked by race-tick calls

| binding | rva, name and race-tick calls |
|---|---|
| provisional (7) | `0x005a6ea0` 2725, `0x005a6f30` 2724, `0x005aa0c0` AudioTreeWalkPredicateSearch 204, `0x005ae800` AudioPoolBlockAlloc 31, `0x005aa560` 20, `0x0045a3a0` 12, `0x005ae650` AudioPoolConstruct 1 |
| stand-in (9) | `0x004950b0` QpcTimeScaledTo3Mhz 27247, `0x00492e90` 2725, `0x004111c0` 2725, `0x004967e0` 2724, `0x0055deb0` RwpWorldSolverHandle_RegAbi 2719, `0x004b5580` ClumpForAllAtomics_Cb4b5560 500, `0x00556e90` 408, `0x004d8480` RwErrSlotWrite 2, `0x0045bed0` PowerupRoundCleanup 2 |
| under-stand-in (11) | `0x00492440` 261600, `0x004671c0` GetOverlayCamera 10900, `0x004266b0` WorldRenderDispatch_End 5450, `0x00426670` WorldRenderDispatch_Begin 5450, then 2725 each: `0x0040df60` ConditionalRenderSubPass, `0x0040de30`, `0x00410b30`, `0x00404320` PerModeRenderMachine, `0x0040dfc0` HudIngameDispatch, `0x00426030`, `0x0040df20` |
| missing | none |
| stop (15) | `0x0046c7b0` VehicleSlotGetter 377385, `0x0046cbb0` VehicleCarStateRead 273243, `0x0057c210` RwpBodyTableLookup 127345, `0x005a75b0` 55735, `0x00463640` 10896, `0x005aac00` 5378, `0x005b0cf0` 4672, `0x004075a0` Global63a5d0Get 3224, `0x0045efe0` 2724, `0x0045d7a0` 2724, `0x0045dd60` 2724, `0x004ccef0` 612, `0x005a9f10` 16, `0x00551460` 4, `0x005a9e60` 3 |
| hand-port, exe-body, rw-lib, shim (17 / 40 / 15 / 1) | in `census_tables_racetick.md` |

Against the census2 fallback in the previous RESULT (another scenario):
- The provisional list adds `0x0045a3a0`.
- The stop list adds 7 RVAs: `0x0046c7b0`, `0x0046cbb0`, `0x0057c210`, `0x005aac00`, `0x004075a0`, `0x004ccef0`,
  `0x00551460`.
- Section 7 of the previous RESULT is superseded by this measurement.

## 5. Entries not instrumented (the counts are a lower bound for them)

| group | count | why | file |
|---|---|---|---|
| `ret` before 5 bytes | 12 | the patch would cover the next function | `not_instrumented.tsv` (census_list.py) |
| `jmp` before 5 bytes | 2 | the same | `not_instrumented.tsv` |
| direct branch target inside the patch window | 13 | a jump into the patched bytes | `not_instrumented.tsv` |
| return address of a call (pc_funcs split) | 7 | a counter between a call's `ret` and the next instruction (c7, acos `0x004a338d`) | `not_instrumented.tsv` |
| CRT functions that use the x87 unit | 68 | the c1..c3 `0xC000008F` trap | `not_instrumented_crtfpu.tsv` |
| `0x00497310` | 1 | install fails ("wrong type"): detorig already replaces it (scripted input) | none |
| `0x0040e430` | 1 | the window overlaps a jump table (it is data, not a function) | `not_instrumented_jumptable.tsv` |

These 104 RVAs never appear in any count. `0x0040e430` is not code, so nothing is missed by excluding it.

**The run is not the ref1 race tick by tick.** The counters shift the boot by 1 or 2 clock calls: bootlen N=236 in r5,
against 234 in ref1. The anchor lands at the same dn_warp, 108. This is the same caveat as the previous RESULT.

## Raw files (not committed)

| file | sha256 |
|---|---|
| D:/MashedWT/T2-RACETICK-CENSUS_data/r1.log | 258c1ab8966b8f8dce6cb56985e03cc6bd7477d6ab2e6925ffde14c7e95bf8e4 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r2.log | 046f88a8049e50eb808792d8c27960246847b4dbfc705fbf9a3b1b239a2d5803 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r2/crash.jsonl | 3bf38cbb2ab74bb355e6bf85045a60380dbb9827b5314329d228df157185ce0e |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r3.log | ba921e3e2b6a927a81a2b1c1b62e519c1450c16547a3f9624ab8616c7adbdfa0 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r3/crash.jsonl | 567edd60b16d32dfb50a422e9e567b415bc878a2374fe0b7f30e52eb494f1b31 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r4.log | a60f8ad77aa2114c9c1af4439c30544de05ba4d0523ed99cf82be16fa0134e7d |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r4/crash.jsonl | 98b05519cb3602b84c5db4583e9ad4087c9cd0b4d06b09bb9699e1b3f5a9107e |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r5.log | 84dda8ce91daa7ea6707f3f2557327caead798e442f099e5a015b8767c395be4 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r5/prov.json (all counts) | 1da430263986c76215069d28d093d99553e382c1cab12c73aa0af38d3a9e1993 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r5/prov_anchor.json | 1f9b2775f53ab332f209e25cc671ebaae165f813d7294a57827013ad743b52f8 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r5/prov_warp.json | 6845bcf0b17d2a60f9aac94e506f48d43aadd888495a1b34993d6b64235e8721 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/r5/meta.json | dec696ab52e92bdadf1fb623e163a9fcf56356fb46172ae04dd5d4e78a5e6146 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/count_full.txt (r2, r4; = c8's count_list8.txt) | c35a34f606f3eb278b348fb7212c404b24347be45127bb816576cb9bbe92226a |
| D:/MashedWT/T2-RACETICK-CENSUS_data/count_c10.txt (r3; = count_list10.txt) | a6f65d80d82cf3287fd24f34ac9184423eb413de56b98684cd7848bd0231f986 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/count_r5.txt (r5) | 69aecd9c33be180cab365e39eb830915dbeaf7e7b53c044d648cb231f6616919 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/count_empty.txt (r1) | e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 |
| D:/MashedWT/T2-RACETICK-CENSUS_data/snap_r3.txt (r3 snaps) | 8aad3051acdee6c142204d1e9e0a07e66226507d333ca05be1faffc109b68508 |

## Files

| file | content |
|---|---|
| `census_T2-RACETICK-CENSUS.tsv` | r5, one row per rva x phase, race tick included (census_join.py) |
| `census_counts_racetick.json`, `census_tables_racetick.md` | the counts and their tables |
| `racetick_status_x_binding.md` | the race-tick status x binding table |
| `racetick_fault_counts_r2.tsv` | r2: the 719 counters that moved between the anchor and the fault |
| `racetick_crash_extract.json` | r2 / r3 / r4: fault, exit records, fault-counts summary and the last 10 snaps |
| `not_instrumented_jumptable.tsv` | the jump-table exclusion |
| `racetick_arms.py`, `racetick_fault.py`, `census_jumptable.py` | launcher, fault reader, jump-table scan |
