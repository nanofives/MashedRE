# RESULT BIND-CRT-HELPERS (2026-11-11)

Verdict: **GREEN.** The six MSVC compiler helpers are bound on the MASHED_ORIG_TOP path to the exe's own VS2022 CRT
helpers through naked `jmp` thunks. Re-measured runnable C4-by-match rows: **1,027 -> 1,063 (+36)**, exactly the
inventory's prediction (RESULT_PORT-OT-BIND-CRT.md). 26 CRT functions remain in reach.

## Binding (`Match/PortTop_CrtHelpers.cpp`, exe-only, `// RVA-LINT: ORIGTOP-ONLY`, +1 line in exe_sources.rsp)

| rva | helper | identity (libid vs XP DDK libcmt.lib, libidentity_functions.tsv) | convention | exe thunk -> VS2022 CRT |
|---|---|---|---|---|
| 0x004a2c48 | `_ftol2` | ftol2.obj, identical-unlisted | ST0 in (popped), EDX:EAX out, truncation | `MashedCrt_ftol2_004a2c48` -> `_ftol2` |
| 0x004a4170 | `_alldiv` | lldiv.obj, identical-unlisted | 2 x int64 on the stack, EDX:EAX, `ret 0x10` | `MashedCrt_alldiv_004a4170` -> `_alldiv` |
| 0x004a4220 | `_allmul` | llmul.obj, identical-unlisted | same | `MashedCrt_allmul_004a4220` -> `_allmul` |
| 0x004a4e40 | `_aulldiv` | ulldiv.obj, identical | same | `MashedCrt_aulldiv_004a4e40` -> `_aulldiv` |
| 0x004aa060 | `_aullrem` | ullrem.obj, identical | same | `MashedCrt_aullrem_004aa060` -> `_aullrem` |
| 0x004a3440 | `_chkstk` | chkstk.obj, identical-unlisted | EAX = bytes, ESP lowered, pages probed | `MashedCrt_chkstk_004a3440` -> `_chkstk` |

- **What a thunk is:** a single `jmp`, because the DDK and VS2022 helpers share the compiler ABI. It is glue, not a
  body. The port column keeps all six at `none`, the same as the existing `crt` bindings (printf, vsprintf, exit,
  sprintf).
- **How a matched source uses it:** a TU binds a helper with `#define FUN_004a2c48 MashedCrt_ftol2_004a2c48`. The 31
  sources declaring `ulonglong FUN_004a2c48(void)` are not edited, so their match.py status is unchanged.
- **BINDINGS:** `update_bindings_bindcrthelpers.py` through `bindings_rows.apply`, six rows, kind `crt` (510 -> 516).
- **Self-contained bodies:** all six original bodies were checked with capstone before the test ran them in place
  (no absolute address; the `_ftol2` hits are the constants 0x80000000 / 0x7fffffff / 0xfffffff0).

## Equivalence: unit test against the DDK bodies (`verify/bindcrthelpers_20261111/`)

`gen_orig.py` copies the six bodies from `original/MASHED.exe.unpatched` (Ghidra sizes) into `orig_helpers.h`.
`helper_test.cpp` (VS2022 x86 `/O2`, includes the thunk TU itself) runs each original in place from executable memory
against its thunk. `helper_test.out` holds the result: **`TOTAL 451 checks, 0 FAIL`**.

- **`_ftol2`, 408 checks.** 34 values × 12 x87 control words: precision PC_24 / PC_53 / PC_64 × rounding nearest /
  down / up / chop, all exceptions masked.
  - Values: ±0, ±0.5, 0.49999999, ±1.5, ±2.5, ±7.999999, ±123456789.987, 2147483647.5, -2147483648.5,
    4294967296.75, ±1e10, ±9.2e18, ±9.3e18, ±1e19, ±1e300, ±inf, NaN, ±1e-300, 16777217, 0.1f, 33.3f.
  - Compared: the full EDX:EAX, and the x87 TOP after the call (ST0 popped the same way).
  - So the result is the DDK's truncation in every control word MASHED runs, PC_24 included (memory
    x87-and-asm-port-gotchas-hub), not the current FPU mode.
- **`_alldiv` / `_allmul` / `_aulldiv` / `_aullrem`, 9 vectors each.** Signs, values over 32 bits, a divisor over 32
  bits, INT64_MIN / 1. Compared: the full 64-bit result.
- **`_chkstk`, 7 sizes** (4, 0x100, 0xfff, 0x1000, 0x1001, 0x3000, 0x10000). Compared: the ESP drop across the call.

## Gates

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi` not deployed. mashed_re.exe sha256 `24fad5ab137ceda560625537468d2c4df5ed84272539d1d00b0f81b1a60c1ae3` |
| knob boots (`MASHED_ORIG_TOP=1`, 3000 frames): RWMOD off, RWMOD on x2 | rc 0, `WinMain 0x00492370 returned 0 after 3000 frames` in all three (27-29 s each; a quiet machine, 8/8 test slots free) |
| 12 captures, RWMOD off / on | 8/8 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new. A comment table opening lines with `0x<rva>` first bound the thunks as bodies (CROSS-TARGET vs the .asi's CrtStackProbe hook). It was reworded to `rva <hex>`, so there is no allowlist entry. |
| port column | `backfill_port.py --allow-stale` (the map is older than the comment-only reword): 9 rows changed, all of them TL-MATCH-REST's new C1 rows getting `none` in their empty port cell. The six helpers stay `none`. |

No helper is called on the knob path yet. None of the matched sources calling them is in the exe, so the boots and
gates show the new TU is inert on both paths. Each binding comes into use when the row that compiles such a source
binds the helper.
