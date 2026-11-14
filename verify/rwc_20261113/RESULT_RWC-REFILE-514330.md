# RESULT RWC-REFILE-514330 (2026-11-13, class P2, L-127)

Row kind: tracker/text. No build, no gates. Branch `orch/RWC-REFILE-514330` from `orch/integration` @ 69cd03777.
Follows `verify/rwcd3dx9_20261113/RESULT_RWC-D3DX9-LINK.md` section 1 and `verify/rwsizing_20261111/RESULT_RW-SIZING.md` line 60.

## Executed d3dx-labelled rows (8)

Source: `re/coverage/reach.tsv` joined to hooks.csv rows whose subsystem contains `d3dx9` (runs_menu or runs_race above 0/3).
RW-SIZING counted the same 8. Libmatch column: `verify/c4rdxlib_20261107/d3dx_sum2003.tsv` (Summer 2003 d3dx9.lib).

| RVA | old label | libmatch d3dx9 | identity evidence | action |
|---|---|---|---|---|
| 0x00514330 | d3dx9-backward-extension | not-compared, no function paired | RtPNGImageRead: calls 0x004cc230(2,1,name) at 0x0051434b, then 0x005173f0 with string "1.0.12" at 0x00620e1c; bound `MashedRw_RtPNGImageRead_00514330` (`mashedmod/src/mashed_re/LibRw/RwApiBind.cpp:839-841`) | refiled to `third-party-library[renderware]` |
| 0x00515e50 | d3dx9-backward-extension | not-compared | callee of 0x00523e40 png_handle_gAMA (libpng 1.0.12 pngrutil.c, `re/match/s23/00523e40.c:223`, decl :160 `png_set_gAMA`) | refiled to `third-party-library[libpng-1.0.12]` |
| 0x00515fd0 | d3dx9-backward-extension | not-compared | callee of 0x00523640 png_handle_IHDR (libpng 1.0.12, `re/match/s23/00523640.c:178`, decl :111 `png_set_IHDR`) | refiled to `[libpng-1.0.12]` |
| 0x005163a0 | d3dx9-backward-extension | not-compared | callee of 0x005238c0 png_handle_PLTE (`re/match/s23/005238c0.c:284`, decl :224 `png_set_PLTE`) | refiled to `[libpng-1.0.12]` |
| 0x00516870 | d3dx9-backward-extension | not-compared | callee of 0x00525bd0 png_handle_tRNS (libpng 1.0.12, `re/match/s23/00525bd0.c:291`, decl :47) | refiled to `[libpng-1.0.12]` |
| 0x0051085a | d3dx9-shader-compiler | identical-ambiguous (1 byte, same bytes as many d3dx9 destructors) | 1-byte RET (`re/analysis/cluster_0051_first_pass/0051085a.md`), callers 0x004fcc67, 0x004fe2fe, 0x004fe9cf, 0x005c2d9b, 0x005c2dd9 | **not refiled** [UNCERTAIN]: a bare RET has no identity, and callers span the d3dx band and the audio glue at 0x005c2d9b |
| 0x005b8200 | d3dx9-fx | not-compared | plate `re/analysis/bucket_00565cd0/0x005b8200.md`: the d3dx9-fx reading rests on a 0x20-stride record pattern and strings 0x005e6454 / 0x005e64b4, marked [UNCERTAIN] in the plate | **not refiled** [UNCERTAIN]: no evidence for another subsystem |
| 0x005b8680 | d3dx9-fx | not-compared | plate `re/analysis/bucket_00565cd0/0x005b8680.md`, same band as 0x005b8200 | **not refiled** [UNCERTAIN]: same |

The 4 libpng identities are call-site identities (a matched libpng 1.0.12 chunk handler calls the function with the libpng
argument list), not libmatch identities: the libpng 1.0.12 libmatch TSV (`verify/matchlib_20261031/pngzlib/libpng_O2_G5.tsv`) has no
row at these 4 addresses. Confidence stays C2, status stays `mapped`, file stays empty. None is a C4 promotion.
Reading of the callee bodies in Ghidra was not done in this row [UNCERTAIN: a body read would confirm each png_set_* name].
Names stay `FUN_<rva>`.

## Edits

`hooks.csv`: subsystem cell of 5 rows (10 line changes), identity prepended to notes. `re/analysis/CHANGELOG.md`: 5 lines under
`<!-- ENTRIES -->`. Done with a one-off script that edits only those 5 rows (EOL and quoting preserved, diff shows exactly 5 rows
changed). Standing note: `re-classify` has no documented single-row refile helper for these destinations
(`libid_reclass.py waive-lib` needs a libpng libmatch identity, which these rows lack), so this follows the REFILE-4B3FC0 precedent (b4ac131e5).

## Checks

- `py -3.12 scripts/lint_rva_bodies.py`: OK, NEW=0 (233 known allowlisted).
- hooks.csv re-parsed: 6443 rows, widths {9, 11} as before, the 5 subsystem cells read back as above.
- Closure consequence: executed d3dx9 rows 8 -> 3, all three [UNCERTAIN] (0x0051085a, 0x005b8200, 0x005b8680). RW-SIZING's "5 called from
  RtPNG / libpng code" is confirmed for 5 rows (0x00514330 plus the four png_set_*). The RWC-D3DX9-LINK claim "0 executed d3dx9 rows
  once refiled" does not hold: 3 remain unidentified.
- Wider finding, not acted on: the `d3dx9-backward-extension` band 0x005115aa..0x00516a71 interleaves libpng rows (rows
  0x005150e0 .. 0x00516d30 already filed `[libpng-1.0.12]`) with `d3dx9-backward-extension` ones, so the non-executed rows of
  that band are probably mislabeled too. Out of scope (not executed, no identity evidence read).

## DECISIONS.md

`log/orch/DECISIONS.md` is gitignored (`/log/`) and absent from the worktree, so the line was appended to the live file in the main
checkout: `auto, class P2, RWC-REFILE-514330, verify/rwc_20261113/RESULT_RWC-REFILE-514330.md`.
