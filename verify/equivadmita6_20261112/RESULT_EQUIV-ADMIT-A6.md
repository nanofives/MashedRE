# RESULT EQUIV-ADMIT-A6 (L-132 (a), run 38)

Branch `orch/EQUIV-ADMIT-A6` off `orch/integration` (EQUIV-CALLEE-POPS merged, tool commit 0e635d0c6).
Text-only row: only `re/match/ADMITTED.tsv` changed, no exe code, no build.

## Verdict: 1 of 3 admitted

| rva | source | equiv.py (`equiv3.log`, `--json`) | cross-check default / seeded (16 runs each) | outcome |
|---|---|---|---|---|
| 0x004cba80 | re/match/s20/004cba80.c | EQUIV [BYTE-IDENTICAL] | not run | **Not admitted, superseded.** Since 9e36314ac (toolchain rw9044) the source is a byte match and hooks.csv has C4. `PROVISIONAL_RWC.tsv` still lists it at 90.3, which is stale |
| 0x005be9f0 | re/match/s28/005be9f0.c | EQUIV [ORDER,LAYOUT,FRAME,VALUE-FLOW], A6-paired-pop, A6-pinned-pop | default same=6 inc=8 **diff=2**, seeded same=1 inc=15 diff=0 | **Not admitted.** The static proof and the Unicorn witness disagree, and the gate refuses on any difference (L-126, EQUIV-ADMIT-1 gen_admitted rule) |
| 0x005bf8a0 | re/match/s28/005bf8a0.c | EQUIV [ORDER], A6-paired-pop, A6-pinned-pop, call-args-all | same=16 inc=0 diff=0, both modes | **Admitted**, row appended to `ADMITTED.tsv` |

No `--relax-icall`, no conditional verdict, no x87 order for any row (sources have no float code except the preamble typedef text).

## Binding
No compiled caller exists in `mashedmod/src` for 0x005be9f0 or 0x005bf8a0 (grep over the tree). The only exe body
touching A6 rows is `RwCore_004cba80.cpp` (matched source). So nothing to bind: 0x005bf8a0 binds when its caller lands.

## Open: 0x005be9f0
Unicorn default mode reports 2 DIFFERS in 16 runs (event lists diverge, first difference truncated in
`dyn2_default.log`). Either the harness models the calls through a pointer (the A6 case) differently from the proof,
or A6 has a hole. Resolving it needs a look at the full event pair in `equiv_dyn.py`. Filed as a decision for the lead.

## Files
`equiv3.log`, `admit2_in.tsv`, `dyn2_{default,seeded}.{tsv,log}`.
No builds, gates, or ddkobj check run (text-only row, no exe change).
