# RESULT MATCH-43DFD0-FRAME (round 2, worker "frame")

Row MATCH-43DFD0-RAISE, round 2. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-FRAME` from `orch/MATCH-43DFD0-RAISE` 75a7be877. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Final source sha256 `3f03835794fe6b75d91786d1f0c86425229da3a7ef7214ae59d35dcd55aa68b4`.

## Verdict

GREEN as a raise, not a match. The prologue now matches: `sub esp, 0x2c` as at 0x0043dfd0, and `push ebx / push esi / push edi`
as at 0x0043e02b..0x0043e02d. Every `[esp+N]` stack slot in the body sits at the original's offset. No byte-identical claim, so no
relocheck and no promotion TSV.

| Measure | before (75a7be877) | after (this branch) |
|---|---|---|
| match.py whole function | 32.2% (regs 58.5%, ops 79.6%), 2804/2817 ins | **45.9%** (regs 57.9%, ops 81.1%), 2811/2817 ins |
| `--bytes` | 10978 B whole symbol, 6384 differing unmasked bytes, **first at +0x2** | 11046 B whole symbol, 6421 differing unmasked bytes, **first at +0x5f** |
| instructions equal (`--show` rows marked `=`) | 906 | **1294** |
| frame immediate | `sub esp, 0x28` | `sub esp, 0x2c` |

The differing-byte count rises by 37 because our symbol grew by 68 bytes (more instructions now line up, so offsets after them
shift differently). The score and the equal-instruction count are the measures of fit.

## What the extra slot is

The original's frame has 11 dword slots (0x2c). With 4 registers pushed, the body addresses them as `[esp+0x10]..[esp+0x38]`:

| `[esp+N]` (4 pushes) | Original use (addresses) |
|---|---|
| 0x10 | team-loop counter `local_2c` (0x0043eb44 / 0x0043eb5d, same in all 5 copies) |
| 0x14 | team-loop count `iVar8` (0x0043eb3e / 0x0043eba7) |
| 0x18 | `uVar10` of cases 3-5 (`&DAT_005f65c8` word, stored at 0x0043e9b2, reloaded into ebx at 0x0043ee4e / 0x0043ee5a / 0x0043efd1 at depth +8), then the `= 6` counter at 0x0043f626 / 0x0043f669 |
| 0x1c..0x34 | the 7 dword flags (zeroed 0x0043e3ff..0x0043e417, set 0x0043e43d..0x0043e48b) |
| **0x38** | **`piVar13`, spilled in team-slot copy 3 only**: `mov [esp+0x38], edi` at 0x0043edec, `mov ebx, [esp+0x38]` at 0x0043ee3c |

In copy 3 (`uVar10` in {0x20000, 0x30000, 0xb0000, 0x60000}, original 0x0043eddf) the original keeps `uVar10` in ebx through the loop,
because it is tested again right after (0x0043ee90..0x0043eea2). `piVar13` gets the stack slot, and the inner search loads it into ebx
and reloads `uVar10` from `[esp+0x18]` on both exits. Before this change our build did it the other way round (`piVar13` in ebx, `uVar10`
in memory), so the frame had 10 slots, and the `uVar10` slot and the flag slot swapped places (ours `[esp+0x1c]` / `[esp+0x18]`
against the original's `[esp+0x18]` / `[esp+0x1c]`).

## Change (5 lines moved, source lines 786, 840, 907, 966, 1019)

In all five team-slot loops the original assigns `piVar13` before the `if (0 < iVar8)` test, ahead of the `jle`:
`mov ebx, edi` at 0x0043eb42 (copy 1), 0x0043ec65 (copy 2), 0x0043ef47 (copy 4), 0x0043f078 (copy 5), and the spill store
`mov [esp+0x38], edi` at 0x0043edec (copy 3). The source had `piVar13 = piVar18;` inside the `if`. Moved it above the `if` in all five.

Measured subsets (whole function):

| copies hoisted | score | frame |
|---|---|---|
| 3 only, 1, 2, 1+2, 1+2+3, 4 only, 3+4, 2+4, 4+5, 1+2+5, 1+2+3+5, 2+3+4+5 | 32.2% | 0x28 |
| 1+4 | 45.6% (ops 74.9%) | 0x2c |
| 1+3+4+5 | 45.5% | 0x2c |
| 1+2+4, 1+2+4+5 | 45.9% (ops 77.4-77.5%) | 0x2c |
| 1+2+3+4 | 45.9% (ops 81.0%) | 0x2c |
| **all 5 (kept)** | **45.9% (ops 81.1%)** | **0x2c** |

Copies 1 and 4 both hoisted is what flips the allocation. All five is kept because it is the original's shape in every copy
(addresses above) and scores highest.

Slot check: the ordered list of every `[esp+0x..]` instruction in ours against the original's
(0x0043dfd0..0x00440aac, registers normalised): before, 29 differing lines, with the swapped 0x18/0x1c roles and no 0x38. After,
12 differing lines and every offset present on both sides. The 12 left are order and form, not offsets: `cmp [esp+0x20]` laid out
earlier in ours (the 0x0043f6d4 store-block region), three `mov [esp+0x14], eax` where the original has one (0x0043f63e region), one
`mov [esp+0x10]` swapped with its neighbour, and `mov reg, [esp+0x30/0x34]` where the original has `cmp [esp+0x30/0x34], ebx`
(0x00440457, 0x004409c8).

## Residual (for the dispatch-tree and store-block workers)

- **First differing byte +0x5f: the 0 and 1 registers are swapped.** The original holds 0 in ebx and 1 in edi (`xor ebx, ebx` at
  0x0043e02e, `mov edi, 1` at 0x0043e03b). Ours now holds 0 in edi and 1 in ebx. Before this change ours held 0 in ebp. Not attempted:
  it is a colour choice across the whole function, not a frame slot.
- The store-block lines above (0x0043f63e, 0x0043f6d4, 0x0043f738, 0x0043f752) and the flag tests at 0x00440457 / 0x004409c8 are in the
  other round-2 workers' ranges.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` and `--show` before and after (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022, syntax only, no object, no link) in
  `mashedmod/src/mashed_re/Match`: exit 0. No local was renamed.
- Diff: `re/match/s08/0043dfd0.c` (5 moved lines plus the header score on line 2), `re/orchestrator/UNCERTAIN_REGISTER.md` (one line), this file.
- Scratch tools (session scratchpad, not committed): `ours.py` (capstone listing of our compiled symbol), `odis.py` (original listing
  from `original/MASHED.exe.unpatched`, read only), `exp.py` + `hoist.py` (subset runs).

## [UNCERTAIN]

[UNCERTAIN] The hoist keeps behaviour. This rests on reading the source: `piVar13` is read only inside the five loops' search
(`piVar13[0..2]`), and every later read (source lines 1267, 1268, 1282, 1337, 1343, 1356, 1363) follows its own assignment at lines
1265, 1280, 1335, 1354. With `iVar8 <= 0` the new assignment is dead. Test: `re/tools/match/equiv.py` on this source against
75a7be877's, or the PROVISIONAL knob-path menu walk ON against OFF. Not run here (match-only row, L-168). One line in
`re/orchestrator/UNCERTAIN_REGISTER.md`.
