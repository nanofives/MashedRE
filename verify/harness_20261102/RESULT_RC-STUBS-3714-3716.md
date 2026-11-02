# RESULT RC-STUBS-3714-3716 (text only) -- BLOCKED on tooling, verification complete

## Verdict per row (STUBS.md lines 857-859, status `passthrough`)
S-3714 / S-3715 / S-3716 say FUN_00546b10 calls FUN_00546bf0 / 00546c50 / 00546cb0 as
passthroughs to the original. Current source shows all three are native ports:
- `mashedmod/src/mashed_re/Collision/RwpSolverMath2Quat.cpp`: 0x00546b10 (Shoemake dispatcher),
  0x00546bf0 (X branch, line 87), 0x00546cb0 (Z branch, line 105), each with `RH_ScopedInstall`
  (lines 149, 159) and shadow A/B counters; no `call_original` in the file. Moved there 2026-11-01
  (D4-WSB-55B080-ULP; `mashedmod/x87_tus.txt` lines 108-111, x87 build).
- `mashedmod/src/mashed_re/Collision/RwpSolverMath2QuatY.cpp`: 0x00546c50 (Y branch), 0 `call_original`,
  x87 replay GREEN 1200/1200 (`log/diff/00546c50_rwp_mat2quat_y.replay.csv`, x87_tus.txt line 106).
- 0x00546b10 itself dispatches to the three ported bodies (Quat.cpp lines 73-77).
- No `// STUB S-3714/3715/3716` comment exists anywhere in `mashedmod/src` (only analysis notes cite the IDs).
All three rows are STALE: close as resolved, evidence = the source above.

## Why no edit was made
The re-classify skill documents helpers for DEFERRED (`deferred_close.py`) and UNCERTAINTIES
(`uncertainty_resolve.py`) only. It has no documented operation that strikes a STUBS.md row. The step rules forbid
hand-edits and ad-hoc scripts, so STUBS.md and CHANGELOG are untouched on this branch.

## Proposed edit (for a lead or a stubs helper to apply, convention of the struck row S-0171)
`| ~~S-3714~~ | ~~0x00546bf0 FUN_00546bf0~~ | ~~0x00546b10 FUN_00546b10~~ | vehicle | resolved | 2026-05-13 | RESOLVED 2026-11-02: native port in RwpSolverMath2Quat.cpp (RH_ScopedInstall, no call_original), moved 2026-11-01 (D4-WSB-55B080-ULP). Was: ... |`
S-3715 same with 0x00546c50 / RwpSolverMath2QuatY.cpp (x87 replay GREEN 1200/1200); S-3716 same with 0x00546cb0 / Quat.cpp.
Also: update the Status summary count (open rows minus 3, struck plus 3) and add three
`2026-11-02  S-NNNN  STUBS  Active->struck  ...` CHANGELOG lines.

## Side findings (not edited)
- hooks.csv rows 00546bf0 and 00546cb0 still read `C2,ported` (batch-aa-s3) though the bodies are impl with
  shadow A/B; a C3 reclassification is a separate step needing the shadow evidence (`verify/d4wsb55b080ulp_20261101/shadow_summary.tsv`).
- Analysis notes `replay_record_cont1/0x00546b10.md` and `bucket_vehicle_004922e0_0057c500/0x00546b10.md`
  and `re/account2_stub_resolve_queue.md` lines 347-349 still cite the IDs (history, left alone).
