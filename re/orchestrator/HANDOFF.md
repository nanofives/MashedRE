# Mashed RE orchestrator — resume point (updated 2026-07-27, menu-nav session)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

Status brief — C1 797 / C2 4035 / C3 846 / **C4 185** (one demotion this session: `0x0042ee00`).

---

## HEADLINE: the menu-navigated race path had NEVER been run with hooks installed

It found **four defects in one sitting**. All prior acceptance was structurally blind to them:

- `scenario_launch.py` pokes `DAT_00771968=2` and **bypasses the menu-driven loader**.
- The ~71 s attract/menu acceptance **never leaves the frontend**.

So colour-select → track-select → race-entry had simply never executed with the hook set live.

| # | fault | root cause | status |
|---|---|---|---|
| 1 | AV `0x004c5c00+0x2d`, read @ 0 | `0x0042ee40` + `0x0042ee00` are **argument-rewriting tail-JMP thunks**; both ports guessed the ABI and passed an int where a `const char*` sprite NAME belongs | FIXED (verbatim asm) |
| 2 | AV `0x0045c6c6`, write @ `0x02acdfe0` | `0x0045c640` clobbers **EDX**, which its *unhooked* caller carries across the call as a loop index | FIXED (verbatim asm) |
| 3 | HANG at race entry | `0x00448700`'s C loop kept its counter in **EAX across a call**; the callee's declared `uint32` return reloaded it → infinite loop | FIXED (verbatim asm) |
| 4 | STILL WEDGES at race entry | main thread now **blocked in an ntdll wait**, not spinning | **OPEN — U-9025** |

**Stock control is clean 2/2** (full 45 s round, `phase 0→2→3`, results hooks firing) against hooked 2/2 failing — every attribution above is stock-vs-hooked at matched timing, never "did it crash".

### Two evidence lessons worth keeping

1. **Install-verification is not behaviour-verification.** `0x0042ee00` was **C4** on "E9+rel32 verified / interceptor fired 3/3 / GREEN 10/10". Every one of those confirms the hook is *installed* and that *return values* matched — none inspects the **argument handed to the callee**. Demoted C4→C3.
2. **A value diff cannot see a register clobber.** `0x0045c640`'s "10/10 GREEN" compares the global writes, which were always correct. This class has now hit **nine** times and no gate detects it.

### Method notes

- **PC sampling was misleading**; the **stack walk** identified the real driver. For layer 3, disassembling **our own `.asi`** (not the original) is what root-caused it — the bug was in the *emitted* code, invisible in the C source.
- `ESI == 0x32` identical across **three separate processes** is what proved "stuck" rather than "sampled mid-flight". Repeat-across-processes is the cheap determinism test.
- Filter the exception handler to `type=='access-violation'` — C++ `throw` surfaces as a KERNELBASE `RaiseException` on this path and drowns the signal.

---

## NEW: `statenav.py` is now the menu-nav acceptance driver

`re/frida/statenav.py` (the only harness that drives the real frontend to a race) gained:

- `--hooks` — leave the dev `.asi` **ARMED** (default stays stock, input-drive only)
- `--count-export NAME[,NAME]` — count entries into **our port by its `.asi` EXPORT**, not the patched RVA (an inline JMP makes an RVA-anchored probe ambiguous). Arming retries after resume because the `.asi` loads *after* spawn.
- detach handler printing `GetExitCodeProcess` (this is how the AV was distinguished from a clean exit — **no WER dump is produced** on this path)
- in-process `Process.setExceptionHandler` AV catcher (pc, module, fault address, registers, stack)

`MASHED_HOOK_SKIP=<Name>` (exact-token denylist, already in `HookSystem.cpp`) is the bisect knob — `statenav` inherits the environment, so no code change is needed.

**Frida 17 note:** the static `Module.findExportByName` is REMOVED; use `Process.findModuleByName(m).findExportByName(n)`.

---

## NEXT — recommended order

1. **U-9025 — the layer-4 wedge.** Re-run `--hooks`, walk the blocked thread, and `MASHED_HOOK_SKIP`-bisect the hooks on the chain `0x005ab63d / 0x005b1526 / 0x00551591 / 0x0045d40f / 0x0049270a`. Establish first whether it is hook-caused at all.
2. **`0x00442cbd` is STILL not behaviourally confirmed.** Every run so far died or wedged before the counters printed. `--count-export LoadingState2Enter` is wired and arms correctly; it just needs a run that survives to the round.
3. **Sweep the two register classes** — this is the highest-leverage systematic work:
   - ports whose original preserves registers that compiled C does not (9 instances);
   - ports whose **declared return type differs from the original's** and are called from a C loop (the new `0x00448700` class).
4. **`LobbySlotListRender` (`0x00439210`) is a fabricated scaffold** — invented draw coordinates (330.0f/64.0f/80.0f appear nowhere in the original), a wrong callee set, and `[UNCERTAIN U-k2-01]` on six signatures. It was **exonerated** as the crasher (skipping it reproduced the identical AV) but it is a live NO-GUESSING violation. The original uses `FUN_0040bb50("lock"/"check", ...)`, `FUN_00472c60`, `FUN_00472dc0` with screen-dimension-derived coordinates. A verbatim re-port is ~5.6 KB with 8 x87/register decompiler artifacts — needs its own session and an asm-vs-C decision.
5. **Lane B is quiet:** the `st0_ret_mat3_ptr`/`st0_ret_mat4x3_ptr` lane is **mined out** — all 22 frontier rows screened, no single-pointer ST0-float leaf remains. The one real signal is `0x004c3910` (Vec3Normalize, confirmed `float10` return), blocked only by a second *output* pointer; it needs a new `st0_ret_vec3_out_in_ptr` handler.

## DONE this session

- **Call-target audit re-run — CLEAN.** 2193 sites / 1762 addresses → 1541 ENTRY, 647 NO_FUNC, 5 MID_BODY, **all 5 false positives** (PIZ magic `0x005A4950`, data-base `0x00500000`×2, body-end marker `0x00470914`, SEH handler `0x004a4bc3`). Both real bugs from the prior round are gone.
- **U-9024 RESOLVED** — `0x005c6b60` is a **64-step gain-ramp target setter**, subsystem **audio** (not boot, not menu/cursor). Its sole caller `FUN_005c7330` computes left/right mixer gains and calls it as `(p+0xa4, gainL, gainR)` and `(p+0xb8, v, v)`; the struct is `[0]`=active flag `0x40`, `[1]/[2]`=current fixed-point, `[3]/[4]`=per-step delta `(target*0x100 - cur)/64`.

## OPEN GATES / STOP-AND-ASK

- **D2 renderer commitment** — OPEN (RW-subset verbatim vs `librw`).
- **D4 airborne bit-identity** — OPEN (accept A5 1-ULP float10 residual U-8991 vs naked-asm shim).

## HYGIENE

Ghidra pool: slot 1 released. **`mashed_pool/Mashed_pool0.lock~` is held open by the Ghidra MCP JVM** and cannot be deleted (`Device or resource busy`) — the `feedback_mcp_leaked_project_lock` case; clears only on restarting that server. Use slot 1+.

**Pool-script gotcha:** `ghidra_pool.sh acquire` stakes a **Ghidra-format `.lock`** to reserve the slot, and that file makes `project_program_open_existing` fail with `LockException`. Delete `mashed_pool/<Slot>.lock` after acquiring and before the MCP open; Ghidra then creates its own lock, which serves the same "taken" signal.

All MASHED PIDs spawned this session (32496, 33340, 27056, 23308) were killed by PID. No worktrees. `original/` intact.

TO RESUME: read this file; start at NEXT item 1.
