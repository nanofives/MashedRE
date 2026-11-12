# RESULT EQUIV-A6-5BE9F0-DYN (L-135 (a), run 38)

Branch `orch/EQUIV-A6-5BE9F0-DYN` off `orch/integration` @ b7fb2e66e (EQUIV-ADMIT-A6 already merged).
Tools/text row: `re/tools/match/equiv_dyn.py` (harness), `re/match/ADMITTED.tsv` (one row), the RWC provisional
list, and one generated comment line in `RwCore_004cba80.cpp`. No code change in any exe body, no build, no gates.

## Verdict: harness gap, fixed. 0x005be9f0 admitted. No equiv.py soundness row.

### 1. What the two DIFFERS were

Full event pairs (`pairs_default_before.log`, the old harness, seed 1, 16 runs): runs 2 and 8 differ. In both, the
two sides make the **same multiset of calls** in a different order near the end (run 2: the last 4 of 46 events,
run 8: 3 of the last 4 of 30). Every event records 0 arguments.

The two bodies (`bodies_listing.txt`) have an identical loop (`0x005bea16..0x005bea42` against ours
`0x26..0x52`). They differ only in the order of the saved registers: original `push ebx, ebp, esi, edi`
(`0x005be9f0..0x005be9fb`), ours `push ebx, esi, edi` then `push ebp` inside the branch (`0x1b`). The loop calls
`call dword ptr [edx+4]` (`0x005bea28`) after `push eax` (`0x005bea25`), and re-reads param_3 from
`[esp+0x1c]` (`0x005bea19`) every iteration.

Cause: the stub for a call through a pointer whose target is outside the image (a random heap word here) took
`P = 0, N = 0` (`equiv_dyn.py` "else: P, N = 0, 0"). The proof does not: A6 (`equiv.pin_pops`) pins that call's pop
count at 4 on both sides from the loop join (ours `0x38 -> 4`, original `0x005bea28 -> 4`, unique). With P = 0,
ESP drifts 4 bytes lower every iteration, so after a few iterations `[esp+0x1c]` reads the pushed arguments and the
saved registers instead of param_3. The saved registers sit in a different order on the two sides, so the stores
`mov [ecx+edi*4], eax` (`0x005bea22`) go to different heap addresses, and where those alias a vtable slot read later
the call targets come out in a different order. The difference exists only in the harness's model of a callee that
does not pop its argument. Under the proof's (and the real __stdcall method's) pop count the two bodies are the same.

Classification: **harness gap** (calls through a pointer stubbed with the wrong pop count), not a hole in A6 or in
equiv.py. No soundness row filed.

### 2. The fix (`re/tools/match/equiv_dyn.py`)

`bodies_for` computes `equiv.Ctx(...).pins(side)` for both sides and keys each uniquely pinned site by its return
address in the Unicorn layout (ours: `OURS_BASE` + section offset). `run_one` uses that pin, when present, for a
call that is not an IAT import call: `P = pin`, `N = P / 4` (the same `N` the proof's `info` uses). So the stub pops
what the proof assumed and the event records the callee's arguments. Free (unpinned) sites keep the old rule.

### 3. Checks

| run | file | 0x005be9f0 | 0x005bf8a0 |
|---|---|---|---|
| old harness, real source (repro of ADMIT-A6) | `oldharness_repro.log` | same=6 inc=8 **diff=2** | n/a |
| fixed, default, 16 runs, seed 1 | `dyn_default16.{tsv,log}` | same=9 inc=7 diff=0 | same=16 inc=0 diff=0 |
| fixed, `--seeded`, 16 runs | `dyn_seeded16.{tsv,log}` | same=1 inc=15 diff=0 | same=16 inc=0 diff=0 |
| fixed, default, 64 runs, seed 7 | `dyn_default64s7.{tsv,log}` | same=38 inc=26 diff=0 | same=64 inc=0 diff=0 |

Inconclusive runs are all "too many calls" on **both** sides (random `param_2` loop count over the 300-call cap),
`inconclusive_reasons.log`. The former DIFFERS runs 2 and 8 are now same, with 25 and 17 events.

Negative control (`negctl_argmutant*.log`): a mutant of the source that passes `param_1` instead of `piVar2` to the
pointer call. Fixed harness: DIFFERS same=6 inc=7 diff=3, first difference on the recorded argument. Old harness:
same=6 inc=8 diff=2, the same tally as the real source, so the old harness could not see the argument at all.

Regression over every admitted row (34 rows of `ADMITTED.tsv` before this lane, `admitted_all_in.tsv`, default
and seeded, 16 runs): see section 5.

`equiv.py` re-run at tool commit 0e635d0c6 (`equiv_5be9f0.log`, `equiv_5be9f0.json`): `EQUIV
[ORDER,LAYOUT,FRAME,VALUE-FLOW]`, mode frame, rules `A6-paired-pop, A6-pinned-pop`. Same as ADMIT-A6's measure.

### 4. Admission of 0x005be9f0

Checks of `verify/equivadmit_20261111/gen_admitted.py` applied by hand to one row: verdict reproduced (plain EQUIV,
default mode, same classes), 0 differences in either cross-check, 9 conclusive runs in the better cross-check (8
needed). No `--relax-icall`, no conditional verdict, no x87 code in the body (L-126). FRAME rows are already
admitted under plain EQUIV (0x00417180, 0x00429bd0, 0x005aaac0 and others). Row appended to `re/match/ADMITTED.tsv`.
Not C4 by this status (L-128). No compiled caller in `mashedmod/src` (grep), so nothing to bind.

### 5. Regression sweep (admitted rows under the fixed harness)

All 34 rows admitted before this lane, re-run with the fixed harness (16 runs, seed 1, default and `--seeded`):
**0 differences in 68 cross-checks, and every same/inc/diff tally equals the one recorded in `ADMITTED.tsv`**
(`admitted_all_default16.tsv`, `admitted_all_seeded16.tsv`, compared column by column). The pin path changes
nothing for the existing record, 0x005bf8a0 (the other A6 row) included.

### 6. 0x004cba80 provisional entry dropped

`match.py re/match/s20/004cba80.c --only 0x004cba80`: 100.0% (regs 100.0%, ops 100.0%), 31/31, tc=rw9044. hooks.csv
row 592 is C4. Edits:
- `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`: row `0x004cba80` removed (104 rows left with the header).
- `verify/rwccorerun_20261111/RwCore_Bodies.tsv`: `0x004cba80 provisional 90.3` -> `match100 100.0`.
- `mashedmod/src/mashed_re/RwCore/RwCore_004cba80.cpp` line 2: generated header `0x004cba80 provisional` ->
  `0x004cba80 match100` (the text `gen_rwcore_tus.py` writes for a match100 unit). Comment only, the object is unchanged.

## Files
In this folder: `admit_in.tsv`, `dyn_default16.*`, `dyn_seeded16.*`, `dyn_default64s7.*`, `equiv_5be9f0.{log,json}`,
`inconclusive_reasons.log`, `negctl_argmutant.log`, `negctl_argmutant_oldharness.log`, `oldharness_repro.log`,
`pairs_default_before.log`, `bodies_listing.txt`, `admitted_all_in.tsv`, `admitted_all_{default,seeded}16.{tsv,log}`.
No builds, no gates, no ddkobj check (tools/text row, no exe body change).
