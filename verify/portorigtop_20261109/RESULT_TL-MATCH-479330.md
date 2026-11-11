# RESULT TL-MATCH-479330 (2026-11-10): pilot pass 35.6% -> 97.2%, near miss, one more short pass recommended

Verdict: **one structural pass (L0 timebox) took 0x00479330 from 35.6% to 97.2%, well above the ~60% bar.** It is
not byte-identical, so it stays C2 / not promoted. The residual is confined to one loop.

- **Source:** `re/match/s12/00479330.c`, the by-hand rewrite of the raw g2s output, DDK `/O2 /GS /Ob1`.
- **`match.py --bytes`:** `97.2% (regs 99.5%, ops 99.6%) 826/825 ins`, 2894 B whole symbol, 72 differing unmasked
  bytes, first at +0x932.
- **relocheck:** BAD, `checked=129`. The first BAD site, +0x95f, follows the one-instruction length change at +0x932
  (positional, as in POWERUPINIT-B2). The 129 sites before it agree.
- **Who did the pass:** a local subagent (opus / high), reviewed and re-scored here. It made no mashedmod/ or tracker
  change.

## Shape (what moved the score)

| step | score |
|---|---|
| raw g2s | 35.6% |
| typed records for the 3 params (Course, the 0x40-byte name tables, CourseDef with float fog / ambient fields: `fld` not `fild`), index loops, per-branch printf tails, `switch` on the light type byte, MATCH-HELD static copy of the register-arg 0x004790e0 (world in EAX) | 50.5% |
| the out-params of the uva calls at function scope (frame 0x444 as the original) | 53.5% |
| the final flag store as `if / else` then one `return 1` (fixes a whole-function esi/edi/ebx rotation) | 91.9% |
| `count` and the FUN_00485460 output buffer in one block-scope struct (shares the lights array's stack space) | 96.0% |
| `buf += 0x40` in the `for` header | **97.2%** |

## Residual (only the instance loop, original 0x00479c40..0x00479d59)

- **Register vs memory for two loop values.** The original keeps `count` in EDI and `buf` in ESI across the two calls
  before the inner loop, and stores them only at loop entry (`mov [esp+0x1c], esi; jmp`). Ours reloads `s.count` after
  the calls, because the struct's address is taken, and stores `buf` at once. The result: +1 instruction, a missing
  `jmp`, and register-name and push-order differences in the 0x004c1340 / 0x004850e0 argument setup.
- **Kind:** structural (register allocation coupled with stack-slot sharing). It is not field-only.
- **The trade-off found:** a plain `int count` reproduces the register handling, but its slot then leaves the space
  shared with the lights array. The frame grows to 0x448 and the score drops to 92.8%.

## Recommendation

**One more short pass is worth it, limited to the instance loop.** 72 bad bytes is the 24-30% yield bracket in the
ddk-match-levers note. The goal is `count` as a plain register variable whose slot still overlaps the lights array.
Tries:
- a different shape for the lights block (its own memory-resident count);
- the instance-loop locals placed inside the lights `else`.

If one or two tries do not close it, park it at 97.2%.

**Port note (for TL-PORT):** under `MATCH_PORT` the source calls `FUN_004790e0()` with no argument, while the original
passes the world in EAX. A port of this body needs an EAX bridge for that call (the same kind of calling-convention
glue as PARTICLE-3-SWAP's EDI bridge).

rva-lint: OK, 0 new. No mashedmod/ change.
