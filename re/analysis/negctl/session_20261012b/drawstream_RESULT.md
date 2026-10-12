# drawstream: draw-stream capture for the run_diff fixture lane, 0x00427ad0 + 0x00430b90 re-earned (2026-10-12)

Branch `work/drawstream` (base 8da9ecf2). Triage `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md` section B,
rows 0x00427ad0 and 0x00430b90, order item 9. Built with `MASHED_NO_DEPLOY=1`; the shared `original\mashed_re_dev.asi`
was never touched. No path2, no tracker edits.

## Verdict per row

| row | hook key | port vs the bytes | verdict |
|---|---|---|---|
| 0x00427ad0 MenuMenusBB | `menu_menus_bb` | precision defect: the rect chain rounded `w*xs` to float before `*viewport` (SSE2) | FIXED (own x87 TU), every C2->C3 leg except path2 passed |
| 0x00430b90 ProgressBarSetA | `progress_bar_set_a` | the old body did not follow the listing (8 defects, below) | RE-PORTED from the listing (own x87 TU), every C2->C3 leg except path2 passed |

**Ready for path2: `menu_menus_bb`, `progress_bar_set_a`.**

## The new capability: draw-stream capture

Built as an extension of the `call_log` recorders (followup/a2-stubat) in `scalars_to_scattered_globals`, not a new
handler. A draw callee is a named recorder at its address, installed only while the game's first thread is frozen
(`freeze_main`), and each arm's ordered call sequence with its arguments is the fingerprint. New on this branch:

| key | where | what |
|---|---|---|
| `deref: {i: 'w' \| 'a'}` | call_log entry | log the NUL-terminated UTF-16 / byte string at arg i (max 256 units), not its address. A caller-local buffer has a different address on each arm, its text does not |
| `deref: {i: 'f<n>'}` | call_log entry | log n float32s read at arg i by value (rects, vertex data) |
| `fargs: [i, ..]` | call_log entry | log arg i as its float32 value. JS prints the shortest string that round-trips the f32, so two bit patterns never print alike. `-0` and `nan:<bits>` are explicit |
| `ret_type: 'float'` | call_log entry | the recorder returns `ret` / `stub_rets` in ST0 (needed for 0x004282a0, whose ST0 the caller feeds to the ftol 0x004a2c48). Mirrored in `verify_hook_install_template.js` |
| `x87_cw` / `run_diff.py --x87-cw=0xNNNN` | CONFIG / CLI | load that x87 control word on the A/B thread around each arm, restore after. The CW in force is always logged (`x87 CW: thread 0x27f ...`). CSV names get `_cw<hex>` |
| `{scratch: off, minus: base}` | seed value | scratch+off-base, for base-relative tables (0x00427780 returns `[0x66d828 + id*4] + 0x66d828`) |
| `log/drawstream_<key>.csv` | run_diff.py | written for every call_log hook: one row per recorded call per vector, both arms side by side. A RED prints the first divergent call per vector |
| scratch bound check | diff_template.js | a `{scratch: off}` address past `scratch_size` now errors. Before, a seed there wrote into Frida's heap and hung the run (it happened once on this branch, see "Incidents") |

How to use it on another draw function:

1. Get the callees by Ghidra function boundary: `py -3.12 re/tools/decomp_pc.py <rva> --callees` (from the main
   checkout, the worktree has no pool). Read the listing (`re/tools/disasm_fn.py <start> <end>`) for each call site's
   pushes and any register args.
2. Make every DRAW callee a `call_log` entry with `at`, `nargs`, and `fargs` / `deref` for its float and pointer args.
   Leave computing callees REAL and seed their globals. Before replacing a callee check the caller does not rely on a
   register the recorder clobbers (0x00430b90 relies on EDX surviving 0x0042b8b0, so 0x0042b8b0 stays real).
3. An indirect slot (`call [*(0x007d3ff8)+0x20]`) takes a `call_log` entry without `at`, seeded as
   `{'addr': {'scratch': 0x20}, 'val': {'call_log': 'rs'}}` with `*(0x007d3ff8) = {scratch: 0}`.
4. Run `run_diff.py <key>` and `run_diff.py <key> --x87-cw=0x7f` when the body has an x87 chain, plus `--self-baseline`
   at both. Read `log/drawstream_<key>.csv` for a RED.

Fixtures: `re/frida/negctl_fixtures/z_fix_drawstream.py` (replaces both entries whole). `re/frida/ARG_TYPES.md`
regenerated (only two line numbers moved, no new arg_type).

## 0x00427ad0 MenuMenusBB

**Bytes** (0x00427ad0..0x00427bdd): `0x00427af2 call 0x427780` (id), `0x00427afb call 0x4277a0` (EAX string, EBX
buffer), `0x00427b00 call 0x552d10`, `0x00427b2c call 0x556e90(ctx, &argb x4)`, then the rect at local_214:
`0x00427b31 fld w; 0x00427b38 fmul [0x5cd5a8]; 0x00427b47 fmul [0x67d830]; 0x00427b54 fstp` (w),
`0x00427b5c fld h; fmul [0x5cc560]; fmul [0x67d834]; 0x00427b75 fstp` (h), `0x00427b79..0x00427b86` x,
`0x00427b8a fld y; fmul [0x5cc560]; 0x00427b97 fsubr [0x5cc320]; 0x00427b9d fsub [h]; 0x00427ba1 fstp` (y),
`0x00427ba5 fld size; fmul [0x5cd5fc]; fstp`, `0x00427bb7 call 0x5555b0(font, buf, size, &rect, 1, colour ctx)`,
`0x00427bbf call 0x552d70`.

**Defect.** The old body (Frontend/MenuMenusB.cpp, SSE2) computed `param_4 * x_scale * vp_w` in float, rounding the
first product where the listing keeps it in ST. The new draw-stream fixture read it RED 5/7 at the harness CW 0x27f,
first divergence at the `text` call's rect, e.g. idx 4 orig `@0/-767.0000610351562/1024/768.0000610351562`, port
`@0/-767/1024/768`. For idx 0, 1, 3 and 4 the old port's values equal the ORIGINAL's at CW 0x7f (compare
`drawstream_menu_menus_bb_cw7f.csv`), so the old body only agreed with the original at PC_24. The old fixture could
not see this: the rect reached `0x005555b0`'s render slot, which it stubbed with a bare `ret`.

**Fix.** MenuMenusBB moved to `mashedmod/src/mashed_re/Frontend/MenuMenusBB.cpp`, listed in `mashedmod/x87_tus.txt`,
ST-resident values as `double`, rounded to float only where the bytes `fstp dword`.

**Fixture.** Recorders `push` 0x00552d10, `color` 0x00556e90 (4 colour pointers dereferenced), `text` 0x005555b0
(buffer as UTF-16, size by value, rect as 4 floats), `pop` 0x00552d70. 0x00427780 / 0x004277a0 stay real.
DAT_0067d838 / DAT_0067d83c are sentinels. Three vectors point string-table entry 2 at a scratch string holding the
control codes 8..0xe (remapped by 0x004277ba..0x0042780e to 0x81 0x7f 0x81 0x8d 0x80 0x87 0x8f) plus 0x0f, 0x81, 0x7f,
which pass through. Four vectors use live ids 1, 5, 20, 40, and at attach time those entries read as empty strings
(`w""`) on both arms, so string content is covered by the three seeded vectors only. 7 vectors, viewport 640x480 up
to 1680x1050, fractional coordinates.

## 0x00430b90 ProgressBarSetA

**Bytes** (0x00430b90..0x0043122d, jump table 0x00431230 = 0x00430d54 / 0x00430f3d / 0x0043107a / 0x004311b7).
Callees by Ghidra boundary: 0x0040bb50, 0x00427e00, 0x004282a0, 0x0042ac00, 0x0042ac50, 0x0042b8b0, 0x0042b8c0,
0x00472c60, 0x00473870, 0x004739f0, 0x004a2c48, plus the render-state slot `call [*(0x007d3ff8)+0x20]`.

Defects in the old body (Frontend/SpriteCluster.cpp), each corrected from the listing:

1. Render states (6,0),(8,0) are issued BEFORE the gate (0x00430b99..0x00430bbe). The early-out (mode 0, alpha < 0x60
   signed) issues (6,1),(8,1) and returns (0x00430bd8..0x00430bf8). The old body returned first and issued none.
2. 0x0042ac00 counts 0xff060000 tokens in the dword stream at **EDX** = `*(0x0067ed38 + DAT_0067e9f8*64)` for mode 1,
   else `*(0x0067ed78 + ...)` (0x00430bfe..0x00430c18). 0x0042ac50 takes the count in **EAX** and 0x1e in **ECX**
   (0x00430c27) and returns the first row's y as an int. The old body called both with no register setup.
3. Three colour words, alpha byte only: `0x??2080d8` (0x00430b9d..0x00430bac), `(a>>1)<<24` and `a<<24`
   (0x00430c4b..0x00430c7a). The old body used one colour for everything.
4. Per row, 0x0040bb50("Arrow" at 0x005cd828) is called twice, once before each sprite. The old body called the in-TU
   SpriteLookupC once, outside the loop.
5. Left sprite 0x004739f0(spr, x, y, w, h, a<<24, 0x3f7ff972, 0, 0, 1.0, 0, 0) with x = (short)W*0x164/640,
   y = ((rowY - 8.0) * (short)H) * [0x5cc560], w = W*[0x5cc9a4], h = H*[0x5cc324] (0x00430c7e..0x00430d3c).
6. Rows 0..2: five quads at 374.0 / 477.0 (0x43ee8000, the old body had 461.0) and a fill quad at x 377.0
   (0x43bc8000, the old body had 375.0), height 10.0 (0x41200000, the old body had 18.0), drawn only when the
   UNROUNDED product `fill * 100.0 > 1.0` (0x00430e5a fcomp + `test ah,0x41`; NaN skips).
7. Row 3: id = 0x59 / 0x1b2 / 0x1b1 for DAT_007f0f10 = 0 / 1 / 2, otherwise the stale 0x0042ac50 result still in
   [L+0x24]; 0x00427e00(id, 376.0, rowY, a<<24, 0.7, 0), then edx = ftol(0x004282a0(id, 0.7)) (0x004311b7..0x00431229).
8. Right sprite 0x00473870(spr, (short)W*(edx+0x17e)/640, y, w, h, a<<24, 1) with edx = 0x66 except after row 3
   (0x00430e94..0x00430eec). Alpha is re-read from 0x0067e7fc per row (0x00430efb).

**Port.** `mashedmod/src/mashed_re/Frontend/ProgressBarSetA.cpp`, listed in `x87_tus.txt`. Register-ABI callees
through naked call shims; 0x004282a0 and 0x004a2c48 called back to back in one shim so ST0 passes untouched; every
callee called at its original address. Every float literal was decoded from the immediates in the listing with Python
after the first run caught the two literals copied from the old comment (fix in the same commit, before any evidence).

**Fixture.** Recorders `rs` (slot), `quad` 0x00472c60, `lookup` 0x0040bb50 (string arg), `spr12` 0x004739f0, `spr7`
0x00473870, `icon` 0x00427e00, `tsize` 0x004282a0 (float return). Real: 0x0042ac00, 0x0042ac50, 0x0042b8b0,
0x0042b8c0, 0x004a2c48. 9 vectors: early-out, 0 rows, 1 / 3 / 4 / 5 / 6 / 40 rows, modes 0 / 1 / 2 / 5, alpha 0x20,
0x60, 0x7f, 0x80, 0x90, 0xff, 0x1a0, -127, screen 320x240 to 0x7fff x -2, fills including NaN, 0.0100000007 and 1e30,
selector 0 / 1 / 2 / 7 (stale-id path, logged `icon(0xc3,..)`), tsize 37.9 / 12.5 / -3.75 / 3e9 / 99.5. Streams A and
B carry different counts, so a wrong table pick changes the row count. Vector 8 (40 rows, H = 0x7fff) pushes
`|(rowY-8)*H|` past 2^24, so the original's own output differs between CW 0x27f and 0x7f on 10 calls.

## CW measurement (x87 TU choice)

| row | original differs between CW 0x27f and 0x7f | port at 0x27f | port at 0x7f |
|---|---|---|---|
| 0x00427ad0 | 5 of 28 calls (5 of 7 vectors) | GREEN 7/7 | GREEN 7/7 |
| 0x00430b90 | 10 of 409 calls (vector 8 only) | GREEN 9/9 | GREEN 9/9 |

The harness thread runs at 0x27f. Without `--x87-cw` every earlier run_diff of a float body measured PC_53 only.

## Evidence (`re/analysis/negctl/session_20261012b/drawstream_evidence/`)

Final build: both TUs recompiled "(source newer)" after the witness restore; `.asi loaded` from the worktree build.

| leg | menu_menus_bb | progress_bar_set_a | files |
|---|---|---|---|
| run_diff, CW 0x27f | GREEN 0/7 | GREEN 0/9 | `<key>.csv`, `drawstream_<key>.csv` |
| run_diff, CW 0x7f | GREEN 0/7 | GREEN 0/9 | `<key>_cw7f.csv`, `drawstream_<key>_cw7f.csv` |
| --self-baseline, CW 0x27f | GREEN 0/7 | GREEN 0/9 | `<key>_selfbaseline.csv` |
| --self-baseline, CW 0x7f | GREEN 0/7 | GREEN 0/9 | `<key>_selfbaseline_cw7f.csv` |
| negctl real / baseline | GREEN / GREEN | GREEN / GREEN | `negctl_drawstream_20261012_191038.tsv` |
| negctl null | RED 7/7 | RED 9/9 | same |
| negctl ret | SKIP (void) | SKIP (void) | same |
| negctl field_first / field_last | NA (flips=0) | NA (flips=0) | same |
| negctl field_none | CTRL-GREEN | CTRL-GREEN | same |
| witness (geometry-only defect) | RED 7/7 | RED 7/9 | `<key>_witness_red.csv`, `drawstream_<key>_witness_red.csv` |
| re-run after restore | GREEN (rows above) | GREEN (rows above) | |

negctl command: `py -3.12 re/frida/run_negctl.py --handlers scalars_to_scattered_globals --hooks
menu_menus_bb,progress_bar_set_a --any-conf --classes null,ret,field_first,field_last,field_none --slots 1`.

**Why the REDs are through the draw stream.** Both fixtures have `observe: []` and `fold_ret: False`, so the call log
is the only fingerprinted channel. null RED: the arm makes no calls. Field mutants are NA because there is no memory
window to flip (flips=0): this function class writes nothing but draw calls. The witness is the field-class
substitute, a defect that changes ONE draw argument and nothing else:
- BB: `rect[1] = (c320 - y*ys) + h` instead of `- h`. RED 7/7, every vector's first divergence is call 2 (`text`),
  e.g. idx 0 y `-41.06188201904297` vs `42.738128662109375`, other rect members equal.
- PBSA: fill quad height 10.5 instead of 10.0. RED 7/9, first divergence is the fill quad, e.g. idx 1 call 9
  `quad(377,191,50,10,..)` vs `quad(377,191,50,10.5,..)`. The 2 GREEN are idx 0 (early-out) and idx 4 (0 rows), which
  draw no fill quad on either arm.
Under the old fixtures both defects were invisible: BB's rect only reached a stubbed render slot, and PBSA had
`arg_type: none` + `crash_equal_ok` (both arms AV'd on an uninitialised device).

## Incidents (no effect on evidence)

- The first 40-row PBSA vector put stream A (+0x100, 324 bytes) into stream B (+0x200) and stream B past the 0x300
  scratch; the run hung. I killed my own PIDs only (MASHED 21500, run_diff 9348 and its child 31780, then MASHED 35824,
  the hung loop's second iteration, which held the .asi open). Fixed by spacing the streams (+0x100 / +0x400, 0x700
  scratch, assert in `_stream`) and by the new scratch bound check in `resolveAddr`. All evidence above is from runs
  after the fix.
- A first witness attempt never applied (a failed `rmdir` stopped the command chain); its GREEN copies were deleted
  and the witness above was re-injected, rebuilt "(source newer)" and run.

## Commits

| commit | content | keep/drop |
|---|---|---|
| `197985e7` | call_log draw-stream extension, `x87_cw`, drawstream CSV, path2 mirror of `ret_type`, ProgressBarSetA.cpp and MenuMenusBB.cpp x87 TUs, old bodies removed, fixture | keep |
| `6410e555` | scratch bound check, 40-row CW vector + stream layout, ARG_TYPES.md regen, evidence, this note | keep |
| (this) | commit hash filled in this table | keep |

## Path2 (orchestrator)

Handler keys: `menu_menus_bb`, `progress_bar_set_a` (arg_type `scalars_to_scattered_globals`, `freeze_main`,
`call_log` with `at` entries). The `ret_type: 'float'` mirror in `verify_hook_install_template.js` passes
`node --check`; it has NOT been run. Both installs are live: `RH_ScopedInstall(MenuMenusBB, 0x00427ad0)` in
Frontend/MenuMenusBB.cpp and `RH_ScopedInstall(ProgressBarSetA, 0x00430b90)` in Frontend/ProgressBarSetA.cpp.

## Proposed re-classify transaction (not applied)

hooks.csv:
- `00427ad0` MenuMenusBB: C2 -> **C3**; file `mashedmod/src/mashed_re/Frontend/MenuMenusBB.cpp`; frida_diff
  `re/analysis/negctl/session_20261012b/drawstream_evidence/menu_menus_bb.csv`; append to notes: "drawstream
  2026-10-12: C2->C3. Draw-stream fixture (z_fix_drawstream.py) records 0x00552d10 / 0x00556e90 / 0x005555b0 (rect as
  4 floats, buffer as UTF-16) / 0x00552d70. Old SSE2 body RED 5/7 at CW 0x27f (rect rounded w*xs early); moved to an
  x87 TU, GREEN 7/7 at CW 0x27f and 0x7f, self-baseline GREEN at both, negctl null RED 7/7, witness (rect y) RED 7/7.
  Ref re/analysis/negctl/session_20261012b/drawstream_RESULT.md".
- `00430b90` FUN_00430b90: C2 -> **C3**; file `mashedmod/src/mashed_re/Frontend/ProgressBarSetA.cpp`; frida_diff
  `re/analysis/negctl/session_20261012b/drawstream_evidence/progress_bar_set_a.csv`; append: "drawstream 2026-10-12:
  C2->C3. Re-ported from the listing (8 defects: render-state order, EDX/EAX/ECX register calls, colour words, lookup
  per sprite, literals 377/477/10.0, fill gate, case-3 stale id, ftol right-arrow offset) as an x87 TU. Draw-stream
  fixture GREEN 9/9 at CW 0x27f and 0x7f, self-baseline GREEN at both, negctl null RED 9/9, witness (fill height)
  RED 7/9. Ref re/analysis/negctl/session_20261012b/drawstream_RESULT.md".

STUBS / UNCERTAINTIES / DEFERRED: no row added or closed. CHANGELOG: one entry for the two promotions and the new
harness capability (draw-stream keys, `--x87-cw`).

C4 is not claimed: it needs path2 plus a canonical-scenario install-observe with the inline JMP live.
