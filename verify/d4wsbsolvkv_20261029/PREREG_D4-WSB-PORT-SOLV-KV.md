# PREREG D4-WSB-PORT-SOLV-KV

Committed alone, before the Unicorn arm. Branch `orch/D4-WSB-PORT-SOLV-KV` off `orch/integration` @ `a087d5bd9`.
Input: the 4 rows `D4-WSB-PORT-SOLV` handed off (`verify/d4wsbportsolv_20261029/RESULT_D4-WSB-PORT-SOLV.md`,
"Handoff"): `0x0056a7a0 0x0056aae0 0x0056ac40 0x0056adb0`, all C2 mapped.

## What is ported

One TU, `mashedmod/src/mashed_re/Collision/RwpVtableKV12.cpp`, written from capstone disassembly of
`original/MASHED.exe.unpatched` (`re/tools/disasm_fn.py`), decomp (`decomp_pc.py`, pool10) as a reading aid only.
Frames recovered from the call-site pushes and the caller's `add esp`: 69 words into `0x0056adb0` (`0x0056b310`,
`add esp,0x114` at `0x0056b645`), 15 into `0x0056ac40`, 56 into `0x0056aae0`, 39 into `0x0056a7a0`. Every port calls
its callees through the original RVA.

## Default-OFF

- Registered as `L2_FUN_<rva>`: installed only through `MASHED_HOOK_ONLY` or `MASHED_HOOK_LANE2=1`
  (`Core/HookSystem.cpp:244-253`). A default `.asi` run does not install them.
- In `asi_sources.rsp` only, not `exe_sources.rsp`: `mashed_re.exe` compiles nothing new. The only other source edit
  is a comment in `RwpVtableKV1.cpp` (also `.asi` only).
- No default flips. No original launch in this step (audio endpoint absent).

## Arm U (offline Unicorn, original bytes vs the port's own code)

`verify/d4wsbsolvkv_20261029/uc_arm.py`. No game process.

- Original side: Unicorn x86-32 maps every section of `MASHED.exe.unpatched` at its VA, a data arena at
  `0x20000000`, a stack, and a stub that loads x87 CW `0x007f` (PC_24, round-to-nearest, all masked, the in-game
  state) and jumps to the RVA. MXCSR is Unicorn's default `0x1f80`.
- Port side: a 32-bit harness exe built with the `.asi` flags (`cl /EHsc /W3 /O2`, default `/arch:SSE2`), whose
  source is the `RwpVtableKV12.cpp` text between `namespace mashed_re {` and `// --- registrations`, extracted
  verbatim at run time (no edit). Image base `0x10000000`. It commits the same arena at `0x20000000` and a page at
  `0x0056a000` holding `jmp` thunks from each of the 4 RVAs to the port bodies, so the port's RVA calls reach the
  ports. x87 PC set to 24 with `_controlfp_s`.
- Cases, fixed seed `20261029`: 300 per function, each function called directly with its own frame
  (`0x0056a7a0`, `0x0056ac40`, `0x0056aae0` with its callee, `0x0056adb0` with the whole chain). Floats from a
  bounded domain ([-2, 2] float32, plus forced equal lanes so every compare branch, the clamp ties, the mask
  equalities and the adaptive stop path are reachable), pass limit 1..40, W69 in {0, 1}.
- Compared bitwise: the whole arena after the call, plus the float return of `0x0056ac40` (stored with `fstp m32`
  on the original side).

### Negative controls (rounding-level mutants of the extracted port text, one build each)

| id | site | mutation |
|---|---|---|
| M1 | `0x0056a7a0` accumulate | `m0`/`m1` add order swapped |
| M2 | `0x0056ac40` sum of squares | accumulated in double, rounded to float per step |
| M3 | `0x0056aae0` final update | whole expression evaluated in double, rounded once |
| M4 | `0x0056adb0` new pass limit | `fistp` (round) replaced by C truncation |

Each mutant runs on the same 1200 cases.

### Read-out (fixed now)

- **Arm U GREEN**: the unmutated port matches every case of all 4 functions bitwise, and each mutant M1..M4 differs on
  at least 1 case of a function that executes its site.
- **Arm U RED**: any unmutated mismatch. Reported with the function, case id and first differing arena offset.
- **Arm U INVALID**: a mutant with 0 differing cases. That site is reported as not covered by the arm.
- Arm U is offline evidence only. It does not move any row to C3 (no Frida diff, `re/CONFIDENCE.md`). It is reported
  next to the shadow A/B leg that the next original-side step owes.

## Default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this build's `mashed_re.exe`, test slot +
`mashed_lock.py`, `A4_ROOT=D:/MashedWT/D4-WSB-PORT-SOLV-KV_root`. Pass = all six sha8 equal `re/orchestrator/merge_train.json`
(`5943c5f1 306186d4 52052890`, `3a03670c 9fcdbd3a 53778664` after the L-56 flip at `a087d5bd9`).

## Trackers

No promotion. The 4 rows stay C2. Through re-classify: `file` -> the port, status `mapped` -> `impl`, dated note with
the Arm U verdict, and U-9589 / U-9590 (the `0x0056a7a0` frame-depth question) closed with the disassembly cites if Arm U
is GREEN. The original-side shadow A/B (census scenario, one hook alone per boot, as in `D4-WSB-PORT-SOLV`) is queued
as `next:`.
