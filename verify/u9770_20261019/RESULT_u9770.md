# RESULT u9770: car 1's row 149 is the pickup's frame order; car 1 passes regime C (2026-10-19)

Branch `work/u9770` from `f3e534629` (= `race/first-frame-parity` `38de9c963` content). PREREG `PREREG_u9770.md` with the
knob (`e237ee677`). Exe `877d5a04` (`MASHED_NO_DEPLOY=1`), private root `D:\MashedWT\d3bc_root`. Model claude-opus-5-5,
account2. Stack `FULL` = d3bc full stack + the M4 mode-7 set + `MASHED_CC_PC24` + `MASHED_A4_ORDER`, on the ccinv default.

## 0. Verdict

1. **Car 1's row 149 is a frame-order difference.** The original's car 1 collects the pickup it drives to (mode 7) at the end
   of frame 148: the held word `(&DAT_0088fc88)[v*0x2d]` turns `0x5f9b58` at row 149 (o_c30b `--c30-probe`), so FUN_00414490
   returns 0 and FUN_00414c30 returns 0. Writer `0x0045bfb5` via FUN_00458920 (`0x00458a20`) <- FUN_00485070 (`0x00485085`)
   <- FUN_0040fc00 (`0x0040fd0c`) (`--watch-write da98`). FUN_0040fc00 runs FUN_0040da50 (`0x0040fc75`), FUN_0045bba0
   (`0x0040fcd0`) and FUN_00485070 (`0x0040fd07`) after FUN_00425a40 (AI tick + physics, `0x0040fc6c`). The port ran them in
   UpdateRace, before the AI tick, so its held word turned non-zero one row later (row 150).
2. **`MASHED_REG_LATE=1` (default OFF)** runs the three after the cars' physics, in that order around the `[U-9683]` refdist
   site. On `FULL` + the knob: car 1's held word is non-zero at row 149 (G-HELD PASS), and **car 1's command stream equals
   o_reg_r19 on all 196 rows of the C window: car 1 regime C PASS**, the first C pass. In A it equals o_reg_r1 through row 179
   (A PASS). The knob alone is inert (`RL` = the default's hashes): the ticks only act with the M4 set.
3. **(b) on `FULLRL`:** A 1/3 (car 1), B 1/3 (car 1), C 1/3 (car 1). Not a default candidate (it needs the whole default-OFF
   stack, which regresses cars 2 / 3).
4. **Next per-car splits on `FULLRL`** (new U-9771): car 2 at record call 136 (`+0x9b8` -3311.94531 vs -3308.72559, position,
   and the wheel-ring fields `+0x194 / +0x214..+0x21c / +0x2d8..`), commands equal through row 138; car 3 at record call 7
   (`+0x9b8` -1964.98584 vs -1964.98596, the only differing field), commands through row 57.

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | knob unset, exes `00592c86` / `877d5a04`: `d6d87a6c / b903bddb / 0d23fb1f` | PASS |
| G-HELD | `FULLRL` K0: held word 0 at 148, `0x13` at 149 (port encoding), FUN_00414c30 returns 0 at 149 | PASS |
| G-B | section 0 item 3 | report only |

## 2. Note on a name

In the u9769 hand-off I called the car<->car arithmetic knob `MASHED_CC_PRECISION_FIX`. The variable in the code is
`MASHED_CC_PC24` (`Collision/CarCarContacts.cpp`); the merge message of `3ca5a4756` repeats my wrong name.

## 3. Evidence

o_c30 / o_c30b (`--c30-probe 2000 / 3600`), o_wheld (`--watch-write da98`), all regime-A argv on car 1. Evidence in
`D:/MashedWT/u9770_data` (`MOVED_OFF_GIT.txt`).
