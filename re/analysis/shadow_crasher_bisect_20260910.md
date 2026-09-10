# The shadow-lane "crashes to bisect" list, actually bisected (2026-09-10)

`re/NEXT_SESSION.md` listed `0x0056f350 0x0056fea0 0x00570090` as a **group** crash — never
bisected to a single site — plus `0x0056f0a0` separately, and `0x0056bce0`. Bisected all five
one site per boot, and ran the two controls the original recipe did not:

| RVA | armed, alone | control (`--no-shadow`, hook live + A/B **disarmed**) | verdict |
|---|---|---|---|
| `0x0056bce0` | **CLEAN 48/48** | — | **C3, closed** (`bce0_edx_contract_20260910.md`) |
| `0x0056fea0` | RACE_OK, **NO_SAMPLES** (n=0) | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
| `0x00570090` | RACE_OK, **NO_SAMPLES** (n=0) | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
| `0x0056f0a0` | **CRASH** 30s | **RACE_OK** | crash is in the **A/B window**, not the port |
| `0x0056f350` | **CRASH** 27s | **CRASH** 26s | crash is in the **installed port** |

So the group's crash localises to `0x0056f350` and `0x0056f0a0`, and those two are **different
failure classes**. Two of the five were never crashers at all — they were guilty by association
with the group they were booted in, the same staleness shape as memory
`stale-demotion-and-0arg-verify-gap`.

> **Harness caveat that bites if you skip it:** `--no-shadow` sets `MASHED_NO_SELFTEST=1`, so a
> control run logs **no samples by construction**. A control's `NO_SAMPLES` says nothing about
> whether the function fired — only an **armed** RACE_OK with n=0 does. Both `0x0056fea0` and
> `0x00570090` above are armed runs; the `NO_SAMPLES` on the `0x0056f0a0` control is
> uninformative and is not evidence it never fires (it demonstrably does — arming it crashes).

## `0x0056f350` — a float used as a pointer, inside our own port

Caught with `re/frida/poll_attach_catch_crash.py` alongside the **control** boot
(`log/crash_eip_0056f350.txt`):

```
type         access-violation      mem_op read   mem_address 0x3f7f97ff
eip          0x6b47e597            -> mashed_re_dev.asi base 0x6b420000, offset 0x5e597
esi          0x3f7f97e3
bytes_before_eip  ... 85 f6 74 0a          = test esi,esi / jz +0xa
bytes_at_eip      f6 46 1c 08 0f 85 ...    = test byte ptr [esi+0x1c],8 / jne
```

Resolved against `mashedmod/build/mashed_re_dev.map` (preferred base `0x10000000`, so
`0x1005e597`): the nearest preceding symbol is
**`?FUN_0056f350_impl@Collision@mashed_re@@YAXHPAMM@Z` at `0x1005e420`, i.e. `+0x177` into it**,
object `RwpSolverCore10.obj`. That mangling is
`void __cdecl FUN_0056f350_impl(int, float*, float)`.

**`ESI = 0x3f7f97e3` is a plausible IEEE-754 float, `0.9984113574028015`**, and the port is
null-checking it and then dereferencing `[esi+0x1c]`. A float value is reaching a pointer slot.
This is the argument-shape class (`feedback_installed_hook_abi_mismatch`, memory
`decomp-is-silent-about-register-args`) rather than the caller-saved-register class that
`0x0056bce0` turned out to be — the register screen agrees: `0x0056f350`'s only caller
`0x00560260` decompiles with **no** `extraout_*`.

**Next step, and it is the same move that cracked `0x0056bce0`:** disassemble the original
`0x0056f350` prologue with capstone and read how it actually consumes its three arguments,
against the port's `(int, float*, float)`. Do that **before** touching the body — if the shape
is wrong, every line of the body was transcribed against the wrong signature.

## `0x0056f0a0` — a NULL write in a *third* function, and only when the A/B is armed

Armed boot (`log/crash_eip_0056f0a0.txt`):

```
type         access-violation      mem_op write  mem_address 0x0
eip          0x0056caf4            -> inside MASHED.exe (base 0x400000)
ebx 0x0   esi 0x0   eax 0x2   edx 0x11a62988
bytes_before_eip  ... 8b 1a 8b 5c 83 fc     = mov ebx,[edx] / mov ebx,[ebx+eax*4-4]
bytes_at_eip      89 33 ...                 = mov [ebx],esi
```

`0x0056caf4` is inside **`FUN_0056caa0`** — neither the hooked function nor one of its callers.
The chain `mov ebx,[edx]` then `mov ebx,[ebx+eax*4-4]` produced 0, so an indirection table it
walks came back empty, and it then wrote through it.

Because the **control boots clean**, the installed port is not what breaks this. The suspect is
the A/B window itself for this site: `ShadowAB` uninstalls the inline JMP, calls the original,
restores the tracked region, reinstalls, then runs the port. For a function whose side effects
are **not** confined to the snapshotted span — a pool allocation, a list splice, a counter — the
restore does not undo them, and the second (port) call then runs against state the original
already advanced. `ShadowAB.h`'s own LIMITS block names exactly this
("the original must be re-entrant with respect to itself"; "restoring a memory region does not
un-write a file"), and it is the same hazard memory `count-it-before-designing-a-witness`
records: check the callee is side-effect free *before* designing a run-both-and-compare A/B.

**Next step:** read `FUN_0056f0a0`'s body for a side effect outside its tracked span (`0x0056f0a0`
is called from both `0x0056f350` and `0x00570090`). If it has one, this site is **not a
RunRegion/RunTracked candidate at all** and should be moved out of the lane rather than
re-tried — that is a finding about the witness, not about the port.

## Artifacts

- `log/crash_eip_0056f350.txt`, `log/crash_eip_0056f0a0.txt`
- `log/shadow_ab/shadow_ab_20260910_1432*_1x_0056f350.log`, `..._1434*_1x_0056f0a0.log`
- `re/parity/shadow_results.tsv` — verdict rows updated by each boot
