# RESULT C4R-DXLIB-TAG11 (2026-11-08)

**Verdict: GREEN.** 11 d3dx9 rows added under `third-party-library[d3dx9-psgp]` and promoted C1 -> C4 by route `libid` (L-93).
Decision: Mariano 2026-11-08, d3dx9-psgp (d3dxmathx86.obj 5 rows, x3d_vec4.obj 6 rows). The 12 d3dxvec2.obj labels stay out.
Text and tracker only: no build, no game run, no `mashedmod/` change.

- `rows.py` (copy of `verify/c4rdxlibghidrafn_20261108/rows_d3dx9.py`, restricted to the 11 addresses, sub-tag forced to psgp for the two
  objects): per address one library function, one object, pc_funcs entry present, pc_funcs body size == symbol size (no alignment-fill
  exception used). 11 add, 0 skip (`rows_dryrun.txt`, `candidates.tsv`, `rows_apply.txt`). Rows line-based, C1 baseline, name = undecorated symbol, full
  symbol in the note.
- `c4_reclass.py batch` route libid: 11 pass, 0 refused, 0 already C4 (`dryrun_output.txt`, `c4_apply_output.txt`).
- tracker_lint before/after: `lint_before.txt`, `lint_after.txt` (NEW 0). Ladder: `ladder_before.json`, `ladder_after.json`.
