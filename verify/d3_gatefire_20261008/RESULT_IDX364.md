# RESULT — GATEFIRE: who writes `0x0089a364`, and why the port reads `0`

Date 2026-10-08. Follows `RESULT_GF0.md` §2 and §6 item 1. **Static resolution — no run, no build,
no code change.** `original/` untouched. No C-level. Nothing was seeded.

## 0. Verdict first

**Both writers are identified, and the disagreement is fully explained.** The port reads `0` not
because something writes `0`, but because **nothing writes it at all** and the standalone's
blank-mapped memory is zero — so the original's `-1` *sentinel* never gets set. This is memory
`zeroed-granule-vs-minus-one-sentinel` exactly: "standalone zeroes globals so `-1` reads as `0`;
seed the producer."

**Side effect: this closes the open `[UNCERTAIN] U-D3-DIFF360`** at
`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:395`.

## 1. The two writers

| writer | what it does to `0x0089a364` | when |
|---|---|---|
| **`FUN_00414060`** (`re/analysis/bucket_util_0040e4b0_0042f790/0x00414060.md`, step 6) | `DAT_0089a364 = 0xffffffff` (**−1**) — an **unconditional reset**, alongside `_DAT_0089a870/874/878/87c = -1.0f` | at race reset, via caller `FUN_004111c0` |
| **`FUN_00414220`** (`.../0x00414220.md`, step 2) | `DAT_0089a364 = iVar1`, where `iVar1 = FUN_0040e4b0()` = `Race::GetSoleFinishedPlayer` | **only when a sole finisher exists** — step 1 is `if (iVar1 == -1) return;` |

**That fully explains the original's `-1` on 512/512** (`RESULT_WITNESS.md:66`): `FUN_00414060`
writes the sentinel at reset, and `FUN_00414220` early-returns for the whole race because nobody
has solely finished during the 220-call window. No guess is required.

**And it explains the port's `0` on 53,992/53,992**: neither function is ported. `hooks.csv` has
`00414060` and `00414220` both **C2 `new`**, with `file` pointing at an analysis plate and an empty
`exe_file`; `0040e4b0` is **C2 `mapped`**. `exe_main.cpp:56` VirtualAlloc-maps the region blank, so
the address reads `0` — a value that is *in-range for a vehicle index* and therefore silently wrong
rather than obviously wrong.

## 2. The half-seed already in the tree, and U-D3-DIFF360

`TrackRenderer.cpp:392-397` currently reads:

```cpp
// [D3 2026-09-26] FUN_00413fe0 race reset, and DAT_0089a360 = 2.5: the value
// measured on every AI step of the original race (o_inputs, diff_a360 column);
// FUN_004177b0 truncates it to the difficulty-table row. Its frontend writer is not
// traced ([UNCERTAIN] U-D3-DIFF360: next, reference_to 0x0089a360 WRITE sites).
Ai::Ai_ResetRace();
Ai::F32(0x0089a360u) = 2.5f;
```

**`FUN_00414060` is the untraced writer that comment is asking for.** Its step 5 computes and
clamps the value into `_DAT_0089a360`; its step 6 resets `DAT_0089a364` to `-1`. They are the same
function, two consecutive steps.

So the port currently **hardcodes one half of `FUN_00414060`'s output and omits the other half** —
which is precisely why the census found `flt360 = 2.5` matching the original on 100% of rows while
`idx364 = 0` disagrees on 100% of rows. Two columns of the same census, one producer, one seeded
and one absent.

## 3. Cost of the faithful fix

`FUN_00414060`: 185 bytes, `__fastcall` (`param_1` in ECX), C2 plate, callees all accessor leaves.

| callee | role (plate) | exe-side availability |
|---|---|---|
| `FUN_0042f6a0` | mode getter | **EXE** — `Util/GameStateGetters.cpp`, C3 |
| `FUN_0042fe80` | mode-10 value | **EXE** — `Frontend/MenuInit.cpp`, C4 verified |
| `FUN_00431d80` | mode-6 value | **asi-only** — `Frontend/SplashGameMode_*.cpp`, C3 |
| `FUN_00413fa0` | index lookup | **no `.cpp` at all** — C3 `impl` pointing at a plate |

So two of four callees need exe-side bodies before `FUN_00414060` can be ported whole. Its
plate carries one open row, `U-6009` (the `param_1` index domain and the `&DAT_0089a384` table
contents), marked non-blocking for C2.

**Note the structural fact, without recommending on it:** step 6 is an unconditional tail that
reads none of the four callees' outputs. Steps 1–5 produce `0x0089a360`; step 6 resets
`0x0089a364` and the four floats at `0x0089a870..87c`.

## 4. What is NOT claimed

- That `FUN_00414060` and `FUN_00414220` are the **only** writers. The evidence is a grep of
  `re/analysis/` plates plus the port tree, not a Ghidra `reference_to` sweep — Ghidra MCP is
  unavailable on this account (`re/ACCOUNT2_CAPABILITIES.md`). A `reference_to 0x0089a364` sweep
  via `analyzeHeadless` would close it. **[UNCERTAIN]** — though the two found writers already
  explain both sides' observed values completely, which is strong but not exhaustive.
- Any claim about `bias374` (`0x0089a374`). That is `RESULT_GF0.md` §6 item 2 and is untouched
  here. Note `AiStandalone.cpp:1577` **does** write `0x0089a374`, so its disagreement has a
  different shape from this one and must not be assumed to resolve the same way.
- That fixing `idx364` makes branch 2 fire. It removes one of the reasons it could not be
  *measured*; `GF1-FIRE` remains unmeasured.

## 5. Next — this is a fidelity decision, not an analysis one

The standing rule is `verify/d3_modes37_20261002/RESULT_STEP2.md`: **"seeding the globals would not
be a port."** It was applied unchanged by the 2026-10-03 witness and by leg 0. The tree already
contains one seed at the exact line in question (`0x0089a360 = 2.5f`, 2026-09-26), so the options
differ in whether that debt is paid or doubled:

1. **Port `FUN_00414060`** — pays the debt, closes `U-D3-DIFF360`, replaces the `2.5` seed with a
   computed value. Costs two callee bodies (`FUN_00431d80`, `FUN_00413fa0`).
2. **Port only step 6** — faithful transcription of the reset, no callees needed, but it is a
   partial port of a function and leaves the `2.5` seed standing.
3. **Seed `0x0089a364 = -1` beside the existing seed** — one line, and explicitly against the
   standing rule. Would make the census agree while producing nothing.

Recommendation: **(1)**, with (2) as the fallback if the two callees prove expensive. (3) should
not be taken silently; if it is taken, it must be recorded as a knob-gated seed with its own row.
