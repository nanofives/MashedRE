# RESULT HOOKS-SHA-RESYNC-AUDIOTICK (run 47)

Branch orch/HOOKS-SHA-RESYNC-AUDIOTICK, based on orch/AUDIO-STACK-STEP1 @ 994fe6644. Tracker row, no build, no gates.
Verdict: BLOCKED on the refresh itself (no documented re-classify operation), verification of all 7 sources done.

## 1. Seven sources (list from RESULT_PORT-TOP-AUDIOTICK-STOPS.md lines 60-73)
Checked on the stack tip with `py -3.12 re/tools/match/match.py --bytes <src>` and `py -3.12 re/tools/match/relocheck.py <src>`.

| rva | source | old src sha256 (16, hooks.csv note) | current sha256 (16) | match.py --bytes | relocheck |
|---|---|---|---|---|---|
| 0x00462dd0 | re/match/s10/00462dd0.c | ea5082998cc2a73b | f60e8f24192c3ede | 100.0% 65/65 ins, 240 B BYTE-IDENTICAL, 0 differing unmasked bytes | OK checked=24 unchecked=3 |
| 0x00462ec0 | re/match/s10/00462ec0.c | aff0cadf0330f224 | 1438b096d167d07a | 100.0% 39/39 ins, 132 B BYTE-IDENTICAL | OK checked=11 unchecked=1 |
| 0x0045cc50 | re/match/s10/0045cc50.c | 07f440b85d1a5fb7 | cf6cfdbc4de09518 | 100.0% 20/20 ins, 47 B BYTE-IDENTICAL | OK checked=3 unchecked=0 |
| 0x004645c0 | re/match/pilot/a1_004645c0.c | bb2e111af766ce04 | a1e7ac5ec33da862 | 100.0% 59/59 ins, 171 B BYTE-IDENTICAL | OK checked=11 unchecked=0 |
| 0x0045d250 | re/match/pilot/a4_0045d250.c | b636755ba58c30c7 | f029b17e7e9f13b4 | 100.0% 80/80 ins, 220 B BYTE-IDENTICAL | OK checked=17 unchecked=0 |
| 0x00411530 | re/match/s02/00411530.c | 0f47729eea7fbbd5 | 314881c70136ec2b | 100.0% 20/20 ins, 65 B BYTE-IDENTICAL | OK checked=1 unchecked=0 |
| 0x004115c0 | re/match/s02/004115c0.c | 30da0a03283117cd | 1d79ce9b946e624f | 100.0% 21/21 ins, 58 B BYTE-IDENTICAL | OK checked=2 unchecked=0 |

All 7 are byte-identical with relocheck OK (flags /O2 /GS /Ob1, tc=ddk). Each is eligible for a refresh.

## 2. Why hooks.csv is not refreshed
- The re-classify skill's documented C4 operation is `c4_reclass.py batch`. Its help states "Rows already at C4 are skipped (no write)"
  (re/tools/c4_reclass.py lines 24-25, 464). The seven rows are C4 already, so a batch run would report SKIP and write nothing.
- The skill (re-classify/SKILL.md lines 125-151) and c4_reclass.py (`sha` only built at lines 321 and 377 for a first promotion)
  document no sha-refresh, re-stamp or notes-update operation. The row instructions forbid an ad-hoc script and a hand-edit of hooks.csv / CHANGELOG.
- Nothing was written to hooks.csv or CHANGELOG. The old sha256 values above stay in hooks.csv as they were.

Options for the lead (decision, not taken): (a) add a documented `--refresh-sha` mode to c4_reclass.py (a tool edit, its own row, rewrites the
`src sha256` token in the notes of a row already C4 only when the gate passes now, one CHANGELOG line citing this RESULT);
(b) a decision row allowing a one-off hand-edit of those 7 notes. Recommendation: (a), because the next cast-only edit will stale more rows.
If the tool is edited, its `.py` edit goes through claude3 rules only if it triggers a policy prompt (not tested here).

## 3. PORT-TOP-TEARDOWN-TABLE-5F99B0 (read-only check on the stack tip 994fe6644)
Row scope: bind 0x004540c0 as Match/PortTop_004540c0.cpp + BINDINGS.tsv through codeptr_image.py, and the 7 later entries
(0x005f9a30..0x005f9bb0 -> 0x004555d0 0x00457b10 0x0045a790 0x0045b2d0 0x00454af0 0x00456ca0 0x004530b0) that the row says have no hooks.csv row, no re/match source and are not in pc_funcs.csv.

Evidence, all on the stack tip:
- Wrappers present: mashedmod/src/mashed_re/Match/PortTop_{004540c0,004555d0,00457b10,0045a790,0045b2d0,00454af0,00456ca0,004530b0}.cpp all exist.
  PortTop_004540c0.cpp:1-6 names the matched source re/match/s09/004540c0.c and slot 0x005f99f0 (0x005f99b0 + 0x40*1).
- Matched sources present: re/match/s09/{004530b0,004540c0,00454af0,004555d0,00456ca0,00457b10,0045a790,0045b2d0}.c (8 files).
  The row's "no re/match source" is stale.
- hooks.csv has 7 rows for the 7 later rvas (grep count 7 of 7). The row's "no hooks.csv row" is stale.
- BINDINGS: verify/porttop_20261108/BINDINGS.tsv:620-627 lists all 8 as `matched`, `Orig_<rva>`, note "IMG-5-TEARDOWN-BODIES: Match/PortTop_<rva>.cpp = re/match/s09/<rva>.c (100%, relocheck OK)".
- Image slots: re/tools/image/CODEPTR_IMAGE.tsv:664 (0x005f99f0 -> 0x004540c0, bound) and 676, 690, 705, 719, 732, 745, 758
  (0x005f9a30..0x005f9bb0 -> the 7 later targets), every one `bound` with `Orig_<rva>`.
- Not checked: pc_funcs.csv membership of the 7 later rvas, and runtime behaviour of the AUDIOTICK control arms past 0x004540c0 (no run made here).
  [UNCERTAIN] whether the image-arm stop at 0x004540C0 reported in RESULT_PORT-TOP-AUDIOTICK-IMGTRAP-4568A0.md is gone: it needs an arm run on the stack tip.
Finding: static scope fully covered (the bind of 0x004540c0 and the 7 later entries exist on the tip). Candidate void.
Remaining, only if the lead wants the arm proof: one image-arm run on the stack tip showing the control arms (vs_audio, ddk_audio) pass 0x004540c0 without a stop.
The row is not set done here.
