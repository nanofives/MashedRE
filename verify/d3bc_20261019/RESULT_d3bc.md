# RESULT d3bc: D3 (b) regime C, the steer path of FUN_00416250 (2026-10-19)

Branch `work/d3bc` from `race/first-frame-parity` `de7551b9c`. PREREG `PREREG_d3bc.md` (section 1 + amendments 1..4, each
committed before its arms). Exe `MASHED_NO_DEPLOY=1`, private root `D:\MashedWT\d3bc_root`. Model claude-opus-5-5, account2.
Scorer `vsum.py` (`check_regime` on `re/tools/b_envelopes.json`, C per car at 196 calls).

## 0. Verdict

1. **Regime C still 0/3 on every stack. No default candidate.** Baseline reproduced (K26 `395fb8b7` x2, K54 = K57
   `961f1ea4` x2, same 5 / 4 / 5 failing items per car as the orchestrator's state).
2. **C is A for 149 rows in the original.** o_reg_r1 (A) vs o_reg_r19 (C): first difference car 1 row 149, car 2 row 151,
   car 3 row 158 (`flag_a368` excluded); r19 vs r20 equal on cars 1 / 2 over all 196 rows. C's envelope is width 0 per car, so
   passing C needs the port's command stream near-exact over ~200 calls, the same work as A, scored exactly.
3. **Three faithful fixes in FUN_00416250's steer path, all default OFF, each measured bit-exact:**
   - `MASHED_STEER_ACOS_CRT`: FUN_00415e20's angle. The port called acosf (`std::acos(float)`) and used 57.2957802 for the
     double at `0x005cc970` (`0x404ca5dc20000000` = 57.295780181884765625). The original's CRT acos FUN_004a3384 keeps the
     caller's PC_24 (FUN_004a70a5 loads `(cw & 0x300) | 0x7f`, `0x004a70a9..0x004a70b7`): `1+x`, `1-x`, product and FSQRT at
     float, FPATAN extended, then FMUL qword rounded to float. With the LUT normalize (`MASHED_AI_LUT` bit 2, FUN_004c39b0)
     the offline form (`emucrt.py`) matches the original's stored error (`0x008032d8 / 0x008032dc`) on **1054 / 1054** rows of
     four original captures (o_aq0 car 1 188, o_c2 205, o_c3 344, o_wobj2 317); a double acos misses 43.
   - `MASHED_AI_FWD_ATROW`: the AI forward host (`aib_own_fwd_xz`, `TrackRenderer.cpp:153`) returned `(cos, sin)` of
     `atan2(at.z, at.x)`; FUN_0046d510 (call `0x0046d53a`) gives the raw `at` row, record `+0x948 / +0x950`.
   - `MASHED_STEER_SCALE_RDATA`: `_DAT_005cd0e8` is `0x3b449ba6` = **0.003f**, the port had 0.0030034f. With the original's
     own err and speed, `trunc(err * speed * 0.003f)` gives its fresh-steer byte on 54/54 (car 2), 76/76 (car 3), 48/48
     (car 1) rows; 0.0030034f misses 6. An audit of every `_DAT_`-annotated constant in `AiStandalone.cpp` finds no third.
4. **G-ERR PASS.** On the u9767 stack + `MASHED_AI_LUT=7` + the three knobs (final exe `4f068afa`), the port's stored steer
   error equals the CRT model on the port's own inputs (`emuportcrt.py`): B car 1 103/103, C car 1 194/194, C car 2 192/194,
   C car 3 180/194. The car-3 misses (rows 44..53) are FUN_00414a70 return-2 rows (`c4 0 / c5 255`, the exit before
   `0x00416596`), where no error is computed and the stored value is the previous one; car 2's two are the last rows. It
   equals the original's on every row while the record is exact (car 1 through 148 in C, `histdiff.py`).
5. **First command row per car, full stack (`EXL7AFSM7` = u9767 stack + LUT 7 + the three + the existing M4 mode-7 set):**
   car 1 row **149** in C and A (was 4 on the default, 71 on the u9767 stack), car 2 row 23 (was 4), car 3 row 39 (was 11).
   Car 1's record equals o_aq0 through call 148 apart from a one-call `+0x39c` difference at call 117. Car 1 C then fails only
   `c0# 19 [20]` and `steer# 39 [40]`.
6. **U-9768 resolved.** The B call-104 look point `(-1.009332, -20.2197)` is FUN_00414c30's mode-7 target (call
   `0x004163c3`, return 2): the same point at C/A row 100. With the M4 set car 1 takes it and B runs exact to row 161.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knobs unset, exes `3c47a766`, `3ca4369f`, `65de976f`, `4f068afa`: `6efb49ed / 3e0724ac / 395fb8b7` each | PASS |
| G-ERR | section 0 item 4 | PASS |
| G-B (report) | `L7AFS` (default + LUT 7 + the three): A 2/3, **B 1/3** (cars 2 / 3 `c1# 50 [17,46]`), C 0/3. `F2` alone: A 2/3, B 3/3, C 0/3. `SC` alone: A 2/3, B 1/3, C 0/3. `EXL7AFSM7`: A 0/3, B 1/3, C 0/3 | FAIL as default |
| G-DEF | not run: G-B regresses B on the default | - |

The pattern of u9764..u9767 holds: each fix makes a car more exact and moves (b) counts both ways, so an offsetting error
remains elsewhere on the default.

## 2. Next gaps (new U-9769)

On `EXL7AFSM7`, the first divergence per car:
- **car 1 row 149** (C and A): the original's FUN_00414c30 returns 0 (A then takes the FUN_004148b0 return, `ret148b0 = 1`,
  `c4 0 / c5 255`; C steers off mode 7); the port's returns 2. Stored error equal at 148, record equal through 148.
- **car 2 row 23**: own position equal at row 22, then `own_x` 1.00629162 (original) vs 1.00472975 at row 23, the call of the
  car-2 contact with the player (`ai_override` rises, 1440 vs 1390). The contact response differs.
- **car 3 row 39**: both take FUN_00414a70's mode 3 at row 39 with own position equal, but the target point differs
  (`(-0.7680652, -2.8144028)` vs `(-0.7629314, -2.8055846)`), so the targeted car's position differs.

Cars 2 and 3 both point at the parked player's state near calls 22..39. Car 2's physics also splits earlier at record call 11
(`+0x214`, 1 ulp) and car 3's at call 7 (`+0x9b8`, 1.2e-4) on the stack without the CRT form; not re-measured on the final exe.

## 3. Code / tools / evidence

`Ai/AiStandalone.cpp` (`MASHED_STEER_ACOS_CRT`, `MASHED_STEER_SCALE_RDATA`), `D3d9Render/TrackRenderer.cpp`
(`MASHED_AI_FWD_ATROW`). Tools here: `emu15e20.py` / `emucrt.py` / `emuport.py` (FUN_00415e20 offline), `regcmp.py`,
`firstmode.py`, `histdiff.py`, `recfirst2.py`, `rowcmp.py`, `cmpint.py`. New original captures `o_c2` / `o_c3` (regime A
argv with `--statediff-car 2 / 3`). Evidence in `D:/MashedWT/d3bc_data` (`MOVED_OFF_GIT.txt`).
