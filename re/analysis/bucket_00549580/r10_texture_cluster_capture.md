# r10 — texture/raster cluster: the real capture, and what it measured

**Date** 2026-09-02 · **Lane** parent booted-race (solo) · **Anchor** `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`

Executes r8's own recommendation ("if promoting these, do it as a cluster under one real
texture-load capture rather than one scenario per row", `r8_texture_raster_neighbourhood.md:49-50`).
Nothing is promoted here. **No reimplementation exists for any of the 12** — all are C2 with
`status=new|mapped` and `file=` a `.md`. What this round delivers is the capture itself plus a
per-row measurement of *whether each row has a witnessable observable at all*, which is the
question that has to be answered before a port is worth authoring.

## The harness

`re/frida/scenario_launch.py --observe-texture-cluster` (additive, same shape as the
`--assert-course-load` extension: new rpc exports `texObserve` / `texResults`, new flag, no
existing behaviour touched). It attaches to the 12 RVAs during a normal scenario run and records,
per call, the arguments, the return value, and per-row dereferenced memory read **after** the call.
Output: `log/texture_cluster_observe.json` plus a per-row verdict table.

Per-row record caps keep the hot member (`0x004cc5e0`, 15262 calls/load) inside the Interceptor
budget; each row keeps counting after it stops recording.

**Counting was deliberately not treated as the deliverable.** A count table is exactly what got
`0x0047b9e0` refused on 2026-09-01: install proven, nothing separating "ran and was correct" from
"never ran". The verdict column is computed from distinct return values and distinct dereferenced
values, not from call counts.

## Finding 1 — all 12 fire on an ordinary track load; no provocation needed

`--track 3 --mode 10 --cars 4 --hold 12`, counters armed before the phase poke:

| RVA | track 3 | track 12 |
|---|---|---|
| `0x004d5340` | 203 | 257 |
| `0x004c76f0` | 197 | 251 |
| `0x004c7860` | 813 | 1196 |
| `0x004d5310` | 803 | 1186 |
| `0x004c7600` | 813 | 1196 |
| `0x004c77c0` | 223 | 275 |
| `0x004cc5e0` | 15262 | 18847 |
| `0x004cee90` | 787 | 1170 |
| `0x004cefd0` | 801 | 1184 |
| `0x004cdd00` | 804 | 1187 |
| `0x004c7650` | 4 | 4 |
| `0x004db2e0` | 7 | 7 |

r8 did not say how to provoke a texture load (`NOT IN NOTES`). It turns out nothing special is
needed: a plain `scenario_launch` run to phase 3 exercises the whole cluster.

## Finding 2 — CORRECTION to r8's stated mechanism: `FUN_0054fd60` does not run

r8 asserts the 12 "all fire in `FUN_0054fd60`'s own execution" (`r8:50`). Measured in the same run
that produced the table above, with a counter armed on it identically:

```
FUN_0054fd60 = 0        (all 12 callees ran; the parent ran zero times)
```

So the co-location claim is right in effect — one capture does cover the set, which is what the
recommendation was for — but the **explanation is wrong**. In a race load these 12 are reached
through some other path, not through `FUN_0054fd60`. Anyone reasoning from "they are the C2 direct
callees of `FUN_0054fd60`" should stop treating that function as the driver.

## Finding 3 — per-row observable verdict (two tracks, consistent)

`d_args` / `d_ret` / `d_obs` = distinct argument tuples / return values / dereferenced-value tuples
across the recorded calls.

| RVA | recs | d_args | d_ret | d_obs | verdict |
|---|---|---|---|---|---|
| `0x004d5340` | 24 | 8 | 2 | **14** | non-degenerate |
| `0x004c76f0` | 24 | 24 | 4 | 4 | non-degenerate |
| `0x004c7860` | 24 | 24 | **24** | 1 | non-degenerate |
| `0x004d5310` | 24 | 24 | 11 | 3 | non-degenerate |
| `0x004c7600` | 24 | 11 | 11 | 6 | non-degenerate |
| `0x004c77c0` | 24 | 17 | 20 | 1 | non-degenerate |
| `0x004cc5e0` | 200 | 17 | 2 | 1 | non-degenerate |
| `0x004cee90` | 24 | 2 | 9 | 1 | non-degenerate |
| `0x004cefd0` | 24 | 12 | 8 | 1 | non-degenerate |
| `0x004cdd00` | 64 | 14 | **1** | 1 | **no return observable** |
| `0x004c7650` | 4 | 1 | 1 | 1 | **under-exercised** |
| `0x004db2e0` | 7 | 7 | 7 | 1 | non-degenerate |

**10 of 12 have a measured, varying observable.** The two that do not are called out below rather
than rounded up.

## Finding 4 — three specific corrections to r8's per-row observable claims

1. **`0x004c7600` — r8 says "pure side-effect on the device; no scalar observable" (`r8:30`). That
   is wrong.** Measured: it returns the raster pointer (`d_ret`=11) *and* both flag bytes move.
   Sample records:
   ```
   args 0x4871448 ...  ret 0x4871448   +0x22=0x1  +0x23=0x3
   args 0x48714a0 ...  ret 0x48714a0   +0x22=0x0  +0x23=0x3
   args 0x48714f8 ...  ret 0x48714f8   +0x22=0x0  +0x23=0x2
   ```
   It is witnessable. The claim was inherited rather than measured; this round measured it.

2. **`0x004c76f0` — r8 warns "the no-call path returns constant 1 (degenerate)" (`r8:27`).** Both
   paths occur in one load, so the degenerate case does not dominate:
   ```
   +0x23=0x3   (high bit clear) -> ret 0x1     [the constant path]
   +0x23=0x82  (high bit set)   -> ret 0x7     [the device path]
   ```
   Reading the flag byte alongside the return is what makes the return attributable to a branch.

3. **The 7 Group-B rows for which r8 identifies NO observable (`r8:36-42`).** Five of them do have
   one, measured: `0x004c77c0` (d_ret 20), `0x004cee90` (9), `0x004cefd0` (8), `0x004db2e0` (7 over
   7 calls), `0x004cc5e0` (2 over 200). r8 was not wrong to leave them blank — it was reading
   decomp, not running the game — but they are no longer blank.

`0x004d5340`'s out-params are the cleanest observable in the set. They are genuinely written and
genuinely vary, which is the exact thing r8 predicted a synthetic run could not produce
("fake buf -> lock returns 0 -> out-params untouched"):
```
w=0x100 h=0x80 d=0x10 fmt=0x304     (256x128)
w=0x20  h=0x20 d=0x10 fmt=0x204     (32x32)
```

## The two rows that are NOT ready

- **`0x004cdd00`** (image destroy/free): 14 distinct argument tuples, return **constant `0x1`** over
  64 records on both tracks. There is no return observable. Promoting it off this capture would be
  a degenerate green. It needs a *designed* memory observable (the harness currently reads only
  after the call, which is the wrong side of a free) before a port is worth authoring.
- **`0x004c7650`** (raster pre-resize helper): exactly 4 calls per load, **identical arguments on
  both track 3 and track 12** — `0x48714a0, ...`. Constant input, so nothing about it is
  discriminated by this scenario. Either find a scenario that varies it or design a memory
  observable; do not author against this capture.

## Caveat on `d_args`, so the number is not over-read

The spec records a fixed 4 (or 6) dwords from the stack. For a function that actually takes fewer
arguments, the extra slots are caller stack junk, and that junk varies — which **inflates
`d_args`**. Visible in `0x004cdd00`, where `arg0` is constant `0x486e9e8` across every record and
the "variation" is entirely in `arg2` (values like `0x5cd648`, a `.rdata` address). `d_args` is
therefore a weak signal; `d_ret` and `d_obs` are the load-bearing ones. **Pin each row's real arg
count from its decomp before authoring** — do not take `nargs` in the spec as a finding.

## What this unblocks

Ten rows now have a recorded original-side witness and a scenario that reproduces it. That makes
the *port authoring* child-workable (verbatim transcription against a known observable), with only
the final booted verification staying in the parent lane. It does not promote anything by itself:
there is still no reimplementation for any of the 12.

Raw captures: `log/texture_cluster_observe_track3.json`, `log/texture_cluster_observe_track12.json`.
