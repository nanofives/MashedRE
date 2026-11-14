# RESULT RO-PIZFS-READERS-RUN (run 46, 2026-11-14)

Run test for RO-PIZFS-UNMASKED-READERS (`RESULT_RO-PIZFS-UNMASKED-READERS.md`, static). Question: which pcs read or
write `0x007d3e4c..0x007d3e6b`, the slots `0x008cd9c0..0x0090daaf` and the directory `0x0090dac0..0x00911abf` on the
ORIGINAL over a boot plus a track load. Evidence for the [UNCERTAIN] masks of RO-PIZFS-MASK-B and L-157's slot
question. **No mask applied**, `re/frida/replay_masks_manual.tsv` untouched. No build, no gates.

**Verdict: GREEN (evidence row).** Every run-observed accessor of the three groups sits in `0x004b6xxx`, except four
pcs. The four outside pcs are: `0x004b2ec9` (CRT `___ascii_stricmp`, reading directory entry names, called through
`__stricmp` from `0x004b6xxx`), and three RW file-system reads of slot header fields: `0x005505aa` (slot `+0x20`),
`0x00550960` (slot `+0x38`) and `0x00550c07` (slot `+0x20`, every frame in menu and race). The static RESULT's R4
route is confirmed by the run, and `0x00550960` is one of its [UNCERTAIN] `[reg+0x38]` readers, now observed reading
slot 0 `+0x38`. The `0x004b2ec9` reader is new relative to the static RESULT. Group 1 is exact (hardware watch).
Groups 2 and 3 are sampled (guard pages) and carry [UNCERTAIN] for missed readers (section 4).

## 1. Method

- Binary: the private root `D:\MashedWT\RO-PIZFS-READERS-RUN_root\original\MASHED.exe` (patched boot recipe, SHA-256
  `b9977dab...fe87`, robocopy of `original\`). Disassembly for the tables: `original\MASHED.exe.unpatched`
  (`BDCAE093...3C0E`, the anchor). Nothing written to either.
- Driver: `re/frida/scenario_launch.py` (Popen + attach, warp launch `DAT_00771968 = 2`, `--hold 10`, mode 10, 1 car),
  new option `--access-watch` loading `re/frida/js/access_watch.js`. No Interceptor, no code patch, no memory write.
  Windows: `boot` (attach to phase 1), `menu` (phase 1, about 0.5 s), `load` (launch poke to phase 3), `race`
  (phase 3, 10 s). Every launch: `test-slot` outside, `mashed_lock.py run --label RO-PIZFS-READERS-RUN` inside
  (`run_readers.sh`). Hub: offline, all runs local. MASHED_ROOT = the private root.
- Two watch modes:
  - **hw** (group 1 and four sampled buffer dwords): debug-register read/write watchpoints, 4 bytes, up to 4 per run,
    on every thread, re-armed every 1 s and at each window change. Every access on an armed thread traps. The trap pc
    is the next instruction; `aggregate_run.py` resolves the accessing instruction by a linear sweep from the
    containing function's start (inventory) that ends exactly at that pc.
  - **guard** (groups 2 and 3): `MemoryAccessMonitor` on the pages covering the range, re-armed every 25 ms. Each
    cycle records only the FIRST access to each page, filtered to the range. pc = the faulting instruction.
- Stack column: the first 3 dwords on the stack that fall in image `.text`. These are candidate return addresses.
  Stale ones are possible (for example `0x0041b000`, `0x0049f800`).

## 2. Runs

| run | target | mode | track | result |
|---|---|---|---|---|
| g1_run1 | `0x007d3e4c..0x007d3e6b` | guard 25 ms | 0 | race reached. 2 in-range of 666 callbacks: the page `0x007d3000` is dominated by neighbouring globals. Superseded by g1hw_a/b |
| g1hw_a | `0x007d3e4c,50,54,58` | hw | 0 | race reached |
| g1hw_b | `0x007d3e5c,60,64,68` | hw | 0 | race reached |
| g2_slots | `0x008cd9c0..0x0090daaf` (65 pages) | guard 25 ms | 0 | **TIMEOUT before the menu** (phase 0 for 40 s). Boot-window data only. [UNCERTAIN] cause not established: no crash dump was written |
| g2_hdr | slot headers + tails: `0x008cd9c0..0x008cda23`, `0x008eda24..0x008eda9b`, `0x0090da9c..0x0090daaf` | guard 25 ms | 0 | race reached |
| g2_hdr_t3 | same | guard 25 ms | 3 | race reached |
| g2_bufhw_t3 | buffer dwords `0x008cda24`, `0x008dda24` (slot 0 `+0x64`, `+0x10064`), `0x008eda9c`, `0x008fda9c` (slot 1 same) | hw | 3 | race reached |
| g3_dir | `0x0090dac0..0x00911abf` | guard 10 ms | 0 | **game exited before the menu**, no data (drain after exit). [UNCERTAIN] cause: no crash dump found. Not a finding |
| g3_dir_r2 | same | guard 25 ms | 0 | race reached |
| g3_dir_t3 | same | guard 25 ms | 3 | race reached |

Slot layout seen in g2_slots (offsets from `0x008cd9c0 + s*0x20078`): `0x004b6c9d rep stosd` and `0x004b6b88 /
0x004b6e68 rep movsd` touch `+0x64..+0x1f6a4` (the buffer), headers below `+0x64`, tail `+0x20064..`. That is why
g2_hdr watches only the header and tail ranges.

## 3. Readers by group (counts summed over the listed runs)

Full tables with addresses and stacks: `run_g1_007d3e4c.tsv`, `run_g2_slots.tsv`, `run_g3_directory.tsv`
(`aggregate_run.py` over the raw TSVs).

### 3.1 `0x007d3e4c..0x007d3e6b` (exact, hw, g1hw_a + g1hw_b)

All 25 accessing instructions are in `0x004b6610..0x004b6dae`. None outside `0x004b6xxx`.

| pc | instruction | count | windows |
|---|---|---|---|
| `0x004b6618`, `0x004b661d` | `mov [0x7d3e5c],eax`, `mov [0x7d3e60],ecx` (FUN_004b6610) | 1 each | boot |
| `0x004b66ad`, `0x004b66bb` | `mov [0x7d3e50],eax`, `mov [0x7d3e54],edx` (FUN_004b6640) | 1 each | boot |
| `0x004b6710`, `0x004b6770` | `test byte [0x7d3e50],1` | 12 each | boot/load |
| `0x004b67a5`, `0x004b67ab`, `0x004b67bd` | read `[0x7d3e58]`, write `[0x7d3e4c]`=0, write `[0x7d3e58]`=0 (FUN_004b67a0) | 12 each | boot/load |
| `0x004b67e0` | `mov al,[0x7d3e50]` (FUN_004b67e0) | 190 | boot/load |
| `0x004b6875`, `0x004b687e` | read `[0x7d3e5c]`, `[0x7d3e60]` (FUN_004b67e0) | 195 each | boot/load |
| `0x004b68e0` | `mov eax,[0x7d3e4c]` (FUN_004b68e0); stack returns `0x00411f73`, `0x00458804`, `0x0048ff4e` | 3 | boot |
| `0x004b69a8`, `0x004b69ae` | write `[0x7d3e64]`, `[0x7d3e68]` (FUN_004b6940) | 11 each | boot/load |
| `0x004b69ca`, `0x004b69d3` | read `[0x7d3e5c]`, `[0x7d3e60]` | 12 each | boot/load |
| `0x004b6a14`, `0x004b6acb`, `0x004b6ad0` | write `[0x7d3e4c]`=1, write `[0x7d3e58]`, read `[0x7d3e54]` | 12 each | boot/load |
| `0x004b6b5e`, `0x004b6b78` | write `[0x7d3e64]`, `[0x7d3e68]` (FUN_004b6b20) | 153 each | boot/load |
| `0x004b6d95`, `0x004b6da0`, `0x004b6da8` | `cmp ecx,[0x7d3e64]`, `cmp ecx,[0x7d3e68]`, `sub eax,[0x7d3e64]` | 13026, 12987, 12880 | boot/load |

The `0x004b68e0` stack returns are the three thunk call sites the static R1 lists (`0x00411f6e`, `0x004587ff`,
`0x0048ff49` + 5). No access in the `race` or `menu` windows.

### 3.2 Slots `0x008cd9c0..0x0090daaf` (sampled: g2_hdr, g2_hdr_t3, g2_bufhw_t3, g2_slots boot)

Outside `0x004b6xxx`, all reads:

| pc | instruction | function | field (address) | count | windows | stack (top) |
|---|---|---|---|---|---|---|
| `0x005505aa` | `mov ecx,[eax+0x20]` | FUN_00550580 | slot 0 `+0x20` (`0x008cd9e0`) | 26 | boot/load | `0x005508fa|0x004ac660|0x004a45c2` x24 |
| `0x00550960` | `mov eax,[ecx+0x38]` | FUN_00550950 | slot 0 `+0x38` (`0x008cd9f8`) | 4 | boot/load | `0x004cbd67|0x00550970|0x004cc414` |
| `0x00550c07` | `cmp [eax+0x20],ebx` | FUN_00550be0 | slot 0 and 1 `+0x20` (`0x008cd9e0`, `0x008eda58`) | 1802 | boot/load/menu/race | `0x0045d40f|0x0049270a|0x004922c2` x1802 |

Inside `0x004b6xxx`: `0x004b65e2` / `0x004b65f1` (write `+0x20068`), `0x004b66c3` / `0x004b66d3` (write `+0x38`),
`0x004b6b40` / `0x004b6cf4` (read `+0x20064`), `0x004b6c8f` (write `+0x20064`), and the buffer copies
`0x004b6b88`, `0x004b6d4b`, `0x004b6e68` (`rep movsd`, read) and `0x004b6c9d` (`rep stosd`, write). The four
buffer dwords under hw were touched only by these four rep instructions (g2_bufhw_t3, 731 hits).

`0x00550c07` runs in every window including `race`, so FUN_00550be0 reads slot `+0x20` each frame on the original.
The static RESULT's route R4 (slot pointer from `[table+0x14]` = `0x004b6630`) is the one consistent with that.
[UNCERTAIN] the run does not show how FUN_00550be0 got the slot pointer, only that it reads it.

Not observed although the static RESULT lists them: the writes `0x0055138a` / `0x00551397` (slot `+0x20` / `+0x18`,
FUN_00551330) and `0x005505bf` (slot `+0x20` = 5). [UNCERTAIN] sampling: FUN_00551330 runs right after
`0x004b66c3` / `0x004b66d3` wrote the same page, so a single cycle catches only the first.

### 3.3 Directory `0x0090dac0..0x00911abf` (sampled: g3_dir_r2, g3_dir_t3)

| pc | instruction | function | outside | count | addresses | stack (top) |
|---|---|---|---|---|---|---|
| `0x004b2ec9` | `mov ah,[edi]` | `___ascii_stricmp` (`0x004b2eb0`, inventory) | **yes** | 139 | entry `+0x00`: `0x0090dac0`, `0x0090e040`, `0x0090f040` | `0x004b305c|0x004b6bcb|0x00550bb0` x115; `0x004b305c|0x004b6c68|0x00550637` x20; `0x004b305c|0x004b690e|0x0048ff58` x2 |
| `0x004b6a60` | `rep stosd` (FUN_004b6940) | no | 101 | all 5 pages | |
| `0x004b6ab0` | `rep movsd` write (FUN_004b6940) | no | 57 | | |
| `0x004b6b59` | `mov ecx,[edx+0x74]` (FUN_004b6b20) | no | 195 | entry `+0x74`, 35 entries | `0x004b6e3e|0x00550970|0x004cbd67` |
| `0x004b6cfa` | `mov eax,[ecx+0x78]` | no | 6 | entry `+0x78` | `0x00550970|0x004cbd67|...` |

`0x004b305c` is inside `__stricmp` (`0x004b302f`, inventory), and the next returns are `0x004b6bcb` (FUN_004b6ba0),
`0x004b6c68` (FUN_004b6c00) and `0x004b690e` (FUN_004b68f0). So the outside reader is CRT code called from the
`0x004b6xxx` lookups, comparing entry names. The static sweep could not see it: it reaches the directory through
an argument pointer.

The read `0x004b692b mov eax,[esi+0x78]` (static R2, FUN_004b68f0) was not observed. [UNCERTAIN] sampling: its own
`__stricmp` call reads the same entry's page first (stack row `0x004b690e` above).

## 4. Coverage and [UNCERTAIN]

- Group 1 is exact for threads alive at an arm (re-armed every 1 s). A thread that starts and touches these dwords
  within 1 s of creation would be missed. The attach comes after `Popen`, so accesses before attach are missed too.
  The first write by FUN_004b6640 (`0x004b66ad`) was caught, so the pizfs init ran after the attach.
- Groups 2 and 3 are sampled: at most one access per page per 25 ms cycle. A reader that never comes first on its
  page within a cycle is missed. Two static sites (`0x00551397` / `0x0055138a`, `0x004b692b`) are not in the run
  data for that reason or another one. The run lists pcs that do access. It does not prove no other pc does.
- The slot buffer interior (`+0x64..+0x20063`) was watched by guard pages only in g2_slots (boot window, before the
  stall) and by hw on 4 dwords. Readers of buffer bytes beyond those 4 dwords are covered only by the boot window.
- The g2_slots stall and the g3_dir exit happened under guard-page monitoring before the menu. Neither left a dump.
  The runs that reached the race are the evidence. The two failures are harness effects, not game findings
  [UNCERTAIN].
- Window `menu` is about 0.5 s, `race` 10 s on tracks 0 and 3 only. A frontend menu walk was not run. The warp launch
  skips the frontend selection path.

## 5. For RO-PIZFS-MASK-B / L-157

- `0x007d3e4c..0x007d3e6b`: run confirms the static claim. Every accessor is in `0x004b6xxx`. The `0x004b68e0` value
  goes to the 3 call sites R1 shows discarding it.
- Slots: the readers outside are RW FS code (`0x005505aa`, `0x00550960`, `0x00550c07`) on header fields `+0x20` and
  `+0x38`. `0x00550c07` reads every frame in the race. If the ORIG_TOP exe never registers the 0x58 table
  (FUN_004b6640 replaced by PIZ_FS), these RW walks do not reach the slots there. That makes the slot state a
  pizfs-bound difference, provided the decision also accepts the RW FS manager list difference (static RESULT
  section 4).
- Directory: the one reader outside is CRT `___ascii_stricmp` called from the `0x004b6xxx` lookups. That supports the
  static proposal (mask as bound family `pizfs`).

## Files

Committed (this folder): `RESULT_RO-PIZFS-READERS-RUN.md`, `aggregate_run.py`, `run_readers.sh`,
`run_g1_007d3e4c.tsv`, `run_g2_slots.tsv`, `run_g3_directory.tsv`. Tooling: `re/frida/js/access_watch.js`,
`re/frida/scenario_launch.py` (`--access-watch`, `--access-watch-period`, `--access-watch-out`).

Rebuild the tables: `py -3.12 verify/pizfs_readers_20261114/aggregate_run.py ../../original/MASHED.exe.unpatched
re/analysis/function_inventory.csv D:/MashedWT/RO-PIZFS-READERS-RUN_data <out.tsv> <run>[,<run>..]`.

Raw data, `D:/MashedWT/RO-PIZFS-READERS-RUN_data` (SHA-256):

| file | SHA-256 |
|---|---|
| g1_run1.tsv | d01ca0003f8f16c33007393ad7cdf52ebcf85c1ba9212fd0622d696a59ebafdb |
| g1hw_a.tsv | c9961fcb9c70703f6dfe2ec45ab40279a37c5d6fb59affddc96c87093f6e0994 |
| g1hw_b.tsv | 3d8ffdfee7932ec1b592a848ba2fc771a092ef094c3a5f49f79ba75414ab093d |
| g2_slots.tsv | 9c81e49c8175e0e6184ff682d8f2c0d0e9d7c62ee5b177699f06b3f37f5c9780 |
| g2_slots.tsv.stream.tsv | 9e2e930434f7de880b6be08b9b0fa53a82108dceeb507d3cef505816cce3958a |
| g2_hdr.tsv | a815800cd9565464cb803cbd1c33886290eeb85886319abe02cd87c91d79e41e |
| g2_hdr_t3.tsv | 83315d3775de4c445179c81ac75ca4daf1ae79836353ba79f1d59688efcd2daf |
| g2_bufhw_t3.tsv | c7fc9d43798586ed9569d745bf40dded5d55d5d0913cffd9498dc5cce2e8ae60 |
| g3_dir_r2.tsv | a64253ea86da6d71e07c9012f7fdccedf1ebb8f8a70ab2cdfaeacb2b3d20e5d9 |
| g3_dir_t3.tsv | d71842fcb9906b2fa8cadbd05213dc3e24c999b5843fe827f2c350384c9798ca |
| g1_run1.log | 6631ae6f8b27bd21cd3748cb0c25a1e7535355626cd5dfd7a4327d9602328bd9 |
| g1hw_a.log | ef837aaaa655ce020d6937781da6cf0fba4bdbad121704b9d4db7e44ba0da2a3 |
| g1hw_b.log | 72c91b9b00e8871bdf5308246da472da0b22a8b5dd6608bb884d9c37cfb55eef |
| g2_slots.log | a24a9862b383678011ea70ae720254370a7a650b71e6a842795f55d690e9eb4f |
| g2_hdr.log | 3fb85bb58e5c10a8c3deb5a62f78e30c3169d9cde96ffd1afc643dbc84e89f26 |
| g2_hdr_t3.log | b0a98bc379135b2fbc9fa03fd194fe3a24bff6d49b62697be363058f2cd44f03 |
| g2_bufhw_t3.log | 5a30ee42a4f34a8c1561d9418ba3b2f08f0b63ce2839adfac96b121ffe2f8bc2 |
| g3_dir.log | d80d544a187411fc2eae79c313a5be221518666edcd0ddbddad9fbac290ad2de |
| g3_dir_r2.log | 39a83b552e4f6d66d6e79264b7a237cc4dbef37f4165c39295c13953da7b0982 |
| g3_dir_t3.log | e8ee61e674305df6e1292919dbae04a56e9810a1d8dca0c6bbe8952f6dd41ca4 |

The `.first.tsv` and other `.stream.tsv` files in the same folder are per-run side outputs (first window per pc, and
the in-process stream of new keys). They hold no row the TSVs above lack.
