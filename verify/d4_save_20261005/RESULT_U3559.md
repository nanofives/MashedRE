# RESULT — U-3559 was already RESOLVED in May. A stale tool header is what misled three legs.

**RAN 2026-10-05.** Static read via `re/tools/decomp_pc.py` (read-only pool slot `Mashed_pool0`;
Ghidra MCP unreachable, `decomp_pc.py` is the sanctioned no-MCP path). **No C-level moved, no band
moved, no game run, `original/` read-only.**

---

## 1. The answer, and it was in the tracker since 2026-05-22

`UNCERTAINTIES.md:532`, **U-3559**, verbatim:

> *"Tail [0x24440..0x24F9F] IS written by Save::SerializeToBuffer (0x00404EE0). Two explicit writes:
> (1) RVA 0x00404F19 MOVSD.REP copies 0x148 dwords from championship table DAT_007F0A40 → 0x827D98
> (save_buf+0x24A40); (2) RVA 0x00404F23 MOV [0x00828254], EAX stores save-counter. … ghidra_eval
> confirmed 0 XREFs … no second writer exists."*

**I re-derived this from the decompilation rather than reading the row.** The region
`0x24cb0`..`0x24f5b` that `RESULT_SPAN.md` called "undocumented by any tool in the tree" is simply
the rest of that 0x148-dword championship table.

## 2. The layout, confirmed from `FUN_00404ee0` and checked against both files

| file range | size | source | citation |
|---|---|---|---|
| `0x0000` | 4 | `0xDEADBEEF` | `_DAT_00803358 = 0xdeadbeef` |
| `0x0004`..`0x24A40` | **`0x24A3C`** (150,076) | `*DAT_008A94A8`, `0x928F` dwords | copy to `&DAT_0080335C` |
| `0x24A40`..`0x24F60` | **`0x520`** | `DAT_007F0A40`, `0x148` dwords | copy to `&DAT_00827D98`, RVA `0x00404F19` |
| `0x24EFC` | 4 | `DAT_008A95AC` save counter | `DAT_00828254 = DAT_008a95ac`, RVA `0x00404F23` |
| `0x24F60`..`0x24FA0` | **`0x40`** | **nothing** | the only never-written bytes |

Verified against the committed files:

- the profile ends **exactly** at the championship base `0x24A40` that `gamesave_edit.py:10` documents;
- `0x008A94AC` read live = **150,076** = the profile byte length;
- file `0x24EFC` holds **0** in the original and **9** in the standalone's — consistent with a save
  counter and `GameFlow`'s `++g_saveCounter`;
- **all 345 differing bytes fall inside regions `FUN_00404ee0` writes — 0 outside**;
- **0 differences** in the 64-byte never-written residual.

So the real unwritten region is **64 bytes, not 0xB60**, and nothing diverges in it.

## 3. What was actually broken — and it is a live bug, not a comment

`re/tools/gamesave_parse.py` carried the stale layout **as executable constants**:

```python
PROFILE_SIZE: int = 0x2443C  # 150,076 bytes; 0x928F dwords via REP MOVSD
TAIL_OFFSET:  int = 0x24440
TAIL_SIZE:    int = 0xB60    # UNCERTAIN U-3559
```

`0x2443C` is **148,540**. `0x928F` dwords is **150,076 = `0x24A3C`** — a transposed digit, and the
comment beside the constant already said the right number. **The two sizes still summed to `0x24FA0`,
which is why it went unnoticed**, but the profile/tail boundary sat `0x600` too low, so every caller
got `0x600` bytes of profile prepended to `tail`.

**U-3560 had already recorded the fix**: *"HEX CORRECTED: previously recorded as 0x2443C, which is
148,540 — the decimal was right and the hex was wrong."* The tracker was corrected; the tool was not.

**Fixed:** `PROFILE_SIZE` → `0x24A3C`, `TAIL_OFFSET` → `0x24A40`, `TAIL_SIZE` → `0x560`, and the
module docstring and `GameSave` dataclass docs rewritten with the real sub-structure. Constants
re-verified to sum to `0x24FA0` and `TAIL_OFFSET == 4 + PROFILE_SIZE`.

**One existing test had to change, and it is evidence for the bug rather than against the fix.**
`test_tail_first_nonzero_offset` asserted tail-relative `0x604` while its own docstring named **file
offset `0x24A44`** — both true only because `TAIL_OFFSET` was `0x600` low. The file offset is the
real observation and is unchanged; the test now asserts it directly and is boundary-independent.
That the old tail began with exactly `0x604` guaranteed zeros *was the bug's signature*: `0x600`
mis-attributed profile plus championship row 0 column 0. **16 of 16 tests pass.**

Also corrected: `re/analysis/boot_app_init_d3/0x004113b0.md:21` glossed `0x24a3c` as "decimal
148540" — the same transposition in the opposite direction. The hex was always right.

## 4. The process finding, which is the real output

**A corrected fact lived in `UNCERTAINTIES.md` while the stale version lived in a tool header that
people actually read.** That header misled, in order: `PREREG_SAVE.md` (built its whole three-way
LIVE/FROZEN/DEAD gate on "a tail not written by SerializeToBuffer"), `RESULT_SAVE.md`, and
`PREREG_SPAN.md`/`RESULT_SPAN.md`. Three legs, one stale comment.

The existing memory `read-the-rva-plate-before-building-a-probe` covers the inverse of this. The
addition: **when a tool header and a tracker row disagree, the tracker wins — and the tool header
should be fixed on the spot, because it is the one that gets read.**

## 5. What is and is not closed

- **U-3559: already resolved, nothing owed.** No tracker edit is needed; a pointer from the tool
  header to the row has been added instead.
- **The `0x24cb0`..`0x24f5b` question from `RESULT_SPAN.md` is CLOSED** — it is the championship
  table beyond `gamesave_edit.py`'s documented first 13×`0x30`. `0x148` dwords total, of which
  156 are the documented grid and **172 are not decoded**. Naming those 172 is open and is U-3560-adjacent,
  but it is a *decode* question, not a *writer* question.
- **Still open, unchanged:** whether the original accepts or behaves correctly on the standalone's
  save. No game was run.
- **No C-level moved**, no band moved, no code under `mashedmod/` touched.
