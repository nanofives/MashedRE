# RESULT — D-11072 the race_pct bridge write (0x008a96ec)

Pre-registration: [`PREREG_BRIDGE.md`](PREREG_BRIDGE.md), committed **UNRUN** at `257fcd39`.

**Verdict: the race_pct bridge is CORRECT and consumed by a LIVE reader, but behaviourally
INERT at `fd0=0`** — exactly the pre-registered §5 first branch. The bridge is **necessary but
not sufficient**; the next D-11072 blocker is the game-mode gate on the consumer, not the progress
state.

**No C-level moved** (a bridged write is not a behavioural diff, registered in advance).
**`original/` untouched** (anchor `BDCAE093…` re-verified before and after). Source footprint: one
**default-OFF** write (`MASHED_RACEPCT_BRIDGE`) + three appended witness columns in
`D3d9Render/TrackRenderer.cpp`. Build SHA-256
`FD060C72272BB3A34F8E8D227073F71E560A9C614024B0F50D1CC5F6AE5262D4`. Tool `re/tools/racepct_scale.py`
extended with the `bridge` subcommand. Nothing ships default-ON.

## 1. The eight gates

Six runs, Training: `N1..N3` (bridge OFF), `Y1..Y3` (`MASHED_RACEPCT_BRIDGE=1`).

| gate | threshold | measured | verdict |
|---|---|---|---|
| `G-WROTE` | Y `ra_ec`==`arcpct` 100 %, N `ra_ec`==0 100 % | Y **4338/5348/8963**, N **4338/5155/8772** (all 100 %) | **PASS** |
| `G-LIVE` (**control, could fail**) | Y `val_880` ≈ `arcpct*0.01+lap` ≥ 95 % **and** Y≠N | Y **1.0000** on all 3 cars; Y nonzero **3387/4781/8339**, **N nonzero 0/0/0** | **PASS** |
| `G-EFFECT` | (e) six digits + (b) bands, Y vs N | **identical** (predicted inert) | **PASS (inert)** |
| `G-INERT` | 0 differing cells, 74 pre-existing cols | **0 of 374,662** (74 × 5063 shared `(frame,seq,v)` keys) vs committed leg-A build `ARC3` | **PASS** |
| `G-DET` | (e) digits + bands identical across 3 repeats/arm | identical both arms | **PASS** |
| `G-BANDS-UNEDITED` | both scorers' git clean | empty before/after | **PASS** |

`G-LIVE` is the gate that makes this a result rather than a bookkeeping write. `val_880` is
written by `AiPreTickRubberBand:1471` as `ra*0.01 + lap`, `ra` being the read of the bridged slot
at `:1468`. On the Y arm `val_880` equals `arcpct*0.01 + lap_9648` to `1e-2` on **every** row and
is non-zero on thousands of them; on the N arm it is **0 on every row**. The null hypothesis "the
write lands in memory nothing live reads" predicts `val_880` identical in both arms — it is not.
So the port's live per-frame AI code demonstrably consumes the bridged race position.

## 2. Why it is inert, and why that is the useful finding

`AiPreTickRubberBand` reads `ra`, forms `val = ra*0.01 + lap`, stores it at `0x0089a880+v*4`, and
that value drives behaviour only through three game-mode-gated paths (`AiStandalone.cpp:1472/1492/
1505`): the finish-order candidate slots (`fd0 ∈ {4,7,8,9}`) and the mode-4/mode-9 speed scaling
(`fd0 ∈ {4,9}`). `AiPreTickRubberBand` takes `fd0` from the global `0x007f0fd0`, which is **0** in
the standalone (image-pad; `aib_game_mode_fd0()` is also 0). With `fd0=0` none of the three fire,
and `0x0089a880` has no other reader in the port — so a correct, live-consumed race position
changes nothing measurable. (e) is bit-identical (`launch` 1426.4/2053.0/2055.2, `ft_median_m0`
2550.6/2053.0/2278.2) and (b) is the same 5 bands on all six runs.

**This advances D-11072 concretely.** Before today the race position was blank-mapped zero and it
was unknown whether any live standalone code even reads it. Now the state is **live and proven
consumed**, and the remaining obstruction is pinned precisely: the consumer's `fd0` gate, not the
progress substrate. The over-speed catch-up branches this chain targets (U-9186) read a *different*
field (`0x008a96e8`, `Fi_UpdateBoostOrder`) whose lights-window effect is null; this write targets
`race_pct` (`0x008a96ec`) only, as pre-registered.

## 3. What the next step is (not this session)

To make the bridged race position do work, the consumer must be reached: either the standalone
must run with the real game-mode selector at `0x007f0fd0` set to a championship/elimination mode
(`fd0 ∈ {4,7,8,9}`), or the `fd0`-gated blocks must be ported to the standalone's own mode state.
That is a game-mode-wiring effort, separately scoped — **not** a progress-state problem, which is
now solved. Writing `0x008a96e8` for the boost-order path is the other open sub-question. **Leg B
stays unstarted.**

## 4. What this establishes

- The monotone `arcpct` writes correctly into the original `race_pct` slot `0x008a96ec`, and the
  port's live AI code (`AiPreTickRubberBand`) consumes it — proven by the `val_880` control.
- It is behaviourally inert at the standalone's `fd0=0`; the next blocker is the game-mode gate on
  the consumer, not the race-position state.
- Default-OFF, bit-reversible, zero gated-(e)/(b) regression, no C-level moved, `original/`
  untouched.
