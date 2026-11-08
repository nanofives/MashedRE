# RESULT C4R-DXLIB-FU (rows C4R-DXLIB-REFILE, C4R-DXLIB-NEWROWS, C4R-INITTIME; 2026-11-07)

**Verdict: GREEN.** 10 rows refiled (9 render -> `third-party-library[d3dx9-psgp]`, `___inittime` boot ->
`third-party-library[msvc-crt-tail]`), 41 rows added and all 41 promoted to C4 by route `libid` (L-93), 118 of the 159
candidate addresses refused with a reason each. U-9135 carries a correction note. Text and tracker only: no build, no
game run, no `mashedmod/` change, nothing from `tools/` committed.

Branch `orch/C4R-DXLIB-FU` from `orch/integration` @ 510fe2b98. Inputs: `verify/c4rdxlib_20261107/d3dx_sum2003.tsv`
(DirectX 9.0b SDK Summer 2003 `d3dx9.lib`, the release C4R-DXLIB chose) and `re/console/pc_funcs.csv`.

## 1. C4R-DXLIB-REFILE: 9 rows render -> `[d3dx9-psgp]`

| rva | identical to (d3dx9.lib object, symbol) | confidence (kept) |
|---|---|---|
| 0x004fb5a0 | `psgpmesh.obj` `c_D3DXPSGPUpdateSkinnedMesh` (1034 B) | C4 (libid, C4R-DXLIB) |
| 0x004ec839 | `d3dxmath.obj` `c_D3DXFloat16To32Array` (165 B) | C4 (match) |
| 0x004ec915 | `d3dxmath.obj` `c_D3DXVec2Hermite` (142 B) | C4 (match) |
| 0x004ec9cd | `d3dxmath.obj` `c_D3DXVec2CatmullRom` (175 B) | C4 (match) |
| 0x004ecaab | `d3dxmath.obj` `c_D3DXVec2BaryCentric` (67 B) | C4 (match) |
| 0x004ecb97 | `d3dxmath.obj` `c_D3DXVec3Hermite` | C4 (match) |
| 0x004ecc6f | `d3dxmath.obj` `c_D3DXVec3CatmullRom` | C4 (match) |
| 0x004f022d | `d3dxmath.obj` `c_D3DXMatrixShadow` | C4 (match) |
| 0x004f0307 | `d3dxmath.obj` `c_D3DXMatrixReflect` | C4 (match) |

Sub-tag rule: the tag of the existing d3dx9 rows that are `identical` to a function of the same object. `psgpmesh.obj`
and `d3dxmath.obj` map only to `[d3dx9-psgp]` (1 and 89 identical rows). Each row's notes now start with the identity and
`refiled render->third-party-library[d3dx9-psgp] C4R-DXLIB-REFILE 2026-11-07, U-9135 correction`. Name, confidence,
status and file are unchanged.

U-9135 (`UNCERTAINTIES.md`, resolved 2026-09-12) gets a `**CORRECTION 2026-11-07 (C4R-DXLIB-REFILE):**` note. The
row's per-row note on 0x004fb5a0 called it a caller of PSGP bodies, "a client of D3DX, not D3DX itself". The bytes
show it is the D3DX function itself, and that function calls other PSGP bodies (0x004ecf29, 0x004ecf53). The note
also lists the 8 match-route rows.

## 2. C4R-INITTIME: 0x005c2a0e `___inittime`

Subsystem `boot` -> `third-party-library[msvc-crt-tail]` (the bucket of `0x005c0000..0x005d0000`, same rule as
`libid_reclass.py crt-waive`). Status `mapped` -> `waived-platform-crt`. Notes start with the L-67 waiver in the form
the other msvc-crt-tail rows use: `identified, waived: platform CRT (L-67); libid identical ___inittime (libcmt.lib
obj\i386\clock.obj @comp.id 28/9178); libmatch: ... (verify/c4rlibid_20261107/crt_libcmt.tsv)`. The `@comp.id` was
read from the DDK `libcmt.lib` member with `coffpe`. Confidence C4 is kept (libid, C4R-LIBID).

## 3. C4R-DXLIB-NEWROWS: 159 addresses, 41 added, 41 promoted

`newrows.py` reads every `identical-unlisted` address in the 2003 TSV that has no hooks.csv row (159) and checks each:
one library function and one object at the address, a sub-tag from the object (rule in section 1), a pc_funcs entry
at the address, and pc_funcs `body_size` equal to the symbol size. Output: `candidates.tsv` (all 159, with the
reasons).

**Added (41):** 40 `[d3dx9-psgp]` (`d3dxmath.obj` 1, `d3dxmathsse.obj` 5, `x3d_quat.obj` 7, `x3d_matx.obj` 15,
`x3d_vec3.obj` 4, `x3d_vec2.obj` 4, `x3d_plane.obj` 4) and 1 `[d3dx9-shader-compiler]` (0x0050245e
`D3DXShader::compare`, `cassembler.obj`). Every pc_funcs body size equals the symbol size. Row format: name = the
undecorated identifier of the library symbol (`?x3d_D3DXMatrixMultiply@@YG...` -> `x3d_D3DXMatrixMultiply`,
`?compare@D3DXShader@@...` -> `D3DXShader::compare`, `_x3d_D3DXVec3Transform@12` -> `x3d_D3DXVec3Transform`), full
symbol in notes. Status `new`, port `none`.

Baseline confidence: **C1, not C2.** The brief asked for C2. `re/CONFIDENCE.md` C1->C2 needs a plate (a hand-read
decompilation or a checked `c2_note.py` plate), and none of these addresses has one. The re-classify skill's library
add operation (`libid_reclass.py add-rows`) writes these rows as C1 ("the identity names the function, the library
fixes the subsystem"). All 41 then went to C4, so only the CHANGELOG `C1->C4` text differs from a C2 baseline.
`libid_reclass.py` itself refuses this hooks.csv (no csv round trip at 0041d820), so `apply.py` does the same edit
line-based.

**Promoted:** `c4_reclass.py batch --tsv added_batch.tsv` dry run 41 PASS / 0 REFUSED (`dryrun_output.txt`,
`dryrun_verdicts.tsv`), then `--apply --result verify/c4rdxlibfu_20261107/RESULT_C4R-DXLIB-FU.md --date 2026-11-07`:
41 rows -> C4, 41 CHANGELOG lines (`c4_apply_output.txt`). No added row was ambiguous, because uniqueness is a check
before the add.

**Refused (118),** grouped by the first check that failed (`refused_summary.txt`, full reasons in `candidates.tsv`):

| reason | n | addresses |
|---|---|---|
| A ambiguous identity: several library functions or objects at the address | 22 | 0x004ecb01, 0x004ecb25, 0x004ecb49, 0x004ecb6d, 0x004ed05f, 0x004ed19c (`init_` stubs), 0x00505ff8 (`sse2_` twin), 15 MASM labels in `0x00508b7a..0x00509956` (`TransformLoop` / `ProcEpilog` / `ProcPrologue` present in 2 or 3 objects) |
| B sub-tag ambiguous: `d3dxmathx86.obj` maps to `[d3dx9-psgp]` (3 rows) and `[d3dx9-shader-compiler]` (0x004fcb51) | 5 | 0x004fc6f5, 0x004fc75e, 0x004fc793, 0x004fc81e, 0x004fc87d |
| C no sub-tag: the object is matched by no existing d3dx9 row (`d3dxvec2.obj` 12, `x3d_vec4.obj` 6) | 18 | 0x00508d00 .. 0x005092e8 (12), 0x0050fc7c, 0x0050fcfe, 0x0050fd5d, 0x0050fe8f, 0x0050ff00, 0x0050ffc8 |
| D no boundary: the address lies inside another pc_funcs body (MASM labels of `FUN_00509340`, `FUN_005093e0`, `FUN_00509700`, `FUN_0050f700`, `FUN_0050f840`, `FUN_005137a0`, `FUN_005138a0`) | 17 | 0x0050937a, 0x00509420, 0x00509459, 0x00509561, 0x005096af, 0x00509740, 0x00509779, 0x0050983d, 0x0050992e, 0x0050f73f, 0x0050f7f2, 0x0050f812, 0x0050f883, 0x0050f982, 0x0050f9a2, 0x00513820, 0x00513920 |
| E no boundary: the address is in no pc_funcs body | 55 | 31 `c_D3DX*` / `init_D3DX*` bodies in `0x004ece75..0x004f03c6` (e.g. 0x004ece75 `c_D3DXVec3TransformArray`, 0x004ed6c0 `c_D3DXMatrixMultiply`, 0x004edbd2 `c_D3DXMatrixInverse`), 0x004fba47 `c_D3DXPSGPTessellateNPatch`, 0x00504489 / 0x005044bd `CBuffer::Init` / `Release` (also sub-tag ambiguous), 0x00510809 / 0x005108cd `IsEqual`, 19 math entries in `0x00513000..0x00513ac0` (`_a_atan`, `_a_sqrt`, `_a_pow`, ...) |
| F size: pc_funcs body 379 B, symbol `ProcPrologue` 67 B (the identity covers only the first 67 B) | 1 | 0x0050f840 |

Follow-ups, not done here: E needs Ghidra functions at those starts (transient-create or a master write), then pc_funcs
regenerated and this check re-run. 36 of the 55 are named D3DX bodies with one candidate (`c_` / `init_D3DX*`, `CBuffer`, `IsEqual`). The other 19 are the `_a_*` math entries and their labels.
B and C need a sub-tag decision: tag by object, by address band, or a new tag. D and F are MASM labels inside one
assembled routine. A row there would need a per-routine identity over all its labels, which libmatch does not produce.

## 4. Port column, lint, ladder

`scripts/backfill_port.py --allow-stale --build-dir ../D4-FLIP-ST5-LEG2/mashedmod/build` (the build dir the last batch
used): `rows changed: 0`, `hooks.csv already current` (`backfill_port_output.txt`). `--allow-stale` is needed because
a fresh worktree checkout makes every source newer than the map. That build's `mashedmod` tree (3a11f01e) is not this
branch's (5e598fea), and no worktree build matches 5e598fea. This branch changes no `mashedmod/` file, and the 51
touched rows are library rows whose port is `none`, so the result does not depend on that build.

`py -3.12 re/orchestrator/tracker_lint.py --repo . --base orch/integration`: before `OK ... hooks.csv 5974 rows ...
pre-existing=3 NEW=0`, after `OK ... hooks.csv 6015 rows ... pre-existing=3 NEW=0` (`lint_before.txt`,
`lint_after.txt`).

`py -3.12 verify/c4rmatchbatch_20261107/ladder_counts.py hooks.csv` (`ladder_before.json`, `ladder_after.json`):

| | before | after |
|---|---|---|
| overall | C1 10, C2 2306, C3 221, **C4 3437**, other 32 (6006) | C1 10, C2 2306, C3 221, **C4 3478**, other 32 (6047) |
| game | C2 719, C3 103, **C4 2556**, other 32 | C2 719, C3 103, **C4 2546**, other 32 |
| library | C1 10, C2 1587, C3 118, **C4 881** | C1 10, C2 1587, C3 118, **C4 932** |

Game C4 -10 (render 701 -> 692, boot 151 -> 150). Library C4 +51 (the 10 refiled rows plus the 41 new ones):
`[d3dx9-psgp]` 144 -> 193, `[d3dx9-shader-compiler]` 69 -> 70, `[msvc-crt-tail]` 44 -> 45.

## 5. Tracker diff and merge note

`git diff --numstat`: hooks.csv 51 added / 10 removed (the 10 refiled lines rewritten in place, 41 rows appended at
the end, as `add-rows` does), UNCERTAINTIES.md 1 / 1 (U-9135), `re/analysis/CHANGELOG.md` +93 below `<!-- ENTRIES -->`
(1 U-9135 line, 9 refile, 1 inittime, 41 add, 41 C4). A text conflict at the CHANGELOG marker against a parallel
batch means keep both blocks. hooks.csv appends at the end conflict only with another branch that appends rows. Then
keep both and re-run tracker_lint for DUP.

## Files

`newrows.py` (candidate checks), `candidates.tsv` (159 addresses), `refused_summary.txt`, `apply.py` (tracker
transaction, line-based, idempotent), `apply_dryrun.txt`, `apply_output.txt`, `added_batch.tsv`, `dryrun_output.txt`,
`dryrun_verdicts.tsv`, `c4_apply_output.txt`, `backfill_port_output.txt`, `ladder_before.json`, `ladder_after.json`,
`lint_before.txt`, `lint_after.txt`, this RESULT.
