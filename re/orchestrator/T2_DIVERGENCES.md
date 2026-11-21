# T2 known divergences (ROADMAP.md "T2", Exit: every divergence filed with its first tick, address and cause)

One row per divergence the replay oracle shows on the menu-less race against ref1. A row here is a known divergence,
not a race-code fault. Remove a row only when the oracle no longer shows it.

| word | first tick | writer on the original (address, function, tick) | cause | effect on the knob-on arm | evidence | follow-up |
|---|---|---|---|---|---|---|
| `0x0067e9f8` | 16 (menu phase, before the warp) | `0x0043d7a3` in `0x0043d2a0`, tick 16 | menu body is the L-168 stand-in (`0x0043dfd0` hand-menu stand-in allowed while MATCH-43DFD0-RAISE runs) | the knob-on IDENTICAL-FOR-N count stops at record 16 until the menu body lands | `verify/racewriter_20261121/RESULT_RACE-WRITER-0067E9F8-E848.md` (branch orch/RACE-WRITER-0067E9F8-E848 f52ef86be) | RO-READERS-0067E9F8-E848 (run 55): read-watch from the warp to the end of ref1 |
| `0x0067e848` | 162 (menu phase, before the warp) | `0x0042e6ad` in `0x0042e5b0`, tick 162 | menu body is the L-168 stand-in (same as above) | as above, after record 16 | same RESULT | RO-READERS-0067E9F8-E848 (run 55) |

Neither word is seeded and no T1 row is opened for them (parent, run 54): a seed is a mask-class change for Mariano with
reader evidence, and a T1 replacement of `0x0043dfd0` runs into L-168. A seed or a T1 row becomes a decision only if
RO-READERS-0067E9F8-E848 names a race-load or race-tick reader.
