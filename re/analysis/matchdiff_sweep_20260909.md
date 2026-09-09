# TT-2 — operand-correspondence sweep, first full run (2026-09-09)

Follow-on from `matching_compiler_spike_20260909.md`. Runs the compiler-version-independent
half of that spike across every `RH_ScopedInstall`'d function and asks one question per row:
**does the port touch the same `.data` globals the original touches?**

Offline: no Ghidra at run time, no Frida, no running game.

    py -3.12 re/tools/matchdiff_sweep.py            # C3+C4 (default)
    py -3.12 re/tools/matchdiff_sweep.py --conf C2  # authoring pre-screen

## Result

| | |
|---|---:|
| registrations parsed (`RH_ScopedInstall`) | 1,333 |
| rows at C3/C4 | 1,130 |
| **PASS** | **931** |
| **FAIL** | **155** |
| SKIP (symbol mangled/static/absent, TU not in target) | 44 |
| `.data`-address correspondence over comparable rows | **85.7%** |
| advisory: immediate set differs | 453 / 1,086 |

Full per-row output: `re/parity/matchdiff_sweep.csv`.

## The gate was calibrated DOWN twice, and that is the main methodological result

Both times a high failure rate on **already-verified** code turned out to be a defect in the
measurement, not a defect rate in the port. Recording this because the tempting reading of run 1
was "we have 608 broken functions", and that reading was wrong.

**Run 1 — combined address+immediate set: 608 FAIL of 1,086 (44% pass).** Decomposed:
- **183** failures were purely **call targets**. The original encodes a call as `E8 rel32`, which
  carries no absolute operand at all; a port calling `reinterpret_cast<fn*>(0x00xxxxxx)`
  materialises the target as a literal. Comparing them compares *encodings*.
- **217** were **immediates-only**: constant folding and control-flow shape. The original builds
  an ARGB literal byte-by-byte into a stack buffer (`-0x3a,0x3d,0x4e`) where we write one packed
  dword (`+0xff3d3a98`); `xor r,r` vs `mov r,0` changes whether a `0` appears at all.

**Run 2 — gate restricted to `.data`: 155 FAIL (85.7% pass).** Section ranges from the anchored
PE (`.text 00401000-005cc000`, `.rdata 005cc000-005ea000`, `.data 005ea000-00914704`). This drops
two more classes on principle rather than by tuning:
- **jump tables** live in `.text` — a switch compiles to `jmp [idx*4 + <table>]` and the table
  address is position-dependent;
- **pooled float/string literals** live in `.rdata` — ours land in *our* `.rdata` behind a COFF
  relocation, which is masked, so they can never correspond.

`.data` is what a transcription defect gets wrong, and it comes from the source rather than the
optimiser. `.text`, `.rdata` and immediates are still computed and reported, as advisory rows.

**Controls still hold after both recalibrations** (non-degeneracy is proven, not assumed):
3/3 known-good C4 functions PASS; the injected wrong-base defect (`0x899f7c`→`0x899f80`) FAILs
the gate; the injected wrong-stride defect (`0x341`→`0x340`) FAILs the *advisory immediate row*.
Note the second: **the gate alone does not catch constant errors** — that is what the advisory
row is for, and it is why immediates are reported rather than discarded.

## Two further artifact classes, found in triage, deliberately NOT gated

- **loop vs unrolled.** We enumerate N addresses where the original walks a base register, so the
  originals' single base appears against our N members (`Bool0Out8a94e0`: `+0x8a94e4,0x8a94e8,0x8a94ec`).
- **loop-induction rewriting.** The compiler rotates a loop to index `1..12` against `base-1`.
  This is why C4 `0x00404ee0 SerializeToBuffer` reports `-0x7f0f54 +0x7f0f53`: the original stores
  `mov byte [ecx + 0x7f0f54], dl` with `ecx = 0..11`, ours addresses the identical bytes from
  `0x7f0f53` with the index shifted by one. **False positive, confirmed by reading both sides.**

## Real finding: U-9086 (a C4 row)

**`0x00404320 PerModeRenderMachine`** — flagged `data -0x7d3ff8 +0x7d4018`, then confirmed against
the original's own bytes:

```
00404335  a1 f8 3f 7d 00     mov eax, [0x7d3ff8]      ; load the POINTER
0040433a  8b 30              mov esi, [eax]           ; camera = **(0x7d3ff8)
0040437c  8b 0d f8 3f 7d 00  mov ecx, [0x7d3ff8]
00404385  ff 51 20           call [ecx + 0x20]        ; fn = *(*(0x7d3ff8) + 0x20)
```

The port (`Render/PerModeRender.cpp:119-131`) models `0x007d3ff8` as an **array in place**:
`GetCameraPtr()` returns `*(int*)0x7d3ff8` (one dereference short), and `SetRWState()` reads the
function pointer from `0x007d3ff8 + 8*4` = **`0x007d4018`**, an unrelated `.data` address (two
levels short). The source comment "`DAT_007d3ff8[8]` is a function pointer pointer at offset
8*4=32 bytes from base" is the wrong plate — `FUN_004335f0` and `FUN_0042bcb0` both use the
pointer form. 13 sites in this one function.

Evidence is **static** (original bytes vs port source); nothing was executed this session, so the
row is filed as an uncertainty rather than demoted. `U-9086` carries the resolution path.

## Standing limits (do not overclaim)

- A **PASS moves no C-level**. Identical operands with wrong control flow still passes. It means
  "no transcription defect detected", nothing more, and never substitutes for a `diff-original`
  Frida diff.
- A **FAIL is a candidate**, not a defect: four artifact classes above produce legitimate FAILs.
- 154 FAIL rows remain untriaged. That is the queue, not a bug count.

## Artifacts

- `re/tools/matchdiff_sweep.py` — the driver.
- `re/tools/ghidra_scripts/FuncBoundsPC.java` → `re/console/cache/func_bounds.csv` (5,812 function
  bounds, cached so the sweep needs no Ghidra session; regenerate only if the master is re-analysed).
- `re/parity/matchdiff_sweep.csv` — per-row results.
