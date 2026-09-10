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
| `0x0055bd80` | **CRASH** 17s (1 sample, ndiff=0) | **RACE_OK** | **witness** crash — port is fine |
| `0x00560260` | **CLEAN 24/24**, twice | RACE_OK | **not a crasher any more → promoted C3** |

### `0x00560260` was listed as a crasher and just runs clean — re-run before believing it

`re/NEXT_SESSION.md` had it as "CRASH after 24 clean samples, heap effects unrestored". It now
completes `RACE_OK` with `n=24 ndiff=0`, `pages diff:0 stack:0 noise:0 ovf:0`, proof
`A/B-IS-REAL installed=e9 uninstalled=83`, and the control is `RACE_OK` too, so there is no
witness problem either. Because the standing record called it a crasher, the CLEAN was **not
taken on one boot** — reproduced on two independent boots (`..._152255`, `..._152457`), both
24/24. This is the stale-blocker shape: the blocker had been fixed, or was a group/timing
artefact, and the row simply needed re-running.

Promoted C2→C3. Gate: caller `0x00561040` C3; callees `0x0056f350`/`0x00570090`/`0x0056f0a0` all
C2; no stubs in `RwpSolverPartition13.cpp`. Its plate carried **three unfiled `[UNCERTAIN]`
markers with zero rows in `UNCERTAINTIES.md`** — filed as **U-9131/U-9132/U-9133**, each
`Blocks=none` under the standing rule for naming/intent uncertainties on an A/B-verified verbatim
body, with the inline markers rewritten to carry their ids.

`0x0055bd80` is **classified, not fixed**: armed CRASH after 1 sample (`ndiff=0`, real proof) but
control `RACE_OK`, so like `0x0056f0a0` it is a witness crash with a working port. Stays C2; it
needs the same lane decision, not a body change.

### Running total on the 9-item list

| RVA | state |
|---|---|
| `0x0056bce0` | **fixed → C3** (EDX contract) |
| `0x00560260` | **CLEAN → C3** (stale blocker) |
| `0x0056fea0`, `0x00570090` | never crashers (armed `NO_SAMPLES`) |
| `0x0056f0a0`, `0x0055bd80` | **witness** crashes — port fine, lane decision pending |
| `0x0056f350` | genuine port defect, **more than one** divergent path; needs a per-iteration trace |
| `0x00421960`, `0x004219c0`, `0x00495fe0` | Lane 2 generated, still untriaged |

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

### Argument shape: checked, and it is CORRECT

Disassembled the original's prologue. `sub esp,0xf0` then `push ebx/ebp/esi` puts esp at
`E-0xfc`, so `0x0056f359 mov esi,[esp+0x104]` = `E+8` = **arg2**; after `0x0056f365 push edi`
(esp `E-0x100`) the later reads `[esp+0x104]` = `E+4` = **arg1** and
`0x0056f677 fld [esp+0x10c]` = `E+0xc` = **arg3, a float**. Three cdecl args, and the port's
`(int param_1, float *param_2, float param_3)` maps onto them exactly — the port's
`mov eax,[ebp+8]` / `mov edi,[ebp+0Ch]` put arg1 and arg2 in the same roles the original's
arg1/ESI have. **So the signature is not the defect.** ESI holding a float at the fault is a
*downstream* value, not a mis-passed argument.

### What was then checked line-by-line against the disasm, and HOLDS

| port | original | verdict |
|---|---|---|
| `pfVar9 = param_2 + 4` (float*, = +0x10) | `0x0056f49f lea ebp,[esi+0x10]` | same base |
| `puVar3 = *(void**)(pfVar9+3)` (= +0xc) | `0x0056f4a2 mov ebx,[ebp+0xc]` | same field |
| `pfVar9 = pfVar9 + 10` (= +0x28) | `0x0056faab add ebp,0x28` | same stride |
| bound `*(int*)(param_2+0x2b)` (= +0xac), re-read | `0x0056faa4 mov ebx,[esi+0xac]`, re-read | same |
| `puVar3==0 \|\| !(byte[puVar3+0x1c]&8)` → body | `0x0056f4ad test/je` + `0x0056f4d9 test byte[ebx+0x1c],8 / jne` | same |
| `local_78[27]` handed to `FUN_0056fad0` / `FUN_0056f1f0` | `fad0` writes ≤ dword `0xe`; `f1f0` only READS, ≤ `[ecx+0x60]` = dword `0x18` | 27 dwords suffices — **no stack overflow** |

**Iteration index at the fault, computed:** `EAX 0x11aba8a4 − EDI 0x11aba7d0 = 0xD4`; minus the
`+0xC` base leaves `0xC8`, `/0x28` = **k = 5**. So it faults reading
`[param_2 + 0xC + 5*0x28 + 0x10]` = `[param_2 + 0x14C]` — and the original reads that **same
address on that same iteration**. Since base, stride, field and bound all match, either the
original does not reach k=5, or `param_2`'s array content differs by then. Neither is explicable
from the callee alone, and only `0x0056f350`'s hook was installed
(`MASHED_HOOK_ONLY`), so every callee (`FUN_0055b750`, `FUN_0056fad0`, `FUN_0056f1f0`) was the
original.

### One real transcription defect found — and it is NOT the crash

The loop counter `local_b0` was declared `float`. The original's is a plain integer:

```
0x0056f485  mov  dword ptr [esp+0x50], 0
0x0056faa0  mov  edi, dword ptr [esp+0x50]
0x0056faaa  inc  edi
0x0056faae  cmp  edi, ebx                 ; ebx = [esi+0xac]
0x0056fab0  mov  dword ptr [esp+0x50], edi
0x0056fab4  jb   0x56f4a2                 ; unsigned
```

Ghidra typed the local `float` and the port inherited it, so MSVC emitted
`cvttss2si / inc / cvtdq2ps / movss` **plus a helper `call`** before
`cmp eax,[edi+0xAC]` on every iteration. Retyped to `int`. It agrees numerically below 2^24, so
it was never going to be the crash — **and it wasn't: the crash reproduced, but the fault
MOVED**, to `0x6b47e8d7` (`divss xmm0,[eax]` with `EAX = 0`, reached through
`cmovae ecx,edx` / `cmovb eax,[esp+0x50]`, with ECX/EDX holding the `.rdata` defaults
`0x5e57c4`/`0x5e57d0`). A `cmov`-ised default-pointer ternary is yielding 0 where the original
yields its `0x5e57cc`-family default.

**That the fault moved is the finding:** `FUN_0056f350` has more than one path where the port
reaches a state the original does not, and single-crash whack-a-mole on it is low yield.

**Standalone regression checked** — the counter change is a no-op there. `MASHED_PARITY=1` walk,
17 screens vs the pre-change build: 15/17 byte-identical, and the s6/s7 residue is **13 and 25
pixels confined to `x 212..259, y 130..178`**, the pulsing category-sprite band that the
same-build B-vs-B2 control already showed varies run to run. Nothing in the checklist column
`x 524..541` moved.

**Next step is a runtime probe, not more static reading:** log `k`, `[param_2+0xac]` and
`[param_2+0xc+k*0x28+0x10]` per iteration out of the port, and diff against a Frida trace of the
original's loop over the same call. Until the two iteration traces are side by side, the state
divergence cannot be localised.

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

### `0x0056f0a0`'s side effects, read — and they are exactly the excluded shape

`FUN_0056f0a0` pads a contact batch to a 4-multiple and then **advances every running offset in
the batch**. Confirmed on the original's disassembly (113 instructions, `0x0056f0a0..`):

- ~16 read-modify-write accumulators. They are load/add/store *through registers*, not memory-RMW
  instructions — which is why a naive `add [mem]` grep finds none:
  `0x0056f178 mov ebp,[eax+edx*4]` / `0x0056f17e add ebp,edi` / `0x0056f180 mov [eax],ebp`.
- Four of those accumulate into **separately allocated arrays reached through pointers stored in
  the argument struct**, indexed by cursors also stored there: `[esi+0xb8]` (idx `[esi+0xf8]`),
  `[esi+0xc4]` and `[esi+0xd0]` (idx `[esi+0xd4]`), `[esi+0xdc]` (idx `[esi+0xe0]`) —
  `0x0056f0ae`, `0x0056f19e`, `0x0056f188`, `0x0056f169`.
- It bumps the batch cursor itself: `[esi+0xf8] += 1` (`0x0056f1be` onward).
- And it calls `FUN_0056f1f0` in a loop, which writes into yet more pointer-reached arrays
  (`[eax+0x10]`, `+0x1c`, `+0x28`, `+0x34`, `+0x40`, `+0x4c`, `+0x58`, `+0x64`, `+0x88`, each
  `+ edx*4`, plus an `[eax] + esi<<6` block).

`RunTracked` restores the pages it observed being written between the two runs. A write set
scattered across heap arrays selected by cursors *that the call itself advances* is what
`ShadowAB.h`'s LIMITS block excludes ("the original must be re-entrant with respect to itself"),
and it matches the observed failure: a table walk `mov ebx,[edx]` / `mov ebx,[ebx+eax*4-4]`
returning 0 and then being written through.

**Hedge, because the obvious generalisation is WRONG.** A screen of every `RunTracked`/`RunRegion`
site in the tree for the accumulator shape flags exactly three, and **two of them are recorded
CLEAN 24/24** (`0x0056f020`, `0x0056d070`). So "accumulator ⇒ invalid candidate" does not hold.
The discriminator that survives n=3 is narrower: `0x0056f020`'s 7 accumulators are **direct on
the argument struct** (same page as `param_1`, trivially in the tracked set) and `0x0056d070`'s
are two `*piVar` locals — whereas `0x0056f0a0` is the only site whose accumulators go **through
pointers loaded from the struct into other allocations**. That is a hypothesis with a named
discriminator and n=3, not a law.

**Next step:** decide the row rather than re-run it. If the pointer-indirected accumulator
reading holds, `0x0056f0a0` should leave the lane — the CRASH is a finding about the witness, and
the `--no-shadow` control already establishes the port itself is fine.

### Harness defect found and fixed: a control boot destroyed the armed verdict

The crash-triage recipe *requires* a `--no-shadow` control — and running one silently overwrote
the result it was meant to explain. `--no-shadow` sets `MASHED_NO_SELFTEST=1`, so no samples are
logged and every site returns `NO_SAMPLES`; `shadow_batch.py`'s `done.update(results)` merged that
like a normal result. Observed directly in `re/parity/shadow_results.tsv`: `0056f0a0` was `CRASH`
at 14:34 and `NO_SAMPLES` at 14:35, the moment its control ran. **That is how a real crasher
silently reads as "never fired."**

Fixed: control mode now records the boot outcome in its own `control` / `control_at` columns and
leaves `verdict`/`n`/`ndiff`/`proof` untouched (creating a row with an empty verdict if none
exists, rather than inventing one). Non-control runs carry any earlier `control` value forward.
The tally line reports the boot outcome in control mode, since the per-site verdict is meaningless
there. Verified end-to-end: armed boot → `verdict=CRASH`; control boot → row still
`verdict=CRASH` with `control=RACE_OK`.

The table now encodes the whole discriminator in one line, so nobody has to redo this triage:

```
rva       verdict  control
0056f0a0  CRASH    RACE_OK    <- witness problem (port is fine)
0056f350  CRASH    CRASH      <- port problem
```

## Artifacts

- `log/crash_eip_0056f350.txt`, `log/crash_eip_0056f0a0.txt`
- `log/shadow_ab/shadow_ab_20260910_1432*_1x_0056f350.log`, `..._1434*_1x_0056f0a0.log`
- `re/parity/shadow_results.tsv` — verdict rows updated by each boot
