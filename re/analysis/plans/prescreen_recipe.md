# Candidate pre-screen — "is this RVA exercised in the target scenario?" (2026-07-29)

**Run this BEFORE authoring a STATE port.** One boot screens dozens of candidates. Skipping it
cost a full session: four hand-picked candidates were authored, built, registered and diffed
before anything revealed that Quick Battle never calls any of them.

## Recipe

```bash
RVAS=$(head -24 re/analysis/plans/prescreen_candidates.txt | cut -f1 | paste -sd,)
MASHED_COUNT_RVAS="$RVAS,0x00470670,0x0047eb30,0x004233e0" \
  py -3.12 re/frida/statenav.py --round 70 --shot-dir verify/prescreen_<date>
```

`statenav.py` counts any number of RVAs in one boot, by RVA (so our inline JMPs cannot hide a
hit), and dumps a snapshot at each milestone plus a cumulative total at the end.

### Two rules that make the result trustworthy

1. **ALWAYS append the three validated in-race probes** — `0x00470670` VehicleControlUpdate,
   `0x0047eb30` VehiclePhysicsWorldStep, `0x004233e0` HeadingAtan2ToGameAngle. They read 0 at
   title / track-confirm / start-attempt and 11,266 / 2,867 / 59,690 during a race, so they
   certify the run reached one. **A pre-screen where they stay 0 is void.** The first attempt at
   this screen never left the frontend (`phase=3` throughout, `first_results_at=None`) and would
   have been read as "these 33 candidates are never exercised" — all it measured was boot+menu.
2. **Keep the batch to ~24 candidates.** `Interceptor.attach` on a hot path destabilises MASHED in
   about six seconds (CLAUDE.md), and a candidate's call rate is unknown before you measure it.
   48 + 15 statenav built-ins was the run that failed to reach a race; 24 + 3 succeeded. Whether
   that is causal is unproven, but the cheap batch is not worth risking a boot over.

### Reading it

Compare the **`[counts @start-attempt0]` snapshot** (pre-race baseline) against the final
`EXERCISED` list (cumulative non-zero):

| baseline | final | meaning |
|---|---|---|
| 0 | >0 | **race-gated** — fires only once racing; needs the race scenario |
| >0 | >0 | exercised, but also reachable at menu/boot — cheaper to verify there |
| 0 | 0 | **never exercised in this scenario** — do not author a port for it |

## First result — 24 candidates, Quick Battle (`prescreen_result_20260729.tsv`)

**14 of 24 exercised (58%)**, versus 0 of the 4 hand-picked candidates authored the day before.

**Race-gated (5)** — the highest-value STATE targets, since the race scenario is what the batch
lane provides:

| RVA | size | subsystem | name |
|---|---|---|---|
| `0x005b0f40` | 15 B | audio | FUN_005b0f40 |
| `0x005b8080` | 15 B | audio | FUN_005b8080 |
| `0x0045bac0` | 28 B | powerups | PowerupSlotDeactivate |
| `0x0041f290` | 36 B | gameplay | FUN_0041f290 |
| `0x0046cc10` | 41 B | ai | FUN_0046cc10 |

**Exercised pre-race too (9)** — `0x00495110` (11 B, util), `0x0040d020` (21 B, track),
`0x0047b860` / `0x0047b880` (input), `0x004039c0` (vehicle), `0x00494460` (frontend),
`0x00494480` (video, 749 calls before the race even starts), `0x0049f210` (particle),
`0x0047a0f0` (track). These can be verified at the menu, which is cheaper and more quiescent
than a race.

**Never exercised (10)** — `0x00404820`, `0x0041a3d0`, `0x0041b440`, `0x0041c2c0`, `0x004292d0`,
`0x00483a40`, `0x00489240`, `0x0048bc10`, `0x004cbb50`, `0x005571c0`. Several are visibly
scenario-specific in hindsight (`SkySecondaryDispatch`, `FontSys_ShutdownFontPool`, two boot
functions) — but hindsight is the point: this is measured, not guessed.

## Why this matters for throughput

The STATE pool at C2 with a plate and no registry entry is **187 rows** at 8–60 bytes. At the
observed 58% exercise rate that is roughly **108 viable candidates** reachable through one
scenario — and the batch lane verifies at ~20 hooks/min of in-race window. The pre-screen is what
turns that pool from a guess into a work-list.
