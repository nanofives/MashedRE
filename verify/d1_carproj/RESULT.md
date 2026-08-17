# D1 — camera space settled: `ctrl+0x40` is NOT the world eye; the RwCamera frame is (2026-08-16)

Settles the question left open by `verify/d1_frame/RESULT.md`, and **overturns the
refutation recorded there**.

## Method

One capture, no correspondence machinery. Read each car's world position out of the *render*
hierarchy — the space RenderWare actually draws in — then project it with a candidate camera
and compare against where the cars appear in the same capture.

Car path (documented by the 2026-08-01 frame BFS, `mashed_qol.cpp:283-287`, no new RE):
`renderable = *(DAT_0063da18 + i*0x2ac)` → `frame = *(renderable+4)` → `root = *(frame+0xa0)`
→ world matrix at `+0x50` (LTM), position at `+0x30`.

Projection is left-handed to match the standalone, and uses the measured `viewWindow`
directly — RenderWare's view window *is* the half-extent at unit distance, so
`ndc = (offset/z) / viewWindow` with no intermediate FOV conversion.

## Candidate A — the controller pose we transplant: WRONG

```
eye=(0.43,3.45,5.17) at=(-0.12,0.49,-1.42)
car0 world=( 0.50,0.43,-2.00)  z= 7.74 -> (273.9,228.6)
car1 world=(-1.10,0.47,-0.90)  z= 6.85 -> (399.3,255.2)
car2 world=( 1.14,0.50, 0.20)  z= 5.67 -> (214.6,304.9)
car3 world=(-0.47,0.54, 1.30)  z= 4.78 -> (383.5,357.4)
```

Predicts four cars **close** (z 4.8-7.7) and **spread across the middle of the frame**. The
capture shows four cars **clustered tight and small** near (250-320, 195-250), far up the
road. Overlay: `p_carproj.png`. The prediction is wrong in the one way that matters most —
distance — and therefore in size and spread.

## Candidate B — the RwCamera frame: RIGHT on scale and depth

Same cars, projected with the frame's own basis (`right`/`up`/`at`/`pos` from the LTM,
including its roll):

```
frame pos=(1.599,3.155,19.673)
car0 z=21.80 -> (359.7,223.5)     car1 z=20.71 -> (395.8,246.3)
car2 z=19.63 -> (344.4,221.5)     car3 z=18.54 -> (384.1,246.9)
```

Depth 18.5-21.8 instead of 4.8-7.7, and the four collapse into a tight cluster at the right
height — which is what the capture shows. Overlay: `q_frameproj.png`.

**Conclusion: `ctrl+0x40` is not the world camera position.** The controller's position and
aim fields live in some other space (or are inputs to the camera rather than its output), and
the camera's RwFrame is the world pose. Every pose transplant to date has been sourced from
the wrong field.

## Correction to `verify/d1_frame/RESULT.md`

That note refuted the frame matrix on the grounds that its `right.y = 0.440` implies ~26 deg
of roll while "the original capture's horizon is level". **That judgement was wrong.** This
capture is visibly rolled — the buildings lean and the road runs diagonally across the frame.
The roll is real and is in the image. I called a tilted frame level by eye, and used that to
discard the correct hypothesis.

The frame also moves between reads (pos z = 22.600 in one run, 19.673 in the next), so it is
live per-frame data, not a stale matrix — the other reason that note gave for doubting it.

## Residual, stated and not explained

The frame projection is offset **~80-100 px to the right** of the actual cars (predicted
x 344-396, actual ~245-320); vertical agreement is good. At `fovx = 61.93 deg` over 640 px,
90 px is ~8.7 deg of azimuth.

The most likely cause is that the pose is read a moment before the shim's capture fires, so
the two are not the same frame and the camera pans in between. **That is a hypothesis, not a
measurement** — read-vs-capture were not synchronised in this run. Ruling it in or out means
reading the frame *at* the Present that dumps the backbuffer, rather than before it.

So: the camera-space question is settled; the sub-10-degree residual is not.

## Consequence

`MASHED_CAM_POSE` should be fed from the RwCamera frame (`pos`, and `pos + at`), not from
`DAT_00897fe0 +0x40/+0x4c`. Note the env var takes eye and an at-point only, with up assumed
to be world Y — it **cannot express the roll that is measurably present**, so a faithful
transplant needs a basis, not a look-at pair. That is a change to the interface, not a value.
