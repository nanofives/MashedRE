# STATE lane — session write-up (2026-07-28 → 29)

33 commits. Opened as *"is there a quicker way to fan out C2→C3 promotions in batches?"* and ended
with a three-screen pipeline, 187 candidates classified, and 3 promotions.

## Result

| | |
|---|---|
| Promotions | `0x0042fab0` SpriteSlotDispatch (tracker drift), `0x0047d150` SlotObjectField8, `0x0046cc10` VehicleFloatFieldAsInt |
| Counts | C2 4034 → 4030, C3 849 → 853 |
| Authored, not promoted | `0x004671d0`/`0x00467210` (GREEN but degenerate), `0x005b0f40` (not synthetic-verifiable), `0x0044b000`/`0x00407600`/`0x00421930`/`0x004b4050` (candidates the screens later disqualified) |
| New tooling | `prescreen_batch.py`, `shape_screen.py`, `semantic_screen.py`, `run_diff_scenario_batch.py` extensions, `out1_idx` arg_type |

## The pipeline

```
187  STATE pool (C2, plated, unregistered, 8..60 B)
 90  exercised in Quick Battle (48%)      screen 1 — prescreen_batch.py, 1 boot / ~24
 48  shape `direct`                       screen 2 — shape_screen.py, free
  5  semantically safe                    screen 3 — semantic_screen.py, free
  4  authored
  2  promoted with non-degenerate evidence
```

**~90% of even shape-clean candidates mutate state or are non-idempotent.** That is the
quantitative answer to the original question: the flat C2→C3 lane is not slow because of candidate
supply or harness throughput — the C2 pool is *procedures*, and a synthetic A/B structurally
cannot judge a procedure. Moving the remainder needs observation-based verification of the
installed hook, which is also the only route to C4.

## What the screens caught before anything was authored

`CloseHandle(*(p+0xc))` · `WaitForSingleObject(h,0)` · two `EnterCriticalSection` pairs · a
DirectInput device teardown · an asset re-loader · a timer read that would have false-RED'd a
correct port · a dirty-flag matrix sync that clears the flag it tests.

A synthetic A/B calls its target **twice per vector**. Each of those would have corrupted the game
rather than measured it.

## Harness defects fixed (each was producing wrong verdicts)

| defect | effect |
|---|---|
| Both-sides-identical crashes reported as RED | 3 hooks filed as "real port defects" that were null-pointer arguments |
| Both-errored row outvoting discriminating rows | 9 distinct bit-identical values labelled INCONCLUSIVE |
| Composite fingerprints not counted as distinct | every `out1_idx`/`cache_roundtrip` run read GREEN-DEGENERATE |
| `zero_arg` baseline outranking distinct spread | a good control discarded |
| Passive dwell; `--scenario race` returns at frame 0 | live state not yet built at the diff point |
| Counters armed before `dev.resume` | Interceptor overhead stalled menu navigation |
| Nav exits on "not the menu" rather than "racing" | ~50% of screening chunks void |

## Mistakes worth carrying forward

**Five confident wrong conclusions, all from unvalidated instruments.**

1. *"The scenario doesn't populate these arrays"* — assumed, never measured.
2. *"The game is hung"* — from a white screenshot. The window grab misses the D3D9 backbuffer; a
   stock run with a **provably** running race screenshots equally white.
3. *"The lane never reaches a race"* — from probes that were actually the results/round-end
   subset, all `frontend` in hooks.csv.
4. *"The GPU driver is wedged, reboot"* — **the user corrected this one.** The run order already
   disproved it: a 48-probe screen failed, a 24-probe screen *immediately after* succeeded. A
   wedge does not heal. Real cause was Interceptor overhead.
5. *"The nav lands on phase=2"* — it never left phase=3, and my nudge walked it backward 4 → 3.

The common fault is not the individual errors — it is **drawing a strong conclusion from an
instrument that was never validated**. A counter never seen to fire anywhere proves nothing when
it reads zero. The fix that broke the cycle was mechanical: attach three probes validated against
a known-good race (0 at title/track-confirm/start-attempt, 11,266 / 2,867 / 59,690 in-race) and
treat any run where they stay silent as VOID.

**Four unbounded body scans** produced confident wrong answers — a linear `capstone` sweep that
truncates at the first bad byte (cost a valid candidate by reporting zero callers), a shape screen
stopping at the first `ret` (a false "leaf, no calls"), its over-corrected replacement merging a
15-byte function with its neighbour, and a callee walker overrunning into neighbours and rejecting
all 22 candidates. Bound every body; use a byte search or an E8 scan, never a bare linear sweep.

**One refinement reverted on purpose.** Teaching the store detector that argument-derived pointers
are harmless out-params cleanly separated two known cases — and then cleared a mutator that loads
its pointer from `[esp+8]`. A false positive costs a missed candidate; a false negative certifies a
mutator and corrupts everything it produces. Not symmetric; the rule stays blunt.

## Standing gotchas for this lane

- `--scenario race` returns at race **frame 0**; use `--dwell` and drive input.
- The diff-point **screenshot is white in-race even when the race is running**. Never judge from it.
- Always carry the validated in-race probes; a run where they stay 0 is void.
- `MASHED_COUNT_LATE=1` for pre-screens, else Interceptor overhead stalls navigation.
- Nav flakiness remains ~50% per chunk and is independent of everything above.
- The `--repeat-first` control is trustworthy; the sentinel-delta column is confounded.
