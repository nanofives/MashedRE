# RESULT MATCH-RELOC-521520

Row kind: match-only (no build, no gates). Route: `c4=match` (L-93).

## Cause
`re/match/s23/00521520.c` (zlib 1.1.3 `deflate`) was byte-identical under match.py but relocheck reported BAD:
the two callee declarations and call sites carried swapped names.

- before: `+0x286 _FUN_00528ef0: ours 0x00528ef0 vs orig 0x00529050; +0x297 _FUN_00529050: ours 0x00529050 vs orig 0x00528ef0`
- 0x00529050 is `_tr_align` (1 arg, `flush == 1` branch, call at +0x286). Its own source `re/match/s23/00529050.c` is `void FUN_00529050(deflate_state *s)`.
- 0x00528ef0 is `_tr_stored_block` (4 args, call at +0x297). Its own source `re/match/s23/00528ef0.c` takes `(s, buf, stored_len, eof)`.

Fix: swapped the two prototypes and the two call sites in 00521520.c (4 lines), and replaced the stale 84.5% header
comment. No other source changed. Compiled bytes are unchanged apart from the relocation targets, which match.py masks.

## Scores (after the fix)
```
py -3.12 re/tools/match/match.py re/match/s23/00521520.c --bytes
0x00521520 _FUN_00521520: 100.0% (regs 100.0%, ops 100.0%)  420/420 ins  flags=/O2 /Ob1 /G5 tc=rw9044
   bytes: 1121 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s23/00521520.c
0x00521520	OK	checked=7	unchecked=0	00521520.c
```
Toolchain `rw9044` (VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044, L-131), flags `/O2 /Ob1 /G5`.
Not an inline-asm transcription: plain C with `static __forceinline` helpers.

## Promotion
`py -3.12 re/tools/c4_reclass.py batch --tsv verify/matchreloc_20261112/batch.tsv` dry run:
`[1/1] 0x00521520 match PASS byte-identical 1121 B, relocheck OK (7/0)`, `1 pass, 0 refused`. Then `--apply --result` this file.
Rows promoted to C4 by `c4=match`: 1 (0x00521520, C2 -> C4). `c4=libid`: 0. `c4=frida-replay`: 0.

Commit SHA: 2e8c4cadc (source, hooks.csv and CHANGELOG change); a follow-up commit only records this line.
Gates: not run: match-only.
