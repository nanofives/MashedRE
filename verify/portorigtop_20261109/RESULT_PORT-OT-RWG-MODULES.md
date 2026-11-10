# RESULT PORT-OT-RWG-MODULES: the four RW modules registered on MASHED_ORIG_TOP (knob MASHED_ORIG_TOP_RWMOD), 14 rows released

Branch `orch/PORT-OT-RWG-MODULES`: orch/integration `1be11e86d` + orch/PORT-OT-RWG-SLOTS `f4b994948` (merge
`090e404de`, which carries PORT-OT-SEED-2 `a33c7f238`). PREREG `c81b8ac06` (committed alone), code `c22889724`.
Worker cmwa9vtqk1tftle1czfhnvys9.

## Verdict: GREEN

- With `MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1`, RwGlobals rows: **OK 61, needs binding 11**. PORT-OT-RWG-SLOTS had
  47 / 25, PORT-OT-MEASURE 30 / 42. This matches the PREREG prediction row for row.
- 14 rows released (`RWG_MODULES_RELEASED.tsv`):
  - 10 of the 12 module rows: 0x004c3b30, 0x004c3b90, 0x004c3bf0, 0x004c3d60, 0x004c3d90, 0x004c3dc0, 0x004c3df0,
    0x004c4600, 0x004c5770, 0x004c5890.
  - The four rows whose only blocker was call slot +0x11c: 0x00554050, 0x00555280, 0x00555830, 0x00556e40.
- No row went from OK to needs binding.
- Of PORT-OT-MEASURE's 42 needs-binding rows, 31 are now OK.
- Default path unchanged: G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3 (the `merge_train.json` refs).
  The knob-ON boot exits 0, and OFF and ON give the same 12 captures.

## The change (`LibRw/RwApiBind.cpp`, the only mashedmod file)

**Knob.**
- `MASHED_ORIG_TOP_RWMOD=1`, default OFF, read once (`RwModKnob`).
- It acts only inside `MashedRw_InstallRwGlobals`, which runs only on the MASHED_ORIG_TOP path (`Match/PortTop_00492370.cpp`,
  `Match/PortTop_00402750_Pieces.cpp`). The default path never reads it.
- Registration runs at the first install that sees the B15 device, which is the install right after piece 4
  (`PortTop_Piece_00403640`). Piece 4 points 0x007d3ff8 at the 0x34-byte B15 device until that install runs. Offsets
  registered before then would index past that device.

**Record.**
- `MashedRwGlobals` grows from 0x218 to 0x440 bytes, which covers +0x3f4 + 0x40 and +0x170 + 0x220.
- With the knob OFF, every byte past +0x114 stays zero, as before.

**Module offsets (MASHED.exe values, original-side probe "mods", set from 3.02 s on):**
`*(0x007d3ffc) = 0x134`, `*(0x007d4028) = 0x14c`, `*(0x007d4054) = 0x3f4`, `*(0x007d4628) = 0x170`.

**Module slots.** "orig" is the original-side probe at 33.02 s (`D:/MashedWT/PORT-OT-RWG-MODULES_data/orig/orig_probe.json`).
"exe" is the knob-ON exe probe at 30.02 s (`probe.json`).

| module + | orig | exe binding | exe probe |
|---|---|---|---|
| vector +0x0 | `0x0cc00048` (heap LUT root) | `RwLutSqrtTable()` (Math/RwLutTables.cpp, equal to the original's table, the sentinels of Math/RwLutGuard.h) | `0x00f30360` |
| vector +0x4 | `0x0cc04050` | `RwLutInvSqrtTable()` | `0x00f34360` |
| vector +0x8 | `0x004dc4a0` | `RwModPointSingle` = `RwV3dTransformPointsCPU_C(out, in, 1, m)` | `0x001b8490` |
| vector +0xc | `0x004dc370` | `RwV3dTransformPointsCPU_C` (Math/RwV3dTransformPointsCPU.cpp, u9759) | `0x0013be90` |
| vector +0x10 | `0x004dc530` | `RwModVectorSingle` = `RwV3dTransformVectorsCPU_C(out, in, 1, m)` | `0x001b84b0` |
| vector +0x14 | `0x004dc410` | `RwV3dTransformVectorsCPU_C` | `0x0013bf60` |
| matrix +0x0 (= vector +0x18) | `0x007d4004` | `0x007d4004` (freelist header, below) | `0x007d4004` |
| matrix +0x4 | `0x00020000` | `0x00020000` | `0x00020000` |
| matrix +0x8 | `0x005cb2a0` | `RwMatrixMultiplyCPU_C` (Math/RwMatrixMultiplyCpu.cpp, the exe body of 0x005cb2a0) | (no row reads it) |
| matrix +0xc / +0x10 / +0x14 | `0x3c23d70a` x3 | `0x3c23d70a` x3 | |
| image +0x218 | `0x007d4604` | `0x007d4604` (freelist header) | `0x007d4604` |
| image +0x21c | heap (`0x0365ce4c`) | 0: empty format list | `0x0` |
| texture +0x0 / +0x4 | heap (`0x0e684b24` / `0x0365a8dc`) | the list head's own address (empty, self-linked) | `0x00ffa754` |
| texture +0x8 / +0xc | `0x007d405c` / `0x007d4030` | the same (freelist headers) | `+0xc` `0x007d4030` |
| texture +0x1c | 0 | 0 | |

- **+0x8 / +0x10 are the batch bodies with count 1.** Checked against the disassembly of the patched-install binary
  (`MASHED.exe.unpatched`, read with capstone).
  - 0x004dc4a0 runs the same SSE sequence as 0x004dc370: mulps / addps at 0x004dc4fa..0x004dc50c against
    0x004dc3d2..0x004dc3e7. It takes (out, in, matrix) at [ebp+8 / +0xc / +0x10] and returns out (0x004dc524).
  - 0x004dc530 runs the same sequence as 0x004dc410: 0x004dc57e..0x004dc58d against 0x004dc466..0x004dc478. It
    returns out (0x004dc5a5).
- **Image +0x21c starts empty.** The original's list is filled by format registrations through 0x004ce790, called from
  0x00493600. The knob path has not ported 0x00493600 (`Match/PortTop_00493710.cpp` returns 1).
- **Texture +0x0 / +0x4 start empty.** The exe's dictionaries are librw's, so the RW-side list starts the way
  rwLinkListInitialize leaves it.
- **Slots left at 0, with no reader found on the knob path:**
  - matrix +0x18 / +0x1c / +0x20 (orig `0x007d3e78` / `0x007d45a0` / `0x007d3e9c`)
  - image +0x0..+0x217 (orig +0x0 `0x007d4630`, +0x4 heap, +0x8 `0x100`, then bytes 00 01 02 ...)
  - texture +0x14 / +0x18 / +0x2c / +0x30 (orig `0x004c61b0` / `0x004c7490` / `0x004c7200` / `0x004c6140`, which are
    original code addresses with no exe body)

  [UNCERTAIN] Whether any later knob-path body reads these slots. Evidence missing: a capstone pass over every
  ORIG_TOP row for these module displacements. `ot_measure.py lists` records only the displacements its 72 rows use.

**Freelist headers.** Read from the original at the four addresses the modules hold. The probe dumped 0x24 bytes at each
address per sample, and words +0x0..+0xc and +0x18 are constant from 3.02 s to 33.02 s:

| header | +0x0 entry size | +0x4 per block | +0x8 bitmap bytes | +0xc align | +0x18 flags | entry size matches |
|---|---|---|---|---|---|---|
| `0x007d4004` (matrix +0x0) | 0x40 | 0x100 | 0x20 | 4 | 3 | RwMatrix, 0x40 (re/match/s19/004c4600.c) |
| `0x007d4030` (texture +0xc) | 0x18 | 0x5 | 0x1 | 4 | 3 | RwTexDictionary, 0x18 (re/match/s19/004c5890.c) |
| `0x007d405c` (texture +0x8) | 0x60 | 0x80 | 0x10 | 4 | 3 | |
| `0x007d4604` (image +0x218) | 0x34 | 0x80 | 0x10 | 4 | 3 | ImgFmt, 0x34 (re/match/s20/004ce790.c) |

- The exe writes these words at the same addresses (the exe maps the original data range), with the block list
  (+0x10 / +0x14) empty.
- Field meanings come from the disassembly of 0x004cca80 / 0x004ccba0:
  - +0x0: `imul ecx,[ebp]` at 0x004ccaf1, `div dword ptr [edi]` at 0x004ccbde
  - +0x4: 0x004ccaa8 / 0x004ccbc1
  - +0x8: 0x004cca8e / 0x004ccbab
  - +0xc: the round-up at 0x004ccaf5..0x004ccb06
  - +0x10: block list head, 0x004cca8b / 0x004ccba8
  - +0x18: `test al, 2` at 0x004ccc03

**RwGlobals +0x118 / +0x11c.**
- Orig values: `0x004cca80` / `0x004ccba0`.
- +0x118 binding: `RwGlobalsFreeListAlloc(fl, hint)` makes one allocation per entry through the librw memory functions.
  It uses the size and alignment from the header and does not pool blocks. 0x004cca80 allocates its blocks through
  RwGlobals +0x108 (0x004ccb48), which is librw's malloc in this record.
- +0x11c binding: `RwGlobalsFreeListFree(fl, p)` keeps the two results of 0x004ccba0 (re/match/rwl01/004ccba0.c,
  BYTE-IDENTICAL):
  - returns 0 and frees nothing when the entry is not one this freelist handed out (0x004ccbd3..0x004ccbd9)
  - otherwise frees the entry and returns the freelist (0x004ccc42 `mov eax, edi`)
- 0x004cca80 has no matched source and no hooks.csv row. This binding is a stand-in, not a port of 0x004cca80.

**Behaviour the knob changes (the reason it is a knob).** With the knob ON, the exe readers of those globals stop
seeing offset 0:
- `Math/RwLutGuard.h` callers resolve the LUT roots (the sentinels match) and take the LUT path instead of the CPU
  fallback.
- `Collision/RwpSolverMath2.cpp` RWP_MATMUL / RWP_IDENTW read matrix +0x8 / +0x4.
- `Render/TextureLoaderCluster.cpp` reads and writes texture +0x8 / +0x1c.

Nothing changes with the knob OFF, and the default gates confirm it.

## Remaining 11 needs-binding rows (`OT_MEASURE.tsv`)

| row | blocker |
|---|---|
| 0x004ce790 | call slots +0xd0 +0xe8 +0xf8 +0xfc. Orig `0x004a34b0` _strncpy, `0x004af340` (msvc-crt), `0x004d86d0` RwStrupr, `0x004d8700` RwStrlwr. Also image +0x21c = 0 against the original's heap list. |
| 0x004ce8e0 | +0xdc (orig `0x004d8750` RwStrrchr), +0xe8. Image +0x21c empty until 0x00493600 registers the formats. |
| 0x004113b0 | +0xc4 (orig `0x004a2b60` sprintf) |
| 0x00427840 | +0xf4 (orig `0x004a9410` _strlen) |
| 0x00555910 | +0xe8 +0xf4 |
| 0x005adf60 | +0xe8 |
| 0x00413a40 | +0x24 +0x34 (orig `0x004d6910` D3d9State_Get, `0x004dbc90`) |
| 0x004430e0 | +0x34 |
| 0x004d52d0 | +0x60 (orig `0x004de1b0`) |
| 0x0047ade0 | +0x0 (orig holds 0 as well, RESULT_PORT-OT-RWG-SLOTS) |
| 0x004c1680 | data +0xbc (orig heap `0x02b49a64`) |

- Four of these need only RwGlobals string-function slots: 0x004113b0, 0x00427840, 0x00555910, 0x005adf60
  (+0xc4, +0xe8, +0xf4). Binding them follows the same pattern as PORT-TOP-04G's strcpy / strcat at +0xcc / +0xd4.
- 0x004ce790 and 0x004ce8e0 also need string slots (+0xd0, +0xdc, +0xe8, +0xf8, +0xfc), and on top of that a non-empty
  image format list (+0x21c), which needs 0x00493600's format registrations.

## Arms (PREREG)

**A1 knob boot: PASS.**
- Run: MASHED_ORIG_TOP=1 MASHED_ORIG_TOP_RWMOD=1 MASHED_DET_FRAMES=3000 (`ot_measure.py probe`), rc 0, 8 samples.
- The exe log has: `[RW_BIND] RwGlobals [MASHED_ORIG_TOP_RWMOD]: modules registered (0x007d3ffc 0x134, 0x007d4028
  0x14c, 0x007d4054 0x3f4, 0x007d4628 0x170), freelist alloc / free at +0x118 / +0x11c, record 00FFA360 size 0x440`.

**A2 measure: PASS.**
- `ot_measure.py verdict --orig` on this step's original probe: RwGlobals OK 61, needs binding 11. Seed OK 111, needs
  seed rule 13 (unchanged).

**A0 default gates: PASS.**
- `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe sha8 `11ed1669`:
  - H_DEF: step `8284be46`, gates `67931834`, ptrace `31fe043e`
  - B: `1d5bf4f9` x3

**A3 captures: PASS, against the current default.**
- `capflake_runs.py` OFF ON OFF ON with MASHED_ORIG_TOP_RWMOD=1 in every run's environment. Two sets:
  - arm RWMOD: pin OFF, freeze `live pos=0 hash=28538c74`
  - arm RWMOD_PIN0: `@0`, the worker rule, freeze `pinned pos=55 hash=28538c74`
- All 8 runs: rc 0, and all 12 hashes equal across OFF and ON.
- `capflake_runs.py` reports class `?` (11/12 equal to its E list). The one difference is walk_04:
  `4d53d93f87f0332d` against E's `2871487f1fcef51c`. That is the default since L-110 (a) made
  MASHED_COLOURSEL_BAR_Y default ON. `RESULT_PORT-COLOURSEL-BAR-Y.md:100` records walk_04 = `4d53d93f87f0332d...` with
  the knob ON. The E list in `verify/porttop_20261108/capflake_runs.py` predates that flip. The OFF runs are the default
  path and show the same walk_04, so the difference is not this step's.
- Deviation from the PREREG wording ("class E"): the pass test applied is OFF == ON on all 12 walks, plus
  E-with-BAR_Y-walk_04. Refreshing the E list is a harness chore for whoever owns capflake_runs.py.

**A4 lint: PASS.** `py -3.12 scripts/lint_rva_bodies.py`: `[rva-lint] OK -- no NEW duplicate bodies (2 known)`.

## Tooling

- `ot_orig_probe.py`:
  - `--modlen` (default 0x40) sets how much module data is read.
  - `--dump a:n[,a:n]` reads raw bytes at absolute addresses per sample.
  - `--out` names the output file.
- `OT_MEASURE.tsv`: regenerated from this step's exe probe plus `--orig`.
- `RWG_MODULES_RELEASED.tsv` (new): the 14 rows, each with its blocker before.

## Evidence (raw data in `D:/MashedWT/PORT-OT-RWG-MODULES_data`, not committed)

| file | SHA-256 |
|---|---|
| orig/orig_probe.json (original probe, 8 samples, modlen 0x220, freelist header dumps) | ae6d8a62f6ec97dc8f530f15796a8d4649864ab07cf7fa3ad6275a968b1cf2c2 |
| orig/orig_probe_run.log | 408cdcaf210cce42da03118776841e31bc77337cb05719b02e9b4884efe5ee87 |
| probe.json (exe, MASHED_ORIG_TOP=1 + RWMOD=1, rc 0) | 5b80d6c4e6bee6723862e5b058f85d8b3e1536439eb03a6bf6b867919ccb68dd |
| probe_run.log | 8f734f34ead03cebd63c1267669c61fe8f234b9f2e29a83661650fad7345dd7d |
| probe_mashed_re.log (the exe log of that boot, `modules registered` line) | 8c5c36651f740ed960dc22a2a0ed26a3697cf86dd4c7bc9d56dd9939f1a51869 |
| ot_rows.json (= PORT-OT-MEASURE / RWG-SLOTS) | a6dc8309f8ceaf08fc4df5b1694d27c9909422c8ed64383920d2c5e43bd87641 |
| OT_MEASURE_before.tsv (the RWG-SLOTS table) | cf7329a4dc3ba7a52d4f29817778ac8300de79edb32d11b8f7704ffc25522e89 |
| gates_run.log (A0) | 479c5fb1f6134812df81b05c1d7db2417aefdb16823f514cbd6bb105feb61ea1 |
| cap/runs.tsv (A3, both sets) | 41bc6bc08d10c80ea72c905fb8a01ee9322941b33dd8250863c19883fe254bd7 |
| mashed_re.exe (built from c22889724) | 11ed1669e13f7b2a421a0f8f4461e3eede16659f08fe920b0b4ab7dfa1617a87 |

Probed original binary: `D:/MashedWT/PORT-BATCH-TOOL_root/original/MASHED.exe`, the same file as in
RESULT_PORT-OT-RWG-SLOTS (SHA-256 `b9977dab...fe87`).

## REPORT

```
REPORT PORT-OT-RWG-MODULES
verdict: GREEN
branch: orch/PORT-OT-RWG-MODULES @ <RESULT commit>
evidence: verify/portorigtop_20261109/RESULT_PORT-OT-RWG-MODULES.md
gates: G-CTRL 8284be46/67931834/31fe043e  B 1d5bf4f9/1d5bf4f9/1d5bf4f9
decision: none
next: PORT-OT-RWG-STRINGS: bind the RwGlobals string slots +0xc4/+0xd0/+0xdc/+0xe8/+0xf4/+0xf8/+0xfc on the same knob (4G pattern), which releases 4 of the 11 (0x004113b0, 0x00427840, 0x00555910, 0x005adf60)
```
