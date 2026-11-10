# RESULT PORT-OT-MEASURE (L-111 (a)): RwGlobals and initialised data for the ORIG_TOP rows, measured per row

Branch `orch/PORT-OT-MEASURE` from orch/integration `3fbf77214`.
- No mashedmod change.
- Tool: `verify/portorigtop_20261109/ot_measure.py` (`lists`, `probe`, `verdict`).
- Output: **`verify/portorigtop_20261109/OT_MEASURE.tsv`**, one line per RVA with these columns:
  `rva, name, subsystem, rwg_verdict, rwg_detail, seed_verdict, seed_detail`
- `origtop_size.py` is the PORT-ORIGTOP-SIZE file, unchanged. It is added here so the row sets regenerate, and it is
  identical to the copy on that branch.

## Result

The rows are the PORT-ORIGTOP-SIZE set runnable once every binding family is bound, recomputed on `3fbf77214`:
- 72 read RwGlobals (`DAT_007d3ff8`)
- 124 read nonzero original .data / .rdata (SIZE counted 123 on `488fa1d9f`)
- 191 distinct rows, 5 of them in both sets

| subsystem | RwGlobals OK | RwGlobals needs binding | seed OK | needs seed rule |
|---|---|---|---|---|
| render | 6 | 22 | 5 | 17 |
| frontend | 0 | 10 | 9 | 13 |
| util | 0 | 2 | 7 | 11 |
| boot | 4 | 1 | 5 | 8 |
| audio | 3 | 1 | 0 | 9 |
| gameplay | 1 | 0 | 4 | 7 |
| hud | 1 | 6 | 0 | 5 |
| RenderWare-Physics-3.7 | 12 | 0 | 0 | 0 |
| particle | 2 | 0 | 2 | 5 |
| vehicle | 1 | 0 | 4 | 3 |
| powerups | 0 | 0 | 1 | 4 |
| save | 0 | 0 | 2 | 0 |
| ai | 0 | 0 | 0 | 1 |
| track | 0 | 0 | 0 | 1 |
| renderware | 0 | 0 | 1 | 0 |
| **total** | **30** | **42** | **40** | **84** |

- 67 of the 191 rows need nothing.
- 124 need a binding or a seed rule, or both.

## RwGlobals (72 rows)

**What the slot holds.** One knob-ON boot (MASHED_ORIG_TOP=1, MASHED_DET_FRAMES=3000) was probed with
ReadProcessMemory at 0.7 / 1.5 / 3.4 / 6.2 / 10.7 / 15.2 / 22.2 / 28.0 s.
- `*(0x007d3ff8)` is 0 for the first ~1.5 to 6 s. From then on it is `0x00ff8290`, the exe's fake RW device, the
  B15 bridge (`exe_main.cpp:9806`, `D3d9Render/RwIm2DBridge.cpp`).
- That table fills only these slots:

  | slot | value |
  |---|---|
  | +0xcc | 0x1b74e0 |
  | +0xd4 | 0x1b74a0 |
  | +0x108 | 0x1b7480 |
  | +0x10c | 0x1b7450 |
  | +0x114 | 0x1b7420 |

- Every other slot the rows use reads 0:
  - +0x0, +0x18, +0x20, +0x24, +0x30, +0x34, +0x60, +0xbc, +0xc4, +0xd0, +0xdc, +0xe8, +0xf4, +0xf8, +0xfc,
    +0x118, +0x11c
- The four module-offset globals the rows add to RwGlobals all read 0, so no RW module is registered in the exe:
  - `0x007d3ffc`
  - `0x007d4028`
  - `0x007d4054`
  - `0x007d4628`

**How each row's offsets were found.** From the original binary, not the source text. The matched sources spell these
uses as `DAT_007d3ff8->member` as often as `(DAT_007d3ff8 + off)`.
- A capstone pass over each function tracks registers loaded from `[0x007d3ff8]`, alone or plus a `[0x007d3ffc..]`
  module offset, and records every `[reg + disp]` use.
- `call` / `jmp` through it is a call slot. Anything else is a data field.
- `pc_funcs.csv` `body_end` is the last instruction's address, so the pass reads a little past it. That catches the
  `jmp [reg+0x10c]` tail calls of 0x004522d0, 0x004898d0 and 0x005adf60.

**Verdict:**
- **OK (30):** RwGlobals is non-null and every slot the row uses is non-null. These rows use only +0xcc / +0xd4 /
  +0x108 / +0x10c / +0x114. That includes the 12 RW-Physics rows (+0x108 / +0x10c).
- **needs binding (42):** a null call slot, a zero data field, or an unregistered module. Rows per missing piece:

| missing piece | rows | example (matched-source spelling) |
|---|---|---|
| call slot +0x20 | 19 | 0x00401f10 `DAT_007d3ff8->setState(0x14, 1)`, 0x00472c60 `->fpRenderStateSet(1, 0)` |
| data field +0x18 | 14 | 0x00472b10..0x004736c0 `->zBufferNear` / `->nearZ` |
| call slot +0x30 | 13 | 0x00472c60 / 0x00473220 draw calls |
| module 0x007d3ffc unregistered | 7 | 0x004c3b30 FastSqrt, 0x004c3bf0 Vec2Length (the RW math LUT reads) |
| call slot +0x11c | 5 | |
| call slot +0xe8 | 4 | |
| modules 0x007d4028 / 0x007d4628 | 2 each | |
| +0x34, +0xf4, +0x118 | 2 each | |
| others | 1 each | +0x0, +0x24, +0x60, +0xbc, +0xc4, +0xd0, +0xdc, +0xf8, +0xfc, module 0x007d4054 |

- 17 of the 42 are blocked only by +0x18 / +0x20 / +0x30, so three entries in the fake device table release them.
- The 12 module-offset rows need the RW module registration, or its offsets, first.
- [UNCERTAIN] Whether 0 at +0x18 (a near-z float the rows read) is wrong: the original's value at that field was not
  read in this step.

## Initialised data (124 rows)

**Static check:** each address must lie inside one of the two seeds:
- `Compat/PortStateSeed.inc` (L-99). It is applied at exe WinMain entry, before the ORIG_TOP fork
  (`exe_main.cpp:10487`).
- `Match/PortTop_RdataSeed.inc`, on the knob path.

**Runtime corroboration:** the same probe read every address and compared it with MASHED.exe's bytes.
- The first sample (0.7 s) precedes the seed: every address reads 0 there.
- From the second sample on, 82 of the 85 seeded references hold their original bytes. The other 3 are the same
  bytes read at a longer size by another row, so the comparison is a prefix.
- That validates the method.

**Verdict:**
- **OK (40):** every nonzero address the row reads is seeded, or the exe holds the original bytes there at some sample
  (1 reference).
- **needs seed rule (84):** at least one address is not seeded and reads 0 in the exe at every sample.
  - 200 references over 180 distinct addresses: 117 in .rdata, 63 in .data.
  - `seed_detail` lists each address with its size, section, the exe bytes and the original bytes. Example: 0x0040e4a0
    reads 0x005f29b8 (4 B, .data), exe `00000000`, original `ff000000`.
  - The fix is mechanical: add the addresses to `verify/portstate_20261108/SEED_CANDIDATES.tsv` and regenerate
    `Compat/PortStateSeed.inc` with `re/tools/gen_portstate_seed.py` (L-99 rule).
  - They could also go to the knob-only `PortTop_RdataSeed.inc`. They are not in either seed today because the L-99
    inventory covers only what default-path bodies read.

## For the step-1 subsystem commits

- Filter `OT_MEASURE.tsv` on `subsystem`.
- A row with `rwg_verdict` empty or OK and `seed_verdict` empty or OK can go in as is.
- A row with `needs seed rule` goes in after its addresses are added to the seed. That is one regeneration per batch.
- A row with `needs binding` waits for the slot or module named in `rwg_detail`.

## Evidence (raw data in `D:/MashedWT/PORT-OT-MEASURE_data`, not committed)

| file | SHA-256 |
|---|---|
| mashed_re.exe (orch/integration 3fbf77214, MASHED_NO_DEPLOY build, lint unchanged) | 617d0a324f8a97d7be52f960113f423c5b2e4de7a7143e9043f980a0387ea4f1 |
| build.log | 9caca5365c6cf8ffbd885308a680b45aada9d6e4135bfa8ecc17db1cfdc023e0 |
| ot_rows.json (the 191 rows, offsets and seed addresses) | a6dc8309f8ceaf08fc4df5b1694d27c9909422c8ed64383920d2c5e43bd87641 |
| probe.json (run 2, module offsets resolved; the verdict input) | ea9b9441dbf2bebf60129ae45e05526b487bc617869301c1549de8ec0894c9a2 |
| probe_run2.log | 4408c3480fab9b51e4cccacef8caa5394e25ac77df4ee609425ef8c7b6a82c54 |
| probe_run1.json (run 1, text-parsed offsets, superseded) | 99c2faaa6a850ab77902f041dcd01b565b4fc847b47af7ac53db3b40a5f61880 |

**Runs:**
- Both boots ran inside `test-slot` and `mashed_lock.py run --label PORT-OT-MEASURE`, root
  `D:/MashedWT/PORT-BATCH-TOOL_root`. The probe only reads memory.
- Run 1 exited 0 after 3000 frames.
- Run 2 started under the memory cap after a 10 min wait. It had not finished its 3000 frames 30 s after the last
  sample, so the probe killed its own PID (rc 1). All 8 samples had been taken.
- A first attempt of run 2 failed in the script before spawning (sort of mixed int / str offsets), so no game ran.
