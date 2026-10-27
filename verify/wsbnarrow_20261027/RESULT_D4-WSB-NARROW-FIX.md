# RESULT D4-WSB-NARROW-FIX

Verdict: **BLOCKED.** The fix is built and committed behind two default-OFF knobs, with the PREREG committed alone
before it, and the default gates are unchanged on this exe. But no probe arm could be scored: since 2026-10-26 22:28
no `MASHED.exe` on LIVING-PC reaches the main menu, with or without any `.asi`. Unblocking needs a machine-state
fix that only Mariano can make (see "The blocker"). After that, the arms re-run as written, with one command.

Branch `orch/D4-WSB-NARROW-FIX` off `orch/integration` @ `1158f5c1c`:
- `4e72462e6` PREREG alone (`verify/wsbnarrow_20261027/PREREG_D4-WSB-NARROW-FIX.md`)
- `4ecd2e09f` knobs + arm driver
- this RESULT (+ a driver fix: `sha()` tolerates an absent `.asi`, used by the stock boot below)

## What was wrong (static, before any run)

Read from `re/tools/disasm_fn.py` on `MASHED.exe.unpatched` with a symbolic x87 stack trace of every FSTP / FCOMP.
The full table with addresses is in the PREREG. In short:

1. **Print-order sums (the dcphys habit)** in 9 functions: `0x00577ec0` (8 sums), `0x005784a0` (5),
   `0x00578bd0`, `0x00578cb0`, `0x00578d90` (1 each), `0x00575b60` (3 rotate dots + a 6-term flip sum),
   `0x00578610` (2), `0x00575880` (3 rotate dots + a 6-term flip sum). The original adds the first two products it
   loads. Ghidra prints the third one first, so C++ groups the sum differently.
2. **Double precision in SSE TUs.** The race loop runs at x87 PC_24. `RwpSolverCore18`, `RwpSolverCore20` and
   `RwpSolverGlue5` are not in `mashedmod/x87_tus.txt`, so their `float10` (= `double`) expressions run in double:
   `0x0055c230`'s three rotate dots (already in the right association) and `0x00578610`'s depth tail.
   `0x005667c0` has the same problem, and the existing HB-6 knob `MASHED_LEAVES1_X87=1` already fixes it.
3. **Pair record `+0x0c` (`0x00575c60` / `0x00575fe0`)**, the `[UNCERTAIN]` row of the prior RESULT. It is resolved
   here statically. `0x0057644d` stores the integer sum `MOV ECX,[EAX+0x44]` (`0x00576444`) `+ EDX`. The port writes
   `(int)pfVar6[0x11] + param_6` (`RwpSolverCore17.cpp:685`), a float-to-int conversion of an integer word.
   `0x000100b6` read as float is a denormal, which converts to 0, and 0 is the value the probe logged for the port.
   The `0x00575c60` emit block's own `+0x0c` store is 0 in both (`0x00575f64`), and so is the other `0x00575fe0`
   site, `rec[3] += param_6 + 1` (`0x00576518..0x00576542`).
4. Checked and found already exact: `0x0055c0f0` and `0x0055c2d0` (Broadphase3, x87, disasm order), and the
   `0x00578bd0` non-dot tail (the dot stays on the FPU across the `0x0055c2d0` call, with the same rounding points
   as the port). So `0x005757d0`'s diffs, which the prior RESULT put on `0x0055c0f0`, come from `0x0055c230`'s
   double-precision dots (cause 2), not from `0x0055c0f0`.

`0x00578e50` has no arithmetic of its own (`0x00578610` + `0x005757d0`).

## The change (default OFF, not seeded in `Core/FaithDefaults.cpp`)

`mashedmod/src/mashed_re/Collision/NarrowFixKnobs.h`:
- `MASHED_NARROW_DOT_ORDER=1`: causes 1 and 2. Every site is commented with its FSTP / FCOMP address.
- `MASHED_NARROW_PAIR_INT=1`: cause 3.
- The probe arm adds the existing `MASHED_LEAVES1_X87=1`.
OFF = the previous code, unchanged.

## Gates (default exe, no knob): GREEN

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, exe `94b1e187`, private root
`D:\MashedWT\D4-WSB-NARROW-FIX_root`, inside the test slot and `mashed_lock.py run --label D4-WSB-NARROW-FIX`:

| arm | file | sha8 | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | step | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## Probe arms: NOT SCORED

Every boot exited at `game exited while waiting for menu (phase 1)`, 4 to 16 s in (one waited 375 s in the lock
queue first). None reached the race, so no SUM line exists for any row. These are not findings about the fix:

| boot | `.asi` in the root | result |
|---|---|---|
| FIX, 8 rows x 2 attempts (the run was stopped after row 3) | mine, `4fb372aa` | exit before menu, 1 hook installed |
| OLDASI, `00577ec0` x 2 | the prior step's `e42797f2`, which reached the race at 22:27 on 2026-10-26 | exit before menu |
| OLDASICL, `00577ec0` x 2 | `e42797f2` + `__COMPAT_LAYER="RunAsInvoker WIN98RTM HighDpiAware EmulateHeap"` | exit before menu |
| STOCK, `00577ec0` x 2 | `e42797f2` + `MASHED_RE_NO_AUTO_HOOK=1` (it did not stop the explicit HOOK_ONLY install) | exit before menu |
| NOASI, `00577ec0` x 2 | **no dev `.asi`** (only `mashed_qol.asi`, also present in the root that booted) | exit before menu |

The OFF control was not reached either. Under the PREREG's acceptance rules none of this is scorable. Nothing is
promoted or demoted.

## The blocker

- **Machine-wide.** The last scenario_launch boot to reach a race on this machine was 2026-10-26 22:27
  (`D4-WSB-C4-NARROW_data/launch_P_00575c60_a1_r2.txt`). The next boot (`0x00575fe0`, 22:28) hung for 27 min and
  was killed. Since then: no successful boot in any `D:\MashedWT\*_data`, my 20 boots failed, and another
  session's root (`D4-FU-MENUANIM`) logged an AV at `MASHED.exe+0x1baf48` at 01:32.
- My root differs from the root that booted yesterday only in `mashed_re_dev.asi` (sha256 compared on MASHED.exe,
  d3d9.dll, d3d9_real.dll, dinput8.dll, mashed_qol.asi, videocfg.bin, gamesave.bin), and the old `.asi` fails too.
- Matches `re/diag/KNOWN_ISSUES.md:246` ("game exited while waiting for menu (phase 1)" on every boot, even stock).
  PCA has added `$ DWM8And16BitMitigation` under **HKLM** Layers for this root and for `D4-WSB-C4-NARROW_root`
  (seen with `reg query`). The no-elevation `__COMPAT_LAYER` override did not help. Per that entry, the
  remaining causes are the GPU-driver wedge after many rapid spawns (cleared only by a reboot) or the HKLM entries
  (cleared only from an elevated shell). The standalone `mashed_re.exe` still runs (the gates above ran to 14400
  frames). `[UNCERTAIN]` which of the two it is: no dump was written and my boots left no Application-log entry.
- **Needs Mariano:** reboot LIVING-PC, or in an elevated PowerShell remove the HKLM Layers values for
  `D:\MashedWT\*_root\original\MASHED.exe`. Neither is something a worker should do.

## Re-run after the unblock (nothing else changes)

The root already has my `.asi` (`4fb372aa...`) back in place. From the worktree:

```
test-slot bash -c 'py -3.12 verify/wsbnarrow_20261027/run_c4n_fix.py --arm FIX --legs P --env MASHED_NARROW_DOT_ORDER=1 --env MASHED_NARROW_PAIR_INT=1 --env MASHED_LEAVES1_X87=1 && py -3.12 verify/wsbnarrow_20261027/run_c4n_fix.py --arm OFF --legs P --rvas 00577ec0,00575c60'
```

Score against the PREREG as written. Run one plain boot first to confirm the machine boots.

## Where the same blind spot affects BROAD and SOLV (for re-scoping D4-WSB-C4-BROAD / -SOLV)

The 8 RED rows passed C3 on `ShadowAB::Run`, which compares the return value only and does not roll back. I
classified every C3 row of the two bands by the wrapper its C3 rests on (static scan of the generated wrappers in
`mashedmod/src/mashed_re/**`, plus a crude scan of the port body for direct pointer stores; a write done only
through a callee is missed, for example `0x00578e50`):

- **BROAD `0x00558000-0x0055efff`**, 16 C3 rows:
  - return-only `Run` (blind to pointer writes, so the C4 probe must compare the write set):
    `0x0055a1f0` (direct pointer writes), `0x0055ab30`, `0x0055abb0`, `0x0055ae50`, `0x0055bae0`, `0x0055bb70`,
    `0x0055e050`, and `0x0055c230`, which writes through the `+0x18` slot and `0x0055c0f0` and carries cause 2 above
    (double-precision dots). Fixed here under the knob. It needs the write-set probe, not a return compare.
  - no generated wrapper at all (C3 evidence must be read from its own note): `0x00558100`, `0x00558140`,
    `0x0055c000` (ported), `0x0055c4f0`, `0x0055c810`, `0x0055deb0`, `0x0055dec0` (`new`).
  - write-compared (`RunTracked`): `0x0055c0f0` only.
  - Another SSE-double risk in the band: `RwpSolverGlue5.cpp` (`0x0055ab30`, `0x0055bb70`, `0x0055c230`,
    `0x0055e050`) builds SSE2, so any `float10` arithmetic in it runs in double, not PC_24.
- **SOLV `0x00566000-0x0056ffff`**, 30 C3 rows:
  - return-only `Run`: `0x00568990`, `0x0056b7a0`, `0x0056ba30` (direct pointer writes), `0x00568dd0`,
    `0x00568fd0`, `0x0056bb30`. `0x0056b7a0` is in `RwpSolverCore18.cpp` (SSE TU).
  - write-compared (`RunTracked` 20 rows, `RunRegion` 4 rows: `0x0056bce0`, `0x0056cf90`, `0x0056ed60`,
    `0x0056fad0`). These compare writes, but `RunRegion` compares only the region it is given (the same limit as
    the NARROW probe).
- So the C4 probe for those two steps needs the `NarrowC4Probe.cpp` write-set form (snapshot, original twice for
  the `noo` control, then the port) for the 14 `Run` rows and the 7 unwrapped rows, not the C3 shadow.
- Also check each three-term sum against the FADD order in the disassembly, and each `float10` expression in an SSE TU.

## Files

Committed: `PREREG_D4-WSB-NARROW-FIX.md`, this file, `run_c4n_fix.py`, `NarrowFixKnobs.h` and the five TU edits.

Raw (not committed), `D:\MashedWT\D4-WSB-NARROW-FIX_data\`:

| file | SHA-256 |
|---|---|
| `gates_run1.txt` | `07311329cd68b8f8759759ab36c01c719e79f88b6d072ec916efeada72095143` |
| `arms_run1.txt` | `83706881a352e072270f61c05283643296d36a62b39add8966f3029b7789748b` |
| `oldasi_run.txt` | `d404595fbf9f5b7d3f69ed33f521d3d919d20d42c37e799de6f4806017681bb7` |
| `oldasi_cl_run.txt` | `06624fe92549c5139326fd049fc6b2f05d92c7038b988155b7c9b9124e9fdf32` |
| `stock_run.txt` | `7b51faba37373f0722e4130feb4318ac1e154cf63d9bfba82f2208159deaee9c` |
| `noasi_run.txt` | `77139e55a85b45a9bff0369978769d234a14d14cc393b576be949566951c97ef` |
| `launch_FIX_P_005757d0_a1.txt` | `72a5ac121a5200189d683ae7cfe248420c2729ffc97604124878908819ceed25` |
| `launch_OLDASI_P_00577ec0_a1.txt` | `2e4e08a631b0127db30637520560bc132f5674763b1089e7b30f13c2516363a2` |
| `launch_OLDASICL_P_00577ec0_a1.txt` | `e9fc3a6093360c3c4429f07b749930bcf1c0b76bcfaf4155dbccc57978c81da2` |
| `launch_NOASI_P_00577ec0_a1.txt` | `3c5f1c890ccc67b2e341515d819bb0ec1726046deecc7d4a02993083242f6665` |
| `build1.log` | `35b489c777f550bad598fe496eba7eeed2b4f50a4fb56e5bda0d5fcae3b68466` |
| `mashed_re_dev_fix.asi` (= root `.asi`) | `4fb372aa718ab9131445da3ba7a8ec6c50940ca0f2bc5a17fed1b535e62c6702` |
| exe `mashedmod\build\mashed_re.exe` | `94b1e187a3b42f2635e29424bcf2854eab83a4fe203adbed7e51ff7af100d732` |

Gate arm outputs: `D:\MashedWT\D4-WSB-NARROW-FIX_data\arms\` (sha8 in the table above).
