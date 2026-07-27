# Mashed RE orchestrator — resume point (updated 2026-07-27 ~10:30 UTC)

MISSION: dual-lane — (A) fix the game per RE_MASTER_PLAN, (B) promote Ghidra functions. Maximize account2.

All work through `e80b91f6` is committed AND pushed to `origin/main`.

## LANE A (fix) — the ~71 s runtime AV

### DONE this phase

**1. Dump-first root-cause.** Archived CrashDumps (it was AT the 10-dump retention cap) to
`%LOCALAPPDATA%\CrashDumps\_archived_by_claude\71s_av_2026-07-27\`. Four MASHED dumps, two
signatures, each reproducing twice:
- `.31084` / `.6168` — `eip=0x0047bda0`, read AV at `0x1c`, `eax=0`, `ecx=6`
- `.12828` / `.26728` — `eip=0x736ef96a` (DLL), read AV at `0x1c`
Shared ret-chain `0x0047bdab → 0x0047bfd5 → 0x0047c130`.

**2. First crasher FOUND and FIXED — EDX-clobber ABI defect (a NEW crasher class).**
`FUN_0047bcc0` (half-edge adjacency builder, *not* hooked) carries EDX across its call to
`0x0047bc90` (which IS hooked):
```
0x0047bd9b LEA EDX,[ESI+0x18] / 0x0047bda0 MOV EDI,[EDX] / 0x0047bda6 CALL 0x0047bc90
0x0047bdc5 ADD EDX,0x18       / 0x0047bdc9 JNZ 0x0047bda0
```
The original `0x0047bc90`–`0x0047bcb3` (15 instrs) only READS `[EDX+0x10]`/`[EDX+0x14]` — it
**preserves EDX**. Our `Match47bc90_RegAbi` naked shim marshalled EDX as an argument but never
restored it, and the compiled C body writes it (`mov edx,[eax+0x14]`, asi rva `0x2fadd`).
Fault arithmetic confirms: EDX is left holding `e[0x14]`, a MOVZX'd word index; `4 + 0x18 ==
0x1c` — exactly the reported fault address.

**The systemic gap: all three `_RegAbi` shim comments reason only about EBX/ESI/EDI being
callee-saved under `__cdecl` and never consider EDX/ECX.**
- `Match47bc90_RegAbi` — FIXED (push/pop EDX).
- `Find42ad90_RegAbi` — FIXED, same latent defect. Original `0x0042ad90` preserves EDX (array
  base, `[EDX+ECX*4+4]` at `0x0042adaf`/`0x0042adc0`, never written); compiled body does
  `lea edx,[edi+4]` / `add edx,4`. Fixed on disassembly evidence, not on a crash.
- `Find42add0_RegAbi` — checked, **correct as-is**: its original clobbers EDX itself
  (`0x0042add8 MOV EDX,[EAX+0x67ed38]`).
Both targets rebuilt clean; emitted shims verified to push/pop EDX.

**3. The fix works but did NOT close the AV — it exposed the next layer.**
Post-fix the `0x0047bda0` fault is GONE (0/2 fresh dumps). New fault, 2/2:
`eip=0x004e66db` in **`RpClumpForAllAtomics`** (`0x004e66d0`–`0x004e6708`, a NAMED RW fn),
`MOV EAX,[EAX+8]` with `eax=0` — **the caller passed a NULL clump**. Caller `FUN_0042a470`
(platform-prefixed asset path builder, returns 1=found / 0=not-found). Neither
`FUN_0042a470` nor `RpClumpForAllAtomics` is hooked (both C2/`mapped`).

**4. PROVEN hook-caused — never tested before this phase.** Same harness, 95 s:

| condition | runs | result |
|---|---|---|
| hooks INSTALLED (post-fix) | 5 | **5/5 AV** `0xC0000005` at 71.7–73.0 s |
| `MASHED_RE_NO_AUTO_HOOK=1` | 3 | **0/3** — all survived to the 96 s harness kill (`0xFFFFFFFF`) |

The ~71 s AV is a **hook regression, not a stock-game bug**. Memory `project_71s_av_open`'s
"pre-existing" framing is **corrected**. Caveat: stock N=3; the hooked side is now 5/5
(deterministic, previously ~75% flaky), so the split is strong but stock deserves more runs.
`verify/scene_t071.png` shows menu chrome at t=71, not a 3D scene.

**5. BISECTED and FIXED — `0x0041F330`, a THIRD register-arg ABI crasher.** Committed `1ed01345`.
Nine bisect steps over the 1203-hook registry (`MASHED_HOOK_LO`/`HI`, 90 s, 2 runs/step),
every split 2/2 in both directions, isolating registry **index 917 = `Search41f330`
@ `0x0041F330`** — installed ALONE it reproduces 2/2; neighbours 918/919 alone are 0/2.
The original reads its key from ESI and nothing from the stack (`0x0041f341 CMP [ECX],ESI`),
but `RH_ScopedInstall` registered the plain `__cdecl` C body — whose own comment already said
"ESI = search key". The live hook read `key` from `[esp+4]`, never matched, returned 0
(`0x0041f350 XOR EAX,EAX`), and that NULL reached `RpClumpForAllAtomics`.
Fixed with a naked ESI marshalling shim. No EDX save needed (the original clobbers EAX/ECX/EDX
itself and preserves ESI/EBX/EDI/EBP — exactly what `__cdecl` gives the C body).

**METHOD NOTE (reusable):** the bisect driver classified each step by **fault signature (eip)**,
not by "did it crash". This mattered — partial hook sets produced two unrelated faults
(`0x6f5bb3ac` in `[0,601)`; `0x0045bfb5` in `[915,917)`). A crash-vs-no-crash bisect would have
gone down the wrong branch. Driver: `scratchpad/bisect_step.sh` (LO HI RUNS →
POSITIVE/CONFOUND/NEGATIVE).

**6. Acceptance: target fault CLOSED, but the ~71 s AV is NOT.** Full 1203-hook set, 95 s, 4 runs:
`RpClumpForAllAtomics` AV **0/4** (gone). A DIFFERENT fault now appears **3/4** at ~72 s:
write AV (`params[0]=1`) to `0x0cfcf000`, `eip` in a DLL (`0x6dc7b43c`), with **`0x004b6520`**
(the memset-like ZeroFillWrapper) on the ret-chain. Third layer. NOT claimed as fixed.
`StructInit418a00_RegAbi` / `StructZero421060_RegAbi` were checked and are correctly shimmed,
so it is not those.

### SYSTEMIC — register-ABI audit (account2) found 3 MORE of the same class

`2854245f` called `0x0042add0` + `0x0047bc90` "the last two register-arg ABI suspects".
**That was an overclaim.** Static audit of every register-oriented `arg_type` row vs its
`RH_ScopedInstall` symbol found three more plain-`__cdecl` installs of register-arg originals:

| RVA | symbol | decl → install | original's register args (verified by disasm) |
|---|---|---|---|
| `0x0045baa0` | `Search45baa0` | `PromoLoop_sessionB.cpp:1285 → :1295` | key in **ESI** (`0x45bab1 CMP ESI,[EAX]`) — sibling of the fixed `0x0041f330` |
| `0x0041e170` | `Split41e170` | `PromoLoop_sessionB.cpp:1555 → :1564` | input **EAX** (`0x41e171 MOV ESI,EAX`), out-ptr **EDI** (`0x41e199/1af/1b2 MOV [EDI+n]`) |
| `0x00477450` | `Init477450` | `PromoLoop_sessionB.cpp:1855 → :1865` | [UNCERTAIN] not yet disassembled to a `ret` |

`0x0045baa0` is **independently corroborated**: the bisect's `[915,917)` step (indices 915+916,
916 = `Search45baa0`) threw a CONFOUND AV at `0x0045bfb5`, in that same `0x45bxxx` region.
[UNCERTAIN] the causal link — that step installed two hooks, so it is not isolated to 916.

### NEXT concrete step

1. Fix the three audited suspects above (verify `0x00477450`'s registers by disasm first),
   rebuild, re-run the 4-run acceptance.
2. Then bisect the remaining ~72 s write-AV to `0x0cfcf000` with the same signature-classified
   driver. Prior: something feeding a size/count into `0x004b6520`.

## LANE B (promote)

### DONE — U-9021 and U-9022 both resolved

Decompiled `FUN_004c4530`, the shared C2 caller. It is **`RwMatrixOptimize`**: takes the matrix
plus a 3-float tolerance triple (defaulting to `DAT_007d4028 + 0xc + DAT_007d3ff8`), runs the
three residual leaves, writes the RwMatrix flag word at `*(uint*)(param_1+0xc)`, and **returns
`param_1`**.

| RVA | tolerance slot | flag bit | metric |
|---|---|---|---|
| `0x004c42d0` | `param_2[0]` | `0x1` NORMAL | normality / diagonal |
| `0x004c4270` | `param_2[1]` | `0x2` ORTHOGONAL | orthogonality / off-diagonal |
| `0x004c4360` | `param_2[2]` | `0x20000` IDENTITY | identity deviation |

Flag constants pinned from first-party prior work, not external memory:
`re/analysis/bucket_004c4270/0x004c4670.md:47-48`, `Math/RwMatrixRotateInner.cpp:47`.
This **independently confirms** last phase's retraction of the "RwV3d bbox accessor" labels —
the slot order assigns 42d0→normality and 4270→orthogonality exactly as the byte-level
derivation concluded. Written up in
`re/analysis/render/0x004c4270_0x004c42d0_matrix_residuals.md` ("Caller resolution" section).
[UNCERTAIN] `0x004c4360`'s byte-level formula is still underived (`SUB ESP,0x18` frame, reads
`+0x30/+0x34/+0x38`); only its role is fixed.

### U-9023 — the band IS over-broad (account2 sweep, $0.95)

`0x5c0000–0x5c8000` is not CRT-only:
- `scripts/promote_frontier.py:86-93` `LIBRARY_BANDS` silently drops every RVA in the range from
  the C3 frontier. `scripts/bulk_add_library_residue.py:64` uses a DIFFERENT upper bound
  (`0x5d0000`) for the same named band — the two disagree with each other.
- The band's own source batch note says only **56/80 are CRT library-residue**.
- `hooks.csv:2907` `005c4d30 CondGet5c4d30` is already **C3 with a clean Frida diff** inside the
  band — promoted 2026-06-13, two days BEFORE the band was added (2026-06-15).
- `re/analysis/bucket_005c1d63/0x005c47e0.md` records `kind: GAME`,
  `library_match: (none — game code)` for `FUN_005c47e0`, six weeks before the band existed.
Conclusion: the band has been silently excluding first-party rows. **Needs a decision** —
narrowing `LIBRARY_BANDS` changes candidate generation project-wide.

## OPEN GATES / STOP-AND-ASK

- **D2 renderer commitment** — OPEN (RW-subset verbatim vs `librw`).
- **D4 airborne bit-identity** — OPEN (accept A5 1-ULP float10 residual U-8991 vs naked-asm shim).
- **U-9023** — narrow the `0x5c0000–0x5c8000` band in `promote_frontier.py`? Evidence above.
- **Push** — `main` is ahead of `origin/main` by `27a376d2` plus this phase's work.

## PHASE 3 (this session, after the bisect) — 3 ABI shims + a missing-indirection bug

Committed `e80b91f6`. **ALL WORK IS NOW PUSHED** — `main` == `origin/main` at `e80b91f6`
(6 commits pushed: `27a376d2`, `8d71f833`, `d9f61dd5`, `1ed01345`, `78fa3cd3`, `e80b91f6`).

**Fixed the three audited register-ABI suspects**, each verified against disassembly first:
- `0x0045baa0` `Search45baa0` — key in ESI; sibling of `0x0041f330`, same NULL-return mode.
- `0x0041e170` `Split41e170` — input EAX, out-ptr EDI.
- `0x00477450` `Init477450` — **MIXED**: dest in EAX PLUS four stack args. Needed a different
  shim shape: forward the four stack args unchanged, and restore EAX after the call because the
  original never writes EAX (callers may rely on the implicit `EAX==dest` return).

**Fifth defect, a DIFFERENT class — missing indirection, `0x00475a60 PendingOpQueueFlush`.**
Diagnosed from the dump WITHOUT bisecting (technique worth reusing):
1. faulting address resolved into our own `.asi` (base `0x736b0000`, `+0x7b43c` = CRT `rep stosb`)
   — `scratchpad/which_module.py`, `scratchpad/nearest_export.py`;
2. call frame read straight out of the minidump — `scratchpad/read_stack.py` — giving
   `memset(dst=0x0cf77ec8, 0, count=0x0d57a380)` (~223 MB) and return address `0x736c38ef`
   = `.asi` RVA `0x138ef` = `ZeroFillWrapper+0xf`, called from `PendingOpQueueFlush+0x37`.
`0x00691604 / 0x00691610 / 0x00691614` are **pointers to arrays**; the original loads the global
and THEN indexes (`0x475a70+75`, `0x475a78+81`, `0x475a8e+9c`). The reimpl indexed the globals
directly, so `size` read a raw pointer word and `size * 0x50` became the 223 MB count.
**`re/analysis/timer_d3_cont1_b/0x00475a60.md` encoded the SAME missing indirection — the reimpl
inherited the error from the note.** Both corrected. Lesson: a wrong plate propagates into ports.

Acceptance (full 1203-hook set, 95 s, 4 runs): the memset fault is GONE (was 3/4 → now 0/4).

## LANE A CLOSED — the ~71 s AV is FIXED (8/8 clean)

Layer 4 was **not** a memory corruptor. `MenuMixed.cpp` forwarded FrontendC2RoundI s debug print
through `0x004a3220` — a **guessed** address its own comment called not independently known.
That address is inside `FUN_004a31f3`, so the CALL decoded `f7 72 ed` = `DIV [EDX-0x13]`
mid-instruction with EDX holding a float, reading `0x3f966653` — the exact reported fault.

Ground truth at `0x00408a70`: `0x00408a89 PUSH 0x5cca5c` / `0x00408a8e CALL 0x004a2cbd`.
`0x004a2cbd` is a NARROW printf; `0x005cca5c` is ASCII put precisepos %d,%f. The reimpl also
passed a `wchar_t*`, so even a right address would have misformatted. Fixed verbatim (`7518a088`).
U-2169 format string / call RVA not known is RESOLVED.

**The single-corruptor hypothesis was wrong, and the tell was free:** all four dumps were
byte-identical (same eip/esp/ebp/eax/ecx). Heap corruption varies run to run. PageHeap /
Application Verifier (elevation + IFEO writes) was never needed — reading the crash frame took
~2 minutes. **Check dump-to-dump determinism before reaching for corruption tooling.**

**ACCEPTANCE:** full 1203-hook set, hooks installed — 4 runs @ 95 s + 4 runs @ 130 s,
**8/8 survived** to the harness kill (every exit `0xFFFFFFFF`), vs a prior deterministic 4/4 AV.
The 130 s runs clear ~2x the old failure window.

All four layers are fixed: `0x0047bc90` (EDX clobber), `0x0041f330` (ABI),
`0x00475a60` (missing indirection), `0x00408a70` (guessed callee RVA).

### Follow-ups this opened

- **Grep for other guessed addresses.** `0x004a3220` sat in-tree behind a comment admitting it was
  unverified. Any hardcoded RVA near observed / not independently known / assumed is suspect;
  verify each is a real function ENTRY — a mid-body address links fine and only fails at runtime.
- `0x0045bfb5` — an unrelated crasher seen during bisect when indices 915+916 were installed.
  Never chased; may already be fixed by the `0x0045baa0` shim, unverified.


## LOCKS / WORKTREES HELD

**None.** Ghidra `Mashed_pool0` closed and released; `.pool_slot` removed. No worktrees.
No stray MASHED processes (tasklist clean).

## UNCOMMITTED STATE

**Clean and PUSHED.** `main` == `origin/main` at `e80b91f6`.

Tracker updates (U-9021 / U-9022 / U-9023, CHANGELOG) are STILL NOT applied — they must go
through `re-classify`. That is the top outstanding Lane-B chore.

TO RESUME: paste this whole block into a new account3 session with the orchestrator prompt.
