# D2 attempt 13, STEP 0 — retroactive collateral review of attempt 12

Read-only. No game was launched, no capture was re-taken, nothing in `original/`
was touched. Every number below comes from files that already existed at
`2f2bf51f`.

## 0a. Did a tool already do this? No, and here is the check

| tool | two arms? | whole record? | log/CSV channels? | noise floor? | banded? |
|---|---|---|---|---|---|
| `re/tools/statediff/statediff.py` | yes, **bit-exact** | yes | no | `--mask` from a prior run | no |
| `re/tools/statediff/field_trace.py` | no, one capture | no, 11 named fields | no | no | no |
| `re/tools/statediff/msd_fields.py` | no, one capture | no, offsets you pass | no | no | no |

`statediff.py` is the closest and it is still the wrong instrument for a
cross-side review: it pairs frame indices and calls any non-identical dword a
divergence, so an original-vs-port run is RED on the first frame for ~every
field and the table carries no information. So **one** new generic tool was
added rather than another per-attempt `aN_*.py`:

**`re/tools/statediff/collateral.py`** — channels `msd:` (every dword of the
0xd04 record), `a6a:` (`MASHED_A6ADUMP`), `kv:` (motion/friction/a5g diag logs),
`csv:` (probe CSVs, site-aware); any number merged per arm. `--floor-a` /
`--floor-b` take a **same-arm repeat** and measure the noise floor from it;
`--anchor FIELD:OP:VALUE` bounce-aligns both arms; `--speed FIELD` bands every
statistic. Two modes, and picking the wrong one invalidates the table:

- `--mode paired` — frame-by-frame, ranked by first frame past the floor.
  Correct for **same-side** A/B.
- `--mode banded` — each arm banded on **its own** speed, band medians compared.
  Required **cross-side**: the trajectories separate, so a frame-paired
  "divergence" is just that mismatch re-reported per field (memory
  `band-scored-off-regime-is-not-a-measurement`). The first run of this review
  demonstrated it: frame-paired, the port's `+0x9e4` read 31.9 against the
  original's 1281.6 in the "1000-1500 band", which is a statement about
  alignment and not about any field.

One defect was caught in the tool before any number was read from it: a floor of
**exactly 0.0** (the two repeats land the band median on the same bit — the
strongest possible floor) was printing as `n/a`, the same cell as *no repeat
data for this band*. Now `exact` / `0-floor` / `N.Nx` / `no-floor` are four
distinct tags. Memory `absent-log-proves-nothing-run-a-control`.

## 0b. A6a's write set and callees, by RVA

Extracted mechanically from `verify/d2_sink_20261001/a6a_disasm.txt` —
**1243 instructions, `0x00467650..0x00468989`, full capstone coverage with no
early stop** (memory `capstone-sweep-stops-at-bad-byte`). Every store whose
destination is a memory operand based on **ESI** (the record base, proven by
`lea eax,[esi+0x9b0]` at `0x00467660`) or on an **EDI** alias. The EDI aliases
are included on purpose: memory `offset-grep-misses-dword-index` — an offset
grep alone misses `lea edi,[esi+0x9b0]` at `0x00468633`.

**20 distinct slots, 22 CALL sites, 7 distinct callees.**

| record offset | stores | sample RVAs |
|---|---:|---|
| `+0x490` / `+0x494` | 2 / 6 | `0x00467982`, `0x00467978` |
| `+0x9b0` (also `edi+0`) / `+0x9b4` / `+0x9b8` | 1+6 / 5 / 5 | `0x00467aee`, `0x00468833`, `0x004688ca`, `0x0046894a` |
| `+0x9bc` / `+0x9c0` / `+0x9c4` | 2 / 4 / 4 | `0x0046858c`, `0x00468925`, `0x0046895a` |
| `+0x9e4` / `+0x9e8` | 2 / 1 | `0x00467673`, `0x004686cc`, `0x00467685` |
| `+0xb14` / `+0xb18` / `+0xb1c` / `+0xb20` | 3 / 3 / 3 / 2 | `0x00467cb7`, `0x00467c8f` |
| `+0xbf4` / `+0xbf8` | 7 / 4 | `0x00467d08`, `0x00467dd7` |
| wheel `edi+0x70/0x74/0x78` = `0x1a4 + w*0xc4 + 0x70..0x78` | 2 each | `0x00467f56..0x00467f70`, `0x00468327..0x00468337` |

| callee RVA | call sites | identification |
|---|---:|---|
| `0x004c3ac0` | **9** | `RwV3dLength` (vector by pointer) |
| `0x004a2c48` | 7 | — |
| `0x0040e350` | 2 | — |
| `0x0040e340` / `0x004c39b0` / `0x004c4d20` / `0x004c3df0` | 1 each | — |

Committed as `re/tools/statediff/scope_a6a.txt` so the classification is a file,
not a paragraph. `downstream` is cited, not assumed: `+0x958/+0x95c/+0x960`
(world position advanced from the velocity A6a writes, memory
`msd-world-position-is-the-0x928-matrix-row`, `scenario_launch.py:187-190`) and
`+0x9d4/+0x9d8/+0x9dc` plus the rest of the `+0x928` block (the orientation
integrated from the angular velocity at `+0x9bc..+0x9c4`,
`scenario_launch.py:189`).

## 0c. The two noise floors, measured

**PORT — the floor is EXACTLY ZERO.** `--mode paired` over
`d2_sink_20261001/ctl/a6a_dump.log` against `arm/a6a_dump.log` (the same recipe,
differing only by attempt 12's default-OFF `MASHED_A5GDIAG` line):

> **205 of 205 paired fields bit-identical on 1627 of 1627 aligned frames.**
> Zero divergent fields.

So the standalone is fully deterministic across boots on this arm, attempt 12's
source change really was a no-change control, and **every port-side band median
in this project rests on a floor of 0**. `ctl` has 1629 lines to `arm`'s 1627,
a kill-timing difference in the trailing frames only.

**ORIGINAL — 533 of 833 record dwords bit-identical, first real divergence at
frame 771.** `orig_fp1.msd` against `orig_fp2.msd` (identical argv, same HEAD
`a7311790`), 2332 frames each, index-aligned with no anchor needed. The 300
fields that do move are led by `+0x214/+0x21c`, `+0x2d8/+0x2e0`, `+0x39c/+0x3a4`,
`+0x460/+0x468` — which is an independent confirmation of 0b: those are exactly
the per-wheel `+0x70`/`+0x78` force slots A6a writes.

## 0d. The cross-side table — outside-scope rows

`--mode banded`, arm A `orig_fp2.msd` (floor `orig_fp1.msd`), arm B
`d2_sink_20261001/arm/a6a_dump.log` (floor `ctl/`), banded on `+0x9e4`, scoped
by `scope_a6a.txt`. 21 fields are paired across the two sides (the port emits no
`.msd`, so only the quantities both channels carry are comparable); 812 record
dwords are original-only and 184 port fields are port-only, both reported as
unpaired rather than silently dropped.

**The port-field-to-record-offset map is source-cited, not inferred**:
`VehiclePhysicsRun.cpp:1301-1304` prints `snap.vel/av/bf/sp/angsp/gc` and
`snap.steer` as `F(r,0x9b0..0x9dc)`, `F(r,0x9e4)`, `F(r,0x9e8)`, `F(r,0x9e0)`
and **`F(r,0x1a8)`** — the same offsets `scenario_launch.py:187-190` reads on the
original. Wheel axes are `0x1a4 + w*0xc4` `+0x7c`/`+0x84` (`a8_wheelaxis.py:34`).

### OUTSIDE SCOPE (not in A6a's write set, no cited chain from it)

| field | first frame | n orig / port | ORIG | PORT | magnitude | floor |
|---|---|---:|---:|---:|---|---|
| **`+0x1a8` steer angle, 100-150** | n/a (banded) | 45 / 30 | **33.867** | **32.527** | **−4.0%** | **0** |
| `+0x1a8`, 150-260 | | 71 / 14 | 33.867 | 28.858 | **−14.8%** | 0 |
| `+0x1a8`, 260-500 | | 60 / 15 | 33.867 | 19.897 | **−41.2%** | 0 |
| `+0x1a8`, 500-1000 | | 153 / 15 | 33.867 | 21.731 | **−35.8%** | 0 |
| `+0x1a8`, 1000-1500 | | 197 / 18 | 33.867 | 24.060 | **−29.0%** | 0 |
| `+0x1a8`, 1500-2000 | | 339 / 22 | 33.867 | 26.882 | **−20.6%** | 0 |
| `+0x220`/`+0x228` w0 axis | — | 45..339 / 14..30 | see CSV | see CSV | up to 2215x floor | 0 .. 2.9e-4 |
| `+0x2e4`/`+0x2ec` w1 axis | — | " | " | " | up to 2215x floor | " |
| `+0x3a8`/`+0x3b0` w2 axis | — | " | " | " | up to 843x floor | " |
| `+0x46c`/`+0x474` w3 axis | — | " | " | " | up to 843x floor | " |

Med speeds, original / port, per band: 126.2/110.8, 193.0/192.0, 371.1/340.8,
821.3/735.8, 1281.6/1262.8, 1778.8/1683.8.

**Only the `+0x1a8` row is read as a finding.** The eight wheel-axis rows are
signed world-frame components of a direction, so their medians over a lap are
dominated by where the car is pointing, not by any law — they are in the CSV and
are **not** claimed as a defect here. `+0x9e0` (grounded) is **within the floor
in every shared band**, the only paired field that is.

### The `+0x1a8` row, stated precisely

`+0x1a8` is the wheel-0 steer angle in degrees, written by **A4 `0x00470670`**
inside `if (input[0] != 0)` (`VehicleControl.cpp:118`, `field_trace.py:45-48`).
**It is NOT in A6a's write set** — the 0b scan finds no `esi+0x1a8` store
anywhere in `0x00467650..0x00468989`. So this is collateral by construction.

> **The ORIGINAL holds `33.867` in every band, to every printed digit, over a
> 14x range of speed, with a measured noise floor of exactly 0 across two
> boots. The PORT holds a speed-dependent value that is short in all six
> bands, worst at 260-500 where it is 41.2% short.**

### Why this is reported and NOT acted on in this step

§21.10 measured the port's **front wheel-axis deflection** as 10.0% / 15.4% /
20.8% short and speed-dependent, called it "a magnitude error, not a missing
write", and left **[U-9156]** open because "its writer is not located".
§21.10's port-side steer column read `+1.000` — that is motion_diag's `steer=`
**input command**, not an angle, so §21.10 never compared the two sides'
`+0x1a8`. This row does, on the same record offset on both sides.

Per the step-0 charter this is **exploratory and cannot change any
pre-registered verdict**. It bears on `l_60` (the wheel forward axis is one of
`ld4`'s two inputs, and it is the non-circular one), so it is **noted here and
must be pre-registered before anything is built on it**. It is not a
pre-registered result of this attempt and no fix follows from it in step 0.

## Artefacts

- `re/tools/statediff/collateral.py` (new, standing), `scope_a6a.txt` (new)
- `collateral_step0_cross.txt` / `.csv` — the cross-side banded table
- `collateral_step0_portpair.txt` / `.csv` — the port's zero floor
