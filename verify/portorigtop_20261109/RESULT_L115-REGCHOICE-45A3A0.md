# RESULT L115-REGCHOICE-45A3A0 (2026-11-10): every differing byte of 0x0045a3a0 follows from the register choice; 0 "other"

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/L115-REGCHOICE-45A3A0` on
`orch/HB-CHANGELOG-MARKER` `67b607ab3`. Text only, nothing in `mashedmod/`. 0x0045a3a0 stays the logging stand-in
(POWERUPINIT-C) until L0 decides.

## Inputs

- **Source:** `re/match/s09/0045a3a0.c`, unchanged.
- **Command:** `py -3.12 re/tools/match/match.py re/match/s09/0045a3a0.c --bytes --show` -> `81.0% (regs 90.5%, ops
  90.5%) 40/42 ins`, `117 B whole symbol, 32 differing unmasked bytes (first at +0x1)`.
- **Compiler:** XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C.

The `--bytes` count is positional. It compares byte k of ours with byte k of the original, so every byte after a
length change counts, even when the instruction is the same. To account for each byte, a second pass aligned the two
instruction streams (capstone on the obj symbol and on MASHED.exe.unpatched 0x0045a3a0..0x0045a412, paired with
difflib on mnemonic + operands, jump targets as function offsets, the two relocated `edi` immediates as `<abs>`). It
then compared the raw bytes of every aligned pair, relocation sites masked. Script output: ours 117 B / 40
instructions, original 115 B / 42 instructions.

## Result in one line

- **Aligned pairs:** all 32 have identical bytes (0 pairs with a byte difference), and that includes the three rel8
  loop jumps.
- **Unpaired instructions:** 8 on the original side and 6 on ours, and every one is the register choice or one of
  its knock-ons.
- **Length:** the difference is +2 B net (ours 117, original 115).

## Full diff table

Columns: "orig" is the address in MASHED.exe.unpatched, "ours" the offset in the obj symbol, "shift" = ours - (orig
- 0x0045a3a0).

| # | orig | ours | original bytes / instruction | our bytes / instruction | label | why |
|---|---|---|---|---|---|---|
| 1 | 0x0045a3a1 | (none) | `55` push ebp | (none) | explained | Prologue save of the callee-saved EBP, needed only because the original keeps param_1 in EBP. Ours never touches EBP. |
| 2 | 0x0045a3a2 | (none) | `8b 6c 24 0c` mov ebp, [esp+0xc] | (none) | explained | The original's load of param_1 into EBP at entry. After push ebx and push ebp, esp+0xc is entry+4, the param_1 slot. Ours loads it later, row 8. |
| 3 | 0x0045a3a6..0x0045a3ad (4 ins) | +0x01..+0x08 | push esi, push edi, mov edi imm32, xor ecx, ecx | same bytes | explained (shift -5) | Identical bytes, 5 bytes earlier in ours, because rows 1 and 2 (1 + 4 B) are absent. The imm32 at ours +0x04 is a relocation: addend 0x1a6c + DAT_0068bd00 = 0x0068d76c, the original's value. |
| 4 | 0x0045a3af | +0x0a | `90` nop | `8d 9b 00 00 00 00` lea ebx, [ebx] | explained | Padding to the 16-byte-aligned outer-loop head at +0x10. The original's code before it ends at +0x0f, so it needs 1 pad byte. Ours ends at +0x0a, 5 B earlier (row 3), so it needs 6, and cl emits a 6-byte no-op lea. Both run once, falling through into the loop. Neither changes a register value (lea writes EBX's own value back) or the flags. |
| 5 | 0x0045a3b0..0x0045a3fd (24 ins) | +0x10..+0x5d | the three-level loop: sub/mov/lea/dec, the ten field stores through EAX, jne x3, cmp edi imm32 | same bytes | equal (shift 0) | The alignment in row 4 puts both loops at +0x10, so every byte matches, rel8 jumps included. The cmp imm32 at ours +0x58 is a relocation: 0x61c + 0x0068bd00 = 0x0068c31c, the original's value. |
| 6 | 0x0045a3d0..0x0045a3ee | (in row 5) | stores `[eax-4..eax+0x2c]` | same | equal | The loop's only stores, all into the global table through EAX (derived from 0x0068d76c). No store in the body targets the stack, so the param_1 slot is unchanged when ours reads it (row 8). |
| 7 | 0x0045a3fe, 0x0045a3ff | +0x62, +0x63 | pop edi, pop esi | same bytes | explained (shift +4) | Identical bytes, 4 bytes later in ours, because row 8 (4 B) comes before them. |
| 8 | (none) | +0x5e | (none) | `8b 44 24 10` mov eax, [esp+0x10] | explained | Ours reloads param_1 after the loop. After push ebx, esi, edi, esp+0x10 is entry+4, the same slot row 2 reads, and it is unchanged (row 6). EAX is free here: the loop's last use of EAX is the store at 0x0045a3ee. |
| 9 | 0x0045a400 | +0x64 | `89 4d 00` mov [ebp], ecx | `89 08` mov [eax], ecx | explained | Same store, same value (ECX = 0 from row 3's xor), same address (EBP = EAX = param_1). [ebp] with no displacement must be encoded with a zero disp8 (mod=01), because mod=00 with rm=101 means disp32. [eax] has no such rule, so ours is 1 B shorter. |
| 10 | 0x0045a403 | +0x66 | `89 4d 14` mov [ebp+0x14], ecx | `89 48 14` mov [eax+0x14], ecx | explained | Same store. Only the ModRM base-register field differs (rm 101 vs 000). |
| 11 | 0x0045a406 | +0x69 | `89 4d 18` mov [ebp+0x18], ecx | `89 48 18` mov [eax+0x18], ecx | explained | As row 10. |
| 12 | 0x0045a409 | +0x6c | `c7 45 0c 00 00 80 3f` mov [ebp+0xc], 0x3f800000 | `c7 40 0c 00 00 80 3f` mov [eax+0xc], 0x3f800000 | explained | As row 10. Same immediate, 1.0f. |
| 13 | 0x0045a410 | (none) | `5d` pop ebp | (none) | explained | Epilogue restore matching row 1. |
| 14 | 0x0045a411, 0x0045a412 | +0x73, +0x74 | pop ebx, ret | same bytes | explained (shift +2) | The net length difference: -5 (rows 1-2) +5 (row 4 pad, 6 vs 1) +4 (row 8) -1 (row 9) -1 (row 13) = +2. |

Order of effects is the same in both: the same ten loop stores in the same order, then the same four param_1 stores in
the same order. The pops (row 7) only touch ESI / EDI, which both restore to the caller's values.

## Machine state after return

| | original | ours |
|---|---|---|
| memory | 4x5x5 table fields 0..6, 8, 11, 12 = 0; param_1[0] = [5] = [6] = 0, [3] = 0x3f800000 | the same stores |
| EBX, ESI, EDI, EBP, ESP | restored / untouched | restored / untouched |
| EAX | the last table record pointer (loop) | param_1 |
| ECX, EDX | 0, 0 | 0, 0 |
| flags | from the last `jne` path | the same (the reload and stores do not write flags) |

EAX differs. The function is `void` (re/match/s09/0045a3a0.c, and Ghidra's `void FUN_0045a3a0(undefined4 *param_1)`),
and EAX is caller-saved under cdecl. Its caller on this path, the matched 0x0045a6b0, does not read EAX after the call:
re/match/s29/0045a6b0.c continues with `r->pool = FUN_004892c0(...)`, and at 0x0045a746 the original reloads
`mov ecx, [0x68d150]` and pushes arguments.

The other difference is the 4-byte saved-EBP slot the original writes below ESP during the call. That is dead stack
under the caller's ESP, and no code reads it after the return.

## Verdict

0 "other" differences. Every one of the 32 positional byte differences, and every unpaired instruction, is either:
- the choice of EBP (loaded at entry) versus EAX (reloaded after the loop) for param_1, or
- a knock-on of that choice: EBP save and restore, the 5-byte shift, the loop-alignment pad length, the [ebp] zero-disp
  encoding, the +4 / +2 shifts.

The only observable state difference is EAX at return, which a void cdecl function does not define. So the function is
a candidate for a per-function L0 decision admitting this register-choice residual under L-115. Until then nothing
changes: 0x0045a3a0 stays the logging stand-in in Match/PortTop_0045a6b0.cpp.

## Checks

- `py -3.12 scripts/lint_rva_bodies.py`: OK, 0 new.
- No `mashedmod/` change.
