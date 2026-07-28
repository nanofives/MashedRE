# Mashed RE orchestrator — resume point (updated 2026-07-28, U-9025 re-characterisation session)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

Status brief — **C1 797 / C2 4034 / C3 849 / C4 184** (was 797/4035/847/185).
Deltas: `0x004c3910` C2->C3 (promotion), `0x0055deb0` **C4->C3 (demotion)**. Trackers mutated
via `re-classify` this session; nothing is committed.

---

## HEADLINE: U-9025's mechanism was wrong. The phenomenon is real; the attribution was not.

The previous session attributed the race-entry wedge to an **audio-hook interaction** on two
"converging" methods. Both are now disproved by direct measurement.

### 1. The failure is NON-DETERMINISTIC (this is the load-bearing finding)

`scripts/bisect_hooks_set.py --measure 6`, **byte-identical** installed set each time (verified
by diffing the `.asi`'s own `MASHED_HOOK_MANIFEST` output — index, RVA and name all equal):

| configuration | completions |
|---|---|
| hooked, indices 75–149, **pre-fix** | **2 / 6**  (0.33) |
| hooked, same set, **post-fix** | **10 / 12** (0.83) — two independent batches of 5/6 |
| **stock** (`.asi` loaded, hooks not installed) | **6 / 6**  (1.00) |

Fisher exact, one-tailed: stock vs pre-fix **p = 0.0303**; post-fix vs pre-fix **p = 0.0573**;
stock vs post-fix **p = 0.43**; batch1 vs batch2 **p = 0.77** (no batch effect — the 0.83
estimate is stable).

**U-9025 REMAINS OPEN and this is not a judgement call: 2 of 12 post-fix runs still wedge.**
Whatever the p-value, the race is not eliminated. Note also that further reps cannot change
that conclusion — they would only sharpen "did the four fixes help" (currently p=0.057,
suggestive, short of 0.05), which is a secondary question. Spend effort on mechanism capture
instead.

**Read this carefully.** Hook installation *does* cause the wedge (stock vs pre-fix is
significant). But it is a **RACE**, not a deterministic interaction. And the post-fix
improvement is **NOT itself significant** at n=6 — post-fix is merely no longer
distinguishable from stock. Do not report U-9025 as fixed.

**What this invalidates:** "[75,150) fails as a set while BOTH halves complete → ≥2 hooks
co-installed". At p(complete)≈1/3 two single runs both completing has probability ≈0.11 —
unremarkable. Every branch of both prior bisects rests on single runs.

### 1b. THE WEDGE OBJECT IS NOW IDENTIFIED (two captures, `scripts/catch_wedge.py`)

`catch_wedge.py` runs until the game wedges and inspects the live pid **before** anything kills
it (the old driver killed the evidence). Caught on attempt 1, twice running. Measured:

- GUI thread — identified via `EnumWindows`+`GetWindowThreadProcessId`, **not** guessed from
  Frida's enumeration order — is parked in `ntdll!ZwWaitForSingleObject+0xc`.
- Its innermost frame is `0x005a840c`, which is exactly the instruction after
  `0x005a8406  CALL [0x005cc090]` in `FUN_005a8390`:
  ```
  005a83f4  MOV EAX,[0x007dcb68] / TEST / JZ 0x005a840c     <- gate read #1
  005a83fd  MOV EDX,[0x007dcae0] / PUSH -1 / PUSH EDX
  005a8406  CALL [0x005cc090]   WaitForSingleObject(h, INFINITE)   <- BLOCKS HERE
  005a840c  MOV EAX,[0x007dcad8]
  ```
- **`0x007dcae0` probes `WAIT_TIMEOUT` — count 0, nobody released it.** It is created
  `CreateSemaphoreA(NULL, initial=1, max=1, NULL)` (pushes 0x005a8290 / 0x005a829c, call
  0x005a82d2): a binary semaphore used as a mutex that starts SIGNALLED. So the wedge is a
  genuine **unpaired acquire** on `0x007dcae0`, not contention.
- The previously-blamed audio pair `0x007dd618` / `0x007dd620` probed **`WAIT_OBJECT_0`
  (signalled) in every wedge** — the main thread was never queued on them.

**REFUTED (pre-registered, do not silently re-open):** the obvious reading — that the acquire
happens under gate read #1 and the release under gate read #2 (`0x005a8423`), so a concurrent
`FUN_005a8460` (`005a846a DEC / MOV [0x007dcb68]`) driving the gate to 0 makes the release get
skipped — predicts `0x007dcb68 == 0` at wedge time. **Measured `0x007dcb68 == 0x1`.** The test
was stated before the run; the gate-zero mechanism is not what is happening.

**Still open — who took `0x007dcae0` and never gave it back.** Leading candidate is
**self-deadlock by re-entrancy**: a binary semaphore is NOT recursive, so if the same thread
re-enters any of the 22 `0x007dcae0` sites (`0x005a833d`..`0x005a8a07`) while already holding
it, it waits on itself forever — which fits count=0, gate=1, and a blocked GUI thread. Next
step: log acquire/release of `0x007dcae0` with thread id + return address (these are stream
calls, not a hot path, so Interceptor is affordable) and find the unpaired acquire. The 22
sites are the whole search space.

### 2. The wedge is NOT the audio semaphore

`re/frida/inspect_wedge.py` on a live wedged process (attach by explicit pid): a 0-timeout
`WaitForSingleObject` on **both** semaphores returned `WAIT_OBJECT_0` — **signalled**. Had the
main thread been queued on `0x007dd618`, the kernel would have handed the count to that waiter,
not to the prober. All 47 threads sit in ntdll; none in MASHED code. So `0x005ab63d` was a
**stale return address**, not the live frame.

Identified on the way (the handoff listed this as an open step): `[0x005cc090]` =
`PTR_WaitForSingleObject`, `[0x005cc094]` = `PTR_ReleaseSemaphore`; `FUN_005ab620` acquires
`0x007dd618` then `0x007dd620` INFINITE and releases both.

### 3. "All 75 are audio" was wrong twice

Indices 75–106 are audio, **107–115 are RW math**, 116–149 menu/HUD/font. And pre-existing
**U-6700** (`re/analysis/bucket_audio_005ab710_005af040/0x005ab710.md`) already records that the
`0x005ab..` cluster **cannot be shown audio-exclusive** — it reads as generic RenderWare stream
I/O, with `0x007dd618` documented as the *streaming lock handle*. No audio worker thread is
documented in this cluster.

---

## Method changes that matter more than the findings

- **`MASHED_HOOK_ONLY` is an arbitrary exact-token allowlist** (`HookSystem.cpp:189`) and takes
  precedence over `LO/HI` (`:150`). Any set can be pinned with **no `.asi` change and no
  rebuild** — so registry indices cannot shift mid-search. `bisect_hooks.py`'s single
  contiguous range was never capable of narrowing an interaction; `scripts/bisect_hooks_set.py`
  (new) does pin+search over arbitrary sets.
- **Select by NAME, never by RVA token.** The manifest has **27 duplicate RVAs** and zero
  duplicate names; an RVA token silently co-installs extras and makes a search unsound.
- **The correct predicate is "PASS iff N/N complete"**, not "FAIL iff 0/N". Innocent≈100%,
  guilty≈33% ⇒ at N=3 the PASS-iff-3/3 rule has ~3.6% false-pass and ~0% false-fail, whereas
  FAIL-iff-0/3 would mislabel a guilty set as innocent ~30% of the time.
- **The `.asi` APPENDS to `MASHED_HOOK_MANIFEST`.** A reused tag stacks runs and inflates the
  installed count (seen as `installed=150` for a 75-hook request). The driver now deletes the
  file first; older logs in `log/bset_m*.log` from this session carry the inflated figure.

---

## Lane A — four defects fixed (built, byte-verified in the deployed `.asi`; NOT yet committed)

Sweep A's nine "ECX/EDX ports" were **predictions about codegen**, not measurements.
`scripts/audit_emitted_regabi.py` (new, self-tested) disassembles our real emitted bodies:
**1 of 9 actually clobbered**, now 0/9.

| RVA | defect | evidence | fix |
|---|---|---|---|
| `0x00430760` `IsMultiplayerMode` | emitted `mov ecx,[0x67e9fc]`; original is EAX-only | caller `004333fd` loads ECX → `CALL` → `TEST ECX,ECX` at `00433410` | verbatim naked |
| `0x0055deb0` `RwpWorldSolverHandle` | unconditional `call` at entry + `mov edx,[0x100ffb94]`; original is 3 insns, EAX-only | caller `004292d0` `MOV EDX,[ESP+4]` → `CALL` → `LEA ESI,[EDX+8]` at `004292e5` | EDX/ECX-preserving naked shim, self-test scaffold intact |
| `0x004c1a00` `IntroSplashVtableSlot6` | sweep's ECX verdict is a **FALSE POSITIVE** (original tail-JMPs); real diff is EAX entering the callee | emitted `mov eax,[esp+4]/mov eax,[eax+0x18]/jmp eax` | verbatim naked (byte-identical) |
| `0x004148b0` `AiLeader_Entry` | **missing `ds:`** — see below | | `ds:` added |

The other six are measured clean: `UtilFloat63b910Get`, `TrackLoaderFloatGet`,
`ClearTable471530`, `ParticleEmitter_SetScalar`, `Mark4d5480`, and `GatedSwitch636ad0`
(its walker verdict is INCONCLUSIVE — ends in a jump-table dispatch — but both arms were
dumped by hand: `mov eax,1/ret` and `xor eax,eax/ret`, no ECX/EDX).

### The `ds:` defect — highest-value find, and it was accidental

MSVC assembles `mov eax, dword ptr [0x0067e9fc]` as **`B8 FC E9 67 00  mov eax,67E9FCh`** — the
ADDRESS as an immediate, not a load. It compiles clean at `/W3 /O2`. Caught by disassembling
the `.obj` of a fix I had **just written myself**, then swept repo-wide: 64 sites already used
`ds:`, **2 in `Ai/AiLeaderTimer.cpp` did not**.

`AiLeader_Entry` is a **default-installed** hook at `0x004148b0` whose whole job is to
re-execute the prologue the 5-byte JMP clobbers. It loaded the address instead, and the
original's next instructions are:

```
004148b5  SUB ESP,0x8
004148b8  CMP EAX,0x2
004148bb  JNZ 0x004148c3
004148bd  XOR EAX,EAX / ADD ESP,8 / RET      <- the early-out
```

EAX held 9020776, never 2 ⇒ **the early-out was unreachable in every race run to date.**
Its C3 diff (`log/diff_ai_leadertimer_004148b0_c3.log`) passed anyway, which means that seed
never drove `[0x0089a368] == 2` — a check that tested nothing.

**Rule now recorded in memory (`feedback_msvc_inline_asm_needs_ds_override`): after writing any
naked/`__asm` body, disassemble the `.obj` and compare bytes. Never trust "it compiled".**

### Trampoline prologue-reexec class — swept, CLEAN

account2 surveyed all **20** trampolines (`AiWallLateral`, `AiLineOfSight`, `AiSplineHooks`,
`AiNavHooks`, `AiWallAhead`, `AiControllerAB`×8, `PhysicsChainHooks`×5, …): **zero byte-count
mismatches** — every re-executed size equals its jump-back delta. So `AiLeader_Entry` was a
`ds:` defect, not a width defect, and the width variant does not exist elsewhere.
- Fixed `PhysicsChainHooks.cpp:2568` comment: it claimed the 5-byte JMP at `0x00468980` also
  clobbers the first byte of `SUB ESP,0x20`; impossible (JMP covers `..0x468984`, `SUB` starts
  at `0x468986`). Verified against the binary. Code was always right.
- **CAVEAT:** that survey checked each comment's arithmetic against its jump-back delta, i.e.
  internal consistency — **not** verification against the binary. A comment wrong *and*
  self-consistent would pass. Confirming all 20 against `.unpatched` disasm is a separate pass.

---

## TRACKER WORK — APPLIED 2026-07-28 (via `re-classify`; NOT committed)

- **`0x0055deb0` DEMOTED C4 -> C3.** Its C4 (`log/phys_c4_b5c_ALL6_GREEN_20260724.log`) is a
  B5C self-test comparing RETURN VALUES and cannot observe register footprint, so GREEN there
  is fully consistent with the proven EDX clobber — it was never C4-grade evidence for this
  failure mode. Re-promote only on a canonical-scenario diff with the shim INSTALLED.
- **`0x004c3910` PROMOTED C2 -> C3** (path1 GREEN 12/12 + path2 installer verified).
- `0x00430760`, `0x004c1a00`, `0x004148b0`: kept C3, **`frida_diff` cleared** — those diffs
  compared return values and could not see a register/prologue defect, so they are not evidence
  for the fixed builds. Each needs a fresh diff for C4; for `0x004148b0` the new diff needs a
  seed that actually drives `[0x0089a368] == 2`, which the old one demonstrably never did.
- **U-9025 rewritten** to the race characterisation with the measured wedge site; the disproved
  audio-interaction text is gone. **U-9026** (EFLAGS unaudited, `0x0055deb0`) and **U-9027**
  (vtable slot-6 targets not statically enumerable) filed.
- 4 CHANGELOG entries prepended.

**GENERAL LESSON worth carrying:** a diff that compares OUTPUTS cannot certify ABI fidelity.
Three separate rows this session carried green evidence that was structurally blind to the
defect they actually had. When a port is rewritten for register/prologue reasons, the old diff
is not merely stale — it never tested the thing that broke.

## NEXT — recommended order

1. **Confirm or refute the post-fix improvement.** 5/6 vs 2/6 is p=0.12 — suggestive, not
   proven. Another 6 post-fix runs would settle it. Until then U-9025 stays OPEN.
2. **Given it is a race, prefer mechanism capture over bisection.** A repeat-predicate bisect
   costs ~3 runs/test × ~14 tests ≈ 4 h. `re/frida/inspect_wedge.py` already dumps threads,
   the global cluster and semaphore state from a wedged pid; extend it rather than bisect.
3. **`0x004c3910` `Vec3NormalizeScale` is a free Lane-B win.** `hooks.csv` says C2
   BLOCKED-ON-ENV: *"no Frida diff obtained: MASHED self-exits ~0.5 s, exitcode -1, NO crash
   dump"* — which is the **screensaver signature** documented later in the same handoff, not a
   code problem. The port is already naked-verbatim and `arg_type vec3_normalize` already
   exists (`diff_template.js:868`). Boot is healthy: just re-run `run_diff.py`.
   (The prior handoff's claim that it needs a new `st0_ret_vec3_out_in_ptr` handler is wrong.)
4. `0x00442cbd` / `LoadingState2Enter` still not behaviourally confirmed.
5. `LobbySlotListRender` (`0x00439210`) remains a fabricated scaffold / NO-GUESSING violation.

## HYGIENE

- **SCREENSAVER blanks the display → MASHED exits `0xFFFFFFFF` ~4 s into boot, no dump**, stock
  and hooked alike. A fast `exit(-1)` with no dump is a clean refusal to start, not a fault.
- **Never run two MASHED-spawning drivers concurrently.** `bisect_hooks*.py` kills every pid
  that appeared during its run — it would kill a concurrent `run_diff`'s game. Serialize.
- **Do not rebuild while a measurement is in flight** — `build.bat` deploys to `original\` and
  would swap the binary under test.
- Ghidra pool: **slot 2** used this session (slot 0 and 1 `.lock~` are still JVM-leaked;
  `acquire` hands out slot 0 anyway and `release` then fails on the busy lock — use
  `acquire <N>` explicitly, then delete `mashed_pool/Mashed_poolN.lock` before the MCP open).
- **CORRECTION to the standing workaround:** the JVM-held `.lock~` does NOT require restarting
  the MCP server. Calling `program_close` on the session releases it — `release 2` failed with
  "Device or resource busy" before the close and succeeded immediately after. So the clean
  shutdown order is: `program_close` -> `ghidra_pool.sh release <N>`. (Slots 0/1 remain leaked
  because those sessions were never closed, not because a restart is required.)
- All MASHED pids spawned this session were killed by pid. No worktrees. `original/` intact.

TO RESUME: read this file; start at NEXT item 1.
