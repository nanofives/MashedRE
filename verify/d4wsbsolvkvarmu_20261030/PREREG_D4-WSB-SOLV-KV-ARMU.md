# PREREG D4-WSB-SOLV-KV-ARMU

Committed alone, before any arm run. Branch `orch/D4-WSB-SOLV-KV-ARMU` off `orch/integration` @ `1611794e7`.
No build of the game targets, no `MASHED.exe` boot, no original-side run, no Ghidra MCP. No source edit:
`mashedmod/src/mashed_re/Collision/RwpVtableKV12.cpp` is read as it stands at the base.

## Why this step

`D4-WSB-PORT-SOLV-KV` ran Arm U (`verify/d4wsbsolvkv_20261029/uc_arm.py`) and got INVALID for the M4 site only
(`RESULT_D4-WSB-PORT-SOLV-KV.md:113-114`). M4 replaces the `fistp` at `0x0056b264` (round to nearest, CW `0x007f`)
with C truncation. The new limit is computed only when `(double)limit > est*100 + 1` (`fcompp` at `0x0056b249`), the
operand is `est * (float)P1_4` (`fmul` at `0x0056b254` / `0x0056b256`), and the first limit is `P1_4 + 1`
(`0x0056afda`). So the operand is under `P1_4^2 / 100`, which is under 0.5 for every `P1_4 <= 7`, where round and
truncation agree. The generator drew `P1_4` in 1..6. The site needs `P1_4 >= 8` cases.

## Arm U, this step

Script: `verify/d4wsbsolvkvarmu_20261030/uc_arm.py`, a copy of the KV step's `uc_arm.py` with the three changes
below and nothing else (same Unicorn setup: every section of `MASHED.exe.unpatched` at its VA, arena `0x20000000`,
x87 CW `0x007f`, MXCSR `0x1f80`; same port harness: the TU text between `namespace mashed_re {` and
`// --- registrations`, verbatim or one mutant, `cl /EHsc /W3 /O2`, image base `0x10000000`, RVA thunk page at
`0x00560000`, `uc_harness_main.inc` copied unchanged). Anchor check on `MASHED.exe.unpatched` before the run.

1. **Cases.** The 1200 pass-1 cases (seed `20261029`) and the 300 `0056adb0A` cases (seed `20261030`) are generated
   exactly as before, same order. Added after them: **300 `0056adb0B` cases**, separate RNG seed `20261031`, drawn by
   the same `0x0056adb0` generator with:
   - `P1_4` (vec4 group count `n`) in **8..20** (uniform integer), every array sized from that `n` as before,
   - W69 = 1, W68 in 60..160, W66 = 0.0 (the amendment-1 adaptive settings),
   - everything else drawn as in pass 1.
2. **Coverage probe (original side only).** A Unicorn code hook at `0x0056b264` reads the float32 at `[esp+0x48]`
   (stored by the `fstp` at `0x0056b25a`, reloaded by the `fld` at `0x0056b260`) and counts, per case label:
   executions of the `fistp`, and executions where round-half-to-even of the operand differs from its truncation toward
   zero ("M4-sensitive"). The hook only reads. The arena compare is unchanged.
3. **Labels and site map.** `0056adb0B` joins the labels that execute the M1, M2, M3 and M4 sites (same as
   `0056adb0` / `0056adb0A`).

Five variants, each on all 1800 cases: `port`, `M1`, `M2`, `M3`, `M4` (the four mutants of the KV PREREG, unchanged
text). Compared bitwise: the whole arena after the call, plus the float return of `0x0056ac40`.

## Read-out (fixed now)

- **GREEN**: 0 Unicorn errors, the unmutated port matches all 1800 cases bitwise, and each of M1..M4 differs on at
  least 1 case of a label that executes its site. For M4 that means at least 1 differing case among `0056adb0`,
  `0056adb0A`, `0056adb0B`.
- **RED**: any unmutated mismatch or Unicorn error. Reported with the label, case id and first differing arena offset
  (or the return word).
- **INVALID**: any mutant at 0 differing cases, or a `0056adb0B` M4-sensitive count of 0. The site is reported as not
  covered. No amendment in this step.
- The M4-sensitive count is reported in every outcome next to the M4 differing-case count.

## What follows from the read-out

- GREEN: through `re-classify`, close U-9589 / U-9590 / U-9591 / U-9592 with the disassembly frame cites of
  `RESULT_D4-WSB-PORT-SOLV-KV.md` ("Frames": 69 / 15 / 56 / 39 words, built and cleaned up at the listed addresses)
  plus this Arm U verdict. The four `hooks.csv` rows stay **C2** (Arm U is offline evidence, not a Frida diff,
  `re/CONFIDENCE.md`). A dated note on each row cites this RESULT.
- RED or INVALID: no tracker change. U-9589..U-9592 stay open. The RESULT cites the diverging cases.

## Default gates

Not run. This step changes no source and builds no game target, so `mashed_re.exe` and the `.asi` are byte-for-byte
the base's. The merge train's own gate run covers the merged tree.

## Data

Raw (`cases.bin`, `out_<variant>.bin`, build folders, run log) in `D:/MashedWT/D4-WSB-SOLV-KV-ARMU_data/uc`, listed with
SHA-256 in the RESULT. Committed: this PREREG, `uc_arm.py`, `uc_harness_main.inc`, `uc_summary.tsv`, `uc_summary.json`,
the RESULT.
