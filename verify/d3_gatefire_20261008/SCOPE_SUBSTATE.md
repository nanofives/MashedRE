# SCOPE — porting the race sub-state machine (`DAT_0063ba8c`)

Date 2026-10-08. Scoping only — **no code written, nothing run beyond re-reading committed
captures and plates.** `original/` untouched. No C-level.

## 0. What the port has today

```cpp
int aib_game_sub_mode() { return 6; }   // TrackRenderer.cpp:100
```

`FUN_0040e350` returns **`DAT_0063ba8c`** (`re/analysis/ai_update/0x0040e350.md:14`), and
`FUN_0040e350() == 6` is documented as **race mode** (`0x0045bba0.md:23`). So the port hardcodes
"always racing" and never represents any other phase.

> **Terminology caution.** Memory `no-driving-hud-race-ui-is-standings` reads `DAT_0063ba8c` as
> "3 = driving, 5/6/7 = standings". That note is about the **HUD/draw lane**. In the **AI** lane
> the measured meaning is the reverse of what it suggests: `6` is the mode whose block `ControlStep`
> runs, and the original is in `3` **before** racing. Do not carry that mapping across lanes.

## 1. What the original actually does — measured, not assumed

`o_t3.msd.aistep.csv`, v1, 903 calls:

| calls | substate | `c4` |
|---|---|---|
| 0-682 | **3** (682), `4` (1) | **`0` on all 683** |
| 683-902 | **6** (all 220) | 255 ×140, 64 ×49, 0 ×31 |

The original runs **683 calls in sub-state 3 with the AI not accelerating**, then transitions to
**6** and races. `ai_ctrl_window.py`'s anchor (`i0` = first `c4 != 0`) lands **exactly on the 3→6
transition**, so the scored window *is* the racing phase.

**This corrects [`RESULT_CALLWISE2.md`](RESULT_CALLWISE2.md) §2.** I described phase 3 as "a start
countdown" and said the windows "cover different phases". More precisely: **both windows are each
side's first 220 calls of mode-6 racing.** The difference is that the original reaches mode 6 after
683 calls of phase 3, while the port is in mode 6 from frame 0. Whether those two mode-6 starts
correspond to the same *physical* race moment is **[UNCERTAIN]** and depends on what phase 3 does.

## 2. Who writes `DAT_0063ba8c`

`FUN_004111c0` — the race dispatcher, **13,730 bytes**, C2, ~58+ callees
(`re/analysis/bucket_util_0040e4b0_0042f790/0x004111c0.md`). It both **switches on**
`DAT_0063ba8c` and **writes** it:

| case | meaning (plate) | writes |
|---|---|---|
| 1 | race init | ends `DAT_0063ba8c = 2` |
| 2 | → shared path | — |
| 3-7, 9, 10 | per-state helpers + `FUN_0040fc00` | 9 → `= 10` |
| 8 | `FUN_00414220` + 2 | `= 2` |
| 0xb | finish | `= 9` |
| shared tail | — | `= 3` if `DAT_005f29b8 == 100000`, else `= 4`; `= 4` if `DAT_007f0fd0 == 6` |

**So the state variable is driven by a 13.7 KB dispatcher.** Porting `FUN_004111c0` whole is not a
leg; it is a subsystem.

## 3. Three sizings, smallest first

**(a) Transition-only stand-in — small, dishonest if mislabelled.** Drive `DAT_0063ba8c` through
`3 → 6` on a timer sized from the capture (683 calls), leaving the dispatcher unported. Cheap, and
it would align the CALLWISE anchors — but it is a **bridge**, not a port, and it would fabricate a
transition the port does not earn. It must be knob-gated and labelled as such, exactly like
`MASHED_SLOT_PLAYER`.

**(b) Port the state writes only — medium.** Transcribe just the `DAT_0063ba8c` assignments and
their guards from `FUN_004111c0`'s cases, leaving the per-case helper calls out. Needs the guards'
inputs live: `DAT_005f29b8` (which `RESULT_A360.md` found reads **0** standalone — image `.data`
the port never loads) and `DAT_007f0fd0` (live, `MASHED_ROUND_RULE`). **`DAT_005f29b8` is already
known-dead**, so this is partly blocked on the same substrate problem as everything else in this
lane.

**(c) Port `FUN_004111c0` — a subsystem.** 13.7 KB, C2, ~58 callees, and it is the function that
already owns race init, the 4-slot spawn loop, the finish scan and the shared tail. This is a
multi-session effort and should be scoped against ROADMAP D3/D4 rather than squeezed into U-9186.

## 4. What this would and would not fix

**Would plausibly fix** (all three traced to the constant in `RESULT_CALLWISE2.md` §3):

- the `bias374` monotone ramp — sub-mode 5 would become reachable, so `FUN_00418560` Branch A could
  re-zero `0x007f0ff8`;
- the inert mode-5 branch already ported this session;
- the CALLWISE anchor mismatch.

**Would NOT by itself fix:** branch 2's firing count. The condition also needs `Prog(v) == 0` and
`6.5 < Prog(0)` to co-occur (`RESULT_WIRE2.md` §5). Necessary, not sufficient.

> **CLOSED 2026-10-08 by [`RESULT_PHASE3.md`](RESULT_PHASE3.md).** Phase 3 **is** a countdown:
> `FUN_004102f0` (172 B) decrements `DAT_005f29b8` per frame and hands off when two gates clear.
> The chain is **3 → 4 → 6**, not 3 → 6, and the capture's lone substate-4 sample is that
> transient. Consequence for §3: sizing **(a) is no longer a fabrication** — modelling a countdown
> as a timer is faithful in kind — but it still needs the real chain, a live `DAT_005f29b8`
> (measured **0** standalone, `RESULT_A360.md`) and the two exit gates.

**[UNCERTAIN, now closed — see above]:** whether phase 3 is a countdown, a rolling start, or something else. The capture
shows only that the AI does not accelerate during it. Reading `FUN_004111c0`'s case-3 helper
(`FUN_004102f0`) would settle it and is cheap.

## 5. Recommendation

1. **Open a `DEFERRED.md` row** for the race sub-state machine, scoped as **(c)** with **(a)** named
   as an explicitly-labelled bridge if a measurement needs it sooner. Re-pickup condition: any lane
   blocked on `aib_game_sub_mode`'s constant — there are now three.
2. **Do not** take (a) silently to make CALLWISE align. A fabricated transition would make the
   anchors match and the comparison meaningless, which is the failure mode this session has hit
   repeatedly from the other direction.
3. Read `FUN_004102f0` first (§4) — one decompile, and it tells us what phase 3 *is* before anyone
   commits to modelling it.
