# RESULT AUDIO-STACK-5-UNMATCHED-BOUND (run 47)

Base: `orch/AUDIO-STACK-STEP1` @ `2e89f3602` (not `orch/integration`). Branch `orch/AUDIO-STACK-5-UNMATCHED-BOUND`.
Top-down row. No exe source changed. The object build changed: 4 units now keep their VS2022 body (section 4).

## Verdict: GREEN. Both builds OK, ddkobj check IDENTICAL 1456/1508 (DIFFERENT 41 -> 37), all 9 knob arms end as on STEP1, every self-test PASS (0 FAIL).

- The 5 units are not matches and not admissible: `equiv.py` says NOT-PROVEN for each one. Each now has a
  `PROVISIONAL.tsv` row (status `provisional`, replay IDENTICAL-FOR-2724) and BINDINGS kind `near-match`.
- 5 more BINDINGS rows had the same false `(c4=match)` note: the provisional bodies 0x005a6ea0, 0x005a6f30, 0x005aa0c0,
  0x005a5e70, 0x005ae650 (all in PROVISIONAL.tsv already). Also 0x0042f530 (C2, provisional) was kind `matched`. All
  six are now `near-match`. 0x005ad320's note now says admitted.
- Generator bug: `verify/portorigtop_20261113/tools/bindings_update.py:28` (on `2e89f3602`). Fixed.
- After the edits, every BINDINGS row of kind `matched` without `c4=match`/`c4=libid` in hooks.csv (49 rows) is either
  IDENTICAL in the stack's byte check or has an ADMITTED.tsv row.
- hooks.csv is not touched. The 5 keep C4 by `c4=frida-replay` (RE axis, L-128). On the port axis they are provisional.
  Byte-matched and replay-verified counts are separate: none of the 5 is a byte match.

## 1. equiv.py verdicts (the 5 units)

`equiv.py` on the branch = `fea5b5483` (last commit). `git diff 2e89f3602 orch/integration -- re/tools/match/equiv.py`
is empty, so the branch copy is the merged copy. Full lines: `verify/audio_20261114/equiv_<rva>.txt`.

| rva | source | flags | match.py (L0 run 46) | verdict line (short) |
|---|---|---|---|---|
| 0x0045cd30 | re/match/s10/0045cd30.c | /O2 /GS /Ob1 | 62.8%, relocheck BAD | `NOT-PROVEN branch-condition: region ours +0x146 / orig 0x0045ce7d: P1('sle',...) \| P1('slt',...)` |
| 0x004647f0 | re/match/s10/004647f0.c | /O2 /GS /Ob1 | 83.3%, relocheck BAD | `NOT-PROVEN call-args: region ours +0x91 / orig 0x00464881 call 0x45e0f0 arg 1: x87o32(...)` (fmul operand order) |
| 0x005a6d60 | re/match/s25/005a6d60.c | /O2 /Ob1 /G5 | 50.0%, relocheck OK | `NOT-PROVEN call-regargs: region ours +0x0 / orig 0x005a6d60 call ld32(...): (ecx ld32(arg4), edx zx16(...)) \| (ecx entry, edx ld32(arg4))` |
| 0x005aab70 | re/match/s25/005aab70.c | /O2 /Ob1 /G5 | 75.6%, relocheck OK | `NOT-PROVEN branch-condition: region ours +0x55 / orig 0x005aab92: P1('eq',(arg32(4)+0xc),ld32(M00(),(arg32(4)+0xc))) \| P1('ne',0x0,arg32(4))` |
| 0x005acda0 | re/match/s26/005acda0.c | /O2 /Op /Ob1 /G5 | 44.3%, relocheck BAD | `NOT-PROVEN call-sequence: region ours +0x4e / orig 0x005acdea: 1 vs 0 calls` |

None is admitted. 0x004647f0 differs in x87 operand order (never admitted). For 0x005a6d60, `--relax-icall` was not
tried, because a proof that needs it would not admit. The causes for 0x0045cd30 and 0x005aab70 are not diagnosed [UNCERTAIN]:
the verdict line is the only evidence.

Re-check of the two admitted rows on the same equiv.py: `0x005ad320 EQUIV [FRAME,VALUE-FLOW]`, `0x005aab00 EQUIV
[VALUE-FLOW]` (`equiv_005ad320.txt`, `equiv_005aab00.txt`). Both ADMITTED.tsv rows still hold.

## 2. New PROVISIONAL.tsv rows (5)

Each row: status `provisional`, equiv `NOT-PROVEN` with the line above, replay column = `IDENTICAL-FOR-2724` from
`verify/provinsitu_20261111/PROV_VERDICTS.tsv` (runs t50_191034_008_2, t80_171707_008_1, t50_191034_025_2,
t50_191034_036_2, t0_195814_016_1). `git log bf44aae7d..HEAD -- <source>` is empty for all 5 sources, so the replay ran the
same source text. No decision row: a NOT-PROVEN body that runs on the knob path is listed as provisional (L-126). These are
not admissions.

## 3. BINDINGS.tsv corrections (12 rows) and audit

| rva | before | after |
|---|---|---|
| 0x0045cd30 0x004647f0 0x005a6d60 0x005aab70 0x005acda0 | matched, `(c4=match)` | near-match, L-126 PROVISIONAL, NOT matched, frida-replay C4 is the RE axis |
| 0x005a6ea0 0x005a6f30 0x005aa0c0 0x005a5e70 0x005ae650 | matched, `(c4=match)` | near-match, L-126 PROVISIONAL, bound through PROV_BIND |
| 0x0042f530 | matched, "C2 source 96.9%" | near-match, L-126 PROVISIONAL |
| 0x005ad320 | matched, `(c4=match)` | matched, L-126 admitted (EQUIV), not a byte match |

`near-match` follows the precedent row 0x00455400 (IMG-5-TEARDOWN-BODIES). `re/tools/image/codeptr_image.py` still binds
`near-match` (`BIND_KINDS`), so the code-pointer slot of 0x005a5e70 is unchanged. `port_closed_sets.py` counts these
callees as open, as L-126 (6a) requires. The exe symbols are unchanged.

Audit (`verify/audio_20261114/tools/audit.py`, byte check = the stack's `BYTECHECK.tsv`):
- Before (`bindings_audit_before.tsv`): 562 matched, 60 without c4=match/libid.
  - (a) 7 are c4=frida-replay only.
  - (b) 53 have no C4 tag. Of these, 10 failed both checks: the 5 provisional binds, 0x0042f530, and the 5 units of (a).
- After (`bindings_audit_after.tsv`): 551 matched, 49 without c4=match/libid, all of them passing:
  - 23 are `(c4=match)`, IDENTICAL, with a waiting promotion row in `re/promotions/PORT-TOP-RWAUDIO-TABLE-632BA8.tsv`.
  - 17 are `L-126 admitted`, with an ADMITTED.tsv row. Their object bytes are DIFFERENT, which is expected for an admitted body.
  - 9 are C4 with no `c4=` tag, from the older frida route (canonical install-observe, 2026-05 to 06), and IDENTICAL in
    the byte check: 0x00492270 0x00498bc0 0x00498bd0 0x004938c0 0x0040d440 0x00498bf0 0x0042b8c0 0x004b3b70 0x00493f70.
    None of them claims `(c4=match)`.

## 4. The generator (scope c)

`verify/portorigtop_20261113/tools/bindings_update.py:28` @ `2e89f3602`:
```
note = '%s: %s (%s), Match/PortTop_%s.cpp' % (TAG, pr[1], 'L-126 admitted, re/match/ADMITTED.tsv' if pr[2] == 'admitted' else 'c4=match', r)
```
Every `bind` that was not `admitted` got the note `c4=match` and kind `matched` (line 27). That included the
`audiotick_bind_gen.py` kinds `replay` (`msrc`, line 41: C4 with c4=frida-replay and no c4=match) and `provisional`
(`PROV_BIND`, line 44). Fix: only `match` is written as `matched (c4=match)`. `admitted`, or an rva in ADMITTED.tsv, is
written as `matched` with an admitted note. `replay` and `provisional` are written as `near-match` with a provisional note.
The updater only rewrites rows of kind `missing` (line 38 of the old file), so the fix applies to new rows. The 12 existing
rows were corrected by `verify/audio_20261114/tools/apply.py`.

## 5. Top-down checks (object build changed)

Adding the 5 rvas to PROVISIONAL.tsv changes the object build: `ddkobj.py` gives `skip:provisional` to a unit whose MATCH
rvas are all provisional (`re/tools/ddkobj/ddkobj.py:751`). So 4 units now keep the VS2022 body in the object build:
0x004647f0, 0x005a6d60, 0x005aab70 and 0x005acda0. The VS2022 build's sources are unchanged.

| build | result | rva-lint | exe sha256 |
|---|---|---|---|
| VS2022 (`MASHED_NO_DEPLOY=1`) | Build OK | 313 known, NEW 0 | `48cd14d8e4110847...` |
| object (`MASHED_DDKOBJ=1`) | Build OK | 313 known, NEW 0 | `d4394909e1a2eb1e...` |

`py -3.12 re/tools/ddkobj/ddkobj.py check`: `IDENTICAL 1456/1508, DIFFERENT 37, not-in-map 15`, provisional units
skipped 280. On STEP1 it was IDENTICAL 1456/1512 and DIFFERENT 41. The 4 units above left the DIFFERENT set. Nothing else
changed: IDENTICAL is still 1456.

Gap [filed as row DDKOBJ-PROV-SHARED-UNIT]: `re/match/s10/0045cd30.c` also carries 0x0045c820 (IDENTICAL), so its unit is
not skipped, and the object build still links the DDK object of the provisional 0x0045cd30. That is against L-126 (6a). It
was left as is because changing `ddkobj.py` or splitting the source is outside this row.

Knob boots: `test-slot bash D:/MashedWT/AUDIO-STACK-5-UNMATCHED-BOUND_data/boots.sh`, a copy of AUDIO-STACK-STEP1's
`boots.sh` with only the paths and label changed. Every launch went through `mashed_lock.py run`. It ran locally, on a
private root. The test slot was acquired after a 2208 s wait (all 8 slots busy, after the 10 min memory-gate wait).

| arm | this branch | AUDIO-STACK-STEP1 |
|---|---|---|
| def_off | rc 0, 0 self-test lines | same |
| def/ddk on_img1_cp_trap1, on_img1_cp_trap2 (4 arms) | rc 0, `Calling AppDestroy`, WinMain returned 0, 3 PASS / 0 FAIL each | same |
| def/ddk aud_img1_cp_trap1 (2 arms) | rc 0, AppDestroy, WinMain 0, 4 PASS / 0 FAIL each (incl. `[OT_TIMER] self-test PASS (0 of 8413 checks failed)`) | same |
| vs / ddk init_audio | exit 0x005ADC10 (the same named stop), 3 PASS / 0 FAIL | exit 0x005ADC10, 0 FAIL |

`original/mashed_image.bin` is absent, so the real original was not touched. Only the PIDs this run spawned were killed,
by knob_boot.py.

`original\mashed_re_dev.asi` not deployed. No G-CTRL, B or 12-capture runs (top-down row).

## Raw data (`D:/MashedWT/AUDIO-STACK-5-UNMATCHED-BOUND_data/`, sha256)
```
b40509cd586df0285e417c310261cc41c64baf0835052b71b8eb5887767ccd30  boots.sh
13427f44edc4a0136cee1f926acc51376910b221fb96c76e2538a5083e99f6fc  boots.out
d91d1272bb8eb0f01722a59a25f688d59808394fb484be5869f86a12c080fdf5  build_vs.log
4141eb191e55729f76b7c187d5420258467734be7126103f519d934f7df47281  build_ddk.log
5607a3ea43c59b6839eed27586321ec1f56df5c6bd7fa8e834acf676e42b79e4  ddkcheck.log
a77a6f8863a7680ac4f9be1b63967183d2152f5c8c6542e7c917ca9c229015c4  boots/ddk_aud_img1_cp_trap1.log
16ae46ab93390ae10396f151d1daab0ab62c0211b43efae2724ff781c940de76  boots/ddk_on_img1_cp_trap1.log
5b1294b07fbbc223072a35ef6851a23354b536b29e3548018039276a509a3df0  boots/ddk_on_img1_cp_trap2.log
47ec4bb16b0fc7a40dd26b0f81903dee81df7ffe20acde32fa510c97a6e6e9fd  boots/def_aud_img1_cp_trap1.log
2a3bd6a03d53482904e78289edcf9a5995108c54e7f8e5248d311a42e2f42eb4  boots/def_off.log
7efec10cb20d2cc485fb004a59534c8279d601f61505dc1f4cc525f1af3d2568  boots/def_on_img1_cp_trap1.log
cc90cc59226b60ec40db2ef301d83d5f7fc8cda245be63c4c6bba5f6b9a84794  boots/def_on_img1_cp_trap2.log
b6a7baa3efbef1ab24924dea437d183c8dffe31c3d500ae735ae4fb7fc385dba  vs_init_audio.log
402856e896935b81867b858f13bd656409159aa8f8b0316d89d96d986dbd1326  ddk_init_audio.log
48cd14d8e4110847c91ee96a0a43ff7a2bccf330b0622e8e33e414c742bbc761  default/mashed_re.exe
d4394909e1a2eb1e5bbd761e625de52140d3e2fb5cff45d4fdbf0b29a2e99601  ddkobj/mashed_re.exe
16a7a4db73cbcb302924aab172f0914c1f01a2be416dca63a5e535bd098bd6f0  ddkobj/BYTECHECK.tsv
```
