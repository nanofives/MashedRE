# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-10, second session — merge + open-item drain) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean, no stray processes, pool locks clear
(slot 0's 10-day orphan released). Trackers: hooks.csv 5,930 rows
(C4 184, **C3 1,010**, C2 3,884, C1 821) · DEFERRED 677.

> **UNCERTAINTIES count — the previous header's "3,082" is not reproducible.** Two rules give
> two numbers: `grep -cE '^\| *U-[0-9]+ \|'` = **3,069** (open-format rows) and
> `grep -cE '^\| *(~~)?U-[0-9]+'` = **3,151** (including struck-through/resolved). Neither is
> 3,082, so whatever rule produced that figure is undocumented. Quoting 3,069 open with the
> command that derives it, per the roadmap's "every number is reproducible from the repo by a
> stated command" gate. Four rows were filed today (U-9130..U-9133).

**Landed this session:**
1. **Merges drained.** `docs/reconcile`, `race/nav-champ`, `race/arctic-cap` merged (the first two
   needed conflict adjudication — HEAD's 2026-08-31 measurement SUPERSEDES nav-champ's
   2026-08-30 "codes 11/12 are inert" claim, and `verify/nav_shots/FINDINGS.md` now carries a
   dated header saying so). 47 unprotected original-side reference captures under
   `verify/arctic_ref/` + `verify/geomlight_broadcheck/` force-added (they were untracked because
   `verify/**/*.bmp` is gitignored; re-capture is not bit-reproducible). The uncommitted
   Challenge-Select `dot` work was rescued off the stale agent worktree (37 commits behind) by
   re-applying onto HEAD. **`area/frontend` is the ONLY branch still unmerged** — deliberately: it
   is a WIP checkpoint whose `PanelSortInit` hook (`0x00420d00`) would go live in the dev ASI
   unverified.
2. **PAL4 was rejected by `QuadRenderer`**, so every BADGES 16x16 sprite silently never drew —
   the whole Finding-38 status-glyph family and the detail-panel checklist have been dead since
   they landed. Fixed; 15/17 screens byte-identical scope control. **U-9130** files the honest
   limit: `verify/orig_screens/s6.bmp` is NOT state-matched (4 unlocked rows vs our 1) so it
   cannot adjudicate the restored draws. `re/analysis/pal4_quad_upload_20260910.md`.
3. **Item B CLOSED — `0x0056bce0` C2→C3, 48/48 CLEAN.** Its hypothesis was wrong (args ARE plain
   cdecl); the real cause is an **implicit caller-saved-register contract**: callers
   `0x0056c310`/`0x0056c0a0` deref EDX at `0x0056c3e7`/`0x0056c180` immediately after the call,
   the original never writes EDX, and MSVC does. Fixed with a naked EDX-preserving shim, plus two
   independent precision corrections. **This is a lane-wide class** —
   `re/analysis/bce0_edx_contract_20260910.md`, and the screen below.
**86 rows moved C2→C3 today**, none resting on a synthetic Frida call. Full write-ups:
`re/analysis/shadow_lane_20260910.md` (Run/Region lanes), `lane2_decomp2port_design_20260910.md`
(transcriber), `lane3_write_tracking_20260910.md` (write tracking), `promotion_lanes_assessment_20260910.md`
(where the remaining volume is).

### Built today (`re/tools/`, `Core/`)
| tool | one line |
|---|---|
| `shadow_gen.py` | installed port → shadow site (`Run` / `--region` / `--tracked`); manifest `re/parity/shadow_sites.tsv` |
| `shadow_batch.py` | boots site groups, retries harness VOIDs, bisects CRASH/HUNG to one site, kills only its own PIDs; `--no-shadow` control |
| `shadow_ab_report.py` | log → CLEAN / DIVERGENT / DIVERGENT_FLOAT10 / UNPROVEN / SKIPPED / NO_SAMPLES |
| `decomp2port.py` + `DecompPC.java --port` | Ghidra decomp → verbatim C++ TU per function, `L2_` opt-in hooks, `INDIRECT_IDIOMS` table, `--prune-failed` |
| `Core/ShadowTrack.h` | `RunTracked`: page-level write tracking + caller stack window; `MASHED_SHADOW_TRACE=1` breadcrumbs + hang watchdog |

### Promotions today
44 (Run lane, 48/48) + 30 (tracked, 24/24) + 5 (region, 48/48) + **7 decompiler-generated ports**
(tracked 24/24) = 86. Every row cites its gate file under `log/shadow_ab/c3_gate_check*.tsv`.

### Open items (all saved; none blocking)
- **Private-memory tracking hangs** on its first sample: the tracked thread waits on a critical
  section that is not an NT heap lock (CRT `_HEAP_LOCK` or Frida interceptor lock). Watchdog log
  shows `EIP 0x77a8b9dc`, stack `77a9ff5c 77b49a54 … 77b4d3c8`. Next: read the CS address from
  the `RtlpWaitOnCriticalSection` frame and its owner thread id. (H)
- **`0x0047e9c0` first-call write at `.data+0x624048`** the port omits — reproducible 3 boots,
  candidate real defect in the K24 root port. (H)
- **Crashes to bisect — BISECTED, 5 of 9 resolved** (`re/analysis/shadow_crasher_bisect_20260910.md`).
  The "group" `0x0056f350 0x0056fea0 0x00570090` had never been split; one site per boot plus a
  `--no-shadow` control each gives:
  | RVA | armed alone | control | verdict |
  |---|---|---|---|
  | `0x0056bce0` | CLEAN 48/48 | — | **C3, closed** |
  | `0x0056fea0` | RACE_OK **NO_SAMPLES** | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
  | `0x00570090` | RACE_OK **NO_SAMPLES** | RACE_OK | **not a crasher** → NO_SAMPLES bucket |
  | `0x0056f0a0` | CRASH 30s | **RACE_OK** | crash is in the **A/B window**, not the port |
  | `0x0056f350` | CRASH 27s | CRASH 26s | crash is in the **installed port** |
  - `0x0056f350`: **argument shape CHECKED and CORRECT** — the original's three cdecl args
    (`E+4`, `E+8`, `E+0xc` float) map exactly onto `(int, float*, float)`. Base (+0x10), field
    (+0xc), stride (0x28), re-read bound (+0xac) and the `local_78[27]` sizing against both
    callees all verified faithful. The fault is at **iteration k = 5** (computed `EAX−EDI`),
    reading `[param_2+0x14C]` — the same address the original reads on the same iteration. Found
    and fixed one real transcription defect (loop counter declared `float`; the original's is an
    int at `[esp+0x50]`), but the crash reproduced and **the fault MOVED** to a `cmov`-ised
    default-pointer ternary yielding 0 (`divss xmm0,[eax]`, `EAX=0`). So there is more than one
    divergent path and whack-a-mole is low yield. Standalone regression-checked: no-op there.
    **NEXT: a runtime probe, not more static reading** — log `k`, `[param_2+0xac]` and
    `[param_2+0xc+k*0x28+0x10]` per iteration from the port and diff against a Frida trace of the
    original's loop on the same call. Full detail in the analysis note.
  - **BOTH witness crashes are now SETTLED and out of the lane** (`SKIP:runtracked-unbounded-effects`,
    verdict `INVALID_WITNESS`). `0x0055bd80`'s fault is EIP `0x00564c8e` `fld [ecx+0x10]` with
    `ECX = 1`, inside `FUN_00564c80` — a target of its volume-descriptor dispatch
    `call dword ptr [edx+0x10]` at `0x0055bdca`. Its port matches the original everywhere
    checkable (the three-way arg2 selection incl. the short-circuit, the `FUN_004c4600` argument
    order, its `uint *` return) and its control boots clean.
    **⇒ LANE RULE, and it refuted my first, broader explanation.** I nearly wrote "indirect
    dispatch through a runtime table cannot be A/B'd". Screened instead: **11** sites make such a
    call, and the split is by **witness kind**, not the dispatch — all **8 of 8** sampled `Run`
    sites are CLEAN 48/48, and **2 of 2** `RunTracked` sites are broken. So:
    > `RunTracked` is the wrong witness for a function making a runtime-dispatched indirect
    > call. `Run` compares one return value (bounded wherever the dispatch goes); `RunTracked`
    > compares pages + the caller stack window *and runs the body twice*, and for a callee chosen
    > from a runtime table neither is boundable.
    `RunRegion` is not an escape either: the region would have to cover whatever the dispatch
    target writes.
  - `0x0056f0a0`: control boots clean, so the port is fine; the armed boot NULL-writes at
    `0x0056caf4` inside a **third** function, `FUN_0056caa0`. **Body read, and it is the excluded
    shape:** ~16 load/add/store accumulators (invisible to an `add [mem]` grep), four of them
    into separately allocated arrays reached through pointers stored in the argument struct
    (`[esi+0xb8]/[0xc4]/[0xd0]/[0xdc]`, indexed by cursors `[0xd4]/[0xe0]/[0xf8]`), plus it bumps
    the cursor `[esi+0xf8]` itself, plus a loop through `FUN_0056f1f0` writing yet more
    pointer-reached arrays. **Hedge:** a tree-wide screen for the accumulator shape flags 3 sites
    and **2 are CLEAN 24/24** (`0x0056f020`, `0x0056d070`), so "accumulator ⇒ invalid" is FALSE.
    The surviving discriminator (n=3) is narrower — those two accumulate *directly on the arg
    struct*, `0x0056f0a0` is the only one going *through pointers loaded from it*.
    **DECISION NEEDED, not another run:** if that reading holds, move this row out of the lane.
  - **Harness defect FIXED — a control boot used to destroy the armed verdict.** `--no-shadow`
    sets `MASHED_NO_SELFTEST=1`, so every site returns `NO_SAMPLES`, and `done.update(results)`
    merged that over the real result. Seen directly: `0056f0a0` was `CRASH` at 14:34 and
    `NO_SAMPLES` at 14:35 when its control ran — that is how a real crasher reads as "never
    fired". `shadow_batch.py` now writes control outcomes to their own `control` / `control_at`
    columns. **The results table now self-documents the discriminator:**
    `0056f0a0 CRASH/RACE_OK` = witness problem, `0056f350 CRASH/CRASH` = port problem.
  - **The caller-saved-register class is RULED OUT for the other eight**: all 14 of their callers
    decompiled and grepped for `extraout_EAX/ECX/EDX` — zero hits. One `extraout_ST1` /
    `extraout_ST1_00` in `FUN_00570090` (x87 stack depth) is now moot, since neither site it
    calls is a crasher.
  - **The whole 9-item list is now triaged.** Beyond the five above:
    | RVA | armed | control | state |
    |---|---|---|---|
    | `0x00560260` | **CLEAN 24/24, twice** | RACE_OK | **promoted C3** — stale blocker, just needed re-running |
    | `0x0055bd80` | CRASH (1 sample, ndiff=0) | **RACE_OK** | **witness** crash, port fine → lane decision |
    | `0x00421960` | CRASH | CRASH | real port defect, EIP `0x00559cb3` `mov edi,[eax+ebp]`, unmapped `0x1d9644f4` after a bitset index calc |
    | `0x004219c0` | CRASH | CRASH | real port defect, EIP `0x004216b0` `mov eax,[ebx+0xf4]`, **`EBX = 2`** — an int in a pointer slot |
    | `0x00495fe0` | CRASH | CRASH | real port defect, EIP `0x00508bde` — **inside the `fix_joypad` boot-patch cave**, `mov eax,[esi]` with `ESI = 0` |
    All three Lane 2 rows are `decomp2port.py` output and need **per-function** review against the
    disasm — three distinct faults, not one shared bug.
    - **`0x00495fe0` — flag:** `0x00508bde` is 9 bytes into `0x00508bd5`, which is `int3` padding
      in `MASHED.exe.unpatched` and `fix_joypad`'s cave in the patched binary (verified by
      diffing both). So a boot patch is in the fault path. The cave reads `[esi]` before testing
      anything, so NULL reaches it unguarded — it was written for the garbage-pointer case.
      **[UNCERTAIN]** whether NULL can arrive there without this port installed is **untested**,
      so this is NOT yet a claim that `patch_mashed_fix_joypad.py` is defective on its own.
  - **Reusable recipe — this is the part that mattered:** (1) `--no-shadow` control, to decide
    port-vs-witness; (2) zero-hook baseline, for a passing control; (3)
    `re/frida/poll_attach_catch_crash.py` in a background job **alongside** `shadow_batch.py`,
    then resolve the EIP against `mashed_re_dev.map` at preferred base `0x10000000`. WER produced
    **no** minidump for any of these, so waiting on one finds nothing.
  - **Harness caveat:** `--no-shadow` sets `MASHED_NO_SELFTEST=1`, so a control logs no samples
    by construction. Only an **armed** RACE_OK with n=0 means "never fired".
- **Precision-class divergences**: `0x0055c2d0` (5/24 caller-frame bytes), `0x0055b750`
  (13/48 region), 5 float10 returns. (C)
- **Lane 1 C3→C4 (387 sites)** waits on the rubric wording decision (G).
- **Lane 2**: 15 indirect-call refusals; next table entries `*DAT_007d4110+off`, `*DAT_007d4108+0x28`;
  register-argument hazard class open. (I)
- **29 NO_SAMPLES** sites need a richer scenario (D). **U-9087**, **D-11069**, `TransformMatrixUpdate`
  (D-10793), `main` 270+ commits behind, 6 orphaned pool locks (F).

---

## PICK ONE

### A. DONE — the 5 region-lane rows are promoted (see CHANGELOG 2026-09-10 REGION LANE)

### B. DONE — `0x0056bce0` is C3 (48/48 CLEAN). The hypothesis below was WRONG; kept for the record.

~~Disassemble the original's prologue/argument use: if `param_3` (float) is not a plain cdecl stack
arg …~~ **Args ARE plain cdecl** (`0x0056bce3`/`0x0056bce7`/`[esp+0x1c]`, `add esp,0x10 / ret`).
The crash was the caller's EDX, not the callee's signature. See
`re/analysis/bce0_edx_contract_20260910.md` and the crash-triage recipe under "Open items".

### B2. Chase the `extraout_ST1` lead on `0x0056fea0` / `0x0056f0a0` **[the natural successor]**
Same shape as B but one register class over — see the bullet under "Open items". Confirm or refute
whether `FUN_0056fea0`'s original leaves the x87 stack at a depth the port does not.

### PARKED (D-11070) — the build is SSE2, not x87, and a load-bearing comment said otherwise
**Owner parked the `/arch` decision 2026-09-10.** Filed as **D-11070** with its re-pickup
conditions. The two false TU comments are already corrected, so nothing is left asserting an
untrue premise, and the 17 `DIVERGENT*` rows can be reclassified `DIVERGENT_FLOAT10`
(build-caused) **without** this decision. Detail below, kept because it is the cause of those rows.

`re/analysis/float_model_is_sse2_not_x87_20260910.md`. **There is no `/arch:` flag anywhere in
`build.bat`** (the only occurrence is a comment at line 26 about the qhull static lib), so MSVC's
x86 default `/arch:SSE2` applies. Measured in the shipped `.asi`: `FUN_0055b750_impl` is
**13 `movss` / 10 `cvtps2pd` / 6 `subsd` / 6 `mulsd` / 3 `cvtpd2ps` / 2 `addss` and ZERO x87
instructions**. Recompiling the same file with `/arch:IA32` added and nothing else changed gives
**11 `fld` / 5 `fstp` / 3 `fxch` / 3 `fsub` / 3 `fmul` / 3 `fadd` / 1 `fsubp`** — the flag is the
whole difference.

Two TUs asserted the opposite in a header comment (`RwpSolverIntegrate6.cpp`,
`Ai/AiTargeting.cpp`); **both comments are corrected in place** — a false statement about the
build is worse than a known gap. What it means: `float10` is a 53-bit double in SSE2, not an
80-bit x87 chain, and plain `float` expressions round after *every* op instead of accumulating at
80 bits. Blast radius: **85 TUs use `float10`, covering 88 `hooks.csv` rows (C4 10, C3 76, C2 2)**,
and **17 `DIVERGENT*` rows** in `shadow_results.tsv` plausibly trace to it.

**The ask:** leave it SSE2 and accept the precision floor, add `/arch:IA32` globally, or apply it
per-TU to the physics/math files only (the `QhullBridge` precedent, `build.bat:43/45`). Note
`/arch:IA32` does **not** make `long double` 80-bit — that ABI is fixed — so only
register-resident intermediates recover. Architecture-level, so not applied.

### C. `0x0055b750` — CAUSE FOUND, and it is the float model above, not a transcription defect
Traced the original `0x0055b750..0x0055b7fc` against the port: args, all three cross-product
formulas (`s0 = r5*d2−r6*d1`, `s1 = r6*d0−r4*d2`, `s2 = r4*d1−r5*d0`), the rounding map (`s0`/`s1`
`fstp dword`; `s2` kept live and added at 80 bits by `0x0055b7f2 fadd st(1)`) and the per-component
pointer re-derivation at `0x0055b7c5`/`0x0055b7e0` **all match**. The divergence hits `+04` and
`+08` and **never `+00`** — and `d0` is the only operand `s0` does not use. `d0` has the longest
live range (held untouched in `ST2`/`ST3` from `0x0055b774` to `0x0055b7b1`); in the port it is a
`float10` local at 53 bits. Reclassify as `DIVERGENT_FLOAT10` (build-caused) rather than chase it.

### C2. `0x0055c2d0` — also NOT a port defect: it is the port's own stack frame
`stack=2 … stk+03c x4`, with `pages diff:0` and `ret=0` — outputs agree 24/24. The original is
`sub esp,0x10`, no pushes, no cookie; the port is `sub esp,0x14` plus
`mov eax,[10103AC0h] / xor eax,esp / mov [esp+0x10],eax` (an MSVC **`/GS` stack cookie**, written
unconditionally at an address inside the original's frame footprint, of a value that differs per
call by construction) and then `push esi / push edi`. **`RunTracked`'s `stack=` channel compares
a window containing the callee's own scratch frame**, and every port's frame differs from its
original's, so that channel cannot separate a real caller-frame write from a layout difference.
Scope checked: **exactly 1** of the 17 `DIVERGENT*` rows is stack-only — this is a single-row
explanation, not a systematic reclassification.

### C-old. Settle the two aliasing rows **[RunRegion re-test]**
Regenerate `0x00577be0`/`0x00577cb0` as `RunRegion` over their output buffer (worker review 2
names it), re-boot, and read `0x0055b750`'s body against the disasm for a precision/transcription
difference. Each is C3 on a clean re-run, or the first real defect this lane has found.

### D. NO_SAMPLES — PRESCREENED 2026-09-10. 43 of 48 are genuinely never called; **5 are a bug**
`py -3.12 scripts/prescreen_batch.py --candidates re/analysis/plans/prescreen_candidates_nosamples_20260910.txt`
Both chunks reached a race (**3/3 validated probes each**, so the zeros are meaningful, not a
boot-only measurement): **c0 0/24 exercised, c1 5/24**. Result table:
`re/analysis/plans/prescreen_result_nosamples_20260910.tsv` (`class` = `never` / `exercised_inrace`).

- **43 `never`** — a 4-car standing race genuinely does not call them. Reaching them needs a
  richer scenario (`--boost`, `--mode 2`, a walled track, `--statediff-drive`), exactly as this
  item assumed.
- **5 `exercised_inrace` — RE-MEASURED PER-RVA, and the batch verdict was only 2/5 right.**
  I first read all five as "fires but never sampled = harness bug". Wrong. Counting each RVA
  individually (single run, `MASHED_COUNT_LATE=1`, gated on the 3 validated probes) gives:
  | RVA | calls/race | reading |
  |---|---:|---|
  | `0x005a6e10` | **29,302** | fires — **harness bug is real here** |
  | `0x005aeed0` | **22** | fires — **harness bug is real here** |
  | `0x005b0f40` | **0** | prescreen FALSE POSITIVE; `NO_SAMPLES` was correct |
  | `0x005b8080` | **0** | prescreen FALSE POSITIVE; `NO_SAMPLES` was correct |
  | `0x005ad2e0` | — | **no TU exists** (`L2_005ad2e0.cpp` absent, not in `asi_sources.rsp`) — its `shadow_sites.tsv` row is a claim without an implementation |
  Probe controls in the same runs: 5,696 / 1,538 / 39,343 — so the zeros are real zeros.
  `0x005b8080` was additionally measured **with its hook installed**: the counter reported
  `armed[JMP->mashed_re_dev.asi@0x6b495db0]` and `MASHED_HOOK_MANIFEST` shows `installed=1`, and
  the call count was still **0**. So install, phase and scenario are all fine; the function
  simply is not called.
  **Lesson:** `prescreen_batch.py`'s per-chunk `EXERCISED` attribution is not reliable at the
  row level — the five it named are exactly the five highest RVAs in the chunk. Trust the
  chunk-level probe gate, verify any individual row with a single-RVA count before acting.
  **SECOND CORRECTION — there was NO harness bug at either row. All five are closed:**
  | RVA | outcome |
  |---|---|
  | `0x005a6e10` | **CLEAN 24/24 on two single-site boots → promoted C3.** ~29,302 calls/race, so the coverage is real. Its NO_SAMPLES came from a group-of-5 boot. |
  | `0x005aeed0` | **INVALID_WITNESS, out of the lane.** Body is `WaitForSingleObject(*param_1,0) != WAIT_TIMEOUT` — an auto-reset event poll. The A/B runs original-then-port on the SAME handle, so the first call consumes the signal and the second correctly returns `WAIT_TIMEOUT`. That is exactly the measured shape (ret-mismatch on sample 0, clean after, twice). No page restore un-consumes an event. |
  | `0x005b8080` | **INVALID_WITNESS.** Calls `CloseHandle` — the A/B would **double-close a handle**. Only saved by never firing (0 calls, probes at 39k, hook installed and manifest `installed=1`). |
  | `0x005b0f40` | genuinely never called |
  | `0x005ad2e0` | `NO_PORT` — no TU on disk |
  **`decomp2port.py` hardened:** new `IRREVERSIBLE_SIDE_EFFECT` refusal (event wait/signal,
  critical sections, semaphores/mutexes, handle close, thread/file/registry mutation, message
  sends, `Interlocked*`). `ShadowAB.h`'s LIMITS block already excluded this in prose; nothing
  enforced it. Self-tested: both real bodies refuse naming the API, a no-API control accepts.

- **⇒ THE RULE THIS BUCKET TAUGHT, measured on four independent rows:** a **group-of-N boot
  falsifies BOTH verdict classes.** It produced false CRASHes (`0x0056fea0`, `0x00570090` were
  never crashers) *and* false NO_SAMPLES (`0x005a6e10` was CLEAN, `0x005aeed0` was sampling).
  **Re-run any group verdict at `--group 1` before believing it, in either direction.**

- **THE REST OF THE BUCKET IS GENUINELY EMPTY — measured, and it corrects my own framing.** I
  called the 32 group-boot NO_SAMPLES rows "a standing pool of possibly-free promotions". They
  are not. Re-ran all 31 remaining ones at `--group 1`, one boot each: **31/31 `boot_state`
  RACE_OK and 31/31 still `NO_SAMPLES`.** Zero converted. And the prescreen agrees on the same
  31 by an independent method (Frida invocation counters gated on the 3 validated in-race
  probes): **31/31 `never`**. Two methods, one conclusion — these functions are simply not
  reached by a 4-car standing race, so **item D's original premise holds and the work is
  scenario enrichment**, exactly as it said. The group-boot hazard is real but did not
  contaminate this set.

### E. Next slice of void ports **[RunRegion with two spans]**
12 MULTI_REGION rows in `log/shadow_ab/worker_void_regions.txt` need a `RunRegion2`; 18 INDIRECT
and 7 GLOBAL_WRITES need a snapshot of the pointed-to/global state instead. Verify every span with
the LHS scanner before applying — the worker's specs are claims.

### G. C3->C4 through the shadow lane **[DECISION first: rubric L37 wording]**
`re/analysis/promotion_lanes_assessment_20260910.md`: 387 C3 rows are shadow-generatable today
(315 outside audio), 293 more need a region. `re/CONFIDENCE.md` L37 names a Frida CSV as the C4
evidence; the shadow report is a stronger canonical-scenario diff but not a Frida CSV. Amend, or
keep C4 Frida-only. If amended: `shadow_gen.py --sweep re/parity/matchdiff_sweep_c3.csv --apply`
(exclude audio; frontend/hud with `--phase any`), then `shadow_batch.py --cars 4 --hold 60`.

### H. Lane 3 — pool closed; open defects and the private-memory mode **[investigation]**
55 converted void C2 ports: 30 CLEAN (all promoted, C3 = 1008), 13 unreached, 3 CRASH, 2
DIVERGENT, 7 unbooted (`re/analysis/lane3_write_tracking_20260910.md`). Open: (1) private-memory
tracking hangs on the first sample — the tracked thread waits on a critical section that is not an
NT heap lock (CRT `_HEAP_LOCK` or Frida's interceptor lock are the candidates); the hang watchdog
(`MASHED_SHADOW_TRACE=1`) logs EIP/stack; next step is resolving the CS address on the hung stack
and its owner thread. (2) `0x0047e9c0` reproducible first-call page diff at `.data+0x624048` = a
real candidate defect in the K24 root port. (3) crash bisect of `0x0056f350 0x0056fea0 0x00570090`
(`shadow_batch.py --rvas ... --group 1 --launch-arg=--cars --launch-arg=4`).

### I. Lane 2 transcriber — grow the accept set **[tooling]**
`re/tools/decomp2port.py` + `INDIRECT_IDIOMS` table (design: `re/analysis/lane2_decomp2port_design_20260910.md`).
29/53 reachable unported C2 rows compile; the 7 idiom-recovered ones are **promoted C3** (first
decompiler-generated ports verified effect-identical). 15 indirect-call refusals remain: next table
entries are `*DAT_007d4110+off` (4 rows) and `*DAT_007d4108+0x28` — add only with a hand port that
pins the convention. 3 generated ports crash alone (`0x00421960 0x004219c0 0x00495fe0`): open hazard
class (register args the callee prototypes miss). Re-run: `decomp_pc.py --file rvas.txt --callees
--port --json -o d.json` → `decomp2port.py d.json --emit-dir Lane2 --apply --report r.tsv` →
`build.bat` → `decomp2port.py --prune-failed log/build_lane2b.txt --report r.tsv`.

### F. Carried over from 2026-09-09, untouched
U-9087 E9-thunk guard decision (10 hooks never install, 4 C4); D-11069 (4 duplicate RVAs in two
targets); `TransformMatrixUpdate` pos.x/pos.z defect (D-10793); desktop verification of the
playtest commits; `main` 270+ commits behind; 6 orphaned pool locks (`Mashed_pool{0,1,10,11,12,13}`).

---

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md`, then pick ONE
> of A–I (G needs your rubric decision first). The shadow A/B mass lane is live (`re/tools/shadow_gen.py`, `shadow_batch.py`,
> `shadow_ab_report.py`; write-up `re/analysis/shadow_lane_20260910.md`). Standing rules that bit
> this session: run with `--cars 4 --hold 60` (a 1-car race never fires the contact solver); a
> `Run()` DIVERGENT is not a defect until the body has been read for state it also writes; float10
> returns cannot be compared bit-exactly through MSVC `long double`; verify every worker-supplied
> region spec against the body's assignment LHSs before applying it.
