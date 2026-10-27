# PREREG D4-WSB-C4-SOLV-WS, addendum 3 (committed before the `0x00563e70` census re-run)

Leg R, census, `0x00563e70`: both sides armed (`asi=True`, ON `jmp=[233]` = `0xE9`, OFF `[81]`), both reached
frame 1208, **0 calls on both sides**, twice (the driver's automatic re-run). Verdict `HOLD orig-not-observed,
modded-not-observed`. The run used the tool's default window, A4 calls 300..1200. The BVH-FU census leg this PREREG
named for the row ("its BVH-FU leg") ran with `C4_WINDOW="1 3000"` (`verify/wsbbvhfu_20261027/RESULT_D4-WSB-BVH-FU.md`
section 3: "census, tool v7, A4 calls 1-3000"). This step did not carry the window over. That is a recipe slip in this
step, not a result for the row.

Re-run: `C4_SCENARIO=census C4_WINDOW="1 3000" run_bvh4.py 00563e70`, same build, tool v7. The PREREG pass rule
applies unchanged (C4-CLEAN, `installed=True`, > 0 calls each side). 0 calls again = stays C3.

Arm N (addendum 1) read-outs, for the record before more arms run:
- `0x00568fd0`: 3 counted samples, all OK, heap 1..2 pages per sample, 4 heap pages total, 1000..1249 EMPTY discards
  (every one `rv=param_1`). **HEAP-CLEAN** under addendum 1.
- `0x00568990`: 0 counted samples, 1000..1249 EMPTY discards, every logged one `rv=0`. **NO-WRITES-SAMPLED**.
- `0x0056b7a0`: 0 counted samples, 1500..1749 EMPTY discards, every logged one `rv=0`. **NO-WRITES-SAMPLED**.
