# PREREG D4-WSF-SAVE-24EFC (2026-10-26): no port-only increment of gamesave 0x24efc

Committed alone, before any arm. Base `orch/integration` @ `637d58d28` (the KEEP_IMAGE flip branch is not needed: the
change is on the paths that the flip does not touch, and S2 / KEEP_IMAGE already leaves `0x24efc` as loaded).

## The original's write path (static, cited)

`re/tools/decomp_pc.py 0x00404ee0 0x00404f50 0x0040ad30 0x004098c6 0x00409c5e 0x00409e30 0x008a95ac --xrefs --callers`
plus `re/tools/disasm_fn.py` on `MASHED.exe.unpatched`:

- Serialize `FUN_00404ee0`: `MOV EAX,[0x008a95ac]` at `0x00404f03`, `MOV [0x00828254],EAX` at `0x00404f23` (file
  `0x24efc` = buffer `0x00803358` + `0x24efc`). It copies the live value. No arithmetic on it.
- Write `FUN_00404f50`: `FUN_004b3bb0("gamesave.bin", &DAT_00803358, 0x24fa0)` then `FUN_004099e0(0)`.
- Callers: Serialize at `0x00409da6` / `0x0040a05c` / `0x0040a89c`, SaveWrite at `0x00409db8` / `0x0040a082` / `0x0040a8c2`.
- Every reference to `0x008a95ac` in `.text` (byte scan for `ac 95 8a 00`, 9 hits = Ghidra's 9 xrefs):
  - reads `0x00404f03` (Serialize), `0x004099a0` (`FUN_004099a0` autosave guard), `0x0040ad20` (`FUN_0040ad20` getter)
  - writes `0x00404ec1` (Deserialize, `= [0x00828254]`), `0x004098c6` (`FUN_004098b0`, `xor eax,eax` at `0x004098bf`, = 0),
    `0x004098eb` (`FUN_004098d0`, `xor eax,eax` at `0x004098d0`, = 0), `0x00409c5e` (`MOV [..],0`), `0x00409e30`
    (`MOV [..],1`, after `MOV [0x008a9588],0x11` at `0x00409e26`), `0x0040ad34` (`FUN_0040ad30`, `= param_1`).
  - none is an `INC` / `ADD` on the dword.

## The port's increment

`mashedmod/src/mashed_re/Race/GameFlow.cpp` `SaveProgress`: `++g_saveCounter` at the in-place path (line 222,
`MASHED_SAVE_ORIGINAL=1` without KEEP_IMAGE) and at the default rebuild (line 231). `g_saveCounter` models
`DAT_008a95ac` (`GameFlow_SaveCounterPtr`). Fix: store `g_saveCounter` unchanged, as `0x00404f23` does. No knob: the
gates never save (D4-WSF-SAVE-RT P8), so the default gates cannot move. If they do, the change becomes a default-OFF knob.

## Inputs

`F_P` = `D:/MashedWT/savelink_data/arm_P.bin` (`0x24efc` = 0), `F_G` = `.../ghost_g1/gamesave_captured.bin` (0),
`F_1` = `F_P` with the dword at `0x24efc` set to 1 (1 is a value the original writes, `0x00409e30`), none (first run).

## Arms and predictions

| pred | arm | prediction |
|---|---|---|
| Q1 | W3 original, `F_1` -> `O_1` (savewrite probe, private root, no .asi) | report only: `O_1` `0x24efc` = 1 if no setter runs between load and write on the probe path, 0 if `0x004098b0` / `0x004098d0` / `0x00409c5e` runs. Either way the original's value is not `in + 1` = 2 |
| Q2 | SB base exe, `F_1` | `0x24efc` 1 -> 2 (the increment, measured) |
| Q3 | SF fixed exe default, `F_P`, `F_G`, `F_1` | `0x24efc` out == in (0, 0, 1) |
| Q4 | SF vs D4-WSF-SAVE-RT S1 (`S1_P`, `S1_G`) and vs SB_1 | exactly 4 bytes differ in the `0x24efc` dword, no other byte |
| Q5 | SF first run (none) | `0x24efc` = 0. The original's first run wrote 1 (`O_0`, D4-WSF-SAVE-RT P5), set by a writer the port does not model (`0x00409e30` or `0x0040ad34`, which one is `[UNCERTAIN: no --watch-write run]`). Reported as a residual, the old 1 was `++0`, not that writer |
| Q6 | SO fixed exe, `MASHED_SAVE_ORIGINAL=1`, `F_1` | `0x24efc` = 1 |
| Q7 | gates, fixed exe, no knob | G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (`merge_train.json`) |

Q3, Q4, Q6, Q7 all held = GREEN. Q1 is the original-side witness for a non-zero input. Raw files in
`D:/MashedWT/D4-WSF-SAVE-24EFC_data`.
