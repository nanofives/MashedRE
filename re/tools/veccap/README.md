# veccap — captured-vector offline verification (2026-07-16)

Additions to the promotion pipeline so that C3/C4-style verification can iterate
**without booting the game for every check**. Live Frida diffs (`diff-original`)
remain the final promotion gate; these tools attack the iteration cost before it.

**Now registry-driven** (`veccap_registry.py`): one declarative entry per
function drives all three tools. Onboarded so far = the RW fast-sqrt family
(6 fns) + the first B5e solver-island pure leaves (3 fns / 2 new void kinds):

| function | rva | kind | notes |
|----------|-----|------|-------|
| Vec3Magnitude | 0x004c3ac0 | f_ptrN (3) | |
| FastSqrt | 0x004c3b30 | f_f | |
| FastInvSqrt | 0x004c3b90 | f_f | |
| Vec2Length | 0x004c3bf0 | f_ptrN (2) | |
| Vec2Normalize | 0x004c3c60 | f_out_in (2→2) | degenerate path calls RW error stubs |
| RwV3dNormalize | 0x004c39b0 | f_out_in (3→3) | degenerate path calls RW error stubs |
| FUN_00566830 | 0x00566830 | v_out_in (3→3) | B5e perp-vector; pure; synthetic-only |
| FUN_00565ef0 | 0x00565ef0 | v_out_2in (7+7→7) | B5e AABB min/max merge; out idx 3 = padding |
| FUN_00565fa0 | 0x00565fa0 | v_out_2in (3+3+scalar→7) | B5e AABB span; inflate scalar = last input |

Latest run (2026-07-17): **offline replay 9/9 PASS both modes; Unicorn differ
9/9 PASS**. The 3 B5e leaves are C1 on main / C2 on `r7/b5e-solver-island`;
their ported TU (`Collision/RwpSolverLeaves1.cpp`) was copied verbatim from that
branch so the replay build can compile it. veccap is **per-leaf bit-identity
evidence toward C3, NOT the diff-original promotion gate.**

To onboard a new function: add an entry to `veccap_registry.FUNCS` (rva, export,
signature kind, source TU) and, if it reads new statics, to `STATIC_READS`. The
capture/replay/unicorn tools pick it up with no code change **when it fits an
existing kind**. Signature kinds:
`f_f` (float→float), `f_ptrN` (float*→float, n reads), `f_out_in`
(out,in→float, compares return AND out-buffer), `v_out_in` (VOID, out,in →
compare out-buffer only), `v_out_2in` (VOID, out,in1,in2[,scalar] → compare
out-buffer only; `n_a` = in1 float count, in2 is the contiguous slice after it,
`scalar: True` consumes the last packed float as a by-value cdecl arg). The
export string in the registry **must match the C++ symbol exactly, case-included**
(the replay linker resolves by name; capture/unicorn resolve by RVA). Functions
whose degenerate branch calls live-only stubs set `degenerate_stubs: True`; those
vectors are skipped offline and **counted**, never silently dropped. Out-buffer
slots a function leaves untouched (e.g. AABB padding index 3) are seeded with the
`0xcc` capture sentinel in all three tools so they compare equal.

## History — the three pilots

All three were first proven on `0x004c3ac0 Vec3Magnitude` before generalization.

## Pilot 1 — captured-vector offline replay  ✅ WORKS

1. `capture_vectors.py` — spawns its own MASHED (PID-tracked kill), snapshots the
   memory regions the function reads (RW globals slot + the 16 KB fast-sqrt LUT
   that RwEngineOpen builds at runtime), records real argument vectors at the
   menu (brief Interceptor, auto-detach — hot-path rule respected), and takes
   ground-truth returns by calling the original in-process (`NativeFunction`
   with `float` return handles the x87 ST0 convention). One JSON out.
2. `replay_offline.py` / `replay_offline.cpp` — builds a 32-bit exe that links
   the ported TU **unchanged** (hook framework stubbed), reconstructs the
   captured regions in-process, and bit-compares every vector. Two modes:
   - `relocated` — LUT placed inside the port validator's legal window; tests
     the core transcription. **Result: PASS 873/873.**
   - `faithful` — captured addresses reproduced; tests the port's behavior for
     the real heap layout. **Result: FAIL 511/873 — real finding, see below.**

Address-space notes (hard-won):
- The replayer is `/BASE:0x10000` with a referenced image-pad array so the exe
  IMAGE spans the low game range [0x10000, 0xb80000); the fixed game addresses
  the ports read (RW selector 0x007d3ff8, .rdata statics 0x005d757c) land in our
  own committed image pages — no allocation lottery. Only FAITHFUL-mode captured
  HEAP addresses (high, ~0x03exxxxx) are mapped dynamically; high user space is
  uncontended. (`MEM_RESERVE` is 64 KB-granular; `MapAt` reserves the aligned
  granule before committing a free page.) The pad MUST be referenced or `/O2`
  strips it — that was an early "pad vanished" bug.

### Finding VECCAP-1 — RESOLVED 2026-07-16
`Vec3.cpp`'s LUT-root validator (and the same pattern in `RwSqrt.cpp`,
`RwV3dNormalize.cpp`, `RwV2d.cpp`) rejected any RW-globals/LUT address above
`0x00b40000`. Live captures show the RW engine instance and LUTs are heap
allocations above that bound (`0x013ba528`/`0x02b7a528` across runs) → the
ports silently took their CPU fallbacks, which are NOT bit-identical
(511/513 non-zero vectors differed). Menu-scenario live diffs can't see it
because menu-time inputs are all zero-vectors.

**Fix:** shared `Math/RwLutGuard.h` — VirtualQuery readability over the slot
and full table span (any placement OK) + exact content sentinels captured from
the live tables (both sqrt and inv-sqrt), so the standalone's garbage selector
chain can't false-pass. Result cached per (globals, offset) pair for the hot
paths. **Re-verified:** offline faithful replay 874/874 PASS; live in-process
A/B (`live_check_veccap1.py`) Vec3Magnitude / FastSqrt / FastInvSqrt each
513/513 PASS; both build targets compile and deploy.

## Pilot 2 — Unicorn emulator differential  ✅ WORKS

`unicorn_diff.py` — maps `original/MASHED.exe` at 0x400000 plus the captured
regions into Unicorn (QEMU x86), emulates a caller stub
(`call target ; fstp dword [result]`) per vector, and bit-compares against the
live-captured ground truth. **Result: PASS 873/873** — Unicorn's x87
(FLD/FMUL/FADD/FSTP + the LUT integer path) reproduced the live original
bit-for-bit. No game, no Frida, ~seconds for 873 vectors.

Caveat: one function so far. Before trusting the emulator as an oracle for
promotion evidence, validate on FSIN/FCOS-class transcendentals and a
float10-chained callee (the known x87 danger zones).

## Pilot 3 — scripted TTD record + offline query

- **Recording: BLOCKED for unattended use** — `ttd.exe -launch` requires
  administrative elevation (verified empirically, error 0x80070005). Unattended
  recording would need a one-time elevated Task Scheduler lane (like
  MashedRE-Hygiene) that the user must approve/set up once.
- **Querying existing traces: see pilot log** — four 2 GB MASHED traces from the
  user's 2026-06-17 TTD lane live in `log/ttd/`; queried offline via the WinDbg
  data model (`cdbX64 -z <trace.run> -c "dx @$cursession.TTD.Calls(...)"`).

## Files

| File | Role |
|------|------|
| `veccap_registry.py` | declarative per-function config driving all tools |
| `capture_vectors.py` | live capture (the only step that touches the game) |
| `replay_offline.py` + `replay_offline.cpp` | offline replayer (ported C++) |
| `unicorn_diff.py` | emulator differ (original machine code) |
| `live_check_veccap1.py` | in-process live A/B (real game, hooks bypassed) |
| `out/` | captures, packed vectors, built replayer (gitignored candidates) |

## Unicorn gotcha (documented for the next function)

Unicorn/QEMU caches translated basic blocks, so **self-modifying code is not
re-translated**. Do not patch an immediate inside the stub per vector — the
emulator reuses the first value. Place arguments on the stack (cdecl reads them
at `[ESP+4]`…) and keep the stub static (`call ; fstp`).

All steps run unattended end-to-end except TTD *recording* (elevation).
