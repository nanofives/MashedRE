# RESULT — BRANCH 2 FIRES. 201 times, one car, one episode — and the arms are not frame-aligned.

Date 2026-10-08. Follows `RESULT_SLOTPLACE.md` §6 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**. **RAN**, 2 arms. Default-OFF behind `MASHED_SLOT_PLAYER`. No C-level.
`original/` untouched, `.asi` code path untouched.

Raw: `GF1c_{on,off}.gates.csv`.

## 0. Verdict first

> **`GF1-FIRE` is non-zero for the first time in this lane: 201 rows at exit site 114, with the LOS
> conjunct passing on all 201.** The bridge did exactly what it was predicted to do — **site 105
> drained from 1,324 to 0** — and the chain ran on to fire.
>
> **Read the caveats before treating this as success (§3).** All 201 are **one car** in **one
> contiguous 201-frame episode**, and the two arms are **not frame-aligned**, so this is a
> behavioural change that has not been re-scored against criteria (b) or (e).

| | OFF | ON |
|---|---|---|
| `E470` per car | `0 / 2 / 2 / 2` | **`1` / 2 / 2 / 2** |
| site 99 (table gate) | 51,348 (95.10%) | 52,948 (95.52%) |
| site 105 (no active car) | **1,324** | **0** |
| site 123 (`TimerAt == 0`) | 0 | 303 |
| site 126 (`TimerAt < 0x1195`) | 1,320 | 1,980 |
| **site 114 (FIRE)** | **0** | **201** |
| `lt_fire` | 0 | **201** |
| frames | 13,498 | **13,858** |

## 1. The bridge works, and the mechanism is the predicted one

`MASHED_SLOT_PLAYER` sets the player's slot-state cell to `1`
(`FUN_0042b960`'s `FUN_0040e480(0,1)`), so `FUN_004148b0:104`'s
`if (E470(i) == 1) last = i` finally matches — on car 0, the player. Site 105 goes to **zero
rows**, exactly as `RESULT_E470.md` predicted. Nothing else about the gate chain was touched.

## 2. What the 201 firings actually are

- **All car 2.** Not spread across the three AI cars.
- **Frames 300-500**, contiguous — **one episode**, not 201 independent events.
- All at `bias374 = 0`, i.e. before the ramp leaves band 0 at frame 661. Consistent with §0's
  site-99 share: once the ramp moves, the table gate closes again.
- `lt_los` passed on **201/201**, so the LOS conjunct is not the limiter here.
- The leader XZ written is **one distinct value**, `(10.4092064, -22.8361263)`. That is coherent
  rather than suspicious: `last` resolves to **car 0, the player**, and the player is stationary in
  this harness — the same `vel = [0,0,0]` the original-side captures show.

**Why the episode ends** is visible in the body: `:113` does
`if (TimerAt > 0x2710) { TimerAt = 0; RankAt += 1; }` before returning 1. Once `RankAt` reaches
`limit` (= 1 at table index 10), `:99`'s `limit <= RankAt` is `1 <= 1` — true — and the gate closes
permanently. The new site-123 rows (303) and the grown site-126 (1,320 → 1,980) are the timer
states that follow. That is a self-consistent reading of the measured exits, **not** a claim that
it matches the original's cadence.

## 3. Caveats — these bound the claim

- **The arms are NOT frame-aligned**: 13,498 frames OFF vs **13,858** ON. The bridge changed the
  run. That is expected for a real behavioural change, but it means the ON/OFF percentages share no
  denominator and must not be differenced.
- **Criteria (b) and (e) were NOT re-scored.** No stepdump arm was taken with
  `MASHED_SLOT_PLAYER=1`. Until that runs, nothing is known about whether this helps or harms the
  AI metrics this lane exists to fix.
- **`GF1-CALLWISE` remains impossible.** The original's **64** firings are over a 220-call window
  per car in a different scenario; 201 rows over a 13,858-frame capture is not the same
  measurement. Comparing the two numbers directly would be the "schedule-derived counts" error.
  Pairing them needs the port driven through the original's scenario, which is a separate leg.
- **One car, one episode** (§2) is not obviously the original's shape, where branch 2 fires 64
  times across the window. Unmeasured whether that is a difference or a scenario artefact.

## 4. What is NOT claimed

- **No C-level.** A firing count is not a behavioural diff.
- Not that the bridge is correct beyond the one cell it writes. It deliberately omits
  `DAT_007f1a0c = 1` and the `[playerBlockIdx, -1, -1, -1]` index pattern
  (`RESULT_SLOTPLACE.md` §4), both pre-existing divergences with their own rows.
- Not that branch 2 now behaves like the original's. §3.
- Not that this should go default-ON. It is a bridge over a menu path the standalone does not run,
  and it changes the race.

## 5. Next

1. **Take a stepdump arm with `MASHED_SLOT_PLAYER=1`** and re-score criteria (b) and (e). This is
   the first change in the lane that actually moves the race, so it is the first that can regress
   them. Until that exists the bridge should stay OFF.
2. Then decide whether the one-car/one-episode shape is a defect or a scenario artefact — which
   needs the port driven through the original's 220-call window.
3. Site 99 (95.52%) is untouched and still belongs to the race sub-state-machine row
   (`RESULT_M5.md`).
