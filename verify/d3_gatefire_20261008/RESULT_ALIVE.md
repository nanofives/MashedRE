# RESULT — the port ELIMINATES cars the original keeps alive. That is upstream of the whole lane.

Date 2026-10-08. Follows `SCOPE_CALLWISE.md` §5 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**. **READ-ONLY on the game** — `ReadProcessMemory` poll, no injection, no hooks of our
own, no writes, no build. `original/` untouched. No C-level.

Raw: `o_alive.csv`, 734 samples at 10 Hz, `o_t1`'s scenario (`--hold 60`), `slotptr` non-null
734/734.

## 0. Verdict first

> **Restricted to the racing phase, the original keeps all four cars alive and the port does not.**
> `FUN_0046c7b0` reads `1` on **100%** of substate-6 samples for v0/v1/v2 and 77.2% for v3. The
> port runs `ControlStep` for v1 on **10.4%** of frames and v2 on **22.9%**.
>
> **This is upstream of the GATEFIRE lane entirely.** It explains the empty visit set directly:
> `ControlStep` barely runs for v1/v2, so branch 2 has almost no opportunity to fire regardless of
> whether its logic is right.

| car | ORIGINAL (substate 6) | PORT | |
|---|---|---|---|
| v0 | **100.0%** (206/206) | n/a — not stepped | — |
| v1 | **100.0%** (206/206) | **10.4%** (1,446/13,858) | **port kills it** |
| v2 | **100.0%** (206/206) | **22.9%** (3,177/13,858) | **port kills it** |
| v3 | 77.2% (159/206) | **100.0%** (13,858/13,858) | **port over-keeps it** |

## 1. The filter is what makes this readable, and the raw numbers mislead

Over **all** 734 samples the original reads alive on v0 39.0%, v1 41.7%, v2 76.7%, v3 34.9% —
which looks like "the original also kills cars". That is an artefact: the capture's substate mix is
`9`×236, `6`×206, `0`×134, `5`×74, `7`×53, `11`×23, so **most of it is post-race and standings**,
where cars are legitimately not alive.

The port's stepdump is `substate == 6` on **100%** of its rows, so substate 6 is the only phase
where the two are comparable. Restricting to it **reverses the apparent conclusion for v1 and v2**:
from "both sides kill cars" to "the original keeps them alive throughout and the port does not".

(Memory `band-on-speed-compares-different-moments` — the same failure mode, caught by filtering
rather than by assuming the capture was homogeneous.)

## 2. Stride caution, recorded because it would have been silent

`FUN_0046c7b0` is rendered by the decompiler as `(&DAT_008815a4)[v*0x341]` — **dword-indexed**.
The byte stride is **`0xd04`**. Polling `0x341` *bytes* would read a different field for every car
except v0 and return a confident wrong answer. The tool carries this reasoning in a comment, not
just the constant (memory `offset-grep-misses-dword-index`).

## 3. What this does and does not settle

**Settles:** the port's uneven `ControlStep` visit set (`RESULT_CALLWISE.md` §1) is **not**
faithful. The original's visit set would be ~100% for v1/v2. So `RESULT_WIRE.md`'s "0 firings in
18,481 calls" was measured on a race where two of the three AI cars are mostly dead.

**Does not settle:**

- **Why** the port eliminates them. Unmeasured. Could be the rule engine, a collision/damage path,
  or elimination scoring — none of which this poll touches.
- Whether fixing it makes branch 2 fire. The firing condition also needs `Prog(v) == 0` and
  `6.5 < Prog(0)` to co-occur; more visits is a necessary condition, not a sufficient one.
- v3's inverse discrepancy (port 100% vs original 77.2%). Smaller, opposite in sign, and
  unexplained.
- Sampling asymmetry, stated plainly: the original figure is a **10 Hz time poll** (206 substate-6
  samples) and the port figure is an **exact per-frame call count** (13,858). Both are "fraction of
  the racing phase", but they are not the same instrument, and the original's 206 samples are a
  thin basis for a 100.0% claim.

## 4. What this means for `GF1-CALLWISE`

`SCOPE_CALLWISE.md` §3 flagged the risk that the port's 220-call window "covers a different phase
of the race". **It is worse than a phase difference**: for v1 the port needs 220 calls and only
produces 1,446 across the whole race, drawn from the 10% of frames where the car is alive. The
original's 220 calls come from a car alive the entire time.

**Recommendation: do not run `GF1-CALLWISE` until the alive discrepancy is understood.** A `0 vs
64` result would be real but uninterpretable — it would be measuring the elimination bug, not
branch 2.

## 5. Next

1. **Find what eliminates v1/v2 in the port.** That is now the highest-value question in this lane,
   and it is upstream of U-9186 rather than part of it. It may also bear on criterion (b), since
   `v2` is the car whose bands fail.
2. `GF1-CALLWISE` after that, not before (§4).
3. `MASHED_WIRE_B2` stays default-OFF, faithful and inert.
