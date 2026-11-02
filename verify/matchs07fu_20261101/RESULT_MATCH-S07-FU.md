# RESULT MATCH-S07-FU, 2026-11-01

**Verdict: GREEN.** 8 of the 9 RVAs named in the row (plus the in-file callee 0x00431b80) are swapped in: the MATCH-S07
matched source is each RVA's only port body (`Match/MatchS07_<va8>.cpp`). 0x00430b90 stays unswapped. The two swaps an
exe caller reaches keep the pre-swap exe behaviour behind default-OFF knobs (PREREG committed alone first,
`PREREG_MATCH-S07-FU.md`, 3a0ca614f). Default gates on this branch's exe: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = merge_train.json refs (P1 holds). Knob arms: both knobs ON give the same hashes (P2 holds: inert on the gate scenario).
No re-classify here (trackers through re-classify after the merge; text below).

Branch `orch/MATCH-S07-FU`: code 758d209c0, PREREG 3a0ca614f, this RESULT on top.

## Static leg (re-run on this branch)

Sources unchanged from MATCH-S07 (git-content sha256 equal to `verify/matchs07_20261101/results.tsv`; `swaps.tsv`).
`match.py --bytes` (DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`) and `relocheck.py`, full output in
`match.txt` / `relocheck.txt`:

| rva | name | match.py (whole symbol, relocations masked) | relocheck |
|---|---|---|---|
| 0x0042b960 | CarSlotInit1P | 116 B BYTE-IDENTICAL (insn 90.9% on Ghidra's short size) | OK checked=11 unchecked=0 |
| 0x0042b9e0 | CarSlotAssign | 377 B BYTE-IDENTICAL (insn 96.3%, same reason) | OK checked=14 unchecked=0 |
| 0x0042d290 | MenusLapTimeFmt | 111 B BYTE-IDENTICAL, 34/34 ins | OK checked=10 unchecked=1 (`__ftol`) |
| 0x0042f7b0 | FrontendCursorUpdate | 288 B BYTE-IDENTICAL (insn 98.0%) | OK checked=4 unchecked=0 |
| 0x0042ef40 | VehicleUnlockFlagGet | 215 B BYTE-IDENTICAL, 37/37 ins | OK checked=5 unchecked=0 |
| 0x00431d00 | CarSelectReset | 98 B BYTE-IDENTICAL, 21/21 ins | OK checked=10 unchecked=0 |
| 0x00431b80 | FUN_00431b80 (static callee in the same file) | 370 B BYTE-IDENTICAL (insn 96.7%) | OK checked=29 unchecked=0 |
| 0x0042e3a0 | MenuChromeShellA | 487 B BYTE-IDENTICAL, 138/138 ins | OK checked=17 unchecked=0 |
| 0x0042fb70 | MenuSpriteDispatchB | 701 B BYTE-IDENTICAL, 209/209 ins | OK checked=40 unchecked=1 (`__ftol`) |

## Swaps

`swaps.tsv` has wrapper, targets, x87, symbol, install, source blob. `rva_body_scan`: one body per RVA, in the wrapper.
Build lint `[rva-lint] 3 finding(s) ... allowlisted=3 NEW=0`, `[asm-lint] OK`; `dual_copy_allowlist.txt` unchanged
(none of these RVAs was listed). x87 TUs added: MatchS07_0042d290, _0042e3a0, _0042fb70.

| rva | old body (removed) | what differed from the matched source | how the swap lands |
|---|---|---|---|
| 0x0042b960 | MenuButtonDetect.cpp | exe copy skipped FUN_0040e480 (CallSlotWrite, standalone no-op) | outright: no exe caller; callee bound to `CarSlotStateSet` (one body, both targets) |
| 0x0042b9e0 | MenuButtonDetect.cpp | same skip; exe caller in Team Select (`RenderFrame`) | **knob** `MASHED_CARSLOT_ASSIGN_SLOTSTATE` (default OFF = skip, as before) in the exe's FUN_0040e480 forwarder; the .asi forwarder always calls `CarSlotStateSet` (old .asi called 0x0040e480 by address, installed to the same body) |
| 0x0042d290 | MenuMixed.cpp (.asi only) | old body ignored param_2 and `_ftol`ed whatever ST0 held; original converts float param_2 | outright (.asi only, no exe body); sprintf (0x004a2b60, platform CRT) through a naked tail-jump forwarder (MatchS05_00423480 form) |
| 0x0042f7b0 | MenuHelpers.cpp | decrement guard `> 0`, original `!= 0` (differs only for a negative entry); exe caller in Ability Select | **knob** `MASHED_ABILITY_CURSOR_MATCHED` at the exe call site: OFF (default) calls `FrontendCursorUpdatePreSwap` (the old text, exe only, no RVA comment, MenuHelpers.cpp), ON calls the matched body. The .asi installs the matched body |
| 0x0042ef40 | VehicleMeta.cpp | unknown param_2 read byte 0, original returns 0 | outright: no exe caller |
| 0x00431d00 (+0x00431b80) | GameModeCarSelect.cpp | old called the mover as (0,0),(r,0),(0,0) through CallCursorMover (exe: a stand-in); original FUN_00431b80(0,1,0),(1,1,0),(2,1,0) | outright: no exe caller; still uninstalled in the .asi (old install was commented out); 0x00431b80 gets its first port body (static in the wrapper); CallCursorMover stays for CarSelectCycleColour |
| 0x0042e3a0 | MenuSpriteDispatch.cpp | **second amber tick at y = 65.5; the original draws y = 67.0** (float `0x42860000` at 0x0042e52f, `MASHED.exe.unpatched`; no 65.5 in 0x0042e3a0..0x0042e586) | outright: the exe's only call is under `if (false && g_b16_chrome_ready)` (exe_main.cpp B16), so no exe caller; .asi install moved |
| 0x0042fb70 | MenuSpriteDispatch.cpp | old `int id = 0` where the original passes an unwritten stack slot (row 0, DAT_0067ea84 outside 0..3); double vs float intermediates | outright: no exe caller; HELD callees bound to MenuGroupCount / Calc42ac50 / ScreenWidthGet / ScreenHeightGet (all both targets), the rest raw forwarders as the old body called them |

The old 0x0042e3a0 body is the copy behind the row's **C4 (frida route)**; its 65.5 constant is a port error that
the cited evidence (`diff_menu_chrome_shell_a.csv`, `navigate-gts-canonical-sidediff`) did not catch. The tick lines
draw only when DAT_0067f17c > 9.

### Not swapped: 0x00430b90 ProgressBarSetA

Unchanged from MATCH-S07: compiled as the port body, the matched text needs the `#ifdef MATCH_PORT id = y;` line
(the original's case-3 `id` shares y's stack slot; C leaves it uninitialised), so the port body would not be the
matched text. Tried here:

- `id = y;` compiled for the DDK too: 50 differing bytes.
- y used as `id` (one variable): 13 differing bytes, only the frame slots of y and c3 swapped
  (`[esp+0x1c]` / `[esp+0x20]`); four declaration orders give the same 13 bytes.
- `else id = y;` after the three cases: 205 differing bytes.
- `match_permute.py` on the one-variable form, 900 s, `--walk 60`: no improvement (96.6%, 384 compiles).

The old .asi-only body stays. Raw: `D:/MashedWT/MATCH-S07-FU_data/perm/`.

## Exe callers (L-68)

`verify/matchs07_20261101/tools/exe_reach.py` on this build (`exe_reach.txt`):

| symbol | direct sites | reach from `_WinMain@16` |
|---|---|---|
| CarSlotInit1P, VehicleUnlockFlagGet, CarSelectReset, MenuChromeShellA, MenuSpriteDispatchB | 0 | no |
| CarSlotAssign | 1 | RenderFrame <- WinMain (Team Select) |
| FrontendCursorUpdate | 1 | RenderFrame <- WinMain, only when `MASHED_ABILITY_CURSOR_MATCHED` is set |
| FrontendCursorUpdatePreSwap | 1 | RenderFrame <- WinMain (the default) |
| MenusLapTimeFmt | not in the exe | |

## Gates

Runner `verify/savelink_20261023/run_arms.py` (new `STACK['S7FU']` = both knobs ON), exe `3d467f09` (build of 758d209c0,
copied to `D:/MashedWT/MATCH-S07-FU_data/exe_g1/`), private root `D:/MashedWT/MATCH-S07-FU_root` (robocopy of
`original/`), `test-slot` + `mashed_lock.py run --label MATCH-S07-FU` (queued behind another worker's lock and a
merge-train priority hold, then ran). Every arm exit 0, self-exited.

| arm | step / gates / ptrace (H), step (B) | merge_train.json ref | PREREG |
|---|---|---|---|
| H_DEF | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | P1 holds |
| B_DEF_K0, K1, K26 | 1d5bf4f9 x3 | 1d5bf4f9 x3 | P1 holds |
| H_S7FU | 8284be46 / ba407a16 / 31fe043e | same | P2 holds |
| B_S7FU_K0, K1, K26 | 1d5bf4f9 x3 | same | P2 holds |

Witness for P2: `ABIL_TICK` occurs 0 times in all 8 arm logs, so the Ability Select step never ran. As pre-registered,
this shows the knobs are inert on these gates. It does not measure what either knob does on Team Select or Ability
Select.

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue.

## Re-classify text (apply after the merge, through re-classify; not applied here)

Common evidence: `match.py --bytes re/match/s07/<va8>.c` BYTE-IDENTICAL under `flags=/O2 /GS /Ob1 tc=ddk`,
`relocheck.py` OK (table above), one body (`Match/MatchS07_<va8>.cpp`), gates unchanged on this build. The `file`
cell moves to the wrapper.

- 0x0042b960, 0x0042ef40, 0x00431d00, 0x0042fb70, 0x0042d290 (.asi only): stay at their level (C3, C3, C2, C3, C3);
  `notes` += `match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`.
- 0x00431b80: C2, first port body; same note (exe-callers=0).
- 0x0042e3a0 MenuChromeShellA: C4 by the frida route today. That evidence measured the old body, which drew the
  second tick at 65.5 where the original draws 67.0, so it does not cover the body that now ships and was not a
  faithful body. Recommended: demote to C3 with `match evidence: identical, swapped, exe-callers=0 (the exe call is
  under if (false && ...)); old frida C4 measured a body with a wrong constant (65.5 vs 67.0 @0x0042e52f)`. Whether a
  frida-route C4 survives a body swap is a rubric question for the lead; flagged, not decided here.
- 0x0042b9e0 CarSlotAssign and 0x0042f7b0 FrontendCursorUpdate: the exe calls CarSlotAssign, but by default through
  a forwarder that skips FUN_0040e480, and calls FrontendCursorUpdatePreSwap instead of the matched
  FrontendCursorUpdate. So by default the exe does not run the original's behaviour on either path: **not C4** while
  the knobs are OFF. `notes` += `match evidence: identical, swapped; exe default keeps the pre-swap behaviour behind
  MASHED_CARSLOT_ASSIGN_SLOTSTATE / MASHED_ABILITY_CURSOR_MATCHED (default OFF, MATCH-S07-FU)`.

## Next

- A flip of either knob needs an arm that reaches the screen (Team Select for CarSlotAssign, Ability Select for the
  cursor step); the gate scenario never runs them (P2). Then a roadmap decision row per knob.
- 0x00430b90: a deeper permuter run (frame-slot order of y/c3) or a lead ruling on the one port-only line.

## Raw files (not committed; `D:/MashedWT/MATCH-S07-FU_data`)

| file | sha256 |
|---|---|
| exe_g1/mashed_re.exe (build of 758d209c0) | 3d467f09fc71bec005681015c286d8813f3bf7959602afe7b008d17d177ee2cf |
| exe_g1/mashed_re_dev.asi | 70521f1a3f06c6bb310e0f227cc7c7310623ee1bb8bcf6df68a403a7b3a2094b |
| exe_g1/mashed_re.map | b15e86e9fd713a9d53320a06433eaf1b7a025be34e0aacda9d7c42ccb18df143 |
| build1.log (Build OK) | fbc01f7097eb55e258d310b236ded127427eaf7d35b54dbe18a2d6baa4271c43 |
| arms/H_DEF.step.csv, arms/H_S7FU.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv, arms/H_S7FU.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log, arms/H_S7FU.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_{DEF,S7FU}_K{0,1,26}.step.csv (all six) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/RUNLOG.txt | 7378a24e8a50abbecac727f2d96dac27e482dbf203d37808b74094f89b1050d3 |
| gates.log | f75e1f1685fc9346f4d07d681e0af2500a79c6fbad65e8af4162c5c39ff5e908 |
| perm/00430b90.c (one-variable form, permuter output) | 3aac53fae431e24f745b779b1fcf9ec338e4f982a7862832308ba41643229c68 |
| perm/perm.log | 9b05e56f64755295b2359628266ebc1e0978bf0af3e5cd7f02b4575634f30008 |
