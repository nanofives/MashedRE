# FUN_0040eee0 — Round-end car-elimination + score distribution

**RVA:** `0x0040eee0`
**Body extent:** `0x0040eee0..0x0040fbfc` (3356 bytes — large but single contiguous body).

## Signature

```c
undefined4 FUN_0040eee0(int param_1, int param_2);
```

- `param_1` = car index being eliminated/scored (0..3).
- `param_2` = score delta to apply.
- Returns `1` on the elimination-fallthrough success path
  (case `iVar5 == 1` final branch), `0` otherwise.

The existing C1 plate already documented the per-car-index + score signature.

## Callers

| RVA          | Name        | Note                                              |
|--------------|-------------|---------------------------------------------------|
| `0x00410d10` | FUN_00410d10 | Per-frame elimination-event handler.              |
| `0x00424eb0` | FUN_00424eb0 | Match-end / round-end finalizer.                  |

## Callees (14 direct, Ghidra `function_callees`)

| RVA          | Use                                              |
|--------------|--------------------------------------------------|
| `0x00408a50` | `FUN_00408a50(slot)` — read per-car float metric (return float10). |
| `0x00408a70` | `FUN_00408a70(slot, float)` — write per-car float metric. |
| `0x00408ad0` | `FUN_00408ad0(slot)` — read per-car position/altitude metric for tie-breaks. |
| `0x0040b290` | `FUN_0040b290(slot, delta)` — apply score delta to slot. |
| `0x0040b6d0` | `FUN_0040b6d0(slot)` — read accumulated rank metric. |
| `0x0040d590` | `FUN_0040d590(a, b, c, d)` — 4-car triple-tiebreak resolver. |
| `0x0040e470` | `FUN_0040e470(slot)` — read elimination-state-class (return 0/1/2/3). |
| `0x00422fd0` | `FUN_00422fd0(slot)` — mark car eliminated (sets in-engine flag). |
| `0x0042f500` | `FUN_0042f500()` — game-mode predicate (returns 0 for normal mode). |
| `0x0042f6a0` | `FUN_0042f6a0()` — current game-mode enum getter (returns 3/4/5/6/10 amongst others). |
| `0x00431d80` | `FUN_00431d80()` — special-state predicate (e.g. "round in final segment"). |
| `0x0046c700` | `FUN_0046c700(slot, value)` — set per-car selection/team-id. |
| `0x0046c7b0` | `FUN_0046c7b0(slot)` — predicate: is car still active in this round (== 1 means yes). |
| `0x004a2cbd` | `FID_conflict:_wprintf` — debug logging. |

## Body summary

Single-shot per-elimination handler called when a car drops out. The flow:

### Phase 1 — Count remaining cars
- Get current game-mode via `FUN_0042f6a0()` → `iVar2`.
- Print `"car reset=%d, score=%d\n"` for diagnostic trace.
- Loop slots 0..3: count cars where
  `*(int*)(PTR_PTR_005f2770 + 0x34 + slot*4) != 0` AND `FUN_0046c7b0(slot) == 1`.
  Sum into `iVar5` (= "numcarsremaining").
- Print `"numcarsremaining=%d\n"`.

### Phase 2 — If exactly one car remains, capture its index
- Repeat the same loop; record the surviving slot in `local_8`.

### Phase 3 — Append to ranking-history array `DAT_008a94c0[]`
- Walk `&DAT_008a94c0[i]` until `-1` sentinel.
- Append `param_1` (eliminated-car index) at the next slot.

### Phase 4 — Branch on `DAT_008a94d0` (game-type)

**`DAT_008a94d0 == 2`** — Knockout / sudden-death mode:
- If exactly 1 car remains: `FUN_0040b290(eliminated, -1)` + `FUN_0040b290(winner, +1)`.
- Else: `FUN_0040b290(0, 0)` + `FUN_0040b290(1, 0)` (clear-only).

**`DAT_008a94d0 == 3`** — Race-rank-based mode:
- Branch on `FUN_0042f500()` (mode-predicate):
  - **`== 0`** (normal): nested case on car-count
    - `iVar5 == 2`: apply `-param_2` to eliminated, then if `DAT_007f0fd0 == 2`
      do team-aware partner score sync; if game-mode `iVar2 in {3,4,5,10}` AND
      `DAT_007f0fd0 == 0`, do a 2-car tie-break using altitude metric
      `FUN_00408ad0`.
    - `iVar5 == 1`: clear-zero eliminated, set winner to +1.
  - **`!= 0`** (special mode):
    - `iVar5 == 2`: read team-IDs from `&DAT_007f1a18[slot*4]`; if same team,
      tie-break by altitude `FUN_00408ad0` (using boundary-aware
      `_DAT_005cc730`/`_DAT_005ccd6c`/`_DAT_005cc568` constants); winning car
      gets `FUN_0046c700(slot, 1)`; then broadcast `±1` to all team members.
    - `iVar5 == 1`: broadcast team-based `±1` to all slots based on whether
      they share team with `local_8`.

**`DAT_008a94d0 == 4`** — King-of-the-Hill / status-objective mode:
- Pre-check: `FUN_0042f6a0() == 6` AND `FUN_00431d80() != 0` AND
  `iVar5 == 2` → 2-car altitude tie-break with `param_1=2, other=1` score.
- Then branch on `FUN_0042f500()`:
  - `== 0` (normal): nested on car-count (1/2/3)
    - `iVar5 == 3`: triple-elim. Apply `-2*param_2` to eliminated. If team-
      aware (`DAT_007f0fd0 == 2`), zero non-self-team partners. If special-
      mode condition (`iVar2 in {3,4,5,10}` AND `DAT_007f0fd0 == 0`) call
      `FUN_0040d590(slot, prev_a, prev_b, param_2)` (triple-tiebreak resolver).
    - `iVar5 == 2`: same as DAT_008a94d0==3, `iVar5==2` path. Identical
      sub-structure (team-aware partner sync + altitude tie-break).
    - `iVar5 == 1`: special case using `FUN_0040b6d0(param_1)`-based
      rank metric: if `> 10`, zero `param_2`; apply to eliminated + +2 to
      winner; fall through to `LAB_0040fbbb`.
  - `!= 0` (special mode):
    - `iVar5 == 3`: 3-team-id collation, double-tiebreak by altitude,
      broadcast `±param_2` to all slots by team membership.
    - `iVar5 == 2`: 2-team altitude tie-break + ±1 broadcast.
    - `iVar5 == 1`: team-id-broadcast (same shape as DAT_008a94d0==3
      special-mode `iVar5==1`).

**`DAT_008a94d0` is anything else with `iVar5 == 1`**: skip directly to
`LAB_0040fbbb`.

### LAB_0040fbbb — final-survivor reward broadcast
- Read winner's per-car float metric `FUN_00408a50(local_8)`.
- Write that float to all 4 cars via `FUN_00408a70(0..3, value)`.
- Return `1`.

All other paths return `0`.

## Cited constants / offsets

| Address          | Use                                              |
|------------------|--------------------------------------------------|
| `PTR_PTR_005f2770` | Per-car PE-relative table base; offset 0x34 + slot*4 yields a "car active" pointer (non-null iff slot in play). |
| `DAT_007f0fd0`   | Team-mode enum (0 = no teams, 2 = team-aware).   |
| `DAT_007f0fd8`   | Self-team / camera-focus slot id.                |
| `DAT_007f1a18`   | Per-car team-id table, indexed `slot*4` (stride 0x10 — note the `piVar8[-1]` look-behind for sentinel `-1`). |
| `DAT_008a94c0`   | Ranking-history array (terminated by `-1`).      |
| `DAT_008a94d0`   | Game-type enum (2 = knockout, 3 = race-rank, 4 = KotH/status). |
| `_DAT_005cc568`  | Altitude tie-break threshold-A.                  |
| `_DAT_005cc730`  | Altitude tie-break lower bound.                  |
| `_DAT_005ccd6c`  | Altitude tie-break upper bound.                  |
| `0x7f1a58`       | End-of-team-table sentinel (loop terminates when `piVar8 >= 0x7f1a58`). |

## Uncertainties

- U-4303: identity of `DAT_008a94d0` enum (2 / 3 / 4). Inferred semantics
  (knockout / race-rank / KotH) from message-ID similarity and
  `FUN_0040b290` argument shapes, but not confirmed against game-mode
  source. Resolution: cross-ref with `FUN_0042f6a0()` return-value map
  (returns 3/4/5/6/10 inline — possibly aliased).
- U-4304: altitude-based tie-break interpretation. The `_DAT_005cc730 /
  _DAT_005ccd6c / _DAT_005cc568` constants form a wrap-around-aware
  ring-comparison (likely angular position on a circular track, not raw
  altitude). Document at C3 time.
- U-4305: `FUN_0040d590` (triple-tiebreak resolver) is a 4-arg sibling
  used only in the `DAT_008a94d0==4` `iVar5==3` `DAT_007f0fd0==0` corner.
  Its semantics need confirmation when reimpling — for now treated as
  opaque triple-elimination scorer.

## Promotion rationale

C1 → C2: body decompiled end-to-end via Ghidra; 4 top-level dispatch
branches enumerated; 14 callees identified and each annotated with its
purpose; ~15 globals cited with their offsets. The function is large
(3356 bytes) but uniformly structured — same "count / capture / dispatch
on game-type" pattern repeats per branch with minor variations. No
mystery callees, no inline assembly, no cross-translation-unit jumps.

Why bulk-batch workers deferred: same as `FUN_0040acd0` — a single
oversized function takes a whole bulk-batch budget. One focused
session handles it cleanly.
