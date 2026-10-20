# RESULT u9773: car 3's call-149 split is the wheel solver's missing 3-wheel alignment (2026-10-20)

Branch `work/u9773` from `work/f5flip` `4f4078955` (= merge tip `9cd8831ff` content). PREREG `PREREG_u9773.md` and amendments 1..3,
each committed before its arms. Exe `202ff279` (last build), `MASHED_NO_DEPLOY=1`, root `D:\MashedWT\u9756_root`. New original
captures (B recipe, car 3, each checked against `o_c3`): `o_ww958c3_2`, `o_ww194_1` (write watches), `o_wsp3_1`
(`--wheelstate-probe`), `o_bp3_2` (`--batch-probe`). Evidence in `D:\MashedWT\u9773_data` (`MOVED_OFF_GIT.txt`). No default changed.

## 0. Verdict

1. **U-9773 RESOLVED.** Car 3 (call 149, position first) splits because the port's wheel contact solver FUN_0046f6c0 never runs its
   3-grounded-wheel orientation alignment, so the body stays flat where the original tilts it onto the terrain:
   - the original calls FUN_0046c5f0 at `0x0046fd57` on three per-wheel contact points (`[esp+0xac]`; indices (0,1,2), (0,1,3),
     (0,3,2) or (1,3,2) by the promotion path `0x0046fc09..0x0046fd35`) into `local_e0`; the port skipped the call
     (`WheelContactSolver.cpp`, "residual; no state effect"), leaving `local_e0` 0;
   - the alignment then ran through three stubs (`Collision/ContactStubs.cpp`): `Rw_SetRotation` (empty; the original calls
     FUN_004c52f0(ring, m, 2), RwMatrixCombine, at `0x0046ffff`), `Math_Acos` (returns 0; the original's CRT acos FUN_004a3384 at
     `0x0046ff8f`), `Sys_TickCount` (returns 0; FUN_004a2c48 is `_ftol`, of `+0x9e0` loaded at `0x0046f6e2`, gate `< 3` at
     `0x0046fe10`);
   - and the gate constant: `_DAT_005cea88` is 0.999f (`0x3f7fbe77`), the port had 0.998901f; the dot is grouped
     `(e0*up.x + e1*up.y) + e2*up.z` (`0x0046fdaa..0x0046fdca`).
   Evidence chain: equal per-call solver entry states through the frame (`--wheelstate-probe` vs `MASHED_WCS_POSLOG.ent`); at that
   frame's second classifier call the wheel points differ (original tilted y -0.0325..-0.0652, port flat -0.0474862 x4) and so does
   the body up `+0x9c8` (orig (-0.000394, 0.999063, 0.043615), port (0, 1.000043, 0)); `--batch-probe` vs `MASHED_BATCH_DUMP`.
2. **Knob `MASHED_WCS_TRINORMAL=1` (default OFF)** does all of the above and carries the solver's ring rotation back into the port's
   separate body basis (`VehiclePhysicsRun.cpp`, after the solver, with `MASHED_POS_WHEELS`). With it car 3's wheel points and
   rotation rows at call 149 equal the original's to 1-2 ulp (was flat), and car 3's record holds to call 157 (|dv| > 1e-3 at 158,
   was 150), from ulp drift in the alignment's own arithmetic.
3. **(b) with the knob** (`B_TN`, `28a76cf2 / 76a0b4cd / 040ce640`): A 1/3 (car 3), B 2/3 (cars 1, 3), C **2/3 (cars 1, 3)**.
   Against the default (A 2/3 cars 2, 3; B 2/3; C 1/3): C car 3 FAIL -> PASS, A car 2 PASS -> FAIL (`c0_distinct 36 [13,35]`).
   Not a default candidate under the rule (a passing car fails); user decision.
4. **D2 with the knob**: 0.1900 / 0.2550 / 1964.25, the motion_diag log byte-identical to the default's on the common prefix
   (the solo player never takes this path differently). 13 tracks and G-CTRL not run (not a candidate).
5. **Next (new U-9776):** car 3 past call 157 (the alignment's 1-2 ulp at call 149: acos / RwMatrixRotate / combine order), and
   with the knob, A car 2's new failure; car 2's own C split (call 150) is unchanged.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | every exe of the lane, knobs unset: `B_DEF_K0` `c165d2e5`, `B_RP3_K26` / `B_BD3_K26` `2e937af7` (= the default) | PASS |
| G-149 | `TN` car 3: wheel points and rows equal to 1-2 ulp at 149; record to 157 | PASS (partial: 157) |
| G-B | above | reported |
| G-D2 | 1 run, in band, byte-identical to the default on the common prefix | reported |

## 2. Other findings

- The car-3 drive-torque rings `+0x1ac.. / +0x270..` hold two stale entries in the port (the original fills 16); their one known
  reader is FUN_0046bda0 (render-side car-part steer). Not on this path.
- Hardware write watches miss alternate writes; the original integrates two 25 ms substeps per frame (x step / velocity = 50 ms).
- Diagnostics added (default OFF): `MASHED_WCS_POSLOG` (+ `.ent` entry channel), `MASHED_WCS_TRILOG`; tools `recfull.py`,
  `origcmp.py`, `bqcmp.py`.
