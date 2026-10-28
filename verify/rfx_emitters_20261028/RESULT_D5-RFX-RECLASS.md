# RESULT D5-RFX-RECLASS: the D5-RFX-EMITTERS tracker transaction

Verdict: **GREEN**. The `RESULT_D5-RFX-EMITTERS.md` section 5 transaction is applied through re-classify. There is no
C-level change on any existing row. 7 new rows are at C1. Branch `orch/D5-RFX-RECLASS` off `orch/D5-RFX-EMITTERS`
`696021549`. The exact transaction with its gate checks is `RECLASS_TXN_D5-RFX-RECLASS.md`. The mechanical edit was
run by the tracker-editor agent through the csv module (cp1252, CRLF kept).

## What changed

- `hooks.csv`: 19 rows got `exe_file` set and a `[D5-RFX-RECLASS 2026-10-28]` note appended. Columns 1..8 are
  unchanged, verified cell by cell against `HEAD`. 7 rows were appended: `00536780` `00536e90` `00536fe0`
  `00537b20` `00537d20` `00537d70` `00537e60`, all `third-party-library[renderware]`, C1, mapped,
  exe_file `Render/RfxEmitter.cpp`.
- `re/analysis/CHANGELOG.md`: 26 lines below the exact `<!-- ENTRIES -->` line (28). The lines added are 29..54, and
  there are 0 deletions.
- STUBS / UNCERTAINTIES / DEFERRED: no change.

## Deviations from section 5 (ground truth over the candidate text)

- `004770a0`: exe_file is `mashedmod/src/mashed_re/LibRw/RwRaceFx.cpp`, not `RfxEmitter.cpp`. RfxEmitter.cpp has no
  citation of it. Its exe body is `RaceFx_DrawEmitter` at `RwRaceFx.cpp:2248`, an exe-only TU compiled from
  `mashedmod/build.bat`.
- `00534b60` already had a row (C2 mapped). It got exe_file only, so there are 7 new rows, not 8.

## Gates

- C1 for the new rows: there are no analysis notes, so C2 is not met. Subsystem is the same as `00534b60`. The xrefs
  come from a byte scan of `original/MASHED.exe.unpatched`. `0x00536780` is called at `0x00536337` (E8).
  `0x00536e90` appears as an immediate at `0x00536366` / `0x00536843`, `0x00536fe0` at `0x00536804`, `0x00537b20` at
  `0x005363c2` / `0x005368e2`, `0x00537d20` at `0x00536913`, `0x00537d70` at `0x0053687b` and `0x00537e60` at
  `0x005368ad`. `decomp_pc.py` reports `0x00536780` as never disassembled, so it is Ghidra-missed like the six
  callbacks.
- Body check: `Render\RfxEmitter.cpp` is in `exe_sources.rsp:78`, and a `// 0x<rva>` citation or an `Instance` body
  reference exists for each RfxEmitter row.
- `scripts/lint_rva_bodies.py`: CROSS-TARGET=16, allowlisted=16, NEW=0. It does not read hooks.csv.
- No build and no game run were needed, because no source changed.
