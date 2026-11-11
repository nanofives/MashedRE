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

## Second pass (TL-MATCH-479330-B, 2026-11-10): PARKED at 97.2%

This was the last pass the brief allows: the instance loop only, three tries. Each was scored with
`match.py --bytes`. None beat 97.2%, so `re/match/s12/00479330.c` is unchanged.

| try | change | score | frame |
|---|---|---|---|
| 1 | the lights block declares its own count; the instance loop uses a plain `int count` and `int tmp[6]` | 92.8% (regs 92.8%, ops 99.5%), 823/825 ins | 0x448 |
| 2 | one wrapper block around the instance `for` holding count / buf / tmp | 92.8% (same bytes as try 1) | 0x448 |
| 3 | `count` in a two-int struct `{count, pad}` whose address is never taken, plus a separate `tmp[6]` | 93.1% (regs 93.2%, ops 99.5%), 823/825 ins | 0x44c |

Tries 1 and 2 put `count` in EDI as the original does. In every block shape or scope tried, though, its stack slot
does not land inside the lights array's space. The frame then grows by 4 bytes and every later stack offset moves.

**Parked:** 0x00479330 stays at **97.2%** (regs 99.5%, ops 99.6%), 826/825 ins, 72 bytes, C2. The residual is the one
recorded above: in the instance loop 0x00479c40..0x00479d59 the original keeps `count` (EDI) and `buf` (ESI) in
registers across the two calls and stores them at loop entry. Ours reloads `count` from an address-taken struct
slot. The open problem is a plain register `count` whose slot still overlaps the lights array. No round three.
