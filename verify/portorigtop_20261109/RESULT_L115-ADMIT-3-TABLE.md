# RESULT L115-ADMIT-3-TABLE (2026-11-10): 0x005aa060 needs no admission, it byte-matches (C2 -> C4)

One line: **all 4 differing bytes are explained by the register rotation eax->ecx, ecx->edx, but the rotation crosses
the function boundary (EAX at `ret`), and fixing the prototype that caused it makes the row BYTE-IDENTICAL.** It comes
off the admission list.

## 1. The per-byte table (the void source, as TL-MATCH-REST left it)

Script: `verify/l115admit3_20261110/regtable.py` (generalises the 0x0048e820 table to any register mapping; each
differing byte must differ only in a 3-bit register field that maps ours' register to the original's).
Output: `verify/l115admit3_20261110/table_005aa060_void.txt`.

`0x005aa060: 82 B whole symbol, 8 relocation-masked bytes, 4 differing bytes, instruction layout identical, mapping
ours->original: eax->ecx, ecx->edx`

| # | address | original byte | our byte | field | original instruction | our instruction | label |
|---|---|---|---|---|---|---|---|
| 1 | 0x005aa09e | `4c` | `44` | bits 3-5 (ModRM reg) | mov ecx, [esp+0x30] | mov eax, [esp+0x30] | explained by register eax->ecx |
| 2 | 0x005aa0a5 | `c9` | `c0` | bits 0-2 + 3-5 | test ecx, ecx | test eax, eax | explained by register eax->ecx |
| 3 | 0x005aa0a9 | `54` | `4c` | bits 3-5 (ModRM reg) | mov edx, [esp+0x18] | mov ecx, [esp+0x18] | explained by register ecx->edx |
| 4 | 0x005aa0ad | `11` | `08` | bits 0-2 + 3-5 | mov [ecx], edx | mov [eax], ecx | explained by register eax->ecx, ecx->edx |

relocheck (void source): `0x005aa060 OK checked=2 unchecked=0`.

## 2. Boundary check: EAX crosses it

- Arguments are on the stack and ecx / edx are caller-saved scratch, so those two do not cross.
- **EAX does.** In the original, EAX at `ret` still holds 0x005aa0c0's return value, because the param_3 store uses
  ecx / edx. In ours (void), EAX is overwritten with param_3.
- Every one of the 18 call sites of 0x005aa060 reads EAX right after the call:
  - `push eax`: 0x0046284c, 0x005a7fba, 0x005a8000, 0x005afd60, 0x005b85ba, 0x005b8600, 0x005bb85f, 0x005c6bfa,
    0x005c6c40, 0x005c80c0, 0x005c8673, 0x005c923f;
  - `test eax, eax`: 0x005ac3f7, 0x005bb8c5, 0x005c7d87;
  - `mov [esp+0x10], eax`: 0x005afa30;
  - `cmp edx, eax`: 0x005c87ca;
  - `cmp [ecx], eax`: 0x005c88ce.
- So the void source was a wrong prototype, and the register difference was its symptom. It was not a register
  allocation choice.

## 3. The fix: BYTE-IDENTICAL

`re/match/s25/005aa060.c` now declares `int FUN_005aa060(...)` returning `r = FUN_005aa0c0(...)` (and 0x005aa0c0 as
returning int):
- `match.py --bytes`: `100.0% (regs 100.0%, ops 100.0%) 27/27 ins`, `82 B whole symbol, 0 differing unmasked bytes ->
  BYTE-IDENTICAL`;
- relocheck: `OK checked=2 unchecked=0`;
- promoted C2 -> C4 with `c4_reclass.py batch --apply --strict` (route match).

## 4. Lesson for the register-only class

A register-only residual can be a prototype error when a register that differs is EAX (or EDX:EAX) at `ret`. Before
any admission request, check the return register against the callers. 0x0048e820 (L-118) is not affected: its swap is
ESI <-> EDI, both callee-saved, and the function is `void(void)` with the pair restored before `ret`.

rva-lint: OK.
