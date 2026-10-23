# PREREG: savelink2 (b) slice 1, RwStreamWrite 0x004cbe80 becomes one shared body (Render/RwStreamCore.cpp)

Written before any arm. Build: exe `e7fd1526`, dev .asi `5b14f29f` (work/savelink2, uncommitted tree on top of b574bee12).

## Change

- `RwStreamCoreWrite` is added to Render/RwStreamCore.cpp. It is linked in both rsp files and read from the disassembly
  0x004cbe80..0x004cc037 (jump table 0x004cc040).
- Save/RwStream.cpp (the asi-only C4 copy) is removed. Its export name `RwStreamWrite_s2` and the install at 0x004cbe80 move
  into RwStreamCore.cpp's .asi section.
- Defect in the removed copy, from the disassembly: every error path is `blk = {1, FUN_004d7ff0(code[, a])}` followed by
  `FUN_004d8480(&blk)` (0x004cbeb9..0x004cbed4, 0x004cbf15..0x004cbf35, 0x004cbf8c..0x004cbfad, 0x004cc014..0x004cc02c).
  The old copy called `FUN_004d8480()` with no argument, and it dropped the second argument `0x200` / `grow - cap` on the
  two allocator failures. Its non-error paths match the bytes.
- Exe: dead code (no exe caller). Its host thunks are a failing file write, `malloc`/`realloc` in place of the RwGlobals
  slots `+0x108`/`+0x110`, and the `Rwl_Err*` stubs. This is a declared deviation.

## Arms

- **W1 sweep.** Original + dev .asi, private root `D:/MashedWT/savelink_root`. `MASHED_LEAF_SELFTEST=1`,
  `MASHED_LEAF_SELFTEST_SWEEP=1`, `scenario_launch.py --hooks 0x004cbd30,0x004cc050,0x004cbe80 --cars 4 --hold 30`.
- **W2 live.** As W1 without the sweep, `--hold 60`.
- **G gates.** Exe `e7fd1526`, `run_arms.py H_DEF-1 B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **L lint.** `scripts/lint_rva_bodies.py`.

## Predictions

- **P1 (W1).** The SWEEP line reports cases = 116 (read/skip, unchanged from dcstream) + 66 (write) = 182, with noo 0 and
  ndiff 0. The write cases are 35 memory with room, 3 first-alloc, 12 grow, 8 unknown type and 8 callback. The error-slot
  comparison is included in every case.
- **P2 (W2).** The read/skip SUM lines show noo 0 / ndiff 0 as in dcstream t0b. Any write calls show noo 0 / ndiff 0 on
  the pure class. Zero live write calls is an allowed outcome. It means the live path is unexercised, and I will report it
  that way.
- **P3 (G).** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6 and B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.
- **P4 (L).** 0 NEW findings (8 allowlisted). Already run once on this tree: 8 / NEW 0.

## Decision rule

- All hold: commit. 0x004cbe80 keeps its C-level. The write body is now the shared one with a sweep A/B. A promotion or
  file move goes through re-classify only, and only the `file` column changes (Save/RwStream.cpp -> Render/RwStreamCore.cpp).
- P1 or P2 fails: report the first failing case. No commit of the code.
- P3 fails: layout sensitivity (memory sim-trajectory-is-code-layout-sensitive). Report it and stop for Mariano.
