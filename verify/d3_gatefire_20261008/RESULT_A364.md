# RESULT — `FUN_00414060` step 6 ported (PARTIAL). `idx364` now carries the `-1` sentinel.

Date 2026-10-08. Follows `RESULT_A360.md` §5 option (1), chosen by **USER DECISION (Mariano,
2026-10-08)**: "Port step 6 only — fixes idx364, no regression, recorded as a partial port of
0x00414060."

**RAN**, 4 runs. Default-OFF behind `MASHED_A364_RESET`. No C-level. `original/` untouched, `.asi`
code path untouched. Nothing seeded — this is a transcription of two constant stores.

Raw: `A364_GATES.txt`, `A364_RUN.txt`, `A364{off,on,on_r2}.gates.csv`, `A364step.csv`.

## 0. Verdict first

**All five gates PASS. Both of step 6's stores land, the knob genuinely gates them, the default
build is untouched, and the change is INERT exactly as predicted.**

| gate | verdict | figure |
|---|---|---|
| `A364-WROTE` | **PASS** | ON arm: `idx364 = -1` **and** `a870/a874/a878/a87c = -1` on **53,992/53,992** rows, 1 distinct value each |
| `A364-CTL` | **PASS** | OFF arm: all five read `0` on 53,992/53,992 — the knob is a true gate, not a no-op |
| `A364-KNOBOFF` | **PASS** | `A364step.csv` hashes **`86b7b2bb`**, the sixth build to match `E2off_1` / `H1step` / `H3step` / `GF0step` / `A360step` |
| `A364-DET` | **PASS** | `A364on` and `A364on_r2` byte-identical (`0dd43283`) |
| `A364-INERT` | **INERT, as predicted** | the arms differ in **exactly** `idx364, a870, a874, a878, a87c` and **nothing else** |

## 1. This is a PARTIAL port and the record must say so

`FUN_00414060` (0x00414060..0x00414118, 185 bytes, `__fastcall`) has six steps. **Only step 6 is
ported.** Steps 1-5 are measured-blocked and stay out
(`RESULT_A360.md`): every one of their inputs reads `0` standalone, so porting them would write
`0x0089a360 = 0.0` and regress it from the `2.5` the original carries.

| step | what it does | status |
|---|---|---|
| 1-5 | compute + clamp a float into `_DAT_0089a360` | **NOT PORTED — blocked.** `FUN_0042f6a0` = `0` (no override arm fires), `FUN_00431d80` = `0`, `FUN_00430790` / `_DAT_0089a37c` / `DAT_0089a384[idx]` = `0`. Blocker is upstream: `U-1305` + `0x0067e9fc` |
| 6 | `DAT_0089a364 = -1`; `_DAT_0089a870/874/878/87c = -1.0f` | **PORTED HERE**, bit-faithfully. Reads no inputs, needs no callees |

So `0x0089a360` keeps its seed (`TrackRenderer.cpp`) and **`U-D3-DIFF360` stays open** — now with
its producer identified as this same function's step 5. `0x0089a364` gets its real sentinel.

**Owed to `re-classify`, not hand-edited:** `hooks.csv` row `00414060` stays **C2** and must record
*partial (step 6), steps 1-5 blocked*, with `exe_file` = `Race/AiDifficultyReset.cpp`. A row that
reads as a whole-function port would overstate this.

## 2. The instrument covers the whole change

`A364-WROTE` was first drafted against `idx364` alone. Step 6 has **two** stores, so that gate
would have verified half the port — the same instrument-scope mistake that cost `G-TOOK`,
`H1-CONVERGE` and `H3-WROTE` earlier this session. The `a870/a874/a878/a87c` columns were added and
the build redone **before** the gate ran, not after it passed.

`A364-CTL` is the matching negative: with the knob off all five cells read `0`, so the ON result is
the port's doing and not something else in the build.

## 3. Why INERT is the expected result, not a disappointment

Nothing in `mashed_re.exe` reads `0x0089a364`: the only consumer is `Ai/AiLeaderTimer.cpp:94`,
which is `.asi`-only. So this leg **removes a blocker for a future `GF1-FIRE`; it does not change
behaviour today**, and `A364-INERT` showing exactly the five written cells is the confirmation that
it changed nothing else.

What it buys is specific. At `AiLeaderTimer.cpp:94` the value decides whether `FUN_0040e470` is
called at all — the original, reading `-1`, **skips that call on all 512 calls**; a `0`-reading
port would take it. With the knob ON the port now agrees with the original on this input, so a
branch-2 measurement would no longer be measuring a different function on this axis.
`bias374` remains unresolved (`RESULT_GF0.md` §6 item 2) and still blocks that measurement.

## 4. What is NOT claimed

- **No C-level.** Step 6 is bit-faithful but has no behavioural diff against the original; it is
  verified against the plate and against the port's own census, not against a Frida run.
- Not that branch 2 can now be measured. `bias374` is the remaining upstream disagreement.
- Not that `FUN_00414060` is ported. It is not — §1.
- Not that `-1` is correct for the standalone *in principle*; it is correct **as a transcription**
  of what the original's step 6 writes. The two sides now agree because the port executes the
  original's store, which is the only claim being made.
- `U-D3-DIFF360` and `U-1305` both remain open.

## 5. Next

1. **`bias374`** — the last upstream disagreement before branch 2 is measurable
   (`FUN_004177b0`, exe copy C2-demoted, `Race/RuleEngine.cpp`).
2. The `re-classify` transaction in §1, plus the two tracker defects already named:
   `hooks.csv` `004148b0`'s stale `exe_file`, and U-9186's standing instruction (superseded by
   U-9187 — `PREREG_GATEFIRE.md` §1).
3. Whether `MASHED_A364_RESET` should go default-ON. It is inert today, so this is cheap and can
   ride with `H2`'s `MASHED_SLOTSTATE_SEED` question rather than needing its own leg.
