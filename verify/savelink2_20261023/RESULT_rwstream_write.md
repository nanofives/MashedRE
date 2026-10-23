# RESULT: savelink2 (b) slice 1, RwStreamWrite 0x004cbe80 as one shared body

PREREG: `PREREG_rwstream_write.md` (12365dcdf). Final build: exe `6136bda0`, dev .asi `b66a1544`. Evidence:
`D:/MashedWT/savelink2_data/leaf_ab/` (W1, W1b, W2 `.out` / `.leaf.log`) and `D:/MashedWT/savelink2_data/arms_rws.txt`.

**Verdict: P1 to P4 hold.** One harness amendment, stated below. RwStreamWrite now has one body in Render/RwStreamCore.cpp
for both targets (exe: dead code). The asi-only copy in Save/RwStream.cpp is removed.

## Defect fixed in the removed copy (static, from the disassembly)

| Path | Original | Removed copy |
|---|---|---|
| short file write | `blk={1, FUN_004d7ff0(0x8000001c)}`, `FUN_004d8480(&blk)` (0x004cbeb9..0x004cbed4) | `FUN_004d8480()`, no argument |
| first-alloc fail | `FUN_004d7ff0(0x80000013, 0x200)` (0x004cbf15..0x004cbf35) | no 2nd arg, no block |
| grow fail | `FUN_004d7ff0(0x80000013, grow - cap)` (0x004cbf8c..0x004cbfad) | no 2nd arg, no block |
| unknown type | `FUN_004d7ff0(0xe)`, block (0x004cc014..0x004cc02c) | no block |

The non-error paths of the removed copy matched the bytes. The 2026-10-11 negctl re-earn ran a bad-type case, but its
fixture observed the scratch and the return value and not the RW error slot, so its GREEN did not see this.

## Arms

| Arm | Result |
|---|---|
| W1 (sweep, first run) | 182 cases, noo 2, ndiff 2. Both were grow cases with **noo = 1** (the original disagreed with itself). |
| W1b (sweep, harness amended) | **182 cases, noo 0, ndiff 0**, 65 with an error emitted. The error slot was compared in every case. |
| W2 (live, 60 s, 4 cars) | read 45,568 calls (pure 2883 0/0, file 42,685 0/0/u3), skip 16 (0/0). **Write: 0 live calls.** |
| G (exe 6136bda0) | H_DEF-2 bacfc747 / 8dd1c004 / 6f9932b6. B_DEF_K0/K1/K26 f9deb9b9 / d3601fcc / 9da9a26a. Unchanged. |
| L lint | 8 findings, allowlisted 8, NEW 0. |

Harness amendment (W1 -> W1b): in the allocating cases the sweep compared the first 96 bytes of the new block. A grown or
first-allocated RW-heap block is uninitialized past the defined data. The comparison now covers the defined prefix only,
`max(starting capacity, new position)` bytes. The body code is unchanged between W1 and W1b.

An earlier W1 attempt is void. The private root held a stale `mashed_re_dev.asi` from 2026-10-13, so it ran old code. The
fresh .asi is now deployed in `D:/MashedWT/savelink_root/original` (old one kept as `mashed_re_dev.asi.bak_1013`).

## Write sweep coverage (66 cases)

- 35 memory writes with room: positions 0/1/5/60/64, sizes 0..32.
- 3 first-alloc: n 0 / 10 / 0xff.
- 12 grow from a 64-byte RW-heap buffer: positions 0/60/64, n 5/10/65/0xff.
- 8 unknown types: 0, 5, 0x80000000, 0xffffffff, each with n 0 and 4.
- 8 callback writes: return 0 / 3 / 0x80000000 / 0xffffffff, each with n 0 and 8.

Not covered by any A/B:
- The file write path (types 1/2).
- The two allocator-failure paths, which are the 2-argument error form.

Those rest on the disassembly reading only.

## Trackers

re-classify, file move only: hooks.csv row 0x004cbe80 file -> Render/RwStreamCore.cpp, with a note and a CHANGELOG entry.
**Level kept at C4, standing flagged.** The C4 evidence predates this body, and the new body ran 0 times in a 60 s canonical
race (strict-C4 rule: the body must execute). Whether C4 stands, or the row drops to C3 until a canonical scenario runs a
write (manual save or replay save), is a decision for Mariano.
