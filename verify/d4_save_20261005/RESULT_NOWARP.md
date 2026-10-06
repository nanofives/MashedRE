# RESULT — `--no-warp` built and working. The save is STILL not read, and that is now a measurement.

**RAN 2026-10-05.** Harness change to `re/frida/scenario_launch.py` (`--no-warp`). **No C-level
moved, no band moved, no tracker row changed, `original/` untouched.**

---

## 1. The harness change, and why it is one line of behaviour

`RESULT_ACCEPT2.md` concluded the blocker was the harness: `scenario_launch.py` warps straight into
a race, bypassing the frontend. The warp turned out to be **a single call** — `E.launch()`, poking
`DAT_00771968 = 2` at `:3157`. `--no-warp` gates it and holds at the menu for `--hold` seconds,
peeking on the same cadence as the race loop (every 6th 0.6 s tick), so menu-side and race-side
output are directly comparable.

**It works**: the run reaches `phase=1` and prints `*** HOLDING AT MENU (no warp) ***`.

A first attempt produced **no peek output at all** — the peek lives inside the race hold loop, which
my early exit skipped. Fixed by giving the no-warp hold the same peek call; recorded because a
silent "no output" would have looked like a null result rather than a harness bug.

## 2. The positive control still fails — and now so does the buffer

Arm P is the shipped save with **`r2c1` edited `0 → 1`** (`gamesave_edit.py`: *"exactly 1 bytes
differ, all intended"*, file offset `0x24aa4`).

| peek | meaning | expected if the save is loaded | measured, menu hold, 7 samples |
|---|---|---|---|
| `0x007f0aa4` | live champ table r2c1 | **1** | **0** |
| `0x007f0a44` | live champ table r0c1 | 1 | 1 (= the default) |
| **`0x00803358`** | **serialization buffer magic** | **`0xDEADBEEF`** | **0** |

**The buffer magic is `0` on every sample.** That is the decisive one: it means
`gamesave.bin` is **never read** in this configuration — not merely that the table is populated from
somewhere else.

So **the warp was not the only blocker.** Removing it was necessary and not sufficient.

## 3. What this does and does not license

**Established:** with the patched `original/MASHED.exe`, held at the main menu for ~26 s under
Frida, **the save file is not read and the live championship table stays at code defaults.**
Measured on three arms now (C, S, P) across two harness modes.

**NOT established, and deliberately not guessed between:**

- the read happens on a **deeper screen** than the top menu (my hold never navigates);
- a **boot patch** suppresses it (`patch_mashed_fix_fopen.py` NULLs garbage `FILE*` returns — it
  should not affect a valid open, but it has not been ruled out);
- the read happens **before the Frida attach** and the buffer is cleared after deserialize — though
  this is in tension with arm P, where the *table* also lacks the edited value.

## 4. A standing claim in the repo is now in doubt

`re/tools/gamesave_edit.py:16-18` records:

> *"LINKAGE (measured 2026-08-30): the shipped save's span col1/col11 signature — only row 0 set —
> is IDENTICAL to the live launch gate `DAT_007f0a40` the race/nav-champ probe read, so editing span
> col1 propagates to the gate on load."*

**The premise is an identity of values, not a demonstration of causation** — and the shipped save's
span equals the code defaults, so observing that they match cannot distinguish "loaded" from "never
loaded". That is the same degeneracy that voided my own arm C.

**Arm P is the first test that could falsify it, and it did not propagate.** I am flagging this as
**in doubt and needing re-measurement**, not refuted: the 2026-08-30 reading came from the
*nav-champ probe*, which navigates deeper into the championship flow than my top-menu hold. It is
entirely possible the linkage holds there and not here. **Whoever re-measures should use an edited
save, not the shipped one.**

## 5. Where the save thread stands after today

**Still unanswered: does the original accept the standalone's `gamesave.bin`?** Four attempts, and
each one moved the blocker rather than solving it:

1. `0x00803358` — wrong address class (transient buffer). VOID.
2. `*0x008a94a8` — never run; superseded.
3. `0x007F0A40`, race-warped — right address, wrong harness. VOID on a degenerate control.
4. `0x007F0A40`, menu hold — **right address, right harness, and the file is still not read.**

The next step is no longer an address or a flag: it is **navigate the frontend far enough to trigger
the load**, with the arm P save in place and `0x00803358` watched as the witness. `nav_agent.js` and
`orig_nav_hold.py` already drive the menus; neither honours `MASHED_ROOT`, which is the one gap.

**Everything else from today's save work is unaffected**, because none of it needed the original to
load anything: the four `GameSave` functions are live in the standalone (5/5 and 3/3 gates), the
file layout is fully resolved, and `gamesave_parse.py`'s live constant is fixed.
