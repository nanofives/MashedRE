# RESULT — U-9193 RESOLVED: `+0x9c0` is the yaw TORQUE. The yaw rate is `+0x148`.

**Static read, 2026-10-05.** Ghidra headless via `re/tools/decomp_pc.py` (`-readOnly` against pool
slot `Mashed_pool0`; the Ghidra MCP server is **not** reachable in this session, and
`decomp_pc.py` is the sanctioned no-MCP path — still Ghidra reading the anchored binary, not a
documentation fallback). Constants read **from `original/MASHED.exe.unpatched`** (anchor SHA-256
`BDCAE093…3C0E`) rather than trusted from Ghidra's `_DAT_` rendering (memory
`audit-annotated-consts-against-the-binary`).

**The answer, and it was already in this repo.** `FUN_0046e9e0`'s C2 plate — committed **2026-05-12**
— already states it:

> `re/analysis/vehicle_promote_c2/0046e9e0.md:27-28`
> `ESI[0x26f..0x271]` (= +0x9bc..+0x9c4) — **torque XYZ**
> `ESI[0x51..0x53]`  (= +0x144..+0x14c) — **angular velocity XYZ**

So **`+0x9bc`/`+0x9c0`/`+0x9c4` is the TORQUE triple and `+0x144`/`+0x148`/`+0x14c` is the angular
velocity.** The field to measure for yaw rate is **`+0x148`**.

**Two of my measurement legs were therefore measuring the wrong field** — leg 3's `.msd` KA-1 and this
session's Frida entry-hook probe. A grep of the existing plate for `0046e9e0` would have prevented
both. That is the `grep-the-harness-for-the-rva-before-writing-a-probe` lesson applied to **analysis
notes** instead of harnesses, and it is mine to own.

---

## 1. What the disassembly shows, cited

`FUN_0046e9e0 @ 0x0046e9e0`, size 1419. Register arguments the decompiler does not declare (memory
`decomp-is-silent-about-register-args`): **ESI = the vehicle record**, **EDI = source basis matrix**,
**EBX = destination basis matrix**. All record indices below are **dword** indices — `×4` for the byte
offset (memory `offset-grep-misses-dword-index`).

**The rotation actually applied** is a first-order cross-product integration of three basis rows:

```
*unaff_EBX   = (local_20 * unaff_EDI[2] - local_1c * unaff_EDI[1]) + *unaff_EDI;
unaff_EBX[1] = (local_1c * fVar1        - local_24 * fVar2)        + unaff_EDI[1];
unaff_EBX[2] = (local_24 * fVar3        - local_20 * fVar4)        + unaff_EDI[2];
```

and the same pattern for `EDI[4..6] → EBX[4..6]` and `EDI[8..10] → EBX[8..10]`. That is
`row += ω × row` with **ω = (local_24, local_20, local_1c) = (x, y, z)**, so the **yaw rate is
`local_20`**.

`local_20` is built, in order:

1. **Seeded from the torque triple.** `fVar1 = param_1 * _DAT_005cc948`; then
   `local_1c = fVar1 * _DAT_005cc32c`, `local_24 = local_1c * ESI[0x26f]`,
   `local_20 = ESI[0x270] * local_1c`, `local_1c = local_1c * ESI[0x271]` — i.e. scaled
   **`+0x9bc`/`+0x9c0`/`+0x9c4`**.
2. **Discarded when `ESI[4] == 0`** (byte `+0x10`): the branch sets all three to `0.0` and **rebuilds**
   them from the direction triple `FUN_0046d700(&local_18, *ESI)` returns, the throttle/brake bytes
   `param_2[0]`/`param_2[1]`, and the speed-derived `fVar2` (from `ESI[0x279]` = `+0x9e4`). Only **x and
   z** are re-added from the torque (`local_24 += … ESI[0x26f]`, `local_1c += … ESI[0x271]`) and only
   when `ESI[0x278] != 0x40800000` — **no y term**.
3. **Fed by the persistent accumulator.** Unconditionally
   `ESI[0x51..0x53] = (local_* + ESI[0x51..0x53]) * ((_DAT_005ccd08 - param_1*_DAT_005cc35c) * _DAT_005cc948)`,
   and then when throttle and brake are **both 0**,
   `local_* += param_1 * _DAT_005ce018 * ESI[0x51..0x53]`.

**Constants, read from the anchored binary** (bits first, per `plate-hex-gloss-authoritative`):

| symbol | VA | bits | f32 |
|---|---|---|---|
| `_DAT_005cc948` | `0x005cc948` | `0x39aec33e` | 0.00033333332976326346 (≈1/3000) |
| `_DAT_005cc32c` | `0x005cc32c` | `0x3f000000` | 0.5 |
| `_DAT_005cea80` | `0x005cea80` | `0x3b360bc0` | 0.002777799963951111 |
| `_DAT_005ccd08` | `0x005ccd08` | `0x453b8000` | 3000.0 |
| `_DAT_005cc35c` | `0x005cc35c` | `0x40800000` | 4.0 |
| `_DAT_005ce018` | `0x005ce018` | `0x3b03126f` | 0.0020000000949949026 |
| `_DAT_005cd0fc` | `0x005cd0fc` | `0xbdcccccd` | -0.10000000149011612 |
| `DAT_005d757c` | `0x005d757c` | `0x00000000` | 0.0 |
| `_DAT_005cc348` | `0x005cc348` | `0x3fc00000` | 1.5 |
| `_DAT_00613108` | `0x00613108` | `0x42c80000` | 100.0 — **seed only**, memory `dat-annotated-constant-may-not-equal-runtime` records A3 rewriting it to 105 at runtime |

`0x40800000` is **4.0f**, and `+0x9e0` is the grounded-wheel count — an independent cross-check, since
"all four wheels grounded ⇒ `+0x9e0 == 4`" was established separately (U-9174/U-9177 work).

**Second independent cross-check:** my reading of step 1's *linear* block
(`fVar1 = dt*_DAT_005cc948`, `× _DAT_005cea80`, `× ESI[0x26c..0x26e]` = `+0x9b0..+0x9b8` velocity)
reproduces the already-documented A8 position law verbatim, including
`re/analysis/data/A8_position_law_20260825.md:26`'s own annotation that `fVar1` is **"SHARED with omega
seed"**. So the dword→byte mapping and the whole read are corroborated by a note written months
earlier and not consulted until after the read.

---

## 2. This explains every measurement U-9193 made

- **`+0x9c0` co-occurs with turning but is not proportional to Δheading** — because it is the yaw
  **torque**. Torque is non-zero exactly while the car is being steered, and torque is not a rate.
  Support matching turning perfectly (0 of 289, and 0 of 288, turning pairs with it zero) is precisely
  what a torque does.
- **`+0x9bc` and `+0x9c4` being exactly 0.0 on all 2433 A6a-PRE rows** is the **pitch/roll torque**
  being zero on flat ground. **U-9175's physics was right and its label was wrong:** it read "the
  original's omega.x/z are exactly 0", and the measurement stands — it is the torque that is zero.
- **Changing the sampling phase changed nothing** because the phase was never the problem.

---

## 3. Correction owed to U-9175, and its scope

U-9175 names `+0x9bc`/`+0x9c4` as **`omega.x`/`omega.z`** and states the mechanism as
`at.y += omega.z*at.x - omega.x*at.z`. **The label is wrong** — they are torque — and the mechanism is
imprecise, because the cross-product input is the locally-built ω described above, which on the
`ESI[4] == 0` arm comes from the accumulator and the steer/throttle chain rather than from
`+0x9bc`/`+0x9c4`.

**What still stands in U-9175:** its measured numbers (the port's ~3.05e-08 forward-Y epsilon, the
~5e-10 noise, the original's exact zeros) and its conclusion (physically negligible, no faithful
single producer, not the D2 recovery defect). Nothing there is overturned; the field naming is.

**The same mislabel is in the port's own comment**, `BodyOrientationIntegrate.cpp:63`
(`kAngVel = 0x9bc; // angular velocity triple`). **But the port's behaviour is faithful**: it
implements both arms (`BodyOrient_OmegaFromAngVel` for `ESI[4]!=0`, `BodyOrient_OmegaFromSteer` for
`ESI[4]==0`, `:194` noting the latter is "the arm a normal driving car takes"), carries the
`+0x144/+0x148/+0x14c` accumulator with the `in[0]==0 && in[1]==0` gate (`:304-315`), and reproduces
the **y-term omission** in the `!= 4.0` re-add (`:275-280`). Its own header at `:30` already records
that "the omega SOURCE is still forked and I will not" claim which arm is live — an honest open note
that predates this session.

---

## 4. What this does to U-9191, stated carefully

**It removes a candidate rather than supplying one.** The port's rotation-rate construction is
structurally faithful to the original, so car 1's confirmed **0.9866 deg** heading divergence is **not**
explained by the port feeding its orientation integrator the wrong field.

**What is now actionable:** the quantity to compare cross-side is **`+0x148`** (and `+0x144`/`+0x14c`),
which **both** sides carry. The port's `AiStepDump` needs one more column triple; the original's
`.msd` already contains them at those offsets, so **no new original-side run is needed**.

**Not established, and not to be assumed:** whether the port's `+0x144..+0x14c` *values* match the
original's, and which of the two omega arms each side actually takes per frame. The port's own
`:30` note says that fork is unresolved, and nothing here resolves it.

---

## 5. Owed

- **U-9193 RESOLVED** — `+0x9c0` is the yaw torque; the yaw rate is `+0x148`. No further probing of
  `+0x9c0` as a rate.
- **Amend U-9175's field naming**, scoped to the label and mechanism only; its numbers and conclusion
  stand.
- **Correct `BodyOrientationIntegrate.cpp:63`'s comment** (`kAngVel` → torque) and
  `U-9175`'s wording. Comment-only; **no behaviour change**, and the port's transcription is already
  faithful.
- **Next for U-9191:** add `+0x144`/`+0x148`/`+0x14c` to the port's dump, and compare against the
  original's `.msd` at those offsets — plus **determine which omega arm each side takes**, since
  comparing the accumulator without knowing the arm would be the same class of error as comparing a
  torque to a rate.
- **No C-level moves.** `0x0046e9e0` is already C2 `mapped` with two plates; this session adds a field
  identification and a correction, not evidence for a promotion.
- **(b) is unchanged** and still failing.
