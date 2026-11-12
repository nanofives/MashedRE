# RESULT MATCH-9044-ALL-OBJECTS (2026-11-11)

Row: L-133 (1), Mariano 2026-11-11. Branch `orch/MATCH-9044-ALL-OBJECTS` off `orch/integration`. Kind: match-only/tracker,
no build, no gates.

## Verdict

**68 of 69 rows promoted to C4 by match, 1 refused.** The 69 sources in
`verify/rwcompiler9044_20261111/rw9044_hold.tsv` each got `// TOOLCHAIN rw9044` as line 1.
`re/tools/c4_reclass.py batch` then recompiled every one under that toolchain, compared whole-symbol `--bytes`,
and ran `relocheck.py` on it. Every row is an existing `// MATCH:` source with real C bodies, none naked inline-asm
(the gate refuses those and refused none).

By group (rw9044_hold.tsv `group`): libpng/zlib 45 rows, 44 promoted. RenderWare Audio plus mixed row 0x005c4e20
22 rows, 22 promoted. Lua 4.0 2 rows, 2 promoted. Byte-matched (`c4=match`) count this row: 68. No `libid` and no
`frida-replay` rows.

## Refused

| rva | source | reason |
|---|---|---|
| 0x00521520 | re/match/s23/00521520.c | relocheck BAD: `+0x286 _FUN_00528ef0: ours 0x00528ef0 vs orig 0x00529050` and `+0x297 _FUN_00529050: ours 0x00529050 vs orig 0x00528ef0`. The two callees are swapped relative to the original, so the 100% byte score hides a wrong callee binding (the MATCH-S02 failure mode). Left at its prior level. The source keeps the `// TOOLCHAIN rw9044` line, which is the compiler it matched under. Fix is to swap the two callee names in the source, then re-run the batch. |

## Promoted

The 68 rows are listed with their verdict line in `apply.log` and `applied.tsv` (dry-run: `dryrun.log`,
`dryrun.tsv`). Inputs: `batch_c4.tsv` (rva, route `match`, evidence = source). `hooks.csv` shows 68 rows changed to
C4 and `re/analysis/CHANGELOG.md` got 68 lines.

## Changes

- 69 sources under `re/match/**`: line 1 `// TOOLCHAIN rw9044`.
- `re/CONFIDENCE.md`, match route: per-object compiler note (Rich prodid 0x30 objects take rw9044, any contents).
- `hooks.csv`, `re/analysis/CHANGELOG.md`: through `c4_reclass.py batch --apply`.
- The toolchain itself (`tools/vc6pp`) is gitignored and comes from MATCH-RW-COMPILER-9044 (provenance in its RESULT).

## Notes for the lead

- `ddkobj.py` honours `// TOOLCHAIN`, so units among these 69 that the object build links will now be compiled with
  rw9044. Not checked here (no build in this row kind).
- CRT / libpng / zlib rows stay out of the P-DoD port count (L-67 / L-70). C4 here certifies the RE only.
