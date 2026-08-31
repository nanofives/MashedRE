# U-9064 mechanism found: the librw fog COLOUR reaches the pixel shader as BLACK

**The ambient fold was never the mechanism. It was compensating for fog that renders
toward black instead of toward the track's fog colour.** Arctic's sea is bright in the
original because Arctic's fog is dense and light grey; our librw path fogs it toward
(0,0,0), which on an already-dark surface is a near no-op.

## The chain, measured

**1. The original's Arctic sea luma equals its AUTHORED PRELIT.** `SEA.DFF` geometry
flags `0x1000f`, 252 verts, prelit luma **mean 28.6** (min 12.9, max 78.4, alpha all 255).
The original renders that surface at **28.2**. Our standalone renders it at **6.3**.

So we are DARKENING it by ~4.5x, not failing to add ambient. That inverts the whole
premise the fold was built on.

**2. Texture modulation accounts for the darkening.** Arctic material `sea` is white
(255,255,255,255); its texture `sea` (256x256, PAL8) has mean luma **66.9**, i.e. a
modulate factor of **0.263**. 28.6 x 0.263 = **7.5**, against our measured 6.3. Prelit x
texture is what we draw, and it is a reasonable thing to draw.

**3. Fog is what the original adds on top, and ours adds nothing.** Arctic's
`COURSE.LUA` declares `Setup_Fog(0.1, 70, 40, 44, 48)` — fog from essentially the camera
out to 70, colour (40,44,48) = **luma 43.3**. Lifting 6.3 toward 43.3 needs a fog factor
of ~0.59 to land on the original's 28.2, which is ordinary for a sea spanning mid-to-far
distance under a 0.1..70 ramp.

Measured on the Arctic s8 sea mask (71.04% of frame):

| arm | sea luma |
|---|---|
| ORIGINAL | 28.2 |
| standalone, fog ON (shipping) | 6.3 |
| standalone, `MASHED_NO_FOG=1` | 6.3 |
| standalone, ambient fold ON | 28.5 |

**Fog contributes +0.0.** And whole-frame, `MASHED_NO_FOG=1` changes **0.00% of pixels,
mean delta 0.000, max 0.0** — fog is inert everywhere in the librw path, not just on water.

**4. The reason is already instrumented and was never chased.** `log/librw_race.txt` from
that run:

```
P7 fogcaps:  RasterCaps=07732191 WFOG=1 ZFOG=1 FOGTABLE=1 FOGVERTEX=1 FOGRANGE=1
P7 fogconst: start=5.000 end=70.000 range=-0.015385 disable=1.0
P7 fogcolor: d3d9=282C30 librw_ps=(0.0000,0.0000,0.0000) *** MISMATCH ***
ok: first frame submitted — ... fog=1[0.1..70.0]
```

Fog is ENABLED (`fog=1[0.1..70.0]`). `d3d9=282C30` is (40,44,48), Arctic's colour, correctly
computed. **The librw pixel shader's fog colour is (0,0,0).** The `*** MISMATCH ***`
diagnostic at `RwRaceSubmit.cpp:886` exists precisely to catch this and is firing.

Note also `start=5.000` in the shader constants against the requested `0.1` — a second,
smaller discrepancy in the same area.

## Why this explains Arctic-vs-SuperG, which the fold never could

A uniform ambient add cannot be right on two tracks whose fog colours differ:

| track | fog | colour luma | fold's effect |
|---|---|---|---|
| Arctic | `Setup_Fog(0.1,70,40,44,48)` | 43.3 | accidentally ~= the missing fog lift, so it "worked" |
| SuperG | `Setup_Fog(0,120,255,255,255)` | 255.0 | wrong shape and wrong amount, so it broke |
| Forest | `Setup_Fog(0,180,136,155,141)` | 148.9 | wrong |

The fold matched Arctic's *average* missing fog contribution on one frame. That is why it
looked like a fix, and why it failed the moment a second reference existed. **U-9064's
"same class, opposite verdicts" contradiction dissolves: neither track wants ambient, both
want fog.**

## Prime suspect for the defect itself

`SetRenderState(rw::FOGCOLOR, ...)` is issued at `RwRaceSubmit.cpp:524` — **before**
`g_cam->beginUpdate()`. The fog RANGE had exactly this bug: `beginUpdate` welds
`fogData.end` to the far plane, and the fix (`[I4-fog CLOSED]`, `RwRaceSubmit.cpp:614-627`)
was to re-issue the range **after** `beginUpdate`, with the comment *"Must follow
beginUpdate, not precede it."* The COLOUR was never moved. Same trap, same location, one
of the two halves fixed.

`[UNCERTAIN]` — this is a strong structural match, not a confirmed cause. It is not proven
that `beginUpdate` clears `fogColor`, only that the range needed exactly this treatment and
the colour arrives black. Confirm by moving the FOGCOLOR write after `beginUpdate` and
re-reading the `P7 fogcolor` line for the mismatch to clear.

## What this does not answer

- The `start=5.000` vs `0.1` discrepancy.
- Forest's water being over-bright even unfolded (39-44 vs 20-22) — Forest's `WATER.DFF`
  carries flags `0x1004f`, i.e. `rpGEOMETRYMODULATEMATERIALCOLOR` (0x40), which Arctic and
  SuperG do NOT have, and its material alpha is 209 rather than 255. That is a separate
  lead for a separate defect.
- Whether fixing fog removes the need for the water fold entirely. Expected, not measured.
