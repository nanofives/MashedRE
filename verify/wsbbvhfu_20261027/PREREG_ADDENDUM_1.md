# PREREG D4-WSB-BVH-FU, addendum 1 (committed before any arm it names)

Written after leg V and the first W row (`0x00563e70`) of the PREREG had run, before S2, S and every arm below.
It only adds requirements. No row can be promoted under this addendum that the PREREG would have refused.

## Why

1. Leg V (`0x00564c80`, 24/24 divergent) logged values at `0x007dc8c8` and `0x007dc8cc`. In every sample the port ends
   exactly 1 above the original on both counters (sample 1: `+8c8` pre `0x4e7`, orig `0x508`, port `0x509`, and `+8cc`
   pre `0x15c`, orig `0x15c`, port `0x15d`). In the port body these are the pair-test increment
   (`RwpSolverCore4.cpp:136`/`:205`) and the pair-accept increment (`:148`/`:219`, followed by `FUN_00563e70`).
2. The same samples log `pages=o:1,n:1`: the original's run wrote one tracked page, `0x007dc000`. But
   `0x00564c80` stores the inflated box at `param_1 + (param_2 & 0xffff) * 0x20` (`RwpSolverCore4.cpp:267-272`) and
   links the primitive into the octree. Those stores land in untracked memory.
3. `Core/ShadowTrack.h:382` / `:409-417`: private (heap) pages are protected only with `MASHED_SHADOW_PRIVATE=1`.
   Without it, only `MASHED.exe` image pages are tracked. Neither `verify/wsbbvh_20261027/run_shadow.py` nor this
   step's `run_shadow2.py` sets it. So an original-side heap write is never restored before the port's run.
4. Prior step's raw logs (`D:/MashedWT/D4-WSB-BVH-WRITESET_data/<rva>/shadow_ab.log`), samples by tracked page count:
   `0x0055fea0` 24 x `o:0,n:0`, `0x0055ff90` 24 x `o:0,n:0`, `0x00564040` 24 x `o:0,n:0`, `0x00565260` 20 x `o:0,n:0`,
   `0x00563e70` 24 x `o:0,n:0`, `0x00561040` 24 x `o:1,n:1`, `0x00561390` 24 x `o:1,n:1`. This step's W row `0x00563e70`:
   24 x `o:0,n:0`. A `RunTracked` sample with `o:0,n:0` compared no memory at all.

[UNCERTAIN] that the V divergence is the untracked-restore effect (the original's link of the primitive persists, so
the port's query meets the primitive itself once more: +1 test, +1 accept). Evidence for: exact +1 on both counters
in 24/24, `o:1` while the body writes heap. Missing: the same compare with heap pages tracked (arm P below).

## Added arms

P. **Heap-tracked write-set**, `MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 SHADOW_TAG=priv run_shadow2.py <rva>`,
   census scenario, one hook per boot. STW is set because the heap walk in `Protect()` is only valid with the heaps
   locked, which `Compare` does only under STW (`ShadowTrack.h:574-582`).
   - Verdict rows: `0x00564c80`, `0x00561390`, `0x00563e70`.
   - Diagnostic rows (the prior step's 5 promotions, no tracker change here, result reported to the lead):
     `0x0055fea0`, `0x0055ff90`, `0x00561040`, `0x00564040`, `0x00565260`.
   - CLEAN-P = n >= 20 counted samples, 0 divergent, `A/B-IS-REAL installed=e9`, AND every counted sample has `o >= 1`.
     A counted sample with `o:0` means the call's writes were not tracked: the row is NOT-COVERED (stays C3).
     NOISY samples are discarded as before. n < 20 after one re-run (deadlock, crash, no log) = stays C3.
R2. **Quick Battle racediff, `0x00564c80`**, on this exe (`run_bvh3.py 00564c80`, tool v7). Rule unchanged from
   PREREG 3 of `verify/wsbbvh_20261027/PREREG_D4-WSB-BVH-WRITESET.md` (arg 3 of the `0x005646c0` call at 16 bits).

## Revised promotion rule (replaces the last paragraph of the PREREG)

- `0x00564c80`: CLEAN-P + R2 C4-CLEAN.
- `0x00561390`: CLEAN-P + R C4-CLEAN. S (STW without heap) and S2 become diagnostics.
- `0x00563e70`: CLEAN-P + C C4-CLEAN. W becomes a diagnostic (it tracked no page).
- `0x00565200`: unchanged, W + C. Its wrapper is `RunRegion` (output region 0x1c at `param_3`), not page tracking, and
  the prior step showed statically that all 6 stores of the body fall inside that region
  (`verify/wsbbvh_20261027/RESULT_D4-WSB-BVH-WRITESET.md`, static item 4).
