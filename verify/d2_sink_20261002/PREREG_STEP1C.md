# PRE-REGISTRATION — D2 attempt 19, STEP 1C: why the drift gate is OPEN on the port

Committed **before the run**. Base `4bbb5db7`.

STEP 1B named the site: **`wcs_drift`**, the airborne lateral-drift write at
`WheelContactSolver.cpp:337`, carries **100.00 %** of `D_wheel` and **84.33 %** of the port's
`T_post` (47 hits over 29 frames, `d = 222..250`, n = 29, median speed 633.2, median frame 237,
`d` from release R = 1, `L = 0`). `wcs_fric` and `wcs_imp` fire **0** times.

The registered prediction *"`wcs_drift` fires 0 times because `gnd == 4.0`"* was **NOT MET**,
and the reason is in the same log: the `b=` field on those 47 lines is the gate's own `gc`, and
its median is **2**, not 4.

**The gate transcription is CORRECT.** From `original/MASHED.exe.unpatched`
(`py -3.12 re/tools/disasm_fn.py 0x004701b2 0x00470260`), read this session:

```
004701bf  fild [esp+0x14]        ; ST0 = (float)<grounded count>
004701c3  mov eax,[edi+0x9f0]
004701c9  cmp eax,ebp            ; ebp = 0
004701cb  fst  [edi+0x9e0]       ; STORE the count to +0x9e0, ST0 KEPT
004701d1  jne 0x470449           ; +0x9f0 != 0 -> skip
004701d7  fcom [0x5d757c]        ; vs 0.0
004701dd  fnstsw ax
004701df  test ah,0x44           ; C3|C2 -> PF clear only on EQUAL
004701e2  jnp 0x470449           ; skip when count == 0
004701e8  fcomp [0x5cc574]       ; vs 2.0
004701ee  fnstsw ax
004701f0  test ah,0x41           ; C3|C0 -> PF clear on EQUAL and on LESS
004701f3  jp  0x47044b           ; skip when count > 2.0 or unordered
004701f9  ... the drift body
```

`test ah,0x41 / jp` skips on *greater* and on *unordered*, and falls through on *equal* and on
*less*, so the original's condition is **`0 < count <= 2.0`**. The port's
`(gc != kZero) && ((gc < kGroundThr) != (gc == kGroundThr))` is `gc != 0 && (gc < 2 XOR gc == 2)`
= **`0 < gc <= 2`**. **Identical. There is nothing to fix at the gate.**

So the defect is the gate's **input**, and `0x004701cb` stores that same input to `+0x9e0`.

## 1 The original's own value for that input — already captured, not re-derived

Attempt 18, `verify/d2_budget_20261002/RESULT_STEP2.md` §1.2, on the running anchored original:

> **`+0x9e0` at A6a entry has exactly ONE distinct value — `4` — across all 2333 frames of the
> capture**, and A6a reads it twice and writes it zero times over all 1243 instructions of
> `0x00467650..0x0046897b`.

So on the original the count reaching `0x004701e8` is **4 > 2** and **the drift branch never
runs**. That is consistent with `WheelContactSolver.cpp:33-46`'s independent witness: the
velocity is bitwise unchanged across `0x0046f6c0` on **2945 of 2945** samples.

**Gate WS (blocking, static).** `+0x9e0` must have **exactly one writer** on the original —
`0x004701cb` — or the "A6a entry sees the solver's own count" chain is broken. Measured with
`re/tools/fold_sweep.py` (its `+0xbf8` known answer must PASS in the same run), **not**
`findoffset.py --writes`, which is blind to x87 stores and would miss this very `fst`.

## 2 The instrument — one more field on the same default-OFF channel

`wcs_cnt`, emitted **once per `WheelContactSolver` call** immediately after the grounded-count
combine (`WheelContactSolver.cpp:202-207`) and **before** the `bVar16 == 4` drop:

```
a = bVar16 (pre-drop)
b = state0*1000 + state1*100 + state2*10 + state3   (self[0x66], [0x97], [0xc8], [0xf9])
c = bVar4 ? 1 : 0        (the "high-speed but no slow-contact" flag, line 157)
d = iVar8                (its second term, line 156)
```

and the existing `wcs_drift` line gains the same packed state word in its `d=` field, so the
states at the combine and at the gate are both visible.

## 3 Gates and the registered reading

| id | asks | bar |
|---|---|---|
| **WS** | `+0x9e0` has exactly one writer on the original, `0x004701cb`, by `fold_sweep.py` with its `+0xbf8` known answer PASSING | 1 writer |
| **KC1** | every `wcs_drift` line in the window has a `wcs_cnt` line in the same solver call | 100 % |
| **KC2** | the `wcs_cnt` `a=` value equals the `wcs_drift` `b=` value whenever `a != 4` (no drop ran) | 100 % of such calls |
| **EV3** | STEP 1's EV re-scored | must hold |

**Registered reading.** The distribution of the packed state word at `wcs_cnt`, over
`d = 222..250`. Exactly one of the following is true and the run decides which:

- **(i) STATE-2 WHEELS.** Two or more wheels sit at state **2**. Then the port's count is low
  because `bVar16` counts `state == 1` only while `ReassertContacts`
  (`VehiclePhysicsRun.cpp:343-357`) counts `state != 0` — two counters of the same thing
  disagreeing, and `ReassertContacts` is a port-only construct.
- **(ii) STATE-0 WHEELS.** Two or more wheels sit at state **0**, i.e. the demotion branch
  `WheelContactSolver.cpp:170` (`((kState2Lo < fv) && bVar4) || piVar9[0x15] == -1`) is firing.
  Then `c=`/`d=` say whether it is the `bVar4` arm or the `-1` arm.
- **(iii) neither** — report and stop.

## 4 The fix rule — registered before the run

A fix is authored in STEP 2 **only** if the run lands on a **transcription defect at a cited
RVA** — a condition, a constant or a field offset in `0x0046f6c0`'s own code that differs from
the disassembly. Anything else (a missing upstream producer, a stand-in's behaviour, a
port-only construct that has no original) is **reported named and UNFIXED**, STEP 3 runs on the
unchanged build, and it goes to the handoff. **No knob, no clamp, no fitted constant, no
threshold chosen to make the branch close.**

## 5 Run plan

```
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_sink_20261002/p_cnt 90 \
    MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0 \
    MASHED_TITLE=d2-a19-cnt MASHED_D2SINK=<abs>/verify/d2_sink_20261002/p_cnt/sink.log
```

Muted (recipe), `MASHED_WIN_POS=primary-bl` (recipe), PID tracked and reaped by the runner.
No source change outside `if (armed)` and the probe TU. AI slots 1+ untouched.
