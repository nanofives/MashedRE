# RESULT — U-9193: the "zeroed phase" explanation is REFUTED. `+0x9c0` is not the yaw rate.

**RAN 2026-10-05.** Pre-registration `PREREG_OMEGA.md`, committed **unrun**. Binary anchor verified
before arming: `original/MASHED.exe.unpatched` SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`, matching the recorded anchor.

**Headline: reading `+0x9c0` at a known program point changed NOTHING.** On a matched denominator the
entry hook and the per-frame snapshot give the same counts to within one row. So U-9193's "the `.msd`
samples a zeroed phase" candidate is **refuted**, and its other candidate stands: **`+0x9c0` is not
`omega.y`.** The original's yaw-rate field is **still unlocated**, and U-9191's
generated-vs-accumulated question therefore **cannot be answered yet**.

**My registered prediction was WRONG** — I predicted both the phase explanation and the rate
identification would pass. **And one of my own gates was ill-posed again** (§3).

---

## 1. Arm and controls

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_omega_20261005/o1.msd \
    --statediff-car 1 --statediff-aistep --axis-probe --hold 40
```

The standing original-side (b) recipe plus `--axis-probe`. `--statediff-car 1` targets **AI car 1**.
Entry hooks only, no `onLeave`, no mid-function probe. **Game stayed stable for the whole 40 s** —
2433 frames, no crash, `err: null` on every agent.

- **KA-A PASS.** `fwdConst = [0, 0, 1]` — the probe reads `DAT_00614708` at the right VA, so its
  float reads can carry a verdict.
- **KA-C PASS.** 2433 A6a-PRE rows for car 1, of which **437** in the active-drive regime
  (`+0xbf8 == 0`, `speed > 1.0`) against a registered floor of 30.
- **Call-rate budget held.** A6a fired 9672 times (4 cars × 2418 frames), 7239 skipped by the ESI
  filter; A6b 9672. ~480 callbacks/s across both sites, under the ~1000/s destabilisation line, and
  the run completed cleanly.

### KA-B FAILED — and it is a real finding about the inherited probe

`a6bEsiRec = 0 of 9672`. **ESI does not hold a record pointer at A6b** on a single row. Per the
pre-registration, POST rows are therefore **unattributable and only A6a-PRE is used** — which is what
every number below rests on.

**Precise consequence, not overstated.** 9672 POST rows against 2433 PRE rows is **3.98x**,
consistent with A6b firing once per car per frame while the unfiltered hook reads **car 1's record
every time**. So POST rows are **redundant duplicates of one record**, not another car's data
misattributed. **U-9175's committed `b18nzPost = 0` is unaffected** — zero is zero under any
duplication, and its arm had one car live. What the gap *would* break is any **per-row POST
statistic** on a multi-car arm, which would be ~4x over-weighted. Now measured instead of assumed.

---

## 2. G-ZERO — PASS, and U-9175 extends

`+0x9bc` and `+0x9c4` are **exactly 0.0 on all 2433 A6a-PRE rows** (`wxNz = 0`, `wzNz = 0`). U-9175
measured that for the **player** from `.msd` snapshots; it now also holds for an **AI car** at a
**known program point**. The "no pitch/roll torque on flat ground" reading is not a phase artifact.

---

## 3. G-OMEGA — the gate was ILL-POSED, and the hypothesis it tested is refuted

`+0x9c0` non-zero, same two denominators on both instruments:

| denominator | A6a entry (known phase) | `.msd` per-frame snapshot |
|---|---|---|
| **active-drive rows** | **312 of 437 = 71.40 %** | **312 of 436 = 71.56 %** |
| all rows / all frames | 317 of 2433 = 13.03 % | 317 of 3623 = 8.75 % |

**The same 312 and the same 317.** The phase makes no difference at all.

**The gate as I drafted it "passes" and that verdict is void.** Its PASS clause was "non-zero on
≥ 50 % of A6a-PRE **active-drive** rows" — met at 71.40 % — but the figure it was meant to be
compared against, the snapshot's **8.75 %**, was computed over **all frames**. Two different
denominators. Scored properly, the snapshot gives **71.56 %** on active-drive rows, essentially
identical to the entry hook's 71.40 %.

So the registered PASS does **not** support its registered conclusion. **The "zeroed /
part-accumulated phase" explanation is REFUTED**, which is the opposite of what the gate's nominal
verdict would have said if read carelessly.

This is the **same error class I have been policing all session** — a statistic compared across
mismatched denominators (memories `rate-stats-per-sample-not-totals`,
`band-on-speed-compares-different-moments`) — and it is the **second ill-posed gate of mine today**,
after yesterday's position-bound-vs-angle H-JITTER and this session's mis-scoped KA-1. Recorded as a
drafting failure, not smoothed over.

---

## 4. G-RATEID — FAIL at the known phase too

Single-scale fit `wrap180(Δ atan2(+0x9dc, +0x9d4)) = k · (+0x9c0)` across consecutive A6a-PRE rows:

| | Pearson | required |
|---|---|---|
| A6a entry (this run) | **0.057699** | 0.95 |
| `.msd` snapshot (leg 3) | 0.068251 | 0.95 |

No improvement — slightly worse. And the pattern reproduces exactly:

- **288** turning pairs (`|Δheading| > 0.01` deg), and on **0 of them** is `+0x9c0` zero. The
  **support matches turning perfectly** on both instruments.
- But the ratio scatters: `Δheading / +0x9c0` median **-6.8962**, p10 **-1.276e4**, p90 **+73.73** —
  sign-inverted, four orders of magnitude of spread.

So `+0x9c0` **co-occurs with turning exactly and is not proportional to it at any sampling phase**.

**One observation, offered as an observation and not a conclusion:** the ratio's median is
**-6.90** here and **-8.87** on the `.msd` — the same order and the same sign. There is a
characteristic scale buried in it, with huge scatter around it. That is the shape of a quantity
**related** to the yaw rate but accumulated or scaled non-uniformly (for example over a variable
substep count), rather than of an unrelated field. **What `+0x9c0` actually is remains
unestablished and is deliberately not guessed.**

---

## 5. The cross-side leg was not reached — as registered

`PREREG` §5 made the GENERATED-vs-ACCUMULATED comparison **conditional on G-RATEID** and registered
up front that it "may well not be reachable in this session, and that reporting it as unreachable is
the correct outcome rather than forcing a number." G-RATEID failed, so **no cross-side rate number
was computed** and U-9191's open question is untouched.

---

## 6. What is owed

- **U-9193 resolves one way:** the phase explanation is dead; `+0x9c0` is not the yaw rate. The row's
  remaining question narrows to **what field carries the original's yaw rate** — and note the body
  forward row `+0x9d4`/`+0x9dc` demonstrably *does* rotate, so the rate exists somewhere.
- **The next instrument is not another field guess.** Locating the producer of `+0x9d4`/`+0x9dc` is a
  **static** question: `BodyOrientationIntegrate` (`FUN_0046e9e0`) is already named as the writer, so
  the right next step is to **read what that function actually reads** in Ghidra and let the
  disassembly name the rate input, rather than probing offsets by trial. That also directly addresses
  U-9193's second open question — what drives the original's orientation if `omega.x`/`omega.z` are
  identically zero.
- **Fix or document the A6b gap** in `--axis-probe` before anyone uses POST rows on a multi-car arm.
  The clean fix is to filter A6b on whatever register *does* hold the record there, which is a static
  question too.
- **No C-level moves.** Probe columns and an analysis pass only; nothing reimplemented at an RVA.
- **No D2 WATCH.** `PREREG` §6 made one owed only if the cross-side leg ran and showed a rate
  divergence. It did not run. Field identification on the **original** implicates no port code.
- **(b) is unchanged** and still failing; the 2026-10-02 counterfactual matrix had no arm passing it
  on any car.
- **Existing `--axis-probe` results stay valid:** columns were appended, A6a's filter untouched, no
  existing read changed.
