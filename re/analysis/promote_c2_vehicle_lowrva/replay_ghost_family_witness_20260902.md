# Replay/ghost family — witnessing observables, measured

**Date** 2026-09-02 · **Lane** parent booted-race (solo) · **Anchor** `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`

Rows: `0x00411870` Replay::LapFinish, `0x00411ae0` Ghost::PlaybackTick, `0x00411d90`
Replay::CreateOrLoad. All three C2, **no reimplementation exists for any of them**. The task was to
name a witnessing observable per row before authoring. Nothing is promoted here.

The six existing notes propose **no** witnessing observable for any of the three (surveyed
2026-09-02; all three sections came back "NOT IN NOTES"). So the observables below are measured,
not inherited.

## The observable, and why it is the right one

All three write into one compact contiguous control block, `DAT_0063bb04 .. DAT_0063bb2c`
(44 bytes). Snapshotting that block **on both sides of the call** makes the delta the witness — a
post-only read cannot separate "ran and wrote this" from "this was already the value".

Measured layout during a Time Trial (track 0, 1 car), decoded from the capture:

| offset | global | observed | meaning (from the notes) |
|---|---|---|---|
| +0x00 | `DAT_0063bb04` | `0x15870338` | replay slot 0 ptr |
| +0x04 | `DAT_0063bb08` | `0x15870b80` | replay slot 1 ptr |
| +0x08 | `DAT_0063bb0c` | `0` | ghost (disk-loaded) replay ptr |
| +0x0c | `DAT_0063bb10` | `0` | best-lap replay ptr |
| +0x10 | `DAT_0063bb14` | `0x15870338` | current-lap ptr (= slot 0) |
| +0x14 | `DAT_0063bb18` | `0` | slot toggle index |
| +0x18 | `DAT_0063bb1c` | `0` | playback cursor |
| +0x1c | `DAT_0063bb20` | `0` | lap-finished flag |
| +0x20 | `DAT_0063bb24` | `0` | override flag (U-new1: purpose unestablished) |
| +0x24 | `DAT_0063bb28` | `0` | (U-1569: identity unestablished) |
| +0x28 | `DAT_0063bb2c` | `0` | has-recorded-a-lap |

Harness: `scenario_launch.py --observe-texture-cluster` with
`MASHED_OBSERVE_SPEC=re/frida/specs/replay_ghost_family.json`. The capture was generalised this
session to take an arbitrary spec file and to read **absolute** blocks pre/post, not just argument
dereferences. The spec carries a control row (`0x00411ce0` Ghost::SetupRender, already C3, ~9000
calls/race) so a run with no records is distinguishable from a run where the capture never armed.

## Per-row result

### `0x00411d90` Replay::CreateOrLoad — WITNESSED, observable named

1 call per race, and the block **moves on 1/1 calls**: all-zero before, populated after.
```
pre   00000000 00000000 00000000 00000000 00000000 ...
post  38038715 800b8715 00000000 00000000 38038715 ...   ret 0x15870338
```
The return value is slot 0's pointer and it matches `DAT_0063bb04`. **Observable: the 44-byte block
delta, with `DAT_0063bb2c` as the branch discriminant** — `1` = loaded `c:\toast\ReplayN.rep` from
disk, `0` = fresh alloc. Measured `0` here, i.e. the fresh-alloc branch, because no `.rep` exists.
Note that means **the disk-load branch is unexercised** and a port verified only against this run
would have that arm untested; say so rather than claim full coverage.

The path buffer `DAT_008a94a8` (U-1567, "exact format unknown") did **not** change across the call
in the fresh-alloc branch — consistent with the sprintf living on the disk-load path.

### `0x00411ae0` Ghost::PlaybackTick — RUNS, but writes nothing in this scenario

9016 calls in a 150 s Time Trial. The block **never moved**, across 48 consecutive recorded calls,
in three separate runs. Return `0x0` every time. This is not a defect in the capture — the same
capture recorded `CreateOrLoad`'s write in the same run.

The reason is visible in the block: `DAT_0063bb0c` (ghost ptr) `= 0` and `DAT_0063bb10` (best-lap
ptr) `= 0`. The notes gate the ghost branch on the former and the best-lap branch on the latter, so
with neither present there is nothing to play back and the function is inert.

**Observable named** (for when the state exists): the cursor `DAT_0063bb1c` advancing, clamped to
`*(DAT_0063bb10 + 0x174)`, plus which of `bb0c`/`bb10` is non-null to attribute the branch. Args
are recorded as `[0x0, 0x0, <rising float>, ...]` — the rising float at index 2 is the input time;
arg index 3 reads `0x4112c0`, a code address, so the real arg count is at most 3 and must be pinned
from decomp before authoring.

**Do not author against this capture.** It would produce a green that proves only the early-out.

### `0x00411870` Replay::LapFinish — NEVER RUNS; exact trigger now known

0 calls in every scenario tried: quick race (track 0, 4 cars), Time Trial 45 s / 60 s / 150 s, and
Time Trial with `--poke-lap 0:1 --poke-delay 25`. `--poke-lap` writes the lap counter at
`0x008a9620+car*0x30c+0x28`, which is a different structure, and does not reach this path.

`CallersPC.java` gives exactly 2 call sites:
```
00429530 | FUN_00429310 | UNCONDITIONAL_CALL
0040e56e | FUN_0040e560 | UNCONDITIONAL_CALL
```
Runtime counts in a Time Trial: `FUN_00429310` = 3633 calls, `FUN_0040e560` = **0**. So the live
caller is `FUN_00429310`, and the call is conditional inside it. Its decompilation gives the gate
verbatim:

```c
if ((DAT_008991bc == 0xb) && (iVar2 = FUN_0040e350(), iVar2 == 6)) {
    ...
    FUN_00411870(1);          // <- Replay::LapFinish, param_1 = 1
    FUN_0040e360(7);
}
```

`DAT_008991bc` is the sector counter that `FUN_00429310` advances 1..11 against thresholds of
`DAT_008991bc * 10` compared to `FUN_00408ad0()`'s return. **Measured over a 150 s Time Trial, it
reaches only `1`** (distinct values across 4000 recorded calls: `{0, 1}`). Ten more sector
advances are needed.

That is consistent with `FUN_00408ad0()` returning lap progress rather than wall time: the launcher
drives by pulsing control 4 (accelerate) with no steering, so the car never gets round the track.
**The blocker is the driver, not these functions.**

## Net

- `0x00411d90` — has a measured observable and a scenario that reproduces it. Port is authorable;
  the disk-load arm will be unverified without a `.rep` present.
- `0x00411ae0` — observable named but **not reachable in a meaningful state** until a best lap or a
  ghost exists. Depends on `0x00411870` having run at least once.
- `0x00411870` — trigger fully characterised, still unreachable. Needs a completed Time Trial lap.

The family is therefore a **dependency chain gated on completing one Time Trial lap**. Ways to get
one, none of them taken here because they are a user-level call:
1. Drive a real lap and record it with the existing `re/frida/record_session.py` /
   `replay_session.py` pair, then replay it as the scenario driver. Faithful, no contrived state.
2. Poke `DAT_008991bc` to `0xb` and force `FUN_0040e350()` to 6. Cheap, but it **bypasses the very
   computation that decides the call**, and the resulting run would witness LapFinish's body while
   proving nothing about when it should fire. C3-grade contrived state at best.
3. Place a `c:\toast\ReplayN.rep` on disk to light up `CreateOrLoad`'s load branch and
   `PlaybackTick`'s ghost branch without needing LapFinish. Only helps two of the three rows.

Captures: `log/replay_ghost_observe_tt.json`, `log/replay_ghost_observe_tt_long.json`,
`log/replay_ghost_observe_pokelap.json`, `log/sector_probe.json`.
