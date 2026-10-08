# PRE-REGISTRATION — `H2`: do `MASHED_SLOTSTATE_SEED` and `MASHED_SLOT_PLAYER` go default-ON?

Date 2026-10-08. Written before the default is touched. Task (C) from the session kickoff.

## 1. The decision splits — the two knobs are NOT the same class of question

`RESULT_SP.md` §5 said "the two should be decided together since the player store is a no-op
without the seed". The *dependency* is real. The *standard of evidence* is not shared, and reading
the source makes that plain:

| | `MASHED_SLOTSTATE_SEED` | `MASHED_SLOT_PLAYER` |
|---|---|---|
| what it writes | `I32(0x005f2770) = 0x005f2728` | `Ai_SetCarSlotState(0,1)` + `(1..3, 0)` |
| what it IS | restores a **load-time `.data` constant the binary itself carries**; `TrackRenderer.cpp:372-383` calls it "not a bridge" | the source calls it, in capitals, "**A BRIDGE, EXPLICITLY, AND NOT A PORT OF CONTROL FLOW**" (`:391`) |
| faithful site? | n/a — it is a static initialiser, not control flow | **no.** Original writer `FUN_0042b960` lives in a menu message handler the standalone never runs: "there is no faithful site for these stores" (`:394-396`) |
| completeness | total — one dword, one value | **partial by design**: `FUN_0042b960` also sets `DAT_007f1a0c = 1` and writes the index table as `[playerBlockIdx,-1,-1,-1]`; neither is reproduced (`:411-414`) |
| why it exists | the port is simply wrong here (reads 0 where the original reads `0x005f2728`) | to let **branch 2** fire (`:407-409`) |

So the seed is a **faithfulness correction**; the player store is a **partial stand-in at a
knowingly-wrong site whose only purpose is to enable a feature that is itself default-OFF**
(`MASHED_WIRE_B2`). Shipping the enabler without the thing it enables buys nothing and carries
risk the current metrics cannot see — `RESULT_SP.md` §2 is explicit that the stepdump never logs
car 0, so **the player store's effect on the player is unmeasured**.

**Registered position:** seed is a candidate for default-ON; player store is **not**, and is
decided with branch 2's wiring, not before.

## 2. Gates for `MASHED_SLOTSTATE_SEED` → default-ON

| gate | PASS condition | status |
|---|---|---|
| `H2-FAITHFUL` | the seeded value equals the dword the image carries at `0x005f2770`, read independently of the decomp witness | **PASS (pre-run)** — `0x005f2728` at file offset `0x1f2770` in **both** `MASHED.exe` and `MASHED.exe.unpatched`; `.data` offset `0x008770` < RawSize `0x04d000`, so it is genuinely file-backed, not a BSS-tail read |
| `H2-SCOPE` | over the committed `SP_base` vs `SP_seed` pair, the ONLY columns that differ are the seed's own witnesses | **PASS (pre-run)** — 19,418 shared keys, 80 columns, exactly **3** differ: `ss_base`, `ss_v`, `ss_raw`. 77/80 identical on every row |
| `H2-DEFAULT` | after the flip, a knob-off capture differs from the pre-flip knob-off control (`WS_ctl.step.csv`) in **exactly** `ss_base`/`ss_v`/`ss_raw` and nothing else | to run |
| `H2-OPTOUT` | with the new `MASHED_NO_SLOTSTATE_SEED` set, the capture is **identical to `WS_ctl.step.csv` in every column** — the old default is still exactly reachable | to run |
| `H2-BE` | (b) and (e) print the committed baseline: `launch` 1426.4 / 2053.0 / 2055.2; (b) v1/v3 PASS, v2 the same five bands with the same numbers | to run |

`H2-OPTOUT` exists because a default flip that cannot be undone from the environment destroys the
A/B lane this project runs on. The opt-out follows the house convention for default-ON features
(`MASHED_NO_UVSCROLL`, `MASHED_NO_FOG`, `MASHED_NO_COPTERS`, `MASHED_NO_PARTICLES`,
`MASHED_NO_ELIM`).

## 3. What a PASS here does and does not mean

- It makes the default build **more faithful by one dword** that the original's image literally
  contains and the port's image-pad zero-fills.
- It is **not** a behavioural improvement and must not be reported as one. The change is measured
  **inert** on every metric the project has: (b), (e), and 77 of 80 stepdump columns.
- **No C-level.** No function moves on the rubric; this is a data-initialisation correction, and
  `TrackRenderer.cpp:379-380` already says "no C-level follows".
- The reason it is worth doing anyway is that the value is a *precondition* other ports will read.
  Leaving the port reading 0 where the original reads `0x005f2728` is a latent wrong answer
  (memory `zeroed-granule-vs-minus-one-sentinel`).

## 4. What stays OFF

`MASHED_SLOT_PLAYER`, `MASHED_WIRE_B2`, `MASHED_A364_RESET`, `MASHED_RACEPCT_BRIDGE`,
`MASHED_RACEMETRIC_ARC`. None of them is touched by this leg.
