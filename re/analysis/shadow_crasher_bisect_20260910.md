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
| `0x00421960`, `0x004219c0`, `0x00495fe0` | **all three genuine port defects**, three distinct faults (below) |

### The three Lane 2 generated ports — all real, and not one shared bug

Each crashes on **both** arms (armed and `--no-shadow` control), so all three are defects in the
installed port, not in the witness. That is the expected shape for `decomp2port.py` output: it is
mechanically generated, so it is the most likely to be wrong. Each fault is in **MASHED code
downstream of the port**, at a different place:

| RVA | EIP | faulting instruction | bad value |
|---|---|---|---|
| `0x00495fe0` | `0x00508bde` | `mov eax,[esi]` | `ESI = 0`, reads `0x0` |
| `0x004219c0` | `0x004216b0` | `mov eax,[ebx+0xf4]` | `EBX = 2`, reads `0xf6` |
| `0x00421960` | `0x00559cb3` | `mov edi,[eax+ebp]` | reads unmapped `0x1d9644f4` |

`0x004219c0`: a small integer (2) is in a struct-pointer slot. `0x00421960`: the fault follows a
bitset index computation (`shr ebp,5` / `shl ebp,2` / `and ecx,0x1f`), so a bitset base or index
is garbage.

#### `0x00495fe0` faults **inside the `fix_joypad` boot-patch code cave** — flag this loudly

`0x00508bde` is 9 bytes into `0x00508bd5`, which CLAUDE.md documents as `fix_joypad`'s cave.
Verified by diffing the two binaries at that address:

```
original/MASHED.exe            0x00508bd5  83ec6c    sub esp,0x6c
                               0x00508bd8  a138606100 mov eax,[0x616038]
                               0x00508bdd  50         push eax
                               0x00508bde  8b06       mov eax,[esi]     <<< EIP, ESI = 0
original/MASHED.exe.unpatched  0x00508bd5  cc         int3   (padding — no code here)
```

Two consequences:

1. **This crash involves our patched reference binary.** Anyone re-testing `0x00495fe0` needs to
   know a boot patch is in the fault path; it is not purely a port-vs-original question.
2. The cave's guard reads `[esi]` **before** testing anything, so a NULL `ESI` reaches it
   unguarded — it was written for the observed *garbage-pointer* case, not for NULL.
   **[UNCERTAIN]** whether NULL can arrive there without this port installed is **untested**, so
   this is not yet a claim that `patch_mashed_fix_joypad.py` is defective on its own.

Consistent with the timing: `0x00495fe0` dies in 4–7s, i.e. at boot, and its callers
(`0x004967e0`, `0x004976a0`) are in the input/boot cluster.

**Next step for all three:** these are `decomp2port.py` outputs and need per-function review
against the disasm, not a shared fix. `0x00495fe0` should be reviewed with the cave interaction
in mind.

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

### The runtime probe, built and run

Added a temporary loop trace to the port behind **`MASHED_TRACE_F350=1`**
(`RwpSolverCore10.cpp`, `F350Trace()` + one `fprintf` at the top of the loop body; nullptr and
one static test per iteration when unset). It writes `f350_trace.txt` to the process CWD, i.e.
`original/`. One traced boot produced **one line**:

```
k=0 bound=4 pfVar9=0F73A7E0 field[+0xc]=00000000 flags=00
```

So: the bound is **4** (the loop should run k=0..3), `puVar3` is **NULL** on iteration 0 — which
sends the body down the `(puVar3 == 0)` branch — and **only k=0 was ever logged.** The crash
happens *inside iteration 0's body*, before the k=1 line. That supersedes my earlier "k=5"
reading, which came from interpreting `EAX−EDI` on an older build and does not survive direct
measurement.

### The apparent aliasing bug is FAITHFUL — do not "fix" it

While reading iteration 0's body I found what looked like a clear defect: three sequential
normalisations where each later `sqrt` re-reads a component the previous line just overwrote —

```c
local_f0 = (1.0f / sqrtf(local_f0*local_f0 + local_ec*local_ec + fVar4)) * local_f0;
local_ec = (1.0f / sqrtf(local_f0*local_f0 + local_ec*local_ec + fVar4)) * local_ec;  // uses the NEW local_f0
fVar2    = (1.0f / sqrtf(local_ec*local_ec + local_f0*local_f0 + fVar4)) * fVar2;
```

**The original does exactly the same thing**, and the disassembly is unambiguous:

```
0x0056f8a4  fsqrt                  ; sqrt #1
0x0056f8a6  fdivr dword [0x5cc320] ; 1.0 / it
0x0056f8ac  fmul  dword [esp+0x10]
0x0056f8b0  fst   dword [esp+0x10] ; <-- STORES THE SCALED COMPONENT BACK
0x0056f8b4  fmul  dword [esp+0x10] ; squares the NEW value
0x0056f8be  fsqrt                  ; sqrt #2, over the already-scaled component
0x0056f8ca  fst   dword [esp+0x14] ; <-- again
0x0056f8d6  fsqrt                  ; sqrt #3
```

Three separate `fsqrt`s, with `fst` write-backs between them. It is a progressive
renormalisation, it looks wrong, and it is what the binary does. **Recording it explicitly
because the obvious "cleanup" — hoist one reciprocal length — would silently introduce a
divergence into a faithful transcription.** This is what the NO-GUESSING rule is for.

### Where `0x0056f350` stands

Ruled out, each against the disassembly: argument shape/arity, loop base (+0x10), the field
(+0xc), the stride (0x28), the re-read bound (+0xac), `local_78[27]` sizing against both callees,
the float loop counter (wrong, fixed, not the cause), and now the apparent aliasing (faithful).
Established: it dies **inside iteration 0** with `puVar3 == NULL` and `bound = 4`.

### Staged the trace, and the answer changed the nature of the problem

Added stage markers through iteration 0's body (a `F350_STAGE` macro, one per call site) so a
single boot would name the dying statement. One traced boot:

```
k=0 bound=4 pfVar9=10EBA7E0 field=00000000 flags=00  ENTER
  after the six deltas (local_e4=00000000 local_e0=10E2B60C)
  after b750 #1
  after b750 #2
  after fad0 #1 (param_2)
  after f1f0 #1          <-- last line; dies after this
```

So it dies after `FUN_0056f1f0((int*)param_1, local_78)`, with `local_e4` NULL (first `b750`
skipped) and `local_e0` live.

**Then it stopped crashing.** With the markers compiled in: **4 boots, 4× RACE_OK.** And the
discriminating run matters — with the markers compiled in but **`MASHED_TRACE_F350` unset**, so
the `fprintf` never executes, it is still **2/2 RACE_OK**. The logging is not what changes it;
**the presence of the call sites is**, because they change MSVC's register allocation and stack
frame.

**So `0x0056f350`'s defect is FRAME-LAYOUT DEPENDENT.** That is the most useful single fact
established about it, and it explains why every static comparison came back clean: the bug is not
in an expression, it is in memory layout.

### The obvious follow-up hypothesis, tested and REFUTED

Header note 3 records that `local_78[0x19]` and `[0x1a]` are deliberately left uninitialised "as
the original leaves them", and that `FUN_0056f1f0` copies `[0x1a]` **raw**. Consuming
uninitialised stack is exactly the shape that would be frame-layout dependent. Tested it: reverted
to the crashing baseline and added *only* `local_78[0x19] = 0; local_78[0x1a] = 0;`.

**3 boots, 3× CRASH.** Not the cause. (Experiment reverted — that initialisation would not be
faithful anyway.)

### Where it stands, and the next step

Ruled out, each against the disassembly or by experiment: argument shape/arity, loop base `+0x10`,
field `+0xc`, stride `0x28`, re-read bound `+0xac`, `local_78[27]` sizing vs `FUN_0056fad0`
(writes no further than dword `0xe`), the float loop counter (a real defect, fixed, not this),
the apparent aliasing (faithful — three `FSQRT`s with `FST` write-backs), and the uninitialised
`[0x19]/[0x1a]` slots (zeroed, still crashes 3/3).

Established: dies inside iteration 0 after `FUN_0056f1f0 #1`; `bound=4`, `puVar3=NULL`,
`local_e4=NULL`, `local_e0` live; and the defect follows the stack frame.

**Next:** frame dependence points at either an overrun into a local adjacent to `local_78`, or a
read of some *other* uninitialised local. `FUN_0056fad0` is cleared as the overrun source.
`FUN_0056f1f0`'s writes through its buffer argument are **not yet cleared** — a naive scan found
three `mov dword ptr [ebx], ebp` at displacement 0, but EBX is reassigned mid-function, so those
may not be the buffer at all. That needs a proper def-use trace of EBX across
`0x0056f1f0..0x0056f341`, not a regex. If `FUN_0056f1f0` writes past dword 26, `local_78[27]` is
simply too small and the victim is whichever local MSVC placed next — which would explain every
observation at once.

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

## `0x0055c2d0` — the one stack-only DIVERGENT, and it is the port's frame, not its arithmetic

Detail: `pages=o:0,n:0,diff:0 stack=2 ... ret=0 stk+03c x4` (+ one more sample with `stack=1`).
So 5 of 24 samples differ **only** in the caller-stack window, at `+0x3c`, while the function's
actual outputs (`*param_4`, `*param_5`) and its page writes agree on all 24.

Compared the two frames directly:

```
original 0x0055c2d0:   sub esp,0x10        ; 16-byte frame, loc[0..3] at [esp]..[esp+0xc]
                       (no pushes, no cookie)

port ?FUN_0055c2d0_impl @0x10049020:
                       sub esp,0x14        ; 20 bytes
                       mov eax,[10103AC0h]
                       xor eax,esp
                       mov [esp+0x10],eax  ; <-- /GS STACK COOKIE the original never writes,
                                           ;     landing exactly past the original's frame
                       ...
                       push esi / push edi ; the original pushes nothing
```

The port's frame is 4 bytes larger **plus** two saved registers, and it performs a `/GS` cookie
write at an address inside the original's frame footprint — a write the original does not make,
of a value (`cookie XOR esp`) that differs between the two calls by construction.

So the `stack=` channel of `RunTracked` is comparing a window that contains **the callee's own
scratch frame**, and the port's frame necessarily differs from the original's. `0x0055c2d0`'s
DIVERGENT is a **witness artefact**, not a port defect.

**Scope, checked rather than assumed:** splitting all 17 `DIVERGENT*` rows by which channel
diverges gives **exactly one** stack-only row (this one). The other 16 diverge in `pages`,
`fields` or `ret`, i.e. in real outputs — five are already `DIVERGENT_FLOAT10` (the parked
build issue, D-11070). So this is a single-row explanation, **not** a systematic reclassification,
and I am not claiming one.

[UNCERTAIN] which dword `+0x3c` is has not been pinned to the cookie specifically; the cookie is
the strongest candidate (written unconditionally, differs per call) but "5 of 24" would fit a
conditionally-written slot better. The verdict above does not depend on which: any port whose
frame layout differs makes the `stack=` channel unreliable, and the outputs agree 24/24.

## `0x0055bd80` — and the rule that came out of it: `RunTracked` + indirect dispatch

Caught the fault (`log/crash_eip_0055bd80.txt`): EIP `0x00564c8e`, `fld dword ptr [ecx+0x10]`
with **`ECX = 1`**, reading address `0x11`. `bytes_before_eip` decodes the prologue, so the
faulting function is `FUN_00564c80` (`sub esp,0x4c / mov edx,[esp+0x54] / mov ecx,[esp+0x58] /
push ebx / mov eax,edx`), and `[esp+0x58]` after `sub esp,0x4c` + one push is **arg2** — so
arg2 = 1 where a pointer is expected. `FUN_00564c80` is reached through `FUN_0055bd80`'s
volume-descriptor dispatch `call dword ptr [edx+0x10]` at `0x0055bdca`.

**The port matches the original everywhere checkable.** The original's three-way selection of
that argument —

```
0x0055bd8f  jne 0x55bdb7        ; flag A set  -> arg2 = param_2   (0x0055bdb7)
0x0055bd98  jne 0x55bdb7        ; flag B set  -> arg2 = param_2
0x0055bda0  je  0x55bdb3        ; param_2==0  -> arg2 = param_1   (0x0055bdb3 mov eax,esi)
0x0055bda9  call 0x4c4600       ; else        -> arg2 = the call's result
```

— is exactly what Ghidra's comma-in-condition idiom reproduces
(`if ((A==0) && (B==0) && (iVar1 = param_1, param_2 != 0)) iVar1 = FUN_004c4600(...)`), including
the short-circuit that leaves `iVar1 = param_2` when either flag is set. Argument order at the
`FUN_004c4600` call matches the original's `push eax / push esi / push ecx` too, and the port
declares it `uint *` so the return is not truncated. **And the `--no-shadow` control boots
clean**, so the installed port is fine.

### The screen, which refuted my first explanation

I was about to write "indirect dispatch through a runtime table cannot be A/B'd". Screened the
lane instead: **11** shadow sites make a runtime-dispatched indirect call, and the split is
**not** by the dispatch —

| witness kind | sites | outcome |
|---|---:|---|
| `Run` (return value only) | 9 | **8 of 8 sampled are CLEAN 48/48**; the 9th (`0x005729a0`) is NO_SAMPLES, never called |
| `RunTracked` (pages + caller stack window) | 2 | **2 of 2 problematic** — `0x0055bd80` CRASH, `0x0055c2d0` DIVERGENT |

So indirect dispatch is fine when the witness is bounded. The rule that survives is narrower and
better supported (8/8 vs 0/2):

> **`RunTracked` is the wrong witness for a function that makes a runtime-dispatched indirect
> call.** `Run` compares one return value — bounded no matter where the dispatch goes.
> `RunTracked` compares page writes plus the caller stack window *and runs the body twice*; for a
> callee chosen at runtime from a data table, neither the write set nor the re-entrancy can be
> bounded in advance.

`0x0055c2d0` is the mild form of the same thing (its divergence is the port's own `/GS` frame,
above). `0x0055bd80` is the severe form.

**Disposition:** `0x0055bd80` should leave the lane. Converting it to `RunRegion` is not
available — the region would have to cover whatever the dispatch target writes, which is exactly
what is unknowable. [UNCERTAIN] why arg2 is specifically `1` on the second pass is not pinned;
the verdict rests on the control being clean plus the witness/kind split, not on that value.

## Artifacts

- `log/crash_eip_0056f350.txt`, `log/crash_eip_0056f0a0.txt`
- `log/shadow_ab/shadow_ab_20260910_1432*_1x_0056f350.log`, `..._1434*_1x_0056f0a0.log`
- `re/parity/shadow_results.tsv` — verdict rows updated by each boot
