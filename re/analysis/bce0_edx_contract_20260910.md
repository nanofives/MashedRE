# The `0x0056bce0` shadow crash is an implicit EDX-preservation contract (2026-09-10)

Open item **B** from `re/NEXT_SESSION.md`. RESOLVED: root-caused, fixed, and the site now
runs 48/48 CLEAN. Two unrelated precision defects were found on the way and are also fixed.

## The kickoff's hypothesis was wrong, and the disassembly says so

Item B proposed that `param_3` might not be a plain cdecl stack argument (x87 args being
invisible to Ghidra), which would make both the shadow call signature and the installed port
ABI-wrong. **Refuted.** Raw capstone disasm of `0x0056bce0`:

```
0056bce0  83ec10      sub esp, 0x10
0056bce3  8b442414    mov eax, dword ptr [esp+0x14]   ; = orig_esp+4  -> param_1
0056bce7  8b4c2418    mov ecx, dword ptr [esp+0x18]   ; = orig_esp+8  -> param_2
...       d84c241c    fmul dword ptr [esp+0x1c]       ; = orig_esp+0xc -> param_3
0056bde0  83c410      add esp, 0x10
0056bde3  c3          ret
```

All three arguments are plain stack dwords in cdecl order, and the epilogue is caller-cleanup
`ret`. `void(__cdecl*)(float*, float*, float)` is correct.

## Triage sequence (each step ruled something out)

| step | run | result | rules out |
|---|---|---|---|
| 1 | A/B armed, group 1 | CRASH in 12s, **one** CLEAN sample first (`ndiff=0 fields=0/4`, `installed=e9 uninstalled=83`) | port is bit-identical on the first call; the wrapper ABI is fine |
| 2 | `--no-shadow` (hook live, A/B **disarmed**) | CRASH in 11s | the Uninstall/Install window and the A/B machinery |
| 3 | zero hooks, same scenario | RACE_OK, drove 30s | the scenario, the save, the harness |

So the crash was caused by *installing this port*, not by the A/B. Step 3 is the baseline the
lane needs and step 2 is the control that separates a bad port from a bad witness.

## The crash, measured

Caught with `re/frida/poll_attach_catch_crash.py` running alongside the repro
(`log/crash_eip.txt`):

```
type         access-violation
eip          0x0056c3e7          <- inside MASHED.exe (base 0x400000)
edx          0x00000000
mem_address  0x4       mem_op read
bytes_at_eip      d9 42 04 ...            = fld dword ptr [edx+4]
bytes_before_eip  ... 52 e8 f9 f8 ff ff   = push edx / call rel32
```

`call rel32` with displacement `0xfffff8f9` = -1799 from the next instruction `0x0056c3e7`
lands on **`0x0056bce0`** exactly. So the faulting instruction is the one immediately after
the call to the hooked function, and it dereferences EDX.

Confirmed at the source: two of the three callers hold the quaternion pointer in EDX **across**
the call:

```
FUN_0056c310:  0056c3d7 shl edx,4 / 0056c3da add edx,eax / 0056c3e1 push edx
               0056c3e2 call 0x56bce0
               0056c3e7 fld dword ptr [edx+4]      <-- EDX must survive
FUN_0056c0a0:  0056c17a push edx
               0056c17b call 0x56bce0
               0056c180 fld dword ptr [edx+4]      <-- same
```

The original honours that contract by accident of its codegen: it touches only EAX, ECX and
the x87 stack and **never writes EDX** — which is precisely what this TU's header note 6
already recorded as the origin of the decompiler's `extraout_EDX`. Our C++ port carries no
such constraint, MSVC uses EDX as scratch, and so the caller's pointer became 0.

`FUN_0056be80` is **not** exposed: its call site holds the pointer in ESI
(`0056bf6a fld [esi+4]`), and ESI is callee-saved, so MSVC preserves it for us.

## Fix 1 — the register contract (this is what stopped the crash)

A `__declspec(naked)` shim around the shadow wrapper pushes/pops EDX and restores
`EAX = param_1` on exit (the original leaves EAX = param_1 at `0x0056bce3` and never rewrites
it). ECX is deliberately **not** restored: the original genuinely clobbers it at `0x0056bce7`,
so preserving it would be less faithful, not more.

**Result: `RACE_OK` in 42s, `n=48 ndiff=0` CLEAN, proof `A/B-IS-REAL installed=e9
uninstalled=83`** (`log/shadow_ab/shadow_ab_20260910_142315_1x_0056bce0.log`).

## Fixes 2 and 3 — two precision defects found on the way (NOT the crash cause)

Both were applied and re-tested before the EDX fix; the crash reproduced unchanged, so
neither causes it. They are corrections in their own right, from the same disasm.

**(a) Normalization denominator association.** The `FLD/FMUL-ST/FADDP` cascade at
`0x0056bd8b..0x0056bda9` combines `{fVar3², fVar5²}` first, then `+fVar1²`, then `+fVar2²`:

```
0056bd91 fld st(3) / 0056bd93 fmul st(4)     -> X*X   (X = fVar3)
0056bd95 fld st(2) / 0056bd97 fmul st(3)     -> Y*Y   (Y = fVar5)
0056bd99 faddp st(1)                         -> X²+Y²
0056bd9b fld st(1) / 0056bd9d fmul st(2)     -> Z*Z   (Z = fVar1)
0056bd9f faddp st(1)                         -> (X²+Y²)+Z²
0056bda1 fld [esp+0x14] / 0056bda5 fmul same -> W*W   (W = fVar2)
0056bda9 faddp st(1)                         -> ((X²+Y²)+Z²)+W²
```

The port carried the decompiler's printed left-associated order
`fVar2²+fVar1²+fVar5²+fVar3²`, which combines `{W²,Z²}` first — a **different tree**, not a
commutation. This is exactly the class header note 7(b) corrected for the three sibling sites
(`0x0056bf6a / 0x0056c180 / 0x0056c3e7`) on 2026-07-17; `0x0056bce0`'s own normalization at
`0x0056bd8b` was not in that list and was missed. Rewritten fully right-associated, the same
way note 7(b) writes the siblings.

**(b) One rounding, not two.** `0x0056bdab fsqrt` leaves the root in 80-bit ST0;
`0x0056bdb1 fdivr dword [0x5cc320]` divides 1.0f by that 80-bit value; only
`0x0056bdb7 fstp dword [esp+0x1c]` rounds — once — to float32, and the four component
multiplies reload that float32. The port cast the sqrt to `float` *before* dividing, inserting
a rounding step the original does not have. The cast moved to the assignment.

The four MAD numerators were each re-verified against `0x0056bceb..0x0056bd54` and **do**
match: their inner adds are single 2-term nodes, hence commutative.

## Consequence for the lane — this defect class is not specific to this RVA

Any hooked port whose caller keeps a value in a **caller-saved** register (EAX/ECX/EDX)
across the call inherits this. Ghidra surfaces it as `extraout_EAX` / `extraout_ECX` /
`extraout_EDX` **in the caller's decompilation**, which is why reading only the callee cannot
find it. Callers using EBX/ESI/EDI are safe because MSVC must preserve those anyway.

Practical screen for the remaining shadow-lane crashers: decompile the CALLERS and grep for
`extraout_`. If a caller dereferences one right after the call, the port needs a
register-preserving shim, not a body change.

## Artifacts

- `log/crash_eip.txt` — the caught access violation
- `log/shadow_ab/shadow_ab_20260910_142315_1x_0056bce0.log` / `.tsv` — 48/48 CLEAN
- `log/shadow_ab/launch_20260910_1414*.txt`, `..._1419*`, `..._1421*` — the three crash boots
- `log/build_bce0_fix_20260910.txt`, `log/build_bce0_edx_20260910.txt`
