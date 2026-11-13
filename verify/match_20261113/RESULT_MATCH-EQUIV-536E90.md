# RESULT MATCH-EQUIV-536E90

Row: re-run the L-126 admission for `0x00536e90` (`re/match/rwl03/00536e90.c`, 99.1%, posStride reload 3 slots late) with the integer-motion-across-x87 rule (EQUIV-X87-INTEGER-MOTION, L-147 (a)).

## Verdict

**GREEN. Admitted** (row appended to `re/match/ADMITTED.tsv`). Admitted is not C4 (L-128): no promotions tsv, no re-classify.

- `equiv.py` default mode: `0x00536e90  EQUIV  [ORDER,FRAME]`, rules `x87-barrier`, `x87-motion` (`equiv_default.log`, `equiv_trace.log`). No conditional, no `--relax-icall`.
- **x87 order unchanged.** The x87 instruction sequence of the original and of ours, extracted from the `--trace` pairing, is 288 against 288 instructions and identical once absolute global operands are normalised (the original shows `[0x5cc32c]`, ours a relocation placeholder `[0]`). Only integer instructions moved (`mov ebp,[esp+0x14]` stride reload, ours +0x125, 3 slots later). Check: `x87_sequence_check.log`.
- **Caller scan.** Verdict is plain EQUIV, so no caller assumption applies. For the record, the two references in the original (`re/match/rwl03/00536780.c:97`, `q->cb28 = FUN_00536e90`; immediates at 0x00536364 and 0x00536843 per hooks.csv) take its address, there is no direct call.
- **Unicorn cross-check** (`equiv_dyn.py`, single-source mode):
  | mode | runs | same | inconclusive | differs |
  |---|---|---|---|---|
  | default | 16 | 4 | 12 | 0 |
  | seeded | 16 | 4 | 12 | 0 |
  | default | 64 | 19 | 45 | 0 |
  | seeded | 64 | 19 | 45 | 0 |
  Seeds 2 and 3 at 16 runs: same=8 / same=6, diff=0. The 16-run tally is below the 8-conclusive floor of EQUIV-ADMIT-1, so the row records the 64-run tally (19 conclusive, 0 differences). No seed builder exists for this RVA and none was added (tool not changed).

## Tool provenance

- Tool commit a9ca480f1, branch `orch/EQUIV-X87-INTEGER-MOTION` @ 5c6ebac0d. **`orch/integration` (69cd03777) does not carry it yet** (`merge-base --is-ancestor a9ca480f1 orch/integration` = 1), so I merged that branch head into `orch/MATCH-EQUIV-536E90` (merge 78eef3395, `merge.autoStash=false`). The lead handles ordering: enqueue this row only after EQUIV-X87-INTEGER-MOTION, or directly behind it.
- Blob shas run against: `re/tools/match/equiv.py` b720231cb25f9ebc18e1cb2adebf2f1695733f1b, `equiv_rules.md` 9195859b882ee26535139fae7595a0cbe70f876a, `equiv_dyn.py` 4d304f357169fa94b1dc07ec056162d0b9c5a141. The tool was not edited, so `test_equiv.py` was not re-run.
- If the merged tool on integration differs from these blobs, re-run `equiv.py --trace re/match/rwl03/00536e90.c` before the row is final.

## Files

`equiv_default.log`, `equiv_trace.log`, `x87_sequence_check.log`, `dyn_default.{log,tsv}`, `dyn_seeded.{log,tsv}`, `dyn_default64.{log,tsv}`, `dyn_seeded64.{log,tsv}`, `re/match/ADMITTED.tsv` (+1 row).

## Follow-up

`re/match/PROVISIONAL.tsv` row 813 for `0x00536e90` still lists it as provisional (`NOT-PROVEN`). The merge train or the next tracker pass should move it to ADMITTED. Not edited here.
