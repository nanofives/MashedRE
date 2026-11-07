# RESULT MATCH-FIX-45CA30, 2026-11-06

**Verdict: GREEN.** I fixed the port correctness bug MATCH-S10 found at 0x0045ca30 (`FloatClamp3Level45ca30`). The old .asi
body walked 6 floats at 0x0088f5e0. The original walks 36 (0x0088f5e0..0x0088f670, a 6x6 block). The port now
compiles the S10 byte-identical source `re/match/s10/0045ca30.c` (118 B, `match.py --bytes` BYTE-IDENTICAL per
`verify/matchs10_20261102/RESULT_MATCH-S10.md`). The default gates match the refs, so no L-n decision is needed. The level
stays **C3**: the body is asi-only and the exe has 0 callers (L-68).

Branch `orch/MATCH-FIX-45CA30`, cut from orch/integration `ac2e9ed63`. The swap is commit `6205e02ef`.

## What changed

| file | change |
|---|---|
| `mashedmod/src/mashed_re/Match/MatchS10_0045ca30.cpp` | new wrapper TU (S10 pattern): `MATCH_PORT`, `#define FUN_0045ca30 FloatClamp3Level45ca30`, includes `re/match/s10/0045ca30.c`, `RH_ScopedInstall(FloatClamp3Level45ca30, 0x0045ca30)` |
| `mashedmod/src/mashed_re/Util/PromoLoop_sessionB.cpp` | old 6-float body removed, replaced by a pointer note |
| `mashedmod/asi_sources.rsp` | new TU added after `MatchS10_0045c860` |
| `re/frida/hooks_registry.py` | `float_clamp3_45ca30` seeds and observes all 36 floats (3 sets: the -2 / -4 boundaries, ulp neighbours, +-0, +-inf, NaN in every row). The old vector observed 6, which is why the 6-float body passed at r223 |
| `hooks.csv`, `re/analysis/CHANGELOG.md` | via re-classify: file column now points at the new TU, plus the note. C3->C3 |

`x87_tus.txt` is not touched. The body only compares and stores the constants 0 / 0xbf800000 / 0xc0000000, so x87 and
SSE2 give the same results, and editing that list would force a full rebuild of both targets.

## Which targets bind the body

- **.asi: yes.** The old body was only in `asi_sources.rsp` (via `PromoLoop_sessionB.cpp`), and the new TU is too.
- **exe: no.** There was no body and no caller before, and there still is none. The only caller of 0x0045ca30 in the original
  is FUN_0045d250 (hooks.csv note), and the port has no body for 0x0045d250. A grep of `mashedmod/src` for
  `45d250` / `45ca30` finds only the TUs above. `exe_sources.rsp` is unchanged.
- I checked the exe inputs against the `/sourceDependencies` json of every object
  (`verify/matchfix45ca30_20261106/exedeps.py <wt> ac2e9ed63`). In the exe, **0 of 630** objects depend on a changed
  file. In the asi, 2 of 1185 objects depend on a changed file, and they are exactly `MatchS10_0045ca30.cpp` and
  `PromoLoop_sessionB.cpp`.

## Diff (pure-leaf lane)

`run_diff.py` refuses this arg_type: `near_leaf_seed_multi_obs` has no handler in `diff_template.js`. Its handler is in
`re/frida/early_window_leaf_diff.py` (it re-seeds before each side). That lane loads the shared
`original\mashed_re_dev.asi` by default, so I set `MASHED_EW_ASI` to the worktree build on every run.

| body | asi | cases | mismatches | verdict |
|---|---|---|---|---|
| new (S10 matched) | worktree build, sha256 `c5a3735b...` | 3 | 0 | **GREEN** |
| old (6 floats), negative control | pinned copy of the Oct 24 deployed asi, sha256 `906f8601...` | 3 | 3 | **RED** |

In the RED runs the original's output differs from index 6 onward. Set 2 also shows NaN kept as-is by the original
(`7fc00000` / `ffc00000` in its output), while the old body wrote -2.0 there. Committed evidence:
`log/diff_float_clamp3_45ca30.csv` (new body, sha256 `9b9ad6f0...`) and `diff_old_control_RED.csv` in this folder.

## Gates (default, exe `5d41fe7d`, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

| gate | ref (`merge_train.json` on orch/integration, checked at gate time) | measured |
|---|---|---|
| G-CTRL step / gates / ptrace | 8284be46 / 67931834 / 31fe043e | 8284be46 / 67931834 / 31fe043e |
| B K0 / K1 / K26 | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

## Raw data (not committed), `D:/MashedWT/MATCH-FIX-45CA30_data/`

| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, K1, K26 | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| deployed_old.asi (control) | 906f8601201f69b4670bfef68b438b719e515e0e4e523f892d92c9f095b8a8ad |
| ew.log, ew2.log, gates.log | run logs |

## Not done

- No C4: the body has 0 exe callers (L-68). It becomes C4 when a body for FUN_0045d250 lands in the exe and calls it.
