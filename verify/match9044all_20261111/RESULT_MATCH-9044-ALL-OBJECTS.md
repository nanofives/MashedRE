# RESULT MATCH-9044-ALL-OBJECTS (2026-11-11)

Row: L-133 (1), Mariano 2026-11-11. Branch `orch/MATCH-9044-ALL-OBJECTS` off `orch/integration`. Kind: match-only/tracker,
no build, no gates.

## Verdict

**67 of 69 rows promoted to C4 by match, 1 refused, 1 already C4.** (Rebased 2026-11-12 onto orch/integration 9f1ee4c95 after PROV-PROMOTE-44; the batch was re-applied there. Row 0x005aaac0 was already C4 on integration, so the batch skipped it with no write. The 43 `c4=frida-replay` rows are untouched, count 43 before and after.) The 69 sources in
`verify/rwcompiler9044_20261111/rw9044_hold.tsv` each got `// TOOLCHAIN rw9044` as line 1.
`re/tools/c4_reclass.py batch` then recompiled every one under that toolchain, compared whole-symbol `--bytes`,
and ran `relocheck.py` on it. Every row is an existing `// MATCH:` source with real C bodies, none naked inline-asm
(the gate refuses those and refused none).

By group (rw9044_hold.tsv `group`): libpng/zlib 45 rows, 44 promoted in the first run the rebase (group counts below are from the first run). RenderWare Audio plus mixed row 0x005c4e20
22 rows, 22 promoted. Lua 4.0 2 rows, 2 promoted. Byte-matched (`c4=match`) count this row: 67. No `libid` and no
`frida-replay` rows.

## Refused

| rva | source | reason |
|---|---|---|
| 0x00521520 | re/match/s23/00521520.c | relocheck BAD: `+0x286 _FUN_00528ef0: ours 0x00528ef0 vs orig 0x00529050` and `+0x297 _FUN_00529050: ours 0x00529050 vs orig 0x00528ef0`. The two callees are swapped relative to the original, so the 100% byte score hides a wrong callee binding (the MATCH-S02 failure mode). Left at its prior level. The source keeps the `// TOOLCHAIN rw9044` line, which is the compiler it matched under. Fix is to swap the two callee names in the source, then re-run the batch. |

## Promoted

The 68 rows are listed with their verdict line in `apply.log` and `applied.tsv` (dry-run: `dryrun.log`,
`dryrun.tsv`). Inputs: `batch_c4.tsv` (rva, route `match`, evidence = source). `hooks.csv` shows 67 rows changed to
C4 and `re/analysis/CHANGELOG.md` got 67 lines.

## Changes

- 69 sources under `re/match/**`: line 1 `// TOOLCHAIN rw9044`.
- `re/CONFIDENCE.md`, match route: per-object compiler note (Rich prodid 0x30 objects take rw9044, any contents).
- `hooks.csv`, `re/analysis/CHANGELOG.md`: through `c4_reclass.py batch --apply`.
- The toolchain itself (`tools/vc6pp`) is gitignored and comes from MATCH-RW-COMPILER-9044 (provenance in its RESULT).

## Notes for the lead

- `ddkobj.py` honours `// TOOLCHAIN`, so units among these 69 that the object build links will now be compiled with
  rw9044. Not checked here (no build in this row kind).
- CRT / libpng / zlib rows stay out of the P-DoD port count (L-67 / L-70). C4 here certifies the RE only.

## Rebase 2026-11-12

Command: `py.exe -3.12 re/tools/c4_reclass.py batch --tsv verify/match9044all_20261111/batch_c4.tsv --result verify/match9044all_20261111/RESULT_MATCH-9044-ALL-OBJECTS.md --apply --jobs 4 --report verify/match9044all_20261111/applied.tsv --date 2026-11-11`.
Result: 67 pass, 1 refused (0x00521520, relocheck BAD, unchanged), 1 skipped (0x005aaac0, already C4 on integration via frida-replay). Relocheck OK on all 67 promoted rows (`apply.log`). `hooks.csv` and `CHANGELOG.md` came only from the tool run on the integration tip. `c4=frida-replay` rows: 43 before, 43 after.
