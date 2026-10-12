# Re-earn fix: cluster `replay` (2026-10-12)

Branch `reearn/replay`, fix commit `e58dfc6c`. Built with `MASHED_NO_DEPLOY=1` only, so the shared deployed .asi was not touched. Not run here: path2 (`run_verify_hook.py`), tracker edits, re-classify. The parent owns those after the merge.

| Row | Hook key | Verdict |
|---|---|---|
| 0x00411580 | `replay_get_best_time` | FIXED |
| 0x004115c0 | `replay_get_current_time` | FIXED |
| 0x00411600 | `replay_record_frame` | FIXED |
| 0x004117b0 | `replay_save` | FIXED |
| 0x00432080 | `race_end_check_finish` | FIXED |

All five rows run under arg_type `scalars_to_scattered_globals` (fixtures b06 / b09, plus `re/frida/negctl_fixtures/z_fix_replay.py` for `replay_save` and `replay_record_frame`). Evidence lives under `re/analysis/negctl/reearn_20261011/evidence/`. negctl output: `evidence/fix_negctl_replay.tsv` (copy of `runs/fix_replay/negctl_20261012_042938.tsv`).

## Harness change (applies to two rows)

`re/frida/diff_template.js`: `scalars_to_scattered_globals` now honours `stub_at` (`[addr | {addr, nargs, abi, ret}]`). It uses the same 5-byte install guard as `stub_dispatch_observe`, and when the handler's `recorder` is configured it plants that recorder. The change is additive and does nothing unless a fixture sets `stub_at`.

Why the fixture needed it: the logger 0x004987b0 ends in `call [0x005cc1b0]` = KERNEL32!OutputDebugStringA (0x004987eb). Under Frida, its debug-print exception 0x40010006 fails BOTH arms with `system error`. The first run of the new vectors showed this on `replay_save` idx 0/4 and `replay_record_frame` idx 4, including original-vs-original. A seed cannot redirect it either, because the IAT slot 0x005cc1b0 sits in read-only `.rdata` (characteristics 0x40000040).

With the stub in place, the recorder stores the call count and each call's first argument (the format-string address) in the scratch, and the fixture observes that. The logger call is therefore an observed output.

`run_diff.build_config` already forwards `stub_at` and `recorder` generically.

Merge note: `diff_template.js` is shared. The hunk is a single block inserted right after the recorder setup in that handler.

---

## 0x00411580 ReplayGetBestTime: FIXED

**Defects** (Vehicle/Replay.cpp):
1. `call 0x00411530` gets five args with the record pointer first:
   - 0x004115a8 `push eax` (DAT_0063bb10)
   - 0x004115a7 `push ecx` (idx)
   - 0x00411598..0x004115a6 push p4, p3, p2
   - 0x004115ae `add esp,0x14`

   The old port passed four args with no record, so 0x00411530 read idx as the record and p2 as idx. Its decomp plate was right; the port misread it.

**Re-read, matches:** both guards (0x00411585 and 0x00411594), index scale `eax+ecx*4+0x17c` (0x0041158d), and return values 1 (0x004115b1) and 0 (0x004115b7).

**Evidence:**
- run_diff: `fix_diff_replay_get_best_time.csv` GREEN 5/5.
- Self-baseline: `fix_diff_replay_get_best_time_selfbaseline.csv` GREEN 5/5.
- negctl: real GREEN 5/5, baseline GREEN 5/5, null RED 3/5, ret RED 5/5, field_first RED 3/5, field_last RED 3/5, field_none CTRL-GREEN 0/5.
- Witness (record pointer dropped, alone): RED 3/5, idx 0/1/3, reimpl errors on idx 1 and 3 (AV). This matches the demotion run. File: `fix_witness_dropobj_replay_get_best_time.csv`.

**Confidences (hooks.csv):**
- Callee 0x00411530 Replay::GetTimeAtIdx: C3.
- Callers: FUN_0041e1e0 C2, TimeTrial::Tick 0x00429310 C2, TimeDisplay::InitForLap 0x004298e0 C2.

**[UNCERTAIN]:** none new. **Plate:** none needed (0x00411580.md already shows the five-arg call).

## 0x004115c0 ReplayGetCurrentTime: FIXED

**Defects:** same as 0x00411580, on DAT_0063bb14:
- 0x004115e8 `push eax`, 0x004115e9 `call 0x00411530`, 0x004115ee `add esp,0x14`.

**Evidence:**
- run_diff: `fix_diff_replay_get_current_time.csv` GREEN 5/5.
- Self-baseline: GREEN 5/5.
- negctl: real/baseline GREEN 5/5, null RED 3/5, ret RED 5/5, field_first RED 3/5, field_last RED 3/5, field_none CTRL-GREEN.
- Witness: RED 3/5, idx 0/1/3. File: `fix_witness_dropobj_replay_get_current_time.csv`.

**Confidences:**
- Callee 0x00411530: C3.
- Callers: FUN_0041e1e0 C2, 0x00429310 C2, FUN_00463590 C2.

**[UNCERTAIN]:** none new. U-1078 (audio_sfx_dispatch xref) is meaning-only and unchanged. **Plate:** none needed.

## 0x00411600 ReplayRecordFrame: FIXED

**Defects** (whole body re-ported from 0x00411600..0x0041174d):
1. **Wrong value passed to 0x004829d0.** `0x0046d4a0(&L3, 0)` stores ONE dword:
   - The stored pointer is `0x00881ec8 + idx*0xd04 + DAT_00881f48[idx*0xd04]*0x40` (0x0046d4ac..0x0046d4c6).
   - 0x00411644 loads that pointer value and 0x00411653 passes it as arg 2.
   - The old port passed the ADDRESS of a zeroed 0x4c-byte local. This is the principal defect.
2. **Missing logger call.** The `0x004987b0("Time trial recording error time %d\n" @0x005ccedc, DAT_007f0ff4)` call was dropped (0x0041161e..0x0041162f, taken when DAT_0063bb14 == DAT_0063bb10).
3. **Missing per-checkpoint calls.** The per-checkpoint `0x00411350(t, &frac, &secs, &mins)` call and the CRT printf pair were dropped:
   - Checkpoint 0 (0x004116a3/0x004116ae/0x004116cd): formats 0x005ccec4 / 0x005ccea4, args `(mins, secs, (double)frac)`.
   - Checkpoint 1 (0x00411716/0x00411721/0x00411740): formats 0x005cce8c / 0x005cce6c.
   - The printf callee is 0x004a2cbd, narrow printf, not wprintf.
4. **No re-reads of DAT_0063bb14.** The old port cached it once. The bytes re-read it at 0x00411648, 0x00411672, 0x00411687, 0x004116e5 and 0x004116fa, and `t` comes from the re-read at 0x0041168d / 0x00411700.

**Re-read, matches:** mode gate `sub eax,2; jne` (0x00411608), the `10` arg (0x0041164e), the DAT_0063bb20 = 0 store (0x0041165c), and `0x00407a00(0,0)` / `(0,1)` (0x00411658 / 0x004116d5).

**Evidence:**
- run_diff: `fix_diff_replay_record_frame.csv` GREEN 6/6, all six fingerprints distinct.
- Self-baseline: GREEN 6/6.
- negctl: real/baseline GREEN 6/6, null RED 5/6, field_first RED 5/6, field_last RED 2/6, field_none CTRL-GREEN, ret SKIP (void).
- Witness (address of a zeroed 0x4c local, alone): RED 5/6, idx 0/1/2/4/5. File: `fix_witness_vehstate_addr_replay_record_frame.csv`.

**Fixture** (z_fix_replay.py, overriding b06): b06's four vectors, plus:
- `_rf(4321, same=True)`, which seeds DAT_0063bb10 == DAT_0063bb14 so the logger branch runs.
- `_rf(3600, ck0=1, ck1=1)`.

The 0x004987b0 stub is described above.

**Confidences:**
- Caller: TimeTrialRecordPlayback 0x00411170, C3.
- Callees:
  - 0x0042f6a0: C3
  - 0x004987b0: C2
  - 0x0046d4a0: C3
  - 0x004829d0 Replay::WriteFrame: C2
  - 0x00407a00: C3
  - 0x00411350: C3
  - 0x004a2cbd: C2

**Coverage gap (not a U-row, stated for the gate):** the four printf calls write to stdout, which the fixture does not observe. Their arguments (`t`, `mins`, `secs`, `frac`) are therefore not compared. The 0x00411350 outs are stack locals, so they are not compared either. Checkpoint slot writes, frame-chain writes and the logger call are compared.

**Naming note:** hooks.csv names 0x004a2cbd `FID_conflict:_wprintf`. It is the narrow CRT printf (stdout FILE* 0x00616110, see Frontend/MenuMixed.cpp:575). This is naming only.

**Plate corrected:** `re/analysis/promote_c2_vehicle_lowrva/0x00411600.md`
- wprintf → 0x004987b0 logger / 0x004a2cbd printf.
- `vehicle_state` is a 4-byte pointer local, and its value is passed.
- The `t` re-read is now spelled out.

## 0x004117b0 ReplaySave: FIXED

**Defects** (whole body re-ported from 0x004117b0..0x00411865):
1. **Wrong calling convention on the RwGlobals +0xc4 slot.** The slot is a `__cdecl` function-pointer field, not a `__thiscall` vtable slot:
   - 0x004117e0..0x004117e6 `esi = DAT_007d3ff8 + 0xc4`.
   - 0x00411801 `call [esi]`.
   - 0x00411843 `add esp,0x30` pops both 4-arg calls plus the 0x004987b0 and 0x00483ca0 pushes.

   The old port called it `__thiscall` with no caller cleanup. ESP drifted and the process died at the /GS check. This is the principal defect.
2. **Missing logger call.** The `0x004987b0("saving replay %s\n" @0x005ccf00, buf)` call at 0x0041180d was dropped.
3. **Wrong format strings.** The old port used its own literals. The bytes push `"%sReplay%d.rep"` @0x005ccf14, `"c:\toast\"` @0x005cce1c and `"%sAll%d.rep"` @0x005cce10.
4. **Null guard the original lacks.** The old port guarded `DAT_007d3ff8 != 0`; the bytes do not.
5. **Slot and global read order.** The slot address is read before `0x00426c00` (0x004117e0 vs 0x004117ec). DAT_0063bb10 and DAT_007d3ff8 are re-read at 0x00411812 and 0x00411823.
6. **Buffer size.** The buffer is 0x40 bytes (`sub esp,0x44`, cookie at +0x40).

**Re-read, matches:**
- Latch early-out with no write (0x004117c7).
- DAT_0063bb10 / 0x00430820 gates (0x004117d4 / 0x004117dd).
- `0x00483ca0(DAT_0063bb10, 0x008a94a8)` (0x0041181e).
- `0x10b` (0x00411829).
- `0x004099a0` (0x00411846).
- Latch = 1 (0x0041184c).

**Evidence:**
- run_diff: `fix_diff_replay_save.csv` GREEN 5/5. The inner-body vectors idx 0 and 4 are distinct. idx 1/2/3 share one fingerprint because all three are early-out paths that end with latch = 1.
- Self-baseline: GREEN 5/5.
- negctl: real/baseline GREEN 5/5, null RED 4/5, field_first RED 4/5, field_last RED 4/5, field_none CTRL-GREEN, ret SKIP (void).
- Witness (`__thiscall` slot call, alone): the process died after the .asi loaded (`frida.TransportError: the connection is closed`) on 2 of 2 boots, the same failure as the demotion run. Files: `fix_witness_thiscall_replay_save.txt`, `fix_witness_thiscall_replay_save_boot2.txt`. There is no CSV because the process dies before results are sent.

**Fixture** (z_fix_replay.py): restores the inner vector b06 left out, `_rs(0,1,0)`, adds a record-content variant `_rs(0,1,0,0x0F0F0F0F)`, and stubs 0x004987b0 with the recorder. The recorder observes the call count and the format pointer 0x005ccf00. The 0x40-byte local buffer is not observed. Its only consumers are the logger (stubbed; the second arg is a stack address) and nothing else.

**Confidences:**
- Caller: thunk_FUN_004117b0 0x0040de00, C4. It reaches this body through C++ linkage, and was re-run GREEN 5/5 (`fix_regress_diff_thunk_replay_save.csv`).
- Callees:
  - 0x00430820: C3
  - 0x00426c00: C3
  - 0x004987b0: C2
  - 0x00483ca0: C3
  - 0x004099a0: C4
  - RwGlobals+0xc4: indirect, RW library

**[UNCERTAIN]:** none new.

**Plate corrected:** `re/analysis/promote_c2_vehicle_lowrva/0x004117b0.md`
- "vtable[0xc4]" → RwGlobals `__cdecl` function-pointer field, citing 0x00411843.
- wprintf → 0x004987b0.

## 0x00432080 RaceEndCheckFinish: FIXED

**Defects** (Vehicle/ReplayCluster.cpp):
1. **Case-2 slot stride.** The slot pointer steps 0x10:
   - 0x00432170 `mov esi,0x7f1a14`, 0x004321ad `add esi,0x10`, 0x004321b1 `cmp esi,0x7f1a54; jl`. That is 4 iterations.
   - The old port stepped 4 bytes (16 iterations, `0x0040e470(0..15)`, ids read from 0x007f1a18/1c/20). This is the principal defect.
2. **Case-0x1000 early break.** The old port stopped at the first hit. The bytes evaluate all eight pairs (0x004320b5..0x00432164, each hit re-stores 1). The final state is identical, so this one is not observable.

**Re-read, matches:**
- DAT_0067f19c = 0 before the guard (0x0043208b).
- Dispatch `dec/dec/sub 0xffe` (0x0043209c..0x004320af).
- Case-1 path (0x004321f2..0x0043221a).
- A/B byte tests `cmp byte [..],bl; je` / `jne` against 0x007f1041 / 0x007f1501 + id*0x4c (`imul eax,eax,0x4c` at 0x00432194).
- Finalise order: 0x0042d3a0, DAT_0067ecac = param_1, e844/e9f8/e914 = 0, f19c = 2, return 1 (0x004321c3..0x004321f1).

**Evidence:**
- run_diff: `fix_diff_race_end_check_finish.csv` GREEN 11/11.
- Self-baseline: GREEN 11/11.
- negctl: real/baseline GREEN 11/11, null/ret/field_first/field_last RED 11/11, field_none CTRL-GREEN.
- Witness (stride 4, alone): RED 2/11, idx 7 and 9. These are the same indices as the demotion run. File: `fix_witness_stride4_race_end_check_finish.csv`.

**Confidences:**
- Caller: FUN_004929d0, C2.
- Callees: 0x0040e470 CarSlotStateGet C3, 0x0042d3a0 RaceTransitionBufZero C3.

**Tracker note for the parent:** the hooks.csv row for 00432080 reads `FUN_00432080, vehicle, C2, mapped, file=re/analysis/promote_c2_vehicle_lowrva/0x00432080.md`. The implementation is `mashedmod/src/mashed_re/Vehicle/ReplayCluster.cpp` (`RaceEndCheckFinish`), so re-classify should set the file, name and status columns.

**[UNCERTAIN]:** U-1049 / U-1052 / U-1053 (A/B byte-array meaning) are naming-only and unchanged. **Plate:** none needed. It already states `piVar2 += 4` on an `int*` and a 0x10 stride; the port misread it.

---

## Regression re-runs (other C3/C4 bodies in touched files)

All GREEN, on the rebuilt post-witness .asi:

| Hook | Result | File |
|---|---|---|
| `replay_cleanup` | GREEN 4/4 | `fix_regress_diff_replay_cleanup.csv` |
| `replay_start_lap` | GREEN 5/5 | `fix_regress_diff_replay_start_lap.csv` |
| `time_trial_record_playback` | GREEN 5/5 | `fix_regress_diff_time_trial_record_playback.csv` |
| `thunk_replay_save` | GREEN 5/5 | `fix_regress_diff_thunk_replay_save.csv` |

Process note: the first rebuild after restoring the sources did not recompile. The restore preserved the pre-witness mtime, which is older than the witness .obj. Both sources were touched and rebuilt, and the build log shows `Replay.cpp`/`ReplayCluster.cpp (source newer)`, before the final `fix_diff_*` and `fix_regress_*` runs.
