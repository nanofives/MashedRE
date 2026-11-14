# RESULT RO-PIZFS-UNMASKED-READERS (run 46, 2026-11-14)

Follow-up to L-152 (a). Question: which code outside `0x004b6xxx` reads or writes the addresses L-152 (a) left
unmasked: the slots `0x008cd9c0` / `0x008eda38` (0x20078 B each, to `0x0090dab0`), the directory `0x0090dac0`, and
`0x007d3e4c..0x007d3e68`. Text/static only. No build, no game run, no Ghidra MCP.

**Verdict: GREEN (static).** No instruction outside `0x004b6xxx` takes any of these addresses as an absolute operand.
Code outside `0x004b6xxx` reaches them by four indirect routes (section 3). Each route goes through a function in
`0x004b65xx..0x004b6fxx` or through the 0x58-byte callback table built in FUN_004b6640. A mask is proposed for all
three groups (section 4). It is a proposal for the L-152 decision row, not applied here.

## 1. Method

Binary: `original\MASHED.exe.unpatched`, SHA-256 `BDCAE093...3C0E` (the anchor). The same sweep on the patched
`original\MASHED.exe` (SHA-256 `b9977dab...fe87`) gave an identical in-range CODE hit list (`diff` empty). Both files
were read only.

1. **Absolute-operand scan** (`sweep.py`). Every byte offset of every section is read as a little-endian dword. A
   dword in `[0x008cd9c0, 0x00911ac0)` or `[0x007d3e4c, 0x007d3e6c)` is a hit. In `.text` every start `hit-12..hit-1`
   is decoded with capstone 5.0.7. A decode is kept when the instruction covers the 4 hit bytes at its `disp_offset`
   or `imm_offset`. Range end: the directory is `0x0090dac0..0x00911abf` (`0x004b6a45 mov ecx,0x1000` +
   `0x004b6a60 rep stosd` = 0x4000 B, entries 0x80 B at `0x004b691b` / `0x004b6bd8`, count dword `0x008ad9a8`).
2. **Byte anchoring** (`cover.py`). Each kept decode outside `0x004b6xxx` is checked against a linear sweep. The sweep
   starts at the containing function's start in `re/analysis/function_inventory.csv`. Outside a listed body, it
   starts at the first byte after the previous body that is not `0x90`/`0xCC` padding. A hit is CONFIRMED only when
   the sweep decodes an instruction at exactly that address.
3. **Register-relative fields.** The slot fields `+0x20064..+0x20077` are reached through a register base
   (`[reg+0x20064]`). They were swept as displacement values `0x20060..0x2007f`, same anchoring.
4. **Pointer escapes.** I followed the calls and thunks that hand these addresses, or values loaded from them, to code
   outside `0x004b6xxx` (`callers.py`: E8/E9 rel32 sites and raw dword references, linear-sweep confirmed). I also
   followed the RW file-system code that calls `[table+0x14]` (`disrange.py` over `0x00550130..0x00552000`).

Caveats:
- An access through a computed base cannot be found by a value scan, except for the slot displacement fields in
  step 3. Section 3 lists every escape route I found. Section 5 lists the ones I did not follow.
- The function inventory has 5,800 rows and lacks some functions, for example FUN_004b6cd0 (`0x004b6cd0..0x004b6ec1`
  per RESULT_RO-ORACLE-GATE). This affects only the "outside body" labels in the TSVs. The anchor rule above covers
  those cases.

## 2. Absolute references

### 2.1 Outside `0x004b6xxx`: none

- 93 candidate decodes outside `0x004b6xxx` (89 distinct hit byte positions, `abs_outside_cover.tsv`).
- All 89 are **MISDECODE**: the linear sweep decodes a different instruction over those bytes.
- Typical cases: `0x0044fa0c` is the tail of `0x0044fa0d mov [esp+0x90],ecx`. `0x004dd79e` is inside
  `0x004dd79f cmp ecx,0x9000`. `0x0055db88 mov esp,0x8ee924` is inside `0x0055db86 mov [esp+edi*4+0x24],eax`.
- 750 `RAW` `.text` hits have no operand decode at all (instruction bytes such as `81 ec 8c 00` = `sub esp,0x8c`).
- The 4-aligned `.data` dwords whose value falls in range (`0x0061c8fc..0x0061edf8`, `0x006344d4`; for example
  `0x008f0000`, `0x00910000`) sit in 16-bit-pair tables of the same shape RESULT_RO-ORACLE-GATE described at
  `0x0061eee0`. [UNCERTAIN] I did not run a code-reference check on those table bases. No absolute code operand
  points at them from this sweep's ranges.

### 2.2 Inside `0x004b6xxx` (all SYNC; context only)

| address | instruction (exact address, mechanical) | function |
|---|---|---|
| `0x007d3e4c` | W `0x004b67ab` (=0), R `0x004b68e0`, W `0x004b6a14` (=1) | FUN_004b67a0, FUN_004b68e0, FUN_004b6940 |
| `0x007d3e50` | W `0x004b66ad`. R `0x004b6710`, `0x004b6770` (test byte). R `0x004b67e0` (byte) | FUN_004b6640, 4b6710, 4b6770, 4b67e0 |
| `0x007d3e54` | W `0x004b66bb` (edx = 0x58 table), W `0x004b6700` (=0), R `0x004b6ad0` (pushed to `0x00550790`) | FUN_004b6640, 4b6700, 4b6940 |
| `0x007d3e58` | R `0x004b67a5`, W `0x004b67bd` (=0), W `0x004b6acb` (return of `0x005507a0`) | FUN_004b67a0, 4b6940 |
| `0x007d3e5c` | W `0x004b6618`, R `0x004b6875`, R `0x004b69ca` | FUN_004b6610, 4b67e0, 4b6940 |
| `0x007d3e60` | W `0x004b661d`, R `0x004b687e`, R `0x004b69d3` | FUN_004b6610, 4b67e0, 4b6940 |
| `0x007d3e64` | W `0x004b69a8`, W `0x004b6b5e`, R `0x004b6d95` (cmp), R `0x004b6da8` (sub) | FUN_004b6940, 4b6b20, 4b6cd0 |
| `0x007d3e68` | W `0x004b69ae`, W `0x004b6b78`, R `0x004b6da0` (cmp) | FUN_004b6940, 4b6b20, 4b6cd0 |
| slot 0 `0x008cd9c0` | `0x004b663a add eax,0x8cd9c0` after `imul eax,eax,0x20078` (returns slot base) | FUN_004b6630 |
| slot 0 `+0x38` `0x008cd9f8` | W `0x004b66c3` (= table) | FUN_004b6640 |
| slot 0 `+0x60` `0x008cda20` | W `0x004b66c9` (=0) | FUN_004b6640 |
| slot 0 `+0x20068..70` `0x008eda28/2c/30` | W `0x004b65e2/e7/ec` (=0) | FUN_004b65e0 |
| slot 1 `+0x38` `0x008eda70` | W `0x004b66d3` (= table) | FUN_004b6640 |
| slot 1 `+0x60` `0x008eda98` | W `0x004b66d9` (=1) | FUN_004b6640 |
| slot 1 `+0x20068..70` `0x0090daa0/a4/a8` | W `0x004b65f1/f6/fb` (=0) | FUN_004b65e0 |
| directory `0x0090dac0` | `mov reg,0x90dac0`: `0x004b6902`, `0x004b6a4a` (rep stosd 0x1000 dwords), `0x004b6a62` (fill dest), `0x004b6bbf`, `0x004b6c5c` | FUN_004b68f0, 4b6940, 4b6ba0, 4b6c00 |

Slot displacement fields `+0x20064..+0x20070`: 35 SYNC sites, all in `0x004b6630..0x004b6fa7` (`disp20064_hits.tsv`).
The 9 hits outside `0x004b6xxx` are all MISDECODE (`disp20064_outside_cover.tsv`).

## 3. Indirect routes from outside `0x004b6xxx` (the reader evidence)

**R1. `0x007d3e4c`, read through FUN_004b68e0.**
- `0x004b68e0 mov eax,[0x7d3e4c]; ret` is reached only through the thunk `0x004b65a0 jmp 0x4b68e0`.
- The thunk's callers are `0x00411f6e` (FUN_00411f30), `0x004587ff` (FUN_004587a0) and `0x0048ff49` (FUN_0048ff20).
- At all three sites the next instructions are `push imm32; call 0x4b65b0`, and the return of that call is what
  gets pushed (`0x00411f73/78/7d`, `0x00458804/09/0e`, `0x0048ff4e/53/58`).
- So eax from `0x004b65a0` is overwritten before any use. **No caller consumes the value of `0x007d3e4c`.**

**R2. Directory entry `+0x78`, read through FUN_004b68f0.**
- `0x004b692b mov eax,[esi+0x78]` reads it, with esi = `0x0090dac0 + i*0x80`. The function is reached only through
  the thunk `0x004b65b0`.
- Callers: `0x00411f78`, `0x00458809`, `0x0048ff53`. Each one pushes eax as an argument to `0x004b3b70`
  (`0x00411f7d`, `0x0045880e`, `0x0048ff58`). **The value is consumed.**
- In the ORIG_TOP exe, `0x004b68f0` is bound to `PortTop_PizFs_EntryLength_004b68f0`
  (`mashedmod/src/mashed_re/Match/PortTop_00402750_Pieces.cpp:200`), which answers from the PIZ_FS mount stack. So the
  consumer gets the value without the directory.

**R3. Directory entry pointer stored in slot `+0x20064`.**
- `0x004b6c8f mov [edx+0x20064],esi` (FUN_004b6c00, table `+0x28`) stores the entry pointer, with edx = arg 2.
- Every reader of `+0x20064` is in `0x004b6xxx` (section 2.2).

**R4. Slot fields `+0x18`, `+0x20`, `+0x38`, read and written by RW file-system code.**
- These code paths get a slot pointer by calling `[table+0x14]`. For the table FUN_004b6640 builds, `+0x14` is
  `0x004b6630` (stored at `0x004b666e`), which returns `0x008cd9c0 + i*0x20078`.
- FUN_00551330 (called at `0x004b66e3`; esi = arg 1 = the table, `0x00551361`; ebx = arg 2 = 2) loops
  `0x00551384 call [esi+0x14]`, then:
  - `0x0055138a` writes dword `[slot+0x20]` = 1
  - `0x00551397` writes dword `[slot+0x18]` = `[table+0x10]`
  - For i = 0, 1 those are `0x008cd9e0`, `0x008cd9d8`, `0x008eda58`, `0x008eda50`.
- FUN_00550580: `0x005505a7 call [edi+0x14]`, `0x005505aa` reads dword `[slot+0x20]`, `0x005505bf` writes
  `[slot+0x20]` = 5.
- FUN_00550be0 walks the list at `[0x7dc754]`: `0x00550bfd call [esi+0x14]`, `0x00550c07` reads dword `[slot+0x20]`,
  `0x00550c0c` reads dword `[slot+0x38]` (= the table), then `call [ecx+0x3c]`.
- [UNCERTAIN] Other `[reg+0x38]` readers in this range take a file pointer as an argument whose source I did not
  trace: `0x0055091b` (FUN_00550910, which also writes `[reg+0x20]` = 1 at `0x00550928`), `0x00550960`, `0x00550990`,
  `0x005509b4`, `0x00550a32`, `0x00550ab8`, `0x00550af8`, `0x00550bd8`, `0x00550dab`, `0x00550f28`, `0x00551097`.
- Every route into the slots goes through the 0x58 table. That table is allocated, filled and registered only in
  FUN_004b6640 (`0x004b6657` alloc through `[0x7d3ff8]+0x108`, `0x004b66e3 call 0x551330`, `0x004b66e9 call 0x550430`).
  FUN_004b6640 is reached only by `0x004b6540 jmp`, and the ORIG_TOP exe replaces that with PIZ_FS (PORT-TOP-04E,
  RESULT_RO-ORACLE-GATE).

**Writers outside, through thunks** (they store caller arguments, they do not read):
- `0x004027c7` (FUN_00402750) calls `0x004b6560`, a jmp to FUN_004b6610, which writes `0x007d3e5c` / `0x007d3e60`.
- `0x00402b51` (FUN_00402a40) calls `0x004b6550`, a jmp to FUN_004b6700, which writes `0x007d3e54` = 0.
- `0x00495326` (FUN_004952f0) calls `0x004b6590`, a jmp to FUN_004b67a0, which writes `0x007d3e4c`, `0x007d3e58` and
  `0x007d3e48` = 0 and calls `0x004b65e0` (zeroes the slot `+0x20068..70` words).

## 4. Mask proposal (for the L-152 decision row, not applied)

| group | reader evidence | proposal |
|---|---|---|
| `0x007d3e4c..0x007d3e6b` (8 dwords) | All reads are in `0x004b6xxx`. The one value that leaves (`0x007d3e4c` via `0x004b65a0`) is discarded at all 3 call sites (R1). No absolute reference outside. | **mask** as bound family `pizfs` |
| slots `0x008cd9c0..0x0090daaf` | Readers outside are RW FS code (R4). They reach the slots only through the table that FUN_004b6640 registers, and the exe does not run FUN_004b6640. `+0x20064..70`: only `0x004b6xxx`. | **mask** as bound family `pizfs`, provided the decision accepts that the RW FS manager list state (`0x7dc754` chain, out of scope here) differs for the same reason |
| directory `0x0090dac0..0x00911abf` | Absolute references only in `0x004b6xxx`. The entry pointer escapes only into slot `+0x20064` (R3). Entry `+0x78` reaches 3 consumers outside (R2) through `0x004b68f0`, which the exe binds to PIZ_FS. | **mask** as bound family `pizfs`. Whether the bound `0x004b68f0` returns the same length is a value check for the oracle, not a reason to keep the directory unmasked |

## 5. Not covered / [UNCERTAIN]

- I did not trace the provenance of the `[reg+0x38]` / `[reg+0x20]` readers listed under R4 that take a file-pointer
  argument.
- I did not check which tables RW's list at `0x7dc754` holds besides this one.
- The `.data` 16-bit-pair dwords in section 2.1 have no code-reference check.
- Side finding, outside this row's addresses:
  - FUN_004b65c0 (`0x004b65cf mov esi,0x8ab7e0`, `mov ecx,0x82b`, `0x004b65d4 rep movsd`) copies
    `0x008ab7e0..0x008ad8ab` into its caller's buffer. That range includes the counters
    `0x008ad868`/`878`/`87c`/`880`/`884`/`888` that L-152 (a) masked on "sole reader `0x004b6db2`".
  - Its caller is `0x00495312` (FUN_004952f0), which passes the buffer to `0x00496400` at `0x00495321`.
    `0x00496400` is a site of the `disable_log` boot patch.
  - So L-152 (a)'s sole-reader statement is incomplete by one site. That site's consumer is the boot file log.

## Files (all in this folder)

`sweep.py`, `cover.py`, `callers.py`, `disrange.py` (scripts); `abs_hits_inrange.tsv` (173 CODE decodes in range),
`abs_outside_cover.tsv` (89 anchoring verdicts), `disp20064_hits.tsv`, `disp20064_outside_cover.tsv`, `callers_a.tsv`,
`callers_b.tsv`. Commands, from this worktree:
`py -3.12 verify/pizfs_readers_20261114/sweep.py ../../original/MASHED.exe.unpatched re/analysis/function_inventory.csv 008cd9c0:00911ac0 007d3e4c:007d3e6c`.
