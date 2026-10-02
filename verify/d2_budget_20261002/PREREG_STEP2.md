# PRE-REGISTER — D2 attempt 18, STEP 2: does grip-clamp #6 RUN on the original at matched `d`?

Written and committed **BEFORE** the run. Nothing here has been executed.

---

## 1 The question, and why the snapshot cannot answer it

`RESULT_STEP1B.md` §2.2: on the ORIGINAL over `d = 222..250` (n = 29) the measured
`1 - R^2` is **1.2194e-05**, while grip-clamp #6's own arithmetic on its own measured inputs
demands **>= 3.75e-04** (31x more; **1140x** at `d = 200..222`, where `s' = 0.338317`). The
candidate is its gate

```
0x00468761  cmp dword ptr [esi+0x9e0], 0x40800000
0x0046876b  jne 0x00468970                        ; not all four grounded -> SKIP the clamp
```

and the reason the `.msd` cannot decide it is that **A5 `0x0046ddb0` zeroes `+0x9e0` at
`0x0046ddd1`, one call before A6a**, rebuilding it per wheel; the render-tick snapshot
carries the **post-substep** value, which is 4.0 on 29/29 frames and is a different value.

A6a itself reads `+0x9e0` **twice and writes it zero times** over all 1243 instructions of
`0x00467650..0x0046897b` (`0x00468117`, `0x00468761`, both `cmp ... 0x40800000`). **So
`+0x9e0` read at A6a's ENTRY is bit-identical to the value the clamp's gate reads**, and an
entry hook answers the question exactly. No mid-function probe is needed or permitted
(memory `frida-interceptor-is-entry-only`).

## 2 The instrument — the EXISTING probe, no new code

`re/frida/scenario_launch.py --lat-bracket` (`scenario_launch.py:875-920`), already in the
harness and already used in §21.3/§21.5. Three entry hooks, reads only, no `onLeave`:

```
site 0  A6a entry  0x00467650   ESI-filtered to the player record
site 2  A6b entry  0x00468980
site 1  substep    0x004709a0
```

Its sample already carries `+0x9b0/b4/b8`, `+0x9d4/d8/dc`, `+0x9e4`, **`+0x9e0`**, and
`+0x9bc/c0/c4` — every field this step needs. Rate ~4/frame = ~240/s, far under the
~1000/s destabilise floor. **No probe is written, no source is changed, so `NEW = 0` by
construction for STEP 2.**

Command (muted, own PID, `--poke-ctrl-slots`, `MASHED_WIN_POS` not applicable to the
original-side Frida arm, `MASHED_TITLE` set on the spawn), matching
`orig_sl1.msd.provenance.json`'s argv except for the probe:

```
py -3.12 re/frida/scenario_launch.py --statediff-out verify/d2_budget_20261002/orig_lb18.msd
    --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38
    --poke-ctrl-slots --lat-bracket
```

## 3 Gates. A failing gate STOPS the step and is reported as a failure, not amended.

### Gate CV — coverage, counted not assumed
`latBracketStats` must report `err = null`, `a6a >= 1200`, `a6b >= 1200`, `sub >= 2400`,
and the per-frame site pattern must be **`0,2,1,1`** (A6a, A6b, substep, substep) on
**>= 99 %** of frames — which is simultaneously the **U-9160 measurement on the original
side**: exactly **2** substeps per frame, as §14.6 recorded. **FAIL => STOP.**

### Gate KA-B — KNOWN ANSWER, scored on the ORIGINAL
The probe's `+0x9e4` read at **A6b entry** must equal the same frame's `.msd` `+0x9e4`, and
its `|+0x9b0..b8|` at A6b entry must equal the `.msd`'s `|+0x9b0..b8|`, each within
**4 ulps** of `max(value, 1)`, on **>= 99 %** of matched frames. Rationale: A6b writes
neither (§26.4) and the substeps' only velocity writer is `0x0046ef70`, which `+0x9ec == 0`
disables on 29/29 frames in the window — so A6b entry and the snapshot must agree. Matching
is by the probe's own frame ordinal against the capture's frame index, and the release frame
is taken from **this capture's own `+0xbf8` marker**, not inherited from `orig_sl1.msd`'s 890
(`RESULT_STEP2.md` §2 of attempt 17: `886` was capture-specific; the same discipline applies
here). **FAIL => STOP** — it would mean the interval's contents are not what §2.1
established.

### Gate EV — the capture carries the event
`d = 222..250` must exist with `n >= 25`, and this capture's own `d = 222..250` median speed
must be within **15 %** of `orig_sl1.msd`'s **820.5**, so the two original captures are the
same manoeuvre at the same `d`. **FAIL => the run is reported as a different manoeuvre and
the comparison against `orig_sl1.msd` is not made.**

## 4 The registered reading — one number, two branches

Scored at **A6a entry**, over `d = 222..250` and `d = 200..222` separately, n reported:

> **`G4 = (+0x9e0 == 0x40800000)` as an exact dword compare.**
>
> - **Branch CLOSED** — `G4` false on **>= 80 %** of frames: clamp #6's velocity stores do
>   **not** execute on the original in this window. That explains `1 - R^2 = 1.2194e-05`
>   completely (it is then pure float32 rounding, consistent with `R > 1` on 14 of 29
>   frames), and it makes the PORT's gate the first thing to compare. The port's own gate
>   reads `Ri(v, 0x9e0) == 0x40800000` at `Integrate2.cpp:665` over a value its own A5
>   zeroes at `ForceIntegrator.cpp:55` and rebuilds at `:59` — so the named defect would be
>   **the rebuild**, not the clamp and not `l_60`.
> - **Branch OPEN** — `G4` true on **>= 80 %** of frames: the clamp DOES run and still costs
>   31x less than its arithmetic demands. Then one of clamp #6's transcribed terms is wrong,
>   and the next measurement is the original's `s` and `l_60` at the clamp's **own** phase
>   via the existing `--mag-probe` sites `0x004686a9` (the post-W1 `RwV3dLength` of `+0x9b0`,
>   self-checked 1424/1424 in §21.9) and `0x004680fb` / `0x0046820f` (`le4` / `ld4`).
> - **Neither** (`G4` true on 20-80 %): report the duty cycle with `n` and name no branch.

Reported alongside, not as a gate: the per-frame `|v|` change across **[A6a entry -> A6b
entry]**, which is A6a alone and therefore contains clamp #6 and W1 — i.e. §21.5's `I_a6a`
interval, on the speed channel instead of the lateral one.

## 5 What STEP 2 will NOT do

No `mashedmod/src` change, no knob, no clamp, no fitted constant, no tracker mutation
outside `re-classify`. AI slots 1+ (`VehiclePhysicsRun.cpp:702`) untouched. The run spawns
one `MASHED.exe`; its PID is tracked and **only** that PID is killed.

If Branch CLOSED fires, a FIX is still **not** authored in this attempt unless the port's
gate state is also measured — which needs a port-side diagnostic and its own
pre-registration. Stated now so the attempt cannot claim a fix it did not earn.
