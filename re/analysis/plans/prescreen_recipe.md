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

## Full-pool run — 96 of 187 screened before the driver stopped itself

`scripts/prescreen_batch.py`, 4 chunks of 24 (one hand-run + three driven):

```
c0  ok (3/3 probes)   exercised  8/24  (race-gated 3)
c1  ok (3/3 probes)   exercised 13/24  (race-gated 4)
c2  ok (3/3 probes)   exercised 14/24  (race-gated 8)
c3  VOID no probe fired (first_results_at=None)   DISCARDED
c4  VOID no probe fired (first_results_at=None)   DISCARDED
    stopping: consecutive void boots — likely a wedged driver
```

**96 screened, 49 exercised (51%), 20 race-gated, 47 never.** They were discarded rather than
merged — had they been trusted, 48 candidates would have been permanently mislabelled "never
exercised". **91 candidates remain.**

> ### CORRECTION — the void chunks were NOT a GPU wedge
> I read them as the documented ~15-boot d3d9 wedge and said a reboot was needed. **Wrong**, and
> the run order already disproved it: the 48-probe screen failed, a 24-probe screen **immediately
> after** succeeded, three more 24-probe chunks succeeded, then two failed. *A wedged driver does
> not heal itself for the next boot.* The failures track the **probe set**, not elapsed boots.
>
> Cause: `statenav` armed every counter **before `dev.resume`**, so 24–48 Interceptors ran through
> the entire menu navigation and the nav's fixed `time.sleep(1.5)` waits timed out — exactly the
> hot-path hazard CLAUDE.md documents. The failing chunks contain hot RenderWare functions
> (`0x004b40f0`, `0x004c7730`, `0x004c5860`).
>
> Fix: `MASHED_COUNT_LATE=1` arms the probes **after** race entry, so navigation runs at native
> speed. The chunk that "wedged" twice then navigated fine **with no reboot**. Cost: the pre-race
> baseline is lost, so race-gated/pre-race classification needs a separate menu pass.

### The 20 race-gated candidates

| RVA | size | subsystem | | RVA | size | subsystem |
|---|---|---|---|---|---|---|
| `0x005b0f40` | 15 B | audio | | `0x005b10a0` | 38 B | audio |
| `0x005b8080` | 15 B | audio | | `0x005b10e0` | 38 B | audio |
| `0x005b1080` | 18 B | audio | | `0x00421560` | 39 B | render |
| `0x00421980` | 26 B | render | | `0x0048fce0` | 39 B | particle |
| `0x0045bac0` | 28 B | powerups | | `0x0048fd10` | 39 B | particle |
| `0x00421960` | 30 B | audio | | `0x0048fd40` | 39 B | particle |
| `0x005aeed0` | 33 B | audio | | `0x004e4320` | 39 B | render |
| `0x0045bfa0` | 35 B | powerups | | `0x0046cc10` | 41 B | ai |
| `0x005af200` | 35 B | audio | | `0x0047d150` | 41 B | gameplay |
| `0x0041f290` | 36 B | gameplay | | `0x00412100` | 38 B | render |

Clusters worth taking as units: **audio** (8 race-gated, several 15–18 B), **particle**
(`0x0048fce0/fd10/fd40` are consecutive 39-byte siblings — almost certainly one family), and
**render** around `0x00421560..0x00421980`.

Two notes on individual rows:
- `0x0047d150` is the bounds-checked slot lookup that `SlotRecordPositionPtr` and
  `PlayerPositionPtr` call. It **is** exercised in-race — so the lookup runs, but through callers
  other than the two array accessors whose arrays were empty. Those two ports remain unverifiable
  in this scenario; the callee is fine.
- `0x005aeed0` wraps `WaitForSingleObject(handle, 0)`. It is exercised, but a zero-timeout wait
  **acquires** when signalled, so force-calling it mutates state. Do not batch it without a
  restore.

## COMPLETE — 187/187 screened

```
exercised           90 (48%)
  race_gated        20   (baseline available: fires only once racing)
  exercised_prerace 29   (also reachable at the menu — cheapest to verify)
  exercised_inrace  41   (late-armed runs; in-race by construction, no baseline)
never               97
```

### Void chunks: TWO independent causes, and I conflated them

1. **Interceptor overhead** — real, fixed. Counters armed before `dev.resume` ran through the
   whole menu nav and its fixed waits timed out. `MASHED_COUNT_LATE=1` fixed it; the chunk that
   had failed twice then ran with no reboot.
2. **Plain navigation flakiness** — pre-existing, unfixed, roughly half of chunks. Late arming
   cannot touch it.

I diagnosed (2) as "the nav lands on phase=2 instead of the racing phase 0" and added a
`MASHED_COUNT_GATE` that nudges confirm until a validated probe fires. **Wrong, and the nudge was
actively harmful.** Surfacing the child process's own log showed:

```
start-attempt 4: depth=4 phase=3        <- still in the MENU after 5 attempts
[gate] simulation NOT confirmed after 9 nudge(s), phase=3
FINAL: depth=3 phase=3                  <- depth walked BACKWARD, 4 -> 3
```

Nine blind confirms from a stuck menu moved it backward. The gate now refuses to nudge while
`phase == 3` and bails so the caller can retry the chunk.

**I could only see this because I stopped swallowing the child's stdout.** The driver had been
capturing it, so the `[gate]` line I had just added to diagnose the problem was invisible — an
instrument you cannot read is not an instrument.

## Second screen: SHAPE (`scripts/shape_screen.py`) — exercised ≠ diffable

The exercise screen says the scenario calls it. It does not say a synthetic A/B can safely call
it, and those diverge badly. Run both.

| class | total | direct | reg_arg | indirect | destructive |
|---|---|---|---|---|---|
| race-gated | 20 | **5** | 2 | 11 | 2 |
| exercised pre-race | 29 | **21** | 1 | 5 | 2 |

`destructive` is not a style objection. A synthetic A/B calls the target **twice per vector**:

```
0x005b8080  call dword ptr [0x005cc088]  ->  KERNEL32!CloseHandle(*(p+0xc))
0x005aeed0  call dword ptr [0x005cc090]  ->  KERNEL32!WaitForSingleObject(h, 0)
0x0049ff30  0x0049cc40                   ->  Enter/LeaveCriticalSection
```

Closing a live kernel handle, acquiring a signalled object, or taking a critical section under a
force-call corrupts the running game rather than measuring it.

### The menu-verifiable pool is the richer one

**21 direct candidates are exercised BEFORE the race** — they need no race scenario at all and go
through the ordinary `run_diff` menu-attach lane, which is cheaper, more quiescent, and avoids the
spawn-budget problem entirely. Smallest first: `0x00495110` (11 B, util), `0x0040d020` (21 B,
track), `0x0047b860` (22 B, input), `0x004039c0` (23 B, vehicle), `0x00491780` (23 B, render),
`0x0047b880` (25 B, input), `0x004b7fd0` / `0x004c0ed0` (31 B, render), `0x00494460` (32 B,
frontend), `0x004b9600` (33 B, input), `0x0048fef0` (35 B, particle), `0x0048bbe0` (36 B,
particle), `0x00494480` (38 B, video), `0x005a60e0` (38 B, audio), `0x0045d430` (39 B, util),
`0x00496940` (40 B, render), `0x00534920` (40 B, util), `0x00425b90` (41 B, frontend),
`0x0047a0f0` (59 B, track).

### Three detector defects found by checking it against real disassembly

Every one produced a **false** verdict, and `reg_arg` / `direct` are exactly the verdicts used to
skip or accept a candidate:

1. **Stopping at the first `ret`.** `0x0047d150` read as *"leaf (no calls), 18 B"* when it calls
   `0x0057c210` nine bytes later — a false clean bill.
2. **Over-correcting the body end.** Requiring 3 bytes of padding merged `0x005b0f40` (15 B, ONE
   nop, then a different function) with its neighbour and *invented* a call. The two errors are
   one-sided in opposite directions: truncating hides calls, over-running fabricates them.
3. **Calling a prologue save a register argument.** `push esi` / `push edi` mis-flagged
   `0x004c0ed0`, `0x0048fef0`, `0x005a9de0`, `0x0047a0f0`, `0x004c1be0`, `0x00495080` — six
   candidates that would have been skipped. Position is not the rule; **a push whose register is
   popped in the same body is a save**. Also fixed: EAX after a `call` is a return value, not an
   argument, and `xor r,r` is a zeroing idiom rather than a read.

After the fixes, `direct` in the pre-race class went **10 → 21**.

## Why this matters for throughput

The STATE pool at C2 with a plate and no registry entry is **187 rows** at 8–60 bytes. At the
observed 58% exercise rate that is roughly **108 viable candidates** reachable through one
scenario — and the batch lane verifies at ~20 hooks/min of in-race window. The pre-screen is what
turns that pool from a guess into a work-list.
