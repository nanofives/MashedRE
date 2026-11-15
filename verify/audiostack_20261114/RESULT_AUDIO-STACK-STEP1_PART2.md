# RESULT AUDIO-STACK-STEP1 part 2: match/relocheck audit and DIFFERENT classification (run 47)

Verdict: GREEN with findings. 2 units are labeled `matched` in BINDINGS.tsv but are below 100% and are in neither
ADMITTED.tsv nor PROVISIONAL.tsv. 6 DIFFERENT units are neither admitted nor provisional, and all 6 were already
DIFFERENT on the integration base. None of the 19 units newly DIFFERENT against the base is unclassified. This part
adds one ADMITTED.tsv row (0x00554200, plain EQUIV) and does not touch any source, binding or build input.

- Branch `orch/AUDIO-STACK-STEP1`. Fast-forwarded `2e89f3602` -> `870a6f86f` (orch/AUDIO-STACK-5-UNMATCHED-BOUND,
  GREEN) with `git -c merge.autoStash=false merge --ff-only orch/AUDIO-STACK-5-UNMATCHED-BOUND`. The commit that adds
  this file is the head reported to the lead.
- No exe source and no binding changed, so the builds, `ddkobj.py check` and the 9 knob arms of
  `verify/audio_20261114/RESULT_AUDIO-STACK-5-UNMATCHED-BOUND.md` (run on `870a6f86f`) stand: both builds OK, rva-lint
  NEW 0, `IDENTICAL 1456/1508, DIFFERENT 37, not-in-map 15`, self-tests PASS. They were not re-run.
- `re/tools/match/equiv.py` on this branch equals orch/integration (`git diff orch/integration --
  re/tools/match/equiv.py` is empty). Last commit of the tool: `fea5b5483`.

## 1. match.py --bytes and relocheck.py on every matched-bound source

Command (tool committed here): `py -3.12 verify/audiostack_20261114/tools/match_all.py
verify/audiostack_20261114/match_relocheck_matched.tsv`. For each source it runs
`py -3.12 re/tools/match/match.py --bytes <src>` and `py -3.12 re/tools/match/relocheck.py <src>`. match.py takes each
function's toolchain from its `// TOOLCHAIN` line (default ddk = XP DDK cl 13.00.9176 / c2 13.00.9178 in
`tools/vc7ddk`, rw9044 for the RW rows under L-131) and its flags from its `// FLAGS:` header.

How a row maps to a source: the `re/match/...` paths in the BINDINGS note. 103 notes name only the exe .cpp. For those
the script takes the re/match file named after the rva, or else every file with a `// MATCH: 0x<rva>` annotation.

- Scope: 551 BINDINGS rows with kind `matched`, 546 source files. Full log: `D:/MashedWT/AUDIO-STACK-STEP1_data/match_all.log`
  (sha256 `7ffe269c4c55d6ea91fffa82afdd02b43b699c217d345ae96c8efdbaa5b38fb6`). Per-row table:
  `match_relocheck_matched.tsv` (this folder).
- Pass rule: `bytes: ... -> BYTE-IDENTICAL` on the whole obj symbol and relocheck `OK`.
- **532 / 551 pass.**
  - 508 of them score 100.0% on instructions.
  - 24 are BYTE-IDENTICAL with relocheck OK but score under 100% on instructions. Examples: 0x00453f30 55.6%
    (9/5 ins), 0x0043d7c0 99.6% (458/456). In every one of the 24, our instruction count is at least the original's.
    The original-side instruction list ends at the pc_funcs extent, which is shorter than the whole symbol. The source
    header of 0x00453f30 records the same thing ("Ghidra's size (24 B) stops at the loop label").
  - The 24: 0x004015a0, 0x004025f0, 0x00412890, 0x00421080, 0x0043d7c0, 0x00453f30, 0x004548e0, 0x00459290,
    0x0045b930, 0x0045bae0, 0x0045ca30, 0x0045cbe0, 0x0045d330, 0x00461650, 0x004625b0, 0x00463f40, 0x00464a50,
    0x00465a30, 0x00475ef0, 0x00476390, 0x004862d0, 0x00489290, 0x004892c0, 0x0048a460.
- **19 fail.** 17 of them are in ADMITTED.tsv (L-126 EQUIV, labeled `matched` with the note "L-126 admitted"): 0x0045c480, 0x005a6b30,
  0x005a6e50, 0x005a7810, 0x005a78b0, 0x005a8890, 0x005a9970, 0x005aa480, 0x005aab00, 0x005ad030, 0x005ad320,
  0x005adb70, 0x005ae210, 0x005ae5b0, 0x005ae610, 0x005ae920, 0x005aea60.

**Finding 2a: two `matched` rows below 100%, neither admitted nor provisional.** Not relabeled here.

| rva | source | match.py | relocheck | ADMITTED | PROVISIONAL | BYTECHECK | BINDINGS note |
|---|---|---|---|---|---|---|---|
| 0x00493900 | re/match/s15/00493900.c | 66.9%, no BYTE-IDENTICAL | BAD | no | no | DIFFERENT 392 B | "best 66.9% (hooks C2), the only source" |
| 0x00493710 | re/match/s15/00493710.c | 94.2%, no BYTE-IDENTICAL | BAD | no | no | DIFFERENT 236 B | "h8 best 94.2%, the only source" |

Both notes state the sub-100% score. Only the kind column says `matched`. Both were already DIFFERENT on the
integration base (section 2).

## 2. The 37 DIFFERENT units of the object build

Input: `ddkobj.py check` of `870a6f86f`,
`.worktrees/AUDIO-STACK-5-UNMATCHED-BOUND/mashedmod/build/ddkobj/BYTECHECK.tsv`, copied to
`D:/MashedWT/AUDIO-STACK-STEP1_data/BYTECHECK_870a6f86f.tsv`
(sha256 `16a7a4db73cbcb302924aab172f0914c1f01a2be416dca63a5e535bd098bd6f0`).

Base for "new": `verify/ddkregconv7_20261113/BYTECHECK.tsv` (commit `3f39d5a0b`). The STEP1 RESULT used the same
file for the integration-side DIFFERENT set. It lists 18 DIFFERENT units. All 18 are still DIFFERENT and none left the set.
[UNCERTAIN: that check ran on the run-45 integration tip, not on a fresh check of `d1e5ac586`. The STEP1 RESULT
section 3 makes the same identification.]

Verdicts below come from the merged equiv.py, `py -3.12 re/tools/match/equiv.py --only 0x<rva> <src>`. The output of
each run is in `equiv_<rva>.txt` in this folder.

### 2.1 Newly DIFFERENT against the base: 19, all classified

| rva | source | diff B | class |
|---|---|---|---|
| 0x0045c480 | re/match/s10/0045c480.c | 77 | ADMITTED (EQUIV) |
| 0x0045cd30 | re/match/s10/0045cd30.c | 319 | PROVISIONAL (NOT-PROVEN) |
| 0x00498510 | re/match/s16/00498510.c | 11 | PROVISIONAL (NOT-PROVEN) |
| 0x005a6b30 | re/match/s30/005a6b30.c | 58 | ADMITTED (EQUIV) |
| 0x005a6e50 | re/match/s30/005a6e50.c | 20 | ADMITTED (EQUIV) |
| 0x005a7810 | re/match/s30/005a7810.c | 8 | ADMITTED (EQUIV) |
| 0x005a78b0 | re/match/s30/005a78b0.c | 14 | ADMITTED (EQUIV) |
| 0x005a8890 | re/match/s25/005a8890.c | 40 | ADMITTED (EQUIV) |
| 0x005a9970 | re/match/s30/005a9970.c | 21 | ADMITTED (EQUIV) |
| 0x005aa480 | re/match/s30/005aa480.c | 7 | ADMITTED (EQUIV) |
| 0x005aab00 | re/match/s25/005aab00.c | 63 | ADMITTED (EQUIV) |
| 0x005ad030 | re/match/s30/005ad030.c | 68 | ADMITTED (EQUIV) |
| 0x005ad320 | re/match/s26/005ad320.c | 219 | ADMITTED (EQUIV) |
| 0x005adb70 | re/match/s30/005adb70.c | 109 | ADMITTED (EQUIV) |
| 0x005ae210 | re/match/s30/005ae210.c | 43 | ADMITTED (EQUIV) |
| 0x005ae5b0 | re/match/s30/005ae5b0.c | 15 | ADMITTED (EQUIV) |
| 0x005ae610 | re/match/s30/005ae610.c | 61 | ADMITTED (EQUIV) |
| 0x005ae920 | re/match/s26/005ae920.c | 3 | ADMITTED (EQUIV) |
| 0x005aea60 | re/match/s30/005aea60.c | 17 | ADMITTED (EQUIV) |

### 2.2 DIFFERENT on the base too: 18

| rva | source | diff B | class | equiv.py (merged, fea5b5483) |
|---|---|---|---|---|
| 0x004332a0 | re/match/s08/004332a0.c | 64 | ADMITTED (EQUIV) | row exists |
| 0x00475a00 | re/match/s12/00475a00.c | 2 | ADMITTED (EQUIV) | row exists |
| 0x004c0f00 | re/match/s19/004c0f00.c | 1 | PROVISIONAL | row exists |
| 0x004c0c20 | re/match/s19/004c0c20.c | 1 | PROVISIONAL | row exists |
| 0x0043c5b0 | re/match/s08/0043c5b0.c | 2559 | PROVISIONAL | row exists |
| 0x00552890 | re/match/rwl04/00552890.c | 23 | PROVISIONAL (RWC list) | NOT-PROVEN call-memory @ orig 0x0055289c |
| 0x005530f0 | re/match/rwl04/005530f0.c | 129 | PROVISIONAL (RWC list) | NOT-PROVEN call-args @ orig 0x005530f0 (x87 term) |
| 0x005533d0 | re/match/rwl04/005533d0.c | 349 | PROVISIONAL (RWC list) | NOT-PROVEN x87-region @ orig 0x005533d0 |
| 0x005538a0 | re/match/rwl04/005538a0.c | 805 | PROVISIONAL (RWC list) | NOT-PROVEN branch-condition @ orig 0x005538a0 |
| 0x00554940 | re/match/s24/00554940.c | 1262 | PROVISIONAL (RWC list) | NOT-PROVEN x87-region @ orig 0x0055497f |
| 0x005572c0 | re/match/s24/005572c0.c | 1071 | PROVISIONAL (RWC list) | NOT-PROVEN call-regargs @ orig 0x00557309 |
| 0x00554200 | re/match/s24/00554200.c | 6 | **ADMITTED by this row** (was RWC list) | `EQUIV [ORDER]` |
| 0x00493900 | re/match/s15/00493900.c | 392 | **neither** | NOT-PROVEN call-regargs @ orig 0x0049393d (call 0x4a3ac9) |
| 0x00493710 | re/match/s15/00493710.c | 236 | **neither** | NOT-PROVEN call-args @ orig 0x00493710 (call 0x5c9d00 arg 0) |
| 0x00498810 | re/match/s16/00498810.c | 4 | **neither** | NOT-PROVEN call-regargs @ orig 0x00498831 (call 0x4d52d0) |
| 0x00401690 | re/match/s01/00401690.c | 713 | **neither** | NOT-PROVEN branch-condition @ orig 0x0040169a |
| 0x004929d0 | re/match/gffu/004929d0.c | 47 | **neither** | NOT-PROVEN call-args @ orig 0x004929d0 (call 0x4927c0 arg 0) |
| 0x0045a3a0 | re/match/s09/0045a3a0.c | 36 | **neither** | `EQUIV-IF-CALLERS-IGNORE(EAX) [ENC,FRAME,VALUE-FLOW]`, callers 3: 0x0045a741 ignores, 0x0045a7d9 ignores, **0x0045ac2a unknown** |

"RWC list" = `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv` (RWC-CORE-RUN, 990 rows), not `re/match/PROVISIONAL.tsv`.
`ddkobj.py` reads only `re/match/PROVISIONAL.tsv` (`re/tools/ddkobj/ddkobj.py:55`, skip at `:751`). That is why these
7 units are byte-checked and come out DIFFERENT instead of `skip:provisional`. [UNCERTAIN: whether the object build
links the DDK object or the VS2022 body for these 7 units. This part did not read the link map.]

**Finding 3a: 6 DIFFERENT units neither admitted nor provisional** (all carried from the base, none new):
0x00493900 (s15/00493900.c), 0x00493710 (s15/00493710.c), 0x00498810 (s16/00498810.c), 0x00401690
(s01/00401690.c), 0x004929d0 (gffu/004929d0.c), 0x0045a3a0 (s09/0045a3a0.c).
- 5 are NOT-PROVEN. The L-126 place for those is PROVISIONAL.tsv. Adding a PROVISIONAL row makes `ddkobj.py` skip the
  unit (`skip:provisional`, VS2022 body). That changes object-build inputs, so the builds, check and arms would have
  to be re-run. Filed as a new open row (below), not done here.
- 0x0045a3a0 is a conditional verdict with an unknown caller (0x0045ac2a), so it is a decision under WORKER_RULES and
  no row is written here.

## 3. ADMITTED.tsv row added: 0x00554200

- Verdict (merged equiv.py `fea5b5483`): `0x00554200 EQUIV [ORDER]`. The `--json` rules are A6-paired-pop,
  A6-pinned-pop, A7-stack-image and jmp-threading, in `strict` mode.
- It is plain EQUIV, so no caller scan is needed. No `--relax-icall`. The ORDER class is not x87 order: the residue is
  one integer store, `mov [esi+0x10],eax` at 0x005542a2 (source header, MATCH-RT2D-RESID run 40).
- Source `re/match/s24/00554200.c` @ `b7414f7ae`, `// TOOLCHAIN rw9044`, `// FLAGS: /O2 /Ob1 /G5`.
- Local callee `_FUN_00553e80`: `match.py --bytes --only 0x00553e80` gives 100.0% BYTE-IDENTICAL, relocheck OK, and
  BYTECHECK IDENTICAL.
- Cross-checks:
  - `equiv_dyn.py --only 0x00554200`: SAME same=32 inc=0 diff=0.
  - `--seeded`: INCONCLUSIVE same=0 inc=32 diff=0.
- Port binding: `RwCore/RwCore_00553e80.cpp` (MASHED_ORIG_TOP=1 + MASHED_RWC_CORE=1).
- Status: never C4 by this row (L-126 / L-128). The RWC list still names it provisional (it is an RWC-CORE-RUN file,
  and this part left it as it was).
- What the row affects: nothing reads `re/match/ADMITTED.tsv` in the build, `ddkobj.py` or rva-lint. The only readers are
  `verify/portorigtop_20261113/tools/bindings_update.py` and two generators, and 0x00554200 has no BINDINGS row, so no
  check was re-run.

## 4. Unfinished scope

- New open row `AUDIO-STACK-NEITHER-6`:
  - Add PROVISIONAL.tsv rows for the 5 NOT-PROVEN units in 3a.
  - Relabel 0x00493900 and 0x00493710 from `matched` to `near-match`.
  - Re-run both builds, `ddkobj.py check` and the knob arms.
- Decision filed on new row `AUDIO-0045A3A0-CALLER`: 0x0045a3a0 EQUIV-IF-CALLERS-IGNORE(EAX), caller 0x0045ac2a is
  unknown. The options are to settle 0x0045ac2a and admit the unit, or to list it as provisional.
