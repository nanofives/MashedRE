# PREREG REPLAY-FALLBACK-8 + PROV-INSITU-REF1-NOSRC (run 54, 2026-11-21)

Committed before the first body run. Method = `verify/provinsitu_ref1_20261121/PREREG_PROV-INSITU-REF1.md` unchanged
(ref1 recipe, compare against ref1/ref2 from tick 0, the same verdict classes), with these differences only:

- Bodies: the 15 (rva, source) pairs of `SOURCES.tsv`, linked by `mkbodies.py` (provlink `link()` on the file the object
  build compiles: the static co-residents 0x0041b7a0 / 0x004969a0 / 0x0041ce00 / 0x00495870 are linked from their caller's
  source, the branch sources are taken with `git show <sha>:<path>`). Step `fb8` = the 8 fallback units, step `nosrc` =
  the 7 NOSRC sources (0x0048ae00 taken from the head `re/match/s14/0048ae00.c`, which is not the copy replayed in run 53).
- Every body runs alone (`prov_bisect.py --group 1 --any-segment`), so no group split is needed.
- Private root `D:\MashedWT\REPLAY-FALLBACK-8_root` = robocopy of the ref1 root `RO-REF-FIXED-LOAD_root`; MASHED.exe,
  .unpatched, gamesave.bin, d3d9.dll, mashed_re_dev.asi SHA-256 equal to it (checked before the first run).
- Controls: c0 (no body) IDENTICAL-FOR-600 against ref1/ref2 before any body verdict counts. Census `--prov-count` over the
  15 rvas: a body with 0 calls is NOT-EXECUTED.
- Write-set leg (L-38 / L-40 / L-54): static provlink store list empty or compared-image absolute stores only; else a
  dynamic leg: the body alone with `--prov-wlog 10`, `prov_writeset.py` against ref1/ref2, every stepped store in the
  frame or the compared image and at least one image store, in a stepped run that itself replays IDENTICAL-FOR-600.
- 0x00495870 sits on the fix_joypad detour (`MASHED.exe` 0x00495870 -> cave 0x00508bd5). The arm places its JMP over the
  detour's 5 bytes, so the body replaces the guarded entry. The verdict for it holds for this scenario only (no device
  pointer guard in the body), and the RESULT says so.
- Promotions: IDENTICAL-FOR-600 + install check + body executed + no STUB + write-set leg covered ->
  `re/promotions/REPLAY-FALLBACK-8.tsv` / `re/promotions/PROV-INSITU-REF1-NOSRC.tsv`, checked with
  `c4_reclass.py batch --tsv` (dry run only).
- Object build (fb8 only, units present on the head: 0x00401690, 0x00498810, 0x00495870): for each IDENTICAL one, its
  PROVISIONAL.tsv row leaves the list on this branch (replay-C4 body, run 54 merge bar), then `set MASHED_DDKOBJ=1` build
  and `ddkobj.py check`: the neighbour must be IDENTICAL in BYTECHECK.tsv. The other five units are not on the head (their
  sources live on orch/PORT-TOP-RENDERFRAME-492E90 and orch/PORT-TOP-06); this row binds nothing, so their object-build
  line belongs to the landing row PORT-TOP-LAND-06-RF.
