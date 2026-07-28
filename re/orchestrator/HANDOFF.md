# Mashed RE orchestrator — resume point (updated 2026-07-28, menu-nav session)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

Status brief — C1 797 / C2 4035 / C3 847 / **C4 185** (one demotion: `0x0042ee00`).

---

## HEADLINE: the menu-navigated race path had never run with hooks installed

It found **six defects**. All prior acceptance was structurally blind to them:

- `scenario_launch.py` pokes `DAT_00771968=2` and **bypasses the menu-driven loader**
- the ~71 s attract/menu acceptance **never leaves the frontend**

So colour-select → track-select → race-entry had simply never executed with the hook set live.

| # | fault | root cause | status |
|---|---|---|---|
| 1 | AV `0x004c5c00+0x2d`, read @ 0 | `0x0042ee40` + `0x0042ee00` are **arg-rewriting tail-JMP thunks**; both ports guessed the ABI and passed an int where a `const char*` sprite NAME belongs | FIXED (verbatim asm) |
| 2 | AV `0x0045c6c6`, write @ `0x02acdfe0` | `0x0045c640` clobbers **EDX**, which its *unhooked* caller carries across the call as a loop index | FIXED (verbatim asm) |
| 3 | HANG (spin) | `0x00448700`'s C loop kept its counter in **EAX across a call**; the callee's declared `uint32` return reloaded it | FIXED (verbatim asm) |
| 4 | HANG (blocked) | `0x00415e20` read **3 qword constants as f32** — `57.2958 → 1.08e-19`, `∓1.0 → 0.0` | FIXED, but still wedges later |
| 5 | wrong value | `0x00426e00` read a **dword constant as a double** (the tracker asserted the wrong width; the port inherited it) | FIXED |
| 6 | **U-9025** race-entry wedge | **AUDIO-HOOK INTERACTION** — see below | **OPEN** |

**Controls:** stock completes the full 45 s round **3/3**; hooked **0/3**. Every attribution is a matched
stock-vs-hooked pair, never "did it crash".

---

## U-9025 — attributed, not closed

Two independent methods converged on **audio**:

- **Stack:** main thread **blocked in an ntdll wait** (`IsHungAppWindow=true`), chain
  `0x005ab63d / 0x005ab15b / 0x005b1526 / 0x005b1ef3 / 0x005ab23a / 0x005ab83b / 0x00551591 /
  0x00551146 / 0x00550c15 / 0x0045d40f / 0x0049270a / 0x004922c2 / 0x004923de` —
  **identical frame-for-frame across three separate processes**.
- **Bisect:** range `[75,150)` **fails as a set while both halves complete** → needs ≥2 hooks
  co-installed. All 75 are audio, in the **same `0x005ab..`/`0x005b1..` band as the stack**.

Blocked-not-spinning is what a broken audio path looks like (waiting on a buffer/mixer thread), and an
interaction explains why `MASHED_HOOK_SKIP` by name and every single-hook search found nothing.

**NEXT:** pin `[75,112)` installed and bisect `[112,150)`, then swap (~12 runs); identify the wait
object; then fix by name.

**Caveat:** the FIRST bisect ran with a loose completion predicate (accepted `round t+30`), so its branch
decisions are suspect — index 616's isolation path may have been luck. The byte-level width defect in
`0x00415e20` is independently proven regardless. The predicate is now strict (`FINAL:` only).

---

## Systematic sweeps — committed, self-tested

Every detector is validated against a known instance first; a zero-finding sweep is worthless otherwise.

| sweep | scope | result |
|---|---|---|
| **A** register-ABI (`sweep_reg_abi/rega_eval/rega_analyze/rega_rank.py`) | 1195 hooks → 1073 plain C → 380 candidates | **16 RVAs / 27 sites, none fixed yet** |
| **B** naked-thunk/EAX (`sweep_classb_emitted.py`) | 204k + 261k instructions | 0 |
| **C** constant width (`sweep_const_width.py`) | 405 float reads / 1143 x87 addresses | found `0x00426e00` |

Sweep A tiers: **9 ECX/EDX** (`0x0040dc80` `0x0041f360` `0x00426e00` `0x00430760` `0x00471530`
`0x00476a30` `0x004c1a00` `0x004d5480` `0x0055deb0`), **4 EAX void-return** (`0x0042b950` `0x0042f7a0`
`0x0045b350` `0x00493580`), **3 EAX value-return to eyeball**. Byte-verified `0x00471530`.
Self-test: un-hiding the fixed `0x0045c640` reproduces it exactly.

**TOOLING BUG WORTH REMEMBERING:** `capstone.disasm()` **stops at the first undecodable byte**, and
`.text` is full of jump tables and padding — a single pass silently truncated both binary sweeps. The
width auditor's self-test caught it (1 of 3 expected constants). A resync loop took the `.asi` scan from
23,901 → **204,135** instructions. Class B was re-run only after that fix.

---

## Harness

`re/frida/statenav.py` is the menu-nav acceptance driver:
`--hooks` (arm the dev `.asi`; default stays stock), `--count-export NAME` (count our port by its `.asi`
EXPORT — an inline JMP makes an RVA probe ambiguous), exit-code on detach (**no WER dump is produced**
on this path), and an in-process AV catcher filtered to `access-violation` (C++ throws surface as
KERNELBASE `RaiseException` here and drown the signal).

`scripts/bisect_hooks.py` — index bisect. Strict `FINAL:` predicate; aborts if the baseline doesn't
reproduce; reports interaction instead of forcing a single answer; kills **only** the MASHED pids that
appeared during each run. Manifest: `log/hook_index_manifest.txt` (1205 entries) via
`MASHED_HOOK_MANIFEST`. **Indices shift when hooks are added — regenerate before reuse.**

**Frida 17:** static `Module.findExportByName` is REMOVED — use
`Process.findModuleByName(m).findExportByName(n)`.

---

## NEXT — recommended order

1. **U-9025** — narrow the audio interaction (above).
2. **Sweep A's 9 ECX/EDX ports** — fix by verbatim transcription; this class has now hit ten times.
3. **`0x00442cbd` is STILL not behaviourally confirmed.** `--count-export LoadingState2Enter` is wired
   and arms correctly; every run so far died or wedged before the counters printed.
4. **`LobbySlotListRender` (`0x00439210`)** is a fabricated scaffold — invented draw coordinates
   (330.0f/64.0f/80.0f appear nowhere in the original), wrong callee set, `[UNCERTAIN U-k2-01]` on six
   signatures. **Exonerated** as the crasher (skipping it reproduced the identical AV) but a live
   NO-GUESSING violation. A verbatim re-port is ~5.6 KB with 8 x87/register decompiler artifacts —
   needs its own session and an asm-vs-C decision.
5. **Lane B is quiet:** the `st0_ret_mat*` lane is mined out (all 22 frontier rows screened). The one
   real signal is `0x004c3910` (Vec3Normalize, confirmed `float10` return), blocked only by a second
   *output* pointer; needs a new `st0_ret_vec3_out_in_ptr` handler.

## DONE this session

- **Call-target audit re-run — CLEAN** (2193 sites / 1762 addresses → 1541 ENTRY, 647 NO_FUNC,
  5 MID_BODY, all 5 false positives). Both real bugs from the prior round are gone.
- **U-9024 RESOLVED** — `0x005c6b60` is a **64-step gain-ramp target setter**, subsystem **audio**
  (not boot, not menu/cursor); sole caller `FUN_005c7330` computes left/right mixer gains.

## OPEN GATES / STOP-AND-ASK

- **D2 renderer commitment** — OPEN (RW-subset verbatim vs `librw`).
- **D4 airborne bit-identity** — OPEN (accept A5 1-ULP float10 residual U-8991 vs naked-asm shim).

## HYGIENE

**The SCREENSAVER blanks the display and MASHED then exits `0xFFFFFFFF` ~4 s into boot** — no crash
dump, stock and hooked alike, even with every injected DLL removed. It cost a long detour through
compat flags / `repatch_original` / `diag doctor` before the user identified it. The tell: a fast
`exit(-1)` with **no dump** is a clean refusal to start (device/display creation), not a fault.
`GetSystemMetrics` still reports the monitors, so that check does not catch it.

Ghidra pool: slot 1 in use this session. **`mashed_pool/Mashed_pool0.lock~` is held open by the Ghidra
MCP JVM** and cannot be deleted — the `feedback_mcp_leaked_project_lock` case; clears only on restarting
that server. Use slot 1+.

**Pool-script gotcha:** `ghidra_pool.sh acquire` stakes a **Ghidra-format `.lock`** that makes
`project_program_open_existing` fail with `LockException`. Delete `mashed_pool/<Slot>.lock` after
acquiring and before the MCP open.

All MASHED PIDs spawned this session were killed by PID. No worktrees. `original/` intact.

TO RESUME: read this file; start at NEXT item 1.
