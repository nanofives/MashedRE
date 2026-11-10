# RESULT PORT-COLOURSEL-OPPCOL (2026-11-10, U-9672)

PREREG: `PREREG_COLOURSEL_OPPCOL.md` (committed alone, 0ca68c460). Branch `orch/PORT-COLOURSEL-OPPCOL` =
`orch/PORT-COLOURSEL-COMMIT` 502277adf + this step. Code and drivers f248a0ebc.

## 1. Answer

**GREEN.** With `MASHED_COLOURSEL_ICON=1`, the challenge launch now gives the opponents the original's colours.
For N = 1, 3, 6 and 5 (6-then-left), slot 1's colour, and so its livery, equals the original's: 1, 0, 0, 0. The colour
confirm now also runs FUN_00431d00 and DAT_0067ea78 = 0, and the eight globals it writes equal the original's on the
challenge select for N = 1 and 3. Both changes sit inside `ColourSelIcon_On()`. ICON OFF is unchanged, and the
default gates are unchanged with ICON OFF and ON.

Default path (ICON OFF): it is wrong today for slot 1 when the player's colour equals the table colour. N = 1 gives
slot 1 colour 0 where the original gives 1. No default-path change ships here, because the ICON-OFF player colour is
itself not the original's (it comes from the cursor 0x0067ea98, RESULT_PORT-COLOURSEL-COMMIT section 4), and the
opponent law reads the player colour. The ICON flip fixes both at once.

Nothing found here blocks the ICON flip. What stays unported in the same arms is listed in section 6.

## 2. Static reading

Bytes of `original/MASHED.exe.unpatched` (`re/tools/disasm_va.py`), decomp `re/tools/decomp_pc.py` 0x00413f90,
0x00431d00, 0x00430670, xrefs of 0x005f2b10 and the eight globals.

- Colour confirm tail (0xff1d0000, cup flow): `call 0x00431d00` (0x0043e58e), `mov [0x0067ea78],ebx` with ebx = 0
  (0x0043e593). FUN_00431d00 = the C4-by-match `re/match/s07/00431b80.c`, exe body `CarSelectReset`
  (`Match/MatchS07_00431d00.cpp`), which no exe path called before this step.
- Challenge launch (0xff260000, DAT_0067e9fc 3/4/5): FUN_00413f90 returns &DAT_005f2b10 (0x00413f90). The only xref of
  0x005f2b10 is FUN_00413f90. Its callers are FUN_00430670, FUN_0043d7c0 and FUN_0043dfd0. FUN_00430670 only reads the
  table. Entry = DAT_005f2b10 + DAT_0067f17c*0x30 (+0x10 mode 4, +0x20 mode 5) = {count, c1, c2, c3}.
- Five event arms run the same loop: loop start / player-colour read / colour store = 0x0043eb4a / 0x0043eb76 /
  0x0043eba4 (0x80000), 0x0043ec6d / 0x0043ec98 / 0x0043eccc (0x50000), 0x0043edfa / 0x0043ee26 / 0x0043ee60
  (0x20000, 0x30000, 0xb0000, 0x60000), 0x0043ef4f / 0x0043ef7a / 0x0043efac (0x00000, 0x10000, 0xc0000), 0x0043f080 /
  0x0043f0ab / 0x0043f0dc (0x90000, 0xa0000). Events 0x70000 and 0x40000 run no loop. Law (0x0043eb74..0x0043eba4):
  c = c_i; if c == DAT_007f1a1c, the first k in 0..5 with k != DAT_007f1a1c, c1, c2, c3 replaces it. Stored at
  0x007f1a2c + (i-1)*0x10.
- Table DAT_005f2b10 (156 dwords, `.data`, file-backed), baked into `exe_main.cpp` kOppColourTable. `table_check.txt`:
  baked 156 = binary 156, equal. Row 0 (the reachable challenge): mode 3 `1,0,-1,-1`, mode 4 `3,2,3,4`, mode 5 `2,5,1,2`.

## 3. Change (f248a0ebc, behind `MASHED_COLOURSEL_ICON`, default OFF, no new knob)

- `Frontend/MenuNavSM.cpp` ApplyActionGameMode, in the ICON + 0xff1d0000 + Q_ModeIsCupFlow block, after the
  DAT_007f1a1c store: `CarSelectReset()` (0x0043e58e), then DAT_0067ea78 = 0 (0x0043e593).
- `exe_main.cpp` `ChallengeLaunchOpponentColours(mode, row)`: event type from `RaceModes::EventType` (the baked
  DAT_005f65c8), no loop for 0x7 / 0x4 / off-table. Otherwise the loop law on the baked entry stores 0x007f1a2c + (i-1)*0x10
  for i = 1..count. MenuLaunchRace, with ICON on and launch mode 3/4/5, calls it with row = Nav_ChallengeRow() (the exe's
  DAT_0067f17c), then copies slot i's colour to `cfg.cars[i].colour` (range guard 0..5, as slot 0). Slots past count
  keep 0, as before.

Build d1928307 (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint `NEW=0`, asm-lint OK.

## 4. Behaviour

Arm 1, original (reused, PORT-COLOURSEL-COMMIT `race_n*.peek.json`, race running) and arm 2, exe ICON ON
(`run_oppcol.py exe`, `oppcol_exe_poll.py`, `[librwrest] livery:` lines):

| N | original 0x007f1a1c / 0x007f1a2c | exe ICON ON slot 0 / slot 1 livery | exe 0x007f1a2c | exe ICON OFF slot 0 / slot 1 |
|---|---|---|---|---|
| 1 | 0 / **1** | 0 ADVANTAGE0 / **1** ADVANTAGE1 | 1 | 0 / 0 |
| 3 | 2 / **0** | 2 ADVANTAGE2 / **0** ADVANTAGE0 | 0 | 1 / 0 |
| 6 | 5 / **0** | 5 ADVANTAGE5 / **0** ADVANTAGE0 | 0 | |
| 5 | 4 / **0** | 4 ADVANTAGE4 / **0** ADVANTAGE0 | 0 | |

Arm 1b, original challenge select (`run_oppcol.py orig`, `menu_draw_burst.py --nav`, no launch), and arm 2's values
after the colour confirm (poll row at the 0x007f1a1c write):

| globals | ea74 | ea78 | ea90 | ea94 | ea98 | ea9c | eaa0 | eaac |
|---|---|---|---|---|---|---|---|---|
| original pre-confirm (N = 1, depth 4) | 1 | 0 | 1 | 0 | 4 | 2 | 3 | 0 |
| original N = 1 (cs1, depth 5) | 1 | 0 | 1 | 0 | 4 | 2 | 3 | 0 |
| exe ICON ON N = 1 | 1 | 0 | 1 | 0 | 4 | 2 | 3 | 0 |
| original N = 3 (cs3, depth 5) | 1 | 0 | 1 | 0 | **1** | 2 | **4** | 0 |
| exe ICON ON N = 3 | 1 | 0 | 1 | 0 | **1** | 2 | **4** | 0 |
| exe ICON ON N = 6 / N = 5 | 1 | 0 | 1 | 0 | 1 | 2 | 3 | 0 |
| exe ICON OFF N = 1 / N = 3 | 0 | 0 | 0 | 0 | cursor | 0 | 0 | 0 |

The PREREG's hand-run predictions (4/2/3 for N = 1, 1/2/4 for N = 3) hold on both sides. The original's pre-confirm peek
already holds the N = 1 values, because the original also runs FUN_00431d00 before this screen with DAT_007f1a1c = 0.
So N = 3 is the case that tells the confirm's own call apart. Before this step the exe held 0 in all eight on the
challenge select (COMMIT `exe_on_n1`, ea98 = 0). The original's race peeks of COMMIT agree: ea98 = 4 for N = 1, 1 for
N = 3, 6 and 5. Exe readers of these globals in a cup race: none. 0x0067ea90 is read only in mode 10
(TrackRenderer.cpp FUN_0042fe80), 0x0067ea94 only in mode 10, and 0x0067ea78 only by the menu sprite dispatch
(0x0042fd27).

## 5. Gates (`run_arms.py`, A4_EXE = the d1928307 copy, private root, `mashed_lock.py run --solo`)

| arm | hashes |
|---|---|
| H_DEF / H_CSIX (ICON + BORDER) | `8284be46 / 67931834 / 31fe043e` both |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSIX_K0 | `1d5bf4f9` |

All equal to `merge_train.json` gctrl.ref / b.ref. The first gate attempt (`--solo`) queued behind two merge-train
priority holds and was stopped before any game started (own PIDs only). This run used a plain `mashed_lock.py run`, as
PORT-COLOURSEL-COMMIT did.

12-capture check not run. No default path is touched, because both changes are inside `ColourSelIcon_On()`.

## 6. What is not ported (same arms, outside this step)

- The challenge launch's other stores: slot +0 (0x007f1a24 + ..., the `local_2c + 1 + i` index), slot +4 (1, or 0 in
  the 0x90000/0xa0000 arm), FUN_0040e480(i,2), DAT_0067ea64 / ea70 / ea74 / ea78 / ecdc, DAT_007f0fd0 (the exe derives
  its race rule separately, RaceModes). After the launch the original's DAT_0067ea74 is the event's value (0 for event
  0x9), while the exe keeps FUN_00431d00's 1. No exe race code reads it.
- Only row 0 / mode 3 (count 1) is reachable on the baseline save, so the count 2..3 rows and the modes 4/5 tiers are
  ported from the table and the loop bytes, but not run.
- The non-cup flows (FUN_00430760 == 0) still use the 0x0042b9e0 team branch, which is not ported (the MenuNavSM
  0xff1d0000 residual).

## 7. Commits

| commit | content |
|---|---|
| 0ca68c460 | PREREG (alone) |
| f248a0ebc | MenuNavSM.cpp + exe_main.cpp, `coloursel/oppcol_exe_poll.py`, `coloursel/run_oppcol.py` |
| this commit | this RESULT |

## 8. Raw files (`D:/MashedWT/PORT-COLOURSEL-OPPCOL_data`, not committed)

| file | sha256 |
|---|---|
| exe/mashed_re.exe | d19283077abdbc93d38eedffe0416f8a2ca19f243c962c61a5ffbdc9eff45d26 |
| build.log | 178497b884f924adc5affeaba4e97b3b3acf30a5c25e9d71569ae9fa523adf9f |
| decomp_small.txt (0x00413f90, 0x00431d00, 0x00430790) | 3a64824891d56e590f2e308c76d46f0fda03c765a6b7e9e33026452582efdb4d |
| decomp_00430670.txt | 68fcc432f5a11da0b9e708fbab4bf48159395d779788272cc547e02cb0816a1a |
| table_check.txt | 475b7c9a3d8c13eb0c997221794e4c1225075e4ca7142b9af76a49981d20fd28 |
| orig_pre1 / cs1 / cs3 .peek.json | 7a66fe6b71fb667beb824b6b487a4bdcb70b2fa66b46d831bc78a4f2be97b596 / 9a432c792be7cb0d4ba69e0437b30419be367737b73245bea2d5dc5ae1f6bee2 / fc584662e73e42d4822092e1c4f50ce5535387c678e9fa36abab63578e03fa8c |
| orig_pre1 / cs1 / cs3 .log | 453ba475c5b0a95dd7f9e2a78e24ab9ff1b0f87273d7bf837d9cdddee0689c1c / 902fd0bbf11582afa7f05f21789d3aaa5e93fc6b415a580de03601a5b55a0cca / 43afb5a94a2d3108a6df4cb210cb87ee6d3ef4a63f5e7192b3f49cbe06a856c7 |
| exe_on_n1 / n3 / n6 / n5l .json | 45a21964286980d5fb7c9a904fb150c5c81a061e02b0901849993b8d8ddd83dd / 9cc6c5954cbb49689d9304e6bd15b7088d54d6462f42da436ab8d95cc43aa23d / 4cc3ba43a44afc11f136a0cacd239ee4dea16dd874ec56ed302c4ae297543b67 / 51123e77fca0c660071b85d3ba1cd8fb4deeeff1cf4d80f12e1f8c225bed4683 |
| exe_on_n1 / n3 / n6 / n5l .log | 3db5a1053195d4c9fd8ff4746b0110478f8f3909fd328213f6e2755a0694c35a / 0e7bd1debdcae095f57ee00c9893797b634089d120c56ccdf464ffaa02ef76b0 / 94a8a5d209409b5bf930ae308aba466a333590047c6cfaacd767034dae23ee90 / 19bf11624722d1b5f88499a8953ba3a6af26336ffe44a4e89ba8f7a417c5281d |
| exe_off_n1 / n3 .json | 0c34c46a4362d0eb074c071d48255f966435c75d3635310170854e9c3e2f5c82 / a58655e66acfa6c81ae13edd7466d289374b27f5d33fc5c690fdc55213964de2 |
| exe_off_n1 / n3 .log | a9733b0ebb0d99bb15e5c1f36c59f972cda3bc78b9301947a33db5ba84bbef0a / 9dd2fdbbda0ef1c93667b24982338f4d78092fb5c8722cad90bf1b6b67169cbb |
| run_orig.out / run_exe.out | 2127589826169a74b9dbfdafa5e04c31234381608f3ba6b062ddc849e76e265c / 1d2a3596465000f65c98c6227530f96d99c027e4a2b78e4378fc2beed29b7c81 |
| gates.log | 80ff39685313f29a50a4f5a62df7b2abaa386383b69119f12775b3a188dce99e |
| arms/H_DEF.step.csv = H_CSIX.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv / ptrace.log | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c / 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv = B_CSIX_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
