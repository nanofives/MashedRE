# RESULT RO-PIZFS-MASK-B (run 47, 2026-11-14)

L-157 (b). Mask `0x007d3e4c..0x007d3e6b` and the piz directory `0x0090dac0..0x00911abf` as bound family `pizfs` in
`re/frida/replay_masks_manual.tsv`, with the reader evidence for each group cited below. Text row: no build, no gates,
no game run.

**Verdict: GREEN (text row).** Two `MASK:pizfs` rows added. Both carry `[UNCERTAIN]` for watch coverage, and the
test for that is its own open row, **RO-PIZFS-GUARD-SAMPLING** (added to the ledger with `roadmap.ps1 add`).
The slots `0x008cd9c0..0x0090daaf` stay unmasked. L-152's six counter rows are not released by this row.

## 1. Change

`re/frida/replay_masks_manual.tsv`:
- header comment: the `pizfs` family note now names the two masked groups and keeps the slots out, citing the
  three RW FS readers outside the layer;
- new row `0x007d3e4c 1 0 32 MASK:pizfs` (8 dwords);
- new row `0x0090dac0 1 0 16384 MASK:pizfs` (0x4000 B = 4096 dwords, 0x80-byte entries).

`re/frida/replay_masks.tsv`: the same two rows appended to section 2 (manual rows). The file was **not** fully
regenerated. Section 7 explains why.

## 2. Reader evidence, group `0x007d3e4c..0x007d3e6b` (8 dwords)

Primary: `verify/pizfs_readers_20261114/RESULT_RO-PIZFS-READERS-RUN.md` section 5 and section 3.1 (merged on
orch/integration, commit `247ec907e`). Hardware read/write watchpoints, runs `g1hw_a` (`0x007d3e4c,50,54,58`) and
`g1hw_b` (`0x007d3e5c,60,64,68`), boot through race on track 0.
- All 25 accessing instructions are in `0x004b6610..0x004b6dae`. None are outside `0x004b6xxx`.
- Writers: `0x004b6618` `0x004b661d` (FUN_004b6610), `0x004b66ad` `0x004b66bb` (FUN_004b6640), `0x004b67ab`
  `0x004b67bd` (FUN_004b67a0), `0x004b69a8` `0x004b69ae` (FUN_004b6940), `0x004b6a14` `0x004b6acb`, `0x004b6b5e`
  `0x004b6b78` (FUN_004b6b20).
- Readers: `0x004b6710` `0x004b6770` `0x004b67a5` `0x004b67e0` `0x004b6875` `0x004b687e` `0x004b68e0` `0x004b69ca`
  `0x004b69d3` `0x004b6ad0` `0x004b6d95` `0x004b6da0` `0x004b6da8`.
- No access in the `menu` or `race` windows.

Static, `verify/pizfs_readers_20261114/RESULT_RO-PIZFS-UNMASKED-READERS.md` R1: the one value that leaves the layer
is `0x007d3e4c`, through thunk `0x004b65a0` (`jmp 0x4b68e0`). At all 3 call sites (`0x00411f6e`, `0x004587ff`,
`0x0048ff49`) the next instructions are `push imm32; call 0x4b65b0`, so eax is overwritten before any use. The run agrees:
the `0x004b68e0` stack returns are `0x00411f73`, `0x00458804`, `0x0048ff4e` (= those sites + 5), 3 hits, boot window.

## 3. Reader evidence, directory `0x0090dac0..0x00911abf`

Primary: RESULT_RO-PIZFS-READERS-RUN sections 5 and 3.3 (guard pages, 25 ms cycles, runs `g3_dir_r2` track 0 and
`g3_dir_t3` track 3, both reached the race).
- Accessors inside `0x004b6xxx`: `0x004b6a60` (`rep stosd`) and `0x004b6ab0` (`rep movsd`) in FUN_004b6940, `0x004b6b59`
  (`[edx+0x74]`, FUN_004b6b20), and `0x004b6cfa` (`[ecx+0x78]`).
- The one reader outside is `0x004b2ec9` (`mov ah,[edi]`, `___ascii_stricmp` at `0x004b2eb0`, CRT). It is reached
  through `__stricmp` (`0x004b302f`, return `0x004b305c`) from `0x004b6bcb` (FUN_004b6ba0), `0x004b6c68` (FUN_004b6c00)
  and `0x004b690e` (FUN_004b68f0). It compares entry names for the layer's own lookups.

Static, RESULT_RO-PIZFS-UNMASKED-READERS:
- **R2**: `0x004b692b mov eax,[esi+0x78]` (FUN_004b68f0, reached only through thunk `0x004b65b0`) is consumed by
  `0x00411f78`, `0x00458809`, `0x0048ff53`, which pass it to `0x004b3b70`. In the ORIG_TOP exe, `0x004b68f0` is bound to
  `PortTop_PizFs_EntryLength_004b68f0` (`mashedmod/src/mashed_re/Match/PortTop_00402750_Pieces.cpp:200`), which answers from
  the PIZ_FS mount stack. The consumers get the value without the directory. Whether the bound function returns the
  same length is a value check for the oracle, not a reason to keep the directory unmasked (static RESULT section 4).
  The run did not observe `0x004b692b` (RESULT_RO-PIZFS-READERS-RUN 3.3: its own `__stricmp` reaches the page first).
- **R3**: the entry pointer escapes only into slot `+0x20064` (`0x004b6c8f`, FUN_004b6c00), and every reader of
  `+0x20064` is in `0x004b6xxx`.

## 4. Not masked by this row

- Slots `0x008cd9c0..0x0090daaf`: the run observed readers outside the layer. These are RW FS code: `0x005505aa`
  (FUN_00550580, slot `+0x20`), `0x00550960` (FUN_00550950, slot `+0x38`) and `0x00550c07` (FUN_00550be0, slot
  `+0x20`, every frame in menu and race) (RESULT_RO-PIZFS-READERS-RUN 3.2). They stay unmasked.
- L-152's six counter rows: not touched and not released. They wait on RO-ORACLE-GATE's FUN_004b65c0 buffer answer.

## 5. [UNCERTAIN] and its open row

Each mask row's note carries `[UNCERTAIN]`:
- Directory: guard-page sampling records only the first access to each page per 25 ms cycle. A reader that is never
  first on its page in a cycle is missed. The static read `0x004b692b` is one such site. The run lists pcs that do
  access. It does not prove that no other pc does (RESULT_RO-PIZFS-READERS-RUN section 4).
- 8 dwords: the hw watch is exact for threads alive at an arm, but the watchpoints are re-armed every 1 s after attach. A
  thread that touches these dwords within 1 s of creation, or before the attach, is missed. The run is also limited to
  tracks 0 and 3 with a warp launch, and no frontend menu walk was run.

Test row: **RO-PIZFS-GUARD-SAMPLING** (section HB, open). It asks for an exhaustive watch started at spawn, over
boot, a menu walk, a track load and a race on 2 or more tracks. The watch must also catch `0x004b692b`. Any new outside
reader that feeds game state reopens the mask.

## 6. Checks run

- `py -3.12 re/tools/gen_replay_masks.py --facts ../../log/ghidra_funcfacts.jsonl --shared <scratch>` (the repo's
  generator for this table): ran OK, output
  `masked dwords by family: {'audio': 127, 'directshow': 2, 'crt': 143, 'lua': 6, 'renderware': 266, 'd3dx9': 179,
  'qhull': 838} spans 389 shared (bound + game code, not masked): 331`. It appended both new rows unchanged.
- Loader check (scratch script, `re/frida/detorig.py` `load_norm` over `warp_normalize.tsv,replay_masks.tsv`, the
  same call as the compare):
  - `0x007d3e4c..0x007d3e6b`: 8 of 8 dwords `MASK:pizfs`;
  - directory: 4096 of 4096 dwords `MASK:pizfs`;
  - slots `0x008cd9c0..0x0090daaf`: 0 of 65596 masked;
  - edges `0x007d3e48`, `0x007d3e6c`, `0x0090dab0..0x0090dabf`, `0x00911ac0`: unmasked;
  - total masked dwords 40402 -> 44506 (+4104 = 8 + 4096), PIN rows 299 unchanged;
  - no overlap with any existing mask row.
- No build, no game run, no gates (text row).

## 7. Generator drift (not taken, for its own row)

The regeneration also added a bound-family row **`0x00914700 MASK:audio`** (1561 dwords / 389 spans against 1560 / 388
in the committed file). That comes from inputs that changed after RO-MASKS (hooks.csv or the facts file), not from this
row. A new mask merges only with its own reader evidence, and none is cited here. So `replay_masks.tsv` keeps the
committed section 1 plus the two new manual rows. Whoever next regenerates the file will pick up that audio dword. It
should be reviewed there.

## Files

Inputs (committed on orch/integration):
- `verify/pizfs_readers_20261114/RESULT_RO-PIZFS-READERS-RUN.md`
- `verify/pizfs_readers_20261114/RESULT_RO-PIZFS-UNMASKED-READERS.md`
- the run tables `run_g1_007d3e4c.tsv` and `run_g3_directory.tsv` in that folder

Raw run data (not copied), `D:/MashedWT/RO-PIZFS-READERS-RUN_data`, SHA-256 as listed in RESULT_RO-PIZFS-READERS-RUN:

| file | SHA-256 |
|---|---|
| g1hw_a.tsv | c9961fcb9c70703f6dfe2ec45ab40279a37c5d6fb59affddc96c87093f6e0994 |
| g1hw_b.tsv | 3d8ffdfee7932ec1b592a848ba2fc771a092ef094c3a5f49f79ba75414ab093d |
| g3_dir_r2.tsv | a64253ea86da6d71e07c9012f7fdccedf1ebb8f8a70ab2cdfaeacb2b3d20e5d9 |
| g3_dir_t3.tsv | d71842fcb9906b2fa8cadbd05213dc3e24c999b5843fe827f2c350384c9798ca |

Generator inputs:

| file | SHA-256 |
|---|---|
| `log/ghidra_funcfacts.jsonl` (main checkout) | 8ee4c0d8c032565325553332556abb2358bb06ec384f75900382fa135377f057 |
| `original/MASHED.exe.unpatched` | bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e |
