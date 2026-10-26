# SCOPING D4-WSH: C4 verification lane

Text-only. Base `orch/integration` @ `c2bf772f8`. Source line: `ROADMAP.md:2997` (WS-H, "Continuous; audit stale"),
`ROADMAP.md:2936-2945` (dual-copy burn-down is consolidation, so `exe_file == file` after it).
Method: `hooks.csv` filtered with `Import-Csv` (comment lines dropped), `git log -1 --format=%cs -- <cpp>` per body file,
dates scraped from `notes` + `frida_diff` + `scenario`. Counts are from this tree, not from memory.

## Population

| Measure | Count |
|---|---|
| hooks.csv rows | 5914 |
| C4 rows | 173 |
| C4 with `exe_file` set and `exe_file != file` | 17 |
| C4 with `exe_file` empty | 88 (12 of them have an `.md` in `file`, no body path at all) |
| C4 whose body .cpp was committed after the newest evidence date in the row | 58 rows in 34 files (heuristic, see caveat) |
| C4 with empty `frida_diff` | 4 |
| C4 whose notes say 0 invocations / harness-limited / ZERO-AR / count=0 | 25 |

Caveat on the 58: the git date is per FILE. A formatting or consolidation commit touching a shared TU flags every row in it,
so 58 is an upper bound. The first step below exists to cut it to the rows whose function body actually changed.

## Findings

1. **17 rows where evidence names one file and the body lives in another** (`exe_file != file`):
   `0x004026d0 0x00431ae0 0x00431af0 0x00431b00` (BootLowRvaCluster.cpp), `0x0040b6d0` (UtilLeaves_ac.cpp),
   `0x0042b8d0` (FrameDispatch.cpp), `0x0041c2d0` (HudDispatch.cpp), `0x0040bb70 0x0040bb90 0x00430a10 0x00430a60 0x00430ab0`
   (SpriteGate.cpp), `0x004325c0 0x00432b30` (MenuNavSM.cpp), `0x00494ef0` (Teardown.cpp), `0x0040b890 0x0040b8e0` (exe_main.cpp).
   Their `frida_diff` cites a hook-era CSV or log (`log/c4_verify_result.json`, `reearn_20261011/evidence/*.csv`,
   `c4_install_observe_19batch_20260724.log`). Those ran against the `.asi` copy. Whether the exe body is the same text is
   unchecked. This is the defect class the dual-copy audit named (`re/analysis/DUAL_COPY_AUDIT_2026-09-29.md`).
2. **4 rows have no `frida_diff` at all**: `0x004c5800 0x00431ae0 0x00431af0 0x00431b00`. The three boot setters carry only a
   count=1 boot-to-menu sighting (notes), the rubric's C4 needs a diff.
3. **25 rows cite zero executions of the body** in their own notes. Strict C4 needs the body to execute: `0x0040b9a0`/`0x0040ba60`
   were demoted on 2026-10-16 for this (memory `strict-c4-needs-the-body-to-execute`). RVAs: `0x0042c2d0 0x0042c2e0 0x0042f500
   0x0040bb90 0x0040b7a0 0x0040ad20 0x00430a60 0x00430ab0 0x0042b310 0x0042aff0 0x0042b180 0x0042b770 0x0042ae10 0x004c3df0
   0x004c5010 0x0042fe80 0x004c3730 0x004c3880 0x004c3bf0 0x004c3c60 0x004c4a50 0x00482900 0x0040b890 0x0040b8e0 0x0040cd90`.
   Some are legitimate (the note says Interceptor cannot count through an E9 JMP, which is a harness limit, not proof the body
   never runs). Each needs a verdict: executes in a canonical scenario (cite it), or demote.
4. **Largest stale clusters by body file**: GameSave.cpp 4 rows (git 2026-10-23 vs evidence 2026-10-11), RwpIntegrator.cpp 4
   (2026-10-22 vs 2026-07-24), BootLowRvaCluster.cpp 3 (2026-10-12 vs 2026-06-09), VehicleState.cpp 3, VideoConfig.cpp 3,
   UtilBatch.cpp 3, SpriteGate.cpp 3. GameSave.cpp overlaps D4-SAVE-B (merged), RwpIntegrator.cpp overlaps D2/D3-CF work.

## Steps emitted (ledger rows, all child of the D4-WSH lead)

| Row | Kind | Source line |
|---|---|---|
| D4-WSH-BODYDIFF | text-only: per-function `git log -L` over the 58 stale-candidate rows, output the list whose body bytes or callees changed | this note, Findings 4 |
| D4-WSH-EXEFILE | text-only then frida-boot: for the 17 `exe_file != file` rows, diff the `.asi` body against the exe body, re-run path1 on the exe body where they differ | this note, Findings 1; `ROADMAP.md:2936-2945` |
| D4-WSH-ZEROEXEC | text-only classify, then re-classify demotions: 25 rows with no cited execution | this note, Findings 3 |
| D4-WSH-NOEVID | text-only: the 4 rows without `frida_diff` plus the 12 `.md`-only rows (no body path), each gets a body path or a demotion | this note, Findings 2 |

Order: BODYDIFF first (it shrinks EXEFILE and the re-runs). EXEFILE re-runs boot MASHED, so they run under `mashed_lock.py`
and in `D:/MashedWT/D4-WSH-EXEFILE_root`. Trackers change only through `re-classify`. None of these steps flips a default.

## Notes for the lead

- `roadmap.ps1 add` takes only `<id> <section> "<title>" -Wave -Evidence -Parent`. `deps`, `needs`, `solo` and `next` cannot be
  set from the CLI, so each row carries them in its `note` (set via `set <id> open "<note>"`). The lead may want to patch the
  ledger fields through whatever path owns them.
- Rows are deliberately split by evidence type, not by file, so a worker needs one tool class per row.
- Consolidation rows from the dual-copy burn-down (D4-DC-*) change bodies after their C4 evidence, which is the same class as
  Finding 1. D4-WSH-BODYDIFF should be re-run after the last D4-DC row merges.
