# RESULT D4-WSB-C4-SOLV-RGN: BLOCKED (audio endpoint)

Verdict: **BLOCKED ("audio endpoint").** No original was launched. No row changed level, no tracker edit, no source
change. Per the standing rule the row is parked and the check is not retried.

## What blocked it

The pre-launch check for this session's first original boot (2026-10-28T19:07Z, taken for `D4-WSB-STACKONLY-C3`, run
first per the lead's order) found no present audio endpoint: `Get-PnpDevice -Class AudioEndpoint -PresentOnly`
returned no object, and the 2 `Philips FTV (NVIDIA High Definition Audio)` endpoints are `Present=False`. Every leg of
this step boots the original, so it is parked on the same condition. Detail:
`verify/wsbstackonly_20261028/RESULT_D4-WSB-STACKONLY-C3.md` on `orch/D4-WSB-STACKONLY-C3`.

## Done before the block

- Branch `orch/D4-WSB-C4-SOLV-RGN` off `orch/integration` @ `0d1285dc7`, worktree `.worktrees/D4-WSB-C4-SOLV-RGN`.
- **PREREG `PREREG_D4-WSB-C4-SOLV-RGN.md` committed alone at `eb1a8b9ce`**, before any arm.
- Tools `run_shadow6.py` (copy of SOLV-WS `run_shadow4.py`, data path / label) and `run_bvh6.py` (copy of `run_bvh4.py`,
  data path / label, reads `c4_racediff_v7.py` from `verify/wsbsolvws_20261027/` unchanged).
- Build and private root are shared with STACKONLY (same commit, no source change on either branch): `mashed_re_dev.asi`
  `c6a264a3bde198ac9379d22d24a59b42f8d05d0ba389fe36a738f3f0e9bd3566`, root `D:/MashedWT/D4-WSB-STACKONLY-C3_root`.
  No raw data exists for this step yet. No car-contact capture is shared: these rows run in the census and Quick Battle.

## Re-pickup (from this worktree root, once an endpoint is present)

```
export MASHED_ROOT=D:/MashedWT/D4-WSB-STACKONLY-C3_root PYTHONIOENCODING=utf-8
MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 test-slot bash -c 'py -3.12 verify/wsbsolvrgn_20261028/run_shadow6.py 0056bce0 0056cf90 0056ed60 0056fad0'
test-slot bash -c 'py -3.12 verify/wsbsolvrgn_20261028/run_bvh6.py 0056bce0 0056cf90 0056ed60 0056fad0 0056bb30'
```

Then the default gates (one run serves both steps, same exe), re-classify per row as the PREREG fixes it.

## Files

Committed: `PREREG_D4-WSB-C4-SOLV-RGN.md`, `run_shadow6.py`, `run_bvh6.py`, this file. No game PID was spawned. No
watcher started.
