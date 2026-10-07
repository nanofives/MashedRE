# RESULT — porting `FUN_00414060` whole: MEASURED BLOCKED. It would regress `0x0089a360`.

Date 2026-10-08. Follows `RESULT_IDX364.md` §5 option (1), chosen by **USER DECISION
(Mariano, 2026-10-08)**: "Port `FUN_00414060` whole — pays the debt, closes U-D3-DIFF360, costs
two callee bodies."

**The body was NOT written.** Its inputs were measured first, and they say porting it whole is a
regression today. MEASUREMENT ONLY — no new body, no `.rsp` change, nothing seeded, nothing
default-ON. `original/` and the `.asi` untouched. No C-level.

Raw: `A360pred.gates.csv` (53,992 rows, one race, `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400`).

**Default-build control PASS.** The prediction columns are behind the existing default-OFF
`MASHED_U9186_GATES`, and a knob-off stepdump taken from this build (`A360step.csv`) hashes to
**`86b7b2bb`** — the same value as `E2off_1` / `H1step` / `H3step` / `GF0step`. Five builds now
bit-identical on the default path.

## 0. Verdict first

> **Every single input to `FUN_00414060` reads `0` in the standalone, so a faithful port would
> compute `0x0089a360 = 0.0` on 100% of rows — against the `2.5` the original carries and the port
> currently hardcodes. Porting it whole trades a right-valued seed for a right-way-computed wrong
> value.**

The prediction was made **without writing the function**, from raw global reads only — the
`H3-WROTE` lesson applied *before* the code exists instead of after.

| input | resolves to | measured standalone | needed for `2.5` |
|---|---|---|---|
| `FUN_0042f6a0` | `*(u32*)0x0067e9fc` | **`0`** on 53,992/53,992 | **`6`** (or `10`) to take an override branch |
| `FUN_00431d80` | `*(u32*)0x0067ea7c` | **`0`** on 53,992/53,992 | **`1`**, since the mode-6 arm is `flag * 2.5` |
| `FUN_00430790` | `*(u32*)0x0067f17c` | `0` | — |
| `FUN_00413fa0` | `g30790*3 + (mode==4?1:5?2:0)` | `0` | — |
| `_DAT_0089a37c` | runtime float | `0` | — |
| `(&DAT_0089a384)[idx]` | runtime int table | `0` | non-zero, on the no-override path |
| **`pred_360`** | `FUN_00414060`'s step-5 output | **`0.0` on 53,992/53,992 (0.0000% == 2.5)** | `2.5` |
| current `flt360` | the seed at `TrackRenderer.cpp:397` | `2.5` on 53,992/53,992 | — |

## 1. The four image constants, read not guessed

From `original/MASHED.exe.unpatched` via `re/tools/dump_rdata_floats.py`:

| address | raw | value | role (plate step) |
|---|---|---|---|
| `0x005cc32c` | `0x3f000000` | **0.5** | scale on `_DAT_0089a37c` (step 2) |
| `0x005cd088` | `0x40200000` | **2.5** | multiplier on the mode-6 / mode-10 arms (steps 3-4) |
| `0x005d757c` | `0x00000000` | **0.0** | init + lower clamp (step 5) |
| `0x005cc55c` | `0x41200000` | **10.0** | upper clamp (step 5) |

**`0x005cd088` being exactly `2.5` is the find.** It means the original's `0x0089a360 = 2.5` is
almost certainly `FUN_00431d80() * 2.5` with the tiebreak flag at `1`, on the mode-6 arm — so the
port's hardcoded `2.5` has been the *right number for the wrong reason*, and `U-D3-DIFF360`'s
missing writer is `FUN_00414060` step 5. That part of `RESULT_IDX364.md` stands.

**[UNCERTAIN]:** which arm the original actually takes is *not* measured. It could instead be the
no-override path with `_DAT_0089a37c * 0.5 + DAT_0089a384[idx] == 2.5`. Distinguishing them needs a
Frida entry probe on `0x00414060` in `MASHED.exe` logging `FUN_0042f6a0()` and `0x0067ea7c` — the
same shape as `scenario_launch.py --leader-probe`. Until then the mode-6 reading is inference from
the constant, not evidence.

## 2. Why this blocks option (1) specifically

The chosen option's premise was that the cost is "two callee bodies". It is not. Both missing
callees turned out to be trivial — `FUN_00431d80` is a 5-byte global read
(`SplashGameMode_t5.cpp:118`) and `FUN_00413fa0` is 48 bytes of arithmetic over two getters that
are **already in the exe** (`Util/GameStateGetters.cpp:12`, `Util/SmallLeaves_o6.cpp:22`). Writing
them is easy.

**The blocker is upstream of all four callees:** the two globals they read are themselves
unproduced in the standalone. `0x0067ea7c`'s write-site set is already an open uncertainty —
**`U-1305`**, carried at `SplashGameMode_t5.cpp:110` — and the only exe-side writer in the tree
sets it to **`0`** (`Frontend/SetupScreenRenderers.cpp:343`), with `exe_main.cpp:655` calling
`DAT_0067ea7c == 0` "the canonical case". So porting `FUN_00414060` does not close
`U-D3-DIFF360`; it converts it into `U-1305` plus a new question about `0x0067e9fc`.

## 3. What IS achievable and faithful today: step 6

`FUN_00414060`'s step 6 is an **unconditional tail that reads no inputs**:

```
DAT_0089a364 = 0xffffffff            // -1, the sentinel GF0-IDX364 is missing
_DAT_0089a870/874/878/87c = 0xbf800000   // -1.0f x4
```

It needs no callees, no globals, and no measurement — it is a constant store. Porting it is a
bit-faithful transcription that fixes `idx364`, which is the actual objective of this lane, and it
leaves `0x0089a360`'s seed exactly as it is rather than regressing it.

The honest accounting: that is **a partial port of `FUN_00414060`**, not the whole function, and it
must be recorded as such — steps 1-5 remain unported and blocked, with their blocker now named.

## 4. What is NOT claimed

- That `FUN_00414060` is *wrong*. It is faithful; its **inputs** are absent. The distinction
  matters: this is the same substrate story as `G1-POS`, `H3` and leg 0, not a transcription defect.
- That the original takes the mode-6 arm (§1, `[UNCERTAIN]`).
- That `0x0067e9fc = 0` is wrong for the standalone. `FUN_0042f6a0` is `GetRaceSubMode`; whether it
  *should* read 6 during a race is unmeasured on the original side.
- Anything about `bias374`, still `RESULT_GF0.md` §6 item 2.

## 5. Next — a smaller decision than the last one

1. **Port step 6 only** (recommended): fixes `idx364`, zero regression risk, no callees. Record it
   explicitly as a partial port of `0x00414060` with steps 1-5 deferred.
2. **Measure the original first** with a Frida entry probe on `0x00414060`, to learn which arm
   produces `2.5` before porting anything. Cheap, and it would close §1's `[UNCERTAIN]`.
3. **Port the whole function anyway** and accept `0x0089a360 = 0`. This regresses a value the AI
   difficulty path reads (`AiStandalone.cpp:747`, `:1552`; `AiPreTick.cpp:272`) and is **not
   recommended** without re-scoring criteria (b) and (e).
