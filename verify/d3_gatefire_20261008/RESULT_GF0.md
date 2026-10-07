# RESULT — GATEFIRE leg 0: runtime substrate census

Date 2026-10-08. Pre-registration: `PREREG_GATEFIRE.md` §3, committed **unrun** at `1cb89a5a`.
**RAN**, 7 runs. MEASUREMENT ONLY — no game logic was changed, **no global was seeded**, nothing is
default-ON, `original/` and the `.asi` code path untouched. No C-level.

Raw: `GF0_CENSUS.txt`, `GF0_CTL.txt`, `GF0_RUN.txt`, `GF0{off,on}_{1,2,3}.gates.csv`.

## 0. Verdict first

**Both pre-registered decision rules fire, and they point opposite ways.**

> **Branch 1 (`FUN_00414a70 == 2`, target 36) is INPUT-BLOCKED and is deferred** — `GF0-PROG1`
> measured `0` on 100% of rows, with a coverage check proving the probe works.
>
> **Branch 2 (target 64) is NOT blocked the way the 2026-10-03 witness left it.** Of its three
> independent blockers, **one is resolved** (H3's `refdist`), **two are image-table transcriptions
> already resolved on paper** — and the **genuine** remaining obstacle is a different thing: the
> two upstream disagreements, one of which is now **worse** than recorded.

| gate | verdict | figure |
|---|---|---|
| `GF0-REFDIST` | **PASS** | ON arm: non-zero on **6,784 / 10,578 / 9,632 of 13,498** for cars 1/2/3, 2,973 / 10,441 / 6,160 distinct. Car 0 is `0`, correctly gated out |
| `GF0-IDX364` | **measured — DISAGREES** | port `0` on **53,992/53,992**; original `-1` on **512/512** |
| `GF0-BIAS374` | **measured — DISAGREES, and WIDER than recorded** | port `{4: 39336, 2: 4804, 3: 4804, 0: 2644, 1: 2404}`; original `0` on 512/512. **The witness recorded `{0,1,2,3}`; `4` is new and is the majority value at 72.9%** |
| `GF0-TIMER` | **dead — but see §3, this is NOT a blocker** | `0` on all 4 cars, all rows, both arms |
| `GF0-PROG1` | **0 — branch 1 blocked** | `0` on all 4 cars, all 13,498 rows per car, both arms |
| `GF0-CTL` | **PASS** | `GF0step.csv` hashes to **`86b7b2bb`**, identical to `E2off_1` / `H1step` / `H3step`; `det_prefix` identical over all 19,418 keys. The `*_abs` family stays **DEAD** on both arms |
| `GF0-DET` | **PASS** | 3 repeats byte-identical per arm: OFF `9e7f370f`, ON `8732b3bc` |

## 1. `GF0-PROG1` — branch 1's input is genuinely absent, and the probe is proven live

`prog_96e8` (`0x008a96e8 + v*0x30c`) reads `0.0` on every car on every row of both arms.

**The coverage check that makes this a finding rather than a null** (memories
`zero-of-n-needs-a-coverage-check`, `all-zero-reads-prove-nothing-alone`): the column immediately
beside it, `racepct_ec` (`0x008a96ec + v*0x30c`) — **4 bytes higher, same per-car stride, read by
the same code in the same loop iteration** — is live in the same capture:

| field | v0 | v1 | v2 | v3 |
|---|---|---|---|---|
| `prog_96e8` non-zero | 0/13498 | 0/13498 | 0/13498 | 0/13498 |
| `racepct_ec` non-zero | 13498/13498 | 11834/13498 | 13143/13498 | 13094/13498 |
| `racepct_ec` distinct | 718 | 1103 | **11620** | 4507 |

So the addressing, the stride and the loop are all demonstrably working. `0x008a96e8` has no
writer, exactly as `UNCERTAINTIES.md:61` recorded on 2026-10-06. **Confirmed in-game, not
re-derived.**

**Decision rule applied as registered:** branch 1 is deferred with its blocker named. §5 of the
pre-registration does not start. No producer was built, and no global was seeded, inside this leg.

## 2. The two upstream disagreements — the real branch-2 obstacle, and one got worse

| input | original | port (2026-10-03 witness) | **port (today)** |
|---|---|---|---|
| `idx364` `0x0089a364` | `-1` on 512/512 | `0` | **`0` on 53,992/53,992** |
| `bias374` `0x0089a374` | `0` on 512/512 | `{0,1,2,3}` | **`{0,1,2,3,4}`, with `4` at 72.9%** |

`idx364` is not cosmetic. At `AiLeaderTimer.cpp:94` it gates whether `E470(idx364)` is called at
all: the original, reading `-1`, **skips that call on every one of 512 calls**; the port, reading
`0`, would take it. A branch-2 port measured in this state would be measuring a path the original
never executes.

`bias374` feeds the limit-table index (`bias374 + iVar1*5`, `AiLeaderTimer.cpp:98`). The original's
only index is **10**. With the port's `bias374 = 4` and `iVar1 = 2` (`flt360` is `2.5` on 100% of
rows, matching the original exactly) the index becomes **14**, not 10 — a different table entry.
**The witness never saw `4`**, so this is new information and it widens the gap rather than
narrowing it. Its writer is `FUN_004177b0`, whose exe copy was demoted C3→C2 on 2026-09-29 as "the
finish-order fragment only"; that demotion is the first place to look.

## 3. Correction: `GF0-TIMER` being dead is NOT an independent blocker

I listed `timer_4c8` alongside the real blockers when registering. It is not one. `0x0089a4c8 +
v*0x74` is **`FUN_004148b0`'s own state**: `AiLeaderTimer.cpp:108` does `TimerAt += frameDt`, with
further writes at `:113/:116/:123/:126/:128`. Nothing else writes it. It reads `0` standalone
because the function does not exist yet, so it becomes live by the port's own action and needs no
separate producer. `rank_4c4` likewise reads `0` — and so does the **original** (`RESULT_WITNESS.md:69`),
so those two agree already.

The pre-registration's `GF0-TIMER` decision rule ("dead → report `GF1-FIRE` as structurally-0") is
therefore **withdrawn as written**; it was predicated on the wrong causal model.

## 4. What the census says the remaining branch-2 scope actually is

| blocker, per `RESULT_WITNESS.md` §"why it returns 0" | status after leg 0 |
|---|---|
| the access violation on the first callee | **not a blocker by the prereg's mechanism** — a standalone body, not an `.rsp` addition (`PREREG_GATEFIRE.md` §4.1) |
| the zero limit table (`tbl10` = `0`, original `1`) | **confirmed zero in-game**; resolved by transcription. This is a **port of image `.data`** the standalone never loads, not a seeded runtime global — the witness itself called transcription the resolution (`:106`) |
| the all-zero `Prog` array | **RESOLVED by H3** — `GF0-REFDIST` PASS |
| thresholds (`thr_a8` = `0`, original `6.5`) | **confirmed zero in-game**; same transcription class (`:107`) |
| — | **`idx364` / `bias374` disagreements: unresolved, and `bias374` is wider than recorded** |

`flt360` = `2.5` and `framedt` = `50` match the original **exactly**, on 100% of rows. `mode368`
is `{0: 9852, 1: 44140}` against the original's `0`; it never reads `2`, so the `:91` early-out is
not taken on either side.

## 5. What is NOT claimed

- **Nothing about whether either branch fires.** No predicate was ported in this leg; `GF1-FIRE`
  and `GF2-FIRE` remain unmeasured, exactly as `RESULT_H3_STEP.md` left them.
- That transcribing `0x005f2dd8` / the four thresholds is *sufficient* for branch 2. The two
  disagreements survive all six witness inputs and are measured here to survive them still.
- Any reading of `prog_96e8 = 0` as a statement about `0x008a96ec`'s bridge, which is live and
  unaffected. The two fields sit 4 bytes apart and have different producers.
- That `bias374 = 4` is *wrong*. It is a **disagreement** with the original; which side is correct
  for the standalone's own state is unmeasured. **[UNCERTAIN]**

## 6. Next

1. **Resolve `idx364` first.** It is the cheapest of the two and it can invalidate `GF1-FIRE`
   before it is measured. Find the writer of `0x0089a364`; do not seed it.
2. Then `bias374`, starting at `FUN_004177b0`'s C2-demoted exe copy (`Race/RuleEngine.cpp`).
3. Branch 2's body (`PREREG_GATEFIRE.md` §4) only after both, or `GF1-FIRE` measures a different
   function.
4. **Branch 1 is deferred** pending a producer for `0x008a96e8`. That is a `DEFERRED.md` row, not a
   leg of this pre-registration.
