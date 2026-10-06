# RESULT — VOID. My control was degenerate, and the positive control I nearly skipped caught it.

**RAN 2026-10-05.** Pre-registration `PREREG_ACCEPT2.md`, committed **unrun** at `8c3cf7c6`.
`--peek` only, no Interceptor. **`original/` never written** — SHA-256 `BD18788182B2343E…` before and
after, `git status` clean. **No C-level moved, no band moved, no code changed.**

---

## 1. The three arms

| arm | save given to the original | live `0x007F0A40` table (r0c1, r0c4, r2c4, r0c11) |
|---|---|---|
| **C** control | the shipped `gamesave.bin` (span `1, 2, 2, 1`) | **1, 2, 2, 1** on 6 of 6 samples |
| **S** subject | the standalone's (span `0, 2, 0, 0`) | **1, 2, 2, 1** on 6 of 6 samples |
| **P** positive control | the shipped save with **r2c1 edited 0 → 1** | `r2c1` = **0** on 4 of 4 samples |

**Arm P is the one that matters.** Its file provably carries `1` at `r2c1` (`gamesave_edit.py`
reported *"exactly 1 bytes differ, all intended"*, file offset `0x24aa4`, and I read it back). The
live table reads **0**.

## 2. Verdict: **VOID**

**Saves are not loaded at all in this harness.** The live `1, 2, 2, 1` is a default written by code,
not by the save loader.

Therefore:

- **KA-LOAD passed SPURIOUSLY.** Arm C matched the original file's span only because **the shipped
  save's span happens to equal the defaults**. The control could not distinguish "loaded the save"
  from "never loaded anything".
- **Arm S's result is meaningless.** It would have read `REJECTED/IGNORED` by the registered gate —
  the three discriminators showing the original's values — and that reading is **void**, not a
  finding. Reporting it as "the original rejects our save" would have been wrong.
- **My registered prediction (ACCEPTED) is neither confirmed nor refuted.**

## 3. What I got wrong, specifically

I designed a positive control in the first `PREREG_SAVE.md` and then **dropped it** when the
fixed-address `0x007F0A40` route looked clean, because peeking a known table seemed self-evidently
sufficient. It was not: **a control whose expected value is indistinguishable from the default is
not a control.** Every value in arm C — `1, 2, 2, 1` — is exactly what an unloaded table shows.

The rule this session keeps re-learning, in a new costume: I checked that the control *passed*, not
that it *could have failed*. Arm P is the arm that could fail, and I only ran it because the arm C /
arm S agreement looked suspicious.

## 4. Why the harness cannot answer this

`scenario_launch.py` warps the original **straight into a race**, bypassing the frontend/menu path
where the save is read. That is consistent with the other observation from earlier today: the
serialization buffer at `0x00803358` read **0** on every sample of a race-warped run
(`RESULT_SAVE.md` §2) — the same cause, found twice from different directions and only now explained.

**So the question needs a boot-to-menu harness, not a better address.** `scenario_launch.py` has no
such mode (`--track` is mandatory in effect); adding one, or driving the menu with the existing nav
recipe, is the prerequisite. That is a harness change and belongs in its own leg.

## 5. Where this leaves the save thread

- **"Does the original accept the standalone's save" is still UNKNOWN**, and has been through three
  attempts today — `0x00803358` (wrong address), `*0x008a94a8` (never run), `0x007F0A40` (right
  address, wrong harness). The blocker is now identified and is **the harness, not the instrument**.
- **Everything else from the save thread stands** and is unaffected, because none of it needed a
  running game: the four `GameSave` functions are live in the standalone with 5/5 and 3/3 gates
  (`RESULT_GAMESAVE_EXE.md`, `RESULT_WIRE.md`), the file layout is fully resolved
  (`RESULT_U3559.md`), and `gamesave_parse.py`'s live constant is fixed.
- No tracker row changed. U-3559 remains resolved; nothing new is filed, because a void leg is not a
  finding.
