# MATCH-S19 WIP (stopped run 5, 2026-11-02)

Stopped by the lead (loops paused for usage). Leg reached: **hand-matching (static leg), interrupted**. No body swaps,
no build, no gates.

- Slice 0x004c0740..0x004c7760: 117 game functions (`slice_rvas.txt`, `tools/mkslice.py` from MATCH-S16's copy).
- g2s raw converter: 20/117 at 100%. 9 of those were relocheck BAD (RwGlobals `DAT_007d3ff8` and the module offset
  `DAT_007d3ffc` / `DAT_007d4054` / `DAT_007d40a8` loaded in swapped order). Writing the RwGlobals pointer first,
  `(DAT_007d3ff8 + DAT_007d3ffc + k)`, fixed all 9.
- Flag scan of 8 near-misses over `/O2 /GS /Ob1`, `/O2 /Ob1`, `/O2`, `/G6`, `/Ox`, `/Ob2` moved nothing. The game
  flags stay.
- Helpers h1-h4 (`helper_assign.txt`, brief `BRIEF.md`) were stopped mid-run. At the stop: **59/117 byte-identical**
  (`results.tsv` best%), **relocheck OK on all 59** (`relocheck.tsv`). 96 functions have a source file. The helper
  notes as of the stop are in `helper_notes_h*.md` (h3 only got through 7 of its 24).
- Port bodies found before any swap: 53 of the 117 RVAs (`D:/MashedWT/MATCH-S19_data/bodies_pre.tsv`). Many are the
  exe+asi Math/ physics TUs (RwMatrixRotate, RwMatrixRotateInner, RwV3dNormalize, MatrixOrthoResidual, RwSqrt) with
  knobs. `SWAP_BRIEF.md` carries the strict NOT-SAFE rule for those.

Resume: re-run helpers on the functions still below 100% in `results.tsv` (h3's list first), then the swap leg per
`SWAP_BRIEF.md`, build with `MASHED_NO_DEPLOY=1`, lint, RESULT.

Raw data (not committed), `D:/MashedWT/MATCH-S19_data`:
- decomp.json sha256 31db8bf8728e4f0ef005f4b07487b19e5514812faaf5ca2b49620a134b9a5b27
- g2s/results.tsv sha256 dd299c7c623c2feab148a3124fe125874815ad4d8cfea0902a8ff45d70b10401
- bodies_pre.tsv sha256 ff40666cb015781d7bb929891f23d9540367c6b6c64049fef3179187240cd4e7
