# RESULT RO-BOOT-BLOCK-MASK (run 41, 2026-11-13): boot-variant words of block 0x00773c00 masked in prov_compare.py

Verdict: **GREEN for the two named words.** One finding outside the brief (third word 0x0068bc0c, below).
Row kind text/tooling: no build, no game run, no G-CTRL / B. Existing dumps only. No `hooks.csv` / `re/promotions` change.

## Change (`re/frida/prov_compare.py` only)

- `BOOT_WORDS` = 0x00773c40..0x00773d43 (exe path buffer, 0x104 B, GetModuleFileNameA call 0x004abeb0) plus 0x00773d60
  (kernel32 proc pointer, store 0x004ae2d8). Sources: `verify/rocontroldrift_20261113/RESULT_RO-CONTROL-DRIFT.md` (RO-CONTROL-DRIFT, orch/RO-CONTROL-DRIFT @ 22695e174).
- Block 0x00773c00 is hash-only in the tick records, and a MASK row in a hash-only block drops the whole block
  (`prov_compare.py` old line 133), so no MASK row was added to `warp_normalize.tsv`. Instead:
  - hash-only path: when the block hash differs, `boot_block_check` compares the two runs' `ckptNN.data.bin` over the 1 KB block,
    every checkpoint both runs have, and requires equality on every dword outside `BOOT_WORDS`. The hash must also change at
    the same ticks in both runs. Result is noted in `notes` ("boot-variant block ... masked / NOT masked").
  - raw path (a run recorded with a `--tick-raw` range over the block): `BOOT_WORDS` are dropped like `MASKED` dwords.
- BOOT-CLASS now keys on 0x00773d60 (read from checkpoint 0, since it is not in a raw range) in place of 0x006147d8
  (0x007f1030 stays). Comment updated: RESULT_RO-CONTROL-DRIFT's pair test (354 equal pairs with the same 0x00773d60, 312 differing pairs with a different one, whatever 0x006147d8 holds).

## Proof on existing dumps (`plant_test.py`, output `plant_test.out`)

Reference pair r1/r2 = `D:/MashedWT/PROV-INSITU_data/{r1,r2}` (allowed pair r1,r2), run = today's `PROV-WRITESET-SCENARIO_data/wss_ctl_g04`:

| case | block 0x00773c00 in divergence list | note |
|---|---|---|
| old code, ctl | yes (with 0x0068bc00) | |
| new code, ctl | **no** | 15 checkpoints equal outside the 66 boot words |
| ctl copy, dword 0x00773c10 xored in all ckpts (outside BOOT_WORDS) | **yes**, "ckpt00 dword 0x00773c10 differs" | planted difference still caught |
| ctl copy, 0x00773c44, 0x00773c80, 0x00773d60 xored | no | boot words stay masked |

A second planted word at 0x00773e00 was also caught (run by hand with the same script, address changed; not kept).
BOOT-CLASS sample (`bootclass_sample.json`, RO-MENU-REPLAY runs m_090404_001_4 vs m_090404_004_1): `{'0x7f1030': '39a80501', '0x773d60': '769fe8c0'}` on run and both refs, match True.

## Finding outside the brief (not changed): 0x0068bc00

ctl against r1/r2 still reports DIVERGED at tick 0, now on one block only: 0x0068bc00 (old code: 0x0068bc00 + 0x00773c00).
Checkpoint dwords: 0x0068bc0c = 0x0 in ctl, 0x4 in r1 and r2 (same in ckpt 00, 07, 14, so constant, not boot-session noise from
this pair test). r1 against r2 differs in the same block at 0x0068bc1c / 50 / 54 / b8 / bc, but not at 0x0068bc0c. It is a third
word that the unstepped control holds differently from r1/r2, which is why the scenario RESULT scored stepped runs against the control, not r1/r2. Origin not investigated
[UNCERTAIN]. Suggest a follow-up row if ctl is to be scored against r1/r2. Stepped-vs-control compares are unaffected (same boot, same root).

## Merge order

Independent of RO-MASKS (orch/RO-MASKS @ e116c666b touches `detorig*.py`, `replay_masks*.tsv`; no `prov_compare.py` change) and of
RO-CONTROL-DRIFT. Needs RO-MASKS merged first: **no**. Note for RO-MASKS users: its per-dword mask file serves the detorig compare, not `prov_compare.py`; this row covers `prov_compare.py` only.
Re-recording r1/r2 per boot/root is no longer needed for this block.

## Evidence
Committed: `plant_test.py`, `plant_test.out`, `bootclass_sample.json`, `wss_ctl_g04_vs_r1r2.json`. Raw planted copies in `D:/MashedWT/RO-BOOT-BLOCK-MASK_data` (not committed).
