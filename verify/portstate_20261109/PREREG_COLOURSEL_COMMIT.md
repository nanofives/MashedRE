# PREREG PORT-COLOURSEL-COMMIT (2026-11-10, U-9672)

Branch `orch/PORT-COLOURSEL-COMMIT` = `orch/integration` 1be11e86d + `orch/PORT-COLOURSEL-LEFTRIGHT` d788bd115 (the ICON
left/right law on 0x0067f0c0). This PREREG is committed alone, before the code and the exe arms.

Disclosure: two exploratory original-side probes ran before this file was committed (`orig_c3`, `orig_c3r`, section 4),
and arm 1 was started while this file was written. They read the reference only. No exe arm ran before this commit.

## 1. Static reading (bytes of `original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`; decomp `re/tools/decomp_pc.py 0x0043dfd0`)

The colour-select forward action 0xff1d0000 in FUN_0043dfd0:

| address | instruction | effect |
|---|---|---|
| 0x0043e557 | `cmp edi,0xff1d0000` / `jne 0x0043fc37` | this arm |
| 0x0043e563 | `call 0x00430760` | 1 iff DAT_0067e9fc is 2, 10, 3, 4 or 5 (0x00430765..0x00430781) |
| 0x0043e56a | `je 0x0043e59e` | 0: the FUN_0042b9e0 team branch (not this step) |
| 0x0043e56c | `mov eax,[0x0067f0c0]` | the choice |
| 0x0043e571 | `cmp eax,ebx` / `jne 0x0043e57d` | decomp: `if (DAT_0067f0c0 == 0)`, so ebx = 0 here |
| 0x0043e575 | `mov [0x007f1a1c],ebx` | choice 0: DAT_007f1a1c = 0 |
| 0x0043e57d / 0x0043e57e | `dec eax` / `mov [0x007f1a1c],eax` | else DAT_007f1a1c = choice - 1 |
| 0x0043e583..0x0043e586 | `push ebx; push 6; call 0x0043d2a0` | FUN_0043d2a0(6,0), the challenge select |
| 0x0043e58e | `call 0x00431d00` | not ported by this step |
| 0x0043e593 | `mov [0x0067ea78],ebx` | DAT_0067ea78 = 0, not ported by this step |

The race livery of slot i is DAT_007f1a1c[i*4] (FUN_0040d110, 0x0040d199..0x0040d1a6, already cited at
`TrackRenderer.cpp` ResolveCarLiveries). The exe's launch writes `cfg.cars[0].colour` from the cursor 0x0067ea98
(`exe_main.cpp`, colcur), which the original's colour select does not commit.

## 2. Change (behind `MASHED_COLOURSEL_ICON`, default OFF, no new knob)

- `Frontend/MenuNavSM.cpp` ApplyActionGameMode (runs before the nav dispatch, as the original's store runs before the
  0x0043e586 push): ICON on, action 0xff1d0000, Q_ModeIsCupFlow() (FUN_00430760) -> DAT_007f1a1c = choice == 0 ? 0 :
  choice - 1.
- `exe_main.cpp` race launch: ICON on and the launch mode in FUN_00430760's set -> `cfg.cars[0].colour` =
  DAT_007f1a1c (range guard 0..5, the port's colour table bound). Otherwise unchanged.

## 3. Arms and predictions

| arm | what | prediction |
|---|---|---|
| 1 original | `coloursel/run_commit_orig.sh`: `menu_draw_burst.py --nav` to the cup-flow colour select, choice N = 1, 3, 6 and 6-then-left (5), confirm, challenge confirm, wait 20 s; peek 0x007f1a1c, slots +0x10, 0x0067f0c0, 0x0067ea98, depth, mode, phase 0x00771968 | 0x007f1a1c = N - 1 after the confirm and still N - 1 at the end |
| 2 exe ICON ON | MASHED_GOTO=1 + MASHED_NAV_KEYS to the colour select, the same N, confirm, challenge confirm; read `[librwrest] livery: slot 0 ... colour=` and `player voice bank: colour=` in mashed_re.log, poll 0x007f1a1c | slot 0 colour = N - 1, equal to arm 1 |
| 3 exe ICON OFF | the same keys | slot 0 colour from 0x0067ea98 (the old law), 0x007f1a1c not written by the colour confirm |
| 4 gates | `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` + the ICON stack (H and B_K0) | G-CTRL 8284be46 / 67931834 / 31fe043e, B 1d5bf4f9 x3, both ways |
| 5 captures | 12-capture nav demo, OFF / ON / OFF / ON | OFF captures identical to the default. No default path is touched, so this arm is a check, not a requirement |

GREEN = arm 2 equals arm 1 for every N, arm 3 shows the old law, arm 4 unchanged both ways.

## 4. Exploratory probes (before this commit)

`orig_c3`: nav confirm x3, right x2, confirm -> 0x007f1a1c = 2, 0x0067f0c0 = 3, depth 5, mode 3. `orig_c3r`: the same plus
the challenge confirm and 25 s -> 0x007f1a1c = 2 at the end, depth 5, 0 frontend draws in the burst.
