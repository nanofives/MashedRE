# RESULT RWC-DRIVER (L-124 C): RW D3D9 device layer, match pass

Row: RWC-DRIVER, branch `orch/RWC-DRIVER`, worker claude3 (Opus 5.5 high), 2026-11-11.
Scope: the 48 executed RW functions that make COM calls (verify/rwsizing_20261111/RW_FUNCS.tsv, `d3d_com` set, phase not
`not-run`, tracker row present). 25 were C4 by match before this row. This row worked the other 23 (22 with sources, plus
0x004c7a70 which had none). Compiler XP DDK cl 13.00.9176 / c2 13.00.9178, C, `/O2 /Ob1 /G5` (RW core), `/Oy-` where noted.

## Verdict

- **6 functions matched and promoted to C4 by match** (c4_reclass.py batch, all PASS: byte-identical + relocheck OK, no
  `__asm`): 0x004c90a0, 0x004cb600, 0x004dc5b0, 0x004d5bc0, 0x004d6200, 0x00540340. 4,948 B (tracker sizes).
  The driver is now 31 of 48 C4. Unmatched bytes go from 23,030 to about 18,082.
- **0x004c7a70 `_rwDeviceSystemFn`** (2,810 B, never attempted): new source, 93.2%.
- **0 admitted under L-126.** equiv.py returns NOT-PROVEN on every function of this driver, the byte-identical ones
  included (see "equiv.py and COM calls"). So no ADMITTED.tsv rows. The unmatched sources are provisional candidates.
- No build, no gates: match-only row (WORKER_RULES "Match-only, tracker and text rows").

## Per function

Sizes are the tracker body sizes. Before = score at the start of this row (main checkout sources); after = score
on `orch/RWC-DRIVER`. equiv = `re/tools/match/equiv.py` verdict on the final source (full lines:
`final_scores_equiv.tsv`, baseline: `equiv_baseline.txt`).

| rva | size | before | after | status | residual / note | equiv |
|---|---|---|---|---|---|---|
| 0x004c90a0 | 458 | 68.5% | 100% | **C4 match** | 0x14-byte record struct, non-NULL realloc branch first, formats 0x1a..0x22 as their own switch group (byte-index jump table), search loop breaks out then appends/updates the max | NOT-PROVEN switch-index |
| 0x004cb600 | 235 | 78.6% | 100% | **C4 match** | int return instead of bool, the 0x004cb740 check as an empty `== 0` arm with `else return 1` | NOT-PROVEN ret-esp |
| 0x004dc5b0 | 403 | 80.7% | 100% | **C4 match** | uint fields, tail as `static __inline` helper, list stores reordered (+0xc then +0x10) | NOT-PROVEN ret-esp |
| 0x004d5bc0 | 1594 | 88.6% | 100% | **C4 match** | the five zero-stored globals 0x007d6c24/6c28/6c00/6c04/6c08 typed `float`, stage-cache fill as counted do-whiles over an `int*`, 0x007d6c24 store after the 0x007d5120 memset | no verdict within 300 s |
| 0x004d6200 | 1805 | 90.5% | 100% | **C4 match** | same float-global lever, 0x007d6c24 store after the 0x007d5120 zero loop, 0x007d6c08 store next to 0x007d6c04 | no verdict within 300 s |
| 0x00540340 | 453 | 99.4% | 100% | **C4 match** | no named lookup temp: the record lookup written at each use (macro), `piVar3` assigned in its own `if`; the compiler merges the lookups and emits `mov ebx, eax; mov esi, [ebx]` | NOT-PROVEN ret-esp |
| 0x004d1640 | 1734 | 92.4% | 99.8% | provisional candidate | one opcode: `add esi` vs the original's `or esi` (16-bit early return written `format += 0x200`, same value since those bits are zero there); every `|=` form is tail-merged (91.7%) | NOT-PROVEN ret-esp |
| 0x004d1e30 | 1269 | 99.1% | 99.1% | provisional candidate | case 5 schedule: original runs `not/shr/and` on edi after the argument pushes. ~40 forms + ~2,000 permuter compiles | NOT-PROVEN ret-esp |
| 0x004d2980 | 757 | 53.3% | 97.1% | provisional candidate | rewritten with typed structs and `__stdcall` device calls, palettised errors `goto` one shared tail. Residual: original pushes each message string before the jump, ours `mov eax, imm` + one `push eax`; plus one register pick. Lead: `&width`/`&height` taken into locals at entry gives the per-message push (95.8%) but moves two `lea` to entry | NOT-PROVEN call-regargs |
| 0x004d7480 | 1478 | 96.2% | 96.2% | provisional candidate | eax/ecx/edx assignment in cases 0x10/0x11 (fog type, fog density) | NOT-PROVEN switch-table |
| 0x004db3e0 | 367 | 95.2% | 95.2% | provisional candidate | block layout: original returns esi through a duplicated tail | NOT-PROVEN call-regargs |
| 0x004c9eb0 | 146 | 94.8% | 94.8% | provisional candidate | prologue: `push ebx` at entry, `xor ebx, ebx` inside the branch. best=0 inside the if sinks all three pushes (91.4%) | NOT-PROVEN call-target |
| 0x004c7a70 | 2810 | none | 93.2% | provisional candidate | new source re/match/rwd01/004c7a70.c. Residual: case 0 NULL return shares the Release tail (store of 0x007d4108 not between the pops, DAT_007d410c re-read from memory); case 2 DevCaps (0x00911fbc) loaded before the 0x007d4138 branch in the original; case 0x11 `or ah, 0x80` vs `or eax, 0x8000` (every C form gives the byte form under 9176); cases 5/6 register picks. Most of the gap is jump displacements to the default `return 0` at 0x004c855f shifting with these | NOT-PROVEN unsupported `stmxcsr` |
| 0x004cb4c0 | 319 | 90.2% | 90.2% | provisional candidate | pointer loop gives the down-counter, but ours loads both operands (extra `push ebx`) where the original uses `cmp esi, [eax]` | NOT-PROVEN branch-condition |
| 0x004dba60 | 533 | 87.6% | 87.6% | provisional candidate | ecx/edx swapped throughout; swapping the two camera offsets fixes the float slots but breaks the tail (82.6%) | NOT-PROVEN switch-table |
| 0x004d0290 | 631 | 73.4% | 76.4% | provisional candidate | cases end in `break` with one `return 1`. Residual: original saves ebp/edi at entry, ours delays them past the early `cpPixels` error exit | NOT-PROVEN call-args |
| 0x004cb8a0 | 479 | 43.3% | 71.3% | provisional candidate | element/cache structs, every COM call `__stdcall` (the old cdecl typing was wrong). 97% of instructions identical up to registers; residual is the register for the global table pointer | NOT-PROVEN call-regargs |
| 0x004cb330 | 343 | 18.6% | 64.9% | stuck (frame) | the original has an ebp frame (`push ebp; mov ebp, esp`, one spilled counter at [ebp-4]); nothing in the body forces it (no alloca, array, setjmp, `__try`). `/Oy-` on the FLAGS line (as s20/004cb4c0) gives 64.9% vs 56.1%. Residual: ours keeps param_2 in ebx, the original never uses ebx | NOT-PROVEN indirect-call-esp-reset |
| 0x004cff20 | 841 | 62.5% | 62.5% | provisional candidate | behaviour fix kept (both error blocks stored `pluginID = 1` after the error call, so the compiler dropped the store; now first, as in the original), ops 91.9 -> 92.6%. Same entry-save residual as 0x004d0290 | NOT-PROVEN call-args |
| 0x005412d0 | 2160 | 44.2% | 55.6% | stuck (asm) | full rewrite from the disassembly. The original converts the alpha float with `fistp` (RenderWare inline asm, round to nearest); C without `__asm` gives `__ftol` (truncation). No C form reaches 100%; values differ for non-integral alpha | NOT-PROVEN call-sequence |
| 0x004e0920 | 577 | 20.8% | 46.3% | provisional candidate | element tables as a local 5 x 8-byte array filled field by field; `result = 1` set at the start of the success branch. Residual: original keeps 1 in ebp and 0xff in esi, which shifts every stack offset on the success path (96.6% of instructions identical up to that) | NOT-PROVEN call-args |
| 0x005405c0 | 3339 | 23.8% | 23.8% | not attempted (budget) | raw g2s source | NOT-PROVEN call-args |

The 25 already C4 before this row are unchanged (RW_FUNCS.tsv `mstate=C4-match`).

## equiv.py and COM calls (why nothing was admitted)

`equiv.py` models a call through a pointer as cdecl: rule A4, `re/tools/match/equiv.py:1458` ("a function-pointer call
pops nothing"), pop count `P = 0`. Every D3D9 method is a `__stdcall` vtable call, so the callee pops its arguments, no
`add esp` follows, and the modelled ESP at the return is off by the argument bytes on both sides. The check at
`equiv.py:2230` then refuses with `ret-esp: esp = esp0-N at ret`. Witness: 0x00540340, 0x004cb600 and 0x004dc5b0 are
byte-identical (c4_reclass PASS above) and still come back `NOT-PROVEN ret-esp`. 0x004c9ad0 (already C4) proves only
through the BYTE-IDENTICAL short cut. So no COM-calling function that is not byte-identical can be admitted under L-126
today, whatever its source. The other refusals in the table (`switch-table`, `call-regargs`, `unsupported stmxcsr`)
are real differences or unmodelled instructions, behind the same wall.

Two ways forward, neither taken here (both change the proof rules):
- an equiv.py extension: an indirect call with no cleanup on either side and equal push counts gets `P = N` (callee
  pops what was pushed for it), checked equal on both sides. That is a rule change to L-126's tool: a decision row.
- the NOT-PROVEN sources above as PROVISIONAL bodies on the knob path (L-126 (6)), which this match-only row does not
  wire. No `--relax-icall` was used.

## Levers found (apply to the rest of this driver)

1. **Case bodies as `static __forceinline` helpers.** In 0x004c7a70 this let ebx be saved only inside case 0 (78 ->
   88%), case 2 as a helper storing to 0x007d4120 then returning gave 90.6, case 7 as a helper 92.0. Points at the RW
   driver being compiled with static-function inlining. [UNCERTAIN] whether that means a different /Ob level: a
   `--flags` override did not take effect over the file's FLAGS line, so /Ob2 was not measured.
2. **Per-case scopes for stack locals.** The original shares stack slots between cases (multisample counter and MXCSR
   temp, frame 0x474 -> 0x470).
3. **Switch results through a temp**, stored after the switch, not the destination directly (63.6 -> 72.8 on
   0x004c7a70).
4. **Float-typed zero-stored globals** (0x007d6c24/6c28/6c00/6c04/6c08): closed 0x004d5bc0 and 0x004d6200.
5. **COM calls typed `__stdcall`.** Several raw sources had them cdecl (0x004cb8a0 43 -> 71%).
6. **No named temp for a repeated lookup**: write the lookup at each use and let the compiler merge it (0x00540340).
7. **`/Oy-` for ebp-frame functions** in the 0x004cb330..0x004cb4c0 range (0x004cb4c0 and 0x004cb330 both have frames).
8. `memset(&x, 0, 16)` reproduces `xor edx, edx` + four stores (0x004c7a70 case 2).

## Files

- New: `re/match/rwd01/004c7a70.c`.
- Changed sources: `re/match/rwl01/{004c90a0,004cb600,004dc5b0,004d2980,004cb8a0,004e0920,004cb330}.c`,
  `re/match/rwl03/00540340.c`, `re/match/rwl04/005412d0.c`, `re/match/s20/{004d0290,004cff20,004d1640}.c`,
  `re/match/s21/{004d5bc0,004d6200}.c`. Unchanged near-misses keep their old text.
- Evidence: `c4_batch.tsv` (the batch input), `equiv_baseline.txt`, `final_scores_equiv.tsv`.
- Trackers: hooks.csv + re/analysis/CHANGELOG.md through `c4_reclass.py batch --apply` (6 rows).

## Follow-ups

- RWC-DRIVER-2 (match): 0x004d1640 (99.8), 0x004d1e30 (99.1), 0x004d2980 (97.1, entry-`lea` lead), 0x004d7480,
  0x004db3e0, 0x004c9eb0, 0x004c7a70 (93.2), 0x005405c0 (not attempted). Try /Ob2 per source for the
  `__forceinline` evidence.
- equiv.py callee-pops extension for `__stdcall` indirect calls (decision row, rule change).
- 0x005412d0 (fistp) and 0x004cb330 (frame without a body cause): stay unmatched; port as provisional bodies with the
  fistp rounding written out, or file per L-126.
- 8 COM-calling functions with no census row (RW_FUNCS.tsv `NOROW`: 0x004c9290, 0x004ca160, 0x004cab30, 0x004cac10,
  0x004d05b0, 0x004dd660, 0x00536660, 0x00543500) are outside the 48 and were not worked.
