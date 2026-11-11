# RESULT PARTICLE-3-SWAP (2026-11-10)

Verdict: **GREEN.** On the MASHED_ORIG_TOP path, three rows now run their own bodies in place of stand-ins:
- 0x0048e820 runs its unedited near-match source (L-118 = a: admitted per function under L-115 / L-117, port axis
  only, never C4).
- 0x0048cee0 runs its matched body and builds all 17 particle-bank records at 0x00722470.
- 0x0048cf70 runs its matched body and tears them down through the matched 0x004768c0.

## Change (exe-only, `// RVA-LINT: ORIGTOP-ONLY`)

**`Match/PortTop_0048e820.cpp`**
- Includes `re/match/s15/0048e820.c` unedited, as `Orig_0048e820`. Byte table:
  `RESULT_PORT-OT-PARTICLE-3.md` (18/18 differing bytes = the ESI <-> EDI swap, relocheck 48/48 OK).
- **The emitter index.** The original keeps `i` in EDI across `call 0x0048cee0`, and 0x0048cee0 reads it as its
  argument (`re/match/s14/0048cee0.c` header: Ghidra `unaff_EDI`). The unedited s15 source calls `FUN_0048cee0()`
  with no argument, and VS2022 carries nothing in EDI.
  - Every iteration first calls `FUN_004b6480(&DAT_00722250[i * 8], 0x100)` (s15 source line 58). Its bridge
    `PortTop_PS3_BitClear_004b6480` records `i = (p0 - 0x00722250) / 0x20`, then runs `Orig_004b6480`.
  - `PortTop_PS3_Call0048cee0` then passes that `i` to 0x0048cee0.
  - `s_emitter` is file-static and owned by this TU. Nothing else reads it.
- `PortTop_PS3_Call0048e820(int)` is what the caller 0x0040bbb0 calls. 0x0040bbb0 pushes 0x20
  (`re/match/s02/0040bbb0.c:104`), and the body does not read it. The wrapper logs the record check once.

**`Match/PortTop_0048cee0.cpp`**
- Includes `re/match/s14/0048cee0.c` (100%, 140 B): `static void FUN_0048cee0(int i)` as `Orig_0048cee0`, exported
  through `PortTop_PS3_Cee0_0048cee0(int i)`.
- Its callees:
  - 0x0040bb30 = `Search40bb30`;
  - 0x004770c0 = `Orig_004770c0`;
  - 0x00476cb0 = `Batch476cb0`.
- The source's own stand-in caller is renamed (`PortTop_PS3_s14StandinUnused_0048e820`) and never called.

**`Match/PortTop_0048cf70.cpp`**
- Includes `re/match/s15/0048cf70.c` (BYTE-IDENTICAL, 28 B) as `Orig_0048cf70`, with 0x004768c0 = `Orig_004768c0`.
- `PortTop_PS3_Call0048cf70` logs the record count before the call.
- The source's callee declaration changed from `void FUN_004768c0(int p0)` to `(void *p0)` so the C++ port compiles
  (the body passes `undefined *`). After the change: `match.py --bytes` BYTE-IDENTICAL 28 B, relocheck OK 2/2.

**Callers rebound**
- `Match/PortTop_0040bbb0.cpp`: `#undef FUN_0048e820 / #define FUN_0048e820 PortTop_PS3_Call0048e820`, in the
  rebound block below the stand-in list.
- `Match/PortTop_0040bd00.cpp`: `FUN_0048cf70` changes from `PortTop_StandIn04C_0048cf70` to `PortTop_PS3_Call0048cf70`.

`exe_sources.rsp`: +3 TUs.

## Records: builder and owner

The 17 records of 0x40 bytes at 0x00722470..0x007228b0:
- **Builder:** only 0x0048cee0, called once per emitter from 0x0048e820 at init (0x0040bbb0). It calls
  `0x004770c0(&rec[i], 0x817, 0x100, tex)` and `0x00476cb0(&rec[i], 5, 6)`, and zeroes `DAT_007668b0[i]`.
- **Readers:** only 0x0048cf70 at teardown (0x0040bd00), through 0x004768c0. That reads `+0x4` (the object 0x004770c0
  created) and frees `+0x10..+0x28` through RwGlobals `+0x10c` when non-zero.
- 0x0048e820 also writes the emitter parameters at 0x00722140 (0x10 B x 17), the particles at 0x007228b0
  (0x40 B x 256 x 17), and clears 0x100 bits per emitter at 0x00722250.
- No other exe code names any of 0x00722470 / 0x00722140 / 0x00722250 / 0x007228b0 / 0x007668b0 (`git grep` over
  `mashedmod/src`). None of these addresses is in SEED_CANDIDATES.tsv: they are built at runtime, not seeded.

## Evidence

The `[ORIG_TOP]` lines, identical in shape in all three boots (`knob_boot.py`, `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000`):

| boot | rc | 0x0048e820 line | 0x0048cf70 line |
|---|---|---|---|
| RWMOD off (`knob_on.log`) | 0, 3000 frames | :346 `near-match body ran ...; 0x0048cee0 built 17 of 17 records at 0x00722470 with +0x4 non-zero (rec[0] +0x4 = 0F5A8C60, rec[16] +0x4 = 0F5A74A8), last emitter 0` | :1165 `matched body ran, 0x004768c0 on 17 records (17 with +0x4 non-zero before the call)` |
| RWMOD on (`knob_on_rwmod.log`) | 0, 3000 frames | :346 same, 17 of 17 | :1180 same, 17 |
| RWMOD on, second run (`knob_on_rwmod2.log`) | 0, 3000 frames | :346 same, 17 of 17 | :1180 same, 17 |

"Last emitter 0" agrees with the original's loop order (i = 16 down to 0, `dec edi` at the top of the loop).

## Gates

| check | result |
|---|---|
| build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, `.asi` not deployed. mashed_re.exe sha256 `e241b044c6eb2f8928aa1ea6f04534e1139e955b3db6b5134e3c2f619a22fb2d` |
| 12 captures, RWMOD off / on (`capflake_runs.py nav1`, `nav_rwmod`) | 8/8 `rc=0 pinned pos=55 hash=28538c74 class=E eqE=12` |
| walk_04 | `ba34030d1b7e2fe4` in all 8 rows |
| G-CTRL (H_DEF) | `8284be46 / 67931834 / 31fe043e`, unchanged |
| B (K0 / K1 / K26) | `1d5bf4f9` x3, unchanged |
| rva-lint | OK, 35 known, 0 new. A comment beginning with an RVA directly above the 0x0048e820 wrapper first bound it as a second 0x0040bbb0 body; it was reworded. |
| BINDINGS | `update_bindings_particle3swap.py` through `bindings_rows.apply`: 0x0048e820 `near-match`, 0x0048cee0 / 0x0048cf70 `matched` (507 -> 510 rows) |
| port column | `backfill_exe_file.py` 3 rows; `backfill_port.py` 3 rows changed. 0x0048e820 none -> called, 0x0048cf70 none -> called, 0x0048cee0 none -> body. 0x0048cee0 is a `static` function in its matched source, so the map has no symbol for it and the call-path rule cannot see the call ("body with no map symbol" 62 -> 63). It runs (the 17-of-17 line above). |

Launch setup:
- Every launch ran inside one `test-slot bash -c 'mashed_lock.py run --label PARTICLE-3-SWAP -- ...'`.
- Roots: `D:/MashedWT/POWERUPINIT-C_root` for boots and captures, `_rootA` for gates.

**Default path.** All three TUs are ORIGTOP-ONLY and are reached only from 0x0040bbb0 / 0x0040bd00 on MASHED_ORIG_TOP.
The default-path gates are unchanged.
