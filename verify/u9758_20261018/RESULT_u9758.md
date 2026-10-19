# RESULT u9758: the world forward +0x9d4 (2026-10-18)

Branch `work/u9758`, rebased onto `race/first-frame-parity` `ffb209b32` (f60flip merged; default G-CTRL `b40b47bc / fb690fb1 /
d3a20f4c`). Model claude-opus-5-5, account2. PREREG `PREREG_u9758.md` with the knob, amendments 1 and 2, each committed before
the arms it governs. Exe `fa9b48ff` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\u9756_root`. Original references: o_wobj2
and the new write watch `o_fw1`. The pre-rebase run (`bf451f914`, exe `26ee6117`) gave the same B, H_FA, D2 pass and 13-track
results; evidence in `D:/MashedWT/u9758_data/verify/arms_bf451`.

## 0. Verdict

1. **The writer is found.** Every write of `+0x9d4 / +0x9dc` (o_fw1, 845 hits) is `FUN_004c3df0(rec + 0x9d4, &DAT_00614708, 1,
   matrix)` with `DAT_00614708` = (0, 0, 1) (`.data`, bytes `00000000 00000000 0000803f`), i.e. the matrix's `at` row. The
   callers are A5 (`0x0046ddb0..0x0046ddc9`, matrix = A4's `[+0x9a8]` half, once per frame before the substeps) and
   `FUN_0046d510` (`0x0046d51e..0x0046d53a`, matrix = `rec + 0x928`). `FUN_0046d510` is called by the AI before physics
   (returns `0x00414605 / 0x00414aad / 0x00415f26`, and `0x0047ed3a` inside `FUN_00470c70` before the A4 loop) and after
   physics by `FUN_004252c0` (`0x0042536f`, gated at `0x00425360`), which the frame driver calls at `0x00425a99` right after
   `FUN_00470c70` (`0x00425a8d`) and `FUN_00422ba0` (`0x00425a93`). Nothing writes it inside the substep loop.
2. **`MASHED_FWD_AT` (default OFF) makes the field exact.** The port wrote `(cos yaw, 0, sin yaw)` at frame entry, after
   the heading is taken, and after every substep's orientation step. Under the knob it writes `0*right + 0*up + 1*at` of
   the body basis at the first two sites and nothing in the substep. Car 1's `+0x9d4..+0x9dc` at window call 0 = o_wobj2.
   At later calls the dumped value equals the original's one call earlier: the post-physics `FUN_004252c0` write is not
   ported. Physics in both reads the frame-start pose's `at` row, so this lag shows only in the dumps.
3. **The forward is not the drift driver.** Car 1's drift is unchanged (call 21 1.202e-4, the same look flip), and the
   call-1 velocity `+0x9b0` (-0.00108456612 / -0.00108465552) and omega `+0x9c0` are unchanged.
4. **Next first-differing field: the up vector `+0x9c8..+0x9d0`.** At call 0 the only float difference left is `+0x9cc`
   (0.999988973 original / 1 port; 0.999989033 under `MASHED_UP_TRINORMAL`). New U-9759.
5. **Default candidate, user decision.** `FA` keeps the (b) pass set (A 2/3, B 2/3, C 0/3; B car 3 c1_distinct 52 -> 51,
   still FAIL), D2 3/3, 13 tracks 0 RED. Knob stays default OFF.

## 1. Gates (rebased exe `fa9b48ff`)

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `c7b40cf2 / 5b9905d8 / f680a529`; `H_DEF` `b40b47bc / fb690fb1 / d3a20f4c` | PASS |
| G-FWD | `SPLSBKNF60FAR1` car 1 `+0x9d4..+0x9dc` at call 0 = o_wobj2 | PASS |
| G-DRIFT | call 21 1.202e-4 (unchanged) | FAIL (next: `+0x9cc`, U-9759) |
| G-B | `B_FA_K0/K1/K26` `843d31ac / 381629ba / b66e1546`: A 2/3, B 2/3, C 0/3 | PASS |
| G-D2 | `D2_FA_1..3`: slip 0.1942 / 0.2522, driving-median 1914.57 / 1914.57 / 1913.93, all in; H1 398/400 | 3/3 PASS |
| G-13 | 39 frames, 870 rows, 870 cited, 0 RED, instrument gates PASS (`D:/MashedWT/u9758_data/tracks_fa2`) | PASS |
| G-CTRL | `H_FA` `ded760d5 / fa7a6390 / 27a97e09` | reported |

## 2. Code

`Vehicle/VehiclePhysicsRun.cpp`: `FwdAt()` / `WriteForwardAt()`; the frame-entry write (when the basis is seeded), the
post-heading write, and the substep write skipped, all under `MASHED_FWD_AT`. Knob off: byte-identical dumps.

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9758 RESOLVED (writer found and ported behind the knob; not the drift driver); new U-9759 (the up vector); hooks.csv note on
`0046ddb0`; one CHANGELOG line.

## 4. User decision needed

`MASHED_FWD_AT` default ON (opt-out at the flip)? Faithful forward; (b) pass set, D2 and the 13 tracks hold; G-CTRL would move
to `ded760d5 / fa7a6390 / 27a97e09`.
