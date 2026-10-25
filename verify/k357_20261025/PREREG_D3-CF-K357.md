# PREREG D3-CF-K357: the sub-state-7 exit gate (FUN_00410860), knob `MASHED_R7_EXIT` (2026-10-25)

Branch `orch/D3-CF-K357` from `orch/integration` `b5c84688e`, with `orch/D3-CF-U9779` @ `45dccb4f5` merged (`181af712f`: the
four u9779 knobs, all default OFF). Committed alone, before every knob arm. Evidence `D:\MashedWT\D3-CF-K357_data`. Private root
`D:\MashedWT\D3-CF-K357_root`. Original witness reused: `o_r7_1` (`D:\MashedWT\D3-CF-U9779_data\o_r7_1.*`, u9779 B recipe +
`--r7phys-probe`).

## 1. Static reading (MASHED.exe.unpatched, capstone + headless decomp, read-only pool slot)

Writers of `DAT_0063ba8c` (capstone over the whole image): 1 `0x0040d3e7` / `0x0040de1a`, 2 `0x0040dda3` / `0x004100dd`, 3
`0x00410279`, 4 `0x00410287` / `0x004102ac` / `0x00410387`, 5 `0x0040dbf5..0x0040dc40`, 6 `0x004104e3`, 7 `0x004111a6`, 8
`0x00410a6e`, 9 `0x00410a5e` / `0x00410b02`, 0xa `0x0040ddf5`, 0xb `0x00410645`, and the setter FUN_0040e360 (`0x0040e364`).
The only writers inside the sub-state-7 handler are 8 and 9.

FUN_004111c0 case 7 calls FUN_00410860, then FUN_0040fc00. FUN_00410860 (`0x00410860..0x00410aaf`):

| step | what the decompilation shows |
|---|---|
| alive count | n = number of slots v in 0..3 with `*(PTR_PTR_005f2770 + 0x34 + 4v) != 0` and FUN_0046c7b0(v) == 1 |
| counter | `DAT_005f29b8 = (n != 0 ? DAT_005f29b8 : 400) + 2` (`0x004108fd..0x00410910`) |
| gate | true when, for any v in 0..3, `DAT_007f1a14[v*4]` (stride 0x10) != -1 and the byte `0x007f1042 + idx * 0x4c` != 0, or when `DAT_007f0fd0 != 1` and n == 0 |
| exit | only when `DAT_005f29b8 > 199` (`199 < DAT_005f29b8`) and the gate: points check (FUN_0040b6d0), then `DAT_0063ba8c = 9` (`0x00410a5e`, match over) or `= 8` (`0x00410a6e`, next round: case 8 runs FUN_00414220 / FUN_00426c70 / FUN_0045bed0 and sets 2) |

`DAT_005f29b8` writers: `0x0040ff03` (0xff), `0x004100d3` (100000), `0x00410291` / `0x00410391` (12000), `0x0041036a` /
`0x004103f0` (decrements in states 3 / 5), `0x004104c0` (0, the state-5 tail that sets 6), `0x00410910` (the counter above). None
in state 6, so the counter is 0 when 7 starts and it needs 100 sub-state-7 ticks before the exit can fire.

`0x007f1042` is byte 0xa of the ctrl block `0x007f1038 + idx * 0x4c` (the block base, `Ai/AiState.h` kCtrlBlockBase).

The port: `exe_main.cpp` (R6 match flow, "hold ~3s on the result") calls `NextRoundOrEnd()` -> `StartRound()` when
`t - s_round_end_t > 3.0f`. That 3.0 s is port-side; there is no timer of that kind in FUN_00410860. It is the k = 357 restart (137
frames after the k = 220 round end in u9779's `B_U7PR3_K0`). The port never writes byte 0xa of any ctrl block (the keyboard writer
`WriteMashedInputGlobalsFromKeyboard` writes `0x007f1044 + col`, cols 0..5; `AiController.cpp` writes bytes 0..7).

`o_r7_1` (original, no input): 502 sub-state-7 frames to the end of the capture, clock `0x007f0ff4` 52000, car 3 frozen. With no
input the gate stays false, as the static reading predicts.

## 2. Knob `MASHED_R7_EXIT` (default OFF)

Acts only while u9779's flag `s_roundEnd7` is up (`MASHED_ROUND_END7`). It replaces the port's 3.0 s hold with FUN_00410860's
gate: a counter zeroed when the flag rises (`0x004104c0` zeroed it at the state-5 tail, state 6 never writes it), ticked once per
race frame after the round-end frame as `0x004108fd..0x00410910` does, and the exit taken only when counter > 199 and the gate
reads true from the mapped `0x007f1a14` / `0x007f1042` / `0x007f0fd0`. On exit the port keeps its own `NextRoundOrEnd()`.
Knob OFF: the 3.0 s hold, unchanged.

[UNCERTAIN] the exit branch itself (frame alignment of states 8 -> 2 -> 3/4 against the port's single StartRound, and which
device input writes byte 0xa) is not exercised by the B recipe, which has no input. Out of scope here; the arms below test the
no-input branch only.

## 3. Arms (copy of `verify/u9779_20261024/run_arms.py`; stacks `X` = the knob alone, `U7PX` = U7P + the knob, `*R3` = car 3 records)

| id | arm | prediction |
|---|---|---|
| P0 | `H_DEF`, `B_DEF_K0 / K1 / K26` | `bacfc747 / 8dd1c004 / 6f9932b6`, `f9deb9b9 / d3601fcc / 9da9a26a` (default unchanged) |
| P0b | `B_X_K0` | `f9deb9b9` (inert without `MASHED_ROUND_END7`) |
| P0c | `B_U7P_K0` | `c37d4c0c` (the merged U9779 code reproduces) |
| P1 | `B_U7PXR3_K0` log | one `R6 ROUND END` and no `round2_go` / second `RULE-EVAL` round in 1200 frames |
| P2 | `B_U7PXR3_K0` vs `o_r7_1` (`reck.py`) | no first difference on (+0x958, +0x960, +0x9b0, +0x9b8) for any k the original has (was k = 357); +0x958 / +0x960 = 6.0739159 / -22.0904903 on every k from 220 to the original's last record |
| report | `B_U7PX_K0 / K1 / K26`, `H_U7PX` | hashes reported, no prediction |
| report | original `o_k357_poke`: `o_r7_1` recipe + `--poke-u32 0x007f1040=0x00010000 --poke-delay 20` | report only: whether a one-shot write of byte `0x007f1042` = 1 inside sub-state 7 makes the original leave 7 (car 3 regrid). A miss does not refute P1 / P2 (the input stage may rewrite the byte first) |

P1 or P2 failing = RED, knob not offered for a flip.
