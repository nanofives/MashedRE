# PREREG: mode 7 leg M4, the consumer `FUN_00414c30` + `FUN_00414490` on the faithful registry

Date 2026-10-10. D-11074 (USER DECISION full broker, M1..M4; M3 `../d3_mode7_m3_20261010/RESULT_M3.md`). Committed before any
port code. Measurement tool added first: `scenario_launch.py --c30-probe N`, report `re/tools/c30_report.py`.
Decomps: `../d3_modes37_20261009/decomp_chain.txt` (`FUN_00414c30`, `FUN_00414490`), `decomp_leaves.txt`; control flow
`../d3_gatefire_20261008/ctrlstep_decomp.txt` lines 63-93; bytes read with capstone.

## 0. Measured before code

### 0.1 Original, `o_c30a` / `o_c30b` (o_wobj2 recipe + `--c30-probe 20000`, 60 s)

| quantity | o_c30a | o_c30b |
|---|---|---|
| `FUN_00414c30` calls | 5,327 | 5,450 |
| registry `cc` at entry | **0 on 5,327 / 5,327** | **0 on 5,450 / 5,450** |
| car slot position == that car's record `+0x958` at the call | **21,296 / 21,296** | **21,788 / 21,788** |
| returns 2 (mode 7) | car 1: **49** (frames 47400..49800, consecutive), target pool 0 (-1.0093, -20.2197) | car 1: **49**, same target |
| returns 1 | 0 | car 3: 2,430, all at the frozen post-round frame 56850 (type 0x11 slots present) |
| inputs on the 49 mode-7 calls | refdist 0, held 0, `DAT_0063ba8c` 6 | same |
| nested `FUN_0041f030` / `FUN_0048a630` (type 0x15 arm) | 0 | 0 |

**Order.** `FUN_00414c30` is reached from `FUN_00416250` <- `FUN_00418560` <- `FUN_00418860` <- `FUN_00425a40` (`0x00425a73`),
the first call in `FUN_0040fc00` (`0x0040fc6c`), before the car registrant (`FUN_0040da50`, `0x0040fc75`), the pickup tick
(`FUN_0045bba0`, `0x0040fcd0`) and the publish (`FUN_00485070`, `0x0040fd07`). The two measurements above are the runtime
statement of it: the consumer reads the previous frame's complete publish, and no car has moved since it was registered.
So the "reads d8 while slots [0, cc) are being rewritten" concern does not arise in the original: `cc` is 0 at every read.

### 0.2 Bytes (`FUN_00414c30`, `0x00414c30..0x00414ef0`, 705 bytes incl. the `fstp / jmp` block at `0x00414eea`)

Per published slot (`FUN_00484c70`: `0x006dccb8`, count `d8`), skipping `(int)type >= 0xff00`:
- type 0x15: frustum test (`FUN_0041f030`, `FUN_0048a630`), then `FUN_00414300(..., r*[0x005ccac8] + [0x005cc32c])` -> 1.
- type 0x0A with data object whose `+0x10 < [0x005ccac0]` (0.4): skipped.
- else: `r` (doubled for type 0x0D), `d = car - slot`; if `d.y < r` and `|d| < [0x005cc9f4]` (8.0):
  - type != 7: `FUN_00414300(..., r + 0.5)` -> 1.
  - type 7: `FUN_00442cc0(v) == 0.0` and `FUN_00414490(...) != 0` and `[0x005cd0b0] < car.y - slot.y < [0x005cc950]`
    (-0.75, 0.75) -> **2**. `FUN_00414490`: `FUN_0040e350()` not 11 and not 10, held word `[0x0088fc88 + v*0xb4] == 0`
    (`0x004144b3`), and `dot(norm(fwd.x, 0, fwd.z), norm(slot - car).xz) > [0x005cd090]` (0.866) -> cand = slot xz, 1.
`FUN_00416250` commits mode 7 (look = cand, `ctrl[4] = 0x40` at the tail) on 2 + LOS (`FUN_00416060`), mode 3 on 1 + LOS
(`0x004163c3..0x004163f6`), and calls `FUN_004150e0` otherwise (stubbed 0 in the exe).

### 0.3 Exe today and the prediction

The exe's `ControlStep` (`Ai/AiStandalone.cpp`) already has the mode-7 tail (`m == 7 -> ctrl[4] = 0x40`) and reaches the
`LAB_004163b7` point whenever `FUN_00414a70` returns 0 (always, without `MASHED_TGT3`). From `M3B_ON` (registry + gates traces,
heading taken from the position delta): car 1 meets every condition from **registry frame 97** to 124 (27 frames, target pool 0,
refdist 0 until frame 124); car 2 meets them on frames 16..18 (pool 4) and 127..128 (pool 0), because the exe's refdist bridge
gives car 2 refdist 0 there (the original's car 2 never has refdist 0 before the freeze: 0 / 890 calls).

## 1. Decisions

| callee | decision | evidence |
|---|---|---|
| `FUN_00414c30`, `FUN_00414490`, `FUN_00484c70`, `FUN_00442cc0`, `FUN_004c39b0` | verbatim naked, spec `re/tools/naked_specs/mode7_m4.json` -> `Ai/AiModeSeven_naked.inc`, in `Ai/AiTargetClosest.cpp` with P3's bindings | |
| `FUN_0046d4a0`, `FUN_0046d510`, `FUN_004c3ac0` | P3's `TG_VehPtr`, `TG_Fwd` (the `at` row; does not store `+0x9d4`, U-9213), `TG_4c3ac0` | RESULT_P3 |
| `FUN_00414300` | P3's verbatim `TG_414300` (replay: captured returns, arguments checked) | G-P3-REPLAY 5,490 / 5,490 |
| `FUN_0040e350` | `aib_game_sub_mode()` (as M3) | |
| held word at `0x004144b3` | PATCH to `TG_HeldWord(v)` = the exe's `PowerupSystem` (`aib_held_powerup`); EAX / ECX preserved, EDX = result | the exe never writes `0x0088fc88`; a global mirror would change the default build through the three AI sites that read it (RESULT_M3 finding 4) |
| `FUN_0041f030`, `FUN_0048a630` | counted stubs returning 0 | type 0x15 absent on Training (0 calls in both captures, FINDINGS §2) |
| `.rdata` | exe-safe copies (values read from the image) | |

Knob `MASHED_MODE7` (default OFF), effective only with `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS +
MASHED_REG_CALLBACKS`. ON: `ControlStep` calls the port at the `LAB_004163b7` point (`FUN_00414a70` returned 0, or returned 1
and LOS failed) and commits mode 3 / mode 7 as the original. x87 PC_24 under the fixed `PcScope` (`MASHED_MODE7_PC`).
No change to the exe's frame order: it must reproduce the relation of §0.1, checked by G-M4-ORDER.

## 2. Gates

| gate | pass rule | prediction |
|---|---|---|
| **G-M4-REPLAY** | `MASHED_C30_REPLAY` on `o_c30a` + `o_c30b` (10,777 calls): `ret`, `cand_out` and the nested call list (`FUN_00414300` arguments + returns, `FUN_00414490` returns) equal, PC_24 | 10,777 / 10,777 (PC_53 diagnostic: equal) |
| **G-M4-OFF** | knob OFF and the M3b set without `MASHED_MODE7`: `ae67b13a / c8317ce2 / 31db9384` | PASS |
| **G-M4-ORDER** | knob ON, exe trace: `cc == 0` at every `FUN_00414c30` call and every car slot == that car's record `+0x958` | PASS [UNCERTAIN: the exe publishes in UpdateRace, before its AI tick; if a physics step runs between them the second clause fails, and that is then the finding] |
| **G-M4-STUBS** | 0 calls to `FUN_0041f030` / `FUN_0048a630` | PASS |
| G-M4-COUNT (report) | mode-7 commits per car vs the original (car 1: 49, cars 2/3: 0) | car 1 from frame ~97 on pool 0, fewer than 49; car 2 some frames (refdist bridge residual); the race changes from the first commit |
| (b) per regime, collateral (report) | as M3 | no prediction |

## 3. Not in M4

`FUN_004150e0` (mode 9), `FUN_00414f00` (mode 10), the type-0x0A / 0x11 registrants, the refdist bridge's car-2 zeros.
