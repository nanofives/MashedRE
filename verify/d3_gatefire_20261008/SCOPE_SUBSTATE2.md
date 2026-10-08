# SCOPE — D-11073's two open reads, answered. **The hold does NOT need the camera-path module.**

Date 2026-10-08. Task (B) from the session kickoff: D-11073's two open reads. **Static reading
only** — `decomp_pc.py` against a read-only pool clone, plus a PE image check. **Nothing was run,
nothing ported, no C-level, no tracker mutated.** `original/` untouched.

Raw: `substate_scope_decomp.txt`, `substate_scope2_decomp.txt`, `substate_coord_decomp.txt`,
`substate_datarefs.txt`, `substate_datarefs2.txt`.

## 0. Verdict first — and it corrects `DEFERRED.md`'s D-11073 row

> **Read 1 — who calls `FUN_004430a0(0)`?** `FUN_00445aa0`, at **two** sites. `FUN_004102f0`, the
> other caller, passes **`1`** — it *sets* the flag, it does not clear it.
>
> **Read 2 — what do `FUN_004053d0` / `FUN_00405400` load the clip from?** **Neither loads
> anything.** Both are pure setters. The handle comes from `DAT_00657448`, read at exactly one
> site — and `DAT_00657448` has **no writer anywhere in the image**.
>
> **Consequence, and it is the point of this doc:** D-11073's row says a faithful port of
> `FUN_004102f0` alone "would exit 3->4 on the first frame and produce **NO HOLD**", so phase 3
> "requires the camera-path module at 0x004053d0..0x00405540". **That is wrong about the
> mechanism.** `FUN_004102f0` **sets the exit flag itself**, and the flag alone blocks the exit
> regardless of the clip. What the hold actually needs is **one dword write** —
> `DAT_005f29b8 = 100000` — and that write is already inside `FUN_004111c0`, the sub-state machine
> the row is about.

## 1. The exit test, read literally

`FUN_004102f0` (0x004102f0, 172 B), tail:

```c
if (DAT_005f29b8 == 100000) {        // 0x004102f0  CMP [0x005f29b8],0x186a0
    FUN_00448700(0,0);
    FUN_004430a0(1);                 // <-- SETS the exit flag
    FUN_0040e590();
    FUN_0040d470(1);
}
...
iVar2 = FUN_00405460();              // cursor advance, reads DAT_00639d70
if (iVar2 == 0) {
    iVar2 = FUN_004430b0();          // exit-flag getter, reads DAT_00897fe0
    if (iVar2 == 0) {
        DAT_0063ba8c = 4;            // <-- the 3->4 exit
        DAT_005f29b8 = 12000;
    }
}
```

The exit needs **both** `FUN_00405460() == 0` **and** `FUN_004430b0() == 0`. So a non-zero flag
blocks the exit on its own, and the clip's value becomes irrelevant to *whether* phase 3 holds.
The clip governs `FUN_00405460`, i.e. the *other* conjunct — and the camera motion.

## 2. The seed is already in the state machine

`DAT_005f29b8`, 14 references, **8 writers** (`substate_datarefs2.txt`):

| site | in | value |
|---|---|---|
| 0x0040ff03 | `FUN_004111c0` | `0xff` |
| **0x004100d3** | **`FUN_004111c0`** | **`0x186a0` = 100000** |
| 0x00410291 | `FUN_004111c0` | `0x2ee0` |
| 0x0041036a | `FUN_004102f0` | `EDI` |
| 0x00410391 | `FUN_004102f0` | `0x2ee0` |
| 0x004103f0 / 0x004104c0 | `FUN_004103a0` | `ESI` / `0` |
| 0x00410910 | `FUN_00410860` | `ESI` |

`FUN_004111c0` is the sub-state writer D-11073 already names. **The 100000 seed is its own write**,
not a separate module's. Image value of `DAT_005f29b8` is **`0x000000ff` = 255**, file-backed at
fileoff `0x1f29b8` (verified against both `MASHED.exe` and `MASHED.exe.unpatched`) — so the
`== 100000` arm is correctly NOT taken at load, which is why the row's "exits on frame 1"
observation is right about the *symptom* while wrong about the *cause*.

## 3. The clip chain, end to end

```
DAT_00657448  --read @0x004270bd--> FUN_00426e10 --> FUN_004053d0(DAT_00657448, FUN_004671a0(0))
                                                        |
                                    DAT_00639d70 / d74 / d78
                                                        |
                              FUN_00405430, FUN_00405460 (the cursor advance)
```

- `FUN_004053d0(p1,p2)`: `if (p1 != 0) { d74=0; d70=p1; d78=p2; } else { d70=0; d78=0; }` — a
  setter, no loads.
- `FUN_00405400()`: zeroes d70/d74/d78 — a setter, no loads. Its one caller is `FUN_0040cfd0`.
- `DAT_00639d70`: 3 writers (both setters above), 3 reads (`FUN_00405430`, `FUN_00405460` ×2).
  No other reference.
- **`DAT_00657448`: BSS tail (`.data` off `0x06d448` ≥ RawSize `0x04d000`, zero at load), and
  Ghidra finds exactly ONE reference in the whole image — the read itself. `WRITES: (none)`.**

So on the **original**, taken at face value, `FUN_004053d0` is called with `p1 = 0` and takes the
`else` arm, leaving the clip handles at zero.

**[UNCERTAIN] — and this is the one that matters.** Ghidra's reference analysis proves *addressing
mode*, not absence of a writer: a store through a computed base or a pointer would not appear
(memory `findoffset-blind-to-computed-bases`, `offset-grep-misses-dword-index`). `DAT_00657448`
sits 4 bytes below `DAT_0065744c`/`44d`/`44e` and 8 below `DAT_00657450`/`454`, all used as a group
in `FUN_00426e10` — a struct that something may populate wholesale. **Resolution:** read
`0x00657448` and `0x00639d70` out of a live original during phase 3 with
`re/tools/orig_rampwatch.py` (ReadProcessMemory, no injection). One run, decides it.

## 4. The exit flag, end to end

`DAT_00897fe0`, 6 references:

- **1 direct writer**: `FUN_004430a0` (`MOV [0x00897fe0],EAX`) — a 10-byte setter.
- **1 direct reader**: `FUN_004430b0` — the getter.
- **4 address-taken sites** (`PUSH 0x897fe0`): three in `FUN_00448220`, one in `FUN_00448700`.
  Those can write it indirectly. Note `FUN_004102f0` calls **`FUN_00448700(0,0)`** immediately
  before `FUN_004430a0(1)` — so the set is bracketed by a function that also holds the flag's
  address. **[UNCERTAIN]** what `FUN_00448700` does with it; not read here.

**Callers of `FUN_004430a0`, with their arguments:**

| caller | arg | site / guard |
|---|---|---|
| `FUN_004102f0` | **1** | under `DAT_005f29b8 == 100000` (§1) |
| `FUN_00445aa0` | **0** | guarded by `fVar3 < _DAT_005ce18c` |
| `FUN_00445aa0` | **0** | inside a loop over `&DAT_007f1042`, stride `0x4c`, while `< 0x7f12a2`, firing when `*pcVar13 != '\0'` |

`0x007f1038` is `Ai::kCtrlBlockBase` in the port, so that second clear walks the **per-car control
blocks** and clears the exit flag when a per-car byte at `+0x0a` is non-zero.

## 5. What this does to the cost estimate

**It lowers it, and it moves it.** The row costs phase 3 as "the camera-path module at
0x004053d0..0x00405540 — materially larger than a state-function port". Per §1-§2 the **hold**
needs:

1. `FUN_004111c0`'s write `DAT_005f29b8 = 100000` at `0x004100d3` (entry into phase 3), and
2. `FUN_004102f0`'s own `FUN_004430a0(1)` — already inside the function the row calls a 172 B
   coordinator.

The camera-path module governs the **clip and the motion**, not the hold. A port of (1)+(2)
predicts a **static hold** — phase 3's duration without its fly-in.

**This is a static prediction, not a measurement.** What would falsify it:

- `FUN_00445aa0` running every frame during phase 3 and taking either clear path, which would
  cancel the flag immediately. **Not established here** — `FUN_00445aa0` is 2,579 B and was read
  only at its two call sites. This is the first thing to check before costing the work.
- Side effects of `FUN_0040e590` / `FUN_0040d470(1)` / `FUN_00448700(0,0)`, none of them read here.
- The per-frame decrement `DAT_005f29b8 -= (unaff_EBX * 0x44c) / 0x3c` reaching the `FUN_004103a0`
  / `FUN_00410860` writers. `unaff_EBX` is a decompiler artefact — an uninitialised register read,
  so the real function takes a frame-delta in `EBX` that the decomp does not show. **[UNCERTAIN]**,
  and it means the hold's *duration* is not readable from this decompilation.

## 6. Owed tracker correction (NOT applied here)

`DEFERRED.md`'s D-11073 row carries, as a **MEASURED** claim:

> a faithful port of `FUN_004102f0` ALONE would exit 3->4 on the first frame and produce **NO
> HOLD**. Porting phase 3 therefore requires the camera-path module at 0x004053d0..0x00405540 that
> WRITES those handles

The first sentence is right (at load, `DAT_005f29b8 == 255 != 100000`). **The inference in the
second is not** — the flag, not the handles, is what gates the exit, and `FUN_004102f0` sets the
flag itself. Correcting that row is a `re-classify` transaction, not a hand-edit, and it is left
for whoever picks the row up.

## 7. What is NOT claimed

- **No C-level**, nothing run, nothing ported. Static reads plus a PE offset check.
- Not that porting (1)+(2) is sufficient — §5 lists three unread dependencies and two
  `[UNCERTAIN]` markers.
- Not that `DAT_00657448` is never written — §3 states exactly what the reference count does and
  does not prove, and names the one run that would settle it.
- Not that phase 3's **duration** is known. §5's third bullet says why it is not readable here.
