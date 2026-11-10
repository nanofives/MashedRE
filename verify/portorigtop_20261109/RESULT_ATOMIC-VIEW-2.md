# RESULT ATOMIC-VIEW-2 (2026-11-10): GREEN, entry 1's 0x004548e0 (DepthCharge) on its matched body; oil-slick texture settled (original has one, the exe does not), filed U-9802

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/ATOMIC-VIEW-2` on
`orch/ATOMIC-VIEW` `80b2d88bc`, rebased onto `orch/integration` `5fc6a43e4` (which merged ATOMIC-VIEW). There is no
`mashedmod/` difference between those two bases. With this row, 8 of the 9 type-table inits run matched bodies. Only
entry 2 (0x00455400, 94.0%, row POWERUPINIT-B2) is still a stand-in.

## What changed

| rva | before | now | TU |
|---|---|---|---|
| 0x004548e0 | stand-in `PortTop_PWI_InitStandIn_1` | `Orig_004548e0` via `PortTop_PWI_Init_1` | new Match/PortTop_004548e0.cpp = re/match/s09/004548e0.c (C4) |
| 0x004548a0 (+ statics 0x004547c0, 0x00454170) | (not reached) | `Orig_004548a0` | new Match/PortTop_004548a0.cpp = re/match/s09/004548a0.c (C4) |
| 0x004e69a0 RpAtomicClone | n/a | `MashedRw_AtomicClone_004e69a0` | LibRw/RwApiBind.cpp |
| 0x004e7e30 RpAtomicSetFrame (librw atomics) | n/a | `MashedRw_AtomicSetFrame_004e7e30` at this call site; `Orig_004e7e30` stays the PTank body | LibRw/RwApiBind.cpp |
| 0x004c1040 RwFrameAddChild | n/a | `MashedRw_FrameAddChild_004c1040` | LibRw/RwApiBind.cpp |
| 0x004c39b0 RwV3dNormalize | n/a | `MashedRw_V3dNormalize_004c39b0` = Math::LutNormalize | LibRw/RwApiBind.cpp |
| 0x004c15c0 RwFrameSetIdentity | n/a | `MashedRw_FrameSetIdentity_004c15c0` | LibRw/RwApiBind.cpp |
| 0x004c1210 RwFrameRemoveChild | n/a | `MashedRw_FrameRemoveChild_004c1210` | LibRw/RwApiBind.cpp |
| 0x004e4440 RpAtomicGetWorld | n/a | `MashedRw_AtomicGetWorld_004e4440` | LibRw/RwApiBind.cpp |
| 0x004e43b0 RpWorldRemoveAtomic | n/a | `MashedRw_WorldRemoveAtomic_004e43b0` | LibRw/RwApiBind.cpp |

### Bindings stand only for RW library functions

All eight are filed `third-party-library[renderware]` in hooks.csv. Their matched bodies operate on RW 3.x's own
layouts: the RwFrame child list at +0x98 / +0x9c and the root at +0xa0 (0x004c1210, 0x004c1040, 0x004c15c0), and the
world plugin slots [0x007d7170] (0x004e4440, 0x004e43b0). The exe's librw objects do not have those layouts.

Two need a note:
- **0x004c1210** is C4 by match, but it is RwFrameRemoveChild: a RenderWare library function, not a game function. So
  it is bound, as the brief allows. Its matched body walks the RW child list.
- **0x004c39b0** is RwV3dNormalize (C2). The binding computes it with Math::LutNormalize, which is this function's
  formula on the original's two RW sqrt / inverse-sqrt tables (Math/RwLutTables.cpp). The original's zero-length
  error report is not raised.

### The view

- **0x004b3f90:** for DepthChargeRound.dff, 0x004548e0 receives the existing RW-layout view (MashedRwAtomicView).
- **0x004e69a0 (RpAtomicClone):** resolves the view to its librw atomic and clones that, so all eight round atoms are
  librw atomics.
- **Fields the matched body reads off those atoms:** only `atom +0x04`, the frame (rw::Object::parent, the same
  offset), and `atom +0x02` through 0x004b5240 (rw::Object::flags). Both offsets agree in librw.
- **0x00454170** reads `frame +0x04` (the parent) of a librw frame. rw::Frame starts with rw::Object as well.

### Builders of what the bodies read

- **0x004548e0:** zero-fills both of its arrays (0x00688240, 0x00688020) before it writes them. Its clumps come from the
  matched 0x0042a5d0. Its frames come from the RwFrameCreate binding.
- **0x004548a0's statics:** read only what 0x004548e0 wrote just before them:
  - the round pointers +0x00 / +0x04 (zero), the clone +0x20, and the round +0x30 / +0x34 / +0x38
  - +0x34 has no parent, so 0x004c1210 is not called
  - the atoms and clones are in no world, so 0x004e43b0 and 0x004e45b0 are not called
- **0x005f9808** (12 B .data, {8, 1, 0}), which 0x004c39b0 normalizes in place, was 0 in the exe pad. It is now in the
  port-state seed (SEED_CANDIDATES.tsv, `gen_portstate_seed.py`: 263 entries, 3192 bytes, fnv 0xce5a7775,
  `--check` OK).

## The oil-slick texture slot (from ATOMIC-VIEW (i)): settled

One original-side read, on a MASHED.exe I spawned (pid 5032), under test-slot and mashed_lock. The Frida script
`oilread.py` logs at 0x0040cda0 / 0x0040bae0 / 0x0040baa0 / 0x0053fd30 during the boot-time init (4 calls each), then
kills only that pid. The script is not committed. Its output is `D:/MashedWT/ATOMIC-VIEW-2_data/oilread.json`.

| | original | exe (ATOMIC-VIEW (i)) |
|---|---|---|
| 0x0040cda0 | 4 calls, DAT_00636ad0 = 0, DAT_006905b0 = 0x0D77C09C, DAT_0067ecb0 = 0 | 4 calls, 0 / set / 33 |
| geometry +0x24 materials | 1 | 1 |
| material +0x00 (texture) | **0x0ED90410** (all 4) | **0** (all 4) |
| 0x0040baa0 arm | env map: 0x0053fd30(mat, texture 0x0D782A30, frame 0x0420C8B8), 4 calls | `0x0053fbb0(mat, 0)`, 0x0053fd30 not called |

So the original's oil-slick material has a texture and gets the env map. The exe's does not.

- **The matched bodies are not the cause:** 0x0040baa0 branches correctly on what it reads.
- **The cause is the exe's clump read:** the oilslicker.dff material names texture `PowerUpModelsFinal` (a string in
  the DFF), and librw's material reader inside the RpClumpStreamRead binding leaves `rw::Material::texture` null.
  Both sides call 0x004c5c80(0) in an earlier init before the load.
- **Filed U-9802** (UNCERTAINTIES.md): how RW resolves that name, and the path to resolution. This is clump-read
  binding work, outside this row. Visible effect today: none, because nothing on the knob path draws these clones.

DAT_0067ecb0 reads 0 in the original at this point, while the exe reads 33. Both are != 3, so 0x004672d0 takes the
camera branch on both sides.

## Acceptance

Machine rules as before: the lock waits are logged in this session, every launch went through test-slot and
mashed_lock, and the private roots were `D:/MashedWT/POWERUPINIT-C_root` / `_rootA`.

| check | result |
|---|---|
| syntax | `cl /Zs` on Match/PortTop_004548e0.cpp, PortTop_004548a0.cpp, PortTop_0045bae0.cpp and LibRw/RwApiBind.cpp: no error |
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, mashed_re.exe sha256 `9af1ce3230f191d9669ed053417c0c7ae1987f24d4a03b4a488854256e21bcaf` |
| knob-ON boot, RWMOD off | `exited rc=0 (0x00000000) after 59.6 s`. :308 `9 init pointers stored (8 matched, 1 stand-in)`. :329.. 8 x `0x004e69a0 -> rw::Atomic::clone`. :337 `type table entry 1: rva 0x004548e0: matched body returned, first DepthCharge clone (0x00688260) = 10A61468`. :1183 `WinMain ... returned 0 after 3000 frames` |
| knob-ON boot, RWMOD on | `exited rc=0 (0x00000000) after 52.3 s`, :398 entry 1 matched, 8 clones, :1253 `returned 0 after 3000 frames` |
| 12 captures, RWMOD off | 4/4 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12`. Both ON logs: entry 1 matched, 8 clones, `returned 0 after 578 frames` |
| 12 captures, RWMOD on | 4/4 class E eqE=12, the same evidence |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows. No capture moved. runs.tsv sha256 f560646e…, the same 8 rows as before |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new (53 ORIGTOP-ONLY TUs) |

**Default path.** The new TUs are reached only from the 0x0045bae0 forwarder on MASHED_ORIG_TOP, and the new bindings
only from them. The seed gains 0x005f9808, which only these TUs cite. Gates unchanged.

## BINDINGS, port column, trackers

**BINDINGS:** `verify/portorigtop_20261109/update_bindings_atomicview2.py` runs `update_bindings_atomicview.py` (the
chain tip on `5fc6a43e4`, which reproduces 500 rows). It then writes:
- 0x004548e0: stand-in -> matched.
- Matched: 0x004548a0.
- Bound: the eight RW functions.
- A note on 0x004e7e30.

Result: 500 -> 506 rows; matched 242 -> 244, stand-in 63 -> 62, bound 52 -> 59, rw 29 -> 27.

**Port column:** `backfill_exe_file.py` gave 4 rows an exe_file. `backfill_port.py`: `none=5092 body=552 called=525`,
4 rows changed, none -> called: 0x004548e0, 0x004548a0, 0x004547c0 and 0x00454170.

**Trackers:** UNCERTAINTIES.md +1 row, U-9802. No C-level move.

## Raw files (D:/MashedWT/ATOMIC-VIEW-2_data, not committed)

| file | sha256 (first 16) |
|---|---|
| build.log / mashed_re.exe | 23e0150db95d3314 / 9af1ce3230f191d9 |
| knob_on_1.log / knob_on_rwmod.log | 8d92d9712deeb324 / 7f65d05da46cfdbb |
| runs.tsv | f560646e9825f27b |
| capflake_nav1.stdout / capflake_rwmod.stdout | a2c7e3c7e630db1e / ce94d8715058ea57 |
| arms.stdout | 077753a1339109d7 |
| arms/H_DEF step / gates / ptrace | 8284be46 / 67931834 / 31fe043e |
| arms/B_DEF_K0, K1, K26 step | 1d5bf4f9 x3 |
| oilread.json (original-side read) | 7d0f6e065fb609e7 |

## Next

- **U-9802:** clump-read texture resolution, which brings entry 7's env-map setup to parity.
- **POWERUPINIT-B2:** entry 2 (0x00455400, 94.0%), the last type-table stand-in.
