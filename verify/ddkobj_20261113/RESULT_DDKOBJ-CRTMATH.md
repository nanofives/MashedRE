# RESULT DDKOBJ-CRTMATH (2026-11-13)

Row: L-120 final form. Branch `orch/DDKOBJ-CRTMATH`. Row kind: top-down (object build). No G-CTRL, no B, no 12 captures.
Input: `verify/ddkobj_20261111/RESULT_DDKOBJ-SPIKE.md` section 7 and section 10 item 7.

## Verdict

GREEN. The renamed DDK libc math objects are linked into the object build, a DDK object's reference to one of their
names is renamed to the `_ddkcrt*` form, and the fixture proves it. **No real unit references CRT math today**, so on
the 1440 units the rename path is exercised by the selftest fixture only (section 2).

**Self-test result: `ddkobj.py selftest` = OK, 0 failures, 28 PASS** (`selftest.log`, 3 new checks for this row).

## 1. What changed (tool only, default build untouched)

- `re/tools/ddkobj/mathprobe/ddkcrt_rename.py`: the extraction and rename is now `build(out, members, mp, fixed)`,
  importable. `MATH_MEMBERS` = `sin.obj cos.obj 87disp.obj common.obj genexcep.obj 87except.obj fpctrl.obj`
  (the spike's list, cut at the error path). CLI unchanged.
- `re/tools/ddkobj/ddkobj.py` `build`:
  - extracts and renames the members into `<out>/ddkcrt` (44 definitions renamed to `_ddkcrt<name>`, in 7 objects).
    The 6 names left undefined resolve in the VS2022 CRT: `___fastflag __fltused __handle_exc __matherr __raise_exc __set_errno`.
  - each object gets `@feat.00` (SafeSEH) when it registers no handler. An object that does fails the build
    (exit 3), like a DDK unit. Result: 0 unmarked.
  - an undefined name of a DDK object that the 44 definitions cover is renamed `_ddkcrt<name>` (class `ddkcrt` in
    the build json). A math name the set does not cover stays on the VS2022 CRT and is counted in
    `ddkcrt.crt_math_still_vs2022`. Both counts are in `ddkobj_build.json` and printed in the build log.
  - the 7 objects join `link_ddk.rsp`. VS2022 callers keep the VS2022 CRT, so only DDK objects use the DDK bodies.
- Selftest fixture `math` (`0x004000a0`): `#pragma function(sin, cos)` source. Checks: unit ok and both references
  renamed to `_ddkcrt_sin` / `_ddkcrt_cos`, no un-renamed sin/cos left in its object, and the build json counts them.
- `ftolprobe.c`, `build_ftol.bat`, `compare_ftol.py`, `ftol_range_check.py`: the `__ftol` probe (section 4).

## 2. Object build, byte check, boots

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& set MASHED_DDKOBJ=1&& mashedmod\build.bat"` in the worktree, rc 0, link rc 0
(`mashed_re_ddk.exe` sha256 `788cf4c9...`). Log line: `ddkcrt: 44 renamed definitions in 7 objects, referenced by DDK
objects: none; crt math left on VS2022: none`.

`ddkobj.py check`: **IDENTICAL 1188 of 1214**, DIFFERENT 11, not-in-map 15, units linked 1164 of 1440, refusals 4
(`wrapper-redefines-globals`), 272 provisional skipped (`ddkobj_summary.json`, `BYTECHECK.tsv`).
Every one of the 11 DIFFERENT comes from a source below 100% in `match.py` (re-run today):
`0x00401690` 60.2%, `0x004332a0` 97.3%, `0x0043c5b0` 40.5%, `0x0045a3a0`, `0x00475a00`, `0x004929d0`, `0x00493710`,
`0x00493900` (the five below 100% already listed in the SPIKE RESULT), `0x00498810` 86.7%, `0x004c0c20` 98.9%,
`0x004c0f00` 98.8%. So IDENTICAL holds for every substituted function whose source is 100%.
The 15 not-in-map functions are not in the link map (the SPIKE RESULT explains 3 of them as `/OPT:REF` drops of
callerless functions: `0x004053d0 0x00405420 0x00405430`) [UNCERTAIN: the other 12 were not individually classified here, the
list is in `ddkobj_summary.json`].
No unit refused by the tool beyond the 4 `wrapper-redefines-globals` ones and the provisional skips, whose reasons stay
as in `UNITS` of the build json.

Knob boots of `mashed_re_ddk.exe`, private root `D:/MashedWT/DDKOBJ-CRTMATH_root`, 3000 frames, through
test-slot and `mashed_lock.py run`:

| arm | result |
|---|---|
| knob OFF (`MASHED_DET_FRAMES=3000`) | rc 0 after 27.5 s, `B17-SUMMARY reached-main-loop chrome=YES thunks=6/6` |
| knob ON (`MASHED_ORIG_TOP=1`) | `WinMain 0x00492370 returned 0 after 3000 frames`, exit 0 |

The menu walk was not run: no menu code changed.

## 3. CRT math use in the substituted set

The scan of every DDK object's undefined symbols (`crt_math_still_vs2022`, `referenced_by_ddk_objects`) found **0**
math references on the 1164 linked units. Cause, found with the fixture: cl 13.00 at `/O2` compiles `sin`/`cos`
as inline `fsin`/`fcos` (checked: `/FAs` of a one-line `sin(x)+cos(x)` has no call), so a call into libc appears only
in the function form (`#pragma function`) or for functions without an inline form. Today no matched source needs
the renamed objects. They link in and are dropped by `/OPT:REF` unless referenced.

## 4. `__ftol` probe

`FTOL_PROBE.md`, raw sha256 in `FTOL_RAW_SHA256.txt`. 4126 inputs per mode, x87 PC_24 and PC_53. The DDK build calls
libc `__ftol` (`/FAs` shows `EXTRN __ftol`).

| DDK vs | `(int)double` | `(int)float` | PC |
|---|---|---|---|
| VS2022 `/arch:IA32` | 8 differ | 7 differ | 24 and 53 |
| VS2022 SSE2 | 8 differ | 7 differ | 24 and 53 |

**All 8 `(int)double` differences are inputs outside int32 or NaN/Inf** (`-2147483649.0`, `4294967296.0`, `+-1e30`,
`+-Inf`, 2 NaN): in range there are **0 differences** in 4096 random inputs plus the in-range edges.
Out of range the DDK `__ftol` returns the low 32 bits of the 64-bit conversion (`2147483647`, `0`), VS2022 returns
`-2147483648`. The third form `(int)(d*3+0.25)` against SSE2 gives 480 differences at PC_24: that is the multiply
rounding in x87 PC_24 against SSE2 doubles, not `__ftol`. Direction (b) leaves `__ftol` on the VS2022 version for
VS2022 callers and DDK objects reach VS2022's `__ftol` too (it is not in the 44). In range the two agree, so the 8
units that use it are safe in range. Out-of-range inputs would differ. [UNCERTAIN: whether any of the 8 units can see
an out-of-range operand is not measured here.]

## 5. SSE2 sqrt and PC_24 (from the SPIKE probe, section 7)

An SSE2 `sqrt` ignores the x87 precision control and differs from the DDK `fsqrt` in **4096 of 4096** results at PC_24
(`MATHPROBE.md`). At PC_53 it differs in 0. Consequence: VS2022 code compiled with SSE2 in the exe cannot reproduce
the original's `sqrt` at PC_24. A matched body that calls `sqrt` must be a DDK object (inline `fsqrt`, honours PC_24).
`sqrt` is an inline x87 instruction in DDK code and needs no renamed object.

## 6. Files and raw data

Committed: this RESULT, `FTOL_PROBE.md`, `FTOL_RAW_SHA256.txt`, `BYTECHECK.tsv`, `ddkobj_summary.json`.
Raw (not committed), `D:/MashedWT/DDKOBJ-CRTMATH_data` sha256: `build1.log` ed4faa1f8e31e282b73969806e19ffc94479f570a7cbf0542fa3b29ca8e75620,
`selftest.log` 534cea5866484c4b39dcc68d77706320a62b31bed5e05fef0d59c27dff136aec,
`crt_on.log` 13591604e12caef991f9eb3c194586d1850dd915a6227f3fc85a3c137d9e9b86,
`crt_off.log` a187ee0fdac3db49e47e0a8ec1952618d0a0b104968b510d9822d2ba2f8cd4a4.
Probe binaries and `.bin` outputs: the session scratchpad (`ftol/`), hashes in `FTOL_RAW_SHA256.txt`.

Default gates: not run (top-down row). The default build objects are untouched: the change is in `ddkobj.py` and
`mathprobe/` only, so the default `link.rsp` and exe cannot move.
