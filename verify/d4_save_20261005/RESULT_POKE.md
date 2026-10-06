# RESULT — VOID. The state machine is dormant, and the real gate is one level further out.

**RAN 2026-10-05.** Pre-registration `PREREG_POKE.md`, committed **unrun** at `f5312eba`.
**No C-level moved, no band moved, `original/` never written.**

---

## 1. KA-POKE: FAIL → VOID. Arm S was not run.

The poke landed — `[poke-u32] CONTRIVED -> {'008a9588': 13}`, read back from the target. But:

| sample | `008a9588` (selector) | `00803358` (buffer magic) |
|---|---|---|
| +0.0 s, +3.6 s (pre-poke) | **0** | 0 |
| +7.2 s … +18.0 s (post-poke) | **13, 13, 13, 13** | **0, 0, 0, 0** |

**The selector never advances to `0xf`.** `switchD_00409b0e::caseD_d` sets `DAT_008a9588 = 0xf` on
its way out, so if the case had run even once the value would have changed. It does not, on four
consecutive samples over 11 s.

**Conclusion: the dispatcher at `0x00409b0e` never runs.** The save state machine is **dormant** at
the menu — it sits in state **0** before the poke and does not tick. Writing its selector
accomplishes nothing because nothing dispatches on it.

Per the registered gate this is **VOID** and **no statement is made about arm S**.

**My registered prediction (ACCEPTED on arm S) is neither confirmed nor refuted** — for the fourth
time on this question.

## 2. What this does establish

Two things, both measured rather than inferred:

- **The selector's resting value at the menu is `0`**, and it is stable. The machine is parked, not
  cycling.
- **Setting the selector is not sufficient.** The gate is not the state value — it is **whoever
  calls the dispatcher**. I traced one level (selector → case → `SaveLoad`) and the live gate is one
  level further out than where I looked.

Also worth keeping: the earlier `pokeu32` failure was **not** a crash, it was frida-python's
camelCase→snake_case export mapping (`pokeU32` → `poke_u32`), the same convention this file already
uses for `poke_ctrl_slots`. A two-peek run that dies at the poke looks exactly like a crash; it was
a naming error, and the log said so.

## 3. The precise next step

**Find the caller of the dispatcher**, i.e. the function containing `0x00409b00`/`0x00409b0e`, and
what gates *it*. `decomp_pc.py --create` reads that region (Ghidra has not defined functions there —
the same gap that made `--callers` return `(none)` for all three save functions). The other two
readers of `DAT_008a9588` are at `0x0040aa60` and `0x0040abc0` (the latter inside the *defined*
`FUN_0040ac80`), and **`FUN_0040ac80` is the one lead that is already a named function** — it is the
cheapest place to start.

## 4. The blocker sequence, complete

Every step measured, none guessed:

1. `0x00803358` — wrong address class (transient buffer). VOID.
2. `*0x008a94a8` — superseded, never run.
3. `0x007F0A40`, race-warped — right address, wrong harness. VOID on a degenerate control.
4. `0x007F0A40`, menu hold — right harness, file still not read.
5. nav scan — **a phantom**: push works; my reading lacked a baseline.
6. screens are the wrong dimension — the trigger is a state machine (`0x00409b0e`).
7. **the state machine is dormant; the gate is its caller.**

**Seven steps, six of them real.** The question — *does the original accept the standalone's
`gamesave.bin`* — remains unanswered, and is now bounded to a single named unknown.

## 5. Unaffected

Everything else from today's save work stands, because none of it needed the original to load
anything: the four `GameSave` functions are live in the standalone (5/5 and 3/3 gates), the file
layout is fully resolved, `gamesave_parse.py`'s live constant is fixed, and the `--no-warp` /
`--poke-u32` harness additions are reusable and documented as contrived where they are.
