# RESULT — blocker A located to ONE CELL: the seed leaves the PLAYER slot at 0; the original has 1

Date 2026-10-08. Follows `RESULT_GF1.md` §7 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**. **READ-ONLY on the game** — `ReadProcessMemory` poll, no injection, no hooks of our
own, no writes, no build. `original/` untouched. No C-level.

Tool: `re/tools/orig_rampwatch.py` (extended with the `FUN_0040e470` pointer-chase).
Raw: `o_e470.csv`, 672 samples, `o_t1`'s scenario (`--hold 60`). `slotptr` non-null on **672/672**.

## 0. Verdict first

> **Three of four cars agree. The one that disagrees is the one branch 2 depends on.**

| car | ORIGINAL | PORT | agree? |
|---|---|---|---|
| **v0 (player)** | **`1`** on 581, `0` on 91 | **`0`** on 13,498 | **NO** |
| v1 | `2` on 563, `0` on 109 | `2` on 13,498 | yes |
| v2 | `2` on 455, `0` on 217 | `2` on 13,498 | yes |
| v3 | `2` on 455, `0` on 217 | `2` on 13,498 | yes |

`FUN_004148b0:104` is `for i in 0..3: if (E470(i) == 1) last = i`. In the **original, only the
player car ever reads `1`** — so `last` resolves to car 0 on **581/672** samples. In the **port no
car ever reads `1`**, so `last == -1` on every row and branch 2 dies at site 105.

**`MASHED_SLOTSTATE_SEED` is faithful for the three AI cars and leaves the player slot at `0` where
the original has `1`. That single cell is blocker A.**

## 1. This was already recorded as a gap — and now its cost is known

`RESULT_G1.md` / `NEXT_SESSION` recorded the seed as "solvable today via `MASHED_SLOTSTATE_SEED`
(3 of 4 cars read state `2`, **player slot 0 stays 0**)". The "stays 0" was known. What was not
known is that **`FUN_004148b0` depends on exactly that cell**, and on it being `1` rather than `2`.
The seed's known 75% coverage is the wrong 75% for this consumer.

## 2. `U-1300`'s value semantics, now evidence-backed

The `0040e470` plate carries `[UNCERTAIN U-1300]`: "What does value 2 at
`PTR_PTR_005f2770[param_1*4+0x34]` signify?" This capture answers it for the race context,
measured on both sides:

- **`1` = the player car** (v0 only, 581/672 in the original)
- **`2` = an AI car** (v1/v2/v3, 455-563/672 each)
- **`0` = neither/inactive** (all four cars take it some of the time)

That is consistent with the plate's note that `FUN_0040eee0` filters on `E470(i)==2 &&
FUN_0046c7b0(i)==1` — i.e. that parent wants AI cars, while `FUN_004148b0:104` wants the player.
**Both values are legitimate; they select different populations.** Reported as a data-semantic
observation from 672 samples of one scenario, not as a closed definition — `U-1300` should be
narrowed, not struck.

## 3. What is NOT claimed

- **Not that writing `1` into the player slot is the fix.** The standing rule
  (`verify/d3_modes37_20261002`) is that seeding globals is not porting. `MASHED_SLOTSTATE_SEED` is
  already a knob-gated bridge under a recorded user fidelity decision (D-11072 leg C, "YES"), so
  extending it is *in scope for that decision* — but the faithful route is to find what **writes**
  that cell in the original. Unmeasured.
- Not that clearing blocker A makes branch 2 fire. Blocker B (site 126, `TimerAt < 0x1195`) is
  expected to be downstream (`RESULT_GF1.md` §4) but that is an expectation, not a measurement.
- Not that `bias374`/site 99 is affected. It still accounts for **95.10%** of rows and remains
  downstream of the missing race sub-state machine (`RESULT_M5.md`).
- Nothing about the 91/672 samples where the original's v0 reads `0` rather than `1` — the
  transitions were not characterised.

## 4. Next

1. **Find the writer of `*(*(int**)0x005f2770 + 0x34)`** (the player slot). If it is cheap, port
   it; if not, extending the seed is already within D-11072 leg C's decision and should be recorded
   as a bridge, not a port.
2. Then re-run `GF1b` and read the exit histogram again: site 105 should drain, and whether it
   drains into **114 (FIRE)** or into **126** is the next real datum.
3. Site 99 (95.10%) is untouched by any of this and stays with the sub-state-machine row.
