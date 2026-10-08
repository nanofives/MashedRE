# RESULT — `W-TOOK` PASSES: branch 2 fires 103 times. And the scored window CANNOT SEE IT.

Date 2026-10-08. Pre-registration: `PREREG_WIRE.md`. Supersedes `RESULT_WIRE.md`'s `W-TOOK` FAIL,
which was a harness artefact of mine. No C-level. `original/` untouched, nothing default-ON.

Raw: `ELIM_{round,noround}.gates.csv`, `NR_{off,on,on_r2}.{gates,step}.csv`, `NR_GATES.txt`.

## 0. Verdict first

> **`W-TOOK` PASSES.** With cars not eliminated, branch 2 fires **103 times** for v1 at the wired
> call site, LOS passing on all of them. `NR_off` and `NR_on` hash differently (`8abfd6d1` vs
> `ddf92672`). `W-DET` passes.
>
> **`RESULT_WIRE.md`'s "0 firings in 18,481 calls" was MY harness**, not the branch: every run in
> that leg set `MASHED_ROUND=1`, and the port's elimination round had killed v1/v2 for 90% / 77%
> of the race.
>
> **`W-NOREG-E`/`-B` are NOT EVALUABLE** in the configuration where the branch fires, for two
> independent reasons (§3). This is the leg's real finding.

## 1. Round mode was the whole story

| | `MASHED_ROUND=1` | no round mode |
|---|---|---|
| v1 reached | 1,446 (10.4%) | **14,398 (100%)** |
| v2 reached | 3,177 (22.9%) | **14,398 (100%)** |
| v3 reached | 13,858 (100%) | **14,398 (100%)** |
| **v1 fires** | **0** | **103** |

`aib_alive(v)` returns `g_aib.alive[v]`, set at `TrackRenderer.cpp:4011`/`:4020` as
`(round_mode_ && !race_[v].alive) ? 0 : 1`. `race_[v].alive` is cleared only by
`race_cam_.EliminationCheck` (`:5138`, `:5601`, `:5656`) — Mashed's camera-distance elimination.
**It is gated entirely on `round_mode_`**, so with `MASHED_ROUND` unset no car is ever eliminated.

**This also corrects [`RESULT_ALIVE.md`](RESULT_ALIVE.md).** That leg concluded "the port
eliminates cars the original keeps alive … the port's visit set is NOT faithful". Wrong: the port
was running an **elimination round I had configured**, and the original capture was a QuickRace.
The elimination is faithful behaviour for the mode selected. `RESULT_ALIVE.md` carries the
correction inline.

**A second defect in that comparison, worth recording:** `aib_alive` reads a **port-local snapshot
field**, while `FUN_0046c7b0` reads the vehicle record at **+0x004**. `RESULT_ALIVE.md` compared
the two as if they were the same quantity. They are not.

## 2. The firings, located

v1's 103 firings occur at frames **558-660** — one contiguous episode, LOS passing 103/103.
v2 and v3 fire **0** times.

Against the original's **31 / 4 / 29** (`SCOPE_CALLWISE.md` §1, re-scored from `o_t3`): the port
concentrates all its firings on **v1** where the original spreads them, and the totals (103 vs 64)
are the same order but not agreement.

## 3. Why `W-NOREG-E`/`-B` cannot be read here — two independent reasons

**(a) The firings fall outside the scored window.** `ai_ctrl_window.py` scores car `v`'s calls
`[i0, i0+220)`; for v1, `i0 = 0`, so the window is **frames 0-219**. The firings are at
**558-660**. **0 of 103 are inside.** That is exactly why `NR_off` and `NR_on` produce
**identical** (e) digits and (b) bands while hashing differently — the wiring changes the run,
outside the region the criteria instrument.

**(b) The no-round configuration is not what the criteria are defined against.** The committed
baselines were established under round mode. Without it, (e) and (b) collapse on **both** arms:

| | committed baseline | no-round (both arms) |
|---|---|---|
| (e) v1 `ft_median_m0` | 2550.6 | **1496.0 (-41.4%) FAIL** |
| (e) v2 `launch` | 2053.0 | **1890.4 (-7.9%) FAIL** |
| (b) | v1/v3 PASS, v2 5 bands | **all three FAIL**; v2 `|steer|Med` 44.5 → **107.5** |

So there is no configuration in hand where branch 2 fires **and** the registered criteria apply.

## 4. What is established

- The wiring is **faithful, reached, and live**: it fires, writes `ctrl[5]=0xff` / `ctrl[0]=ctrl[1]=0`,
  and returns early, 103 times.
- `W-TOOK` **PASS**, `W-DET` **PASS**.
- The port's elimination behaviour is **faithful to the mode configured**, not a defect.

## 5. What is NOT claimed

- **No C-level.** No behavioural diff against the original was run.
- **Not that the wiring is harmless.** `W-NOREG` could not see it (§3a). A firing inside the scored
  window would be a different measurement.
- Not that 103 vs 64 is agreement — different car distribution, different scenario, and the port's
  window and the original's cover different phases.
- `W-SHAPE` is still unverified per-frame: the counters prove the branch fired, but no capture
  records `ctrl[4]`'s entry value on a firing frame.

## 6. Next

1. **Decide the scoring configuration.** The criteria assume round mode; branch 2 needs cars alive.
   Either re-baseline (b)/(e) without round mode, or find a scenario with both — a **user-level
   scope call**, since re-baselining touches the project's standing criteria.
2. `GF1-CALLWISE` is now closer than `SCOPE_CALLWISE.md` thought — the port *does* fire — but the
   windows must be made to overlap first (§3a).
3. `MASHED_WIRE_B2` stays default-OFF.
