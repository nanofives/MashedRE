# account3 C4-ready batch — account2 rewire hand-off (survey 2026-07-24)

Source: account2 worker C4-readiness survey (delegate.ps1, Opus, cost $2.05) over the
`account3 hand-off: C4 promotion still needs canonical Frida` markers left by the 2026-07-23
source-truth rewire campaign. Gate = the thunk-callee C4 ruling
(`re/analysis/B5d_COUPLING_BRIDGE_2026-07-15.md` §7 + CHANGELOG 2026-07-23): a container is
C4-eligible iff **every callee in its body is a reversed, C2+ classified function** — an
original-RVA thunk to a C2+ callee does NOT block C4; only a C0/C1/unmapped/STUBS placeholder does.

## Verdict: 19 C4-READY, 0 BLOCKED (all callees C2+)

| RVA | name | file:line | callees (C-levels) |
|---|---|---|---|
| 0x00423cc0 | PlayerScoreTeamAccGatedB | MenuMiscLeaves_t2.cpp:86 | GetRaceSubMode C3(thunk), GetDat0067ea64 C4 |
| 0x00423d50 | PlayerScoreTeamAccBaseB | MenuMiscLeaves_t2.cpp:142 | GetDat0067ea64 C4 |
| 0x00423dd0 | PlayerScoreTeamAccGatedC | MenuMiscLeaves_t2.cpp:196 | GetRaceSubMode C3(thunk), GetDat0067ea64 C4 |
| 0x00423e60 | PlayerScoreTeamAccCumB | MenuMiscLeaves_t2.cpp:250 | GetDat0067ea64 C4 |
| 0x00423ee0 | PlayerBlock2Field00Get | MenuLeaves_s3.cpp:82 | GetDat0067ea64 C4 |
| 0x00423f60 | PlayerBlock2Field04Get | MenuLeaves_s3.cpp:140 | GetRaceSubMode C3(thunk), GetDat0067ea64 C4 |
| 0x00423ff0 | PlayerBlock2Field50Get | MenuLeaves_s3.cpp:201 | GetDat0067ea64 C4 |
| 0x00424070 | PlayerBlock2Field08Get | MenuLeaves_s3.cpp:259 | GetRaceSubMode C3(thunk), GetDat0067ea64 C4 |
| 0x0040b9a0 | PlayerScoreMaxTest | ScoreMasks_ah3.cpp:100 | VehicleSlotGetter C4 |
| 0x0040ba60 | PlayerScoreGateInvert | ScoreMasks_ah3.cpp:125 | VehicleSlotGetter C4 |
| 0x0040b890 | GetGridCount8or12 | Thresholds_ah4.cpp:37 | GetLiveCarCount C4, GetDat0067ea64 C4 |
| 0x0040b8e0 | GetScoreThreshold7or10 | Thresholds_ah4.cpp:53 | GetLiveCarCount C4, GetDat0067ea64 C4 |
| 0x00424100 | TeamBlockZeroGet | BatchAA_s3.cpp:88 | GetDat0067ea64 C4 |
| 0x004241c0 | TeamBlockOneGet | FrontendLeaves_ad1.cpp:73 | GetDat0067ea64 C4 |
| 0x00428610 | ViewportScaledRectDraw | Cluster_v3.cpp:121 | ScreenWidthGet C3(thunk), ScreenHeightGet C4, HudIm2DQuad C3(thunk) |
| 0x004cc6e0 | RwStreamWriteChunked | D3D9Helpers_q5.cpp:72 | RwStreamWrite_s2 C4 |
| 0x004cc770 | RwStreamWriteBytes | RwStreamWriteBytes.cpp:55 | RwStreamWrite_s2 C4 |
| 0x00430b90 | ProgressBarSetA | SpriteCluster.cpp:317 | all C2+ (0042ac00/ac50/b8b0 C3, b8c0 C4, SpriteLookupC C3, 004739f0/00427e00 C2, 004282a0/00472c60/004a2c48/00473870 C3) + RW-driver vtable (007d3ff8+0x20) [external engine] |
| 0x00439210 | LobbySlotListRender | SpriteCluster.cpp:524 | all C2+ (GetDat0067ea64/SpriteSlotGate/HudSlotTypePlayer0/1/2 C4; 00430760/0042ee40/00473870/0042ef40 C3; 00427e00/004391b0 C2) |

## Prerequisites before the canonical-Frida run

1. **REBOOT REQUIRED** — the 2026-07-23 account3 session's MASHED spawn/kill cycles degraded the
   DirectShow boot state (`project_boot_hang_directshow_intro`); no canonical run boots until reboot.
2. **Re-enable LobbySlotListRender (0x00439210)** — its `RH_ScopedInstall` is MASS-DISABLED
   (SpriteCluster.cpp:659). C4 needs the inline-JMP live; re-enable + rebuild before its diff.
   (All other 18 have `RH_ScopedInstall` live.)
3. **ProgressBarSetA (0x00430b90)** — its only non-hooks.csv call is the indirect RenderWare driver
   vtable dispatch `(**(DAT_007d3ff8+0x20))(...)` = external engine code (like HudIm2DQuad's path,
   analogous to the Win32-API no-callee-gate precedent) — NOT a first-party C0/C1 blocker. Its
   earlier C3 note = `crash_equal_ok` (both sides AV identically at a quiescent-menu uninit driver);
   the canonical C4 diff must hit a state where the progress bar actually renders, or use crash-equal.
4. **STUBS.md hygiene (optional, not a blocker)** — several C2+ callees carry stale active passthrough
   STUBS rows (S-0485/3574/3650 GetRaceSubMode; S-0920 HudIm2DQuad [dup already-cleared S-4246];
   S-3633 GetLiveCarCount; S-3910 00473870; S-3122/3998 00427e00; S-3999 004282a0; S-3658/3926/4006/
   4027/4060/4106 004a2c48; S-4398 0042b8b0). Clear via re-classify so a mechanical stub-check doesn't
   false-block. They do NOT block C4 by the ruling (callees are reversed C2+, not placeholders).
5. **SpriteCluster.cpp in-file "Callee table" comments are STALE** (label callees C1/uncatalogued;
   hooks.csv has them C2–C4). Refresh those comments opportunistically; verdicts above use hooks.csv.

## Run plan (post-reboot)

Group by canonical scenario:
- **Menu/frontend leaves** (16): 0x00423cc0/d50/dd0/e60, 0x00423ee0/f60/ff0/070, 0x0040b9a0/ba60,
  0x0040b890/b8e0, 0x00424100, 0x004241c0, 0x00428610, 0x00430b90 — canonical boot-to-menu / menu-nav
  install-observe or diff-original with the hook LIVE (inline-JMP), per each fn's hooks_registry vector
  where present.
- **Save/stream (2)**: 0x004cc6e0, 0x004cc770 (RwStreamWrite_s2 path) — save/write scenario.
- **0x00439210** — re-enable first, then menu/lobby scenario.

Then `re-classify` each GREEN one C3→C4 (cite the canonical CSV + the thunk-callee ruling), ideally as
one transaction when the account2 worker is idle (tracker-clobber avoidance).
