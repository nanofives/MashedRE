# PREREG D3-AI-VEHICLES (committed alone, before any code change or arm)

Step: L-27 follow-up (`re/orchestrator/DECISIONS.md` row L-27). The port sets only `cfg.cars[0].carIndex`
(`exe_main.cpp:2111`) and leaves `RaceConfig::numCars = 1` (`Race/RaceSession.h:46`), so `WireRaceBegin`
(`Race/RaceSession.cpp:215`) gives cars 1..3 engine class 0 (`WireEngineClass(-1)`, D4-WSJ-WIRE RESULT 4c).

## The original's selection site

`FUN_0040d110` (disassembly of `original/MASHED.exe.unpatched`, capstone, 2026-10-26):

| address | instruction | effect |
|---|---|---|
| `0x0040d143`, `0x0040d162` | `mov ecx,[edi+eax*4]` / `mov ecx,[ecx+4]` | id = dword +4 of record `param_1[param_2]` |
| `0x0040d175`..`0x0040d1bd` | `call 0x42f6a0`; `cmp eax,0xa` / `cmp eax,6`; `call 0x431d70` | modes 10 and 6 re-read the id from record `param_1[FUN_00431d70()]` |
| `0x0040d165`..`0x0040d173` | `imul 0x2aaaaaab`, sign fix | id / 6 |
| `0x0040d1bf`..`0x0040d1c2` | `lea edi,[esi+esi*2]; shl edi,1` | group = (id / 6) * 6, one value for the race |
| `0x0040d1de` | `mov ebp,0x7f1a1c` | loop over `DAT_007f1a1c` stride `0x10` |
| `0x0040d1e3`..`0x0040d1e6` | `mov esi,[ebp]; add esi,edi` | car i vehicle id = `DAT_007f1a1c[i*4]` + group |
| `0x0040d1e8`..`0x0040d1f2` | `cmp eax,0xb`; `lea esi,[ebx+edi]` | mode 0xb: id = i + group |
| `0x0040d1fb` | `call 0x420420` (i, id) | per-car vehicle init |
| `0x0040d203`..`0x0040d20d` | `add ebp,0x10; inc ebx; cmp ebp,0x7f1a5c; jl` | exactly 4 iterations, no participant gate |

So every car's vehicle id lies in car 0's 6-livery group: the vehicle TYPE is one per race and only the livery
(`DAT_007f1a1c`, or i in mode 0xb) differs per car. The port's `carIndex` is the vehicle type (catalogue
`kVehicles`, `Race/RaceSession.cpp:39`); livery is already per car (`TrackRenderer::ResolveCarLiveries`).

## Knob

`MASHED_AI_VEHICLES=1` (default OFF). In `RaceSession::Begin`, after `m_cfg = cfg`: `m_cfg.numCars = 4`
(the 4-iteration loop, `0x0040d1de`..`0x0040d20d`) and `m_cfg.cars[i].carIndex = m_cfg.cars[0].carIndex` for
i = 1..3 (one group, `0x0040d1bf`..`0x0040d1e6`). Readers of the two fields: `WireRaceBegin` only
(`grep numCars|carIndex mashedmod/src`: `RaceSession.cpp:130,190,214,215`, `AudioWire.cpp:260-273`). No sim code reads
either field, so the knob cannot move sim state.

Out of scope (residual, named): the group source. The port's car 0 type comes from `MASHED_CAR_SEL` (default 0,
ADVANTAGE), the original's from the event record dword +4 (`0x0040d162`, with the mode 10 / 6 re-read). This step
copies car 0's type to cars 1..3; it does not change car 0's.

## Arms (standalone `mashed_re.exe` built from this branch, `MASHED_NO_DEPLOY=1`, private root `D:/MashedWT/D3-AI-VEHICLES_root`, every boot under `mashed_lock.py run --label D3-AI-VEHICLES`, explicit PIDs)

Runner: `verify/aiveh_20261026/run_arms.py` (copy of `verify/savelink_20261023/run_arms.py`, paths changed, adds
stack `AV` = `MASHED_AI_VEHICLES=1`). Wire arms: `verify/aiveh_20261026/run_port.py` (copy of
`verify/wsjwire_20261026/run_port.py`, paths changed, `ARM_KNOB=1` sets `MASHED_AI_VEHICLES=1`).

- **G0 gates, knob OFF:** `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **G1 gates, knob ON:** `run_arms.py H_AV B_AV_K0 B_AV_K1 B_AV_K26`.
- **W0** `run_port.py W0 menu`, knob OFF (audio wire on, its default since the L-27 flip).
- **W1** `run_port.py W1 menu`, knob ON.
- **D2 x3, knob ON:** `run_arms.py D2_AV_1 D2_AV_2 D2_AV_3` under `mashed_lock.py run --solo`, no build or train
  while they run, any arm under 1600 `motion_diag` lines re-run (max 2 re-runs). Scored with
  `verify/d2rebase_20261019/score_d2_v2.py port`.
- **13 tracks, knob ON:** `run13.py run --exe <exe> --root <root13> --out <dir> MASHED_AI_VEHICLES=1`, then
  `run13.py score --out <dir> --cite verify/rres_20261016/cites_rres.json`, against
  `verify/colllut_20261026/tracks_score.txt`.

## Predictions (scored in this order)

1. **G0:** H_DEF and B_DEF_K0/K1/K26 hashes equal `re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`
   (read at scoring time).
2. **G1:** the same six hashes (no sim reader of the two fields).
3. **W0:** the audio event log has exactly 1 `engine_class` row (car 0, `ADVANTAGE -> eng2`), `wire_race` arg 1,
   and cars 1..3 start `eng1` on v1/v2/v3 and `eng1d` on v5/v6/v7 (the D4-WSJ-WIRE arm B shape).
4. **W1:** 4 `engine_class` rows, cars 0..3 each `ADVANTAGE -> eng2`, `wire_race` arg 4, cars 0..3 start `eng2` on
   v8/v9/v10/v11 and `eng2d` on v12/v13/v14/v15 (base+c and base+4+c), no `eng1` / `eng1d` start in the race segment.
5. **D2 x3 (knob ON):** PASS 3/3 inside `band.json`, `score_d2_v2.py` numbers reported against the D2 default
   (lead's reference 0.1942/0.2492/1896.74, memory 0.1874/0.2488/~1929; the band file wins).
6. **13 tracks (knob ON):** 0 RED, `INSTRUMENT GATES: PASS`, every row's verdict equal to the reference table.

## Verdict mapping

- GREEN (knob-OFF part): 1 holds, the knob is default OFF. ON results go to the RESULT and the flip is a decision row.
- RED: 1 fails (a default gate moved).
- 2, 5 or 6 failing is a finding (an unexpected sim reader), reported with the knob kept OFF.
- 3 or 4 failing: the wire does not read the fields as stated; reported, knob kept OFF.
