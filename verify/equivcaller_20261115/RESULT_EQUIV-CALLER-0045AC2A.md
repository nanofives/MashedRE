# RESULT EQUIV-CALLER-0045AC2A (run 48, 2026-11-15)

Row kind: match tool + table. L-161 (b): no manual admission of 0x0045a3a0; make the caller 0x0045ac2a known to
`re/tools/match/equiv.py`'s caller scan so equiv.py decides. No builds, no gates (tool-only row).

## 1. Verdict

equiv.py on this branch (`re/match/s09/0045a3a0.c`, source commit 261f66a01, flags `/O2 /GS /Ob1`, DDK toolchain):

```
0x0045a3a0	EQUIV-IF-CALLERS-IGNORE(EAX)	[ENC,FRAME,VALUE-FLOW]	callers 3, scan says ignore: 3/3: 0x0045a741:ignores 0x0045a7d9:ignores 0x0045ac2a:ignores
```

(`equiv_0045a3a0.txt`, JSON `equiv_0045a3a0.json`: mode frame, rules `A7-stack-image, jmp-threading`, no local
callees, no halts, `callers_all_ignore: true`, bytes_bad 32.) Before this row (equiv.py @ fea5b5483, PROVISIONAL.tsv
line 931) the same line read `scan says ignore 2/3 ... 0x0045ac2a unknown`.

Every caller is scanned as ignoring and no x87 class is involved, so under L-126 (2) this is an admission with no
per-function decision: ADMITTED.tsv row added. Not C4 by this status (L-126, L-128).

## 2. What changed in equiv.py

The old scan walked forward from the call at 0x0045ac2a and returned `unknown` at the `ret` at 0x0045ac3b, because
EAX is still the callee's value there and FUN_0045ac20 has no direct caller (`callers(0x0045ac20) == []`).

New: `INDIRECT_CALLERS` (entry -> end + table routes) and `indirect_sites()`. At a `ret` inside a listed function the
scan continues after each of that function's call sites: direct E8 sites plus each registered indirect site, with the
instruction length of that site (`caller_reads(..., ilen=)`). Verdict = `reads` if any site reads, `unknown` if any is
unknown, else `ignores`; depth capped at 4 as before. `indirect_sites()` checks every route against the image before
use and raises on a mismatch:

- dword at `slot` 0x005f9abc == 0x0045ac20,
- `slot` = `slot_base` 0x005f99bc + k * `stride` 0x40 with 0 <= k < the count dword at 0x005f9bd8 (9); here k = 4,
- `load` 0x0045b9dd is `mov edi, 0x5f99bc`,
- `site` 0x0045b9e2 is `call dword ptr [edi]` (2 bytes).

Test `test_indirect_callers` in `re/tools/match/test_equiv.py` pins the route, the 0x0045ac2a verdict, and that a
wrong slot (0x005f9ab8) is refused.

## 3. The caller chain (disassembly of MASHED.exe.unpatched, capstone)

FUN_0045ac20 (0x0045ac20..0x0045ac3b, 28 bytes; not in `re/console/pc_funcs.csv`, the Ghidra function list):
```
0045ac20 push esi
0045ac21 mov esi, 0x68bd00
0045ac26 sub esi, 0x68
0045ac29 push esi
0045ac2a call 0x45a3a0
0045ac2f add esp, 4
0045ac32 cmp esi, 0x68bb60
0045ac38 jne 0x45ac26
0045ac3a pop esi
0045ac3b ret
```
EAX is not read inside it, so on the last iteration EAX from 0x0045a3a0 is FUN_0045ac20's return value.

Table: records of 0x40 bytes from 0x005f9998, first dword an id (9, 0xa, 0xb, 0xc, 0x10, 0x11, 0x12, 0x13, 7), then 15
function pointers; count dword 0x005f9bd8 = 9. 0x0045ac20 sits at 0x005f9abc = record 4 (id 0x10 at 0x005f9a98) +0x24.
A search of every section for the dword 0x0045ac20 finds only 0x005f9abc (.data).

Readers of the table (every dword in .text in [0x005f9990, 0x005fa400)): slot-column loads `mov edi, 0x5f99XX` at
0x0045b957 (+0x18), 0x0045b9a8 (+0x20), 0x0045b9dd (+0x24), 0x0045bb45 (+0x14), 0x0045bdf2 (+0x1c), 0x0045be0c
(+0x3c, esi), 0x0045be9d (+0x28), 0x0045bf04 (+0x2c), 0x0045bf76 (+0x30); the count reads of 0x005f9bd8; and the base
0x005f9998 at 0x0045baac. Only 0x0045b9dd touches the +0x24 column:

FUN_0045b9d0 (pc_funcs.csv: 0x0045b9d0..0x0045b9f3, 36 bytes):
```
0045b9d0 mov eax, dword ptr [0x5f9bd8]
0045b9d5 push esi
0045b9d6 xor esi, esi
0045b9d8 test eax, eax
0045b9da jle 0x45b9f2
0045b9dc push edi
0045b9dd mov edi, 0x5f99bc
0045b9e2 call dword ptr [edi]
0045b9e4 mov eax, dword ptr [0x5f9bd8]     <- EAX written before any read: ignores
0045b9e9 inc esi
0045b9ea add edi, 0x40
0045b9ed cmp esi, eax
0045b9ef jl 0x45b9e2
0045b9f1 pop edi
0045b9f2 pop esi
0045b9f3 ret
```

The base route 0x0045baac is the id lookup FUN_0045baa0 (returns the record or 0). Its only caller 0x0045bfa5 stores
the record at obj+0xa8 and calls `[eax+4]` (0x0045bfb5). A scan of every `mov r32, [r32+0xa8]` (disp32 form) in .text
followed within 14 instructions by an indirect call finds only `[eax+0x10]` at 0x0045bac7 and `[eax+0x48]` at
0x0041caab / 0x0041cabf (0x48 is past a 0x40 record). No `+0x24` call through a stored record pointer was found.

[UNCERTAIN] Completeness of the indirect route list rests on these static scans. Not covered: +0xa8 loads in SIB or
disp8 form, a record pointer copied to another field or local before the call, and computed table addresses with no
dword of the table range in .text. Test that would close it: a Frida return-address sample on 0x0045ac20 over a boot to
a race (one function, short run, per the hot-path rule) showing every return lands at 0x0045b9e4.

## 4. Cross-check (equiv_dyn.py, Unicorn, EAX excluded as the verdict lists it)

| mode | same | inconclusive | differs |
|---|---|---|---|
| default, 32 runs | 32 | 0 | 0 |
| `--seeded`, 32 runs | 32 | 0 | 0 |

(`dyn_default.tsv`, `dyn_seeded.tsv`, input `dyn_input.tsv`.) Run with `--only` and no ignore list, both modes report
32/32 differs on EAX only (first: `eax 0x4253050b | 0x0068bd04`), which is the EAX difference the verdict names.

## 5. Tests

`py -3.12 -m pytest re/tools/match/test_equiv.py -v`: see `test_equiv.txt` : 5 passed (test_cases, test_references, test_indirect_callers, test_summary_precision, test_budgets).

## 6. Tracker state

- `re/match/ADMITTED.tsv`: new row for 0x0045a3a0 (tool commit ca7473532).
- `re/match/PROVISIONAL.tsv` line 931 (DDKOBJ-UNLISTED-10) left unchanged: the train re-checks the verdict on the merged
  equiv.py; whoever applies the admission retires the provisional row. hooks.csv untouched (still C2, never C4 by this).
- No Ghidra write: the registry carries FUN_0045ac20's range and the tool checks the bytes, so no function had to be
  created. 0x0045ac20 is still absent from pc_funcs.csv.
