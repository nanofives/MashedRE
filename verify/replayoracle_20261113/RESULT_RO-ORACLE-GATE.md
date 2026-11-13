# RESULT RO-ORACLE-GATE (run 45, 2026-11-13): owners of the record-0 residue left after RO-FIRST-DIVERGENCE run 44

Row: replay oracle 10. Kind: top-down / harness, scoped by the lead to diagnosis this run. Worker cmwflg95v2y7slh1nj82cq1l4
(claude2, moved to claude3 for Ghidra MCP), lead cmwfldzyw2xjolh1npz5ac1rm. Branch `orch/RO-ORACLE-GATE` from
`orch/integration` `ba39eeb6b`.

**Verdict: DECISION.** Each residue word now has its owning code, cited from the decompilation. The audio words are a fix
for the ORIG_TOP audio-init lane, not a mask. The `0x008b1400` block is private state of the original piz file layer that
the ORIG_TOP exe replaces by design. Masking it is the proposed outcome, so it is filed as a decision and not applied.

**No build was needed.** No code, mask, tracker, ROADMAP or `.asi` change, no game launch, no gates (diagnosis only).
Self-test: not applicable, nothing was built or run. The RO-FIRST-DIVERGENCE compare was not re-run (the brief keeps it for
after RWC-FRAME-CLUMP merges).

## Inputs

- `verify/replayoracle_20261113/RESULT_RO-FIRST-DIVERGENCE.md` (run 44), image ON pair o2 vs `RO_ROIMG-fdimg`: record 0,
  260 counted blocks, 32,942 VALUE dwords.
- The full dword list of that pair, `D:/MashedWT/RO-FIRST-DIVERGENCE_data/fd_o2_RO_ROIMG-fdimg_full.json`, re-read here.
- Ghidra: pool slot `Mashed_pool0`, read only, MCP `decomp_function`, `listing_disassemble_range`, `reference_to`,
  `function_callers`, `memory_read`. Slot released after.
- The exe's arm log `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg.log` (which ORIG_TOP pieces ran).

## Range census of the record-0 residue (new)

`oraclegate/record0_ranges.txt` groups the 260 blocks into contiguous ranges. "nonheap" counts the dwords that the block
hash still sees: `detorig_firstdiv.block_hashes` zeroes every value in `0x00995000..0x2fffffff` with the low 2 bits clear
(the heap rule), and the exe holds 0 there.

| range | blocks | VALUE dwords | nonheap | owner (below) |
|---|---|---|---|---|
| `0x008ad800..0x008cdc00` (7 ranges) | 123 | 27,276 | 26,348 | piz chunk cache `0x008ad9c0`, (b) |
| `0x008ed800`, `0x0090d800..0x0090e800`, `0x00911800..0x00913400` | 12 | 772 | 687 | piz file slots / directory, (b) |
| `0x0068f400..0x00690800` | 5 | 347 | 79 | audio voice pool and 0x004669b0 globals, (a) |
| `0x00604400..0x00606000` | 7 | 27 | 27 | permanent-wave table, (a) |
| every other range | 113 | 4,520 | n/a | not classified in this run |

So (b) alone holds 135 of 260 blocks and 28,048 of 32,942 VALUE dwords (85.1%). The cache range includes block `0x008cd800`, which also holds the head of slot 0 at `0x008cd9c0`.

## (a) `0x00604604`, owner FUN_0045d460, and `0x0068f800`

**Store instruction.** The watch hit pc is the trap eip, which is the instruction after the store:

- `0x00604604`: hit pc `0x0045d6a8` (a `JBE`), so the store is `0x0045d6a5 MOV dword ptr [EDI + -0x8],ESI`.
  - `EDI` starts at `0x00604578` (`0x0045d68c MOV EDI,0x604578`) and steps `0x94` per entry.
  - `0x00604604 = 0x00604578 + 1*0x94 - 8`, which is entry 1.
  - `ESI` is the running voice index. Decompilation: `piVar4[-2] = iVar2`, with `iVar2 += 1` per voice of the entry.
  - Value `0x00000004` (signed 4) is the count of entry 0: `0x00604574` holds `0x00000004` in the file image (Ghidra
    `memory_read`).
- `0x0068f800`: hit pc `0x0045d600`, so the store is `0x0045d5fa MOV dword ptr [ESI + 0x68f640],EAX`.
  - `EAX` is the return of `CALL 0x005a6280` at `0x0045d5f5`.
  - The loop at `0x0045d5f0..0x0045d60c` steps `ESI` by 8 up to `0x640`, so 200 slots `0x0068f640..0x0068fc7f`.
  - `0x0068f800` is slot `(0x0068f800 - 0x0068f640) / 8 = 56`.
  - This closes run 44's [UNCERTAIN] on that store.
- **Caller chain.**
  - `0x004669f6 CALL 0x0045d460` (return `0x004669fb`) is in FUN_004669b0, the sound-init hub (hooks.csv C4, match).
  - FUN_004669b0 is reached from boot `0x004028e0`, as the run-44 backtrace `0x0045d6a8 | 0x004669fb | 0x004028e0` shows.

**All 27 differing dwords of `0x00604400..0x00606000`** sit at `0x00604570 + k*0x94` (k = 1..40, offset 0 in each entry).
Their ref values run 4, 8, 12 .. 133, the cumulative counts. So all 27 are the `0x0045d6a5` store
(`oraclegate/audio_blocks.txt`).

**`0x0068f400..0x00690800`** (79 nonheap dwords, `oraclegate/audio_blocks.txt`):

- **The 200 slot handles** at `0x0068f640 + 8*i` are heap values. Example: `0x0068f800` = `0x0d779458` (signed 225,940,568)
  in o2, `0x0d8f9458` (signed 227,513,432) in o5. The heap rule zeroes them in the block hash, so they make no block differ
  now. They need no mask if the exe later writes its own heap handles there.
- **Five floats at the slots' +4 word:**
  - `0x0068f83c` = `0x3f79999a` (signed 1,064,933,786, float 0.975)
  - `0x0068f844` = `0x3f79999a` (the same value)
  - `0x0068fa64` = `0x3f79999a` (the same value)
  - `0x0068fa8c` = `0x3f79999a` (the same value)
  - `0x0068fa94` = `0x3f366d21` (signed 1,060,531,489, float 0.7126)

  The only static writer of `0x0068f644 + 8*i` is `0x0045e157 FSTP float ptr [ESI*0x8 + 0x68f644]` in FUN_0045e0f0. Ghidra
  lists 39 callers, all in `0x0045e460..0x004669a8`. [UNCERTAIN] which of them ran before tick 1. Missing: a watch on
  `0x0068f83c`.
- **`0x0068fc84`** = `0x0000008b` (signed 139): `numpermvirtualvoices`. Writers in FUN_0045d460: `0x0045d5c5`, `0x0045d5e2`,
  `0x0045d76f`.
- **`0x0068fc8c`, `0x0068fcbc`, `0x006900d4`, `0x006904e8` = 1, `0x006904fc` = 1, and the path strings at
  `0x00690158..0x00690404`** (`toastaudio\pc\audio\pcdics\english\<name>.rws`): these sit in the FUN_004669b0 global
  set. hooks.csv `004669b0`: "sets 11 globals". The exe's `Audio/AudioCharacterBankPaths.cpp:34` names FUN_004669b0 as the
  only caller of the path builder. [UNCERTAIN] the per-dword store pc for these. They were not watched, and this run read
  no decompilation of FUN_004669b0.

**What the exe does on this path.**

- The RO arm (`run_arms.py` `RO_` kind, stack `ROIMG`) does not set `MASHED_OT_AUDIOINIT`.
- So `0x004669b0` runs the hand piece 3. Arm log line 209: `[ORIG_TOP] 0x004669b0 sound init: piece 3 HandTop_InitAudio`.
- With `MASHED_OT_AUDIOINIT=1` the matched `0x004669b0` body runs. But `0x0045d460` is still a stand-in that returns
  without running: `Match/PortTop_004669b0.cpp:70` `PortTop_AInit_0045d460`, log text
  `rva 0x0045d460: stand-in, not run (permdict.rws; piece 3 loaded the bank)`.
- The body needs RW stream reads (`FUN_004cc230` with `"toastaudio\\pc\\audio\\pcdics\\PERMDICT.RWS"`, `FUN_004cc5e0` chunk
  `0x809`) and the RW audio layer (`FUN_005a7b60`, `FUN_005a7af0`, `FUN_005a7aa0`, `FUN_005b8570`, `FUN_005a71f0`,
  `FUN_005a6280`, `FUN_005a6dc0`, `FUN_005a6710`).

**Verdict (a): a boot-side fix, not a mask.** It belongs to the ORIG_TOP audio-init lane:

- `MASHED_OT_AUDIOINIT` (PORT-TOP-4669B0, merged).
- PORT-TOP-AUDIOTICK-STOPS (handoff).
- PORT-TOP-AUDIO-HWINIT-4627F0 (blocked: the RW-audio init/term table `0x00632ba8` reads 0 in the exe).
- PORT-TOP-AUDIO-RWA (blocked: `0x005a6ea0` / `0x005a6f30`).

No ledger row names a body for `0x0045d460` (hooks.csv C2, `new`). The work is a body for `0x0045d460` on the ORIG_TOP path,
plus the AUDIOINIT knob on the RO arm, once those two blocked rows clear. The lead files that row. The `0x00632c00..0x00633c00`
range (4 blocks, 286 dwords) is adjacent to that table. [UNCERTAIN] whether it is the same table, because this run did not
classify it.

## (b) `0x008b1400`, owner FUN_004b6b20

**Store instruction.**

- The hit pc `0x004b6b88` is `MOVSD.REP ES:EDI,ESI` itself, with `EDI = 0x008ad9c0` (`0x004b6b83`) and `ECX = 0x8000`
  (`0x004b6b7e`).
- A REP string store traps inside the instruction, so the eip stays at `0x004b6b88`.
- It copies 0x8000 dwords (0x20000 B) from the file slot `+0x64` (`in_EAX + 100`, filled by `FUN_004b67e0`) into
  `0x008ad9c0..0x008cd9bf`.
- `0x008b1400` is offset `0x3a40` in that cache.
- Ref values there are file bytes: `0x00000001`, `0x0000001c`, `0x1c02000a` (signed 469,893,130), `0x80ffffff` (signed
  -2,130,706,433), `0x3e99999a`, ...

**Every static reference into `0x008ad9c0..0x008cd9c0`:**

| from | function | kind |
|---|---|---|
| `0x004b69a3` / `0x004b69b4` | FUN_004b6940 (PizOpenAndParse) | write |
| `0x004b6b83` / `0x004b6b88` | FUN_004b6b20 | write |
| `0x004b6db2` | FUN_004b6cd0 | read: copies the bytes into the caller's buffer |
| `0x004b663a` | FUN_004b6630 | end bound only: `return &DAT_008cd9c0 + param_2*0x20078` |

`0x0061eeec` / `0x0061eef0` are two more Ghidra references, typed as pointers to `0x008c0077` / `0x008c007c`. They are
16-bit values in a table (bytes at `0x0061eee0`: `81 00 2c 00 79 00 7b 00 db 01 7c 00 77 00 8c 00 7c 00 8c 00 ...`), and no
code reference reaches `0x0061eee0..0x0061ef00`. No other code reads the cache.

**The layer:**

- FUN_004b6cd0 and FUN_004b6630 have no direct callers.
- FUN_004b6640 stores both into a 0x58-byte callback table: `+0x14 = FUN_004b6630`, `+0x30 = FUN_004b6cd0`, plus
  `FUN_004b6c00`, `FUN_004b6f60`, `FUN_004b6f90`, `FUN_004b6ed0`, `FUN_004b6ba0`, `FUN_0045b350`. It registers the table
  with `FUN_00551330(puVar1, 2, ...)` / `FUN_00550430`.
- FUN_004b6640 is reached only by `0x004b6540 JMP` (the file-system setup).
- The two file slots are `0x008cd9c0` and `0x008eda38`, each `0x20078` B, ending at `0x0090dab0`.
- The slot `+0x60` words written there are `0x008cda20` and `0x008eda98`.

**Correction to run 44:**

- The backtrace frame `0x004b6e3e` is inside FUN_004b6cd0 (`0x004b6cd0..0x004b6ec1`), not FUN_004b6b90. FUN_004b6b90 is
  the 6-byte floor-align leaf it calls.
- FUN_004b6b20's only caller is FUN_004b6cd0.

**What the exe does.**

- Arm log line 156: `[ORIG_TOP] [PIZ_FS] 0x004b6540 file system setup, mode flag 0 (exe piz reader, Piz/PizReader.h)`.
  Every mount goes through `0x00495280` to the exe reader.
- That is the binding PORT-TOP-04E made ("bind the piz file system"), so the exe never runs `0x004b6640` / `0x004b6cd0` /
  `0x004b6b20`, and the cache and slots stay 0.
- The bytes the cache would hold reach the consumers through the exe reader instead.

**Verdict (b): a mask is the proposed outcome, so a decision row is filed** (below) and no mask is applied.

- **Cache:** the 123 blocks of `0x008ad9c0..0x008cd9bf` and the FUN_004b6cd0 counters `0x008ad868`, `0x008ad878`,
  `0x008ad87c`, `0x008ad880`, `0x008ad884`, `0x008ad888` are written and read only inside the replaced layer.
- **Slots, directory and globals:** this run does not show them private. The slots are handed to the RW file-system
  manager (`FUN_00551330`, `FUN_00550430`) as file objects. [UNCERTAIN] whether RW code at `0x0055xxxx` reads slot fields.
  Missing: the reference list for slot fields `+0x20064..+0x20070` from outside `0x004b6xxx`. The directory `0x0090dac0`
  (FUN_004b6940) and `0x007d3e4c` / `0x007d3e50` / `0x007d3e54` / `0x007d3e64` / `0x007d3e68` are in the same position.

## Other items

- **`0x004325c0` never runs in the exe's ORIG_TOP replay menu.** The exe runs only the `0x0043dfd0` stand-in
  (`Match/PortTop_StandIns05.cpp:24`). The `0x0043dfd0` body is the PORT-TOP-08 lane. Recorded here, no work done.
- `0x0068f800` and the other 199 slot handles are heap values, which the heap rule already handles. Neither (a) nor (b)
  asks for a mask on them.

## Decision filed

`roadmap.ps1 decision RO-ORACLE-GATE` asks this question: on the ORIG_TOP replay oracle, mask the original piz layer's
private chunk cache `0x008ad9c0..0x008cd9bf` and the FUN_004b6cd0 counters, or port the original piz layer onto
ORIG_TOP. The recommendation is to mask the cache and counters now as a bound family. The slots, directory and
`0x007d3eXX` globals come later, after their outside readers are checked. The full text is in the ledger row.

## Evidence

- Committed:
  - this RESULT
  - `oraclegate/record0_ranges.txt` (range census)
  - `oraclegate/audio_blocks.txt` (the perm-table and voice-pool dwords)
  - `raw_sha256_RO-ORACLE-GATE.txt`
- Raw, not committed (SHA-256 in `raw_sha256_RO-ORACLE-GATE.txt`):

| file | SHA-256 |
|---|---|
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/fd_o2_RO_ROIMG-fdimg_full.json` | `2734b1f22f3e1b370cd1848e762332488864820ca2b3ee19cd5e726e8f61c752` |
| `D:/MashedWT/RO-FIRST-DIVERGENCE_data/RO_ROIMG-fdimg.log` | `00b9a78cd302f2da466d33b8b2d7eae18328e783efe05da31cb282f07bdc3193` |
| `D:/MashedWT/RO-ORACLE-GATE_data/record0_ranges.txt` | `eba6132989692ddebad158ffa431a694bb0e45053bb26d52953bdb25808a9030` |
| `D:/MashedWT/RO-ORACLE-GATE_data/audio_blocks.txt` | `db4d496d7c3e4630f8af474501a03842d8c69ef87340acd23fbb799312e71573` |

- Gates: not run (diagnosis row, no build). No 12 captures. Nothing pushed.
