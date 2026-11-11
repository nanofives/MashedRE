# RESULT PORT-OT-PARTICLE-3 (2026-11-10): PARKED, admission request for 0x0048e820 (L-115 order step 2)

Verdict: **no byte match. The full byte-diff table explains every byte as one register-allocation kind: the
callee-saved ESI <-> EDI pair swap.** That is not an already-admitted class (it is not SIB base/index order), so
0x0048e820 stays the stand-in `PortTop_StandIn04G_0048e820`. Swapping 0x0048cf70 (item 2) waits on the admission
decision. No build input changed.

## 1. Match attempt

- **Source:** `re/match/s15/0048e820.c` (MATCH-S15 h2), compiled with the DDK (XP DDK cl 13.00.9176 / c2 13.00.9178,
  `/O2 /GS /Ob1`). `match.py --bytes --show` gives `85.5% (regs 100.0%, ops 100.0%) 124/124 ins`.
- **Recorded before this row:** do/while vs while vs for, `--i`, unsigned i, declaration order, the int literal 1,
  14/15/16 sharing a goto tail, split x/y/z stores, an empty default, and 423 permuter variants. None gained.
- **Tried in this row**, each `85.5%` unchanged (scripts in the session scratchpad, not committed):
  - `int i = 17` as an initializer, and reversed declaration order;
  - `j` in block scope, and `pt` in block scope (block-scope lever, memory ddk-match-levers);
  - a block-scope copy `int k = i` used for every inner use;
  - 1.0f through a named `float one` at function scope, before or at the initializer.
- **Tried in this row, broke the body:** a hoisted `EmitParm *e = &DAT_00722140[i]` (20.2%), `switch ((unsigned)i)`
  (25.8%), and the loop rewritten as `while (i-- != 0)` (71.9%).

## 2. Full byte-diff table (L-115 order)

Script: `verify/particle3_20261110/bytetable_0048e820.py`. Output: `verify/particle3_20261110/bytetable_0048e820.txt`.

- **Method:** the whole obj symbol (code plus the 16-entry switch table, 664 B) is compared byte k to byte k with
  MASHED.exe.unpatched at 0x0048e820. 260 bytes at relocation sites are masked.
- **Rule for each differing byte:** it must be a 6 <-> 7 exchange (ESI <-> EDI) in one 3-bit register field, with
  every other bit equal. The field is ModRM r/m, ModRM reg, or the +r of a one-byte opcode.
- **Masked bytes:** `re/tools/match/relocheck.py` reports `0x0048e820 OK checked=48 unchecked=0`, so every masked
  site resolves to the original's address.
- **Result:** 18 differing bytes in 18 instructions, **0 unexplained**, and 0 differing bytes in the switch table or
  padding.

| # | off | ours | original | class |
|---|---|---|---|---|
| 1 | +0x05 | `be 11 00 00 00` mov esi, 0x11 | `bf ..` mov edi, 0x11 | +r ESI<->EDI |
| 2 | +0x0a | `bf 00 00 80 3f` mov edi, 0x3f800000 | `be ..` mov esi, 0x3f800000 | +r ESI<->EDI |
| 3 | +0x16 | `4e` dec esi | `4f` dec edi | +r ESI<->EDI |
| 4-5 | +0x17, +0x2c | `8b c6` mov eax, esi | `8b c7` mov eax, edi | ModRM r/m |
| 6 | +0x34 | `8d 56 ff` lea edx, [esi-1] | `8d 57 ff` lea edx, [edi-1] | ModRM r/m |
| 7-16 | +0x68 .. +0x209 (10 switch cases) | `8b ce` mov ecx, esi | `8b cf` mov ecx, edi | ModRM r/m |
| 17 | +0x234 | `89 b9 <reloc>` mov [ecx+DAT_00722144], edi | `89 b1 ..` mov [ecx+0x722144], esi | ModRM reg |
| 18 | +0x249 | `85 f6` test esi, esi | `85 ff` test edi, edi | ModRM reg + r/m |

What the swap is:
- The original holds the outer counter `i` (17 -> 0, the emitter index) in EDI and the hoisted 1.0f bit pattern
  `0x3f800000` in ESI. Ours holds them the other way round.
- The other hoisted values agree: `bl = 0xff`, and `ebp = 0x3d4ccccd` (0.05f).
- Both registers are callee-saved and saved in the same prologue order (push esi, push edi at +0x03/+0x04, identical
  bytes), so the swap changes no value, no stack layout, and no instruction length or order.
- 0x0048e820 has no arguments and no return value (`void FUN_0048e820(void)`), so neither register crosses its
  boundary.

## 3. Request to the PORT lead (L-115 / L-117)

**Admit 0x0048e820 per function under L-117, kind "callee-saved ESI <-> EDI swap, no other byte".** The table above
is the evidence: 18/18 differing bytes are that swap, relocheck is OK, and the instruction stream is otherwise equal
(124/124, ops 100%).

If admitted, the next step (item 2) is:
1. Run the unedited s15 source under MASHED_ORIG_TOP as the near-match body in place of
   `PortTop_StandIn04G_0048e820`. Its caller 0x0040bbb0 is on the knob path.
2. Show that 0x0048cee0 (C4 matched, its only callee of interest) fills the 17 records at 0x00722470.
3. Swap 0x0048cf70, a stand-in under 0x0040bd00, onto its matched body, which reads those records and passes them to
   the matched 0x004768c0.

If not admitted, 0x0048e820 stays a stand-in and 0x0048cf70 cannot be swapped.

## Gates

- No build input changed: only the byte-table script and its output under `verify/`.
- rva-lint: OK, 0 new.
