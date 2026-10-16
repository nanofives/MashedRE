# RESULT fwd494fd0 (2026-10-16)

Branch `work/fwd494fd0` from d074095d, worktree `.worktrees/fwd494fd0`. Three parts: **A** fix and re-earn of `0x00495080`,
**B** the inline-asm lint, **C** the AI dual-copy burn-down (ROADMAP D4 item 2, section at the end).

## A. 0x00495080 Fwd494fd0_DoublePlusHalf: fixed, re-earned, C2 -> C3

**Fix** (`Render/PromoLoop_round256.cpp`): `mov esi, dword ptr ds:[771A0Ch]`. Built export read back
(`verify/dupinstall2_20261016/disasm_asi_export.py`): `3E 8B 35 0C 1A 77 00  mov esi, dword ptr ds:[0x771a0c]` (the unfixed
build had `BE 0C 1A 77 00  mov esi, 0x771a0c`). The source comments that called the load a "dead load" were wrong
(`FUN_00494fd0` reads ESI: `mov eax,[esi]` at `0x00494fee`, `call [eax+0x1c]` at `0x00494ff1`) and are corrected.

**Register-input check in path1** (the harness allowed it as an additive, defaulted config key): `stub_esi` in
`re/frida/diff_template.js` `stub_dispatch_observe` replaces each `stub_at` entry with a 6-byte thunk
`mov [slot], esi ; jmp <recorder>` and appends `esi=0x...` to the recorded call; `stub_esi_seed {addr, val}` writes a
sentinel (`0xD1D0C0DE`) to the global the register is loaded from for the run (restored after). Forwarded in
`run_diff.py` `build_config` (the whitelist), turned on for `input_axis_scale_bias_apply`, `ARG_TYPES.md` regenerated.

| step | result | file |
|---|---|---|
| path1 on the UNFIXED build, hook bypassed (negative control) | **RED 8/8**: `orig esi=0xd1d0c0de`, `reimpl esi=0x771a0c`, every stack argument equal | `path1_before_fix.txt` |
| path1 on the fixed build | **GREEN 8/8** (same vectors, ESI compared) | `path1_after_fix.txt`, `path1_after_fix.csv` |
| path2 `run_verify_hook` | **PASS**: E9 + rel32 to the export, 2/2 entries | `win1/path2_input_axis_scale_bias_apply.txt` |
| nav boot to a race, FULL hook set, no `MASHED_HOOK_SKIP` | `drive_to_race=True`, process alive (`poll()=None`), no detach | `win1/nav_probe_full_hookset.txt` |
| full hook set boot, 45 s | 1290 of 1331 hooks installed, `0x00495080 Fwd494fd0_DoublePlusHalf` among them, survived | `win1/install_all/`, `win1/install_observe_all.txt` |
| the three race rows, path2 without the skip | `table_773030_get` 2/2, `plugin_data_dword_a` 1/1, `plugin_data_dword_b` 1/1, all PASS | `win1/path2_*.txt` |

Row `0x00495080` C2 -> C3 via the hooks.csv + CHANGELOG transaction (`tracker.py`). Not C4: no canonical-scenario diff of the
body with the JMP live was run (the function is reached from `FUN_0043dfd0`; not attempted here).
`KNOWN_ISSUES.md` `AUTOHOOK-INLINE-ASM-IMMEDIATE` is closed (FIXED block, guard, workaround retired).
The exe is unaffected: `Render/PromoLoop_round256.cpp` is an asi-only TU, `build.bat` gained only the lint call, so G-CTRL
was not re-run.

## B. scripts/lint_inline_asm_abs.py

Fails the build on a bare numeric absolute memory operand (`[0x771A0C]`, `[771A0Ch]`) in inline asm without a segment override.
- Scope: inside an `__asm` block or on a single-line `__asm`, and in `.inc` files, but only on lines that start with an x86
  mnemonic (after an optional label). That condition matters: `Vehicle/CarMaterialTables_dirt.inc` is a C++ data table
  (`body[0x1e] = 0`), the first draft flagged 13 lines of it.
- `--selftest` (12 positive/negative samples) runs on every invocation, so a broken regex fails loudly instead of passing everything.
- Wired into `mashedmod/build.bat` right after the rva lint (`MASHED_SKIP_ASM_LINT=1` skips).
- On the UNFIXED tree it reports exactly one line, `PromoLoop_round256.cpp:172`, and nothing else; on the fixed tree: OK over
  621 source files. The fix build ran through it (`build_fix.log`).
