# RESULT MATCH-EQUIV-536E90: equiv.py on 0x00536e90 (2026-11-13)

**Verdict: refused (NOT-PROVEN, x87-region).** No ADMITTED.tsv row. Not C4, no promotion TSV.

Kind: match-only (no build, no gates). Model: Opus 5.5 high.

## Run

- Tool: `re/tools/match/equiv.py` at commit `eee2fc626` (last commit touching it on `orch/integration` 145dec4d0).
- Source: `re/match/rwl03/00536e90.c` as on `orch/integration` (99.1% under ddk `/O2 /Ob1 /G5`, the source that
  `verify/match_20261112/RESULT_MATCH-NM-536E90.md` scored). Default toolchain ddk (no `// TOOLCHAIN` line).
- Size: the tool refuses without a `pc_funcs.csv` row (`0x00536e90	NOT-PROVEN	no-size: no Ghidra size` on this
  branch as is). The run used the row from `orch/MATCH-NM-536E90` @ `86fa81eaf`
  (`0x00536e90,0x00536fd9,330,FUN_00536e90`) in the working tree only. The file was restored afterwards, so this
  branch does not carry the row (that branch does, enqueued alone).
- Command: `py -3.12 re/tools/match/equiv.py re/match/rwl03/00536e90.c` (default mode, no `--relax-icall`).

Verdict line, verbatim:

```
0x00536e90	NOT-PROVEN	x87-region: region ours +0x32 / orig 0x00536ec2: integer code moved across x87 instruction 67 (fadd m4[ecx+0x4]): [] | ['mov']
```

## Why this is a refusal and not a decision row

`re/tools/match/equiv_rules.md` section 5 ("Excluded on purpose", x87): in a region with x87 instructions "No other
instruction may cross an x87 instruction". The only difference between the two bodies is that the stride reload
`mov ebp, dword ptr [esp + 0x14]` sits after `fadd dword ptr [ecx + 4]` in the original (`0x00536faa`) and three
x87 instructions later in ours (`+0x125`, before `fadd dword ptr [ecx + 8]`). The x87 instructions themselves are
the same text in the same order, but an integer instruction crosses x87 instructions, which this rule excludes on purpose.
`0x00556e90` is the rules file's own example of this exclusion. The worker rules state that x87 instruction order is
never admitted. So this is not a conditional verdict that a caller scan could resolve. No caller scan or Unicorn
cross-check was run, since both only apply to an EQUIV or EQUIV-IF-CALLERS-IGNORE verdict.

## State after this row

- 0x00536e90 stays `NOT-PROVEN` / provisional in `re/match/PROVISIONAL.tsv` (knob path only, L-126, L-144).
- Neither route to admission is open: the byte match is stuck at 99.1% under both c2 builds
  (RESULT_MATCH-NM-536E90.md) and equiv.py excludes the residue by rule.
- Changing the x87 region rule would be a decision row, not something to do on a worker branch.
