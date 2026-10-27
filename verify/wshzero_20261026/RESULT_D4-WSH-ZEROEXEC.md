# RESULT D4-WSH-ZEROEXEC: strict C4 on the 25 zero-execution rows

Text-only classification plus re-classify demotions. No game run, no build. Source list: `verify/d4scope_20261026/SCOPING_D4-WSH.md` Findings 3.
Rule: strict C4 needs a natural-scenario call count (or hook-live diff) on the surviving body. Precedent: 0x0040b9a0 / 0x0040ba60, 2026-10-16.
Result: 10 keep, 15 demote C4->C3.

## Keep (10)

| RVA | Name | Citation |
|---|---|---|
| 0x0042aff0 | MenuButtonDetectA | `re/analysis/negctl/reearn_20261011/c4_a1_m_RESULT.md` table: hook live (0xE9 pre-attach), game calls through the JMP 2505..2543 per run, 36..37 discriminating calls, control RED 37/37 |
| 0x0042b180 | MenuButtonDetectB | same file: 2505..2543 calls, 35..38 discriminating, control RED 36/36, 37/37 |
| 0x0042b310 | MenuButtonDetectC | same file: 2502..2544 calls, 3 discriminating, control RED 3/3. Coverage note there: screen 0x13/0x1e early-return branch not reached |
| 0x0042b770 | MenuButtonDetectE | `c4_menu_button_detect_e_RESULT.md`: 437..689 detector calls per run through the live JMP, call site 0x004409ce, natural Race Results, mmOrig 0 |
| 0x0042ae10 | MenuReadinessCheckA | `evidence/diff_menu_readiness_check_a.csv` (8 rows), note: canonical input-driven diff, both branches return {0,1}. Forced-call character is a residual caveat |
| 0x00482900 | ReplayGetSize | `reearn_20261011/decisions/race_r1.tsv`: keep C4, negctl live race run real=GREEN baseline=GREEN null=RED ret=RED; `evidence/diff_replay_get_size.csv` |
| 0x0040bb90 | SpriteLookupTableB | `negctl/atrisk_20261011.tsv` calls=10 in C4-scenario; `decisions/b01.tsv` keep C4 |
| 0x004c5010 | RwMatrixScale | `log/c4_hotpath_result.json` "0x004c5010" calls=5376 |
| 0x004c3bf0 | Vec2Length | `log/diff/004c3bf0_vec2_length.replay.csv` 960 replayed race calls, live_ret == orig_ret == port_ret on rows 0,1 (all fid/eq/se/st True) |
| 0x004c3c60 | Vec2Normalize | `log/diff/004c3c60_vec2_normalize.replay.csv`, same shape; `log/replay-c3/negctl_summary.txt` calls=1200 (the RED there is the negative control) |

## Demote C4 -> C3 (15)

None has a natural-run call count with the hook installed. Their cited evidence is E9+rel32 install-observe, harness-limited Interceptor (0 invocations), forced-call RPC A/B, or scenario-attach.

| RVA | Name | Why |
|---|---|---|
| 0x0042c2d0 | GetDat0067ecb4 | sentinel-write force-call A/B + E9 verified; interceptor 0 calls |
| 0x0042c2e0 | GetDat0067ecb8 | `log/c4_verify_result.json` install-observe only |
| 0x0042f500 | GetDat0067ea64 | sentinel-write A/B + E9 verified |
| 0x0040b7a0 | HotkeyStringBaseGet | E9 bytes stable 10 s idle; call check failed in verify harness |
| 0x0040ad20 | FrontendGlobalGet | same |
| 0x00430a60 | HudSlotTypePlayer1 | E9 verified; evidence from broken-loader window per R1-B note |
| 0x00430ab0 | HudSlotTypePlayer2 | same |
| 0x0042fe80 | GetRaceEndFlag | E9 verified, idle 10 s alive |
| 0x004c3df0 | RwV3dTransformPoints | `log/install_observe_wsa2_20260616.txt`: JMP-LIVE only |
| 0x004c3730 | RwV3dTransformPoint | `log/c4_hotpath_result.json` calls=0 |
| 0x004c3880 | RwV3dTransformVector | `log/c4_hotpath_result.json` calls=0 |
| 0x004c4a50 | RwMatrixRotateInner | `log/install_observe_wsa2_inner_20260616.txt`: JMP-LIVE only |
| 0x0040b890 | FUN_0040b890 | harness scenario-attach force-call (GetGridCount8or12 export), no natural count |
| 0x0040b8e0 | FUN_0040b8e0 | same (GetScoreThreshold7or10) |
| 0x0040cd90 | thunk_FUN_00425ef0 | same (CountNonZeroPairs) |

Re-promotion path for the 15: run `verify/dupinstall2_20261016/c4_verify.py` and `c4_racediff.py` by RVA with the survivor installed (boot-to-menu and Quick Battle race), keep rows with `calls>0`. Needs game runs, out of scope for this text-only step. Several (getters, RwV3dTransform*) are likely to execute and re-earn quickly.
Rows 0x0040b890 / 0x0040b8e0 have `exe_file` = `exe_main.cpp`, overlapping D4-WSH-EXEFILE.

## Not run
Gates: not run (tracker-only change, no code change).
