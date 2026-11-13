# RESULT MATCH-RELOCHECK-5444A0 (2026-11-13)

Verdict: relocheck rule added, 0x005444a0 now OK 15/15, unchecked 0. Promotion TSV committed, dry run below.

Refusal: `FUN_004cc790_site2` (rel32 call, case-2 read in `re/match/rwtb/005444a0.c`) fell through relocheck.py because
`undecorate` gives `FUN_004cc790_site2`, which matches none of the `FUN_<va8>` / `DAT_<va8>` patterns, so it was listed unchecked.

Rule (re/tools/match/relocheck.py): `FUN_<va8>_site<N>` resolves to 0x<va8> (+ stored addend for DIR32), then goes through
the same compare as a plain `FUN_`: the original's rel32 operand at THIS reloc's offset is resolved to an absolute target and
must equal 0x<va8>. The suffix is a label only, never part of the target, so a wrong address in the name or a call at the
wrong offset is BAD. Original bytes at the site: the case-2 call targets 0x004cc790 (match.py listing, 197/197 ins, 100.0%).

Tests (`py -3.12 re/tools/match/test_relocheck.py`, 6/6 PASS): `site2_ok` (frozen copy of the source, OK) and
`site2_wrong_target_bad` (same source with the extern named `FUN_004cc791_site2`, BAD: ours 0x004cc791 vs orig 0x004cc790).
Existing four cases unchanged. Only 005444a0.c uses a `_site` name in re/match.

Not run: build, gates (match-only row).
