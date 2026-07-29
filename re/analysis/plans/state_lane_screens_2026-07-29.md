# STATE lane — three screens, and why the batch is 0 hooks (2026-07-29)

## The screens

| # | script | question | cost |
|---|---|---|---|
| 1 | `prescreen_batch.py` | does the scenario CALL it? | 1 boot / ~24 candidates |
| 2 | `shape_screen.py` | is it mechanically callable? | free (capstone + import table) |
| 3 | `semantic_screen.py` | is calling it twice harmless and repeatable? | free (plates + disasm) |

## Funnel on the 96 screened rows

```
187  STATE pool (C2, plated, unregistered, 8..60 B)
 96  screened for exercise                    (91 left, needs a reboot)
 49  exercised (51%)  — 20 race-gated, 29 pre-race
 26  shape `direct`   — 5 race-gated, 21 pre-race
  6  semantically safe (of the 21 pre-race)
  0  survive hand-reading
```

**The batch is zero hooks, and that is the correct answer.** Each screen was written because the
previous one let something through, and the last filter — reading the disassembly myself — still
rejects five of six.

## What hand-reading rejected, and why each screen missed it

| RVA | screens said | disassembly says |
|---|---|---|
| `0x004b9600` | direct, SAFE | `if (tag out of range) errorFormat(L, str, tag)` — **void, no observable**. A diff can only compare a return value that does not exist. |
| `0x0045d430` | direct, SAFE | `if (gate) { FUN_005a60e0(); gate = 0; }` — the hazard is in the CALLEE's plate (teardown), and it **clears its own gate**, so call two is a no-op. Not repeatable. |
| `0x005a9de0` | direct, SAFE | writes `*(*(a+0x4c)) = b` — mutates live state through a pointer argument. |
| `0x00425b90` | direct, SAFE | `FUN_004e66d0(*(p), 0x4516d0, &local)` — a **callback iteration**; the side effects belong to the callback, which the screen never looks at. |
| `0x004c0ed0` | direct, SAFE | returns `p + 0x50` — **trivially derivable from the input**. A port that deletes the conditional `FUN_004d8350(o)` call entirely still goes GREEN. That is memory `feedback_evidence_discipline` §2: *which line could I delete and still pass?* |

Only `0x004c1be0` survives, and one hook is not a batch.

## The real conclusion

**The C2 STATE pool is dominated by procedures, not functions** — initialisers, teardowns,
dispatchers and mutators. Those are exactly the things a synthetic A/B cannot judge: they have no
return value to compare, or calling them twice is not the same as calling them once.

This is the same wall `project_c2c3_pipeline_lessons` already recorded as *"the flat C2→C3 lane is
MINED OUT"*, reached from a new direction and now with a mechanical explanation rather than an
observed yield drop.

### What would actually move it

1. **Observation-based verification, not force-call.** For a procedure, the evidence is *"when the
   game called it naturally, ours produced the same writes"*. That is the installed-hook path
   (`run_verify_hook` / the A/B orchestrator lane), not the synthetic path. It is also, per
   `re/CONFIDENCE.md`, the only route to C4 anyway.
2. **Screen for an OBSERVABLE, not just for safety.** A fourth cheap check: does the function
   return a value that is not a pure function of its arguments? `0x004c0ed0` fails it; that is
   what makes it worthless as a synthetic target despite being safe.
3. **Finish screening the remaining 91** — the funnel rate suggests roughly 5 more semantically
   safe candidates, so this is worth one reboot's worth of boots but will not by itself produce a
   batch.

## Screens are still worth having

They cost nothing to run and they caught real hazards before any of them reached a batch:
`CloseHandle`, `WaitForSingleObject`, two `EnterCriticalSection` pairs, a DirectInput teardown, an
asset re-loader, and a timer read that would have false-RED'd a correct port. The one thing they
must not be used for is to *replace* reading the function.
