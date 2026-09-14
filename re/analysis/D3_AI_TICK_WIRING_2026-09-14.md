# D3 AI — wiring the ported tick, and the first original-vs-standalone control diff

**Date:** 2026-09-14. **Follows:** `re/analysis/D3_AUDIT_2026-09-14.md` (step 1).
**Covers:** D3 kickoff step 2 (AI measurement). Shape: **measured / refuted / open**.

**Verdict up front: the D3 AI gate does NOT close.** The ported tick now executes for the
first time and produces control bytes, but they do not match the original's, and the cause
is localised to a stub the port already records. `MASHED_AI_TICK=1` is a **default-OFF**
gate, which D3's rule forbids for a finished port — it is recorded here as the open gate,
not presented as a landing.

---

## 1. What changed in the tree

| Change | File | Why |
|---|---|---|
| `Ai_Standalone_Tick()` is called | `D3d9Render/TrackRenderer.cpp` (faithful-nav block) | It had **zero call sites** (audit §1.1) |
| `AiBridgeSnapshot()` added + called | same | `g_aib.pos/.vel/.alive` were **never written**, so `aib_own_xz` returned (0,0) and `aib_alive` returned 0 for every car — the tick's `if (car_alive(v)==1) VehicleStep(v)` gate could never fire. The tick was **inert as well as uncalled**. |
| `AiStepDump()` added (`MASHED_AI_STEPDUMP=<path>`) | same | standalone side of the measurement; same CSV columns as the original-side capture |
| `TickTrace()` added (`MASHED_AI_TICKTRACE=<path>`) | `Ai/AiStandalone.cpp` | stage bisect without a debugger |
| `CarSlotStateSet` guarded | `Ai/AiStandalone.cpp` | fixed the AV — see §2 |
| `--statediff-aistep`, `--statediff-aictrl`, `--poke-ctrl-slots` | `re/frida/scenario_launch.py` | original-side instrument — see §3 |

The tick is gated on `MASHED_AI_TICK=1`. **Default-build behaviour is unchanged**: the
Option B motion model still drives the opponents, and with the gate unset not one line of
the new code runs. The ctrl bytes are produced but **not consumed** — measure before
flipping a default, per the kickoff's phase order.

---

## 2. MEASURED — the first wiring AV'd, and why

Wiring the tick unconditionally made `mashed_re.exe` exit `0xC0000005` before the first
screenshot. Isolation was clean:

| Run | `MASHED_AI_TICK` | Result |
|---|---|---|
| A (control) | unset | ran to completion, 2 screenshots |
| B | `1` | `rc=3221225477` (`0xC0000005`) |

`MASHED_AI_TICKTRACE` stopped after `pre-slotstate`, putting the fault in
`CarSlotStateSet` (the port of **FUN_0040e480**), whose body is
`*(int*)(*(uint32*)0x005f2770 + v*4 + 0x34) = state`.

The port's own comment argued this was safe because *"all 40 references … are READ, never
WRITE, so the pointer's value is a load-time .data constant … safe to dereference directly
in the image-pad, same as any other kSpline*/kAiState* address."* **That reasoning is right
about the original and wrong about the standalone.**

Read directly from `original/MASHED.exe.unpatched`: RVA `0x001f2770` lands in section
`.data` (raw size `0x4d000`, initialised on disk) at file offset `0x1f2770`, and holds
`*(uint32*)0x005f2770 == 0x005f2728`. The original loads that word. **The standalone does
not load the original's `.data`** — its image-pad owns the RVA range zero-filled, the same
caveat `Ai/AiState.h` already records for the `DAT_005ccXXX` tuning constants. So `base`
was `0` and the write landed at `0x34`.

The comparison to `kSpline*` / `kAiState*` is what made this look safe and is the actual
error: those are **direct addresses this port itself writes** into the pad, whereas this one
needs a **value** the pad never receives. **A pointer read out of the image-pad is never
safe to dereference.** Generalisable — any other ported leaf that derefs a pad-resident
pointer has the same latent AV.

Fixed by guarding `base == 0`. The original's effect (writing car slot `v`'s state at
`+0x34 + v*4` of the `0x005f2728` table) is **not reproduced standalone** — open stub.
With the guard the tick runs to completion: trace shows
`enter → splines-ok → pre-slotstate → pre-rubberband → post-rubberband →
pre/post-vehiclestep v=1,2,3 → leave`.

---

## 3. MEASURED — the original-side captures were invalid, and the cause is the launcher

This affects more than D3, so it is stated separately.

**Every `scenario_launch.py` race leaves the AI output-slot table at its `.bss` zeros, so
all four cars write controller 0's ctrl block.**

Measured, first capture (`orig_ai_blocks.msd.aictrl.csv`, 2710 frames, 4 cars): block 0
carried 221 distinct `(steer0,steer1,accel,brake)` tuples; **blocks 1, 2 and 3 were
all-zero on every frame**, and identical to each other on 2710/2710. A control-step capture
then showed **3 cars sharing 1 ctrl block**. The poke reported the table's pre-state
literally: `ctrl slot table was [0,0,0,0]`.

Not a mis-read of the byte map — the map is right. `FUN_00418560` decompiled (pool0,
2026-09-14) computes `iVar7 = (&DAT_007f1a14)[param_1 * 4]`, which on an `int*` **is** byte
offset `param_1*0x10`, and derives the block as `0x007f1038 + iVar7*0x4c` (`0x13` DWORDs);
its writes land at block `+0`, `+1`, `+4`, `+5`. `Ai/AiState.h` is correct as written.

The table is populated by the **frontend race-launch path**, which the warp poke skips:
- `FUN_0042b9e0` @`0x0042bab0` resets all four entries to `-1`.
- The allocator at `0x0043f870..0x0043f8a4` scans for the lowest index no entry holds and
  commits it — `MOV dword ptr [EBX],ESI` @`0x0043f895` — then `CarSlotStateSet(car, 2)`
  @`0x0043f89a`. Table bound `0x007f1a54` @`0x0043f832` confirms 4 entries × `0x10`.

`--poke-ctrl-slots` writes `[0,1,2,3]`, which is what that allocator produces for four cars.
After the poke: **3 cars, 3 distinct blocks**. Contrived state (C3-grade), same class as
`--poke-lap` / `--poke-collect`, and it restores what the original itself commits.

Cross-check: `TrackRenderer.cpp` `Ai_BridgeLoad` already commits `slot = v` for the
standalone. **Our port had this right; the original-side capture was the side that needed
repairing.**

**Scope of the damage.** Any prior AI-behavioural observation taken through
`scenario_launch.py` was made on a game where the AI cars overwrite each other's commands.
The A8/D2 physics captures are **not** affected: they drive block 0 directly through the
cook injector (`0x007f1038`, descriptor recorded in the A8 provenance sidecars) and never
read a per-car AI block.

---

## 4. MEASURED — the control diff

Original: `verify/d3_ai_20260914/orig_step_slots.msd.aistep.csv`, 5470 calls, track 0
(Training), mode 10, 4 cars, slot table repaired, no drive injector.
Standalone: `verify/d3_ai_20260914/sa_step.csv`, 1307 calls, `MASHED_AI_TICK=1`.

| Observable | ORIGINAL | STANDALONE | |
|---|---|---|---|
| accel `c4` | `0` on 80-96% of calls; `255` occasionally; `64` seen on car 1 | **`255` on 100% of calls, all three cars** | RED |
| brake `c5` | `255` on 4-5% of calls | `255` on **42-49%** of calls | RED |
| steer `(c0,c1)` distinct values | 33 / 85 / 96 per car; graded magnitudes `6, 8, 9, 19, 23, 255` | **2-3 per car**, only `(0,0)`, `(0,255)`, `(255,0)` | RED |
| `ai_mode` | `0` mostly, with `3` and `7` excursions | `0` only | RED |
| `(ai_type, ai_spline_idx)` | car 1 switches `(0,0) → (0,1)` | `(0,0)` only | RED |

**No observable matches.**

### 4.1 The steer delta is localised, and the port already names the cause

The standalone's steer is **bang-bang** — full lock or nothing — while the original's is
**proportional**. `Ai/AiStandalone.cpp:427-431` predicts exactly this, in the port's own words:

> *"The ported ControlStep bands below full-lock the steer (ctrl=255) for any bearing error
> in 30..180deg … correct in the original because its curvature-walk target (FUN_00443300 /
> FUN_00443dc0 tail) keeps the error <30deg, but that refinement is STUBBED here."*

So: **the bands are faithful; their INPUT is not.** The bearing error fed to them is wrong
because `FUN_00443300` and the `FUN_00443dc0` curvature-walk/wall-march tail are stubbed
(`AiStandalone.cpp:70`, the STUBBED ledger). The measurement is the first evidence that
this prediction is correct, and it makes those two functions the **named gap for the D3 AI
gate** — not the bands, and not the steer sign.

### 4.2 Corrections to the step-1 audit

Two rows of `D3_AUDIT_2026-09-14.md` §1.3 need amending, both from reading the code more
closely than the surrounding comments:

- **`MASHED_AI_PUREPURSUIT` polarity — the audit's "dead in the exe" was right only until
  this session's wiring, and the neighbouring in-source comment is inverted.** The lambda at
  `AiStandalone.cpp:437-443` is `return (e && e[0] != '0')` — **OPT-IN, default OFF**, so
  the **verbatim bands are the default path** inside `ControlStep`. The comment three lines
  above it says *"Env MASHED_AI_PUREPURSUIT=0 reverts to the verbatim bands"*, which is the
  exact inverse; the lambda's own trailing comment (*"OPT-IN (default off -> verbatim
  bands)"*) is the correct one. **Polarity is right for D3** — the flag turns a ported
  behaviour OFF. Only the stale comment needed fixing.
- `MASHED_AI_STEERFLIP`, `MASHED_AI_NAV` are likewise no longer dead once `MASHED_AI_TICK=1`.
  `STEERFLIP` only affects the pure-pursuit shim, so it is inert on the default (bands) path.

### 4.3 [UNCERTAIN] — the accel delta is not localised

The original commanded `c4 = 0` on 80-96% of calls. A racing AI coasting that often is not
self-evidently the steady-state behaviour, and the capture has a known caveat: the player
was idle (control-4 pulses only, no drive injector) and cars 1 and 2 stopped being stepped
after ~27% of frames while car 3 continued to ~97%, which is consistent with
`FUN_00418860`'s `car_alive` gate dropping them. So the original-side accel distribution
may reflect a degenerate race rather than the law.

Evidence missing: an original-side capture on a race the AI actually completes.
Next command:

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/<dir>/orig_lap.msd --statediff-car 1 \
    --statediff-aistep --statediff-drive --hold 120
```

(`--statediff-drive` keeps the player moving so the race progresses; check the per-car call
counts are comparable before reading the accel histogram.)

---

## 5. Open, in priority order

1. **`FUN_00443300` + the `FUN_00443dc0` curvature-walk/wall-march tail** — the named cause
   of the steer mismatch (§4.1). Porting them is the D3 AI gate's critical path.
2. **The accel delta** (§4.3) — needs a non-degenerate original-side capture first.
3. **`CarSlotStateSet` standalone effect** (§2) — currently a guarded no-op.
4. **Consume the ctrl bytes.** The tick produces them; the Option B motion model still
   drives. Switching the consumer over is gated on 1 and 2.
5. **`MASHED_AI_TICK` must be deleted, not inverted**, once 1-4 land — a default-OFF gate on
   a ported behaviour is what D3 exists to remove.
