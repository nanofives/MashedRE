# PREREG D4-WSB-C4-SOLV-WS, addendum 1 (committed before arm N)

Leg H (asi `5c12899c...`, 8 boots, 24/24 samples each, all `ndiff=0`, all `A/B-IS-REAL installed=e9`, every boot
ended by the driver's SAMPLES-DONE guard):

| row | read-out | heap pages compared (sum over 24) | image pages | rv |
|---|---|---|---|---|
| `0x00568dd0` | HEAP-CLEAN | 24 | 0 | `0f58d5e0` x24 |
| `0x0056ba30` | HEAP-CLEAN | 24 | 0 | `0` x24 |
| `0x00564c80` | HEAP-CLEAN | 129 | 24 | `0f51c9b0` x24 |
| `0x00561390` | HEAP-CLEAN | 264 | 24 | `0f52ca90` x24 |
| `0x00563e70` | HEAP-CLEAN | 60 | 0 | (void) |
| `0x00568990` | CLEAN-NO-PAGES | 0 | 0 | `0` x24 |
| `0x00568fd0` | CLEAN-NO-PAGES | 0 | 0 | `0f58d5e0` x24 (returns `param_1`) |
| `0x0056b7a0` | CLEAN-NO-PAGES | 0 | 0 | `0` x24 |

`0x00568990` returns 0 before any store when `param_3 == 0 || param_5 == 0` (`RwpSolverCore21.cpp:205-207`).
`0x00568fd0` returns `param_1` before any store when `*(param_2 + 4) == 0` (`RwpSolverCore11.cpp`, the `_impl` head).

## Arm N (the 3 CLEAN-NO-PAGES rows)

Leg H recipe plus `MASHED_SHADOW_NEED_PAGES=1` (D4-WSB-SHADOW-PRIVATE's opt-in: a sample where neither run wrote a
tracked page and stack and return agree is discarded as an `[..] EMPTY rv=` line, at most 2000, then sampling stops)
and `HOLD=150` (new driver env, default 60, passed to `scenario_launch.py --hold`; the only driver change), tag
`SHADOW_TAG=needpages`. Same build, same asi.

Read-out per row:
- **HEAP-CLEAN**: >= 1 counted sample, every counted sample OK, and heap pages compared > 0. Then the row has leg H.
- **CLEAN-IMAGE-ONLY**: counted samples OK, image pages only. Stays C3 (PREREG rule).
- **DIVERGENT**: any counted sample divergent. Stays C3, values reported.
- **EMPTY only, all rv = 0** (`0x00568990`, `0x0056b7a0`): **NO-WRITES-SAMPLED**, stays C3. EMPTY lines with rv != 0
  and no counted sample: **BUFFER-UNTRACKED**, stays C3, the buffer goes to the next step.
- **EMPTY only** for `0x00568fd0` (rv is always `param_1`, so it cannot separate the two cases): **NO-WRITES-SAMPLED
  [UNCERTAIN]**, stays C3.
- Hang / no samples / no log: re-run once.

The promotion rule of the PREREG is unchanged.
