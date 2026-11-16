# RESULT MATCH-43DFD0-RAISE-1 (slice 1 of 3, source lines 26-679)

Row MATCH-43DFD0-RAISE, slice 1. Match-only (L-168): no build, no gate, no game run, the 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-RAISE-1` from `orch/integration` 7630e53ca. One attempt.

## Verdict

GREEN as a raise, no byte match. Whole-function score **19.2% -> 21.7%** (regs 31.7 -> 46.3%, ops 59.1 -> 67.8%).
No byte-identical range: `--bytes` first differing byte is still at +0x2 (the frame immediate, `sub esp, 0x28` here against
`sub esp, 0x2c` at 0x0043dfd0). That slot sits in slice-2 code (below), so relocheck was not run: there is no byte-identical claim.

## Measurements

Tool: `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c [--show|--bytes]`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178),
`/O2 /GS /Ob1`, C.

| State | Score | regs | ops | ins ours/orig | `--bytes` |
|---|---|---|---|---|---|
| before (7630e53ca) | 19.2% | 31.7% | 59.1% | 2829/2817 | 11106 B, 7009 differing, first at +0x2 |
| after (this branch) | 21.7% | 46.3% | 67.8% | 2808/2817 | 11134 B, 6968 differing, first at +0x2 |

Slice-1 address range. The source lines 26-679 cover the original from 0x0043dfd0 to the `switch (DAT_0067e9fc)` jump
`jmp [eax*4+0x440aac]` at 0x0043e8de. Some uVar10 cases in those lines (`LAB_0043f21d`, `LAB_0043f468`) sit at later addresses.
Over the original's instructions at 0x0043dfd0..0x0043e8e4, counted from the `--show` alignment: **196/611 equal before, 208/611
after** (32.1% -> 34.0%). The rest of the whole-function gain lands in slices 2 and 3, because the frame and flag-slot change
moves every later `[esp+n]`.

Step by step (each step on top of the one before):

| Lever (source lines in slice 1) | Score |
|---|---|
| start | 19.2% |
| `bVar2..bVar7` bool -> int | 17.4% (regs 46.5%) |
| `bVar1` bool -> int as well | 20.5% |
| store before compare in the eca4==1 fade, split `(a) \|\| (b)` flag test, loop reload | 21.3% |
| eca4==2 fade `>= 0` | 21.4% |
| scan loop `p = p + 1; x = *p;` | 21.6% |
| naked-RET definition of 0x0045b350 | 21.7% |

## What changed, with the original addresses

1. **Seven flags are dwords in the original.** Zeroed at 0x0043e3ff..0x0043e417 (`mov [esp+0x34/0x30/0x2c/0x1c/0x28/0x24/0x20], ebx`),
   set at 0x0043e43d..0x0043e48b (`mov [esp+n], edi`), and tested as dwords at 0x0043f6d4, 0x0043fc9a, 0x0043fed7, 0x0043fef9, 0x0043ff1f,
   0x00440457 and 0x004409c8 (`cmp dword ptr [esp+n], ebx`). The source had `bool` (unsigned char), which packed them as bytes
   (`esp+0x12..0x17`). Now `int bVar1..bVar7`. The frame went from 0x18 to 0x28 (the original has 0x2c).
2. **Fade-in (eca4 == 1):** 0x0043e0b8..0x0043e0cc is `mov ecx,[0x67eca8]; add ecx,eax; cmp ecx,0xff; mov [0x67eca8],ecx; jl`, so the
   store comes before the compare. Source: `DAT_0067eca8 = DAT_0067eca8 + iVar8; if (DAT_0067eca8 < 0xff) return;`.
3. **Fade-out (eca4 == 2):** `add dword ptr [0x67eca8], eax; jns` at 0x0043e112 / 0x0043e118. `if (DAT_0067eca8 >= 0)` gives it. `-1 < x` gave
   `cmp ecx,-1; jg`.
4. **Key-scan flag:** two separate `mov edx, edi; jmp 0x43e48f` blocks for 0xff100000 (0x0043e427) and 0xff230000 (0x0043e432), so two
   `if/else if` arms each set the flag. The `||` form merged them.
5. **Scan advance:** `mov eax,[ecx+ebp]; add ecx,ebp` at 0x0043e48f/0x0043e492 (ebp = 4). `piVar18 = piVar18 + 1; iVar8 = *piVar18;`
   gives `mov eax,[ecx+4]; add ecx,4`, the same apart from the register (see residual R3).
6. **Flag in edx across call 0x0045b350.** The original sets edx at 0x0043e3f8 / 0x0043e427 / 0x0043e432, calls 0x0045b350 at
   0x0043e49b and reads edx at 0x0043e4a3 (`cmp edx, ebx`) with no reload. 0x0045b350 is a single RET (`hooks.csv` row 0x0045b350,
   C4 by match). Measured on the DDK compiler:
   - a declaration only: the flag goes to a callee-saved register (esi, then ebp)
   - `void FUN_0045b350(void) {}` defined in the TU: flag in edx, but c2 drops the call (also with `__declspec(noinline)`, an
     address-taken copy, or `__asm { }`)
   - `__declspec(naked) void FUN_0045b350(void) { __asm ret }`: call kept, flag in edx, as in the original. A body with one volatile
     read does the same, but its bytes are not the original's RET.

   The source now defines the naked RET in the C branch only (`#ifndef __cplusplus`). The C++ include in
   `Match/ProvS08_0043dfd0.cpp` keeps the plain declaration, so the provisional exe body's stub mapping is unchanged. The naked
   function is a helper, not the matched body: 0x0043dfd0 itself stays plain C.
   **[UNCERTAIN]**: how the original translation unit knew that 0x0045b350 writes no register. 0x0045b350 is 0x1d380 bytes away
   from 0x0043dfd0, so the two are not neighbours in one object, and LTCG (/GL) is one candidate. The evidence is only this codegen
   reading. Test: the other 32 callers of 0x0045b350, checking whether any keeps a caller-saved register live across the call; then a
   /GL + /LTCG two-TU compile under the DDK. One line in `re/orchestrator/UNCERTAIN_REGISTER.md`.

Tried with no effect, not kept: `piVar18 = piVar18 + iVar9 + 1` rewritten as `iVar9++; piVar18 += iVar9` or as `&piVar18[iVar9 + 1]`
(same 21.4%); input block `x + -4` rewritten as `x - 4` or `x -= 4` (same 21.7%); `iVar8 = *++piVar18` (same as the kept form).

## Residuals (for the lead and slices 2 and 3)

- **R1, cross-slice: the uVar10 dispatch is a `switch`.** At 0x0043e503 the original runs a binary decision tree,
  `cmp edi, 0xff400000; ja 0x43f4e0; je 0x43f468; cmp edi, 0xff2f0000; ja; je ...`. The source has Ghidra's nested
  `if (uVar10 < 0xff400001) { if (uVar10 == 0xff400000)`, which gives `cmp 0xff400001; jae; cmp; je`. The tree spans source
  lines 514-1493 (slices 1 and 2), so no single slice can rewrite it. This is the largest shape lever left and it changes the
  block layout and register weights of everything after it.
- **R2, slice 2: the missing frame slot.** The original has 11 dword slots (frame 0x2c) and we have 10. The missing one is
  `[esp+0x38]`, written at 0x0043edec (`mov [esp+0x38], edi`) and read at 0x0043ee3c (`mov ebx, [esp+0x38]`). It sits in the
  slice-2 list-build loops. Once it exists, `local_20` should move from `[esp+0x18]` to the original's `[esp+0x1c]` (0x0043e40b,
  0x0043e47e, 0x0043e4ab), and `[esp+0x18]` becomes the slot of 0x0043e9b2 / 0x0043f626 (`local_24 = 6`).
- **R3, register webs in slice 1.** Original: ebx = 0; edi = 1 from 0x0043e03b, then uVar10 from 0x0043e4cf; esi = 2; ebp = 4 from
  0x0043e139 (`push ebp; mov ebp, 4` in the eca4 == 3 branch), then -1 from 0x0043e4a0. Ours: ebx = 0; ebp = 1, then -1; esi = 2,
  then uVar10; edi = 3 (`push edi; mov edi, 3`). The 16 `+4/-4` input stores use a scratch `mov eax, 4` instead of ebp, and so
  switch the key loads from `al` to `cl`. The source rewrites tried (above) did not move this. It is coupled to R1, since the
  weights come from the whole function's layout.
- The source header (line 2) still carries the 19.2% text. It is outside slice 1, so the lead updates it after merging the three
  slices.

## Files

- `re/match/s08/0043dfd0.c` sha256 `6cb291d0c4f8dcadba2bd085f6741e880d03c52010268af022b6b3e3595010c2` (edits only in lines 26-679 of
  the base file: declarations at 235, locals at 263-269, the fades at 317-333, the key scan at 470-505).
- `re/orchestrator/UNCERTAIN_REGISTER.md`: one line for item 6.
- No PROVISIONAL / ADMITTED / promotion change: the body is still NOT-PROVEN, far below 100%.
