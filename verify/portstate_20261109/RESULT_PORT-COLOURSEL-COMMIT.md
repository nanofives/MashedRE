# RESULT PORT-COLOURSEL-COMMIT (2026-11-10, U-9672)

PREREG: `PREREG_COLOURSEL_COMMIT.md` (committed alone, 37c1bf19a). Branch `orch/PORT-COLOURSEL-COMMIT` = `orch/integration`
1be11e86d + `orch/PORT-COLOURSEL-LEFTRIGHT` d788bd115 (merge c53e1e53e). Code and drivers d67d18ee4.

## 1. Answer

**GREEN.** With `MASHED_COLOURSEL_ICON=1`, picking colour N on the cup-flow colour select now gives the race the
original's livery for every N tested (1, 3, 6, and 6-then-left = 5). The confirm commits DAT_007f1a1c = choice - 1, as
0x0043e56c..0x0043e57e does, and the race reads slot 0's livery from DAT_007f1a1c, as FUN_0040d110 does. With ICON OFF
the old law stays (livery from the cursor 0x0067ea98), and the default gates are unchanged both ways.

This closes the livery half of the ICON blocker named in RESULT_PORT-COLOURSEL-LEFTRIGHT section 7. The ICON flip
(default ON) is a decision for L0. Two smaller parts of the same original arm are still unported (section 6).

## 2. Static reading

Bytes of `original/MASHED.exe.unpatched` (`re/tools/disasm_va.py`) and `re/tools/decomp_pc.py 0x0043dfd0`
(decomp lines 383..391): in the 0xff1d0000 arm (0x0043e557), when FUN_00430760 returns non-zero (game mode 2, 10, 3, 4
or 5, 0x00430765..0x00430781), the arm does `if (DAT_0067f0c0 == 0) DAT_007f1a1c = 0; else DAT_007f1a1c =
DAT_0067f0c0 - 1;` (0x0043e56c `mov eax,[0x0067f0c0]`, 0x0043e571 `cmp eax,ebx`, 0x0043e575 store 0, 0x0043e57d `dec
eax`, 0x0043e57e store). Then it calls FUN_0043d2a0(6,0) (0x0043e586) and FUN_00431d00 (0x0043e58e), and sets
DAT_0067ea78 = 0 (0x0043e593). The race livery of slot i is DAT_007f1a1c[i*4] (FUN_0040d110, 0x0040d199..0x0040d1a6).

## 3. Change (d67d18ee4, behind `MASHED_COLOURSEL_ICON`, default OFF, no new knob)

- `Frontend/MenuNavSM.cpp` ApplyActionGameMode (which runs before the nav dispatch): with ICON on, action 0xff1d0000
  and Q_ModeIsCupFlow() (FUN_00430760), DAT_007f1a1c = choice == 0 ? 0 : choice - 1.
- `exe_main.cpp` race launch: with ICON on and the launch mode in FUN_00430760's set, `cfg.cars[0].colour` =
  DAT_007f1a1c (range guard 0..5, the port's colour table bound). Otherwise it is unchanged (cursor 0x0067ea98).

Build `ea031d01` (`MASHED_NO_DEPLOY=1`): Build OK, rva-lint `NEW=0`.

## 4. Behaviour

Arm 1, original (`coloursel/run_commit_orig.sh`, `menu_draw_burst.py --nav`, root `D:/MashedWT/PORT-COLOURSEL-COMMIT_root`).
Nav: confirm x3 (cup-flow colour select, mode 3), the moves, confirm (0xff1d0000), challenge confirm, then 20 s of wait.
Peeked after the nav, with session phase 0x00771968 = 3 (race running) in every run:

| N | moves | 0x0067f0c0 | 0x007f1a1c (slot 0) | 0x007f1a2c (slot 1) |
|---|---|---|---|---|
| 1 | none | 1 | **0** | 1 |
| 3 | right x2 | 3 | **2** | 0 |
| 6 | right x7 (clamp) | 6 | **5** | 0 |
| 5 | right x6, left | 5 | **4** | 0 |

Arms 2/3, exe (`coloursel/run_commit_exe.sh`, `commit_exe_poll.py`: MASHED_GOTO=1, MASHED_NAV_KEYS e,e + the same moves +
e,e, then a 20 s pad so the race loads). The slot 0 livery is from the `[librwrest] livery:` line in mashed_re.log:

| N | ICON ON: 0x007f1a1c / slot 0 livery | ICON OFF: 0x007f1a1c / slot 0 livery | original |
|---|---|---|---|
| 1 | 0 / **0** ADVANTAGE0.DFF | | 0 |
| 3 | 2 / **2** ADVANTAGE2.DFF | 0 / 1 ADVANTAGE1.DFF | 2 |
| 6 | 5 / **5** ADVANTAGE5.DFF | 0 / 0 ADVANTAGE0.DFF (cursor wrapped to 0) | 5 |
| 5 | 4 / **4** ADVANTAGE4.DFF | | 4 |

ICON ON equals the original for all four N. ICON OFF shows the mismatch this step closes: N = 3 gives livery 1 and
N = 6 gives 0. The exe-side depth word 0x0067e9f8 reads 0 throughout, because the exe keeps its depth elsewhere. The
exploratory `orig_c3` / `orig_c3r` probes (PREREG section 4) agree with arm 1, N = 3.

## 5. Gates (`run_arms.py`, A4_EXE = the `ea031d01` copy, private root)

| arm | hashes |
|---|---|
| H_DEF / H_CSIX (ICON + BORDER) | `8284be46 / 67931834 / 31fe043e` both |
| B_DEF_K0 / K1 / K26 | `1d5bf4f9` x3 |
| B_CSIX_K0 | `1d5bf4f9` |

12-capture check not run. No default path is touched, because both changes are gated on `ColourSelIcon_On()`.

## 6. What is not ported (same arm, outside this step)

- FUN_00431d00 (0x0043e58e) and DAT_0067ea78 = 0 (0x0043e593) after the commit.
- Opponent colours. The challenge-launch arms read DAT_007f1a1c to choose colours for slots 1..3 (reads at 0x0043eb76,
  0x0043ec98, 0x0043ee26, 0x0043ef7a, 0x0043f0ab, with the loops starting `mov ebp,0x7f1a2c` at 0x0043eb4a etc.).
  Decomp lines 527..535: an opponent whose colour equals the player's takes the first colour 0..5 that is free. The exe
  never writes cars[1..3].colour, so those slots stay 0. In the original's N = 1 run, slot 1 is 1. In the exe run (ICON ON,
  N = 1) slot 1's livery is 0. This gap is the same with ICON OFF, so it is not an ICON regression, but it still makes the
  original and the exe differ. Next row: port those arms' slot-colour writes.
- The non-cup flows (FUN_00430760 == 0) still use the 0x0042b9e0 team branch, which was not ported (MenuNavSM 0xff1d0000 residual).

## 7. Commits

| commit | content |
|---|---|
| 37c1bf19a | PREREG (alone) |
| d67d18ee4 | MenuNavSM.cpp + exe_main.cpp, `run_commit_orig.sh`, `run_commit_exe.sh`, `commit_exe_poll.py` |
| this commit | this RESULT |

## 8. Raw files (`D:/MashedWT/PORT-COLOURSEL-COMMIT_data`, not committed)

| file | sha256 |
|---|---|
| exe/mashed_re.exe | ea031d01cf4956458dc0d9348571d32e07a1ed7ea932713d8c8067742620ca2b |
| build.log | de460d78105df70ba9aa2387ff6c8c3812a926cc9e5813e9bccefffea30662bc |
| decomp_0043dfd0.txt | 6263f810aa539f4717de5fd50d6ddfec2e2eec001791ad8cdb5d1d94c5ce2c66 |
| orig_c3.peek.json / orig_c3r.peek.json | f067f4b58c2cea1a1946905de8b131a0d05ddc56694c34bc33371b0b1b1f9783 / 657bd24b5a6be1f98e0445b2fb2a3f9f5cf344426271051a7001e0adc7784d8a |
| race_n1 / n3 / n6 / n5l .peek.json | 36e18bd3893a01f4320e28a67fe50d26d6580b54441ad40f4251ae95ccb46f50 / b725d1c5e789f1664917616f5d6cb1634fd29921d309f6eb696adb02bb008171 / e0e876b5aae90265770248e918cf8395651585bbc90eb52ccb8dc2be559bad0f / 992282195ac0983c33a6d1e0d83ce7a77c9c72caf098ff7daa3cff90fe8728e1 |
| race_n1 / n3 / n6 / n5l .log | e0fe6a93fcddbf8e9e6e3a28d7c76effbdc577116038c5e8929f66f3a1849d7e / 7b4022c557fc2d9452b7ccba5318ab830375c473d67cde5b508086cf899be09b / 704f28508b1e6c540fccbf397326521306d327ea6f8cb0e783992a5b67800cd2 / 8a834330bb42412225eb6097fd85746e78d699078c632c7d9deadd30101d2486 |
| exe_on_n1 / n3 / n6 / n5l .json | a811e9e048d0c680d17464f74ad500313f0c7b390bc71354b8edd731be7f9af2 / 7d094d73f1a456f036da2f65354f94715b1cb4f22212eb66473a16cccd054d31 / adfcef52e14193e6e9d30006df4be2d85a25757175259efbb5bc333ccd42e084 / b7a67ff272df13aa138bcdc8e25deed6be106a83d530e20379fc45c91be30ec2 |
| exe_off_n3 / n6 .json | 8bc9a36c5df350c64915af36af09b4879decfa4ea6ac4273a7f7bcd492157eb9 / 2ffd5bdc805f82e441003a486b5cb34ff56348d395b789204c3cd8bd5182418b |
| exe_probe_on_n3.json | 1205593f4beed69a8c1c826c0e4dca052fd61440c09d08e896d2c4c6fccf01ae |
| gates.log | 5400380d0da9a7ef05fad2d27badc0ad896b8aa251f347c048fb2afcdbb09091 |
| arms/H_DEF.step.csv = H_CSIX.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv / ptrace.log | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c / 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv = B_CSIX_K0.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
