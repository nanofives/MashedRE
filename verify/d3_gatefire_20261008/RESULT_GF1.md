# RESULT — branch 2's body LANDED. `GF1-REACH` 100%, `GF1-FIRE` 0 — and both blockers are now named.

Date 2026-10-08. `PREREG_GATEFIRE.md` §4, chosen by **USER DECISION (Mariano, 2026-10-08)**, with
the exit-site follow-up also user-directed. **RAN.** Default-OFF behind `MASHED_GF1`.
No C-level. `original/` untouched. Nothing seeded by this leg.

Raw: `GF1{on,off}.gates.csv`, `GF1b.gates.csv` (exit sites), `GF1step.csv`.

## 0. Verdict first

> **The body is written, consolidated, and runs on 100% of rows — and it returns 0 on all of them.
> The exit-site histogram turns that into two named blockers, neither of which is the body.**

| gate | verdict | figure |
|---|---|---|
| `GF1-CONSTS` | **PASS** | 6.5 / 5.5 / 6.0 / 4.0 re-read from `MASHED.exe.unpatched`; limit table all 64 ints, matching the 2026-10-03 **live** read to every digit |
| `GF1-FTOL` | **RESOLVED** | truncation, from original-side measurement — not assumed. §2 |
| `GF1-REACH` | **PASS** | `lt_ran = 1` on **53,992/53,992** |
| `GF1-FIRE` | **0** | `lt_ret != 0` on **0/53,992**. Pre-registered rule: report the next blocker, do not widen scope |
| `rva-lint` | **NEW=0** | 122 known, count **unchanged** — consolidation, not a second body |

## 1. Where it returns — measured, not guessed

```
site  99  limit <= RankAt (the table gate)   51348   95.10%
site 105  no active car (last == -1)          1324    2.45%
site 126  TimerAt < 0x1195                    1320    2.44%
```

The cross-tab against `bias374` is perfectly clean:

| `bias374` | exit | rows |
|---|---|---|
| 1 / 2 / 3 / 4 | **99** | 2404 / 4804 / 4804 / 39336 |
| 0 | **105** | 1324 |
| 0 | **126** | 1320 |

So the 51,348 rows at site 99 are **exactly** the `bias374 != 0` rows — the ramp pushes the table
index to 11-14, where every entry is `0`, so `0 <= RankAt(0)` returns. That is `RESULT_M5.md`'s
already-named blocker (no race sub-state machine → no `0x007f0ff8` reset → the ramp), and it is
**not new**.

The 2,644 rows that got **past** the table gate are all `bias374 = 0`, and they split exactly by
whether H3's `refdist` is live: **1,324 with `refdist == 0` → site 105**, **1,320 with
`refdist != 0` → site 126**. Two distinct downstream blockers.

## 2. `GF1-FTOL` was load-bearing and is resolved by original-side evidence

`DAT_0089a360` is `2.5` — exactly the truncate-vs-round-half ambiguous case:

- truncate(2.5) = 2 → index **10** → `limit = 1` → the `:99` gate **passes**
- round-half-away(2.5) = 3 → index **15** → `limit = 0` → the `:99` gate **fails**

`AiLeaderTimer.cpp:22-23` had deliberately forwarded to the original `__ftol` "rather than assume
truncate-vs-round", and the standalone cannot forward. Resolved from measurement:
`RESULT_WITNESS.md:92` records the original's "only index is **10** (`bias374 = 0`, `iVar1 = 2` on
all 512 calls)" while reading `2.5`. So `FUN_004a2c48` truncates here. The 2,644 rows reaching past
`:99` confirm it in-game.

## 3. Blocker A (site 105) — the slot-state seed holds `2`, the consumer wants `1`

`FUN_004148b0:104` is `for i in 0..3: if (E470(i) == 1) last = i`, and `:105` returns 0 when
`last == -1`. Measured, same table and formula as the dump's `slot_state` column:

| car | `E470` returns |
|---|---|
| v0 | `0` on 13,498 |
| v1 / v2 / v3 | **`2`** on 13,498 each |

**No car ever reads `1`.** The values present are `{0, 2}`, so the loop can never mark a car and
`last` stays `-1`.

This indicts **`MASHED_SLOTSTATE_SEED`** (D-11072 leg C), which seeds `0x005f2770 → 0x005f2728`:
the value it produces is `2`, which `FUN_0040eee0` legitimately wants (the `0040e470` plate's
`U-1300` records that parent using `E470(i)==2 && FUN_0046c7b0(i)==1`), but which
`FUN_004148b0:104` does not.

**[UNCERTAIN], and it is the next measurement:** what the **original's** slot-state table holds per
car during a race is **unmeasured**. The witness never logged `E470`'s returns — `idx364 = -1`
meant the `:94` call site was skipped, and it did not instrument `:104`. Until that exists it is
not established whether the seed's `2` is wrong or whether the original also reads `2` here and the
discrepancy lies elsewhere. `re/tools/orig_rampwatch.py` can poll
`*(*(int**)0x005f2770 + 0x34 + v*4)` directly — one capture.

## 4. Blocker B (site 126) — a self-referential timer, downstream of A

`TimerAt` (`0x0089a4c8 + v*0x74`) reads **`0` on all 1,320 site-126 rows**, and `0x1195` = 4,501,
so the gate can never pass. `TimerAt` is `FUN_004148b0`'s **own** state — its only increments are
`:108` and `:116`, both behind the `fVar4 == kZero` branch that blocker A closes.

**So B is not independent: it is downstream of A.** Clearing A may clear B by letting the timer
accumulate. That should be measured, not assumed, once A moves.

## 5. What was built

- **`Ai/AiLeaderTimer.cpp` is now a SHARED TU**, not a second body at `0x004148b0`. `rva-lint`'s
  allowlist is a burn-down list, so a new duplicate pair was not an option; the `#ifdef
  MASHED_STANDALONE` arms follow H1b's `VehicleRecordPtr.cpp` precedent. The `.asi` arm is
  **byte-for-byte unchanged**, so its C3 evidence is undisturbed — including `LT_EXIT`, which
  compiles to `((void)0)` outside the standalone.
- Standalone arms written for `FUN_0040e470` (14-byte getter), `FUN_00442cc0`
  (reads H3's `0x008989b0`), `FUN_0046d4a0` (H1b's `PtrCompute881ec8`), the four thresholds, the
  64-int limit table, and `Ftol`.
- **A correction to `PREREG_GATEFIRE.md` §2:** it listed `FUN_0040e470` as "RESOLVED — exe body
  `Frontend/MenuStateMachine.cpp`". That file holds only a **call-through to the absolute
  address**. `hooks.csv`'s `exe_file` named a TU containing a thunk — the same stale-column trap
  already flagged for `004148b0`, which I then walked into myself.
- The limit-table index is bounds-guarded; out-of-range returns `0`, the same value the blank table
  gave, so the guard **cannot manufacture a pass**.

## 6. What is NOT claimed

- **No C-level.** The body has never been compared against the original's returns call-by-call;
  `GF1-CALLWISE` is unrun because the port fires 0 times, so there is nothing to pair against
  `o_t1`/`o_t2`/`o_t3`'s 64.
- Not that the body is correct. It is faithful to the plate and every input is resolved, but a
  function that returns 0 on every row has demonstrated only that it runs.
- Not that fixing blocker A makes branch 2 fire — §4's chain is an expectation, not a measurement.
- Nothing is wired to `ctrl`. That remains `PREREG_GATEFIRE.md` §7's non-goal.

## 7. Next

1. **Measure the original's `E470` per car during a race** (§3). One `orig_rampwatch.py` capture
   decides whether `MASHED_SLOTSTATE_SEED`'s `2` is the defect or a red herring.
2. If the seed is the defect, that is a **D-11072 leg C** correction, not a GATEFIRE leg.
3. `bias374`/site-99 stays downstream of the race sub-state machine (`RESULT_M5.md`), which is its
   own `DEFERRED.md` row.
