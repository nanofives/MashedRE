# RESULT C4-NOTAG-AUDIT (run 48, L-163 (a))

Text-only audit. No build, no gates, no tracker edit, no demotion. Base `orch/integration`, branch `orch/C4-NOTAG-AUDIT`.

Table: `verify/c4audit_20261115/c4_notag_audit.tsv` (183 data lines + header; columns `rva`, `cited_evidence_path`, `promotion_commit`, `class`, `reason`).
The `reason` column is filled for every row, not only for "other".

## How the 183 were selected

`hooks.csv` on `orch/integration`, rows with `confidence == C4` and no `c4=` substring in `notes` (csv-parsed, not grep):

```
py -3.12 -c "import csv; r=list(csv.DictReader(open('hooks.csv',encoding='utf-8',newline=''))); \
c4=[x for x in r if x['confidence'].strip()=='C4']; n=[x for x in c4 if 'c4=' not in (x['notes'] or '')]; print(len(r),len(c4),len(n))"
```

Counts: 6483 rows, 4660 C4, **183 without a `c4=` tag**. Checking `c4=` across all columns instead of `notes` gives the same 183. The TSV has 183 distinct RVAs.

Promotion commit = the commit where the row's confidence cell last went from not-C4 to C4 in `git log --reverse -p -U0 -- hooks.csv` (script `promotion_commits.py`, 1588 commits walked, 183 of 183 found, 52 distinct commits). The two other rows of `db06f5319` are `0x0043c5b0` MenuDrawLoop and `0x00473ee0` LogoOverlayDraw (the third, `0x004325c0`, carries a tag).

Classifier: `classify.py` (same dir). For each row it reads the cited files that exist (a path counts as existing in this worktree, the main checkout or as an absolute path, since `log/` is gitignored), the commit message and the commit's own CHANGELOG/RESULT text. It reports what those show, not what the tracker says.

## Counts by class (sum 183)

| class | rows |
|---|---|
| diff with the hook installed | 96 |
| install manifest plus survival | 61 |
| diff with the hook bypassed | 19 |
| file missing | 6 |
| other | 1 |
| **total** | **183** |

## What each class rests on

- **diff with the hook installed (96).** Original vs modded with the inline JMP live: D4-WSB racediff tsv rows (C4-CLEAN, `installed=True`, off_calls == on_calls > 0) or the commit's RESULT stating it (51 rows, racediff raw samples partly off-git with hashes in the RESULTs); negctl re-earn RESULTs of 2026-10-12 (16, hook live 0xE9 before attach, per-call check against the original-bytes model and original runs, old-defect control RED; seven of them use a user-accepted forced input, stated in the reason); leaf/side diffs `c4_leafdiff_gts.json` / `c4_sidediff_gts.json` / `c4_readinessA_gts.json` (6); c4front / c4race compare csv (3); in-process bit-identity self-tests with the JMP installed, logs with `ndiff=0` (5); in-process A/B on the original with the hook live from RESULT_dcleaf / RESULT_dcmix (3, cited paths are csv or off-git `D:/`, not read); `f8fc7622e` rows backed by an entry in `log/c4_racediff_result.json` (10); `474a99d3b` strict in-process A/B, hook installed alone (2).
- **install manifest plus survival (61).** E9+rel32 present, boot-to-menu survival, sometimes a call count, and no original-vs-modded comparison with the hook live. Includes the 2 `db06f5319` rows: the committed manifest `re/analysis/standalone_menu_sm/c4_manifest_frontend_twins_20260610.txt` lists only the three twins as installed=1 and the cited json is the C3 nav-driven twin-vs-original draw-sequence diff, not the C4 evidence. Also 14 save hooks (`1aadeb963`, `observe_c4_save_hooks.csv` install_ok + alive_throughout) and 2 rows (`9bcd1dd55`, `b49625913`) whose canonical run shows execution through an `.asi` export counter while the bit-identity half is the synthetic path1 A/B with the hook bypassed.
- **diff with the hook bypassed (19).** All `f8fc7622e` (the 2026-07-02 re-landing of the stranded `promote-c4` campaign). The cited csv is a path1 force-call original-vs-reimpl diff (all lines match). The commit's CHANGELOG block cites `log/c4_racediff_result.json` for these rows, and that file holds 14 RVAs only: none of these 19. The 2026-10-16 `verify/dupinstall*` manifests list them as installed, later and not part of that commit.
- **file missing (6).** `0x0042b900` (cited `log/c4_stalkerdiff_result.json` is gitignored and absent, here and in the main checkout) and 5 rows (`0x0046ddb0`, `0x00467650`, `0x00468980`, `0x00470670`, `0x0046b540`; commits `fadf5eb63`, `2d9cd2b2f`, `22e9bc96d`) whose cited value is the word `green`, not a path. Their commit RESULTs (`verify/dcphys_20261021`, `verify/dcphys2_20261022`) describe an in-process A/B on the original with the hook live on the default body, so the evidence exists in text but is not at the cited path.
- **other (1).** `0x0042b8c0` (`f8fc7622e`): the cited `log/c4_autodiff_result.json` exists but has no entry for it (it holds `0x0042e3a0`, verdict HOLD).

## Findings for the decision on L-163

1. The class that matches the frida-route bar as written ("original vs modded, canonical scenario, JMP live") is the 96 rows. 87 rows (61 + 19 + 6 + 1) are weaker or unverifiable from their cited path.
2. The 21 rows whose cited path does not support the claim, all from `f8fc7622e`, are the ones to re-earn or demote first: 19 bypassed-diff rows, `0x0042b8c0` (no entry) and `0x0042b900` (file missing).
3. The 5 `green` rows need only a corrected `frida_diff` cell pointing at the dcphys RESULT, if the owner accepts the in-process A/B as the diff.
4. Several `diff with the hook installed` rows rest on a RESULT line, not on a committed per-call file: the racediff samples are in `D:/MashedWT/<step>_data` with SHA-256 in the RESULT. This audit did not re-hash them.

## Limits

- Classification is from the cited file, the promotion commit message and CHANGELOG/RESULT text it points at. No run, no re-hash of off-git data.
- Rows whose cited `log/` file exists only in the main checkout were counted as existing.
- 2 racediff rows had a HOLD or absent line in the cited tsv and a C4-CLEAN run in the RESULT or another tsv. They are classed "diff with the hook installed" and the reason column names the source.
- The class is about the evidence behind the promotion. It says nothing about whether the RE is correct, and the match/libid routes that later landed on some rows (MATCH-S05..S19) are not counted here.
