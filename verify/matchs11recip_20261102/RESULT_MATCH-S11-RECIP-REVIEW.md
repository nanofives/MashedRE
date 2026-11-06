# RESULT MATCH-S11-RECIP-REVIEW, 2026-11-02

**Verdict: GREEN (text-only, static).** In all 6 functions of MATCH-S11 whose port expression changed from
`/ 640.0f` `/ 480.0f` to `* (1.0f / 640.0f)` `* (1.0f / 480.0f)`, the original (anchor `original\MASHED.exe.unpatched`)
multiplies by a stored reciprocal float constant. It contains no `fdiv`, `fdivr` or `fdivp` in any of the six bodies.
The reciprocal form is the original's own, not a compiler fold of a division.

Method: `recip_check.py` (pefile + capstone, this folder) disassembles each function from the unpatched PE and prints every
`fmul`/`fdiv*`/`call`, resolving `[abs]` operands to float bits. Full output: `recip_check.txt`. Run:
`py -3.12 recip_check.py <repo>/original/MASHED.exe.unpatched`. No Ghidra, no game, no master writes.

## Constants

| address | bits | value | (float)(1.0/N), computed in float64 then rounded to float32 | equal |
|---|---|---|---|---|
| 0x005cd5a8 | 0x3acccccd | 0.0015625000232830644 | 1/640 -> 0x3acccccd | yes |
| 0x005cc560 | 0x3b088889 | 0.0020833334419876337 | 1/480 -> 0x3b088889 | yes |

## Sites (each uses both constants, x 0x005cd5a8 for the 640 axis, 0x005cc560 for the 480 axis)

Pattern in every function: `call 0x42b8b0` (screen width getter) or `call 0x42b8c0` (height getter), then
`fmul dword ptr [esp+N]` (the coordinate), then `fmul dword ptr [const]`. That is `(float)dim * coord * recip`, the
same operand order as the matched source.

| function | `fmul [0x5cd5a8]` (x640) | `fmul [0x5cc560]` (x480) | count |
|---|---|---|---|
| 0x00472c60 | 0x00472c77, 0x00472cb3 | 0x00472c95, 0x00472cfe | 4 |
| 0x00472dc0 | 0x00472dd7, 0x00472e13, 0x00472e4f | 0x00472df5, 0x00472e31, 0x00472ea9 | 6 |
| 0x00472f40 | 0x00472f57, 0x00472f93 | 0x00472f75, 0x00472fde | 4 |
| 0x004730b0 | 0x004730c7, 0x00473103 | 0x004730e5, 0x0047314e | 4 |
| 0x00473540 | 0x00473557, 0x00473593 | 0x00473575, 0x004735d3 | 4 |
| 0x004736c0 | 0x004736d6, 0x00473712 | 0x004736f4, 0x00473760 | 4 |

26 multiplies in total, 0 divides. (0x004736c0 listing stops at 0x0047382a because the scan window is 380 bytes; its
four screen-scale multiplies are all inside it, matching `004736c0.c` lines 56-59.)

## Conclusion on the form

The original loads a pre-computed reciprocal from `.rdata` and multiplies. `0x3acccccd` and `0x3b088889` are exactly
`(float)(1.0/640)` and `(float)(1.0/480)`. Rounding differs from a true divide: `x / 640.0f` is one correctly rounded
quotient, `x * 0.0015625f` is a product with a rounded reciprocal, and the two can differ by 1 ulp. VS2022 `/fp:precise`
does not rewrite `x / 640.0f` into a multiply (640 is not a power of two), so the previous port expression computed a
true divide, and the new one computes what the original computes. The change moves the port toward the original, not away.

## Gate impact

Static judgement only, not measured (the display is unavailable, per `RESULT_MATCH-S11.md`). Expected: **no movement of the
default G-CTRL or B hashes.** Reason: all six functions only scale rectangle/point coordinates by screen size and then call
vtable slots `[x+0x20]` / `[x+0x30]` (the draw calls, e.g. 0x00472d79 and 0x00472db1 in 0x00472c60). Their outputs go to the
renderer, not to the step.csv fields the gates hash. The changed value is a draw coordinate that differs by at most 1 ulp
in some inputs.

Stated for the flip decision (not decided here): if a drawstream or draw-list parity artefact is ever hashed, this change
is a flip candidate, in the direction of matching the original. The default-gate run owed in `RESULT_MATCH-S11.md` (Gates
section) covers it: if G-CTRL/B stay at the `merge_train.json` refs, the expectation holds.

Caveat: the port's wrappers compile SSE (float32 per op) while the original runs x87. The original's products are
single-precision-rounded per `fmul` only under PC_24 precision control (memory `asm-bit-identity-in-asi-not-standalone-pc24`).
This review establishes the constants and the multiply form, not bit-equality of the full chain.
