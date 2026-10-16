# RESULT rsky-verify: the merged tip 2dec160eb (rsky + rhooks + carlod2) in a fresh worktree

**Verdict: no regression.** On `race/first-frame-parity` `2dec160eb` (fresh worktree `work/rsky-verify`, `MASHED_NO_DEPLOY=1`) the 13-track x 3-frame scoring gives
**1047 rows, 0 RED**; the build, G-CTRL and every control match what the two lanes measured alone. Nothing to fix.

- Merged exe `log/rsky/exe_verify.exe` SHA-256 `C3F3B432ADE7BD0C0790CC6CB2019EF74B8BA40F913B2E76CB7EF704EB6B2463` (clean build of `2dec160eb`).
  Control exe `exe_pre.exe` `F95216ACE7E0E395D762BBF6C2F932425ACD990DD8A4696DB23E93F5AAAAF37F` = `042acdac4`, the parent of my merge (rhooks and carlod2 in, rsky out), built in a scratch worktree removed with `diag.py wt-remove`.
- Method as in RESULT_rsky.md: private game root, `run13.py` replays (same traces, shots and frames), `drawlist_diff.py` 3D scoring. Cites: `verify/rskyverify_20261016/cites_merged.json` = rhooks' `cites_rhooks.json`
  (the newest merged set) minus C23 / C23b plus C10's pattern for `@shadowrt` (`make_cites_merged.py`; 44 -> 42 entries, nothing added). 39 of 39 replays first attempt in each arm.

## 1. G-CTRL and build

- G-CTRL on the merged exe: step `2b4d47bb`, gates `356821c8`, ptrace `f7a155e1` (18956 / 55433 / 27716 lines) = the expected default (`evidence/run_lines.txt`).
- Build clean, `[skip] MASHED_NO_DEPLOY set` in the log, no `.asi` deployed. Hand-merged hunk in the prop loop checked: `HookTrackFor` / `HookClumpDrawn` run first, then the Forest tree-clump skip; Forest is not a hook-covered track, so
  the two never meet on a clump.

## 2. Rows per track (13 tracks x 3 frames, same cites for both arms)

| track | rsky alone (RESULT_rsky) | rhooks alone (RESULT_rhooks) | control 042acdac4 (rhooks, no rsky) | merged 2dec160eb | RED merged |
|---|---|---|---|---|---|
| Egypt | 67 | 67 | 67 | 67 | 0 |
| Neustein | 67 | 64 | 64 | 64 | 0 |
| Arctic | 103 | n/a (not in rhooks) | 103 | 103 | 0 |
| Highway | 70 | 58 | 58 | 58 | 0 |
| Sands | 85 | 73 | 73 | 70 | 0 |
| SuperG | 210 | 210 | 210 | 210 | 0 |
| Roundabout | 120 | 88 | 88 | 88 | 0 |
| Storm | 69 | 69 | 69 | 69 | 0 |
| Forest | 57 | 96 | 96 | 57 | 0 |
| Dump | 57 | 66 | 66 | 60 | 0 |
| Warzone | 120 | 88 | 88 | 79 | 0 |
| City | 59 | n/a | 66 | 59 | 0 |
| Training | 63 | n/a | 63 | 63 | 0 |
| **all** | **1147** | 879 (10 tracks) | **1111** | **1047** | **0** |

- The 10 capfix tracks of the merged tip total 822 rows. rhooks' 879 minus rsky's own effect on those tracks (Forest -39, Sands -3, Dump -6, Warzone -9) is 822.
- The 13 tracks total 1047. rsky's 1147 minus rhooks' effect (Neustein -3, Roundabout -32, Sands -15, Highway -12, Warzone -41, Dump +3) is 1047.
- Merged against the control, row by row (`evidence/compare_pre_vs_merged.txt`): **15 frames identical, 76 rows removed, 55 counts reduced, 12 rows added**, the same numbers rsky measured on its own base. The 12 added are the sky STATE rows
  (`Sky3`, `Sky4`, `skyCylTop` under C19, `sky` under C20; 3 frames each). The control has 32 RED rows under these cites (12 C23 + 20 C23b), the merged tip 0.
- Matched triangles, merged (%): Neustein 87.3 / 88.0 / 80.0, Warzone 94.0 / 95.2 / 82.5, Roundabout 83.8 / 90.6 / 89.5, Forest 91.9 / 91.9 / 87.7; Egypt, Arctic, Highway, Sands, SuperG, Storm, Dump, City, Training equal the control's
  (`evidence/tables_pre_vs_merged.md`).

## 3. Neustein named capture (the gate-clean sample)

Merged: 227 / 223 / 179 matched draws, 87.3 / 88.0 / 84.4 % triangles, rows 20 / 20 / 21, **0 RED, CITED, gates PASS** (125/125, 122/122, 77/77 named). Control: 1 RED per frame (the C23 `skyCylTop` row). The capfix Neustein row 1169
keeps its r10 I-NAME miss (73/77), the only gate failure of the 13 tracks (`evidence/verify_gates.txt`).

## 4. Controls

- **All six opt-outs** (`MASHED_NO_SKYSLOTS`, `_OBJBSP`, `_SECTOR_BINMESH`, `_HOOKSLOTS`, `_CLUMPANIM`, `_SCENARIOVIS`) on the merged exe, 7 tracks x 3 frames: row identities and counts equal the `f76ae153b` baseline on **21 of 21** frames
  (`evidence/compare_f76ae153b_vs_allknobsoff.txt`).
- Both lanes' defaults stack without interaction: the control-to-merged delta is rsky's isolated delta (above).

## 5. Not measured

Same limits as RESULT_rsky.md section 6 (Forest tree pixels and the object world's lighting, sky pixels). No interaction of rhooks' per-copy cull / ZWRITE states with the sky pass was seen in the rows; pixels were not compared.

## 6. Reproduce

```
py -3.12 verify\rskyverify_20261016\make_cites_merged.py
pwsh -NoProfile -File verify\rskyverify_20261016\run_verify.ps1 -Root <private gameroot> -Out log\rsky
py -3.12 verify\rsky_20261016\run13.py score --out log\rsky\verify --cite verify\rskyverify_20261016\cites_merged.json --prefix rs_
py -3.12 verify\rsky_20261016\compare_rows.py log\rsky\pre log\rsky\verify --prefix rs_
```
