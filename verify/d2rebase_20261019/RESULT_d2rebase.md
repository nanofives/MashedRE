# RESULT d2rebase: the D2 re-baseline (2026-10-19)

Branch `work/d2rebase` from `9145c9c06`. PREREG `PREREG_d2rebase.md` (`d87540af3`), amendment 1 (`7ddfb75fa`: option A).
Exe `3a288253` (`MASHED_NO_DEPLOY=1`), host LIVING-PC. Model claude-opus-5-5, account2. No default changed, no band applied to
any gate yet.

## 0. Verdict

1. **The 8-run band** (`band.json`, rule unchanged): slip1500 **0.18396 .. 0.20586** (3h), slip2000 **0.24555 .. 0.25557**
   (2%), driving-median **1893.63 .. 1997.46** (3h). H1 unchanged (every original run 398/400, median 1333.9).
2. **The scorer.** `score_d2_v2.py` reproduces the four `d81a8df6` original values exactly and scores all eight originals
   identically with and without `--max-lines` (`orig8_scores.txt`). Port mode = the register's reduction (`--max-lines 1080`);
   `port --legacy` = `score_d2.py`'s whole-log reduction.
3. **The registered `--max-lines 1080` does not cover the original's window.** The original's driving-median uses 1153-1185
   samples (slip 305-314 / 540-581). The port at 1080 lines gets 774-871 (slip 195-242 / 328-357), about 70%; the whole port
   log gets 1322-1339 (slip 295-331 / 604-654), about 115%. On the capped window the port's driving-median falls to 1822-1880
   for nearly every arm. The whole log is the closer window. Which window the port should be scored on is a user decision;
   this lane does not pick it.
4. **The three arms, three fresh runs each (each alone), option A recipe:**

| arm | reduction | slip1500 | slip2000 | driving-median | H1 | verdict |
|---|---|---|---|---|---|---|
| current default | 1080 | 0.1890 in | 0.2502 in | 1878.65 **OUT** | 398/400 | FAIL |
| current default | whole log | 0.1942 in | 0.2522 in | 1912.10-1916.54 in | 398/400 | PASS 3/3 |
| pair LUT + F32 | 1080 | 0.1930 in | 0.2543 in | 1861.48 **OUT** | 398/400 | FAIL |
| pair LUT + F32 | whole log | 0.1887 in | 0.2537 in | 1965.90 in | 398/400 | PASS 3/3 |
| triple LUT + F32 + SUSP_LUT | 1080 | 0.1930 in | 0.2542 in | 1855.86 **OUT** | 398/400 | FAIL |
| triple LUT + F32 + SUSP_LUT | whole log | 0.1859 in | 0.2534 in | 1988.09-1989.05 in | 398/400 | PASS 3/3 |

   Runs agree within the new `h` in every arm (the 1080 values are identical across the three runs; whole-log
   driving-median spreads 4.4 / 0 / 1.0, under h = 17.305).
5. **Earlier verdicts** (`hist_table.txt`, 85 logs, 31 arms): with `--max-lines 1080` + the new band every earlier D2 PASS
   turns FAIL (driving-median), and only u9754 `D2_SB` turns FAIL -> PASS. With the whole log + the new band every earlier
   PASS stays PASS and these earlier FAILs turn PASS: u9760 `D2_A6L` (current default), u9761 `D2_F32`, u9762 `D2_TRI`,
   d2ref `D2_RY` / `D2_RYT` / `D2_T5`, u9754 `D2_KN` / `D2_SB` (1/1) and `D2_SBKN` (1/3). Unchanged FAIL either way: d2ref
   `D2_G`, `D2_RG`, `D2_RGB`, `D2_R`, u9749 `D2_CLR` / `D2_CLR5`, u9756 `D2_F44` / `D2_F6044`, a4order `D2_A`.

## 1. Files

`band.json`, `score_d2_v2.py`, `orig8_scores.txt`, `hist_logs.txt`, `hist_v2.txt`, `hist_legacy.txt`, `hist_table.py`,
`hist_table.txt`, `fresh_scores.txt`, `run_arms.py`, `RUNLOG.txt`. Port logs in `D:/MashedWT/d2rebase_data`.

## 2. User decision needed

Which port window the D2 gate uses with the new band: the registered `--max-lines 1080` (every arm, the default included,
fails on driving-median) or the whole log (all three arms pass). Until then lanes report both
(`score_d2_v2.py port` and `port --legacy`).
