# RESULT D4-DC-77b0-NOTE: stale comment on dual_copy_allowlist.txt line 38

Verdict: GREEN. Text-only, comment bytes of one line. No build input, no code, no .ps1.

## Change
`re/tools/dual_copy_allowlist.txt` line 38 (CROSS-TARGET:004177b0). Replaced the clause "what it still reads from places no exe TU
writes (0x008a9648 / 0x008a96ec, ...)" and the sentence "NOT compared line by line in the fwd494fd0 lane (the .asi body is about
160 lines, the exe's about 140)" with the facts of `verify/d4dc77b0lap_20261030/RESULT_D4-DC-77b0-LAP.md` section 3:
exe writers of 0x008a9648 at PathProgStep.cpp:248/289 (MASHED_PATHPROG) and :240/335 (MASHED_PP_PUBLISH=0x1), both OFF;
0x008a96ec has the default-ON bridge producer (PathProgStep.cpp:337, same section); BODYDIFF counts 157/142 lines. Cites
`verify/d4dc77b0diff_20261030` and `verify/d4dc77b0valsrc_20261030`. Key, TU paths, the "DELIBERATE until D3 criterion (b)" lead and
"Row C2." tail unchanged. `git diff --stat`: 1 file, 1 insertion, 1 deletion (line 38 only).

## Gate: `py -3.12 scripts/lint_rva_bodies.py` before and after
Output byte-identical (`fc` no differences, SHA-256 E9DD7A2547772D2722EB778F62FB83AD83E8923D9A9FE732EAF3F772C2AA42C2 both).
Both runs, in full:

```
[rva-lint] 16 finding(s): CROSS-TARGET=16  |  allowlisted=16 NEW=0
[rva-lint] OK -- no NEW duplicate bodies (16 known, tracked in re\tools\dual_copy_allowlist.txt)
```

(after) identical to the above.

Gates G-CTRL / B: not run, no build input changed.



