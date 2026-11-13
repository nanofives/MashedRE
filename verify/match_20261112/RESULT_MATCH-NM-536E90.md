# RESULT MATCH-NM-536E90: compiler shaping for 0x00536e90 (2026-11-13)

**Verdict: RED.** Size row added. The source stays at 99.1% under both recorded compilers, with one
instruction scheduled differently. No promotion TSV (nothing reached 100%).

Kind: match-only (no build, no gates). Model: Opus 5.5 high.
Source: `re/match/rwl03/00536e90.c` (unchanged body; this note is the record of the attempt).
Previous attempt: `verify/match_20261112/RESULT_MATCH-RWT-B.md` lines 60-71.

## 1. `re/console/pc_funcs.csv` size row (committed)

`0x00536e90,0x00536fd9,330,FUN_00536e90`, inserted in address order after `0x00536660`.

Evidence, capstone over `original/MASHED.exe.unpatched` (the anchored binary `match.py` reads):
- `0x00536e87..0x00536e8f`: nine `90` (padding after the previous function's `ret` at `0x00536e86`).
- `0x00536e90`: `83 ec 20` `sub esp, 0x20` (entry).
- `0x00536fd9`: `c3` `ret`, the only exit (the `jle` at `0x00536ebc` targets `0x00536fd4`, inside the body).
- `0x00536fda..0x00536fdf`: six `90`. `0x00536fe0`: `83 ec 34` `sub esp, 0x34` (next function).
- Size 0x14a = 330 B, which agrees with the transient `decomp_pc.py --create` size in RESULT_MATCH-RWT-B.md line 64.
- `body_end` is the last byte (inclusive), as in every other row (`0x00401000,0x004011e1,482`).

With the row, `match.py` and `c4_reclass.py` no longer refuse the function for "no Ghidra size".

## 2. Score

| toolchain | flags | score |
|---|---|---|
| ddk (cl 13.00.9176 / c2 13.00.9178) | `/O2 /Ob1 /G5` (file) | 99.1% (111/111 ins, regs 99.1%, ops 99.1%) |
| rw9044 (VC6 SP5 cl 12.00.8804 + PP c2 13.00.9044, L-131) | `/O2 /Ob1 /G5` | 99.1%, same residue |

Residue: one load, `mov ebp, dword ptr [esp + 0x14]` (the position stride reloaded from its spill slot), in the
4th vertex. Original `0x00536faa`: after `fadd dword ptr [ecx + 4]`, before `fstp dword ptr [eax + 4]` (between the
vertex-4 y and z FP chains, before the y store). Recompiled: after `fsub dword ptr [esp + 0x2c]`, before
`fadd dword ptr [ecx + 8]` (after the y store). Every other instruction, register and stack offset is identical.

## 3. What was tried (all at 99.1% or lower, none moved the load)

About 300 hand variants, each scored under ddk and/or rw9044 (harness in the session scratchpad, not committed):
- prologue order of the four stride/pointer loads: 23 permutations (best 99.1%, others 94.6-98.2%)
- declaration order: 120 random permutations of the 11 locals, both toolchains: 244/244 at 99.1%
- loop forms: `for (; n > 0; n--)`, `while`, `do/while` with guard (56.8%), down-counting `i` (99.1%),
  up-counting `i` (ddk unrolls it, 768 ins; rw9044 99.1%)
- `p` advanced directly with no `pos` (99.1%), `pos += pstride` before vertex 4 / after x / after y (60-63%,
  register allocation changes), after the `v` advance (99.1%)
- corner temporaries `c = -r - u` (76.5%), `w`/`h` inlined (65.2%), commuted `w * x` products (99.1%)
- strides in a local struct `{s, p}` (99.1%) or `{p, s}` (96.4%), in an array (99.1%), address-taken (99.1%)
- RenderWare-style `{data, stride}` lock-struct copies for pos/size/vertex (29.7-58.6%)
- flags (with `MSYS_NO_PATHCONV=1`; without it Git Bash rewrites `/O2`): /G3 /G4 /G5 /G6 /G7 /GB, /Ox, /Og /Oi /Ot /Oy,
  /Oa, /Ow, /GS, /QIfdiv-, /Ob2, plain /O2 /Ob1: never above 99.1%; /Op 84.2%, /Oy- 57.1%, /Os 36.0%
- `match_permute.py` under rw9044, `--rounds 4 --walk 150 --jobs 8`: 1507 compiles, no improvement
  (the earlier attempt ran 667 under ddk)

Observation from probes (source edits that change the vertex-4 statements): the recompiled reload always lands one
FP instruction before the instruction that uses it (`add ecx, ebp`), whatever the statement order. In the original
it sits four FP instructions earlier. No source form found here produces that placement under either c2.

## 4. Not done

- No `re/promotions/MATCH-NM-536E90.tsv` and no `c4_reclass.py batch` dry run: the function is not byte-identical.
- 0x00536e90 stays `NOT-PROVEN` in `re/match/PROVISIONAL.tsv` (L-144: the executed RW tail stays strict).
  `equiv.py` was not run (out of scope for this row).
- Sibling Ghidra-missed functions 0x00536780 (443 B) and 0x00536fe0 (528 B) still have no `pc_funcs.csv` row.

## Next

Either an `equiv.py` admission run for 0x00536e90 (one scheduling difference of a stack reload, no value change),
or park the row until another c2 build turns up.
