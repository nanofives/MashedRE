# RESULT MATCH-NM-5441F0-4C4A50 (L-147 (a)): equiv.py re-run with the integer-motion-across-x87 rule (2026-11-13)

Row kind: match-only/tool row. No build, no gates, no game run. Sonnet 5.5 medium.

## Tool used

- `orch/integration` does NOT carry the tool yet (`git merge-base --is-ancestor a9ca480f1 orch/integration` fails).
  I merged `orch/EQUIV-X87-INTEGER-MOTION` (5c6ebac0d, tool commit a9ca480f1) into this branch with
  `git -c merge.autoStash=false` (merge commit b169e4d3d, no conflicts).
- Blob shas run against: `re/tools/match/equiv.py` b720231cb25f9ebc18e1cb2adebf2f1695733f1b,
  `re/tools/match/equiv_rules.md` 9195859b882ee26535139fae7595a0cbe70f876a.
  `orch/integration` equiv.py blob is 673dde1401a54b0c962d081fca057aaa76e5ede9 (older tool, without the rule). If the
  lead enqueues this row after EQUIV-X87-INTEGER-MOTION lands, the blob must equal b720231cb; otherwise re-run line 1 below.

## Verdicts

| rva | source | verdict (tool a9ca480f1) | admitted |
|---|---|---|---|
| 0x004c4a50 | re/match/s19/004c4a50.c (222d2d5a6, /O2 /GS /Ob1 /G5, rw9044) | **EQUIV [LAYOUT,VALUE-FLOW]**, rules A1-exposed, A6-paired-pop, A6-pinned-pop, A7-stack-image, jmp-threading, x87-barrier, x87-motion | **yes**, ADMITTED.tsv row appended |
| 0x005441f0 | re/match/rwtb/005441f0.c (99.1%) | no verdict: stopped by PID after 24 min (10:40 to 11:05), 787 CPU-s by 10:54, same as the earlier 50-min run | no |

### 0x004c4a50 checks

- Mode: default (no `--relax-icall`, no `--x87-strict`). Plain EQUIV, not conditional, so no caller assumption exists
  to scan (same as ADMITTED row 0x0041f770, which also carries A1-exposed: `n/a (plain EQUIV)`).
- x87 instruction order: classes are LAYOUT and VALUE-FLOW only, no ORDER class. The x87 stream is the original's
  (RESULT_MATCH-NM-5441F0-4C4A50 of 2026-11-12: "x87 instruction stream is identical"). The `x87-motion` tag is the
  integer block (`mov eax,[esp+0xc4]`, `mov [esp+0x60],0`, `sub eax,0`, `mov [esp+0x64],0`, ...) crossing `fsub m4[esp+0x10]`
  at 0x004c4b1b, with no register, flag or slot hazard. No x87 order admitted.
- Cross-check (`equiv_dyn.py --only 0x004c4a50`, log `xcheck_004c4a50.log`): default `same=32 inc=0 diff=0`,
  `--seeded` `same=32 inc=0 diff=0`.
- Logs: `equiv_004c4a50.log` (verdict line), `equiv_trace_004c4a50.log` (`--trace`).
- ADMITTED.tsv row: re/match/ADMITTED.tsv last line. Not C4 (L-128), no promotions tsv, no re-classify.
- PROVISIONAL.tsv row for 0x004c4a50 (`RwCore_004c4a50`, best 77.8, generated source) is untouched: it names the same
  path, now an EQUIV source at 97.2. A lead may want to flip that row to admitted in a later pass. I did not edit it.

### 0x005441f0

equiv.py gives no verdict: the run is CPU-bound with no output, so it is not a rule refusal. It is neither EQUIV nor
NOT-PROVEN under any named rule, hence nothing admitted. It could be listed in PROVISIONAL.tsv (knob path only) as a
99.1% matched source in place of its current lifted-asm row (`RwCoreLift_005441f0`, line 887), but I did not do so
unasked. Where the time goes is [UNCERTAIN]; a `--trace` run or a state cap in the explorer would show it.
Run hygiene: only PIDs 6000, 8944 and 16356 (started by this session) were stopped.
