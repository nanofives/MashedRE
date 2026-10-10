# RESULT: mode 7 leg M1, the world-object registry / collision broker, verbatim

Pre-registration: `PREREG_MODE7.md` (`abb52d44`, before any code). Knob `MASHED_REGISTRY`, default OFF.

## 0. Verdict first

> **Every M1 gate PASSES.** The registrar `FUN_00484cf0`, the publish `FUN_00485070` and the O(N^2) pair
> pass `FUN_00484f70` reproduce the original byte for byte on 3,162 captured frames (`o_rg1` 121, `o_rg4`
> 3,041), and the port's callback sequence equals the original's on **3,003 of 3,003** frames with a
> callback, including the 2 real pickup collections (`FUN_00458920`). Same at PC_24 and PC_53.
> **Knob OFF and knob ON (no registrant) both give `ae67b13a / c8317ce2 / 31db9384`**, on the fixed build.
> **Side finding, fixed:** the x87 precision scope in P1/P2/P3/M1 never restored PC_53, so every knob-ON run
> since P1 ran the whole game thread at PC_24. Measured impact on the reported P2/P3 numbers: none (§3).

| gate | result |
|---|---|
| G-M1-CAPTURE | `o_rg1` (60 s, every 30th frame) and `o_rg4` (240 s, every frame with a callback + every 300th) |
| G-M1-REG | 121 / 121 and 3,041 / 3,041 frames: slots `[0, cc)` byte-identical; only `+0x22..+0x23` (pad after the short index) never written |
| G-M1-PASS | 121 / 121 and 3,041 / 3,041: slots, `d8`, `cc`, `d4` after the pass byte-identical |
| G-M1-CB (added after the coverage hole in §2) | 3,003 / 3,003 callback sequences `(target, a, b)` exact; original witnessed 3,003 calls |
| G-M1-OFF | `ae67b13a / c8317ce2 / 31db9384` (`M1_CTRL`) |
| G-M1-INERT | `ae67b13a / c8317ce2 / 31db9384` (`M1F_ON`, fixed build). The leaky build's `M1_ON` gave `e3c35a25 / c8317ce2 / 7edae968` |

## 1. What the original does on Training (measured, `o_rg1`/`o_rg4`)

- `DAT_006e70c8 = 0` on every sampled frame and `FUN_004847d0` ran 0 times in 18,000+ frames: the grid
  path (`FUN_004847d0`, `FUN_00484ac0`, `FUN_00484a50`, `FUN_00484de0`) is not executed. It is ported
  verbatim but **unexercised** (no gate covers it).
- 8..17 entries per frame: cars `0xff00..0xff03` (r 0.533, no callback), pickups type 7 (r 0.5, callback
  `0x00458920`), type 0x0A (r 0.223 with `0x0047cd50`, r 0.463 without), type 0x11 (r 0.15, callback
  `0x00486630`).
- Callbacks in 14,420 frames: `0x00486630` 3,001 (type 0x11 vs car slot 2), `0x00458920` 2 (pickup slot 4
  by car slot 2, frame 43750; pickup slot 8 by car slot 1, frame 49050).

## 2. Coverage hole found and closed

The pre-registered G-M1-PASS compared slot bytes, and `o_rg1` had 0 callbacks. The every-frame re-capture
`o_rg3` (13,387 frames) also showed 0 "changed" frames. That was the instrument, not the game:
`FUN_00484f70` sets **no** pair bits (only the grid pass `FUN_00484de0` does), so on the live path a firing
leaves no trace in the slots. The probe now hooks the callback targets' entries; `o_rg4` then carried 3,003
firings. `o_rg3`'s null is void.

## 3. The precision-scope bug (fixed in this leg)

`_controlfp_s(&old, new, mask)` returns the control word AFTER the change in `old`. Every `PcScope` (and
`PathNodes.cpp:356`) took `old` from the setting call, so the destructor re-applied PC_24. Measured with
`MASHED_LOG_CW` (`cw_check.txt`): control PC_53 at frames 0..1800; `MASHED_PATHPROG` PC_24 from track load on;
`MASHED_REGISTRY` PC_24 from frame 600 on. After the fix (`cw_check_fixed.txt`) all three arms run at PC_53.
Impact on reported results, measured by re-running on the fixed build:
- `P3F_LIVE` is byte-identical to `P3_LIVE` (`fc8c2faf / 1fdfe687 / bfd2c387`): RESULT_P3 stands.
- `P3F_PP` (`4e250237 / 22cceec1 / dc72ab09`) first differs from `P3_PP` at frame 1134 (car 3 `rec_9c4`,
  `-0.00185375079` vs `-0.0018537509`); its (b) windows are identical. RESULT_P2 §1.2 stands.
- The replay gates (P2, P3, M1) run inside a deliberate PC_24 scope and are unaffected.
- The D2 probe (`verify/d2_pc24_20261010`) used its own per-frame setter, not this scope, and ran solo
  with the physics on SSE2: not affected.

## 4. Method

Nine bodies verbatim naked (`re/tools/gen_naked_asm.py`, spec `re/tools/naked_specs/registry_m1.json` ->
`Race/WorldRegistry_naked.inc`). The generator now accepts a list of instructions per patch, used for the four
indirect callback calls (`0x00484edb`, `0x00484ee9`, `0x0048500e`, `0x0048501c`): store the target in
`g_rg_cbTarget`, call `RG_CbDispatch`, which in M1 records and does nothing. Wired in the exe: `FUN_00484c90`
at race reset (`Ai_BridgeLoad`, as `FUN_004111c0`), `FUN_00485070` per frame in `UpdateRace`.

## 5. Next: leg M2

Registrants: the pickup pool at the original layout and the logic half of `FUN_00459000` (its sprite calls
no-op, `PickupField` still draws), and the car registrant `FUN_0041f710`. The pickup position comes from
`*(entry_obj + 4) + 0x40` (a RW frame translation) and has no exe equivalent: the first M2 question.
Gate: the exe's registry contents per frame vs `o_rg4`'s (types, radii, positions within the ported
trajectories' own tolerance, callbacks). M2 does not need the publish order vs the AI tick; M4 does.

## 6. Raw

`o_rg{1,2,3,4}.msd.reg.tsv` (+ logs, provenance), `m1_replay_{rg1,rg4}_pc{24,53}.txt`, `run_m1_replay.ps1`,
`cw_check.txt`, `cw_check_fixed.txt`, `run_cw.ps1`, `decomp_registry*.txt`,
`../d3_gatefire_20261008/{M1_CTRL,M1_ON,M1F_ON,P3F_PP,P3F_LIVE}.*`.
