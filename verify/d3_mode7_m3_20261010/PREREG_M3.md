# PREREG: mode 7 leg M3, the registry callbacks live (pickup collection on the original's path)

Date 2026-10-10. D-11074 (USER DECISION: full broker, staged M1..M4; `../d3_mode7_20261010/PREREG_MODE7.md` §2 row M3).
Committed before any port code. Measurement tools added before this file (no behaviour change):
`scenario_launch.py --cb-probe`, `re/tools/pu_cb_unicorn.py`, `re/tools/pu_cb_report.py`.
Decomp: `decomp_m3.txt`, `decomp_m3b.txt` (this folder); bytes read with capstone from `original/MASHED.exe.unpatched`.

## 0. Measured before code

### 0.1 Original, three fresh captures (o_rg4 recipe + `--cb-probe`, 240 s, Training, 4 cars)

| capture | FUN_00458920 calls | event 1 | event 2 | ticks between | respawns |
|---|---|---|---|---|---|
| `o_cb1` | 2 | car 2 takes pool 4 (type 12), slot a=4 | car 1 takes pool 0 (type 19), slot a=9 | 106 | both 300 ticks after their 1->2, timer sum 5.000010 |
| `o_cb2` | 2 | same (slot a=5) | same | 106 | same |
| `o_cb3` | 2 | same (slot a=5) | same | 106 | same |

At every call: arm = collect, `DAT_007f0fd0` = 0, `DAT_0063ba8c` = 6, car slot `+0xa8` = 0 before (non-zero after:
`0x5f9a58` / `0x5f9b58`, table entries), `DAT_0063a5d0/d4` = 0/0, `DAT_00639dc4` = 0, pool `+0x18` = 5.0, `+0x1c` = 0 -> 5.0,
state 1 -> 2, witnesses `88c0 | c010:v:type | 5ca0:28` in that order, particle pool free slots 30 / 30.
The ring cursor moves by 27 positions mod 31 on all 6 calls = 120 draws (10 particles x 12 draws in `FUN_00490020`:
11 x `FUN_00472650` + 1 x `FUN_00472690`, `0x00490181..0x004902c8`; the `0x00490311` draw needs `param_4 == 1`, here 0).

Respawn: `FUN_00459000` state 2 (`decomp_registry.txt` lines 84-98): `t <= 0` -> state 1, `t = +0x18`; else `t -= _DAT_007f100c`.
Training's 5 pickups all have `+0x18` = 5.0 (exe `PUPLACE ... respawn=5.000000`; the original's two collected ones 5.0),
not 120.

The round ends 178 ticks after event 1 in all three captures: from then to the end of the capture every car's registered
position is constant (o_cb1 calls 1200..14100) and the frame counter `0x007f0ff4` stops (51450 / 52800 / 54100).

### 0.2 Type 0x11 (`FUN_00486630`) and type 0x0A (`FUN_0047cd50`)

- `FUN_00486630`: 6,002 / 3,001 / 3,001 calls, **all after the round-end freeze**, each capture against one car only
  (car 3 / car 1 / car 2). Its gate `data+0x24 <= _DAT_005cc9bc` (0.8, `0x00486678`) passes on 80%; then it writes the car
  record `+0x144..+0x14c` (`FUN_0046d740`, 1,304..2,463 changes) and draws from the ring (2,400..4,800 calls moved the cursor).
  Registrant: the descriptor store `mov [esp+0xd0], 0x486630` at `0x00486dd3`; not in the exe.
- `FUN_0047cd50`: 0 calls in o_rg1..4 + o_cb1..3. It acts only on types 0x19 / 0x11 / 0x10 (`0x0047cd5a..`), never a car.
- **Decision: neither is ported in M3.** Neither fires while cars move; neither touches a pickup. Recorded as an M3 residual
  (a post-round effect on one car; port with its registrant if a post-round phase is ever scored).

### 0.3 Bytes (corrections to the kickoff wording)

- `FUN_0045c330(v)` = `FUN_0045bff0(v) == 0` (`neg / sbb / inc`, `0x0045c33d..0x0045c341`); `FUN_0045bff0(v)` =
  `[0x0088fc88 + v*0xb4] == 0` (slot `+0xa8`, the active table entry).
- **The bump arm runs when `FUN_0045c330 != 0`**, i.e. when the car's slot `+0xa8` is NON-zero (it already holds a powerup):
  `test eax, eax / push edi / je 0x458a04` at `0x004589ba..0x004589bd` jumps to the collect arm on 0. The kickoff said
  "`FUN_0045c330 == 0` -> bump"; the bytes say the reverse. Confirmed in the emulator (§0.4).
- The bump arm passes EAX = car, ESI = `&local` (`[esp0-0xc]`) and pushes the pool entry to `FUN_00458570`, discards its ST0
  (`fstp st(0)`, `0x004589e2`) and stores the constant vector `(0, 0x3d23d70a, 0)` from `[esp0-0x18]` to `+0x44..+0x4c`.
  `FUN_00458570` writes only `[esi]` and its own frame; its callees `FUN_0046d4a0` (read), `FUN_004c3ac0` (read) and
  `FUN_004c39b0(esi, esi)` write nothing else. Its result is dead; it is ported verbatim anyway.
- Arms of `FUN_00458920(a, b)` (`a` = the pickup's slot, `b` = the other slot):
  1. `a.type != 7` or pool state `!= 1`: nothing.
  2. `fd0 == 5 && FUN_00405890() && b.type == 10 && b.type == FUN_00407580(0)`: state 2, `t = +0x18 - 1.0` (`0x0045896f`).
  3. `b.type` not in `0xff00..0xff07`: nothing.
  4. `FUN_0040e350() != 6`: state 2, `t = +0x18 - 1.0` (same block).
  5. `FUN_0045c330(v) != 0`: bump.
  6. else collect: state 2, `t = +0x18`, `FUN_004588c0(entry)`, `FUN_0045c010(v, +0x24)`, `FUN_00465ca0(0x1c, entry+0x38)`.

### 0.4 Emulator reference (`pu_cb_unicorn.py`, original bytes, PC_24)

3,888 synthetic vectors (grid over type_a, state, fd0, `a5d0/d4`, type_b, `9dc4`, `ba8c`, slot `+0xa8`):
no-op 3,522, collect 60, bump 60, vanish (arms 2 + 4) 246. Written as PK rows, `synth_unicorn.cb.tsv`.

### 0.5 Exe, knob OFF, from the M2b trace (`M2B_ON.registry.csv`, the exe's own registry, cars + pickups)

- Car 2 passes pool 4 at minimum 3D distance **1.0983** (frame 33), outside the sphere sum 0.5 + 0.53339 = 1.03339.
  The exe's `PickupField` collects with `xz <= worldR * 0.04` = 1.1007 (`PickupField.cpp:249/277`) and so arms car 2 with
  type 12 at frame 31 today (registry position as proxy for `cars[s].pos`).
- Car 1 first overlaps pool 0 at frame **143** (distance 1.0084).

## 1. Per-callee decision

| callee | decision | evidence |
|---|---|---|
| `FUN_00458920` | verbatim naked | this leg |
| `FUN_00405890`, `FUN_00407580` | verbatim naked (read exe memory at `0x0063a5d0/d4`, `0x00639dc4`) | reached only if `fd0 == 5`; `fd0` = 0 at every original call and has no exe writer |
| `DAT_007f0fd0` | read verbatim (`ds:[0x007f0fd0]`) | 0 in the original at every call; 0 in the exe |
| `FUN_0040e350` | bind to the exe's existing `FUN_0040e350` host `aib_game_sub_mode()` (`TrackRenderer.cpp:114`: 6, or the live `DAT_0063ba8c` under `MASHED_SUBSTATE`) | the exe has no `DAT_0063ba8c` writer without `MASHED_SUBSTATE`; a verbatim read would send every contact to arm 4. Same binding as the AI host and the dispatcher's `raceState = 6` (`TrackRenderer.cpp:4989`) |
| `FUN_0045c330` | verbatim naked | |
| `FUN_0045bff0` | bind: `!pw_.Armed(v)` (the exe's stand-in for slot `+0xa8`, as `aib_held_powerup`, `TrackRenderer.cpp:337`) | `PowerupSystem::Activate` / `Deactivate` set and clear `activeCode` (+0xa8) and `armed` (+0xac) together (`PowerupSystem.cpp:108-134`) |
| `FUN_00458570`, `FUN_0046d4a0` | verbatim naked; `FUN_004c3ac0` / `FUN_004c39b0` = M2a's `PK_4c3ac0` / `PK_4c39b0` (LUT patched) | result dead (§0.3) |
| `FUN_004588c0` | no-op, counted | render (particles). Deviation recorded: the original takes 120 ring draws per collection here (§0.1); the exe does not reproduce the ring call index of any render-side draw site (`AiStandalone.cpp:657`) |
| `FUN_0045c010` | bind: `PowerupSystem::Activate(v, type)` through a host in `TrackRenderer` (`EnsurePowerupBackend`, `SyncHostCar`, `pw_.Activate`), run at PC_53 (the exe thread's word, `../d3_mode7_20261010/cw_check_fixed.txt`) | `Activate` is the exe's port of `FUN_0045bfa0` (`PowerupSystem.cpp:100-119`) |
| `FUN_00465ca0` | no-op, counted | audio; its only global write `inc [0x008aa2d0]` (`0x00465d95`) has no other direct-address reference in `.text` |

x87: `FUN_00458920` runs inside `Registry_FrameTick`'s fixed `PcScope` (PC_24, U-9216 pattern), as the original's race loop.

## 2. Knob and stand-down

- `MASHED_REG_CALLBACKS`, default OFF. Effective only with `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS`; otherwise
  it is OFF everywhere (PickupField unchanged).
- ON: `RG_CbDispatch` runs `FUN_00458920` for target `0x00458920`; any other target is counted and not run (none expected:
  the exe registers only cars and pickups).
- ON: `PickupField::Update` and `PickupField::CollectAt` never collect; each orb's `active` mirrors its pool entry's state
  (`+0x20 == 1`) for drawing only. `held_` stays -1, so `TickPowerupDispatch`'s slot-0 held path and the slots-1..3 `CollectAt`
  path make no `Activate` call. Every grant comes from `FUN_0045c010`.
- Order in the exe frame (unchanged): `CarsReg_FrameTick`, `Pickups_FrameTick`, `Registry_FrameTick` (callbacks), then later
  `TickPowerupDispatch`. The original's order of `FUN_00485070` vs `FUN_0045bba0` is an M4 input (not needed here).

## 3. Gates

| gate | pass rule | prediction |
|---|---|---|
| **G-M3-REPLAY-LIVE** | `MASHED_PKCB_REPLAY` on `o_cb1..3` PK rows: per row, pool entry bytes `[4, 0x50)` after == captured, grant list == the `c010` witnesses, burst / sfx counts and ids == captured. 6 / 6 at PC_24 | 6 / 6 |
| **G-M3-REPLAY-SYNTH** | same on `synth_unicorn.cb.tsv`, 3,888 / 3,888 at PC_24 (PC_53 diagnostic) | 3,888 / 3,888 both |
| **G-M3-OFF** | knob OFF (nothing set) and the M2b set without `MASHED_REG_CALLBACKS`: `ae67b13a / c8317ce2 / 31db9384` | PASS |
| **G-M3-STANDDOWN** | knob ON: PickupField collections 0, `Activate` calls from the PickupField paths 0, every `Activate` from `FUN_0045c010` | PASS |
| **G-M3-LIVE** | knob ON, exe trace `MASHED_PKCB_TRACE`: (i) collect -> state 1 -> 2, `+0x1c == +0x18` bitwise, slot armed with code == pool `+0x24` after the call; (ii) bump -> `+0x44..+0x4c == (0, 0x3d23d70a, 0)`, state 1; (iii) arms 2/4 -> `+0x1c == +0x18 - 1.0`, state 2; (iv) each respawn 2 -> 1 exactly 300 ticks after its 1 -> 2 (dt `0x3c888888`, 300 decrements from 5.0); (v) 0 calls to any other callback target | PASS |
| G-M3-EVENTS (report) | exe events (tick, car, pool idx, type, arm) vs the original's §0.1 | car 2 does NOT collect pool 4 at the start (1.0983 > 1.0334); first collection car 1 / pool 0 / type 19 at frame 143 [UNCERTAIN: the knob-OFF run has car 2 armed with type 12 from frame 31, which may move car 1 before 143]; respawn 300 ticks |
| G-M3-CTRL-ON (report) | knob ON step / gates / ptrace hashes | all three differ from G-CTRL (car 2 not armed at frame 31) |
| (b) per regime, collateral (report) | `ai_ctrl_window.py --check-regime re/tools/b_envelopes.json`; `collateral.py` with the D2 watch list | no prediction |

U-9217 (spring arithmetic after a bump) closes only on an ORIGINAL capture spanning a live bump; the synthetic replay covers
the bump arm's transcription, an exe bump exercises only the port's spring path. None of `o_cb1..3` has a bump.

## 4. Not in M3

`FUN_00486630` and its registrant, `FUN_0047cd50`, the burst's ring draws, M4 (`FUN_00414c30` + `FUN_00414490`; note
`FUN_00414490` reads slot `+0xa8` at `0x004144b3`, which the exe keeps in `PowerupSystem`, not at `0x0088fc88`).

## 5. ADDENDUM M3b (2026-10-10, after the M3 runs, before any M3b code): the respawn seed `FUN_00458fa0`

**Measured with the M3 build** (`M3_ON`, `../d3_gatefire_20261008/M3_ON.cb.tsv`): both exe respawns took **7,200 ticks**;
every exe pool entry holds `+0x18 = 0x42f00000` (120.0), the verbatim `0x00458ef8` store in `PickupPoolSpawn.cpp:261`.
The original's 5 Training entries hold `0x40a00000` (5.0) from the first captured tick (`o_pk1` row 0) and respawn after
300. **G-M3-LIVE (iv) as registered FAILS** (7,200, not 300). The transcription is consistent with its own input (7,200 =
decrements of `0x3c888888` from 120.0); the input differs.

**The writer.** `FUN_00458fa0(idx, value)` (`0x00458fa0..0x00458fc5`): `0 <= idx < 0x19` -> `[0x0068b1b0 + idx*0x50] = value`
(`0x00458fb7`), return 1; else return 0. Callers (rel32 scan of `.text`): `0x00458fe8` (inside `FUN_00458fd0`, which has no
direct caller) and `0x0047b268`, the Lua glue at `0x0047b250`: `idx = FUN_00458e00(&DAT_006bf1bc, DAT_006132fc);
FUN_00458fa0(idx, DAT_00613300)`. `DAT_006132fc` / `DAT_00613300` are set by the glue functions `0x0047b200` (`_ftol2` of a
Lua number, `0x0047b215`) and `0x0047b230` (`fstp dword` of a Lua number, `0x0047b242`).

**Port.** `PickupPoolSetRespawn` at `0x00458fa0` in the shared TU `Gameplay/PickupPoolSpawn.cpp` (exe + asi),
`RH_ScopedInstall`. Called from `PickupField::InitReal` (the exe's stand-in for the `0x0047b250` glue) right after
`PickupPoolSpawn`, unconditionally (an index of -1 takes the guard), with the spawn's parsed respawn (`Spawn::respawn`,
POWERUPS_GOLD.LUA) as float bits. **Not behind a knob:** `+0x18` is read only by `FUN_00459000`'s state-2 arm (`t = +0x18` on
respawn, `decomp_registry.txt` line 87), which no entry reaches without a collection, and nothing in the exe reads the private
pool's `+0x18` (`PickupField` copies position and type only, `PickupField.cpp:210-218`).

| gate | pass rule | prediction |
|---|---|---|
| G-M3b-SEED | exe trace, first tick under `MASHED_PICKUPS_ORIG`: `+0x18 = 0x40a00000` on all 5 entries (= `o_pk1` row 0) | PASS |
| G-M3b-PATH1 | `run_diff.py pickup_pool_set_respawn` GREEN: accept idx 0 / 3 / 24, reject -1 / 25 / `0x80000000`, return folded | PASS |
| G-M3b-PATH2 | `run_verify_hook.py pickup_pool_set_respawn` GREEN | PASS |
| G-M3-OFF (re-run) | `M3B_CTRL` and `M3B_M2B`: `ae67b13a / c8317ce2 / 31db9384` | PASS |
| G-M3-LIVE (re-run) | `M3B_ON`, relations (i)-(v), (iv) = 300 ticks | PASS |
| events, (b), collateral | report | first collection unchanged (car 1, pool 0, type 19, tick 144); pool 0 back at tick 445 (144 + 1 + 300); more collections than `M3_ON`'s 4 [count not predicted] |
