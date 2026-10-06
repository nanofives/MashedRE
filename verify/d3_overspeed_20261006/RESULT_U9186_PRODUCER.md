# Porting FUN_00442a60 does NOT unblock U-9186 — the producer is itself inert, blocked on two more unported layers

**MEASURED 2026-10-06.** No game run, no build, no code written, no C-level moved, `original/`
untouched. Original decomp via `re/tools/decomp_pc.py` (master Ghidra not opened). Follows
`RESULT_U9186_INERT.md`, which named porting `FUN_00442a60` (`Spectator::ComputeDistances`,
the per-car reference-distance producer that writes `0x008989b0`) as the prerequisite for the
AI over-speed command fix. **That prerequisite is itself blocked**, so no producer was written.

## What `FUN_00442a60` actually needs

`FUN_00442a60` clears `0x008989b0[0..3]`, then writes each as the planar distance from a
**reference car** to car `c` (`FUN_0046d4a0` positions + `FUN_004c3ac0` length — both live in
the standalone). The problem is choosing the reference car, which reads two inputs the
standalone does not produce:

| sub-input | what it reads | standalone state |
|---|---|---|
| `FUN_0040e180` (max-separation pair selector) | `*(int*)(PTR_005f2770 + car*0x34)` — the per-car slot-state table | `0x005f2770` is a **load-time `.data` pointer** (`== 0x005f2728` in the original) that the standalone **never loads**; its image-pad reads **0**, so the deref is near-null. The port already documents this and guards it (`AiStandalone.cpp:1387-1403`, `CarSlotStateSet` skips when `base == 0`). |
| `FUN_00408ad0` (per-car ordering metric) | `*(float*)(0x008a96ec + car*0x30c)` | written only by `FUN_00408610` (**unported**); `0x008a96ec` datarefs show **WRITES: (none)** reachable from the standalone, so it reads **0** for every car. |

So in the standalone `FUN_0040e180` would AV on the null table (or, guarded, select nothing),
and `FUN_00408ad0` returns 0 for all cars. The reference-car selection is degenerate, and
`FUN_00442a60` would write **zeros** to `0x008989b0` — exactly the state `RefDist` reads today.
`RefDist` stays 0, and **U-9186's two branches still never fire.** Porting the producer in
isolation is inert.

## The dependency chain, fully mapped

U-9186's over-speed command fix sits behind a stack of unported race-position bookkeeping,
each layer dead until the one above it is live:

```
U-9186 branches (FUN_00414a70 brake, FUN_004148b0 catch-up)
  need RefDist(c) = 0x008989b0[c]  LIVE
    ← FUN_00442a60 (producer, C2) — CALL SITE EXISTS (the race camera FUN_00446520,
      ported as Race/RaceCamera.cpp, calls it) — but it needs:
        ← FUN_0040e180 (C2, unported) reads the 0x005f2728 slot-state table
            ← 0x005f2770 → 0x005f2728 is a load-time .data table the standalone DOES NOT LOAD
        ← FUN_00408ad0 reads 0x008a96ec per-car progress
            ← written by FUN_00408610 (unported)
```

So the real prerequisite is not one function but **reconstructing the standalone's
race-position state**: (a) the per-car progress field `0x008a96ec` (its writer `FUN_00408610`,
or a standalone-appropriate progress derived from the splines the AI already drives), and
(b) the `0x005f2728` slot-state table (allocate it and point `0x005f2770` at it, then fill it).
Both are sizeable and neither ships a working feature alone — the "binding constraint is
wiring, not reversing" situation the ASI-only triage named, two layers deep.

## Recommendation

**Do not port `FUN_00442a60` now.** Guarded, it produces zeros (RefDist unchanged, U-9186
inert); unguarded, `FUN_0040e180` AVs on the null table. Either way it ships code that does not
change behaviour. The over-speed command fix (U-9186) requires a dedicated effort to
reconstruct per-car race progress + the slot-state table in the standalone, pre-registered on
its own, with a gate-fire counter proving `RefDist` goes non-zero and the branches fire near
36 / 64 before any behaviour is wired.

The over-speed is already **+26/+28/+24 %** on the shipped build (down from +41/+40/+34 % via
the car<->car contact), criterion (e) is MET, and (b) is at 5 failing bands. Weighed against
the size of the race-position reconstruction, the higher-value lanes are U-9191 item (b) (the
genuinely-physics car-1 heading residual) or a different subsystem — see `re/NEXT_SESSION.md`.
