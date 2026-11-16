# RESULT MATCH-43DFD0-RAISE-3 (slice 3 of 3: source lines 1510-2122)

Row kind: match-only. No build, no gate, no runtime of the body (L-168). One attempt.
Base: `orch/integration` @ 7630e53ca. Source: `re/match/s08/0043dfd0.c` (DDK, `/O2 /GS /Ob1`).
Edits: only lines 1623-2122 of the base (the hunks start at 1624). Lines 1-1622 are byte-identical to the base.

## Verdict

GREEN for a raise. The source is not byte-identical, so this is no C4 claim, no promotion TSV and no relocheck.

| Measure (match.py, whole function) | before (7630e53ca) | after (this branch) |
|---|---|---|
| score | 19.2% | **26.9%** |
| regs | 31.7% | 39.9% |
| ops | 59.1% | 68.9% |
| instructions ours/orig | 2829/2817 | 2822/2817 |
| `--bytes` | 11106 B whole symbol, 7009 differing unmasked bytes (first at +0x2) | 11074 B whole symbol, 6995 differing unmasked bytes (first at +0x2) |

The bool flags as int, slice 1's lines 263-269, were applied in a scratch copy only. On that base the score is 20.52% before
and 32.91% after (regs 46.43% -> 40.75%, ops 67.55% -> 69.65%). That is the closest view of the merged state. It is not committed here.

## Slice address range (original)

- Dispatch of the switch at source line 1510: `0x0043f76f..0x0043f792` (`lea ecx,[ebp-2]` .. `jmp [edx*4+0x440ad0]` at `0x0043f78b`).
  In the original this block sits inside slice 2's code (the key-case blocks `0x0043f792..0x0043fc79` follow it).
- Everything after: `0x0043fc7e..0x00440aa9`, which is the cases of switch 1, the bVar3/bVar4/local_20 tests, switch 2
  (dispatch `0x0043ff5d`, table `0x00440b10` / index `0x00440b44`), switch 3 (dispatch `0x0044049a`, table `0x00440b64` /
  index `0x00440b98`), the bVar7 test and the `0x0067eca4` fade tail.

Instructions equal in the range (difflib on the normalized listing, branch targets dropped, our relocations resolved to
their addresses; tool `regdiff.py` in the session scratchpad, not committed). Original `0x0043fc7e..0x00440aa9` = 890
instructions, ours from the first case block of switch 1 (`+0x1c2a`) to the last `ret`:

| | before | after |
|---|---|---|
| equal, exact operands | 362 / 912 ours (39.7%) | **629 / 905 ours (69.5%)** |
| equal, registers ignored | 426 (46.7%) | **727 (80.3%)** |
| dispatch `0x0043f76f..0x0043f792` | 1 / 7 | 1 / 7 (registers come from slice 2's `ebp` = `DAT_0067e9f8`) |

No byte range of the slice is identical at the original's offsets: every earlier length difference shifts it
(first differing byte +0x2, in slice 1's prologue `sub esp,0x18` vs `0x2c`).

## What changed (levers)

1. **Case order of switches 2 and 3 = the original's layout order.** The jump tables give the original's block order
   (case -> target, sorted by address), the same for both switches:
   `0x1f` `0x0043ff64`, `0x21` `0x0043ff92`, `0x20` `0x0043ffc0`, `4` `0x0043ffe1`, `0x13` `0x0043fff8`, `0x1e` `0x004400aa`,
   `0xa` `0x004400e8`, `0x11` `0x0044016a`, `0x1c` `0x004401b4`, `0x12` `0x00440201`, `0x18` `0x004402d3`, `6/7` `0x004403c1`,
   join `0x00440457` (switch 3: `0x004404a1` .. `0x0044092c`, join `0x004409c8`). The decomp had them in value order.
   Our compile lays out case blocks in source order (before: `4, 6/7, a, ..., 21`, the order written). Rewriting the cases
   in the original's order: whole function 19.23% -> 25.45%. Case `0x21` gets an explicit `break` because it is no longer last.
2. **Case 6/7 in the original's shape** (both switches). With the reorder alone the join landed after case 4 (`+0x1f08`).
   The original has it after 6/7, and its 6/7 differs: `DAT_0067f184` is held in a local (`mov esi,[0x67f184]` at
   `0x004403fe` / `0x00440969`), the loop decrements that local (`dec esi` at `0x00440430`, `inc esi` at `0x004409a0`),
   `ea68` is tested as `!= 0` with its own store (`0x004403e6..0x004403f4`, `0x00440951..0x0044095f`), and one shared
   `call 0x42f6b0` tail (`0x00440452` / `0x004409c3`). Switch 3 reads no `DAT_0067f184` in the bVar6 condition
   (`0x00440461..0x0044046e`); its loop saves the start value (`mov edi,esi` at `0x0044099d`) and restores it on `0xc`
   (`0x004409ac`). After this the join follows 6/7 as in the original (ours `+0x238a`, `+0x2949`) and the duplicated
   `FUN_0042f6b0` tails are gone: 25.45% -> 26.86%.

Semantics are unchanged: each moved case still ends in `break`. The 6/7 chains store the same values in the same order
before each call. Switch 3's restore value is still `DAT_0067f184` at switch entry, because nothing writes it between the
`FUN_0042b540` call and the 6/7 block.

## Tried and not kept

- Switch 1 (line 1510) case order. The original's layout is case 1, the join, the bVar3 code, then 4, 0xf, 2, 8, 3/9,
  0x15, 0/5 (`0x0043fcfc..0x0043fed2`). Writing that order costs points: 4,f,2,8,3/9,15,0/5,1 gives 16.03%, and
  1,4,f,2,8,3/9,15,0/5 gives 16.08%. The decomp order stays. Open: what puts these cases after the bVar3 block.
- Re-reading `*(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40)` in place of `iVar8` in switch 1's tests (cases 0/5, 8, 0x15).
  It removes the jump threading in case 8 as in the original (`0x0043fd3c..0x0043fd9a` falls through every test), but scores
  -0.1 point (19.12%) because our `ecx` still holds `DAT_0067e9f8` from slice 2 where the original uses `ebp`. Worth
  retrying once slice 2's registers match.
- Switch 2/3 orders with 6/7 first or 4 last (30.88% / 31.56% on the int-flag base) do not reproduce the original layout.

## Hints for the other slices (no edit made there)

- Lines 1495-1509 (slice 2): the original loads `0x7f0f44..0x7f0f50` once (`0x0043f6fd..0x0043f71d`) and keeps two separate
  store blocks for `0x1f` and `0x21` (`0x0043f738`, `0x0043f752`). Ours merges the two ifs into `cmp 0x1f; je; cmp 0x21; jne`.
- The header score comment on line 2 still says 19.2%. Whoever merges the three slices should update it to the merged score.

## [UNCERTAIN]

- [UNCERTAIN] The original source lists the cases of switches 2/3 in the layout order above. This rests on our compiler
  laying out case blocks in source order, seen in every compile of this session, not on the original source. Test: a byte
  match of the region, or a minimal DDK test unit with permuted case order. One line in `re/orchestrator/UNCERTAIN_REGISTER.md`.

## Reproduce

`py -3.12 re/tools/match/match.py --bytes re/match/s08/0043dfd0.c` on this branch, and the same on
`git show 7630e53ca:re/match/s08/0043dfd0.c` for the before value. The int-flag scratch base replaces `bool bVar1..7` with
`int` and `true`/`false` with `1`/`0` in the function body.
