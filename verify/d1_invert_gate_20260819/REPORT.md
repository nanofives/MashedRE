# D1 renderer inversion — gate report (2026-08-19)

**Change under test:** `RaceSubmit_Requested()` inverted so **librw is the default
renderer** for the standalone. `MASHED_RENDER_LIBRW=0` reverts to the legacy D3D9
path; unset or any non-`0` value keeps librw. Source: `RwRaceSubmit.cpp:148-151`
(`return !(e && e[0]=='0' && e[1]=='\0')`), same "=0 reverts" shape as
`MASHED_RULE_ENGINE`.

## Verdict: GATE PASSED

The librw-default build boots, selects librw with no env set, renders a complete
race scene, is bit-deterministic run-to-run, and does not regress the frontend.
The one non-GREEN result (drawlist_diff RED 6/468) is fully attributed to
free-run animation phase on a single animated element and is provably
independent of the renderer flip.

## 1. Clean-env selects librw (empirical)

`log/librw_race.txt` (snapshot: `evidence/librw_race_cleanenv.txt`) captured with
**no `MASHED_RENDER_LIBRW` set**. `RaceSubmit_Init` ran — which only happens when
`RaceSubmit_Requested()` returns true — proving librw is now the default:

```
ok: init (640x480)
ok: scene built — sectors=12 mats=13 tris=16480 verts=16229 (dicts=1)
f0   instances=27 models=9 | amb=(0.200,0.300,0.300) sun=(0.600,0.700,0.700) has_sun=1
     MATCMP max|d3d9-librw|=7.629e-06 (row 3 col 0)  maxrel=2.042e-07
```

Before the inversion this path was unreachable on a clean env (D3D9 ran instead).

## 2. Instancing consequence is LIVE and BENIGN (intended difference, not broken)

`RaceSubmit_InstancesEnabled()` has been default-ON since 2026-08-02 but was
unreachable on a clean env. Post-inversion it is LIVE: `instances=27 models=9`
(model[1]=5, model[2]=9, model[3]=5, model[8]=4 …) — cars, props, particles and
pickups instance through librw by default.

Visual confirmation (`evidence/race1_01_grid.png`): the harbour grid renders in
full — water, docks, buildings, a flying helicopter and the SUPERSONIC / EMPIRE
billboards (the instanced props logged above), the player car, the "2" countdown
HUD and the HUD colour squares. No black world, no missing car. This is the
roadmap-expected "default build now instances through librw", not a regression.

The librw path also tracks the legacy D3D9 path tightly: per-model MATCMP is
≤7.6e-06 and the previously-measured d3d9-vs-librw pixel A/B is **max 1.01%**
(`../d1_recheck_20260818/REPORT.md`) with a byte-identical control
(`../d1_control_20260818/REPORT.md`). So "different because librw is now default"
amounts to ≤1% at the pixel level.

## 3. Determinism control pair — 0.00% (`../d1_ctrl_20260819/REPORT.md`)

Two clean-env (librw-default) race captures of the standard 16-shot demo:
**16/16 shots at 0.00%**, byte-identical. The librw-default build is
deterministic; no harness noise.

## 4. Frontend regression check — drawlist_diff scr1

Original burst (`log/menu_draw_burst.json`, scr1, 4 frames × 117 draws) vs the
inverted-build standalone stream, `--scale-b 1` (both sides render 640×480; the
old `--scale-b 0.8` default assumes an 800×600 standalone and is stale — with it
the aligner scores 0 matches, which is a flag error, not a defect):

```
scr1_f0  matched 117  mismatched 0  missing 0  extra 0
scr1_f1  matched 117  mismatched 0  missing 0  extra 0
scr1_f2  matched 117  mismatched 0  missing 0  extra 0
scr1_f3  matched 111  mismatched 6  missing 0  extra 0
VERDICT: RED (match=462 mismatch=6 missing=0 extra=0)
```

**0 missing, 0 extra** — the composition is complete; no draw was added or
dropped. The 6 mismatches are all in the last frame (f3), all
`MISMATCH(moved)`, all on `LogoOverlayDraw` (the animated logo corner glyphs),
identical colour `ff808080`, ~4px position/size deltas:

| frame | idx | A (orig @0x474464)              | B (RE @0x74062 LogoOverlayDraw)  | class          |
|-------|-----|--------------------------------|----------------------------------|----------------|
| f3    | 40  | x=562.01 y=1.00 w=22.99 h=21.15 | x=558.00 y=-1.00 w=26.98 h=23.99 | MISMATCH(moved) |
| f3    | 46  | x=543.01 y=22.16 w=22.99 h=19.10 | x=539.00 y=21.92 w=26.98 h=21.32 | MISMATCH(moved) |
| f3    | 52  | x=482.01 y=41.25 w=22.99 h=22.75 | x=478.00 y=43.23 w=26.98 h=20.77 | MISMATCH(moved) |
| f3    | 61  | x=562.01 y=416.00 w=22.99 h=22.16 | x=558.00 y=416.00 w=26.98 h=22.99 | MISMATCH(moved) |
| f3    | 67  | x=543.01 y=438.16 w=22.99 h=19.10 | x=539.00 y=437.92 w=26.98 h=21.32 | MISMATCH(moved) |
| f3    | 73  | x=482.01 y=457.25 w=22.99 h=20.37 | x=478.00 y=459.23 w=26.98 h=19.11 | MISMATCH(moved) |

These are the animation-phase residual the harness documents as impossible to
close on free-running captures ("unsynced captures can never agree",
`re/analysis/parity_tooling.md`). The animated logo runs off its own frame
counter; the original (Frida) capture has no deterministic frame mode, so its
logo phase at frame 200 cannot be synced to the standalone's.

### This RED is renderer-independent (proof)

Standalone-vs-standalone diff of two scr1 draw streams from the SAME inverted
build — one clean env (librw default), one `MASHED_RENDER_LIBRW=0` (legacy D3D9)
(`evidence/scr1_drawstream_librwdefault.json` vs
`evidence/scr1_drawstream_d3d9legacy.json`): **all 84 differing draw-lines have
leading retaddr `0x74062` (LogoOverlayDraw) — nothing else differs.** Every
non-animated frontend draw is byte-identical between the two renderer defaults.
The renderer flip does not touch the Im2D frontend; the only variance is the
free-run logo phase.

Corroborating: the d3d9-vs-librw pixel A/B measured the two frontend screens
(`race1/00_challengeselect`, `race1/02_back_to_menu`) at **0.00%**.

## Files

- `../d1_ctrl_20260819/REPORT.md` — determinism control pair (0.00%)
- `../d1_recheck_20260818/REPORT.md` — d3d9-vs-librw A/B (max 1.01%)  [pre-existing]
- `../d1_control_20260818/REPORT.md` — byte-identical control  [pre-existing]
- `evidence/librw_race_cleanenv.txt` — clean-env librw selection + instance log
- `evidence/race1_01_grid.png` — librw-default race render (instancing live)
- `evidence/scr1_drawstream_{librwdefault,d3d9legacy}.json` — renderer-independence proof
- `evidence/{r5,r6,race1}/*.bmp` — clean-env demo shots
