# RESULT MATCH-PROVDROP-RWC

Kind: match-only / tool. No builds, no gates, no tracker edits, no `--apply`. Base 9a44eee36, branch `orch/MATCH-PROVDROP-RWC`.

## 1. prov_drop.py reads PROVISIONAL_RWC.tsv

Where the table is referenced (grep over re/, scripts/, mashedmod/, verify/rwccorerun_20261111/):
- Written by `verify/rwccorerun_20261111/gen_rwcore_tus.py:800`, file `verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`, columns `rva source best_pct exe_symbol path note` (no `status`, no `name`). 898 data rows.
- `re/orchestrator/merge_train.ps1:271` (`Test-MatchTablePath`, keyed union for the `ADMITTED|PROVISIONAL|PROVISIONAL_RWC|REFUSED_UNITS.tsv` names) and the fixture at `:1202`/`:1215` (`verify\rwc\PROVISIONAL_RWC.tsv`, selftest only).
- `ddkobj.py load_provisional()` (re/tools/ddkobj/ddkobj.py:282) reads only `re/match/PROVISIONAL.tsv`; it does NOT read PROVISIONAL_RWC.tsv. `equiv.py` has no reference to either table. So a stale RWC row does not keep a byte-identical unit out of the object build. Not changed here (out of scope), flagged for a follow-up row.

Change (`re/tools/match/prov_drop.py`):
- Reads both tables by default, same rule, one combined classify pass (one compile per source). `--rwc PATH` (relative to `--root`) overrides the file, `--no-rwc` reads only PROVISIONAL.tsv, a missing RWC file is skipped.
- Evidence table gets a last column `table` (existing column positions unchanged). stderr prints rows and DROP per table.
- `--apply` rewrites each table without its own DROP rows (byte-preserving, per table). The RWC table has no status column, so the evidence `status` shows `-` there.
- Lifted rows (`RwCore/RwCore_Lifted_NN.cpp`) classify as `no-re-match-source` as in PROVISIONAL.tsv.

Tests: `re/tools/tests/test_prov_drop.py`, 3 new (RWC rows classified with the same rule and PROVISIONAL.tsv verdicts unchanged, RWC `--apply` byte-exact and idempotent, `--no-rwc` and missing file). Result: see the end of this file.

### Dry run (real tables, nothing written): `dryrun.log`, `dryrun_evidence.tsv` in this folder
Rows examined 1829: PROVISIONAL.tsv 931 rows, DROP 0. PROVISIONAL_RWC.tsv 898 rows, DROP 12. KEEP reasons over both: no-re-match-source 1392, not-byte-identical 386, not-included 29, admitted 8, match-error 2.

RWC DROP rows (byte-identical, relocheck OK, included by an exe TU): 0x004b3f90, 0x004b4070, 0x004b5300 (rwfc), 0x004c6750 (s19, 1092 B), 0x004cba80 (s20), 0x004cdd60 (rwl01), 0x00552920, 0x00553cf0 (rwl04), 0x00552b50 (rt2d), 0x00554390 (rwl05, 1110 B), 0x0055deb0 (rwfc), 0x005c4c60 (s28). Sources and byte counts are in `dryrun_evidence.tsv`.
Not dropped, for the record: 0x00551a50 stays `not-included` (PROVISIONAL.tsv, byte-identical, no exe TU includes it, as in the earlier run). 0x00536780 (both tables) is `match-error: no Ghidra size`.

## 2. Rows in both PROVISIONAL.tsv and ADMITTED.tsv: 8, not 4

Computed by rva intersection of `re/match/PROVISIONAL.tsv` (931) and `re/match/ADMITTED.tsv` (56). prov_drop reports `admitted` for only 4 of them (0x0045a3a0, 0x00493640, 0x004c4a50, 0x00536e90) because the other 4 are lifted rows whose PROVISIONAL source is `RwCore/RwCore_Lifted_NN.cpp`, which fails `no-re-match-source` before the admitted test. Additionally 0x00554200 is in ADMITTED.tsv and PROVISIONAL_RWC.tsv only (not in PROVISIONAL.tsv), so the intersection over both provisional tables is 9.

Common resolution for all rows: the minimal action is a Mariano decision to either drop the provisional row (the body the exe runs is the provisional one, so dropping it is only safe once the admitted source is what a TU includes) or bind the admitted source in the wrapper. Nothing here changes the exe by itself. Rows are never C4 by admission (L-126).

### A. Source-matched pairs (4)

| RVA | PROVISIONAL source (best_pct, equiv) | ADMITTED source, verdict, xcheck | Why dual-listed | Minimal action |
|---|---|---|---|---|
| 0x0045a3a0 | re/match/s09/0045a3a0.c, 81.0, EQUIV-IF-CALLERS-IGNORE(EAX) with callers scan 2/3 (0x0045ac2a unknown), flags /O2 /GS /Ob1, DDKOBJ-UNLISTED-10 | same source s09/0045a3a0.c, EQUIV-IF-CALLERS-IGNORE(EAX), caller scan 3/3 ignore (0x0045ac2a followed through 0x0045ac20 to the table call 0x0045b9e2), xcheck SAME same=32 inc=0 diff=0 | Admission landed after the provisional row was written. ADMITTED `port_binding` says "also in PROVISIONAL.tsv (DDKOBJ-UNLISTED-10) until the admission is applied". The exe already includes the source: `Match/PortTop_0045a6b0.cpp:39` (`#define FUN_0045a3a0 Orig_0045a3a0`, `#include .../s09/0045a3a0.c`). | Drop the PROVISIONAL.tsv row. The source is included, but relocheck is BAD (36 bytes differ, PROVISIONAL diagnosis), so prov_drop will never drop it by itself: needs a hand drop or an admitted-aware rule (L-161 open per the diagnosis). Decision for Mariano: admitted-aware drop rule. |
| 0x00493640 | re/match/s15/00493640.c, 67.9, NOT-PROVEN (RWC-CORE-RUN generated), in both PROVISIONAL.tsv and PROVISIONAL_RWC.tsv | same source, EQUIV (ENC,LAYOUT; rules polarity), xcheck same=16 inc=0 diff=0, EQUIV-ADMIT-RW | Admitted after the RWC run generated its row. ADMITTED `port_binding`: "no compiled caller on the path yet". The exe includes the source: `RwCore/RwCore_00493640.cpp:28`, listed at exe_sources.rsp:1435, used by `LibRw/RwApiBind.cpp:138`. | Drop from both tables (2 rows). No exe change, the TU already compiles the admitted source. |
| 0x004c4a50 | re/match/s19/004c4a50.c, 77.8, NOT-PROVEN, both tables | same source, EQUIV (LAYOUT,VALUE-FLOW; 7 rules), xcheck 32/0/0, admitted under L-147 (a) | Same pattern. ADMITTED `port_binding`: "provisional RwCore_004c4a50 is the bound body; re-point on the next row that touches it". TU `RwCore/RwCore_004c4a50.cpp:12`, exe_sources.rsp:984. | Drop from both tables. |
| 0x00536e90 | re/match/rwl03/00536e90.c, 0.0, NOT-PROVEN, both tables | same source, EQUIV (ORDER,FRAME; x87-barrier, x87-motion), xcheck same=19 inc=45 diff=0 (64 runs) | ADMITTED `port_binding`: "bound as provisional RwCore_00536e90 (knob path, PROVISIONAL.tsv row 813); now admitted". TU `RwCore/RwCore_00536e90.cpp:10`, exe_sources.rsp:1046. | Drop from both tables. |

### B. Lifted-vs-admitted pairs (4): the admitted source is a different file than the one the exe runs

| RVA | PROVISIONAL (lifted scaffolding, L-137) | ADMITTED source, verdict, xcheck | Why dual-listed | Minimal action |
|---|---|---|---|---|
| 0x0052e5c0 | `RwCoreLift_0052e5c0`, naked transcription `RwCore_Lifted_07.cpp:1822`, `#define FUN_0052e5c0 RwCoreLift_0052e5c0` in `RwCore_0052e310.cpp:23` | re/match/g2seq/0052e5c0.c, EQUIV (SIB), xcheck 16/0/0 | g2s-generated source admitted (EQUIV-ADMIT-RW), but no exe TU includes it ("no compiled caller on the path yet"). The lifted body is what runs on the knob path. | Include `re/match/g2seq/0052e5c0.c` in a wrapper (Mariano names the row, RwCore wrappers are knob-path), then drop the lifted row. Dropping first would leave the call with no body. |
| 0x00533900 | `RwCoreLift_00533900`, `RwCore_Lifted_08.cpp:21`, define in `RwCore_005336d0.cpp:15` | re/match/g2seq/00533900.c, EQUIV (SIB), xcheck default same=10 inc=6 diff=0, seeded 16/0/0 | Same as above. Default xcheck has 6 inconclusive runs, seeded run clean. | Same: include g2seq source in wrapper, then drop the lifted row. |
| 0x00544100 | `RwCoreLift_00544100`, `RwCore_Lifted_09.cpp`, define in `RwCore_00543e50.cpp:25` | re/match/g2seq/00544100.c, EQUIV (ORDER; A6-paired-pop, A6-pinned-pop, A7-stack-image), xcheck default 5/11/0, seeded 10/6/0 | Same. ORDER class admitted by rule, not a conditional verdict. | Same. |
| 0x005516c0 | `RwCoreLift_005516c0`, `RwCore_Lifted_10.cpp`, define in `RwCore_005515a0.cpp:14` | re/match/g2seq/005516c0.c, EQUIV (REG,LAYOUT,FRAME,VALUE-FLOW; A7-stack-image), xcheck 16/0/0 | Same. | Same. |

All four ADMITTED rows cite `verify/equivadmitrw_20261113/RESULT_EQUIV-ADMIT-RW.md` and source_commit "this commit (g2s output copied in)". The lifted wrapper units (`RwCore_Lifted_07..10.cpp`) are scaffolding that contains many other lifted bodies, so only the one row's lifted copy goes away per wrapper change.

### C. 0x00554200 (RWC table only)
PROVISIONAL_RWC.tsv line 866: re/match/s24/00554200.c, 69.5, NOT-PROVEN. ADMITTED.tsv line 57: same source, EQUIV (ORDER; A6-paired-pop, A6-pinned-pop, A7-stack-image, jmp-threading), rw9044. It is in no compiled TU by name (grep of exe_sources.rsp and mashedmod/src finds RwCore_00553e80.cpp and RwCore_00554390.cpp mention it, neither includes s24/00554200.c as a source). [UNCERTAIN]: whether the body runs as the lifted/other copy; not checked. Action: include the admitted source in a wrapper, then drop the RWC row; or drop the RWC row if no TU includes it and nothing binds it.

## Follow-ups worth a row (not done here)
1. `ddkobj.py load_provisional()` should also read PROVISIONAL_RWC.tsv, or stale RWC rows keep byte-identical units out of mashed_re_ddk.exe.
2. prov_drop `admitted` handling: the 4 source-matched dual rows could be dropped by rule when the exe TU includes the admitted source (0x00493640, 0x004c4a50, 0x00536e90 qualify today by the include test, 0x0045a3a0 fails relocheck).
3. `--apply` for the 12 RWC DROP rows (and 0x004cba80 note: integration already carries it match100 per merge note) is a separate tracker step.

## Test result
`test-slot py -3.12 -m pytest re/tools/tests/test_prov_drop.py -q`: 8 passed (5 existing, 3 new).
