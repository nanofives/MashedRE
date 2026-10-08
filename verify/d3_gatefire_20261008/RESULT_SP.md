# RESULT — (b) and (e) are UNCHANGED by the slot bridge. And the GF1c race divergence was the PROBE, not the bridge.

Date 2026-10-08. Follows `RESULT_GF1C.md` §5 item 1, chosen by **USER DECISION (Mariano,
2026-10-08)**. **RAN**, 3 stepdump arms. No C-level. `original/` untouched, `.asi` untouched,
nothing default-ON.

Raw: `SP_GATES.txt`, `SP_{base,seed,player}.csv`.

## 0. Verdict first

> **No regression. Criteria (b) and (e) are identical across all three arms — every digit, every
> band.** `SP_seed` and `SP_player` are **byte-identical** (`396e86a5`), so the player-slot store
> has **no knock-on effect on the three AI cars**.
>
> **And it retires a caveat from the previous leg:** `RESULT_GF1C.md` §3 flagged that the arms
> were not frame-aligned (13,498 vs 13,858). That divergence was caused by **the `MASHED_GF1`
> probe**, not the bridge — with the probe off, the bridge changes nothing.

| arm | knobs | SHA-256 | (e) | (b) |
|---|---|---|---|---|
| `SP_base` | none | **`86b7b2bb`** | 3/3 PASS | v1/v3 PASS, v2 same 5 bands |
| `SP_seed` | `MASHED_SLOTSTATE_SEED` | `396e86a5` | 3/3 PASS | identical |
| `SP_player` | seed + `MASHED_SLOT_PLAYER` | **`396e86a5`** | 3/3 PASS | identical |

`SP_base` is the **ninth** build to hash `86b7b2bb`, so the default path is still untouched.

## 1. The (e) digits and (b) bands, unmoved

All three arms print the committed baseline exactly: `launch` **1426.4 / 2053.0 / 2055.2**,
`ft_median_m0` **2550.6 / 2053.0 / 2278.2** (`PREREG_CARCAR.md:72`). For (b), `v1`/`v3` PASS and
`v2` fails the **same five bands with the same numbers** (`c0_distinct=39`, `c1_distinct=75`,
`steer_distinct=113`, `c1_median=5.5`, `abs_steer_median=44.5`).

As always this is PASS **as unchanged**, not "(b) passes" — `v2` still fails, which is what U-9186
exists to fix.

## 2. The equality is PARTLY STRUCTURAL, and saying so matters

`SP_seed` vs `SP_player` byte-identity is strong, but it must be read with one fact:
**the stepdump logs only cars 1, 2, 3 — never car 0.** Measured: the `v` column's distinct values
are `{1, 2, 3}`.

The knob writes **the player's cell** (`FUN_0040e480(0,1)`). So the stepdump cannot observe the
changed cell directly, by construction — and `ai_speed_env.py` / `ai_ctrl_window.py` only ever
score `v1`/`v2`/`v3`.

Therefore the correct statement is: **the store has no effect on the three AI cars' behaviour**,
which is what (b) and (e) measure and what could have regressed. It is **not** evidence about the
player car, which these metrics never covered. The `ss_base`/`ss_v`/`ss_raw` columns differ
`SP_base` → `SP_player` on all 19,418 rows, but that is the **seed** (`ss_base` 0 → 6,235,944 =
`0x005f2728`, `ss_v` −1 → 2), not the player store.

The direct witness for the player store remains the gates dump: `GF1c_off` read `E470` as
`0/2/2/2` and `GF1c_on` as **`1`/2/2/2** (`RESULT_GF1C.md` §0).

## 3. Retiring the GF1c frame-count caveat — and a disclosure about the probe

`RESULT_GF1C.md` §3 recorded that the ON/ОFF arms ran 13,498 vs 13,858 frames and concluded "the
bridge changed the run". **That attribution was wrong.** Both GF1c arms carried `MASHED_GF1=1`,
and with the probe **off** the bridge is byte-identical to the seed arm. So the divergence came
from the probe.

**The `MASHED_GF1` probe is not a passive observer.** It calls `LeaderTimer`, which **writes**
`TimerAt` (`AiLeaderTimer.cpp:108`, `:113`, `:116`, `:128`) and `RankAt` (`:113`, `:129`). Running
it mutates AI state. That was true from the moment it was written and should have been stated in
`RESULT_GF1.md`, not noticed two legs later. Consequences, recorded rather than buried:

- `GF1c`'s **201 firings** were measured by an instrument that perturbs the state it reads. The
  OFF→ON *delta* is still attributable to the slot store (both arms carried the probe), but the
  absolute count is a property of the probed run, not of a clean one.
- Any future (b)/(e) scoring **must** have `MASHED_GF1` off, as these three arms did.
- A wired branch 2 would make those same writes *legitimately* — so the probe is representative of
  the wiring, just not of the current default build.

## 4. What is NOT claimed

- **No C-level.** Nothing here is a behavioural diff against the original.
- Not that the bridge is harmless once branch 2 is **wired**. It is inert today precisely because
  nothing in the default build reads `E470`; wiring the branch is what gives it consequences, and
  that is a separate leg with its own gates.
- Not that (b)'s `v2` failure moved. It did not, in any arm.
- Not that the player car is unaffected — §2; the metrics do not cover it.

## 5. Next

1. **The bridge can now be considered for default-ON on the evidence that it is inert** — same
   class of question as `H2`'s `MASHED_SLOTSTATE_SEED`, and the two should be decided together
   since the player store is a no-op without the seed.
2. Wiring branch 2 to `ctrl` remains the separate registered leg
   (`PREREG_GATEFIRE.md` §7 non-goal), and is where the 201 firings would acquire meaning.
3. Site 99 (95.52%) still belongs to the race sub-state-machine row (`RESULT_M5.md`).
