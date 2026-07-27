# Mashed RE orchestrator — resume point (updated 2026-07-27, end of session)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

**ALL WORK COMMITTED AND PUSHED.** `main` == `origin/main`. 18 commits this session.
Tree clean. No worktrees. No MASHED processes. Ghidra slot 0 released, `.pool_slot` removed.

Status brief — C1 797 / **C2 4035** / **C3 846** / C4 186.

---

## HEADLINE: the ~71 s runtime AV is CLOSED (8/8 clean)

It was **four INDEPENDENT hook defects**, not one bug and not a memory corruptor, all stacked on
the same ~71–73 s attract/menu path. Each fix exposed the next.

| # | fault | root cause | commit |
|---|---|---|---|
| 1 | `eip=0x0047bda0` read `0x1c` | `Match47bc90_RegAbi` clobbered EDX, which the *unhooked* caller `FUN_0047bcc0` carries across the call | `d9f61dd5` |
| 2 | `eip=0x004e66db` NULL clump | `0x0041f330` installed as plain `__cdecl` though the original reads its key from ESI → returned 0 | `1ed01345` |
| 3 | write AV `0x0cfcf000` | `0x00475a60` missing indirection → `memset(_,0,~223 MB)` | `e80b91f6` |
| 4 | `eip=0x004a3222` | `0x00408a70` called a **guessed** RVA (`0x004a3220`) that landed mid-instruction | `7518a088` |

**Proven hook-caused** — an A/B nobody had run before: hooks installed 5/5 AV at 71.7–73.0 s vs
`MASHED_RE_NO_AUTO_HOOK=1` 0/3 over 96 s. The old memory wrongly framed it as pre-existing; the
memory is now `project_71s_av_closed`.

**Acceptance:** full 1203-hook set, 4 runs @95 s + 4 runs @130 s = **8/8 survived** to the harness
kill (every exit `0xFFFFFFFF`), against a prior deterministic 4/4 AV. The 130 s runs clear ~2x the
old failure window.

### Methods worth reusing (all cheap, all paid off)

1. **Bisect classified by FAULT SIGNATURE (eip), never "did it crash."** Partial hook sets threw
   two unrelated faults (`0x6f5bb3ac`, `0x0045bfb5`) that would have sent a crash/no-crash bisect
   down the wrong branch. 9 steps isolated a single registry index.
2. **Read the crash frame out of the minidump instead of bisecting.** Layer 3 was solved with zero
   extra runs: resolve the faulting address to its module (it was our own `.asi`), then read
   `[esp]`/`[esp+4]`... from the dump's memory streams to recover the callee's real arguments and
   return address. Scratch tools: `which_module.py`, `nearest_export.py`, `read_stack.py`,
   `dump_export.py`, `bisect_step.sh`.
3. **Check dump-to-dump determinism BEFORE reaching for corruption tooling.** All four layer-4
   dumps were byte-identical (same eip/esp/ebp/eax/ecx). Heap corruption varies run to run — that
   one observation disproved the single-corruptor hypothesis for free and made PageHeap /
   Application Verifier (elevation + IFEO writes) unnecessary.
4. **`MASHED_COUNT_RVAS`** on `scenario_launch.py` (new, `ddb0524d`) — proves a path actually ran.
   A clean scenario run verifies nothing about a function that never executed.

---

## Guessed-address / call-target audit (`b215d21f`)

A wrong RVA compiles and links fine and only fails at runtime. Tooling committed and reusable:
- `scripts/ghidra/extract_call_targets.py` — every hardcoded call target (fn-ptr casts, `as_fn<>`,
  `kFn_*` constants, `RH_ScopedInstall` targets), filtered to the image range
- `scripts/ghidra/check_call_targets_eval.py` — READ-ONLY `ghidra_eval` pass classifying each as
  **ENTRY / MID_BODY / NO_FUNC**

2192 sites / 1762 addresses -> 1538 ENTRY, 647 NO_FUNC, 6 MID_BODY. **Two were real:**
`0x00442cbd` (a DIGIT TRANSPOSITION of `0x004a2cbd` — `a2` typed `42`) and a dead second copy of
`0x004a3220` that `7518a088` had missed.

**Lesson: the STRUCTURAL check found both real bugs; a parallel account2 sweep for hedging comments
("observed", "assumed", "not independently known") produced ONLY false positives** — its top hits
`0x4a78f4` and `0x57c2e0` are faithful transcriptions (the original literally does
`0x004a3229 PUSH 0x4a78f4` and `0x0057c27a PUSH 0x57c2e0`), i.e. Ghidra `LAB_`-not-`FUN_` gaps.
**MID_BODY is the signal; NO_FUNC is noisy.** Re-run this after any batch of ports.

**WARNING — `0x00442cbd` is STATICALLY proven only.** `LoadingState2Enter @ 0x00409900` sits on the
loading path; `MASHED_COUNT_RVAS` showed it fired **0 times** in a `scenario_launch` race, because
the launcher pokes `DAT_00771968=2` and BYPASSES the menu-driven loader. Confirming it needs a
**menu-navigated** race. The same run did give first in-race behavioural confirmation of
`0x00408a70` (2 calls) and `0x00475a60` (3 calls), both clean.

---

## LANE B — promotions and tracker

**U-9021 RESOLVED** — `FUN_004c4530` is **`RwMatrixOptimize`**: matrix + 3-float tolerance triple
(default `DAT_007d4028 + 0xc + DAT_007d3ff8`), per-slot compare, writes the RwMatrix flag word at
`+0xc`, returns `param_1`.

| RVA | tol slot | flag bit | metric | status |
|---|---|---|---|---|
| `0x004c42d0` | `[0]` | `0x1` NORMAL | normality / diagonal | C3 (prior session) |
| `0x004c4270` | `[1]` | `0x2` ORTHOGONAL | orthogonality / off-diagonal | C3 (prior session) |
| `0x004c4360` | `[2]` | `0x20000` IDENTITY | identity deviation | **C3 `3ceafa84`** |

All three "RwV3d bbox Y/X/Z accessor" labels are now disproved by their own bytes.

**Promotions this session (both C3, both hook-BYPASSED synthetic A/B — NOT C4):**
- `0x004c4360` `MatrixIdentityResidual4c4360` — ||M - I||^2 over the FULL 4-row matrix. NEW
  `arg_type` **`st0_ret_mat4x3_ptr`** (0x40 scratch, twelve f32; `st0_ret_mat3_ptr` seeds only
  nine, so `0x30/34/38` would be heap garbage). 10/10, 10 distinct fingerprints; four hand-derived
  predictions matched exactly (0.0 / 25.0 / 14.0 / 3.0). Resolves **U-9022**.
- `0x005c6b60` `Delta5c6b60` — 2D fixed-point delta, first row unlocked by the U-9023 band fix.
  5/5. `seed_byte 0x11` chosen deliberately: `0x00` would make `- p[1]` a no-op so a reimpl that
  OMITTED the subtraction would still pass. Declared `u32` not `void` — EAX equals the stored
  `p[0]` at both RET paths. New **U-9024** (purpose/subsystem undetermined).

**U-9023 RESOLVED (`a448e116`)** — the `0x005c0000-0x005c8000` CRT skip band was over-broad and had
silently excluded first-party rows since 2026-06-15. A narrower BOUND alone cannot fix it (28
alternations in address order), so the band is narrowed to `0x005c0000..0x005c3fff` AND paired with
a name test `is_library_name()`, which exempts RenderWare prefixes (`_rw`/`_rp`/`_rt`) — without
that exemption it would have silently dropped `0x004c7a70 _rwDeviceSystemFn`. 37 rows released, 0
newly excluded. `bulk_add_library_residue.py`'s conflicting `0x5d0000` reconciled.

**Deliberately NOT closed** (avoiding overclaim): **U-2169** (registered question is the four-field
distinction; only an unregistered sub-question riding on its ID was resolved) and **U-0004**
(transcription proven faithful, but what lives at `0x004a78f4` is still undetermined).

`0x0045bfb5` crasher **verified closed** — 3/3 clean, was collateral from the `Search45baa0` ABI
defect. Targeted by NAME via `MASHED_HOOK_ONLY`, because appending `Delta5c6b60` shifted registry
indices; an index-based re-run would have tested the wrong pair.

---

## NEXT — recommended order

1. **Re-run the call-target audit.** Cheap, and new hooks landed since it ran. Two scripts above.
2. **Menu-navigated race** to behaviourally confirm `0x00442cbd` (the only fix resting on static
   evidence alone). Needs input injection through the real frontend, not the warp.
3. **U-9024** — decompile caller `0x005c7330` to fix `0x005c6b60`'s purpose and subsystem. Its row
   is tagged `boot`, its only caller is `audio`, and the plate guesses "menu/cursor": all three
   disagree, so it was left at `boot` per the stop-and-ask rule.
4. **Sweep for more `st0_ret_mat3_ptr` / `st0_ret_mat4x3_ptr`-shaped leaves** now both handlers
   exist — this is how the new-arg_type lane pays off beyond the rows already landed.
5. The other 36 rows released by U-9023 are NOT yet frontier-eligible: the frontier is leaf-only
   and most are non-leaf (e.g. `0x005c47e0` has nine depth-1 callees). They become eligible as
   their callers reach C2+.

## OPEN GATES / STOP-AND-ASK

- **D2 renderer commitment** — OPEN (RW-subset verbatim vs `librw`). Confirm before M3/WS-E tokens.
- **D4 airborne bit-identity** — OPEN (accept A5 1-ULP float10 residual U-8991 vs naked-asm shim).

## HYGIENE NOTE

`mashed_pool/` holds **7 stale `.lock` files from OTHER sessions** (dated Jun 26 - Jul 19); slot 0
(this session's) is released and `status` reports it available. Left alone per multi-session
etiquette. `py -3.12 scripts/diag.py doctor` heals stale ghidra locks if you want them cleared.

TO RESUME: paste the kickoff prompt from the end of the session, or this whole file.
