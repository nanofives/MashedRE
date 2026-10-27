# PREREG D4-WSB-C4-BROAD-WS, round 2 (committed alone, before any round-2 arm)

Round 1 (`RESULT_D4-WSB-C4-BROAD-WS.md`, `69c14a09d`) was RED: write-set legs ran image-only (`o:0,n:0` on 3 rows), the
census racediff OFF boot failed, and two Quick Battle racediffs differed only in the high half of a 16-bit callee argument.
Since then `orch/integration` @ `6368c086a` (D4-WSB-SHADOW-PRIVATE: `MASHED_SHADOW_PRIVATE` works, heap pages tracked;
D4-WSB-BVH-FU: census OFF boot fixed in tool v7, value log) is merged into this branch (`430e7feee`).

Rows: `0x0055a1f0`, `0x0055ab30`, `0x0055bae0`, `0x0055c000`, `0x0055c0f0`. Harness `879e1ad65` (RunTracked wrappers) unchanged.
Build: `mashed_re.exe` `ab557985...9399`, `mashed_re_dev.asi` `589a353b...56fc` (copied to the private root only),
`MASHED_ROOT=D:/MashedWT/D4-WSB-C4-BROAD-WS_root`. Every boot under the test slot and
`mashed_lock.py run --label D4-WSB-C4-BROAD-WS`. Not touched: the 0056xxxx solver rows of D4-WSB-C4-SOLV-WS,
`Core/ShadowTrack.h`, `Core/ShadowAB.h`.

## Leg 1: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe. Must equal `merge_train.json`
(`5943c5f1/364350ae/52052890`, `3a03670c/e85ea6fb/53778664`). Any mismatch: verdict RED, no promotion.

## Leg 2: heap-tracked write set (`run_shadow3.py`, one hook per boot)

`MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census scenario (`--track 0 --mode 10 --cars 4
--hold 60 --hooks 0x<rva>`), 24 samples (the SHADOW-PRIVATE recipe). Per row, from `shadow_ab.log`:

- **heap pages compared** = sum over the counted samples of the `heap=o:` count (pages written by the original run,
  each compared against the port run).
- **HEAP-CLEAN**: `PATCHBYTE ... A/B-IS-REAL`, ndiff = 0, every sample `stack=0 noise=0 ovf=0 ret=0`, and heap pages
  compared > 0.
- **CLEAN-NO-HEAP** (ndiff 0, heap pages compared = 0): one follow-up arm with `MASHED_SHADOW_NEED_PAGES=1`
  (`SHADOW_TAG=need`), read as addendum 1 of D4-WSB-SHADOW-PRIVATE: counted samples HEAP-CLEAN -> HEAP-CLEAN;
  EMPTY lines only, all `rv` 0 or void -> NO-WRITES-SAMPLED; no counted sample -> NO-WRITES-SAMPLED. Only HEAP-CLEAN
  passes this leg.
- **DIVERGENT** (ndiff > 0): the row fails this leg. The log's value lines (BVH-FU value log) are reported for the first
  divergent page. Round 1's `0x0055a1f0` sample-0 divergence at `0x00624048` is read the same way: if it recurs under
  STW it is a port difference and the row stays C3; if it does not recur in 24 STW samples, the row passes on this leg
  only when heap pages compared > 0.
- Hang / crash / no samples: re-run once.

## Leg 3: canonical-scenario racediff (tool v10, `run_bvh3.py`, one hook per ON boot)

Tool: `c4_racediff_v10.py`, copied unchanged from `orch/D4-WSB-C4-BROAD-A` @ `020d70c14`. `C4_DEREF=1` on every arm
(field-source tokens; BROAD-A X1 showed `A<k>+<d>` heap distances differ between two boots of the original).

Declared-width masks (`C4_ARGW`, 0-based arg index; each proven from the anchored original that the callee reads only
bits 0-15):

| callee arg | proof |
|---|---|
| `0x00564040` arg 1 (`param_2`) | `argwidth_00564040.txt` (round 1): one load `0x00564047`, read only as `and ecx,0xffff` / `cmp cx,bp` |
| `0x00564c80` arg 1 (`param_2`) | `verify/wsbbvh_20261027/argwidth.txt` item 2 |
| `0x00565260` arg 1 (`param_2`) | `argwidth_00565260.txt` (this round): slot read once into `edi` at `0x0056526b`, slot rewritten at `0x005652a1` before any other read; `edi` read only as `and ebx,0xffff` (`0x00565277`, after `mov ebx,edi`) and 16-bit `cmp bp,di` / `cmp bx,di` (`0x005652ed`, `0x00565322`, `0x005653c9`, `0x005653fe`) until it is overwritten on both paths (`0x0056532a`, `0x00565406`); the callee `0x00563f60` saves `edi` (`push edi` `0x00563f6c`) before writing it |

Arms:

| row | scenario | window (A4 calls) | `C4_ARGW` |
|---|---|---|---|
| `0x0055a1f0` | Quick Battle | 300-1200 | `564040:1:ffff` |
| `0x0055ab30` | Quick Battle | 300-1200 | `565260:1:ffff;564c80:1:ffff` |
| `0x0055bae0` | census, `C4_HOLD=400` | 300-30000 | none |
| `0x0055c000` | census, `C4_HOLD=400` | 300-30000 | none |
| `0x0055c0f0` | census, `C4_HOLD=400` | 300-30000 | none |

A Quick Battle row that is not observed (both sides 0 calls, 2 attempts) gets the census arm. A census row not observed in
2 attempts stays C3 (no scenario). The tool's `installed` must be true (ON byte `0xE9`).

Read-out per row: **C4-CLEAN** (sets equal, non-empty) passes. **BEHAVIOR-DIFF / HOLD**: one OFF-vs-OFF control arm
(`C4_CONTROL=1`, same options, `C4_TAG=_ctl`) and one repeat ON arm (`C4_TAG=_rep`). The row passes only if the control
is CONTROL-DIFF in exactly the differing tokens and the repeat is C4-CLEAN, the BROAD-A addendum-1 rule; otherwise it
stays C3 with the differing tokens in the RESULT. No new mask or option is added after an arm has run without a further
addendum committed alone.

## Promotion rule

A row goes C3 -> C4 through `re-classify` only if leg 1 holds, its leg 2 is HEAP-CLEAN (heap pages compared > 0, with
the count in the RESULT) and its leg 3 passes. Every other row stays C3 with the reason in the RESULT, and a U-n is filed
through re-classify when the reason is a knowledge hole (not a missing scenario). Verdict GREEN if at least one row is
promoted and gates hold, RED if none.
