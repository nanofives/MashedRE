# PREREG dualcopy4 group (b), block 4: 0x0042d3e0 (MenuEntryArrayInit / RecordsZero)

Committed BEFORE any run that touches this RVA. Read against `PREREG_dualcopy4_b.md`.

## 1. What the bytes say (unpatched original, `disasm_va.py 0x0042d3e0 59`, output in `disasm_42d3e0.txt`; decomp in `decomp_42d3e0.txt`)

```
0x0042d3e0 mov eax, 0x898ac4 ; xor ecx, ecx
0x0042d3e7 L: mov [eax-4],ecx ; mov [eax],ecx ; mov [eax+4],ecx            entry = eax-4 : dwords at entry +0, +4, +8
           mov [eax+8],cl ; [eax+9],cl ; [eax+0xa],cl ; [eax+0xb],cl       bytes at entry +12, +13, +14, +15
           mov [eax+0xc],ecx ; [eax+0x10] ; [eax+0x14] ; [eax+0x18]         dwords at entry +16, +20, +24, +28
           mov [eax+0x20],ecx ; [eax+0x24] ; [eax+0x28]                     dwords at entry +36, +40, +44
0x0042d410 add eax,0x34 ; cmp eax,0x8990dc ; jl L                           30 iterations (0x618 / 0x34), entries 0..29
```
Not written: entry +32 (the dword at `eax+0x1c`), and entry 30 onwards. Fourteen stores.

## 2. The three bodies

| body | target | what it does | against the bytes |
|---|---|---|---|
| original | | as above | |
| `Frontend/MenuInit.cpp:MenuEntryArrayInit` (asi installed, C3, key `menu_entry_array_init`, `void_write_observe` on the single dword 0x00898ac0) | both targets compile it; the exe calls it only from exe_main.cpp's bring-up / probe (lines 8686, 8833) | takes `base = (u8*)eax - 4` and writes `base-4, base+0, base+4, base+8` (dwords), bytes `base+9..+12`, dwords `base+16, +20, +24, +32, +36, +40` | **does not match**: it writes `entry-4` (one dword before the entry, i.e. the previous entry's last dword, or 0x00898abc for entry 0), writes bytes +9..+12 where the original writes +12..+15 (so bytes +13..+15 stay), writes +32 which the original leaves, and does not write +28 and +44. The comment "NOT zeroed: +28" repeats the error. Its key could not see this: it observes only the first dword of entry 0 |
| `Frontend/MenuNavSM.cpp:RecordsZero` (exe only) | the exe's menu state machine, `MenuNavSM.cpp:599` | `memset(g_records, 0, sizeof g_records)` over 31 entries and `g_record_count = 0` | zeroes also +32, +48 (the struct's `pad20` / `pad30`) and entry 30, and has no `entry-4` write; a different object (`g_records`, `MenuRecord` 52 bytes) than the original's 0x00898ac0 array |

## 3. Change

`Frontend/MenuEntryZero.h:MenuEntryZeroBody(first_entry, count)` (header-inline) is the one body, written from the disassembly. `MenuInit.cpp:MenuEntryArrayInit` calls it on
`0x00898ac0, 30` (installed, anchored); `MenuNavSM.cpp:RecordsZero` calls it on `g_records, 30` and sets `g_record_count = 0` (RVA anchor removed). The registry key
`menu_entry_array_init` is rewritten as `cache_setter_observe` with seeds over entries 0, 1, 2, 28, 29, 30 plus the dword before the array, all dwords filled with a
pattern, and observes every one of them.

## 4. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| G-MASK (host, no game) | `MenuEntryZeroBody` on a 0xFF-filled buffer clears exactly the 14 stores above (mask from `disasm_42d3e0.txt`), for count 30 | equal. The OLD body, compiled from the git HEAD text into the same test, differs at exactly: entry-4 cleared (extra), bytes +13..+15 kept (missing), +28 kept (missing), +44 kept (missing), +32 cleared (extra) |
| G-MCTRL-old (asi = base.asi, the registered export) | the new `cache_setter_observe` vectors run against base.asi's `MenuEntryArrayInit` | RED, mismatching on the dwords at entry+12 (bytes 13..15), +28, +32, +44 and at 0x00898abc; GREEN on every dword the old body got right |
| G-PATH1 (final asi) | same vectors, GREEN, non-degenerate (the observed dwords go from the pattern to 0 and +32 stays at the pattern) | GREEN |
| G-PATH2 | `run_verify_hook.py menu_entry_array_init` PASS (the vectors need the p2 shape or the keys' seeds) | PASS, or reported as site-verified |
| G-HOST | `run_tests.bat`: navsm GREEN (it builds records through RecordsZero's callers) | GREEN |
| G-REC (exe) | a repo-wide grep finds no reader of `MenuRecord::pad20` / `pad30` or of `g_records[30]` that depends on RecordsZero clearing it | none found; if one is found, `RecordsZero` keeps a memset of the unwritten fields and the row stays split |
| G-CTRL (exe) | `2b4d47bb / 356821c8 / f7a155e1` | UNCHANGED |
| Row | 0x0042d3e0: C3 -> C2 (DEMOTED, the old body was not faithful) -> C3 RE-EARNED on the corrected shared body only if G-MASK, G-PATH1, G-PATH2 and G-HOST pass; `exe_file == file` | |

## 5. Decision rule

If G-MASK shows the old body matching the original (my reading of the bytes wrong), nothing is changed and this block is withdrawn. Otherwise the correction lands with
the evidence above; the exe's `RecordsZero` change is reported as "default build behaviour: yes (pad fields and entry 30 no longer zeroed by the reset)" if G-REC is clean.
