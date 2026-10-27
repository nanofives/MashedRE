# PREREG D4-WSB-NARROW-FIX

Committed alone, before any arm runs. Branch `orch/D4-WSB-NARROW-FIX` off `orch/integration` @ `1158f5c1c`.
Prior evidence: `verify/wsbnarrow_20261026/RESULT_D4-WSB-C4-NARROW.md` (8 NARROW C3 rows write different bytes on
5-53% of calls, return values equal on every call).

## Causes found before any run (static, from `re/tools/disasm_fn.py` on `MASHED.exe.unpatched`)

Each 3-term sum below is written in the port as Ghidra prints it (`c*z + a*x + b*y`, grouped `(c*z + a*x) + b*y`).
The original adds the first two products it loads. The association is read from the FADD order; each cite is the FSTP / FCOMP that consumes the sum.

| site | original association | port file |
|---|---|---|
| `0x00577ec0` \|p2\|^2, \|p6\|^2 | `(x*x + y*y) + z*z` (FSTP `0x00577eec`, `0x00577f0e`) | Core14 (x87 TU) |
| `0x00577ec0` p6.d (both zero-length branches), p2.d, p6.d, p2.p6 | `(a.y*b.y + a.x*b.x) + a.z*b.z` (FSTP `0x00577f99`, `0x0057803a`, `0x005780ae`, `0x005780e0`, `0x005780fc`) | Core14 |
| `0x005784a0` two plane dots, two squared lengths, the append dot | dots `(n.y*d.y + d.x*n.x) + n.z*d.z`, lengths `(y*y + x*x) + z*z` (FSTP `0x005784ee`, FCOMP `0x00578526`, FSTP `0x0057856a`, FCOMP `0x005785a2`, FSTP `0x005785e0`) | Core14 |
| `0x00578bd0`, `0x00578cb0` face dot | `(m.y*n.y + m.x*n.x) + m.z*n.z` (`0x00578be5..0x00578bff`; `0x00578cb0`: FSTP `0x00578cfe`) | Core17 (x87 TU) |
| `0x00578d90` perp dot | `(n.y*d.y + n.x*d.x) + n.z*d.z` (FSTP `0x00578dd1`) | Core17 |
| `0x00575b60` three rotate dots + six-term flip sum | `(m8*a.z + m4*a.y) + m0*a.x` etc.; sum = `((s10*s30 + s14*s34) + s18*s38) + ((t.y*n.y + t.x*n.x) + t.z*n.z)` (FSTP `0x00575b9b`, `0x00575bb6`, `0x00575bd2`, FCOMP `0x00575c35`) | Core17 |
| `0x00578610` sphere-path dot, edge cross squared length | `(b.y*n.y + b.x*n.x) + b.z*n.z` (FSTP `0x005786a8`), `(c1*c1 + c0*c0) + c2*c2` (FCOMP `0x005789aa`) | Core18 (SSE TU) |
| `0x00575880` three rotate dots + six-term flip sum | as `0x00575b60`, except the shape dot is `(s14*s34 + s10*s30) + s18*s38` (FSTP `0x005759fe`, `0x00575a1a`, `0x00575a36`, FCOMP `0x00575a9f`) | Core20 (SSE TU) |

Precision. The game runs its race loop at x87 PC_24 (every operation rounds to a 24-bit mantissa), and the .asi
inherits it. Core18, Core20 and Glue5 are not in `mashedmod/x87_tus.txt`, so they build as SSE2, and their `float10`
(= `long double` = `double` under MSVC) expressions run in double precision:
- `0x0055c230` (Glue5): the three rotate dots are already in the original's association but run in double
  (`RwpSolverGlue5.cpp:178-183`). These rotate the direction that `0x005757d0` hands to the shape `+0x18` slot.
- `0x00578610` (Core18): the depth tail `(d - rA) - rB` and `((rB - rA) + d) * k + s` run in double
  (`RwpSolverCore18.cpp:812-818`, original FSTP `0x00578ad1`, `0x00578b0a`).
- `0x005667c0` (Leaves1, SSE): double. The existing HB-6 knob `MASHED_LEAVES1_X87=1` already selects the
  disassembly-order x87 body `FUN_005667c0_x87` (verify/hb6_20261026). It is used here as is.

Pair arena. `0x00575fe0` writes pair record `+0x0c` at `0x0057644d` as the INTEGER sum
`MOV ECX,[EAX+0x44]` (`0x00576444`) `+ EDX` (`param_6 * 0x10000`). The port writes `(int)pfVar6[0x11] + param_6`
(`RwpSolverCore17.cpp:685`), a float-to-int conversion of that word. The logged original values (`0x000100b6`,
`0x000100be`, `0x00020147`, ...) read as float are denormals, so the conversion gives 0, which is what the port wrote.
`0x00575c60` reaches it through its `0x00575fe0` calls. The `0x00575c60` emit block itself stores 0 at `+0x0c`
(`0x00575f64`) as the port does.

## Change (behaviour change, both knobs default OFF in the exe and the .asi, no FaithDefaults seed)

- `MASHED_NARROW_DOT_ORDER=1`: every sum in the table in the original's association; in the SSE TUs (Core18,
  Core20, Glue5) the affected expressions are computed in `float`, so each operation rounds to float as PC_24 does.
- `MASHED_NARROW_PAIR_INT=1`: `0x0057644d` reads `+0x44` as an integer.
- OFF = the current code, unchanged.

## Arms

All arms: the `run_c4n.py` probe (`Collision/NarrowC4Probe.cpp`, leg P, stride 1, census scenario: track 0, mode 10,
4 slots, 45 s hold), my build's .asi in the private root `D:\MashedWT\D4-WSB-NARROW-FIX_root`, every boot inside the
machine test slot and `mashed_lock.py run --label D4-WSB-NARROW-FIX`, one row per boot.

- **FIX**, one boot per row, 8 rows (`005757d0 00575880 00575c60 00575fe0 00577ec0 005784a0 00578610 00578e50`):
  `MASHED_NARROW_DOT_ORDER=1 MASHED_NARROW_PAIR_INT=1 MASHED_LEAVES1_X87=1`.
- **OFF** control, rows `00577ec0` and `00575c60`, no knob: must reproduce `diff > 0` on this build (the probe still
  sees the defect, so a FIX GREEN is the knob, not a changed build or scenario).
- **Gates** (default exe, no knob): `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Acceptance (decided now)

- A row is **GREEN** when its FIX boot reaches the race (`RACE_OK`), logs `A/B-IS-REAL`, installs exactly one hook,
  has `noo = 0`, `diff = 0` and 0 `DIFF` lines, with at least 100 samples.
- A row is **RED** otherwise. A RED row gets its first-diff offsets in the RESULT and no further fix in this step,
  unless the cause is a site already named above (then the fix is corrected and the row re-run; that re-run is
  reported as a deviation from this PREREG).
- A piz-collision crash (U-9677 signatures) or a `could not attach` VOID is re-run, not scored.
- Gates must equal `re/orchestrator/merge_train.json` refs: G-CTRL `5943c5f1/364350ae/52052890`,
  B `3a03670c/e85ea6fb/53778664`. A moved hash is a DECISION, and the step stops there.
- OFF control: `diff > 0` on both rows. If either shows `diff = 0`, the step is BLOCKED (the probe or scenario changed).

## What this step does not do

- No tracker change. A FIX GREEN covers the knob-ON body only. The default build still runs the OFF body, so no row
  can move to C4 on this evidence (`re/CONFIDENCE.md`, "Which copy the evidence covers"). Promotion follows a flip,
  and the flip is a decision row.
- No default flip, no FaithDefaults seed, no `.ps1` edit.
- The TOI path (`0x0057adb0` / `0x0057a9a0`) that `0x00575880` takes when a shape has flag `0x10` is not changed.
  If `0x00575880` stays RED with a diff from that path, the RESULT says so.

## Amendment 1 (step D4-WSB-NARROW-FIX2, 2026-10-27), committed alone before any of its arms

Branch `orch/D4-WSB-NARROW-FIX2`: `orch/D4-WSB-NARROW-FIX` @ `e005a6283` with `orch/integration` @ `b4aab7780`
merged in (`82b782287`). Trigger: run 13 left `00575c60` (82/6273) and `00575fe0` (81/1985) RED, every diff 1 ulp
in face-record field `+0x1c` (`verify/wsbnarrow_20261027/RESULT_D4-WSB-NARROW-FIX.md`, "Residual").

### Sites added to `MASHED_NARROW_DOT_ORDER` (static, `re/tools/disasm_fn.py 0x00576320 0x00576400` on `MASHED.exe.unpatched`)

`v` = the source record `pfVar6` (`EAX`), `f` = the appended face record `pfVar10` (`EBX`). Port file
`RwpSolverCore17.cpp` (x87 TU), so this is association only, no precision change.

| site | original association (FADD order) | cite | port today |
|---|---|---|---|
| `0x00575fe0` flag-2 cross `f[0xc]` | `((v6-v10)*v1 + (v2-v6)*v9) + (v10-v2)*v5` | `0x0057636d..0x0057638f`, FSTP `0x00576391` | `Core17.cpp:701-702`, reversed |
| flag-2 cross `f[0xd]` | `((v4-v8)*v2 + (v8-v0)*v6) + (v0-v4)*v10` | `0x00576393..0x005763ae`, FSTP `0x005763b0` | `Core17.cpp:703-704`, reversed |
| flag-2 cross `f[0xe]` | `((v1-v5)*v8 + (v5-v9)*v0) + (v9-v1)*v4` | `0x005763b3..0x005763cf`, FSTP `0x005763d1` | `Core17.cpp:705-706`, reversed |
| face-plane dot `f[7]` (`+0x1c`) | `(f[0xd]*v1 + f[0xc]*v0) + f[0xe]*v2` | `0x005763e2..0x005763f5`, FSTP `0x005763f7` | `Core17.cpp:708-709`, `(f[0xe]*v2 + f[0xc]*v0) + f[0xd]*v1` |

The cross block runs only when `*(byte*)(W+0x50) & 2` (`0x00576365..0x0057636b`). Its operands enter as
`(a - b)` then `FMUL m32` (`st0 = (a-b)*m`), so each product's operand order is as written above.

### Arms for this amendment

Same probe, scenario, acceptance and gates as above. Own private root `D:\MashedWT\D4-WSB-NARROW-FIX2_root`, raw data
`D:\MashedWT\D4-WSB-NARROW-FIX2_data`, lock label `D4-WSB-NARROW-FIX2`, driver `run_c4n_fix2.py` (a copy of
`run_c4n_fix.py` with only those three names changed).

- **FIX**, all 8 rows (the base now also carries the `orch/integration` merge, so the 6 GREEN rows are re-run, not
  carried over): `MASHED_NARROW_DOT_ORDER=1 MASHED_NARROW_PAIR_INT=1 MASHED_LEAVES1_X87=1`.
- **OFF** control, rows `00575c60` and `00575fe0`, no knob: must show `diff > 0`, otherwise BLOCKED.
- **Gates** (default exe, no knob, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`): must equal the
  `re/orchestrator/merge_train.json` refs read at gate time (2026-10-27: G-CTRL `5943c5f1/364350ae/52052890`,
  B `3a03670c/e85ea6fb/53778664`). A moved hash is a DECISION. A Core17 edit changes code layout
  (`sim-trajectory-is-code-layout-sensitive`), so the gates are re-run, not carried over.

Acceptance: step GREEN = 8/8 FIX rows GREEN, both OFF rows `diff > 0`, gates equal. A RED row gets its first-diff
offsets in the RESULT and no further fix in this step, unless the cause is a site named in this amendment.
