# The Ghidra project has referenced-but-undisassembled `.text` regions

**Date:** 2026-09-11
**Status:** measured on 12 addresses, all confirmed. Scope of the gap is **not** measured.
**Affects:** any uncertainty that concluded "not defined in Ghidra", "no callers found",
or "may be called via a function pointer".

## The finding

Ghidra's auto-analysis did not create functions for a set of `.text` addresses that the
binary **actually references**. Asking `decomp_pc.py` about one returns
`no function at or containing 0x…`, and asking `--datarefs` returns
`DATA in .text` / `undefined bytes (Ghidra has no data definition here)` with a nonzero
reference count. That combination — in `.text`, referenced, no function, no data
definition — is the signature.

Disassembling them with capstone against the anchored `MASHED.exe.unpatched` shows
ordinary function prologues:

| address | first instructions |
|---|---|
| `0x0048fe30` | `mov ecx,[esp+0xc]` / `fld [ecx]` / `fcomp [0x5d757c]` / `fnstsw ax` |
| `0x0049a2f0` | `sub esp,0xc0` / `push edi` / `lea eax,[esp+0x84]` / `push eax` / `push 3` |
| `0x0049a750` | `sub esp,0x160` / `mov eax,[0x7d3ff8]` / `push esi` / `mov esi,[eax]` |
| `0x004b49b0` | `mov eax,[esp+0x14]` / `mov ecx,[eax]` / `mov edx,[esp+0xc]` / `push ebx` |
| `0x004b4bb0` | `mov eax,[esp+0x14]` / `mov edx,[eax+4]` / `mov ecx,[eax]` / `push esi` |

Stack-frame setup, arguments read from `[esp+N]`, callee-saved registers pushed. These
are functions, not data and not padding.

Also in this class, found earlier the same day and disassembled in full: the **call
sites** at `0x00450a87`, `0x00452e5d`, `0x0054364a` and `0x0049a817`. Each is a plain
direct `call`, and each sits in a region with no containing function — which is why the
callee showed "no callers" even though the call is completely ordinary.

Twelve addresses confirmed in total. Nine more returned the same signature and were not
individually disassembled: `0x004ccef0`, `0x004ccf00`, `0x004ce6f0`, `0x005aead0`
(in `.text`), and `0x005ceb10` (in `.rdata`, so a different case — likely a constant
with no data definition rather than code).

## Why it matters for the trackers

Several uncertainty rows drew a conclusion from Ghidra's silence:

- **U-4398** and **U-5302** hypothesised function-pointer dispatch because the call graph
  was empty. Both are plain direct calls. Resolved 2026-09-11.
- **U-5648** recorded "function at 0x00555910 not defined in Ghidra". It is defined now,
  and its C2 plate still carries the stale warning
  *"Plated from the raw listing — Ghidra defines NO function at 0x00555910 in this
  read-only clone"*. That note is wrong and should not be trusted.

**The rule:** Ghidra showing no function or no callers is evidence about the *Ghidra
project*, not about the binary. Confirm against capstone before recording it as a
property of the code. The discriminator for genuine callback dispatch is whether **any
CALL reference exists at all** — U-5648 is a real callback because its only reference is
a `DATA` xref from the store that installs it.

## The gap, MEASURED

`re/tools/ghidra_scripts/UnanalyzedRefs.java` (new, read-only) iterates every reference
destination and keeps the ones in executable memory with no containing function:

```
py -3.12 re\tools\decomp_pc.py ...            # not this; run the script directly:
analyzeHeadless <pool> <slot> -process MASHED.exe -noanalysis -readOnly \
  -scriptPath re\tools\ghidra_scripts -postScript UnanalyzedRefs.java out.tsv
```

**4,612 distinct `.text` addresses are referenced but have no containing function**,
across 6,194 references. All in `.text`; none in any other block.

| reference type | count |
|---|---:|
| CONDITIONAL_JUMP | 3,273 |
| DATA | 2,108 |
| UNCONDITIONAL_JUMP | 590 |
| COMPUTED_JUMP | 173 |
| READ | 50 |
| **CALL** | **0** |

**Zero CALLs, and that corrects the framing above.** There are two separate phenomena
and they point in opposite directions:

- **(a) The call SITE is undisassembled.** This is the U-4398 / U-5302 case. The callee
  is a perfectly normal function; the *caller* sits in a missed region, so the callee's
  call graph looks empty. This script does not measure (a) directly, but a proxy does:
  **5,152 of the 6,194 references have a source that also has no containing function**,
  i.e. most of this set is internal traffic inside missed regions.
- **(b) The call TARGET has no function.** This is the "no function at or containing"
  case. Because **no CALL reference** reaches any of these 4,612 addresses, they are not
  reached by direct calls at all — they are reached by jumps (inside missed regions) or
  by having their **address taken**.

**The actionable subset is (b)-by-address-take: 826 distinct targets are DATA-referenced
from a REAL, analyzed function** — 982 such references, listed in
`re/analysis/plans/unanalyzed_addrtaken_20260911.csv`. An address in executable memory
whose address is loaded by analyzed code is a function pointer being installed. Every
one of those 826 is a callback Ghidra never turned into a function. That is where the
missed-function population actually lives, and it is a bounded, enumerable list rather
than a vague concern.

## What is NOT established

- **Why auto-analysis missed them.** Not investigated.
- Whether all 826 are genuinely function entries. Five were disassembled and all five
  were clean prologues, but 5 of 826 is a sample, not a census.
- Whether the master project (`Mashed.gpr`) differs from the read-only pool clones here.
  All observations are from pool clones.

## REPAIRED 2026-09-11 — 501 functions created in the master project

Owner approved the master-project write. Done with
`re/tools/ghidra_scripts/CreateMissedFunctions.java`, which refuses an address unless
every guard passes (`SKIP_NOT_EXEC`, `SKIP_HAS_FUNCTION`, `SKIP_MID_INSTR`,
`SKIP_DEFINED_DATA`, `SKIP_NO_CODE`) and has a dry-run mode.

**The 826 figure above was too high, and the guards are what showed it.** Dry run against
a pool clone: 536 would-create, **290 `SKIP_DEFINED_DATA`** — 263 of those are `pointer`
data, i.e. jump tables and pointer arrays embedded in `.text`. Disassembling one
(`0x00402a24`) gives `mov eax,0xb1004027` / `daa` / `inc eax` — garbage. Those are
address-takes of **data**, not of functions, and creating functions there would have been
wrong. So the real population was **536**, not 826.

Applied to the master: **501 CREATED, 290 SKIP_DEFINED_DATA, 35 SKIP_NO_CODE, 0 FAILED.**
The 35 are the delta from the dry run — addresses in the "needs disassembly" subset where
`disassemble()` produced no instruction, which only shows up in the write path.

**Verified three ways, not by trusting the exit code:**

1. Re-ran `UnanalyzedRefs.java` against the master. Orphan addresses **4,612 → 2,750**,
   references **6,194 → 3,578**, address-taken-from-a-real-function targets **826 → 387**.
   The drop is far larger than 501 because creating a function absorbs every jump target
   inside its body.
2. Spot-decompiled three created functions from the master: all recover real signatures
   and bodies (`void FUN_00407670(int,int)`, a five-parameter `FUN_00407a90`, and
   `undefined4 FUN_0040bb10(undefined4,undefined4)` which is a two-line forwarder).
3. Ran `ghidra_pool.sh sync` (16 slots refreshed) and confirmed the ordinary
   `decomp_pc.py` path now returns the new functions.

**Backup before the write:** `Mashed.rep.bak-20260911-pre-createfn` + `Mashed.gpr.bak-…`
(80 MB, verified file-for-file, and added to `.gitignore` so they cannot be committed).
Preconditions checked first: no lock files, no Java process, no other session.

**Residual 387** address-taken targets still have no function — they are the
`SKIP_DEFINED_DATA` set plus the 35 that would not disassemble. Those need a judgement
per address, not a bulk pass.

## Repair option as originally written, superseded by the section above

Creating functions at these addresses in the master Ghidra project would fix the class
permanently and improve every future decompilation. That is a **master-project write**,
which this repo's operating principles require asking about before doing — other
sessions may be attached, and it changes shared state. Flagged, not done.

The read-only workaround is sufficient meanwhile: `re/tools/disasm_fn.py` gives
byte-level evidence at any address without touching Ghidra at all.
