# Runtime observation of the save data-globals (U-3558, U-3560)

Date: 2026-08-19
Harness: `re/frida/probe_save_globals.py` (new). Raw records: `log/probe_save_globals.json`.
Binary: patched `original/MASHED.exe` (10 boot patches; `.unpatched` anchor
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` verified).
Method: spawn-suspended so the init-time writes cannot be missed, hardware write
watchpoints armed pre-resume, snapshots at labelled nav checkpoints, plus live
disassembly (`re/frida/disasm_at.py`). Three runs; spawned PIDs killed by PID only.

Both uncertainties recorded a Ghidra MCP call as their resolution path. That is
unavailable on this account, so the evidence below is behavioural (runtime
observation) plus live disassembly of the two save routines.

---

## Ground truth: the two save routines, disassembled live

### `SerializeToBuffer` @ `0x00404EE0` — gather, then snapshot

```
0x00404EE0  xor  ecx, ecx
0x00404EE2  mov  eax, 0x7f105c
0x00404EF0  mov  dl,  byte ptr [eax]            ; LOW BYTE of record[i] dword0
0x00404EF2  mov  byte ptr [ecx + 0x7f0f54], dl  ; -> packed[i]
0x00404EF8  add  eax, 0x4c
0x00404EFB  inc  ecx
0x00404EFC  cmp  eax, 0x7f13ec
0x00404F01  jl   0x00404EF0
0x00404F03  mov  eax, dword ptr [0x8a95ac]      ; live counter
0x00404F0F  mov  ecx, 0x148
0x00404F0F  mov  esi, 0x7f0a40 / edi, 0x827d98
0x00404F19  rep movsd                           ; span -> snapshot
0x00404F23  mov  dword ptr [0x828254], eax      ; counter -> mirror
0x00404F2A  mov  ecx, 0x928f / esi=[0x8a94a8] / edi, 0x80335c
0x00404F34  rep movsd                           ; profile -> buffer (if ptr != 0)
0x00404F37  mov  dword ptr [0x803358], 0xdeadbeef
```

The gather runs **before** the span copy. Because `0x007F0F54` lies inside the
span (`0x007F0A40 + 0x514`), the packed bytes it stages are carried into the
snapshot by the same `rep movsd`. They are not written to the save buffer
separately.

### `DeserializeFromBuffer` @ `0x00404E80` — restore, then scatter

```
0x00404E87  mov  esi, 0x827d98 / edi, 0x7f0a40
0x00404E91  rep movsd (0x148 dwords)            ; snapshot -> span
0x00404EA0  movzx edx, byte ptr [ecx + 0x7f0f54]
0x00404EA7  mov  dword ptr [eax], edx           ; FULL DWORD, zero-extended
0x00404EA9  add  eax, 0x4c
0x00404EAD  cmp  eax, 0x7f13ec
0x00404EB2  jl   0x00404EA0
0x00404EC1  mov  dword ptr [0x8a95ac], eax      ; mirror -> live counter
0x00404ED2  rep movsd (0x928f)                  ; buffer -> profile (if ptr != 0)
```

**Asymmetry (new).** The gather reads **one byte** (`mov dl, [eax]`); the restore
writes **four** (`movzx` + `mov dword`). A save/load round-trip therefore
**zeroes the upper 3 bytes** of each record's dword0. Any port that treats both
directions as byte-sized is not faithful.

---

## U-3558 — the 12 stride records at `0x007F105C`

### Established

- Array geometry confirmed by an independent writer: `0x00496568` is
  `rep stosd` with `ecx = 0x13` (19 dwords = 76 bytes = `0x4C`) — it clears
  **exactly one whole record**. Record size `0x4C` is therefore real, not inferred.
- **The record base is `0x007F1038`, NOT `0x007F105C`** — from
  `0x0049653B lea esi,[ebp+0x7f1038]` with `ebp = index * 0x4C`
  (`0x00496536 imul ebp,ebp,0x4c`). Therefore the gathered field is at
  **offset `+0x24`** within the record (`0x007F105C - 0x007F1038 = 0x24`), not
  at offset 0. This corrects the first version of this note, which read the
  save's base address as the record base. It also agrees with `exe_main.cpp`
  addressing the same pair of arrays as `0x007f1044 + player*0x4c` and
  `0x007f1504 + player*0x4c`, i.e. offset `+0xC` of the same two bases.
- **A parallel array B at `0x007F14F8`**, same stride/size: `0x00496552`
  `rep movsd` (ecx=0x13) copies `A[i] -> B[i]` immediately BEFORE `A[i]` is
  zeroed. B is a snapshot of the previous value.
- **Array A extent is 16 entries**: `(0x007F14F8 - 0x007F1038) / 0x4C = 16`
  exactly. The save persists only the first **12**; the initializer sets the
  first **8**; the liveness predicate accepts only the first **4** (below).
  These three counts over one array are not reconciled — see "Still open".
- **A third array C at `0x007E95C0`, stride `0x200`**, whose `+0x13C` field is
  the liveness flag: `0x00497450` computes `[eax*0x200 + 0x7e96fc]`
  (`= 0x007E95C0 + i*0x200 + 0x13C`) and returns `!= 0`. That function
  **caps the index at 3** (`0x00497454 cmp eax,3; jle`, else return 0), so
  there are **4 slots maximum**.
- The gathered field's observed value is only ever `0` or `1`.
- Loop bound `0x007F13EC` with stride `0x4C` from `0x007F105C` gives exactly 12
  iterations.
- **These records are SHARED LIVE STATE, not save-private scratch.** Two
  non-save writers were caught by hardware watchpoint on record[1]
  (`0x007F10A8`):
  - `0x00492550` `mov dword ptr [eax], 1` + `add eax, 0x4c`, bound `0x007F12BC`
    (= record index 8). Sets dword0 = 1 for a contiguous run ending at index 7.
  - `0x00496568` `rep stosd` (ecx=0x13) — zero-fills whole records; fired **38
    times in 40 captured hits**, i.e. it is a hot, continuous writer.
- Observed live pattern, stable across boot -> menu -> depth 2 -> 3 -> 4 -> 5:
  `dword0 = [0,0,0,1,1,1,1,1,0,0,0,0]` (indices 3..7 set).
- The shipped `original/gamesave.bin` holds `00 01 01 01 01 01 01 01 00 00 00 00`
  at file offset `0x24F54` (= span `+0x514`), **byte-for-byte identical** to the
  live `0x007F0F54` observed after the save load. Confirms the packed region is
  restored from file as part of the span block-copy.
- **Live records and restored packed bytes disagree at runtime.** After
  `DeserializeFromBuffer` fired (observed, ret `0x00409D34`), packed became the
  file's `[0,1,1,1,1,1,1,1,0,...]` while the records stayed
  `[0,0,0,1,1,1,1,1,0,...]`. The scatter is straight-line code and must have
  run, so the restored values are subsequently overwritten by the live writers
  above. Restoring these records from the save is largely overridden by game code.

### The index correlates exactly with the active-slot count

`0x00496530` is called once per active slot from the loop at
`0x0049680B..0x0049682B`: `0x00496814 call 0x00497450` (liveness predicate),
`0x00496820 call 0x00496530` on true, `0x00496825 inc dword ptr [0x00772FFC]`
(the active-slot counter).

Runtime dump, stable from main menu through depth 5:

| | index 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8..15 |
|---|---|---|---|---|---|---|---|---|---|
| `A[i] + 0x24` | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 1 | 0 |
| `C[i] + 0x13C` | 1 | 1 | 2 | 0 | 0 | 0 | 0 | 0 | 0 |
| counter `0x00772FFC` | 3 | | | | | | | | |

The correlation is exact: the counter is **3**, and precisely indices 0, 1, 2
are the ones with a non-zero liveness field in C **and** the ones cleared to 0
in A. Indices 3..7 retain the `1` written by the initializer at `0x00492550`.
So `A[i]+0x24 == 1` means "initialized and not yet consumed"; the per-slot
activation path (`0x00496530`) snapshots the record to B and zeroes it.

At `t4_hold_end` a late change appeared: `C[9]+0x13C` became `4` while the
counter stayed 3 and `A[9]` stayed 0 — so C is written beyond the 4-slot window
the predicate allows. Not explained.

### Still open

- The **semantic identity** of the slots is NOT established. `[UNCERTAIN]`.
  What the evidence supports: a **4-slot table with a per-slot liveness handle**,
  in the `0x00495xxx`/`0x00496xxx` neighbourhood that also contains the joypad
  code guarded by the `fix_joypad` boot patch (`0x00495870`), with the
  containing routine reached via `0x004967E3 call 0x00495FE0` /
  `0x004967ED call 0x004972B0` and a 32-byte buffer at `0x00773058`
  (`0x004967F2 push 0x20`). A player/controller-slot reading fits every
  observation, but **is not proven** and is recorded here as a hypothesis only.
  Missing evidence to close it: re-observe `0x00772FFC` with the device count
  forced to a known value. `patch_mashed_force_keyboard.py` forces the joystick
  count to 0 and would be the direct test — it must be applied to a COPY of the
  binary, never to `original/MASHED.exe` (the diffing reference).
- The three disagreeing counts over array A — save persists 12, initializer
  sets 8, liveness predicate caps at 4 — are not reconciled.
- Why the initializer's run ends at index 7 (bound `0x007F12BC`) is not established.

**Verdict: NARROWED, not closed.** The tracker's literal question ("which struct
field at each position?") is answered: **offset `+0x24` of a 19-dword (`0x4C`)
record based at `0x007F1038`**, one byte per entry, 12 entries persisted. The
own-vs-bind classification is settled by evidence (shared live state — bind).
The semantic identity of the slots is not.

---

## U-3560 — the profile pointer `0x008A94A8`

### Established

- Written **exactly once per process**, at `0x0041145D`
  (`mov dword ptr [0x8a94a8], edi`), inside the function called at `0x00402874`
  (i.e. entered at `0x004113B0`). Watchpoint caught it in all three runs; value
  differs per run (`0x04A1C8E8`, `0x049EC8E8`, `0x046CC8E8`) — a heap allocation.
- Source: indirect call `call dword ptr [eax + 0x108]` at `0x00411450`, where
  `eax = [0x007D3FF8]`; size argument is `[0x008A94AC]`.
- **`0x008A94AC` is the allocation-size global**; observed `0x00024A3C` =
  150,076 = exactly `0x928F` dwords, i.e. precisely the amount the save copies.
  The whole block is serialized.
- The block is **explicitly zero-filled immediately after allocation**:
  `0x00411465 mov ecx,[0x8a94ac]; shr ecx,2; xor eax,eax; rep stosd` at
  `0x00411472`, tail `rep stosb` at `0x0041147E`.
- **The block is ALL ZERO throughout the frontend.** Chunk scan reports
  `0 of 147` non-zero KB-chunks at every checkpoint (boot, menu, depth 2, 3, 4,
  5), in two independent runs.
- Both copies are gated on the pointer being non-null (`test edi,edi` /
  `je` at `0x00404EBA`/`0x00404EC6`, and `test esi,esi` / `je` at
  `0x00404F21`/`0x00404F28`).

### Correction to the tracker text

UNCERTAINTIES.md records the block as "150,076-byte (0x2443C)". The decimal is
right, the hex is wrong: `0x2443C` = 148,540. The observed size global holds
`0x24A3C` = 150,076 = `0x928F * 4`. Same class of arithmetic slip as the
`0xA0`-vs-`0x40` gap error in `structs/gamesave_layout.md:21,101`.

### Still open

- The **layout/semantics** of the 150,076-byte block. `[UNCERTAIN]` — missing:
  any observation of it non-zero. Nothing populated it in the reachable
  frontend, so no field structure could be observed.
- Whether it ever becomes non-zero requires reaching career/championship play;
  nav reached depth 5 and could not advance further unattended.

**Verdict: NARROWED, not closed.** Owner, allocation site, size global,
zero-fill and gating are all established. The struct layout is not.

---

## Consequence for the standalone port

`GameSaveFormat.h` leaves the profile region zero and does not model the stride
records. For everything observable in the frontend, that matches the original:
the profile block **is** all-zero there. The port's documented
`DAT_008a94a8 == 0` framing differs mechanically from the original (which
allocates and zero-fills a real block, and would copy 150,076 zero bytes), but
the resulting save image is identical because the source bytes are zero.

For the stride records the port drops state that the original **does** persist,
but the observation above shows the restored values are overwritten by live
writers (`0x00492550`, `0x00496568`) shortly after load — so the practical
fidelity cost is smaller than the byte count suggests. Quantifying it requires
the semantic identity that remains open.
