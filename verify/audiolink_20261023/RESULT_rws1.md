# RESULT: audiolink AudioRws slice 1, seven pure leaves in Audio/AudioRwsLeaves.cpp

PREREG: `PREREG_rws1.md`. Branch `work/audiolink`. Build: exe `ed93b4fd`, dev .asi `d4f80839`. Evidence:
`D:/MashedWT/audiolink_data/path1/R1_*`, `arms_rws1.txt`, `disasm_rws1.txt`.

**Verdict: P and G hold.** These seven moved from the asi-only Audio/AudioRws.cpp into Audio/AudioRwsLeaves.cpp, in
both rsp files, one body each and exe dead code:

| RVA | Function |
|---|---|
| 0x005aca80 | AudioFmtSizeCalc |
| 0x005ac980 | AudioFmtDescCopy |
| 0x005ae0b0 | AudioSubStructZeroInit |
| 0x005aeca0 | AudioFieldEndianPack |
| 0x005ae0c0 | AudioWaveFmtCopy |
| 0x005be190 | AudioRwsSubZeroInit |
| 0x005be140 | AudioSubStructThreeWrite |

- Four ports return the pointer the bytes leave in EAX (were void): 0x005ac980 (param_2), 0x005ae0b0, 0x005be190 and
  0x005be140 (param_1). This is static evidence: the registry entries do not read the return.
- **P:** path1 7 / 7 GREEN, 10 cases each.
- **G:** exe `ed93b4fd`. bacfc747 / 8dd1c004 / 6f9932b6, f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. Lint 0 NEW.

## 0x005acaa0 from the bytes (unported, C2; for U-3032 / U-1731)

0x005acaa0 starts right after 0x005aca80, with no padding (`re/console/cache/func_bounds.csv:5281` gives
005acaa0..005acd01). Arguments: p1 = desc, p2 = buf (its slot becomes the cursor), p3 = direction, p4 = swap.

**p3 != 0, serialise desc into buf:**
- Header with swap: 0x1c bytes, field by field through 0x005aeca0:
  - u32 desc[0]
  - u32 `(desc[4] != 0)`, a boolean, not the pointer (0x005acacb..0x005acada)
  - u32 desc[8]
  - u8 desc[0xc], u8 desc[0xd], u16 0
  - u32 desc[0x10], u32 desc[0x14]
  - u8 desc[0x18], u8 desc[0x19], u16 0
- Header without swap: `rep movsd` of 7 dwords (0x005acb93), so +4 is the raw pointer.
- Key: if desc[4] != 0, 0x005ae0c0(desc[4], cursor, p4), cursor += 0x10.
- Extra data: if desc[0x10] != 0, copy desc[0x14] bytes raw from desc[0x10], then `desc[0x18] |= 2` on the source
  (0x005acbec).
- Returns p1.

**p3 == 0, deserialise buf into desc (0x005acbf7):**
- Header: the mirror sequence (pads zeroed), or 7 raw dwords.
- Key: 0x005ae0c0(pos, pos, p4) in place, then 0x005acd60(pos), the global format-table scan, stored into desc[4]
  (0x005acce2). **A NULL lookup returns 0, not desc** (0x005acce9).
- Extra data: desc[0x10] = the current buffer position, no copy (0x005accf8).
- Returns p1.

## What is left of AudioRws.cpp (asi-only)

19 functions. The pool / list family's globals are now explained: they are BSS until the audio open
(`POOL_CONSTRUCTOR.md`: 0x004627f0 -> 0x005a5f00 -> init table 0x00632ba8 -> 0x005ae650). Private zero storage is the
faithful pre-open state in the exe. AudioFmtGlobalScan can use the dumped 0x00633674 keys. AudioContextLookup still
needs 0x005ac930.
