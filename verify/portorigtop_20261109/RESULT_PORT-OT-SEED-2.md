# RESULT PORT-OT-SEED-2 (L-111 (a)): the L-99 seed carries the ORIG_TOP rows' initialised data

Branch `orch/PORT-OT-SEED-2` from `orch/PORT-OT-MEASURE` `165483e21`. Commits:
- `2a0447728`: inventory + regenerated seed + probe fix
- `4e361af81`: the build-mode bisect driver taken unchanged from orch/integration, where the PORT-BATCH-GAMEPLAY
  merge brought it
- this RESULT

## What changed

`verify/portorigtop_20261109/seed2_inventory.py` takes the unseeded addresses PORT-OT-MEASURE found:
182 distinct once a read at two sizes is merged to the larger one (the MEASURE RESULT's 180 counted address +
section). Every action is logged in `SEED2_ADDED.tsv`.

| action | entries |
|---|---|
| added | 158 |
| added + its string target | 2 (a pointer at a NUL-terminated string in .rdata / .data; the string is seeded too) |
| merged into an adjacent / overlapping entry | 3 |
| **left out: code pointer** | **22** |

**Why 22 are left out.** Each entry holds a function entry of the original (`re/console/pc_funcs.csv`):
- `PTR_FUN_` / `PTR_LAB_` / `PTR_thunk_` globals
- tables of them, e.g. 0x005cf9d0 / 0x005cfd30 / 0x005cfd70 / 0x005d0e74 / 0x005d0e88 and 0x00632c28..0x006351fc

In the exe those addresses are exe code, so the original values would be wild code pointers. These need a binding
(the exe symbol of each function), not a seed.

**Inventory and seed:**
- `verify/portstate_20261108/SEED_CANDIDATES.tsv` goes from 100 to 259 rows. New rows carry `exe=origtop` and the
  reading RVAs.
- `Compat/PortStateSeed.inc` regenerated with `re/tools/gen_portstate_seed.py`: 259 entries, 3,143 B, FNV-1a
  0xd8e382f0. `--check` is OK: inventory == .inc == binary.

**Probe fix (the two-size compare).** `ot_measure.py probe` now reads each address once, at the largest size any row
uses. Before, two reads of one address at two sizes were stored under one key, and the last read won. The verdict
compares by prefix.

## Gates (default path; the seed is default ON for every exe run)

`re/tools/port_batch_bisect.py --exe-off <base> --exe-on <seed build>` (build mode), inside test-slot + mashed_lock:
- OFF = `617d0a32`, the PORT-OT-MEASURE build = integration `3fbf77214`
- ON = `a1ad04be`, this branch
- roots: `D:/MashedWT/PORT-BATCH-TOOL_root` / `_rootK`

| pass | exe | G-CTRL | B (K0/K1/K26) | 12 nav captures vs 04E |
|---|---|---|---|---|
| OFF | base | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON | seed | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| OFF repeat | base | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |
| ON repeat | seed | `8284be46/67931834/31fe043e` | `1d5bf4f9` x3 | 12/12 |

- Unchanged on every pass. No bisect was needed, so no address was dropped for moving a gate.
- All arms rc 0. Every nav run ended `NAV_DEMO phase=28 done ok=1`. Table: `verify/portbatch_20261109/bisect_OTSEED2.tsv`.

An earlier run 1 is void and kept in `gate_run1_nobuildmode/`.
- This branch still had integration's older driver, which had no build mode, so all four passes ran the seed build.
- G-CTRL and B were at the refs on all four passes.
- Nav was 1/12 on the first pass and 12/12 after: the known first-run capture flake.

## ORIG_TOP probe: the 84 rows now read their original bytes

One knob-ON boot (MASHED_ORIG_TOP=1) of the seed build with the fixed probe, against the MEASURE row list:
- **71 of the 84** `needs seed rule` rows read the original bytes at every address they use, from 1.5 s on.
- **13** read original bytes at every seeded address. Their only remaining gap is the code-pointer data above, which
  needs a binding:

  | subsystem | rows |
  |---|---|
  | powerups | 0x0045b9d0, 0x0045baa0, 0x0045be90, 0x0045bf30 |
  | util | 0x00493b50, 0x00494ac0, 0x00494c80 |
  | render | 0x00494b20 |
  | audio | 0x005a6110, 0x005a6150, 0x005b36f0, 0x005bbb20, 0x005bbb70 |

- Over all 124 seed rows, 111 now hold their original bytes everywhere.
- No address that was seeded failed at runtime.
- RwGlobals in this boot: `0x00ff9290`, one page higher than in MEASURE: the heap moved with the larger seed. Same
  fake device.

## Evidence (raw data in `D:/MashedWT/PORT-OT-SEED-2_data`, not committed)

| file | SHA-256 |
|---|---|
| mashed_re.exe (seed build, a1ad04be) | a1ad04be8e6e0a4cc3faad4da9cd955a5c47cc00b3de9c58a2bc90e5989dd75d |
| OFF exe: PORT-OT-MEASURE_data/mashed_re.exe | 617d0a324f8a97d7be52f960113f423c5b2e4de7a7143e9043f980a0387ea4f1 |
| build.log (Build OK, rva-lint unchanged) | 9570dd82214f69dde267aaf20a787e2d4974ab4a036921c2e83e0efc4cee5065 |
| gate/passes.tsv (run 2, build mode) | 43f46f69888a801eba4473b25cf3b8a072eb2961b7bda318b74da192077f2e0f |
| gate/02_on_all/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate/02_on_all/arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| gate/02_on_all/arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| gate/02_on_all/arms/B_DEF_K0.step.csv (K1, K26 identical) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| gate/04_on_all2/arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| gate_run2.log | 401abf9bbfb44989381ff1c11088036b7e9d6be88a811f65860140b826a4fba7 |
| probe/probe.json (ORIG_TOP probe of the seed build) | a6daa9a9fd451d5e6d6d1606d3c345d558f25b04118b2f1b1c282415a9785a5a |
| probe_run.log | 73d4f5778c1c799d707a7f0fbdfa4aa3404f7fedba6ffb115d2026dd4f9f66a3 |
| gate_run1_nobuildmode/passes.tsv (void run 1) | 3f619d000c1758b5c80ef82e8fc4a2c67995dd47430c455c1ff4c4b6d256b283 |
