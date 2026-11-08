# C4R-C2-FLAGSWEEP: flag grid over every re/match source below the match gate (2026-11-07)

**Verdict: RED for the route (0 rows move), GREEN for the probe.** No C2/C3 row reaches the match gate under any of
the 74 flag sets per source. Compiler flags are not what keeps these rows below 100%. No `// FLAGS:` line was
changed, and hooks.csv was not touched by this part.

## Scope

Targets: every `re/analysis/match_ledger.tsv` row with `hooks_conf` C2 or C3 and verdict `REFUSED`: **746 rows**
(644 C2 + 102 C3) in **736 sources**. 743 are "not byte-identical", 2 are "bytes equal but shorter than the
original body", 1 is an `asm-in-c` source.

Grid per source (`flagsweep.py`): O {`/O1`,`/O2`} x Ob {`/Ob0`,`/Ob1`,`/Ob2`} x G {default,`/G5`,`/G6`} x
{-,`/Oy-`} x {-,`/GS`} = 72 sets, plus the source's recorded `// FLAGS:` and recorded + `/GF`. `match.py` always
passes `/Gy`, so `/Gy` is not an axis. On the BaseClasses build the 9176 default schedule gives the same rows as `/G6`
(C4R-DSHOW-BASECLASSES), so `/GB` was not run as its own arm. Compiler: XP DDK `cl` 13.00.9176 /
`c2` 13.00.9178, through `match.compare(..., bytes_mode=True)` and `relocheck.check` with the flag set overridden.
Gate = `c4_reclass.gate_match`: 0 differing unmasked bytes, insn 100% (or longer than the Ghidra body), relocheck OK.

```
py -3.12 verify/c4rflagsweep_20261107/flagsweep.py run --jobs 8
py -3.12 verify/c4rflagsweep_20261107/flagsweep.py report
```

## Result

| measure | value |
|---|---:|
| compiles | 54,084 |
| compile errors | 0 |
| control: rows whose recorded-flags score equals the ledger `best_pct` (within 0.05) | 746 / 746 |
| rows reaching the gate under some flag set | **0** |
| rows whose best grid score beats the recorded score | 73 |
| mean gain on those 73 | 4.6 points |
| largest gain | 0x004dfb10, 20.0% -> 51.7% |
| rows at >= 95% under the best grid set | 108 (the same 108 that are >= 95% under the recorded flags) |

No row crosses 95% because of a flag change. `hits.tsv` is empty (header only).

### By address range (64 KB blocks, `ranges.tsv`)

Best grid set per range = the set with the highest mean insn score over that range's rows (ties go to the set most
rows of the range record):

| ranges | rows | best grid set | best mean vs recorded mean |
|---|---:|---|---|
| 0x0040-0x0049 (except 0x0047) | 246 | `/O2 /Ob1 /GS` | equal in 8 of 9 ranges; 0x0044 recorded 80.1 vs 76.1 (recorded sets outside the grid) |
| 0x0047 | 42 | `/O2 /Ob0` (= `/Ob1` there, nothing inlines) | equal |
| 0x004a | 6 | `/O1 /Ob0` | equal |
| 0x004b | 13 | `/O2 /Ob1 /GS` | 73.5 vs 73.7 |
| 0x004c-0x005c | 439 | `/O2 /Ob1 /G5` | equal or recorded higher (0x004f 64.8 vs 60.3, 0x005b 55.1 vs 53.9, 0x005c 37.8 vs 35.4) |

So the address space splits in two per-object flag families, `/O2 /Ob1 /GS` below about 0x004c0000 and
`/O2 /Ob1 /G5` above it, and the recorded `// FLAGS:` lines already follow that split. Within each range, 0 to 13
rows have some other set scoring higher, always by a few points and never to the gate. No range shows a consistent
alternative set, so no FLAGS line changes and no batch follows from this part.

Files: `flagsweep.py`, `sweep.jsonl.gz` (one line per source x flag set: per-RVA insn score, differing bytes,
relocheck), `sweep_log.txt.gz` (`[N/M] source flags STATUS`), `summary.json`, `hits.tsv`, `best_pct.tsv`,
`ranges.tsv`.
