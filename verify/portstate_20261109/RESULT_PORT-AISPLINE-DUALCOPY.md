# RESULT PORT-AISPLINE-DUALCOPY: CatmullRom calls the one exe body of 0x00443300

Verdict: **GREEN.** The exe's race spline `CatmullRom` (`Ai/AiStandalone.cpp:306`) now calls `AiSplineEval_x87_ddk`
(`Match/MatchS08_00443300.cpp`), so `mashed_re.exe` has one body for 0x00443300. Every gate is unchanged, with
`MASHED_X87_PC24` ON (the default since L-105 (a), the acceptance case) and OFF (the opt-out).

Lead cmw7xu1960f37p41557tefpnd. Branch `orch/PORT-AISPLINE-DUALCOPY` from `orch/PORT-AISPLINE-FLIP` `c0afb40de`, with
`orch/PORT-X87-PC24` `ab22937fa` merged (`80b7bd2a5`, one conflict in `run_arms.py`, both stack blocks kept). PREREG
`716bcb0f2` (`PREREG_AISPLINE_DUALCOPY.md`, committed alone, before any arm). Code `d6ed23348`. Exe `f3da6885`
(`MASHED_NO_DEPLOY=1`).

## 1. Change

- `CatmullRom`, standalone: keeps its port-only `count <= 0` guard, then `AiSplineEval_x87_ddk(spline, idx, t, out)`.
  The d3b `compL` arm (AI LUT mask bit 0x4) is deleted. Layout check: `Path_00443300` = 64 `{x, z}` floats, `count` at
  +0x200, which is what `CatmullRom` read (`spline + i*8 + o`, count at +0x200).
- `.asi`: unchanged (`AiLutMask()` is 0 there; the source-order `comp` form stays under `#ifndef MASHED_STANDALONE`).
- AI LUT mask bit 0x4 has no reader left (comment at `AiLutMask`). The mask stays the constant 7.
- Side effect: the look replay's mode 0 (`s_aiLutOverride = 0`) used the source-order `comp` form in the exe and now gets
  the DDK body. Report-only tool, no gate reads it.
- `MASHED_NO_AISPLINE_DDK=1` (L-104 opt-out) still selects the matched body for `AiSplineEval_x87` only (the
  `MASHED_AILEAF` leaf and the `.asi` export). It never reached `CatmullRom`, before or after this step.

## 2. The PC_24 question

The lead's note said `CatmullRom`'s one-expression form matches the original only while the caller holds PC_24. In the
exe it did not depend on the CW: `AiStandalone.cpp` is `/arch:SSE2` (not in `x87_tus.txt`), and its two lambdas in
`mashed_re.exe` `55e5c513` (`0x0007bb90`, `0x0007bc60`) are scalar SSE2 only (`mulss` 9/8, `addss` 7/7, `subss` 5/5,
one `fld` each for the float return), so each op already rounded to float. `AiSplineEval_x87_ddk` rounds each op to
float through `fstp dword` (PORT-AISPLINE-DDKORDER section 2). Same ops, same order (0x00443373..0x00443427), both
CW-independent: the swap is value-neutral by construction, and the arms below confirm it on both CWs.

## 3. Arms (`D:/MashedWT/PORT-AISPLINE-DUALCOPY_data/arms`, merge-train runner, test-slot + `mashed_lock.py run`)

| arm | PC_24 | hashes | reference |
|---|---|---|---|
| H_X24 | ON (default per L-105) | `8284be46 / 67931834 / 31fe043e` | G-CTRL ref, same |
| B_X24_K0 / K1 / K26 | ON | `1d5bf4f9` x3 | B ref, same |
| W_X24 | ON | `eeb92153 / a329ca22 / fa8618df` | PORT-X87-PC24 W_X24, same |
| H_DEF | OFF (opt-out) | `8284be46 / 67931834 / 31fe043e` | same |
| B_DEF_K0 / K1 / K26 | OFF | `1d5bf4f9` x3 | same |
| W_DEF | OFF | `eeb92153 / a329ca22 / fa8618df` | same |
| W_AL | OFF + `MASHED_AILEAF=1` | `eeb92153 / a329ca22 / fa8618df` | DDKORDER W_AL, same |

W reference: `merge_train.json` has no W entry. `eeb92153` is the Warzone step hash that PORT-AISPLINE-DDKORDER (W_AL,
W_ALDDK) and PORT-X87-PC24 (W_DEF, W_X24) measured, with and without `MASHED_AILEAF=1`. RUNLOG: `MASHED_X87_PC24=1` on the
5 X24 arms, `MASHED_AILEAF=1` on W_AL. Every arm self-exited with exit 0.

PREREG P1..P4: all held.

## 4. rva-lint and the allowlist

`scripts/lint_rva_bodies.py` (and the build's own pass): `OK -- no NEW duplicate bodies` (the 1 finding is the existing
allowlisted CROSS-TARGET row). **The allowlist does not go down:** 0x00443300 has no row in
`re/tools/dual_copy_allowlist.txt`, because the scanner never bound `CatmullRom`'s `FUN_00443300 —` banner to the RVA.
The second body was invisible to the guard, so removing it changes no lint count. The banner now says the function is a
call-through.

## 5. Raw files (not committed)

| file | sha256 |
|---|---|
| mashed_re.exe | f3da68859d9f8fbaf5121160c56bdc0a7467298130b519f1fdcc52fdf6bc9010 |
| build.log | 7ebdd2c3db66399022ef05eef2b00cc38666c02427af7f28e0f55b69bdb3d1b3 |
| arms.log | 6d3744d1fc6fc4c737e4aea389a5e30f8f21c041254e307b63275a93212e24b2 |
| arms/RUNLOG.txt | c8f9246140cef793ada70fc2c5f167cdd59aedde460199578283c46cf709bef5 |
| arms/H_X24.step.csv (= H_DEF) | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_X24.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_X24.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_X24_K0.step.csv (= every B arm) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/W_X24.step.csv (= W_DEF, W_AL) | eeb921531da3e3086f8c63eb57c2821c163cf4710a67e83d953813ccf84eb955 |

D2 and the 13-track set were not run: the step is value-neutral on H, B and W at both CWs, and `full_set_paths` covers
`Ai/`, so the merge train runs them on this branch.
