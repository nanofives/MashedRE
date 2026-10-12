# a2-camera RESULT: 0x004464c0 CameraEntry::DispatchAll (2026-10-12)

Branch `work/a2-camera`. Hook key `camera_entry_dispatch_all`. **Verdict: FIXED.** Every C2->C3 leg is in except path2,
which the parent runs. Built with `MASHED_NO_DEPLOY=1`, so nothing was deployed to `original\`. No tracker was edited.

**.asi used by every diff.** `run_diff.py` loads `ROOT/mashedmod/build/mashed_re_dev.asi`, and ROOT is the script's
own repo root (`run_diff.py:21,37,43`). Run from the worktree, that is the worktree build. The agent `Module.load`s that
exact path and uses the returned handle, not a lookup by name (`diff_template.js` runDiff).

## Step 1: the [UNCERTAIN] callee attribution

The hooks.csv row 0x004464c0 note says: "[UNCERTAIN] which callee port differs (0x00445aa0 / 0x00441d40 / 0x00442440,
all x87 float paths)". **Resolved: no callee port differs.** The RED came from the dispatcher's own stack frame.

### Bytes (original/MASHED.exe.unpatched)

| Address | Instruction |
|---|---|
| `0x004464c0` | `mov eax,[0x898994]` |
| `0x004464c5` | `push ebx` |
| `0x004464c6` | `xor ebx,ebx` |
| `0x004464ca` | `jle 0x44651a` |
| `0x004464cc` / `0x004464cd` | `push esi` / `push edi` |
| `0x004464ce` | `mov edi,[esp+0x10]` |
| `0x004464d2` | `mov esi,0x8964c0` |
| `0x004464e0` | `call 0x445aa0` |
| `0x004464f0` | `call 0x441d40` |
| `0x00446500` | `call 0x442440` |
| `0x00446508` | `mov eax,[0x898994]` |
| `0x0044650e` | `add esi,0xd8` |
| `0x00446518..0x0044651b` | `pop edi / pop esi / pop ebx / ret` |

Each call is preceded by `push edi / push esi` and followed by `add esp,8`. The frame is **12 bytes**.

### Ghidra function boundaries (decomp_pc.py, read-only pool slot Mashed_pool0)

- Callers: `FUN_00446520`, `FUN_00448700`.
- Callees: `FUN_00441d40`, `FUN_00442440`, `FUN_00445aa0`.
- None of the three callee decomps has an `in_`/`unaff_` register input.

### What the old port compiled to

The old .asi export (`+0x3d110`) pushed `ebx, ebp, esi, edi`, a **16-byte** frame. It held `ebp=0x898994`,
`ecx=0x445aa0`, `edx=0x441d40` and `eax=0x442440` across the loop.

In the dev .asi it always called the **original** callees at their RVAs. Only the standalone arm calls a port
(`CamEntryType0Step445aa0`). So no callee port can have taken part in the RED.

### Live attribution probe (frozen game thread)

The probe is `a2-camera_evidence/attrib_probe.{py,js}`. It is a diagnostic only, not the verification lane. Each
variant dispatcher is assembled from the original bytes and changes one thing. Every arm restores
`0x008964c0..0x00898997`, runs the original dispatcher once as prep (as the fixture does), runs the variant, then
byte-diffs the window against the original arm. Each arm runs twice: 0 repeat diffs on every variant, in both boots.

The live race had count 7, entry types `0,1,1,1,2,2,3`.

| Variant | Diff vs original (entry+offset) |
|---|---|
| orig vs orig | none |
| verbatim copy | none |
| port EAX/ECX/EDX/EBP at all 3 calls | none |
| port registers at one call (0, 1 or 2) | none |
| **uniform +4 frame** | **e0+0x88, e4+0x68, e5+0x68** |
| old .asi port (`asi_port`) | **e0+0x88, e4+0x68, e5+0x68** (identical to the +4 frame) |
| +4 at the type-0 call only | e0+0x88, e1+0x88 |
| +4 at the type-1 calls only | e1+0x88, e4/e5 +0x68/+0x78/+0x88 |
| +4 at the type-2 calls only | e4/e5 +0x68/+0x78/+0x88 |

Files: `attrib_probe_prefix.json` (old build) and `attrib_probe_fixed.json` (fixed build). In the fixed-build run,
`asi_port` reads `[]`.

### Reading the result

- **The matrix pad slots.** `FUN_00445aa0` ORs `0x20003` into `entry+0x58` and passes `entry+0x4c` to `FUN_004c4d20` /
  `FUN_004c51a0` (decomp `0x00445aa0` tail). With the librw Matrix layout (`mashedmod/deps/librw/src/rwbase.h:337-344`:
  right, flags, up, pad1, at, pad2, pos, pad3), a matrix at `+0x4c` puts flags at `+0x58` and pad1/pad2/pad3 at
  `+0x68/+0x78/+0x88`. Every differing dword is one of those three pad slots.
- **All three callees can carry stack dwords into those slots.** It shows up when their stack depth, or the stack left by
  an earlier callee, changes.
- **Under the old port's frame, two callees did.** `0x00445aa0` wrote e0+0x88 and `0x00442440` wrote e4/e5 +0x68.
  `0x00441d40`'s writes matched under the uniform shift.
- **[UNCERTAIN]** which instruction inside each callee writes the pad dword. Missing evidence: a write-watch on
  `entry+0x88` / `+0x68` across one call. The fix does not depend on it.
- **Side note on race.py's own [UNCERTAIN].** race.py says original-vs-original differed on test 0 of the first fixture
  run. That is consistent with this mechanism, since the first arm's stack holds Frida-thread contents rather than a
  previous dispatcher's. It is not proven: orig-vs-orig read `[]` in both probe boots here.

## Step 2: the fix

`mashedmod/src/mashed_re/Util/CameraEntryDispatch.cpp`:

- **Dev .asi arm:** now `__declspec(naked)` and instruction-for-instruction the original stream. The 12-byte frame and
  `[esp+0x10]` are the original's, the calls are memory-indirect through `s_call_00445aa0/00441d40/00442440`, and EAX at
  RET is as the bytes leave it. Disassembly of the rebuilt export (`+0x3d110`) matches `0x004464c0..0x0044651b`. The only
  differences are a `ds:` prefix and `call [ptr]` in place of `call rel32`.
- **Standalone arm:** keeps the C body (type 0 -> `CamEntryType0Step445aa0`, types 1/2 have no exe body). It is
  unchanged in behaviour.

**Prediction, written before the build:**
- run_diff GREEN with the reimpl fingerprint equal to the original's on every row.
- Probe `asi_port` `vs_orig: []`.
- Witness (+4 frame alone) RED 6/8 at idx 0, 1, 4, 5, 6, 7, GREEN at idx 2/3 (count -1/0 take the JLE before the extra
  push).

**All three predictions were met.**

## Step 3: C2->C3 evidence

### Fixture

`re/frida/negctl_fixtures/z_fix_a2_camera.py` overrides race.py's entry. It keeps the same window, freeze and prep, and
adds two things:
- `fold_ret` (sig uint32). The old fixture left the ret mutant SKIP.
- A seeded count `DAT_00898994` per test: natural x3, -1, 0, 1, 4, 6. That exercises the JLE at `0x004464ca`, the
  type-0-only bound, type 0+1, and type 0+1+2.

### Results

1. **run_diff:** GREEN 8/8, 6 distinct fingerprints (`a2-camera_evidence/fix_diff_camera_entry_dispatch_all.csv`, the
   post-witness re-run). **self-baseline:** GREEN 8/8 (`fix_diff_camera_entry_dispatch_all_selfbaseline.csv`).
2. **negctl**
   - Command: `run_negctl.py --handlers scalars_to_scattered_globals --hooks camera_entry_dispatch_all --any-conf --race
     --classes null,ret,field_first,field_last,field_none --slots 1`
   - TSV: `a2-camera_evidence/negctl_run/negctl_20261012_175554.tsv`

   | Class | Result |
   |---|---|
   | real | GREEN 0/8 |
   | baseline | GREEN 0/8 |
   | null | RED 7/8 |
   | ret | RED 8/8 |
   | field_first | RED 5/8 |
   | field_last | RED 5/8 |
   | field_none | CTRL-GREEN 0/8 |
3. **Witness** (old defect alone: `push ebp` after `push edi`, `mov edi,[esp+14h]`, `pop ebp` before `pop edi`, i.e.
   the old 16-byte frame and nothing else): RED 6/8 at idx 0, 1, 4, 5, 6, 7
   (`a2-camera_evidence/witness_frame16_camera_entry_dispatch_all.csv`).
   - The fix was then restored and the source mtime bumped (17:57:21), and the .asi rebuilt (17:57:33,
     "(source newer)").
   - Re-disassembly showed no `push ebp`.
   - run_diff and self-baseline were re-run, both GREEN 8/8.
   - Pre-fix RED on the same worktree build: `prefix_red_diff_camera_entry_dispatch_all.csv`, RED 4/4 on the race.py
     fixture.
4. **Callers and callees** (Ghidra function boundary, decomp_pc.py):

   | Role | Function | Status |
   |---|---|---|
   | caller | `FUN_00446520` | C2 |
   | caller | `FUN_00448700` VehicleSeedWritePair | C2 |
   | callee | `0x00445aa0` CamEntryType0Step445aa0 | C3 |
   | callee | `0x00441d40` | C2 |
   | callee | `0x00442440` | C2 |

   The dev .asi port calls the original callees, so the callee levels do not gate this diff.
5. path2 was not run, and C3 is not claimed here.

**ready for path2: camera_entry_dispatch_all**

## Proposed tracker changes (parent applies)

**hooks.csv row `004464c0`: C2 -> C3** after path2 passes. Evidence column:
`re/analysis/negctl/followup_20261012/a2-camera_evidence/fix_diff_camera_entry_dispatch_all.csv:GREEN-8/8`.

Append to the note:

> a2-camera 2026-10-12: [UNCERTAIN] callee attribution CLOSED. No callee port was involved (the .asi arm calls the
> original 0x00445aa0/0x00441d40/0x00442440). The RED was the port's 16-byte frame vs the original's 12
> (0x004464c5/0x004464cc/0x004464cd). The callees copy uninitialized stack dwords into the entry's RwMatrix pad slots
> (+0x68/+0x78/+0x88). Live probe: uniform +4 frame alone == old port's diff (e0+0x88, e4+0x68, e5+0x68), registers
> exonerated. Re-ported naked verbatim. run_diff GREEN 8/8 + self-baseline GREEN, negctl null/ret/field_first/field_last
> RED, field_none CTRL-GREEN, witness (+4 frame alone) RED 6/8.
> re/analysis/negctl/followup_20261012/a2-camera_RESULT.md

**Other trackers:**
- New U-row (optional, low priority): "which instruction in 0x00445aa0 / 0x00441d40 / 0x00442440 writes the RwMatrix pad
  dword of the camera entry from stack". Path: a write-watch on entry+0x88/+0x68 over one call. It does not gate
  0x004464c0.
- No STUBS / DEFERRED change.

**Standalone note.** The exe arm's `CamEntryType0Step445aa0` is a C port. If any reader consumes the pad slots, the
standalone's pad values are not the original's. Nothing in this session shows a reader. Not a 0x004464c0 issue.
