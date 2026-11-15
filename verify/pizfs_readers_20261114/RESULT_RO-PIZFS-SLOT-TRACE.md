# RESULT RO-PIZFS-SLOT-TRACE (run 47, 2026-11-15)

L-157 follow-up to RO-PIZFS-UNMASKED-READERS (R4, the 11 [UNCERTAIN] `[reg+0x38]` readers) and RO-PIZFS-READERS-RUN
(section 3.2). Questions: where does each reader's file pointer come from, which tables the list at `0x007dc754`
holds, and what each reader implies for masking the slots `0x008cd9c0..0x0090daaf`. **No mask applied, no mask
table edited.** Masking returns to Mariano. No build, no gates.

**Verdict: HANDOFF (static chain complete, confirming run not executed, section 5; open row RO-PIZFS-SLOT-LIST-RUN).** Every file pointer the 11 readers
receive is a file object returned by FUN_00550580, the only producer in the RtFS code. FUN_00550580 gets it from
`call [table+0x14]` (`0x005505a7`) on the table that the open path picked from the `0x007dc754` list. For the pizfs
table that is a slot (`0x004b6630`), for the Win32 tables it is a 0x60-byte object in `[table+0x58]`. The list
holds the pizfs table ("pzfs" / device "piz:") and Win32 tables made by FUN_00551190. 3 of the 11 readers can never
see a slot (they are Win32-table methods). 8 can.

## 1. Method

- Binary: `original\MASHED.exe.unpatched` (SHA-256 `BDCAE093...3C0E`, the anchor), read only. Linear sweeps with
  capstone 5 from function starts (`disrange.py`, `callctx.py`), E8/E9 and raw-dword reference scans with linear-sweep
  anchoring (`refs_slottrace.py`, this folder). Function starts from `re/analysis/function_inventory.csv`.
- Run: section 5.

## 2. The RtFS objects (static, cited)

**List.** Head `[0x007dc754]`, count `[0x007dc75c]`, max `[0x007dc758]`, default table `[0x007dc76c]`.
FUN_00550430 inserts at the head (`0x005504a9..0x005504c1`: `[t+0] = head`, `head = t`, count + 1) after a name
check over `[t+0x50]` (`0x0055046b..0x00550478`). FUN_00550790 sets the default (`0x00550794`), FUN_005507a0 reads it.

**Table layout** (both builders): `+0x04` file count, `+0x0c` device string, `+0x10` dword copied into each file
`+0x18` (`0x00551391..0x00551397`), `+0x14` getter `(t,i) -> file`, `+0x28` open, `+0x2c` close, `+0x30` read,
`+0x34` write, `+0x38` seek, `+0x3c`, `+0x40`, `+0x44`, `+0x48`, `+0x4c` (called with the name by FUN_00550b00,
`0x00550bad`), `+0x50` name. FUN_00551330 (both builders call it) sets `+0x04`, `+0x10` = 0, `+0x50` (strcpy
`0x00551369`), `+0x0c` (strdup `0x005513c9..0x005513cc`), and for each file i: `[file+0x20]` = 1 (`0x0055138a`),
`[file+0x18]` = `[t+0x10]` (`0x00551397`).

| field | pizfs table, FUN_004b6640 (0x58 B, zeroed `0x004b6668`) | Win32 table, FUN_00551190 (0x5c B) |
|---|---|---|
| `+0x14` getter | `0x004b6630` (`0x004b666e`): `0x008cd9c0 + i*0x20078` | `0x00550c30` (`0x005511cc`): `[t+0x58] + i*0x60` (`0x00550c3d..46`) |
| `+0x18` | `0x0045b350` (`0x004b6675`) | `0x00551290` |
| `+0x28` open | `0x004b6c00` | `0x00550c50` (CreateFileA `0x00550d18`) |
| `+0x2c` close | `0x004b6f60` | `0x00550d80` (CloseHandle) |
| `+0x30` read | `0x004b6cd0` | `0x00550da0` (ReadFile) |
| `+0x34` write | **0** | `0x00550f20` (WriteFile) |
| `+0x38` seek | `0x004b6ed0` | `0x00551050` (SetFilePointer) |
| `+0x3c` | **0** | `0x00551090` (GetOverlappedResult) |
| `+0x40` | **0** | `0x00551180` |
| `+0x44` | `0x004b6f90` | `0x005514e0` |
| `+0x48` | **0** | `0x005512b0` |
| `+0x4c` | `0x004b6ba0` | `0x005512d0` (GetFileAttributesA) |
| files | 2 slots, `[slot+0x38]` = table written at `0x004b66c3` / `0x004b66d3` | count = arg 1, array `[t+0x58]` = alloc `n*0x60` (`0x0055122e..37`), `[file+0x38]` = table for each (`0x0055123e..0x00551249`) |
| name / device | "pzfs" `0x005d8788` / "piz:" `0x005d8780` (args `0x004b66a6..0x004b66ba` from `[0x006172f0]` / `[0x006172f4]`) | per call site, section 3 |

The pizfs functions `0x004b6b20..0x004b6fc0` contain no `[reg+0x18/0x1c/0x20/0x28/0x30/0x34]` access and no indirect
call (sweep of that range). So slot fields `+0x18..+0x34` are written only by RtFS code (`0x0055xxxx`).

**Open paths (the producer).** FUN_005507b0 (`fopen`, mode string parse `0x005507bb..0x0055084b`) and FUN_00550670
(open with callback) both pick the table, then call FUN_00550580 (`0x005508f5`, `0x00550728`):
- If the name has a `:` at index k, the prefix `name[0..k]` is copied (`0x00550884`, `0x005506ab`) and compared with
  each list table's `[t+0x0c]` (`0x0055089b..0x005508aa`, `0x005506c2..0x005506d1`). A match picks that table.
- Otherwise the default `[0x007dc76c]` (`0x005508c6`, `0x005506e9`).
- FUN_00550580: under the critical section `0x009124e0`, `call [t+0x14]` for i = 0.. (`0x005505a7`), takes the first
  file with `[file+0x20]` == 1 (`0x005505aa`, `0x005505b0`) and writes `[file+0x20]` = 5 (`0x005505bf`). If the open
  flags have bit 0x10, or `[file+0x18]` != 0, or `[t+0x10]` != 0, it writes `[file+0x28]` = 2, `[file+0x30]` =
  callback, `[file+0x34]` = context (`0x00550608..0x00550629`). Then `call [t+0x28]` (`0x00550634`). Return 1:
  `[file+0x1c]` = 2 (`0x0055064e`), returns the file. Else `[file+0x20]` = 1 (`0x0055063f`), returns 0.

**Default device and the pizfs table.** FUN_004b6940 writes `[0x007d3e58]` = FUN_005507a0() (`0x004b6acb`, the old
default) and calls FUN_00550790(`[0x007d3e54]`) (`0x004b6ad0..0x004b6ad6`): while it is in effect, every open with no
device prefix goes to the pizfs table. FUN_004b67a0 restores `[0x007d3e58]` (`0x004b67a5..0x004b67b5`).
FUN_004955d0 sets the default to a Win32 table (`0x00495713..0x00495725`).

## 3. Which tables the list at `0x007dc754` holds (static)

Every call of FUN_00550430 in the image (E8 scan, 4 sites):

| site | caller | table | count | name args |
|---|---|---|---|---|
| `0x004624d7` | FUN_004624c0 (called at `0x00462822`, FUN_004627f0) | FUN_00551190 at `0x004624cc`, also stored at `[0x0068f4cc]` (`0x004624d2`) | 5 | `0x005cc4de` (empty string) twice |
| `0x00495663` | FUN_004955d0 (called at `0x00495780`) | FUN_00551190 at `0x0049564e` | 0x14 | `0x005cfee0` "unc" and GetCurrentDirectoryA (`0x0049560d`) + "\\" (`0x00495638..0x0049564b`) |
| `0x00495700` | FUN_004955d0, loop over GetLogicalDrives bits (`0x00495686`), GetDriveTypeA (`0x004956c8`) result not 0 or 1 | FUN_00551190 at `0x004956f1`, one per drive | 0x14 | two stack strings built from the drive letter (`0x004956a9..0x004956ec`) |
| `0x004b66e9` | FUN_004b6640 (only via `0x004b6540 jmp`) | the pizfs table | 2 | "pzfs" / "piz:" |

FUN_00551190 is called only at these three sites, FUN_004b6640 only through `0x004b6540`. There is no other table
builder that calls FUN_00551330 (E8 scan: `0x004b66e3`, `0x00551253` only).

## 4. The 11 readers

All 11 read `[file+0x38]` (the owning table) from a file argument. In the table: method offsets are given from the
table base (`[file+0x38]+0x28` is the base of the method block, then `+4/+8/+0xc/+0x10`).

| reader | function | it calls | callers (E8/E9, linear-sweep anchored) | file pointer comes from | can it be a pizfs slot? |
|---|---|---|---|---|---|
| `0x0055091b` | FUN_00550910 (close; writes `[file+0x20]` = 1 `0x00550928`) | `[t+0x2c]` | `0x004cc17c` (FUN_004cc160), `0x00555a9f` (FUN_00555830), `0x005ab16f` jmp (FUN_005ab150), thunk `0x004b5370` <- `0x0047b93f` | see the chains below | yes (pizfs close `0x004b6f60`) |
| `0x00550960` | FUN_00550950 (read) | `[t+0x30]` | `0x004cbd62` (FUN_004cbd30), `0x005ab943` (FUN_005ab710), thunk `0x004b5380` <- `0x0047b937` | | yes (run-observed on slot 0, READERS-RUN 3.2) |
| `0x00550990` | FUN_00550980 (write) | `[t+0x34]` | `0x004cbead` (FUN_004cbe80) | stream `+0x0c` | a slot would make it call address 0 (`[t+0x34]` = 0) |
| `0x005509b4` | FUN_005509b0 (seek; if `[file+0x18]` != 0 writes `+0x28` = 4, `+0x20` = 3) | `[t+0x38]` | `0x004cc084` (FUN_004cc050), `0x005ab1ef`, `0x005ab2e3` (FUN_005ab1c0), `0x005ab95b` (FUN_005ab710), thunk `0x004b5390` <- `0x0047b917`, `0x0047b929` | | yes. For a slot `[file+0x18]` = `[t+0x10]` = 0, so no write |
| `0x00550a32`, `0x00550ab8` | FUN_00550a20 (line read: read, then seek back) | `[t+0x30]`, `[t+0x38]` | `0x005559fa` (FUN_00555830), `0x00555b8e/c71/cb7` (FUN_00555af0), `0x005560a8/1d5`, `0x00556368/381` (FUN_00555ff0), `0x005567c4`, `0x00556868` (FUN_00556780) | FUN_00555830's own fopen | yes |
| `0x00550af8` | FUN_00550af0 | `jmp [t+0x44]` | `0x004cbd71` (FUN_004cbd30), `0x004cc091` (FUN_004cc050) | stream `+0x0c` | yes (pizfs `0x004b6f90`) |
| `0x00550bd8` | FUN_00550bd0 | `jmp [t+0x40]` | `0x005ab66b` (FUN_005ab620) | `[obj+0x1c]` of the audio stream object | a slot would make it jump to 0 (`[t+0x40]` = 0) |
| `0x00550dab` | FUN_00550da0 = Win32 read | (no table call before use) | only as `[t+0x30]` of a Win32 table (`0x005511e8`) | Win32 file | **no** |
| `0x00550f28` | FUN_00550f20 = Win32 write | | only as `[t+0x34]` (`0x005511ef`) | Win32 file | **no** |
| `0x00551097` | FUN_00551090 = Win32 `+0x3c` | | `[t+0x3c]` (`0x005511fd`) and `0x005512be` (FUN_005512b0 = Win32 `[t+0x48]`) | Win32 file | **no** |

No raw dword reference to any of the 8 wrapper entries exists (scan), so they are reached only by the direct calls
listed. Chains for the callers:

- **RwStream** (FUN_004cc160 close, FUN_004cbd30 read, FUN_004cbe80 write, FUN_004cc050 skip): each switches on the
  stream type `[s]` (tables `0x004cc214`, `0x004cbe70`, `0x004cc040`, `0x004cc14c`) and for types 1 and 2 passes
  `[s+0x0c]`. FUN_004cc230 (stream open) writes `[s+0x0c]` = the return of FUN_005507b0 for type 2 (`0x004cc304`,
  `0x004cc310`), or the caller's raw pointer for type 1 (`0x004cc286..0x004cc29c`). All 27 E8 calls of FUN_004cc230
  push type 2 or 3 (the last push before each call, `so_ctx` list in section 6). So statically `[s+0x0c]` for these
  readers is a FUN_005507b0 result. [UNCERTAIN] another writer of `[s+0x0c]` outside FUN_004cc230 was not searched for
  (stream objects come from an RW freelist, `0x004cc24b`).
- **FUN_00555830** (recursive, 6 E8 callers): `0x005559c8` FUN_005507b0(name, "r" `0x00617378`) -> edi
  (`0x005559d0`), passes edi to FUN_00555af0 / FUN_00555ff0 / FUN_00556780 (`0x00555a2b`, `0x00555a5e`, `0x00555a91`),
  which pass it to FUN_00550a20, then closes edi at `0x00555a9f`.
- **FUN_0047b8d0**: `0x0047b90b` thunk `0x004b5360` -> FUN_005507b0(name, "rb" `0x005cf010`) -> esi (`0x0047b912`),
  then seek, `0x004b53a0` (FUN_00550bc0, returns `[file+8]`), seek, read, close on esi.
- **Audio stream family** (FUN_005ab090 .. FUN_005ab710, callers in `0x005b1260..0x005b2fb5`): FUN_005ab090 opens with
  FUN_00550670(name, flags 0x11, callback `0x005ab710`, ctx) (`0x005ab0b6..0x005ab0ee`) and discards the return
  (`0x005ab0f6`). The file reaches the object only through the callback: `0x005ab100` -> `0x005ab130` writes
  `[[ctx]+0x0c]` = its file argument (`0x005ab13e`). The only caller of a file's callback `[file+0x30]` is
  FUN_00551550 (`0x0055158f`), called only from `0x00550e54` (Win32 read FUN_00550da0) and `0x00551141`,
  `0x00551171` (Win32 FUN_00551090). The pizfs functions never call it (section 2). So statically a pizfs slot opened
  through FUN_00550670 is never delivered to the audio objects, and `0x00550bd8` / the `0x005ab*` calls of
  `0x00550960` / `0x005509b4` get Win32 files. [UNCERTAIN] run-level: not confirmed by a run in this row.

**FUN_00550be0** (the per-frame reader `0x00550c07` / `0x00550c0c`, callers `0x0045d40a`, `0x00462ee1`): walks the
list, `call [t+0x14]` for every file, and for each file with `[file+0x20]` != 1 calls `[[file+0x38]+0x3c]`. For a pizfs
slot `[t+0x3c]` = 0. So the original only runs without a crash if every pizfs slot has `[slot+0x20]` == 1 whenever
FUN_00550be0 runs: no piz file is open across a FUN_00550be0 call. READERS-RUN saw 1802 `0x00550c07` reads in all
windows and no crash. Section 5 reads the value.

## 5. Run (confirmation)

**Not executed. The live list contents, and the value of `[slot+0x20]` when FUN_00550be0 runs, are [UNCERTAIN]
(static only). Open row: RO-PIZFS-SLOT-LIST-RUN.**

- Prepared: private root `D:\MashedWT\RO-PIZFS-SLOT-TRACE_root` (robocopy of `original\`, `MASHED.exe` SHA-256
  `b9977dab...fe87`, the same patched recipe as READERS-RUN). New read-only probe `--peek fslist` in
  `re/frida/scenario_launch.py`: walks `[0x007dc754]` (max 16 tables) and prints for each table its address, `+0x04`,
  `+0x10`, device `[+0x0c]`, name `+0x50`, the getter `+0x14` and the method words `+0x18..+0x4c`, and for each file
  object (getter `0x004b6630` or `0x00550c30` only) `+0x18/+0x1c/+0x20/+0x28/+0x38`, plus the head, max, count and
  default. Plain reads, no call, no hook, no write.
- Command (from this worktree):
  `MASHED_ROOT=D:/MashedWT/RO-PIZFS-SLOT-TRACE_root test-slot py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label RO-PIZFS-SLOT-TRACE -- py -3.12 re/frida/scenario_launch.py --track 0 --hold 10 --peek "fslist,007d3e54:u,007d3e58:u,0068f4cc:u"`
- What happened: `test-slot` waited 10 min for memory (24 GB budget full), then queued for a slot while all 8 slots
  were held by SimGolf and Simcity runs. The calling shell hit its 30-min background limit at about 22:07. The queued
  test-slot process kept waiting and printed `acquired slot 1/8 after 2542s` (42 min) at about 22:20, then exited with
  no further output: `mashed_lock.py run` and `scenario_launch.py` never printed, and no MASHED.exe was started.
  [UNCERTAIN] why the chain exited after the slot: its parent shell was already gone. Not a game finding.
  The log `fs_t0.log` holds only queue lines.
- The probe was never exercised, so it is untested code. RO-PIZFS-SLOT-LIST-RUN runs the command above, when the
  machine has a free slot, at the menu and in the race on tracks 0 and 3.

What the run must show to close the [UNCERTAIN] claims:
1. The tables on the list at menu and race time: which of the section 3 registrations ran (FUN_004624c0 with 5 files,
   FUN_004955d0's cwd table and per-drive tables, the pizfs table), their order, and `[0x007dc76c]`.
2. `[slot+0x20]` for both pizfs slots at each peek (section 4 requires 1 at every FUN_00550be0 call).
3. `[slot+0x18]`, `+0x1c`, `+0x28` for both slots (static: `+0x18` = 0, `+0x1c` = 2 after the first successful open,
   `+0x28` written only by an open with flag bit 0x10).

## 6. What each reader implies for masking the slots (stated, not decided)

Background: the slots are the file objects of the pizfs table. They reach RtFS code only through `[t+0x14]` =
`0x004b6630` of the table that FUN_004b6640 builds and registers (`0x004b66e9`). In the ORIG_TOP exe FUN_004b6640 is
replaced by PIZ_FS (RO-PIZFS-UNMASKED-READERS R4), so the original walks the slots and the exe does not.

Slot fields written by code outside `0x004b6xxx` (all in RtFS, all through a slot pointer from `0x004b6630`):
`+0x18` (`0x00551397`, value `[t+0x10]` = 0), `+0x1c` (`0x0055064e`, = 2), `+0x20` (`0x0055138a` = 1, `0x005505bf` = 5,
`0x0055063f` = 1, `0x00550928` = 1), `+0x28 / +0x30 / +0x34` (`0x00550612..0x00550629`, only for an open with flag bit
0x10). Fields `+0x38` (table) and `+0x60` are written only by FUN_004b6640. Buffer and `+0x20064..` only by `0x004b6xxx`.

| reader | for the slot mask (stated, not decided) |
|---|---|
| `0x0055091b` close | Reads slot `+0x38`, calls pizfs close, writes slot `+0x20` = 1. The file comes from a pizfs open, so it exists only on the original. Its slot writes are pizfs-bound state. |
| `0x00550960` read | Reads slot `+0x38` (run-observed). Calls pizfs read `0x004b6cd0`. The data it returns reaches the caller's buffer, outside the slots. A slot mask does not hide that value: it is compared where the caller stores it. |
| `0x00550990` write | Reads `+0x38`, calls `[t+0x34]`. For a slot that is address 0, so the original cannot have run it on a slot without a crash. No slot implication. |
| `0x005509b4` seek | Reads `+0x38` and `+0x18`. For a slot `+0x18` = 0, so it writes nothing to the slot. Reads only. |
| `0x00550a32`, `0x00550ab8` line read | Reads `+0x38` and `+0x18` (`0x00550ab3`). Same as read + seek. The text goes to the caller's buffer (FUN_00555830 chain). |
| `0x00550af8` | Reads `+0x38`, jumps to pizfs `0x004b6f90`. Its return goes to the RwStream caller (`0x004cbd79`, `0x004cc099`). Reads only. |
| `0x00550bd8` | Reads `+0x38`, jumps to `[t+0x40]` = 0 for a slot. Its file comes from the audio async-open callback, which only Win32 code invokes. No slot implication. |
| `0x00550dab`, `0x00550f28`, `0x00551097` | Win32-table methods only. Never see a slot. No slot implication. |
| (not one of the 11) `0x00550c07` / `0x00550c0c`, FUN_00550be0 | Per-frame reads of slot `+0x20` and `+0x38`. A call into the slot's table happens only if `+0x20` != 1, and that call is to address 0. So on the original these reads have no effect beyond the compare while `+0x20` == 1 [UNCERTAIN until RO-PIZFS-SLOT-LIST-RUN reads the value]. The exe's list does not hold the pizfs table, so the exe does not do these reads. |

Summary statements for the decision (not a decision):
- No reader outside `0x004b6xxx` takes a value out of a slot except `+0x18`, `+0x20`, `+0x38` (compare or dispatch),
  and the bytes the pizfs read/line-read methods return through caller buffers. Those bytes are compared at the
  caller, not in the slots.
- Every slot write outside `0x004b6xxx` is done by RtFS on a file the pizfs table produced. In the exe the same RtFS
  code acts on the files PIZ_FS produces, if any, at other addresses.
- The RtFS list state (`0x007dc754..0x007dc76c` and the table objects on the heap) also differs between the two
  builds by one table (the pizfs one). That is the condition RO-PIZFS-UNMASKED-READERS section 4 attached to the slot
  mask.

## Files

Committed (this folder): `RESULT_RO-PIZFS-SLOT-TRACE.md`, `refs_slottrace.py` (E8/E9 + dword reference scan,
anchored), `callctx.py` (instructions around call sites), `slottrace_refs.txt` (scan output for every target in
sections 3 and 4), `slottrace_streamopen_sites.txt` (the 27 FUN_004cc230 call sites with their pushes). Tooling:
`re/frida/scenario_launch.py` (`--peek fslist`, untested, section 5). Static listings came from `disrange.py`
(RO-PIZFS-UNMASKED-READERS) over `0x00550130..0x00551400`, `0x004624c0..0x00462500`, `0x004955d0..0x00495740`,
`0x004b6640..0x004b66f8`, `0x004b6b20..0x004b6fc0`, `0x004cc230..0x004cc400`, `0x005ab090..0x005ab1d8`,
`0x00551550..0x005515a0`.

Commands, from this worktree:
`py -3.12 verify/pizfs_readers_20261114/refs_slottrace.py ../../original/MASHED.exe.unpatched re/analysis/function_inventory.csv <hex>..`,
`py -3.12 verify/pizfs_readers_20261114/callctx.py ../../original/MASHED.exe.unpatched re/analysis/function_inventory.csv <before> <after> <site,..>`.

Raw data, `D:/MashedWT/RO-PIZFS-SLOT-TRACE_data` (SHA-256):

| file | SHA-256 |
|---|---|
| fs_t0.log (queue lines only, no game output) | f18c2c5c66c514acce14bc3bc5442d9c53a211538d42ee28f13e2e6c69d2a24e |
