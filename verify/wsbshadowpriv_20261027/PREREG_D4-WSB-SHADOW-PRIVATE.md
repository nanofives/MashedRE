# PREREG D4-WSB-SHADOW-PRIVATE: heap-tracked write-set leg

Committed alone, before the verdict arms below. Branch `orch/D4-WSB-SHADOW-PRIVATE` off `orch/integration` @ `2d75d00d1`.

## Fix under test (`Core/ShadowTrack.h`, armed path only)

`MASHED_SHADOW_PRIVATE=1` froze at the first armed sample (6/6 boots, `verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md`
section 5). Two pre-PREREG diagnostic boots of `0x0055fea0` (`MASHED_SHADOW_TRACE=1`, PRIVATE + STW):

- diag1 (unfixed): 72 regions protected, 0 faults reached `Handler`, watchdog EIP `0x77b0a08c` on code `c2 08 00`
  (syscall stub `ret 8`), stack args `0x77bcaa54, 0`, and `0x77bce3c8` further up.
- Fix: the VEH entry (`s.veh`) and every committed private block a dword of it points to (on x86, `entry+8` is the
  refcount pointer) are kept writable as page holes, for every entry reached by Flink. The region holding them is
  protected around the holes. This replaces the old rule that skipped the whole region holding the node.
- diag2 (fixed, same boot recipe): node dwords `0x00cc74f0 / 0x77bce3cc / 0x103323a0`, 4 holes, 24/24 samples
  `pages=o:1,n:1`, 0 noise, CLEAN, `A/B-IS-REAL installed=e9`. Not a verdict arm (trace on, pre-PREREG, older build).
- Added after diag2: each sample line carries `heap=o:<n>,n:<n>` = tracked pages outside MASHED.exe's image.

## Verdict arms (one boot per row, `run_shadow3.py`)

Rows: `0x0055fea0`, `0x0055ff90`, `0x00564040`, `0x00565260`. Env: `MASHED_SHADOW_AB=1`, `MASHED_SHADOW_PRIVATE=1`,
`MASHED_SHADOW_STW=1`, no trace. Scenario: the census one (`--track 0 --mode 10 --cars 4 --hold 60 --hooks 0x<rva>`),
private root `D:/MashedWT/D4-WSB-SHADOW-PRIVATE_root`, asi from this branch's build. Guards: SAMPLES_DONE 24,
STALL_S 300, NOLOG_S 360.

Per row, from the log:
- **HEAP-CLEAN**: `shadow_ab_report.py` verdict CLEAN (n > 0, ndiff 0, A/B-IS-REAL) **and** `heap=o:` > 0 in every
  counted sample.
- **CLEAN-NO-HEAP**: CLEAN but `heap=o:0` in at least one counted sample. Reported, not a verdict leg for that sample set.
- **DIVERGENT**: any counted sample ndiff > 0. Row stays C3. First-page offsets and values go in the RESULT.
- **NO-SAMPLES / hang / crash**: re-run once. A piz-collision crash (`+0x1509b4`, `+0xe66db`) is a re-run, not a result.
  A second failure is reported as is.

Reported per row: n, ndiff, pages o/n min..max, heap o/n min..max, noise count.

## Tracker rule

`hooks.csv` is untouched unless D4-WSB-L38-DEMOTE (demotes these 4 rows C4 -> C3) has merged into `orch/integration`.
After that merge, a HEAP-CLEAN row is re-promoted C3 -> C4 through re-classify in this branch, citing this leg plus
its existing racediff leg (canonical scenario, hook installed). Any other outcome, or no merge yet: rows stay C3, said
in the RESULT.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this branch's `mashed_re.exe`, against the
`merge_train.json` refs (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664`). Any mismatch = RED.
