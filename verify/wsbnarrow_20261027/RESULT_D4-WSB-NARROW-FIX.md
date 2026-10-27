# RESULT D4-WSB-NARROW-FIX

Verdict: **RED, 6 of 8 rows GREEN** (run 13, 2026-10-27, after Mariano's reboot cleared the boot blocker).
With the knobs ON, `005757d0`, `00575880`, `00577ec0`, `005784a0`, `00578610` and `00578e50` write the same bytes as
the original on every call (diff 0, noo 0). `00575c60` and `00575fe0` are still RED, on 1.3% and 4.1% of calls (the
prior step measured 25% and 53%). The `+0x0c` pair-record defect is gone. Every residual diff is 1 ulp in face-record
field `+0x1c`, from a print-order dot in `0x00575fe0` that the PREREG does not name, so under the PREREG it is not
fixed in this step (see "Residual"). The OFF control still shows the defect on both rows, and the default gates are
unchanged on this exe.

Branch `orch/D4-WSB-NARROW-FIX` off `orch/integration` @ `1158f5c1c`:
- `4e72462e6` PREREG alone (`verify/wsbnarrow_20261027/PREREG_D4-WSB-NARROW-FIX.md`)
- `4ecd2e09f` knobs + arm driver
- `e9a0d8cea` RESULT, BLOCKED (+ a driver fix: `sha()` tolerates an absent `.asi`, used by the stock boot below)
- this RESULT update (run 13 scored), no code change

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

Not re-run for run 13: the exe is the same file (`94b1e187...`, re-hashed 2026-10-27), and the refs in
`re/orchestrator/merge_train.json` are still these six values (read 2026-10-27).

## Probe arms, run 13 (2026-10-27, after the reboot)

Exe and `.asi` unchanged since the BLOCKED run: root `.asi` `4fb372aa...` (= `mashedmod\build\mashed_re_dev.asi`),
`MASHED.exe.unpatched` `bdcae093...`. One boot per row, all inside the machine test slot and
`mashed_lock.py run --label D4-WSB-NARROW-FIX`. Every boot reached the race on attempt 1, with no crash and no piz
collision. Command (from the worktree):

```
proyectos_test_slot.py bash -c 'py -3.12 verify/wsbnarrow_20261027/run_c4n_fix.py --arm OFF --legs P --rvas 00577ec0,00575c60 --run _r13 && py -3.12 verify/wsbnarrow_20261027/run_c4n_fix.py --arm FIX --legs P --run _r13 --env MASHED_NARROW_DOT_ORDER=1 --env MASHED_NARROW_PAIR_INT=1 --env MASHED_LEAVES1_X87=1'
```

Every row below: `RACE_OK`, `A/B-IS-REAL`, `installed=1` (only its own `L2_C4N_<rva>`), `ret_diff 0`, `skip 0`.

| arm | row | calls = samples | ok | diff | noo | DIFF lines | verdict |
|---|---|---|---|---|---|---|---|
| OFF | `00577ec0` | 17665 | 14401 | **3264** | 0 | (cap) | control holds (diff > 0) |
| OFF | `00575c60` | 3585 | 3001 | **584** | 0 | 20, all `region 1+c` (pair `+0x0c`, `o=0000002a n=00000000` ...) | control holds (diff > 0) |
| FIX | `005757d0` | 2817 | 2817 | 0 | 0 | 0 | **GREEN** |
| FIX | `00575880` | 3393 | 3393 | 0 | 0 | 0 | **GREEN** |
| FIX | `00575c60` | 6273 | 6191 | 82 | 0 | 20 | RED |
| FIX | `00575fe0` | 1985 | 1904 | 81 | 0 | 20 | RED |
| FIX | `00577ec0` | 16257 | 16257 | 0 | 0 | 0 | **GREEN** |
| FIX | `005784a0` | 8641 | 8641 | 0 | 0 | 0 | **GREEN** |
| FIX | `00578610` | 2369 | 2369 | 0 | 0 | 0 | **GREEN** |
| FIX | `00578e50` | 4161 | 4161 | 0 | 0 | 0 | **GREEN** |

Prior step's rates (`verify/wsbnarrow_20261026/RESULT_D4-WSB-C4-NARROW.md`): the 8 rows ran at 5-53%, `00575c60` 1053/4161, `00575fe0` 1399/2625.

## Residual: `0x00575fe0` face-record `+0x1c` (not named in the PREREG, so not fixed here)

Both RED rows first differ in region 4, the face arena `*(W+0x58)` (stride `0x60`, `NarrowC4Probe.cpp:319`), at
offsets `0x1c`, `0x7c`, `0xdc`, `0x19c`. All of those are field `+0x1c` of one face record. Every diff is 1 ulp
(`o=bf51d6ec n=bf51d6ed`, `o=3da55bb2 n=3da55bb4`, `o=c0172327 n=c0172326`, ...). No `region 1+c` diff is left, so
`MASHED_NARROW_PAIR_INT=1` fixed the pair word. `0x00575c60` writes face records only through its `0x00575fe0` calls
(`RwpSolverCore17.cpp:651`), so it is the same site.

The writer is `0x00575fe0`'s face append (`RwpSolverCore17.cpp:686-710`). Disassembly (`re/tools/disasm_fn.py
0x00575fe0 0x00576560` on `MASHED.exe.unpatched`):

- `+0x1c` dot, FSTP `0x005763f7`: `FLD [ebx+0x34]` / `FMUL [eax+4]` (`0x005763e2`), `FLD [ebx+0x30]` / `FMUL [eax]`,
  `FADDP` (`0x005763ed`), then `FLD [ebx+0x38]` / `FMUL [eax+8]`, `FADDP` (`0x005763f5`). So the original is
  `(f[0xd]*v[1] + f[0xc]*v[0]) + f[0xe]*v[2]`. The port (`RwpSolverCore17.cpp:709`) writes
  `f[0xe]*v[2] + f[0xc]*v[0] + f[0xd]*v[1]`, grouped `(f[0xe]*v[2] + f[0xc]*v[0]) + f[0xd]*v[1]`. This is cause 1
  (print-order sum) at a site the PREREG table does not list. It is not behind `MASHED_NARROW_DOT_ORDER`.
- The `+0x50 & 2` cross block before it (FSTP `0x00576391` / `0x005763b0` / `0x005763d1`) is also in print order in
  the port (`RwpSolverCore17.cpp:700-706`). Example: the original's `+0x30` is
  `((v6-v10)*v1 + (v2-v6)*v9) + (v10-v2)*v5` (`0x0057636d..0x00576391`), and the port's is
  `((v10-v2)*v5 + (v2-v6)*v9) + (v6-v10)*v1`. `[UNCERTAIN]` whether this branch ran in the scenario: the probe
  reports only the first differing byte, and `+0x1c` comes before `+0x30` in the record.

PREREG rule: "A RED row gets its first-diff offsets in the RESULT and no further fix in this step, unless the cause
is a site already named above." This site is not named, so it is left for a follow-up step: add both expressions
to `MASHED_NARROW_DOT_ORDER` (Core17 is an x87 TU, so association only, no precision change), commit a PREREG
amendment first, then re-run FIX on `00575c60` and `00575fe0` and the default gates (a Core17 edit changes code
layout, and `sim-trajectory-is-code-layout-sensitive`).

## History: the boot blocker (2026-10-26 22:28 to the 2026-10-27 reboot)

The first attempt at these arms (commit `e9a0d8cea`) could not score anything: every boot exited at
`game exited while waiting for menu (phase 1)`, 4 to 16 s in, including with the prior step's `.asi` (`e42797f2`),
with a `__COMPAT_LAYER` override, with `MASHED_RE_NO_AUTO_HOOK=1`, and with no dev `.asi` at all. The last good
boot was 2026-10-26 22:27. PCA had added `$ DWM8And16BitMitigation` HKLM Layers entries for this root and for
`D4-WSB-C4-NARROW_root` (`re/diag/KNOWN_ISSUES.md:246`). Mariano rebooted LIVING-PC on 2026-10-27, after which a
plain launch and a Frida spawn stayed alive 25 s, and all 10 run 13 boots reached the race on attempt 1.
`[UNCERTAIN]` which cause the reboot cleared (the GPU-driver wedge or the PCA entries): the HKLM values were not
re-queried after the reboot.

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

Run 13 (same folder):

| file | SHA-256 |
|---|---|
| `arms_run13.txt` (driver output, one line per boot) | `9f2655ec9ab7133d38cdf0e80723347c730b8741b823d30fb836ded6e9dbb346` |
| `c4n_OFF_P_00577ec0_a1_r13.log` | `3481f1639e77a80f76f2467eadea399f01097f6d78d0e5cd8088e03aab4acbc5` |
| `c4n_OFF_P_00575c60_a1_r13.log` | `36e840eddbe5447b0034c044173466b8017a00b5d609d94d4ec673a50d22e8d4` |
| `c4n_FIX_P_005757d0_a1_r13.log` | `b881cb59c54e4730c0135d83071cd39f10e9a745ea8319a98d0d2ddf5c899615` |
| `c4n_FIX_P_00575880_a1_r13.log` | `4320997cb1fbe0a23853e5b4f80add07feffa75967ac63272f385d14ed6ffbf5` |
| `c4n_FIX_P_00575c60_a1_r13.log` | `670101e5092a66b8424b509019be96cf985f26431dca577856a427bebe068502` |
| `c4n_FIX_P_00575fe0_a1_r13.log` | `0082b3f1ef899aed0cc256378c8903672c9f120cf2f894864f1584fdf67276c4` |
| `c4n_FIX_P_00577ec0_a1_r13.log` | `31f1f5c44276e127b6dc90596959617e09cd099af0cbbb66baac324f43636678` |
| `c4n_FIX_P_005784a0_a1_r13.log` | `bc2f2adc54d5acefa79f3df26062f6118da486322aa793caa0cc1316d0724158` |
| `c4n_FIX_P_00578610_a1_r13.log` | `beb0371b39452f1d43bda4f0f9a71d40b8d6c496a9ecd531b0bc9745ae2086d1` |
| `c4n_FIX_P_00578e50_a1_r13.log` | `0c1834053ee701044156805c6d6a2f3cb6c9020ba2e1bf6120e565b15b21287e` |
| `launch_OFF_P_00577ec0_a1_r13.txt` | `983100e7a9c5d507d2683f2eda8c899a58b7cbed87c7800673447de86ae24c1f` |
| `launch_OFF_P_00575c60_a1_r13.txt` | `1eea5aae80b14910f3dfe37b8d875b0b8406b3567f9398cf7d3dc6b139c05e26` |
| `launch_FIX_P_005757d0_a1_r13.txt` | `b15b9f9fbe1cf1d24392a50e7e8ef7b01262988eb3bb505819a285dc837d518e` |
| `launch_FIX_P_00575880_a1_r13.txt` | `f32ce35ecd26a546416e252cf827b547741f6e672827c8866d15f5d52eea203e` |
| `launch_FIX_P_00575c60_a1_r13.txt` | `b4b0349d7cf983d228aca6f4ba0c47f05dadac28f6894f59afbd940467982e20` |
| `launch_FIX_P_00575fe0_a1_r13.txt` | `de5a9220e89afc7d8e7f6eb10f861945c7cb315a71a1c3ad2cb8dde6c6bebd74` |
| `launch_FIX_P_00577ec0_a1_r13.txt` | `995cf3760f6bdccc826aebfdf1866720e974985c6ce2b2ea70394563616a7b11` |
| `launch_FIX_P_005784a0_a1_r13.txt` | `6a3180532578fef1cdad091296b3d01ce41b1b78650976f638576ee5fe6871ba` |
| `launch_FIX_P_00578610_a1_r13.txt` | `01ee11b173f498c7b59f730c806428b8b34e1692b0697c41ea0d8df532701a74` |
| `launch_FIX_P_00578e50_a1_r13.txt` | `95ca949c274bce62f4a2bedcfb7590584fe008f76a7e58ee65d8cb8b52e73197` |

## What this RESULT does not do

No tracker change. The six GREEN rows cover the knob-ON body only, and the default build runs the OFF body, so no row
moves to C4 on this evidence (PREREG, "What this step does not do"). Promotion follows a default flip, which is a
decision row, and should wait for the `0x00575fe0` residual fix so all 8 rows flip together.
