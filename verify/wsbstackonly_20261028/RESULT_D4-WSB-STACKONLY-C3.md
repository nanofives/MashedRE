# RESULT D4-WSB-STACKONLY-C3: BLOCKED (audio endpoint)

Verdict: **BLOCKED ("audio endpoint").** No original was launched. No row changed level, no tracker edit, no source
change. Per the standing rule the row is parked and the check is not retried.

## What blocked it

Pre-launch check, 2026-10-28T19:07Z: `Get-PnpDevice -Class AudioEndpoint -PresentOnly` returned no object
("No se encontraron objetos coincidentes ... PNPClass = 'AudioEndpoint' AND Present = TRUE"). Without
`-PresentOnly` the only endpoints listed are 2 x `Philips FTV (NVIDIA High Definition Audio)`, both `Present=False`,
`Status=Unknown`. `Audiosrv` and `AudioEndpointBuilder` are `Running`. The Philips FTV is the endpoint the ledger note
names as the one that let the original boot past `0x005baf48` on 2026-10-28. Unblock = that display (or any audio
render device) present again.

## Done before the block

- Worktree `.worktrees/D4-WSB-STACKONLY-C3`, branch `orch/D4-WSB-STACKONLY-C3` off `orch/integration` @ `0d1285dc7`.
- Build `MASHED_NO_DEPLOY=1` at `0d1285dc7` (no source change): `mashed_re.exe`
  `779bc9c329b647e759c3df310ed0fe30e87a617569600c054cbc15c8a860c447`, `mashed_re_dev.asi`
  `c6a264a3bde198ac9379d22d24a59b42f8d05d0ba389fe36a738f3f0e9bd3566`. Build log
  `D:/MashedWT/D4-WSB-STACKONLY-C3_data/build.log`. `original/mashed_re_dev.asi` not touched.
- Private root `D:/MashedWT/D4-WSB-STACKONLY-C3_root` (robocopy of `original/`, asi copied in, hash checked;
  its `MASHED.exe` `b9977dab...fe87` = `original/MASHED.exe`).
- **PREREG `PREREG_D4-WSB-STACKONLY-C3.md` committed alone at `fa8ab1fbb`**, before any arm. Tools `run_shadow5.py`
  (copy of SOLV-WS `run_shadow4.py`, data path, label, `TRACK` env) and `heap_pages.py` (copy, data path).

## Row state (unchanged)

| row | level | why it stays |
|---|---|---|
| `0x0055bae0` | C3 impl | L-40 (a): every return is caller stack, 0 heap pages possible (BROAD-WS leg 2). No arm planned. |
| `0x0055c000` | C3 ported | same as above |
| `0x00568990` | C3 impl | heap leg not run (blocked). Census gave 0 counted samples (SOLV-WS section 1). |
| `0x0056b7a0` | C3 impl | same as above |

The dated note for `0x0055bae0` / `0x0055c000` is already on their rows from BROAD-WS. No new note is added here.

## Re-pickup (ready to run, from the worktree root)

The step's finding so far is static: RWP bodies are the track's `RWP_Object` props (`re/analysis/formats/track_world_bsp.md:139`),
and `0x0056b7a0` appends a pair only on two-body AABB overlap (`RwpSolverCore18.cpp:137`). Training (track 0, the census)
gave no pair in 150 s, so the PREREG searches prop tracks: City 12, then Arctic 3, Warzone 11, Dump 10.

```
MASHED_ROOT=D:/MashedWT/D4-WSB-STACKONLY-C3_root MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1 \
MASHED_SHADOW_NEED_PAGES=1 HOLD=150 SHADOW_TAG=t12 TRACK=12 PYTHONIOENCODING=utf-8 \
test-slot bash -c 'py -3.12 verify/wsbstackonly_20261028/run_shadow5.py 0056b7a0'
```

Then arm H (`0x00568990`) on the track that gave counted samples, arm R (racediff v7, Quick Battle), gates, re-classify.
One caveat to read in the first boot's log: NEED_PAGES stops sampling after 2000 discards (`Core/ShadowTrack.h:766-777`).
If a 52-prop track sweeps many bodies per frame, the budget can run out before the first contact. The EMPTY count
lines show it (`(sample discarded, N)`). A delay knob would be a shared-header change, so it would need its own
PREREG and a gate run.

## Files

Committed: `PREREG_D4-WSB-STACKONLY-C3.md`, `run_shadow5.py`, `heap_pages.py`, this file.
Raw: `D:/MashedWT/D4-WSB-STACKONLY-C3_data/build.log` (SHA-256 `0af1900181b36bf6b08e8de859821220b7fc75fba07dae0b7bd8981854319145`). No game PID was spawned. No watcher started.
