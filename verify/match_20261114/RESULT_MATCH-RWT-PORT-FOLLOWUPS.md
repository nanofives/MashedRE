# RESULT MATCH-RWT-PORT-FOLLOWUPS (2026-11-14)

Verdict: GREEN. Branch `orch/MATCH-RWT-PORT-FOLLOWUPS` from `orch/integration` 198109d50 (contains MATCH-RWT-S5 6edaa47c5).
Row kind: top-down (tables + one comment line in a re/match source). Opus 5.5.

## Result

| item | before (198109d50) | after (branch head) |
|---|---|---|
| data rows in `re/match/PROVISIONAL.tsv` | 1002 | 910 (92 dropped) |
| data rows in `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv` | 990 | 898 (the same 92 RVAs) |
| object build `ddkobj.py check` | IDENTICAL 1241/1275, DIFFERENT 19, units linked 1222/1501 | IDENTICAL **1330/1364**, DIFFERENT 19 (same set), units linked 1311/1502 |
| refusals | 9 (externalized-static-register-convention 5, wrapper-redefines-globals 4) | the same 9 |
| newly DIFFERENT units | | **none** |
| RWT ranking | 121 rows | 122 (0x00556e90 added) |

Commits: ce2f65b40 (the 5 named rows), ea81a4bd6 (87 more rows), e018bf5bf (0x004e4c30 tag fix), plus this RESULT and
the ranking row.

## (1) Provisional rows dropped

**The 5 the row named.** Each one checked on the branch before removal:

| RVA | source (the exe wrapper includes it) | match.py --bytes | relocheck | hooks.csv |
|---|---|---|---|---|
| 0x004dcbb0 (S5) | re/match/rwl01/004dcbb0.c (`RwCore_004dcbb0.cpp`) | 100.0%, 369 B, BYTE-IDENTICAL, tc ddk | OK 24/0 | C4, c4=match on this source |
| 0x004ce970 (S6) | re/match/s20/004ce970.c (`RwCore_004ce970.cpp`) | 100.0%, 375 B, BYTE-IDENTICAL, tc ddk | OK 22/0 | C4, c4=match |
| 0x004e4c30 (S7) | re/match/rwl02/004e4c30.c (`RwCore_004e4c30.cpp`) | 100.0%, 338 B, BYTE-IDENTICAL, tc rw9044 | OK 10/0 | C4, c4=match |
| 0x004e6e00 (S7) | re/match/rwl02/004e6e00.c (`RwCore_004e6e00.cpp`) | 100.0%, 377 B, BYTE-IDENTICAL, tc rw9044 | OK 18/0 | C4, c4=match |
| 0x004cee90 (S7) | re/match/rwl01/004cee90.c (`RwCore_004cee90.cpp`) | 100.0%, 320 B, BYTE-IDENTICAL, tc rw9044 | OK 12/0 | C4, c4=match |

0x004dcbb0's row carried the voided 77.5 NOT-PROVEN score. It is gone from both lists.

**87 more rows, same test.** A scan of every PROVISIONAL row whose RVA is C4 in hooks.csv with a `c4=match; match.py <same
source>` note found 93 rows (the 5 above plus 88). All 93 were re-run on the branch with `match.py --bytes` and
`relocheck.py`: **93/93 BYTE-IDENTICAL and relocheck OK** (table: `MATCH-RWT-PORT-FOLLOWUPS_provdrop_check.tsv` in this
folder). These are rows earlier match rows (MATCH-RWT-A/B, S1..S4, RWL01..RWL05, s19..s22) byte-matched and promoted
without dropping them from the provisional lists. 87 of the 88 are dropped. Their exe wrapper includes the same source
(checked by include line). They are in commit ea81a4bd6 alone, so the lead can drop that commit and keep the 5.

Kept: **0x00551a50** (byte-identical, EQUIV row from RWC-FRAME-492E90). No file under `mashedmod/src/mashed_re` includes
`re/match/rwl04/00551a50.c`, so it could not be checked against "the source the exe includes". Left as is.

Not dropped (C4 by another route, the source is not byte-identical): 0x00556ca0 (c4=frida-replay, 14 B, 9 bytes
differ), 0x0043c5b0 and 0x00473ee0 (C4, sources 2473 / 833 bytes differ). Those rows describe the source, which is
still NOT-PROVEN.

Dropped RVAs (87): 0x004c0910 0x004c1040 0x004c2f60 0x004c2fb0 0x004c5930 0x004c5a00 0x004c5cb0 0x004c5f60 0x004c90a0
0x004c9cd0 0x004c9eb0 0x004cb330 0x004cb490 0x004cb4c0 0x004cb600 0x004cb8a0 0x004cbb70 0x004cbbd0 0x004cc820 0x004cd550
0x004cd900 0x004ce480 0x004cf4a0 0x004d0290 0x004d1640 0x004d1d70 0x004d1e30 0x004d2980 0x004d40d0 0x004d4250 0x004d42b0
0x004d4310 0x004d4dd0 0x004d4f00 0x004d5bc0 0x004d6200 0x004d6910 0x004d7480 0x004d80d0 0x004d8480 0x004dba60 0x004dc5b0
0x004dfab0 0x004dfb10 0x004dfb60 0x004e1b60 0x004e1d20 0x004e1df0 0x004e5d30 0x004e5ef0 0x004e5fc0 0x004e6470 0x004e66d0
0x004e6710 0x004e6760 0x004e6920 0x004e7cc0 0x004e7e50 0x004e98d0 0x004e9910 0x004ebb00 0x004f0ae0 0x004f10e0 0x004f2e70
0x004f3be0 0x004f4270 0x004f8580 0x00533d00 0x00534b60 0x00538600 0x0053d5a0 0x0053eaa0 0x0053fa20 0x0053fb00 0x0053fd30
0x00540340 0x00541b50 0x00543e50 0x00544d20 0x00549900 0x0054b6d0 0x0054b740 0x00551c40 0x005584c0 0x005c4ad0 0x004733b0
0x00473220.

**This is not a tables-only change for the object build.** `ddkobj.py load_provisional()` (re/tools/ddkobj/ddkobj.py:283)
keeps every listed RVA out of the object build. Dropping a row lets the unit's XP DDK object join `mashed_re_ddk.exe`.
The default VS2022 build does not read the list.

## (2) Object build and ddkobj check (rw9044 headers honoured: ddkobj.py reads `// TOOLCHAIN`, line 65)

`cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"`, three times (this builds the default exe too):

1. base (198109d50): IDENTICAL 1241/1275, DIFFERENT 19, not-in-map 15, units linked 1222/1501. Same line as the latest
   INTEGRATION_LOG entry.
2. after the 92 drops: IDENTICAL 1329/1363. **88 units joined, 88 IDENTICAL.** No unit left IDENTICAL. Three dropped RVAs
   (0x004c1040, 0x004d5bc0, 0x004d7480) were already IDENTICAL in the base check. 0x004e4c30 was in no unit.
3. after the tag fix: IDENTICAL **1330/1364**, units linked 1311/1502, 0x004e4c30 IDENTICAL (`_RwCore_004e4c30`, 0 bytes differ).

**Newly DIFFERENT units: none.** The 19 DIFFERENT RVAs are the same set before and after (0x00401690 0x004332a0
0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00 0x00552890
0x005530f0 0x005533d0 0x005538a0 0x00554200 0x00554940 0x005572c0). Every one is already in the integration log at
198109d50. None was touched here.

**Refusals (keep their VS2022 body, not worked around):** the same 9 as on the base. `wrapper-redefines-globals`:
re/match/s03/h2_00414180.c, s01/00404e80.c, s01/00404e50.c, s01/00404e00.c. `externalized-static-register-convention`:
s06/00427e00.c, s06/00427f00.c, s06/00427ff0.c, s06/00428140.c, s09/00458920.c. None is new.

**Defect found and fixed: 0x004e4c30 was invisible to ddkobj.** MATCH-RWT-S7 (994c21cd8) joined the `// MATCH: 0x004e4c30
RpLightStreamRead` comment onto the preceding declaration (`int FUN_004e4dd0(int p0);// MATCH: ...`). ddkobj's
`MATCH_RE` (ddkobj.py:62) needs the tag at line start, so the unit was never built. e018bf5bf moves the tag back to its own
line (as in 1a67669e4). Comment only: re-checked `match.py --bytes` 100.0% BYTE-IDENTICAL, relocheck OK 10/0.

Default exe: built OK each time (`=== Build OK ===`). It cannot see these changes (the list is not a VS2022 input, the
source edit is a comment). The object exe can: it links 89 more DDK objects. Section 2b covers it.

## 2b. Self-test and object-build knob boots (added after lead review)

**`py -3.12 re/tools/ddkobj/ddkobj.py selftest`** on the branch head: **OK, 0 failures** (every case PASS: SEH marking,
CRT math rename, register-convention static, DDKOBJ_REGCONV=refuse refusal).

**Exes.** Base = `ddkobj.py build --link` on this worktree with `re/match/PROVISIONAL.tsv` and `re/match/rwl02/004e4c30.c`
checked out from 198109d50 (1501 units, 1222 DDK objects, the same counts as the base build), out dir
`mashedmod/build/ddkobj_base`, sha256 `d9fcf37919a86560...`. Branch = the final object build of section 2, sha256
`145f4914db2a1e8b...`. Both copied to the data folder before the arms ran.

**Arms.** Same env as `verify/rwccorerun_20261111/run_arms_attach.sh` (`MASHED_ORIG_TOP=1 MASHED_WIN_POS=primary-bl
MASHED_DET_FRAMES=600`, core arm `+ MASHED_RWC_CORE=1`, frame arm `+ MASHED_RWC_FRAME=1`), `knob_boot.py` with
`KNOB_EXE`, private root `D:/MashedWT/MATCH-RWT-PORT-FOLLOWUPS_root` (robocopy of `original\`), all six in one
`test-slot` -> `mashed_lock.py run --label MATCH-RWT-PORT-FOLLOWUPS` (no PRIORITY-HOLD, lock free; the slot waited 10 min
for memory, then started under the cap). knob_boot kills only its own PID on timeout. No timeout happened.

| arm | base object exe | branch object exe | base vs branch log (`verify/rwcdevice_20261111/logcmp.py`) |
|---|---|---|---|
| ddk_off | rc 0 after 24.6 s, "WinMain 0x00492370 returned 0 after 600 frames" | rc 0 after 23.0 s, the same line | 1195 / 1195 lines, 1 differs: the cfglog timestamp |
| ddk_core | 0xFFFFFFFF after 3.5 s (by design, as in earlier RESULTs) | 0xFFFFFFFF after 1.3 s | 33 / 33, 1 differs: the timestamp |
| ddk_frame | 0x5257F001 after 3.7 s (first frame presented on the core) | 0x5257F001 after 3.0 s | 49 / 49, 1 differs: the timestamp |

**Self-test lines** (both exes, same counts): `RwTexDictionaryCreate` binding "self-test PASS (0 of 11 checks failed, 0
failing calls so far)" in core and frame arms, "self-test PASS (0 of 260 checks failed)" in core and frame arms, RWC-DEVICE
"selftest PASS (0 fail)" in the frame arm.

**First-frame Im2D compare** (`verify/rwcim2d_20261113/compare_im2d.py` against the original capture
`D:/MashedWT/RWC-FIRST-FRAME-IM2D_data/orig_im2d_1.log`): **IDENTICAL, 18 of 18 fields** on base and on branch
(`IM2D_COMPARE_base_ddk_frame.tsv` and `IM2D_COMPARE_final_ddk_frame.tsv` in this folder, byte-identical files).

**Verdict: no regression.** The 89 extra DDK objects change no line of the knob-path logs in these three arms. No commit
needs dropping on this evidence. The ddk_off arm runs the main loop 600 frames on librw bindings, so it does not reach
every substituted RW core body. The replay oracle on the object build (L-125) is the deeper check, and it is not this row's.

## (3) RWT ranking

Ranking file: `verify/match_20261112/RANKED_MATCH-RWT-REFRESH.tsv` (MATCH-RWT-REFRESH, population = executed functions
in the RW groups of `composition_1066.tsv`, provisional NOT-PROVEN or g2s NOT-PROVEN).

New NOT-PROVEN provisional rows since the refresh base 145dec4d0 (`git diff 145dec4d0 HEAD -- re/match/PROVISIONAL.tsv`):
nine, all from RWC-FRAME-492E90, plus 0x00498510 (BIND-CRT-498510, CRT input) and 0x00455400 (IMG-5, vehicle), which are
not RW.

- **Added: 0x00556e90** (RtCharset, RW toolkit). Executed on the race path (`re/coverage/reach.tsv`: race_run 1), best 97.5
  (`verify/equivcheck_20261112/measure_final.tsv`), equiv kind x87-region, size 631, hooks C2. Appended as rank 122 so the
  existing rank numbers cited by S1..S4 do not move. Under the file's order rule (race path first, then best_pct) it
  sorts first: no other race-path row is at or above 97.5.
- Not added: 0x00556ca0 (RtCharset print): C4 by frida-replay, which the census counts as closed.
- Not added, not RW (menu and chrome game code on the RWC frame path): 0x0042f530, 0x00433f40, 0x0042e5b0, 0x00473c20,
  0x00474890 (NOT-PROVEN), 0x0043c5b0, 0x00473ee0 (C4 by another route). If the lead wants the RWC frame path ranked too,
  that is a separate list.

No promotion file: every dropped row is already C4 by match in hooks.csv.

## Raw data (D:/MashedWT/MATCH-RWT-PORT-FOLLOWUPS_data, not committed)

| file | SHA-256 |
|---|---|
| build_ddkobj.log (base) | 0b059f4b9d0c30e59a49d50d71d49c9b685a3135e437fcd8823c7e84ec83afd8 |
| build_ddkobj_after.log | 0cb6d9520fc00cdfe598d0030ae6f2f3c50650b8de835ccfd0ea6a01cfaccec0 |
| build_ddkobj_final.log | 706e2df4fb5301844e0d13d1764ce2b4a277db15d7e2bbf9af1fefee238824ce |
| BYTECHECK_before.tsv | aa9651fd8fd6c17ac1825992b9efb541221d4048189bb358e31de7ab3d32f61b |
| BYTECHECK_after.tsv | ae1078d695d08f6d4c5fd4f2e36d9798e7c7e95d796656384bf564feb06b3d10 |
| BYTECHECK_final.tsv | 5677189faa4e33fda505fb385a696734b73b15010e41eba8873d1301a7d55c60 |
| cands_check.tsv (= committed provdrop_check.tsv) | 7c246f3d2e31aed8543363f3153764e9e415f1bd739226377692bdabd9e6526d |
| build_ddkobj_base_relink.log | 8d1192106d5b544d5cd88504034bd79bf48211404cf954558d17a403cee1d8f3 |
| mashed_re_ddk_base.exe | d9fcf37919a86560adf4b7dbdf44d2e5dce8078b64f877be24b4f99c3a88ef83 |
| mashed_re_ddk_final.exe | 145f4914db2a1e8b2a978d1d8357cc130c630a169459c7b1ddf328340e6d7104 |
| arms.out | ef97a30fd933f6f2c43d2622970005496e2be18a81f6a16968029db7f7ba68de |
| arms/base_ddk_off.log | d4d38e1d3da1c1fc657cc6d0148bbe03ddc3f1d9bb29a08ab4f56ea97ddd6653 |
| arms/base_ddk_core.log | afdb2b71a924e7404939455172df8d065807af0ce21b852e8803285be1ccf4a7 |
| arms/base_ddk_frame.log | bcfb55c4ae91e4314e0b6671538c93d439a1e17ba6b8fa7ada04c4e02e969275 |
| arms/final_ddk_off.log | 431ae87952f5eba8d25c004560b27ae298268463a457d3f2fb08cf12e38ba583 |
| arms/final_ddk_core.log | f0fe083ae6b9f7cb96dd9c1068b1a20be7ef8d66709bfd6329892447e8f80c7f |
| arms/final_ddk_frame.log | d306f5cc774062bd10fdcbc8af139c656ae8b8ff5e3545b3509add17320bd332 |

## Gates

G-CTRL / B / 12x4 captures not run (top-down row; the default exe cannot see the change). The merge train runs them
once per batch.

## Open proposals (not done here)

- A lint or a ddkobj warning for `// MATCH:` tags that are not at line start. The 0x004e4c30 case kept a byte-matched unit
  out of the object build with no message.
- 0x00551a50: byte-identical but no exe wrapper includes it. Either add the wrapper or drop the row, as a decision for the
  RWC frame owner.
- Future match rows that reach byte identity should drop their PROVISIONAL rows in the same commit. 92 had built up.
