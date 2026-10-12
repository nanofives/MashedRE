# bnormalise: three non-reproducible B rows made observable, 0x00428610 re-ported (2026-10-12)

Branch `work/bnormalise` (base c00795d3). Triage `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md` section B,
rows 0x004b3bb0, 0x004921d0, 0x00428610 (order item 9). Built with `MASHED_NO_DEPLOY=1`, so the shared
`original\mashed_re_dev.asi` was never touched. No path2 and no tracker edits.

## Verdict per row

| row | hook key | port vs the bytes | verdict |
|---|---|---|---|
| 0x004b3bb0 FileWriteWrapper_i3 | `file_write_wrapper_i3` | faithful (listing read, no change) | every C2->C3 leg except path2 passed |
| 0x004921d0 DisplayInit | `display_init` | faithful (listing read, no change) | every C2->C3 leg except path2 passed |
| 0x00428610 ViewportScaledRectDraw | `viewport_scaled_rect_draw` | **3 defects**, re-ported from the listing as an x87 TU | every C2->C3 leg except path2 passed |

**Ready for path2: `file_write_wrapper_i3`, `display_init`, `viewport_scaled_rect_draw`.**

## What the instrument does instead of the triage's proposals, and why

In all three rows the value that does not reproduce between two original calls is produced by a **callee**, not by the
body under test:

| row | non-reproducible value | where it comes from |
|---|---|---|
| 0x004b3bb0 | the return (RW stream pointer) | 0x004cc230 opens a fresh RW stream, and 0x004cbe80 returns it (0x004b3bd6 `mov esi,eax`, 0x004b3be3 `mov eax,esi`) |
| 0x004921d0 | 0x0077195c / 0x00771960, plus the live cameras | 0x004b3eb0 (TXD) and 0x004c5c00 (texture) allocate, and 0x00467110 / 0x0042d560 / 0x0042f660 re-create the cameras |
| 0x00428610 | 0x007d709c | written through the RW / D3D render state that 0x00450b10 drives (`call [edx+0x20]` at 0x00450c23..0x00450c65, `call [edx+0x30]` at 0x00450c77) |

So the normalisation is done **at the source**. Each such callee is a named `call_log` recorder that returns a seeded
value (`stub_rets`). It is installed only while the game's first thread is frozen, so both arms see identical callee
results and nothing live is re-created. Every argument the body passes is logged by value. This is stronger than each
proposal, which fails as follows:

- **Return normaliser for 0x004b3bb0** (fold `ret != 0`): it keeps only one bit of the only output and stays blind to
  the arguments of the three calls. The witness below (close the stream instead of the write result) leaves the
  return unchanged and would read GREEN under it.
- **Restore-only window plus pointer normalisation for 0x004921d0**: with real callees each arm still re-creates the
  live main and menu cameras, so the game's state changes under the run. Recorders never create them.
- **Per-cell baseline filtering for 0x00428610**: by the brief's own guard it cannot give a verdict. The cell the
  original disagrees with itself on (0x007d709c, baseline RED 12/12 with window 0x007d709c+4) is exactly the cell the
  field mutant flips (`re/analysis/negctl/negctl_jobs_20261011.tsv` [viewport_scaled_rect_draw] `flip=0x007d709c`),
  so the filter drops the mutant's target and the guard refuses. Once 0x00450b10 is a recorder neither arm writes
  0x007d709c, and the quad's seven arguments become the output.

Callees that only read or write plain globals stay REAL. Their globals are seeded and, where written, observed:
0x0042b890 / 0x0042b8a0 (store the low word at 0x0042b895 / 0x0042b8a5) and 0x0042b8b0 / 0x0042b8c0 (read
0x0067ea54 / 0x0067ea56).

## The new capability: normalising recorder keys

This extends the `call_log` recorders (followup/a2-stubat, drawstream) in `scalars_to_scattered_globals`. It is not a
new handler.

| key | where | what |
|---|---|---|
| `deref: {i: 'l'}` | call_log entry | Log `stk` when arg i lies in [ESP, ESP+0x4000) at recorder entry, else the raw value. Use it for a pointer to the caller's own locals, whose address differs per arm because the port's frame layout differs. A port that passes NULL, a global or garbage still prints the value. |
| `outs: {i: v \| [v_call0, ..]}` | call_log entry | After logging, write dword v through arg i (an out-parameter the caller reads back). A bad pointer is logged as `name.out<i>@AV` and never thrown. |
| `stub_outs: {name: {i: v \| [..]}}` | test | Per-test override of `outs`. Seed distinct values so that a swapped or dropped out-pointer shows in the NEXT recorded call. |

`outs` / `stub_outs` are mirrored in `verify_hook_install_template.js`, so path2 bodies read back the same values.
Both templates pass `node --check`. `run_diff.py` needs no change, because `call_log` and the tests are forwarded
whole (memory build-config-is-a-whitelist).

How to use it on another row whose output does not reproduce:
1. Find which callee produces the varying value (`re/tools/decomp_pc.py <rva> --callees`, then read each call site's
   pushes with `re/tools/disasm_fn.py`).
2. Make that callee a `call_log` entry with `at`, `nargs` and `stub_rets`. Use `deref 'l'` for args that point at
   the caller's locals, and `outs` for values the callee hands back through them.
3. Leave pure global readers and writers REAL, seed their globals, and observe the ones that get written.
4. Size `scratch_size` so that a 0x400-byte window at every scratch pointer arg stays inside the scratch (see
   Incidents). Otherwise NegCtl's field classes flip Frida heap bytes that the recorders' JS allocations write.

Fixture: `re/frida/negctl_fixtures/z_fix_bnormalise.py`, which replaces the three entries whole. `re/frida/ARG_TYPES.md`
was regenerated (two line numbers moved, no new arg_type).

## 0x004b3bb0 FileWriteWrapper_i3

**Bytes** (0x004b3bb0..0x004b3be6): `push name; push 2; push 2; call 0x4cc230`. `je 0x4b3be6` on 0 returns EAX 0.
`0x004b3bcd push esi` is a callee-save (popped at 0x004b3be5), so 0x004cbe80 gets 3 args (stream, buf, size).
`0x004b3bd8..0x004b3bdb push 0; push esi(write result); call 0x4cc160`. `add esp,0x14` at 0x004b3be0 = 3 + 2 args.
The function returns the write result. The port (Save/SettingsAndIO.cpp:161) matches.

**Fixture.** Recorders `open` 0x004cc230 (the name is logged as a string), `write` 0x004cbe80, `close` 0x004cc160.
The return is folded into the fingerprint. 7 vectors: open fails, the write returns the stream, the write fails,
the write result differs from the stream, and sizes 0 / 3 / 0x40 / 0x24fa0 / 0x7fffffff / 0x80000000. No file is
opened or written: `open` is a recorder.

**Witness** (only the new channel sees it, and it changes the formerly non-reproducible value): `g_StreamClose(handle, 0)`
instead of `(write_result, 0)`. The return is unchanged, so the old return-only channel is blind to it, normalised or
not. RED 5/7, e.g. idx 3 orig `close(0x12345678,0x0)`, port `close(0xffffffff,0x0)`. The two GREEN vectors are where
the defect is a no-op: idx 0 (the open fails, so there is no close) and idx 1 (the write returns the stream itself).

## 0x004921d0 DisplayInit

**Bytes** (0x004921d0..0x0049226b): 14 dwords pushed, cleaned by `add esp,0x38` at 0x00492247.
`vminfo(&L8, (int)(short)vmode())` (movsx at 0x004921da). `w = 0x00498bc0()`, `h = 0x00498bd0()`.
`0x00467110(w, h)`, `0x0042b890(w)`, `0x0042b8a0(h)`, `0x0042d560()`, `0x0042f660()`.
`0x004997b0(0x194, "RWTEXDICTIONARY" @0x005cf6a4, &L4, &L0)` with L0 = L4 = 0 (0x00492229 / 0x0049222d).
`0x004b3eb0(L4, L0, 0, 0)` goes to 0x0077195c (0x0049224d). If that is nonzero, `0x004c5c00(it, "LoadIcon" @0x005cf698)`
goes to 0x00771960 (0x00492263). The port (Boot/SubsystemInit.cpp:260) matches.

**Fixture.** Recorders for the 10 callees: `vmode`, `vminfo` (deref 'l'), `rw`, `rh`, `cam`, `mcam`, `vcam`, `res`
(the string as text, both out-pointers 'l', `stub_outs` = distinct ptr / len), `txd` and `find` (the string as text).
0x0042b890 / 0x0042b8a0 stay REAL. Observed: 0x0077195c+8 and 0x0067ea54+4, seeded with sentinels so the
null-TXD branch shows 0x00771960 untouched. 6 vectors, including a mode of 0x12348003 (logged `vminfo(stk,0xffff8003)`,
which proves the sign extension) and `txd` = 0.
The two dims recorders replace the bytes the `fix_camera_res` patch wrote, but only for the frozen span. They are
reverted before the game resumes, and the game never runs them.

**Witness** (changes a normalised pointer and still goes RED): swap the out-pointers, `0x004997b0(.., &local_20, &local_1c)`.
Both still print `stk`, but the seeded outs land swapped and the next call shows it. RED 4/6, e.g. idx 0 orig
`txd(0xa01000,0x1f40,0x0,0x0)`, port `txd(0x1f40,0xa01000,0x0,0x0)`. The GREEN vectors are idx 2 (both outs 0) and idx 5
(ptr == len), where the swap is a no-op. The old entry (`void`, nothing observed) was BLIND-DEGENERATE.

## 0x00428610 ViewportScaledRectDraw

**Bytes** (0x00428610..0x00428753, `add esp,0x48` = 0x2c locals + 7 args):
- x and y are zeroed at 0x00428615 / 0x0042861d.
- sx = (float)((int)(short)0x0042b8b0() * [0x5cd5a8]) (movsx 0x0042863a, fstp 0x0042864b); sy likewise with
  [0x5cc560] (movsx 0x00428654).
- uv = {[0x5d757c] x2, [0x5cc320] x2}, with the two values left in ST.
- w / h are rounded into [esp+0x14] / [esp+0x18] (0x00428695 / 0x004286a1).
- coordMode: 0/1 store sx*x, sy*y (0x004286db). 2 stores `sx*x - w_stored*[0x5cc32c]` with sx*x kept in ST
  (0x004286ad..0x004286c1). Anything else jumps to 0x004286f3 (`jne` at 0x004286ab) and leaves x = y = 0.
- blendMode 1/2/3 store from ST (0x00428700..0x0042871c).
- tex = 0 (0x0042865f `xor ecx,ecx`), or *tex when tex is non-NULL.
- `call 0x450b10(tex, x, y, w, h, 0xffffffff, &uv)`.

**Defects in the old body** (Frontend/Cluster_v3.cpp, SSE2), all corrected:
1. coordMode not 0/1/2 fell through to the coordMode-0 formula. The original leaves x = y = 0.
2. coordMode 2 rounded sx*x to float before the subtraction. The original keeps it in ST.
3. The screen dims were zero-extended (a `uint16_t` getter and the port's `uint32` ScreenHeightGet). The original
   uses `movsx`. idx 6 (sw = 0x8000, sh = -2): orig w `-0.0512`, h `-137.5`; old body w `0.0512`, h `4505463`.

The old draw_quad_observe entry read GREEN 12/12 with all three defects present. Its vectors used coordMode 0..2 and
positive dims only, and the tex handle never reaches the 0x00898a20 vertex block it observed (0x00450c15 passes it
to the render-state slot).

**Port.** Moved to `mashedmod/src/mashed_re/Frontend/ViewportScaledRectDraw.cpp` (listed in `mashedmod/x87_tus.txt`;
both .rsp files updated). ST-resident values are `double`, rounded to `float` only where the bytes `fstp dword`.
Both getters and the quad are called at their original addresses.

**Fixture.** Recorder `quad` at 0x00450b10: tex as hex, x/y/w/h as float32 values, argb, and uv read as 4 floats.
0x0042b8b0 / 0x0042b8c0 stay REAL with 0x0067ea54 seeded. 12 vectors: tex NULL / handle / 0xffffffff, coordMode
0 / 1 / 2 / 3 / 7 / -1 / 0x80000000, blendMode 0..4 / -1, dims from 320x240 to 0x7fff, negative shorts, and floats
including NaN, -0, 1e30, -1e-30.

**Old-defect witness:** the old body on the new fixture reads RED 6/12 at CW 0x27f (idx 2, 4, 5, 6, 8, 9). The
coordMode-3 / 7 / -1 / 0x80000000 vectors show defect 1, idx 2 and 9 show defect 2, and idx 6 shows defect 3.

**CW measurement.** The original's own output differs between CW 0x27f and 0x7f on 1 of 12 calls (idx 3, x
`614.5474243164062` vs `614.54736328125`). The re-port is GREEN 12/12 at both CWs.

## Evidence (`re/analysis/negctl/session_20261012b/bnormalise_evidence/`)

Final build: SettingsAndIO.cpp and SubsystemInit.cpp recompiled "(source newer)" after the witness restore, and the
`.asi` was loaded from the worktree build.

| leg | file_write_wrapper_i3 | display_init | viewport_scaled_rect_draw | files |
|---|---|---|---|---|
| run_diff, CW 0x27f | GREEN 0/7 | GREEN 0/6 | GREEN 0/12 | `<key>.csv`, `drawstream_<key>.csv` |
| run_diff, CW 0x7f | n/a (no float) | n/a (no float) | GREEN 0/12 | `<key>_cw7f.csv`, `drawstream_<key>_cw7f.csv` |
| --self-baseline, CW 0x27f | GREEN 0/7 | GREEN 0/6 | GREEN 0/12 | `<key>_selfbaseline.csv` |
| --self-baseline, CW 0x7f | n/a | n/a | GREEN 0/12 | `<key>_selfbaseline_cw7f.csv` |
| negctl real / baseline | GREEN / GREEN | GREEN / GREEN | GREEN / GREEN | `negctl_bnormalise_20261012_205150.tsv` |
| negctl null | RED 7/7 | RED 6/6 | RED 12/12 | same |
| negctl ret | RED 7/7 | SKIP (void) | SKIP (void) | same |
| negctl field_first | NA (flips=0) | **RED 6/6** (flip 0x0067ea54) | NA (flips=0) | same |
| negctl field_last | NA (flips=0) | **RED 6/6** (flip 0x00771963) | NA (flips=0) | same |
| negctl field_none | CTRL-GREEN | CTRL-GREEN | CTRL-GREEN | same |
| witness | RED 5/7 (close(handle)) | RED 4/6 (swapped out-ptrs) | RED 6/12 (old body) | `<key>_witness_red.csv` / `_witness_oldbody.csv` + drawstream |
| re-run after restore | GREEN (rows above) | GREEN (rows above) | GREEN (rows above) | |

negctl command: `py -3.12 re/frida/run_negctl.py --handlers scalars_to_scattered_globals --hooks
file_write_wrapper_i3,display_init,viewport_scaled_rect_draw --any-conf --classes null,ret,field_first,field_last,field_none --slots 1`.

The field classes are NA for 0x004b3bb0 and 0x00428610 because neither body writes a byte NegCtl can see: their
only outputs are recorded calls (and the return, for 0x004b3bb0). As in drawstream, the witness stands in for the
field class, a defect that changes one logged argument and nothing else.

Callers and callees by Ghidra function boundary (`re/tools/decomp_pc.py <rva> --callers --callees --no-decomp`):
- 0x004b3bb0: caller 0x00404f50 SaveWrite C4. Callees 0x004cbe80 C4, 0x004cc160 C3, 0x004cc230 C2.
- 0x004921d0: caller 0x00492270 SubsystemInit C4. Callees 0x0042b890 C3, 0x0042b8a0 C3, 0x0042d560 C2,
  0x0042f660 C2, 0x00467110 C2, 0x00498bc0 C4, 0x00498bd0 C4, 0x004997b0 C4, 0x004b3eb0 C2, 0x004c2ed0 C3,
  0x004c2f00 C4, 0x004c5c00 C3.
- 0x00428610: caller 0x00428760 C3. Callees 0x0042b8b0 C3, 0x0042b8c0 C4, 0x00450b10 C3.

## Incidents (no effect on evidence)

- The first viewport run_diff failed to attach (pid 6336 exited during injection, my own spawn). It was gone when
  checked, and the re-run attached.
- The first fixture draft carried `target_global: None` with `__replace__`, so `build_config` raised before any
  spawn. Removed.
- The first negctl run read `file_write_wrapper_i3` field_first **BLIND 0/7** (flips=2, changed=0, flip 0x0eb2320c).
  NegCtl scans a 0x400-byte window at each pointer arg (Harness/NegCtl.cpp, "(a)"). With `scratch_size` 0x200, the
  buf window (scratch+0x100) ran past the scratch into Frida's heap, which the recorders' JS allocations write during
  the call, and the flip landed on such a byte. Fix: scratch 0x600 (file write) and 0x400 (viewport), so every window
  lies in harness-owned memory. The re-run reads NA (flips=0), the evidence above. run_diff and the self-baseline were
  re-run on the final fixture. The two witness runs used the earlier sizes (0x200 / 0x40). That change only widens a
  fill region neither arm reads, and the witness CSVs show the divergent calls themselves.
- No original-side AV occurred (no U-9677 piz collision). Every PID spawned was run_diff's or run_negctl's own and
  exited with its run.

## Commits

| commit | content | keep/drop |
|---|---|---|
| `0e806c3f` | call_log `'l'` deref + `outs` / `stub_outs` (with the path2 mirror), ViewportScaledRectDraw.cpp x87 TU (Cluster_v3.cpp removed), .rsp + x87_tus.txt, fixture, ARG_TYPES.md regen | keep |
| (this) | evidence and this note | keep |

## Path2 (orchestrator)

Handler keys: `file_write_wrapper_i3`, `display_init`, `viewport_scaled_rect_draw` (arg_type
`scalars_to_scattered_globals`, `freeze_main`, `call_log` with `at` entries). Not run here. Installs are live:
`RH_ScopedInstall(FileWriteWrapper_i3, 0x004b3bb0)` Save/SettingsAndIO.cpp:176,
`RH_ScopedInstall(DisplayInit, 0x004921d0)` Boot/SubsystemInit.cpp:309,
`RH_ScopedInstall(ViewportScaledRectDraw, 0x00428610)` Frontend/ViewportScaledRectDraw.cpp:106.
Path2 replaces 0x00498bc0 / 0x00498bd0 (the `fix_camera_res` bytes) only while the main thread is frozen, and reverts
them after the last test.

## Proposed re-classify transaction (not applied)

hooks.csv:
- `004b3bb0` FileWriteWrapper_i3: C2 -> **C3**; frida_diff
  `re/analysis/negctl/session_20261012b/bnormalise_evidence/file_write_wrapper_i3.csv`; append: "bnormalise
  2026-10-12: C2->C3. The non-reproducible return came from 0x004cc230's fresh stream. Recorders on 0x004cc230 /
  0x004cbe80 / 0x004cc160 with seeded returns (z_fix_bnormalise.py) log every call's args and fold the return. GREEN
  7/7, self-baseline GREEN, negctl null RED 7/7, ret RED 7/7, field NA, witness close(handle) RED 5/7 (return
  unchanged, so the old channel was blind). Port unchanged. Ref re/analysis/negctl/session_20261012b/bnormalise_RESULT.md".
- `004921d0` DisplayInit: C2 -> **C3**; frida_diff `.../bnormalise_evidence/display_init.csv`; append: "bnormalise
  2026-10-12: C2->C3. Recorders on the 10 callees that allocate, re-create cameras or read RW state (new keys deref
  'l' and outs/stub_outs for the 0x004997b0 out-pair). 0x0042b890/0x0042b8a0 are real, and 0x0077195c+8 and
  0x0067ea54+4 are observed. GREEN 6/6, self-baseline GREEN, negctl null RED 6/6, field_first RED 6/6,
  field_last RED 6/6, witness swapped out-pointers RED 4/6. Port unchanged. Ref .../bnormalise_RESULT.md".
- `00428610` ViewportScaledRectDraw: C2 -> **C3**; file **`mashedmod/src/mashed_re/Frontend/ViewportScaledRectDraw.cpp`**
  (was Frontend/Cluster_v3.cpp); frida_diff `.../bnormalise_evidence/viewport_scaled_rect_draw.csv`; append:
  "bnormalise 2026-10-12: C2->C3. Re-ported from the listing as an x87 TU (3 defects: coordMode not 0..2 leaves
  x=y=0, the coordMode-2 chain keeps sx*x in ST, the screen dims are sign-extended). 0x00450b10 is a recorder, so
  0x007d709c (render state) is no longer written by either arm. GREEN 12/12 at CW 0x27f and 0x7f, self-baseline GREEN
  at both, negctl null RED 12/12, field NA, old body RED 6/12. Ref .../bnormalise_RESULT.md".

STUBS / UNCERTAINTIES / DEFERRED: none added or closed by this branch. U-0287 / U-0288 (on 0x004b3bb0) concern the
close argument, and the evidence now logs `close(write_result, 0)` on both arms. The re-classify pass may read them
and decide whether they close. This branch did not read them. CHANGELOG: one entry for the three promotions and the
new recorder keys.

C4 is not claimed. It needs path2 plus a canonical-scenario install-observe with the inline JMP live.

## Decisions needed

None. No forced input, no differently-patched MASHED.exe, no new dependency, and nothing written under `original\`.
