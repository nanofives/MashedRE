# RESULT HB-TESTHUB-OFF-REMOVE (2026-11-14)

Row kind: text/script. No build, no gates, no game launch.

## What changed
- `verify/rwcthunk_20261113/city_arms.py`: removed `env = dict(os.environ, PROYECTOS_TEST_HUB="off")` and the `env=env`
  argument of `subprocess.call`. The arms now inherit the caller's environment, so the hub decides by its own rules.
- `verify/rwcframeclump_20261113/RESULT_RWC-FRAME-CLUMP.md`: added a dated note (2026-11-14) after line 163 saying the
  advice to set `PROYECTOS_TEST_HUB=off` is superseded (hub fixed 2026-11-13 21:00, Mashed out of `hub.exclude`, paths
  outside the repo, builds and harness-lock runs stay local automatically). The original text is left as history.

## Grep
Command (worktree on orch/HB-TESTHUB-OFF-REMOVE, before the edit):
`git grep -n "PROYECTOS_TEST_HUB" -- . ':!*RESULT*'`

Output:
```
re/orchestrator/ledger/events.jsonl:4691: ... "note":"... test-slot: set PROYECTOS_TEST_HUB=off for game runs" ...   (ledger history, left as is)
verify/rwcthunk_20261113/city_arms.py:27:    env = dict(os.environ, PROYECTOS_TEST_HUB="off")
```
Only `city_arms.py` was a live script hit. No .sh/.ps1/.bat/.claude hits. After the edit `city_arms.py` has no match.
`py -3.12 -m py_compile verify/rwcthunk_20261113/city_arms.py` exit 0.

## Verbose test-slot check
Command: `PROYECTOS_TEST_HUB_VERBOSE=1 py -3.12 <proyectos_test_slot.py> py.exe -3.12 verify/rwcthunk_20261113/city_arms.py`
(no arguments, so even if it ran it would exit on the missing data dir and launch no game).

Output:
```
[test-hub] running locally: hub offline (10.1s exit 255: ssh: connect to host 192.168.68.114 port 22: Connection timed out)
[test-slot] waiting for memory (0s so far): memory: 20.1 GB committed + 8 GB for this run > 24.0 GB budget
... (same line every 30 s up to 180 s)
```
Result: the run stayed local, reason printed: notebook unreachable over SSH at that moment (not a rule exclusion). It then
queued on the memory gate. I stopped it with TaskStop (my own task) after 180 s. The notebook did not fail a dispatch, so
`dispatch.log` was not needed. Never set `PROYECTOS_TEST_HUB=off` in this row.

## Not done
No full arm run (would launch the game and needs a data dir, private root and mashed_lock).
