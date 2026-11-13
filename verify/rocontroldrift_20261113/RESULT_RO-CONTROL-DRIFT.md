# RESULT RO-CONTROL-DRIFT (run 41, 2026-11-13): replay-oracle control drift in block 0x00773c00

Verdict: **GREEN, settled.** The drift is two CRT-startup words in block 0x00773c00, written before any hook or body
runs: the exe path buffer at 0x00773c40 (it holds the private root folder name, which differs per row) and a kernel32
proc pointer at 0x00773d60 (it changed at the 2026-11-12 12:24 reboot). Dword 0x006147d8 is not the cause. It is a
MASK:os toggle that differs between r1 and r2 too. **Hit rows: none.** 0 of the 81 c4=frida-replay rows store into
the block or the dword: 38 are clear on the dynamic leg (69 single-step logs, 1,036,681 decoded stores, 0 target hits)
and 43 on the static leg (every store has a static absolute address). No re-check row is filed and nothing is demoted.
Nothing here holds a c4=frida-replay enqueue. The harness consequence for new runs is in "What this means for the
oracle".

## (1) Which address is right

Both addresses in the PROV-WRITESET-SCENARIO RESULT (`verify/provwritesetscen_20261112/RESULT_PROV-WRITESET-SCENARIO.md:99-103`)
are VAs, and both are reported correctly. They are two different things:

- **0x00773c00 is the differing compare block** (VA; `prov_compare.py` block index j maps to `0x005ea000 + (j-2)*1024`).
  It is a hash-only block: the control's raw ranges (`meta.json` `ticks.raw`) are 0x603400-0x603800, 0x614400-0x614800,
  0x618000-0x618400, 0x6dcc00-0x6dd400, **0x773800-0x773c00**, 0x7dd000-0x7dd400 and 0x7f0c00-0x7f1400. The block
  starts where the raw range ends, so no per-dword copy of it exists in `ticks.bin`.
- **0x006147d8 is not in that block.** `prov_compare.verdict` prints every differing raw word of the divergence tick
  (`raw` list, `re/frida/prov_compare.py:138`) and does not filter it by MASKED. 0x006147d8 is in raw range
  0x614400-0x614800 and is the MASK:os row `re/frida/warp_normalize.tsv:38` (writers 0x00493eeb / 0x0049479a, reader
  0x00494485). The verdict already excludes it. The RESULT took the first printed raw word to be the cause.

The words that do differ, read from the checkpoint dumps (`ckptNN.data.bin` = .data 0x005ea000..0x00915008), control
`wss_ctl_g04` against r1 and r2 (ckpt 00, 01, 07 and 14 checked, the same words in all four):

| VA | control wss_ctl_g04 | r1 | r2 | content |
|---|---|---|---|---|
| 0x00773c50..0x00773c78 | `-WRITESET-SCENARIO_root\original\MASHED.exe` | `-INSITU_root\original\MASHED.exe` | = r1 | tail of the string at 0x00773c40 |
| 0x00773d60 | 0x75b6e8c0 | 0x769fe8c0 | 0x769fe8c0 | kernel32 proc pointer |
| 0x00911d64 | 0x02b8a088 | 0x02de9fb0 | 0x029a9fb0 | differs r1 vs r2 too (allowed block 0x00911c00) |
| 0x006147d8 | 1 at ckpt 00, 0 at 07 / 14 | 1 / 1 / 1 | 0 / 1 / 0 | MASK:os toggle, differs r1 vs r2 too |

The full string at 0x00773c40 is `D:\MashedWT\<ROW>_root\original\MASHED.exe`.

## (3) What sets these words on the unstepped path

Disassembly of `original/MASHED.exe.unpatched` (SHA-256 BDCAE093..., the anchor):

- **0x00773c40**, a 0x104-byte buffer. Function at 0x004abe86: `and byte [0x00773d44], 0` (0x004abe9d), `push 0x104`
  (0x004abea4), `mov esi, 0x00773c40` (0x004abea9), `push esi`, `push edi` (edi = 0), `call [0x005cc1e4]` =
  KERNEL32!GetModuleFileNameA (0x004abeb0), then `mov [0x007739cc], esi` (0x004abebd). It writes the exe's full path, so
  the bytes are the run's private root (`MASHED_ROOT`).
- **0x00773d60**, a dword. Function at 0x004ae29f: `mov eax, [0x00773d60]` (0x004ae2ab). When it is 0 and
  `[0x0077399c] != 1`, it calls `GetModuleHandleA("kernel32.dll")` (string 0x005d5b1c, call 0x004ae2c2), then
  `GetProcAddress(eax, "InitializeCriticalSectionAndSpinCount")` (string 0x005d5af4, call 0x004ae2d2), and stores the
  result: `mov [0x00773d60], eax` (0x004ae2d8). If that fails it stores the fallback 0x004ae28f (0x004ae2e6). The value
  is a kernel32 address and is fixed for one boot session of the machine.
- No other code reference to 0x00773c40..0x00773d43 or 0x00773d60 shows up in a byte scan of the image for the
  imm32/disp32 values 0x00773c00..0x00773ffc (`mov esi, 0x773c40` at 0x004abea9 and the three 0x00773d60 sites above).

**Witness across all dumps** (`CRTWORDS.tsv`, `crtwords.py`): 411 checkpoint-0 dumps under `D:/MashedWT/*_data`.

| 0x00773d60 | dumps | first .. last dump mtime |
|---|---|---|
| 0x7601e8c0 | 19 | 2026-11-01 15:43 .. 2026-11-01 23:24 |
| 0x769fe8c0 | 279 | 2026-11-11 05:08 .. 2026-11-12 11:51 |
| 0x75b6e8c0 | 113 | 2026-11-12 12:45 .. 2026-11-12 18:11 |

The machine's last boot time is 2026-11-12 12:24:57 (`Win32_OperatingSystem.LastBootUpTime`). The value changes
between the last dump before that boot and the first dump after it. 0x00773c40 has one value per private root (13 roots
seen). 0x006147d8 is 1 in 207 dumps and 0 in 204.

**Pair test** (`menu_replay_pairs.py`, `MENU_REPLAY_PAIRS.txt`): RO-MENU-REPLAY ran in one root on both sides of the
reboot (37 dumps). Over all 666 pairs of its checkpoint-0 dumps, block 0x00773c00 is equal in all 354 pairs with the
same 0x00773d60 and differs in all 312 pairs with different 0x00773d60, whatever 0x006147d8 holds:

| 0x00773d60 | 0x006147d8 | block 0x00773c00 | pairs |
|---|---|---|---|
| same | differs | equal | 177 |
| same | same | equal | 177 |
| differs | differs | differs | 153 |
| differs | same | differs | 159 |

This also refutes the attribution in the BOOT-CLASS comment of `re/frida/prov_compare.py:157-160` (RO-MENU-REPLAY):
"0x006147d8 ... runs with 1 against references with 0 differ in 0x00773c00 / 0x00911c00 from tick 0". The 0x00773c00
difference follows the boot session, not 0x006147d8. 0x00911c00 is already an allowed block (r1 against r2).

## (2) Store lists of the 81 c4=frida-replay rows against the block and the dword

`hitscan.py`, output `HITSCAN.tsv` (one row per RVA) and `HITSCAN_RUNS.tsv` (one row per log). Targets:
[0x00773c00, 0x00774000) and [0x006147d8, 0x006147dc). A store hits when [ea, ea+size) overlaps a target.

- The 81 rows are every `hooks.csv` row whose notes carry `c4=frida-replay`: 43 from PROV-PROMOTE-44, 30 from
  PROV-WRITESET-PROMOTE and the 8 of `re/promotions/applied/PROV-WRITESET-SCENARIO.tsv`. All 81 have a DDK body with
  status OK in `verify/provinsitu_20261111/bodies_all.json`.
- **Dynamic leg:** every `wlog.bin` under `D:/MashedWT/PROV-WRITESET-114_data` and `D:/MashedWT/PROV-WRITESET-SCENARIO_data`
  (69 logs). Each one is re-decoded the way `re/tools/prov/prov_writeset.py` does it (`blob_bytes` relink, effective
  address from the logged registers, `rep movs/stos` as a range). The result is 1,036,681 stores and 0 target hits.
  38 rows have stepped calls in these logs, and all 38 are `clear-dynamic`. They include all 8 PROV-WRITESET-SCENARIO
  rows.
- **Static leg:** the body's absolute store list (`writes.abs_in` / `abs_out`). No absolute store of any of the 81 lands
  in a target. The other 43 rows have no stepped call in these logs (the PROV-PROMOTE-44 `empty` write-set rows and the
  4 `covered` ones). Each of them has `indirect 0` and `indexed_abs 0`, so every non-frame store has a static absolute
  address, and none is in a target: `clear-static`.

| verdict | rows |
|---|---|
| HIT | 0 |
| clear-dynamic | 38 |
| clear-static | 43 |

No re-check row is filed (no hit).

## What this means for the oracle

- **Rows already promoted are not affected.** The evidence for the 8 SCENARIO rows compares stepped runs against an
  unstepped control from the same root and the same boot (IDENTICAL-FOR-2724). Their IDENTICAL-FOR-2724 leg is
  PROV-INSITU's, made in PROV-INSITU_root on the 11-11 boot, the same root and boot as r1 / r2. The 73 earlier rows
  rest on PROV-INSITU runs in that same root and boot. The block's other dwords stay compared: they are equal in every
  pair above.
- **Runs from now on.** A run made in a different private root, or after a reboot, differs from r1 / r2 in block
  0x00773c00 from tick 0. `prov_compare.py` then reports DIVERGED at tick 0. That is a false RED, never a false GREEN.
  The fix belongs to the harness, not to this row. Two options:
  1. Add 0x00773c00-0x00774000 to the `--tick-raw` ranges and add MASK rows for 0x00773c40 (0x104 bytes) and
     0x00773d60 to `re/frida/warp_normalize.tsv`. Masking alone is not enough: a MASK dword inside a hash-only block
     drops the whole block from the compare (`prov_compare.py:133`).
  2. Re-record the references on the current boot in the root the new run uses.

  Next to this, the BOOT-CLASS check should key on 0x00773d60 rather than 0x006147d8.
- **Allowed-block check.** The references r1, r2, x_ref and x_ref2 (the pairs that define the allowed blocks) were all
  made in PROV-INSITU_root on the 11-11 boot (0x00773d60 = 0x769fe8c0 in all four, `CRTWORDS.tsv`), so 0x00773c00 cannot
  enter an allowed set built from them. In the 251 verdict JSON files of PROV-INSITU, PROV-WRITESET-SCENARIO and the
  provinsitu / provwsprom / provwritesetscen verify folders, 0x00773c00 appears in 2: `wss_ctl_g04.verdict.json` and
  `wss_g04.verdict.json`. In both it is a divergence block, never an allowed block. No verdict behind the 81 rows left
  the block unscored.

## Not run

No build, no object build, no game run (text + replay-data row: every input is an existing dump or log). G-CTRL / B:
not run, row kind (no default-path or port change). No file under `mashedmod/` or `re/frida/` changed.

## Evidence

Committed (this folder): `hitscan.py`, `HITSCAN.tsv`, `HITSCAN_RUNS.tsv`, `crtwords.py`, `CRTWORDS.tsv`,
`menu_replay_pairs.py`, `MENU_REPLAY_PAIRS.txt`, `RAW_SHA256.tsv`.
Raw (not committed): `D:/MashedWT/RO-CONTROL-DRIFT_data/hitscan.log`. The dumps it reads are the existing
`D:/MashedWT/PROV-INSITU_data/{r1,r2}`, `D:/MashedWT/PROV-WRITESET-SCENARIO_data/wss_ctl_g04` and the 69 `wlog.bin`
listed in `HITSCAN_RUNS.tsv`. Their SHA-256 values are in this folder's `RAW_SHA256.tsv` and in the source rows'
`RAW_SHA256.tsv`.
