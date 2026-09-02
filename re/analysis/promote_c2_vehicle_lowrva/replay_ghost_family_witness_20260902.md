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

### `0x00411ae0` Ghost::PlaybackTick — WITNESSED under a replayed human scenario

**This section supersedes the one below it, which reported this row as inert. That reading was
wrong and the error is instructive.**

Replaying the recorded human scenario `003-race-drive` under the same capture
(`replay_session.py --observe`) gives **writes on 48/48 calls**, and the observable is exact:

```
args [0x32, 0x1612, 0x1612]  ->  cursor +0x18 = 0x1612
args [0x32, 0x1644, 0x1644]  ->  cursor +0x18 = 0x1644
args [0x32, 0x1676, 0x1676]  ->  cursor +0x18 = 0x1676
```

`DAT_0063bb1c := arg1` exactly, stepping by `0x32` per tick (which is also `arg0`). Arg index 3
reads `0x4112d2`, a code address, so the real arg count is at most 3.

**Why the earlier run looked inert, and the method lesson.** In the auto-driven Time Trial the
arguments were `[0x0, 0x0, <float>]` — `arg1` was **0 on every call**, so the function performed its
write and stored the same `0` the cursor already held. The pre/post delta test reports "did not
move", which is true and yet the opposite of the conclusion I drew from it: **a write of an
unchanged value is invisible to a delta test**. The delta is necessary but not sufficient. Where a
row can write a constant, the argument that feeds the write has to be varied by the scenario before
"did not move" means anything — the same shape as needing coverage counters rather than a bare
GREEN count.

The remaining unexercised part is the Time-Trial-gated matrix interpolation, which still needs
`DAT_0063bb0c` (ghost) or `DAT_0063bb10` (best lap) non-null.

### `0x00411ae0` Ghost::PlaybackTick — superseded first reading (auto-driver only)

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

## The replay driver, and why it was worth wiring up

`replay_session.py` re-drives a recorded human's DirectInput keyboard state per frame, indexed by
`GetDeviceState` call number rather than wall clock, so a game state a human reached is re-reached
without a human. It now takes `--observe SPEC.json`, sharing
`re/frida/observe_block.js` with `scenario_launch.py` so the two capture drivers cannot drift (the
block was extracted from `scenario_launch.py` for this; the refactor was regression-checked to give
byte-identical results on the same spec).

That was the whole difference for `PlaybackTick`: the auto-driver pulses accelerate with no
steering and produced a constant-`0` argument, while a replayed human race varies it and the
observable falls straight out.

Note the four existing recordings (`001-nav-demo`, `002/003-race-drive`, `004-ramp-airborne`) are
all quick-race drives, and each `meta.json` carries a **different `exe_sha256`, none matching the
current anchor** — they were recorded against differently-patched binaries between 2026-06-20 and
06-22. `003-race-drive` still replays into a live race, so the divergence does not break replay, but
a new recording should be re-anchored.

## Authoring outcome (2026-09-02, later the same day)

### `0x00411ae0` — PORTED and promoted C2 -> C3

`mashedmod/src/mashed_re/Vehicle/GhostPlaybackTick.cpp`. Verbatim naked transcription of
`0x00411ae0..0x00411cd0`; the trampoline re-execs the single 5-byte stolen instruction
`MOV EAX,[0x0063bb24]` (`a1 24 bb 63 00`) and jumps to `0x00411ae5`, so no instruction is split.
Byte-verified against the fleet `ds:[imm]` note. Verified at 385 calls / 0 mismatches /
**moved 385/385**, with the slider2 self-test as a same-boot control at 3713 green calls.

Three arms remain unverified and are recorded in the `hooks.csv` row: the `DAT_0063bb24` override,
the clamp, and the entire Time-Trial-gated interpolation half. They unblock with `0x00411870`.

### `0x00411d90` — NOT AUTHORED, deliberately

After pulling the full disassembly (`0x00411d90..0x00411f29`) this is not worth authoring yet, for
three reasons that come out of the disassembly rather than out of caution:

1. **The evidence ceiling is one sample.** It runs exactly once per race, so an in-race A/B yields
   a single comparison per boot — and only on one of its two branches, since the disk-load arm
   needs `c:\toast\ReplayN.rep` and `DAT_0063bb2c` measured `0` (fresh alloc) every time.
2. **The A/B is hazardous.** It allocates (`FUN_00482930`) and loads from disk (`FUN_00483d10`), so
   a modded pass has to stub both. But the loaded pointer is immediately dereferenced —
   `0x00411ea6  MOV EAX,[EAX+0x174]` — and passed to `FUN_00411350`. A stub returning a fake buffer
   faults; one returning null takes a different branch. A clean single-side-effect invariant means
   running the original first and feeding the modded pass the pointers it produced, and a bug there
   corrupts the live replay control block rather than just failing a test.
3. It also carries a security cookie whose tail is `JMP __security_check_cookie` (`0x004a2be9`)
   rather than a `RET`, plus a vtable-indirect sprintf through `DAT_007d3ff8+0xc4`. All
   transcribable, none of it free.

Two acceptance designs that would make it worth authoring:
- **(a)** Put a `.rep` on disk so both branches fire; then the in-race A/B earns its cost.
- **(b)** Skip the A/B and do a **two-boot structural comparison**: stock original under the
  existing block capture, then a boot with the port installed as a full replacement, comparing the
  `DAT_0063bb04..DAT_0063bb2c` block. Weaker on purpose — the slot pointers are heap addresses that
  differ per boot, so this compares *structure* (which fields are null, `bb14 == bb04`, the `bb2c`
  branch flag, `bb18`) and not bit-identity.

## Harness finding: the full-hook-set replay lane is currently broken

A `--asi` replay of `003-race-drive` with the **full** hook set exits the game partway through.
This is **pre-existing, not caused by the new port** — verified rather than assumed: a rebuild with
`Vehicle\GhostPlaybackTick.cpp` removed from `asi_sources.rsp` exits identically, and the
known-good slider2 hook logs 3713 green calls under the same replay+asi combination. The
verification above is therefore scoped with `MASHED_HOOK_ONLY`. Worth its own investigation: it
means the full-hook-set replay lane cannot currently be used end to end.

## Net

- `0x00411d90` — measured observable, reproducible. The mode-2 create path is witnessed (block
  all-zero to populated) and the mode!=2 skip path is witnessed under the replay (returns 9, block
  untouched). The **disk-load arm remains unverified** without a `.rep` present.
- `0x00411ae0` — **witnessed**, `DAT_0063bb1c := arg1`, 48/48 under the replayed human race. The
  Time-Trial-gated matrix interpolation is still unexercised.
- `0x00411870` — trigger fully characterised, still never runs. This is the one genuine blocker
  left, and it needs a **completed Time Trial lap**.

Options for that last one, with the evidential cost of each stated:
1. **Record a human Time Trial lap** with `record_session.py`, then replay it. Faithful, no
   contrived state. Needs a person at the keyboard once; after that it is reproducible forever.
2. Poke `DAT_008991bc` to `0xb` and force `FUN_0040e350()` to 6. Cheap, but it **bypasses the very
   computation that decides the call** — the run would witness LapFinish's body while proving
   nothing about when it fires. C3-grade contrived state at best.
3. Place a `c:\toast\ReplayN.rep` on disk to light up `CreateOrLoad`'s load branch and
   `PlaybackTick`'s ghost branch. Helps two rows, does nothing for LapFinish.

Recording command for option 1, with the candidate set already pointed at this family and its live
caller:
```
py -3.12 re/frida/record_session.py --name tt-lap --cov 0x00411870,0x00411ae0,0x00411d90,0x00429310 --seconds 300
```
Then drive: main menu, **Time Trial**, any track, and complete **one full lap**. Replay it with
```
py -3.12 re/frida/replay_session.py re/scenarios/00N-tt-lap --observe re/frida/specs/replay_ghost_family.json
```
`coverage.tsv` will say directly whether `0x00411870` was hit.

Captures: `log/replay_ghost_observe_tt.json`, `log/replay_ghost_observe_tt_long.json`,
`log/replay_ghost_observe_pokelap.json`, `log/sector_probe.json`.
