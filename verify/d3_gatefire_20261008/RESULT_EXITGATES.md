# RESULT — phase 3 is an INTRO PLAYBACK that re-initialises every car, every frame

Date 2026-10-08. Follows `RESULT_PHASE3.md` §5 item 2, chosen by **USER DECISION (Mariano,
2026-10-08)**. **Static, read-only** — one batched `decomp_pc.py` run against a read-only pool
clone. No build, nothing run. `original/` untouched. No C-level.

Raw: `exitgates_decomp.txt`.

> **CORRECTED 2026-10-08 by [`RESULT_IMGCHECK.md`](RESULT_IMGCHECK.md).** `DAT_005f29b8` is
> **file-backed, value `0xff`**, not unobtainable. See that file §0(1).

## 0. Verdict first

> Phase 3 is not a bare timer. It is an **intro/animation playback** that, **every frame**,
> **re-initialises each alive car's entire vehicle record**. It ends when the playback runs out
> **and** a second flag is clear.
>
> That is why the original's AI shows `c4 = 0` for 683 calls: the cars are being **held in their
> spawn state**, not merely told not to accelerate.

## 1. The three bodies

**`FUN_004430b0`** — 6 bytes. `return DAT_00897fe0;`. A plain getter.

**`FUN_00405460`** — 223 bytes, **takes a signed step** (`param_1`; the decomp header renders it
`(void)` but the body reads it — Ghidra's C1 note calls it *"advances a playback cursor"*).

```c
if ((DAT_00639d70 != 0) && (DAT_00639d78 != 0)) {
    local_48[0] = _DAT_005cc950 / DAT_00803344;  local_48[1] = 0.75;
    FUN_004c1c80(DAT_00639d78, local_48);
    uVar1 = *(undefined4 *)(DAT_00639d78 + 4);
    if (DAT_00639d74 <= *(float *)(DAT_00639d70 + 0xc)) {   // cursor <= clip length
        FUN_00404fa0(local_40, DAT_00639d70, DAT_00639d74);
        ... FUN_004c1480(uVar1, local_40, 0); ...
        DAT_00639d74 = fVar2 * _DAT_005cc948 + DAT_00639d74; // advance the cursor
        return 1;                                            // still playing
    }
}
return 0;                                                    // finished, or never set up
```

So it returns **1 while an animation is still playing** and **0** once the cursor passes the clip
length — or if either handle (`DAT_00639d70` / `DAT_00639d78`) is null.

**`FUN_0046baa0`** — 569 bytes, `(uint param_1 /* player slot */)`. A **per-car record
initialiser**: it writes the slot id to record+0 and then zeroes or presets **~70 fields** across
the `0xd04`-byte record — several to `1.0f` (`0x3f800000`), one to `4.0f` (`0x40800000`), a
16-iteration loop writing `(0, -1.0f, 0)` triples, and an 18-iteration loop writing `0xffffffff`
at stride `0x10` dwords.

> **Stride caution, again.** The decompiler renders these `(&DAT_008815a0)[param_1 * 0x341]` —
> **dword-indexed**. The byte stride is **`0xd04`**, which the sibling expressions
> (`&DAT_00881740 + param_1 * 0xd04`) show directly in the same function. Third occurrence of this
> trap today.

## 2. So what phase 3 actually does

Each frame, `FUN_004102f0`:

1. on the first frame (`DAT_005f29b8 == 100000`) runs a one-shot init;
2. **calls `FUN_0046baa0(i)` for every slot that is non-zero in the slot-state table and alive** —
   i.e. **re-initialises each alive car's record, every frame**;
3. decrements the countdown global;
4. exits to state 4 when `FUN_00405460() == 0` (playback finished) **and** `DAT_00897fe0 == 0`.

**The exit is not the countdown reaching zero.** `DAT_005f29b8` is decremented but the transition
is gated on the **playback** ending. The countdown global is reset to `12000` on the way out.

## 3. What this means for the port

The port has **none** of this: no intro playback (`DAT_00639d70`/`d78` are image-pad), no per-frame
car re-init, no `DAT_00897fe0`. Its cars are free-running from frame 0, which is exactly the 683-call
deficit `RESULT_CALLWISE2.md` measured.

**It also sharpens all three sizings in `SCOPE_SUBSTATE.md` §3:**

- **(a) "timer stand-in" is now clearly insufficient as a faithful model.** The real exit condition
  is an animation finishing, not a timer expiring. A timer would reproduce the *duration* without
  the *mechanism* — acceptable only as an explicitly-labelled bridge, and `RESULT_PHASE3.md`'s
  "no longer a fabrication" should be read narrowly: modelling a *hold* is faithful in kind,
  modelling the *exit* as a timer is not.
- **(b)/(c)** now carry a dependency nobody had costed: phase 3 drives `FUN_0046baa0`, a ~70-field
  per-car record initialiser. Porting the state machine without it would leave cars unheld.

## 4. What is NOT claimed

- Not what the animation *is*. `DAT_00639d70`/`DAT_00639d78` are handles into `FUN_004c1c80` /
  `FUN_004c1480` / `FUN_00404fa0`, none of which were read.
- Not what `DAT_00897fe0` means, nor who writes it.
- Not which of `FUN_0046baa0`'s ~70 fields matter. They are enumerated in the decomp, not decoded.
- Not that the port's cars would behave identically with a hold — unmeasured.
- No C-level. Nothing executed.

## 5. Next

The `DEFERRED.md` row should now record phase 3 as: **intro playback + per-frame per-car record
re-init**, exit gated on playback completion and `DAT_00897fe0`, with `FUN_004102f0` (172 B),
`FUN_00405460` (223 B), `FUN_004430b0` (6 B) and `FUN_0046baa0` (569 B) as the named bodies — and
`DAT_005f29b8`, `DAT_00639d70`, `DAT_00639d78`, `DAT_00897fe0` as the globals that are dead or
unverified standalone.
