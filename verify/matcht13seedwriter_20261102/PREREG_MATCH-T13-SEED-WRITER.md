# PREREG MATCH-T13-SEED-WRITER (2026-11-02)

Decision L-85 = (c) (log/orch/DECISIONS.md): keep `MASHED_RULE10_SEED_DIFF` OFF and first port the `DAT_0089a360` producer
(`FUN_00414060`, `FUN_00414120`, the `DAT_0089a37c` term of `FUN_00417750`) so the rule-10 seed factor is computed live.
Predecessor evidence: `verify/t13seed_20261102/RESULT_MATCH-T13-SEED.md` (orch/MATCH-T13-SEED @ 8c8373015).

Committed alone, after the source commit 983b2ccbf (merged onto orch/integration as e3502e022) and before every arm.

## Change under test

New knob `MASHED_DIFF360_LIVE` (default OFF). Source: `Match/MatchS03_00414060.cpp`, `Match/MatchS03_00413fa0.cpp`,
`Race/AiDifficultyReset.cpp`, call sites in `D3d9Render/TrackRenderer.cpp` (StartMatch, `DAT_0063ba8c = 9`),
`Frontend/MenuNavSM.cpp` (screen 1), `exe_main.cpp` (launch row binding, dev inputs `MASHED_DIFF360_ROW`,
`MASHED_DIFF360_INIT_TABLE`).

Default path: the race-load call of the matched `0x00414060` restores `DAT_0089a360`, so the stored values equal the
old step-6 hand tail. Prediction D0 below.

## Arms

Exe: `mashedmod/build/mashed_re.exe` of the merged build (sha256 e8c8325d...), copied to
`D:/MashedWT/MATCH-T13-SEED-WRITER_data/exe/`. Root `D:/MashedWT/MATCH-T13-SEED-WRITER_root`. Every game run inside
`mashed_lock.py run`. Port arm base env = T13-SEED `p_arms.py` (MASHED_ROUND=1, MASHED_ROUND_RULE=10, MASHED_CAR=1,
MASHED_DETERMINISTIC=1, MASHED_DET_FRAMES=2400, MASHED_RULE10_SEED_LOG=1, MASHED_MUTE=1), plus per arm:

| arm | track view | MASHED_GAME_MODE | DIFF360_ROW | DIFF360_INIT_TABLE | DIFF360_LIVE | RULE10_SEED_DIFF |
|---|---|---|---|---|---|---|
| L_SANDS | sands (0xb) | 5 | 5 | 1 | 1 | 1 |
| L_NEU | Neustein (0x27) | 4 | 2 | 1 | 1 | 1 |
| L_CITY | City (0x1a) | 4 | 12 | 1 | 1 | 1 |
| C_SANDS / C_NEU / C_CITY | same as L_* | same | same | same | **unset** | 1 |

The L arms reproduce the T13-SEED original arms OEV_SANDS / OEV_NEU / OEV_CITY (scenario_launch rows 5 / 2 / 12, modes
5 / 4 / 4, `FUN_00414120` called once before launch, rule 10). Those original values are the reference, not re-measured:
`verify/t13seed_20261102/seeds_scored.txt`, raw hashes in `verify/t13seed_20261102/raw_sha256.txt`.

Gates: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the same exe, knob unset.

## Predictions

- **H1-LIVE** (per L arm): the `[diff360] raceinit` line has `a360_raw` = `40c00000` (6.0) / `40800000` (4.0) /
  `40e00000` (7.0) for SANDS / NEU / CITY, from `0x005f2a70` entries row*3 + (mode 4 -> 1, 5 -> 2) = 6 / 4 / 7 and
  `DAT_0089a37c` = 0.
- **H1-SEED** (per L arm): every `[rule10-seed]` line has `timer_raw` / `bonus_raw` bit-equal to the original's OEV arm:
  SANDS `417b3333` / `408b3333`, NEU `418e6666` / `40799999`, CITY `418f3333` / `40be6667`.
- **H0-CTRL** (per C arm): `live360_raw` = `40200000` (2.5) at every seed call, and the timer equals T13-SEED's P_ON value
  for that course: SANDS `41860000`, NEU `41920000`, CITY `419a0000`. No `[diff360]` line. This shows the knob is the
  cause of any L difference.
- **D0** (gates, knob unset): G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 (= `re/orchestrator/merge_train.json`
  refs at this branch's merge base). A different hash is a gate move: filed as a roadmap decision, not a failure.
- **W-37C** (witness only, no verdict): if any L arm logs `[diff360] roundend37c`, `a37c_raw` stays `00000000`, because
  `DAT_0089a380` has no exe producer (`FUN_00414220` unported). Not reaching it is recorded as "not reached".
- **W-NAV** (witness only): every L arm logs `[diff360] tableinit_navinit` once (Nav_Init roots at screen 1) and
  `tableinit_dev` once.

## Verdict rule

GREEN = H1-LIVE and H1-SEED hold on all 3 L arms, H0-CTRL holds on all 3 C arms, D0 holds.
Any L-arm mismatch = RED with the differing value. A crash at the original-side piz signatures does not apply (no
original arms). A port crash is re-run once, then reported.
