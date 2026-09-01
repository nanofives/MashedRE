# U-0459 resolved — `_DAT_005ceac4` is a static 1/448 Y-scale constant

Area-loop frontend round 1, 2026-09-01. Gates TextSpriteScaled (`0x004739f0`) C2→C3.

## Question (U-0459)

`0x004739f0` mode `param_11 == 2` scales only Y coordinates by
`FUN_0042b8c0() * _DAT_005ceac4` (cited at `0x00473a15` / `0x00473a33`), a global
distinct from the default-mode Y scale `_DAT_005cc560`. The coordinate system this
`param_11==2` mode corresponds to was unresolved; resolution path was a Ghidra trace
of the `_DAT_005ceac4` writer + caller analysis.

## Method

- `XrefRange.java 0x005ceac4 0x005ceac8` against pool slot `Mashed_pool3`
  (analyzeHeadless, read-only; MCP is blocked on account2).
- Read the stored IEEE-754 bytes from `original/MASHED.exe.unpatched` via PE
  RVA→file-offset (image base `0x00400000`; `.data` section maps RVA 1:1 to file
  offset here).

## Findings (NO-GUESSING — every value cited)

**There is no code writer of `0x005ceac4`.** XrefRange returned 4 references into
`[0x005ceac4..0x005ceac8]`, all `read`:
- `0x00473a15` in `FUN_004739f0` — `FMUL float ptr [0x005ceac4]`
- `0x00473a33` in `FUN_004739f0` — `FMUL float ptr [0x005ceac4]`
- `0x0047412e` in `FUN_00473ee0` — `FADD float ptr [0x005ceac8]` (adjacent global, unrelated)
- `0x0047452c` in `FUN_004744a0` — `FADD float ptr [0x005ceac8]` (adjacent global, unrelated)

So `_DAT_005ceac4` is a **statically-initialized `.data` constant**, not a
runtime-written value. Its value is fixed at load; the port's hardcoded read
(`kScaleYOnly = 0x005ceac4`) is therefore always bit-identical to the original.

**Stored value** (file offset `0x1ceac4`, bytes `25 49 12 3b`):
`0x3b122549` = `0.0022321429` = **1/448** (`1/f = 447.99998`).

Context (same method):
| global | bytes | float | 1/float | role |
|---|---|---|---|---|
| `_005cd5a8` | `cd cc cc 3a` | 0.0015625 | 640.0 | X scale (default + full modes) |
| `_005cc560` | `89 88 08 3b` | 0.00208333 | 480.0 | Y scale (default mode) |
| `_005ceac4` | `25 49 12 3b` | 0.00223214 | **448.0** | Y scale (`param_11==2` mode) |

## Conclusion

The `param_11==2` mode normalizes Y coordinates against a **448-unit reference
frame** (1/448) instead of the default 480-unit frame (1/480). X is unchanged
(1/640). The constant is static with no runtime writer, so there is no
caller-context variability — the structural concern behind U-0459 (that a caller
might initialise `_005ceac4` to a context-dependent value) is closed: nothing
initialises it at runtime. Whether "448" carries a further product-level name is
not determinable from the decompilation and is not asserted (NO-GUESSING).

## Also relevant

- U-0458 (UV corner mapping) was already RESOLVED via runtime evidence + decomp 2026-05-21.
- Path1 diff GREEN 10/10 (`log/diff_text_sprite_scaled.csv`), path2 install FULL PASS
  after fixing the `draw_quad_observe` call-through gap in
  `verify_hook_install_template.js` (12-arg vector was falling through to `fn(input)` →
  "bad argument count"; same class as the GAP-5 0-arg hole).
