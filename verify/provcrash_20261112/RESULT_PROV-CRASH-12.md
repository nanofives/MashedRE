# RESULT PROV-CRASH-12: first fault of the 11 CRASH and 1 HANG provisional bodies

Date 2026-11-12. Worker session cmwddkp3b5ptosc1boid1kogx (spawned on claude2, moved to claude3, claude-opus-5-5),
lead cmwddbckl5n0ysc1bokj6l26v. Branch `orch/PROV-CRASH-12` from `orch/integration` `0e277caaf`.
Review: **TEXT-ONLY / harness** (`re/frida/detorig.py`, `re/frida/js/detorig.js`, `re/tools/prov/`,
`verify/provcrash_20261112/`). No `mashedmod/` file, no `re/match/` source, no tracker, no ROADMAP edit, no flip.

## Verdict

**GREEN.** All 12 bodies have a first-fault verdict with the faulting EIP, the address, and the original instructions
the source departs from. **12 of 12 are source defects. None is the harness.**

**Fix tests:** each candidate fix was applied to a scratch copy of the source and relinked with the PROV-INSITU linker.
**9 of 12 then replay IDENTICAL-FOR-2724 with the body live.**

| | Bodies |
|---|---|
| Fixed, IDENTICAL-FOR-2724 | 9 |
| Fixed, reach a second fault (not diagnosed) | 2: `0x005ac210`, `0x005b3b80` |
| Fix path needs a follow-up row | 1: `0x00470c70` (a register argument C cannot pass) |

**The committed `re/match/` sources are untouched.** Each accepted fix goes through its own row:
- `match.py` re-score
- `equiv.py`
- `PROVISIONAL.tsv` / `ADMITTED.tsv`

None of the 12 is listed in `re/match/PROVISIONAL.tsv` or `re/match/ADMITTED.tsv`, so no knob-path body carries
these defects today.

## Per body, cheapest fix first

"fix run" = the PROV-INSITU replay with the fixed blob. Every fix-run row is IDENTICAL-FOR-2724 against
two no-body references from this root. `calls` = body calls in that run (prov.json `calls_total`), install check
`jmp_ok` true for all. Full table with the original citations: `CRASH_VERDICTS.tsv`.

| rva | first fault (run) | defect in the source | original says | fix | fix run (calls) |
|---|---|---|---|---|---|
| `0x005aa0c0` | AV read 0x8 at `0x005ac77a` (inside callback FUN_005ac770), eax = 0xffffffe8 = node 0 - 0x18; blob +0x5c / +0x73 on the stack, N=676 (`c_5aa0c0`) | sentinel test inverted: `param_1 == puVar1 + 4` (source comment: permuter swapcmp adopted) | `0x005aa108` lea eax,[esi+0x10]; `0x005aa10b` cmp edi,eax: node == param_1 + 0x10 | `puVar1 == param_1 + 4`, 1 line | IDENTICAL-FOR-2724 (32) |
| `0x005abfa0` | EIP 0x0, no return address on top of the stack, ebp = esi = 0 popped, N=676 (`c_5abfa0`) | `SH` holds 4 dwords. FUN_005ab380 writes 5 into it, the 5th lands on the blob's return address (blob frame 0x18 + 4 pushes, `h` at [esp+0x18]) | `0x005ab3cd` / `0x005ab3e4` mov [ecx+0x10],edx, ecx = arg 2; original frame `sub esp,0x1c` | `SH` gets a 5th dword, 1 line (fixed blob: `sub esp,0x1c`) | IDENTICAL-FOR-2724 (12) |
| `0x005a7b60` | EIP 0x18, same shape, N=676 (`c_5a7b60r`) | `int hdr[3]` passed to the same FUN_005ab380 (5 dwords); `[p1+0xc]` = eax & 0xffff (`0x005ab3fd`) lands on the return address | original frame `sub esp,0x24`, blob 0x1c | `int hdr[5]`, 1 line (fixed blob: `sub esp,0x24`) | IDENTICAL-FOR-2724 (1) |
| `0x005aba20` (HANG) | no fault. 40 of 40 main-thread samples at `0x005ade2b` (FUN_005ade10 list walk), eax cycles 0x0cce096c / 978 / 984 / 990, head esi = 0x0e6ea5d4 not on the cycle, blob not on the stack, N stops at 676 (`h_5aba20`) | `param_4[2] = uVar3;` before `uVar3 = FUN_005b3b60(..)`: stores an uninitialised local (blob +0xa9 / +0xb0 mov ecx,[esp+0x3c]; mov [ebp+8],ecx) and drops the return value | `0x005abacb` call 0x5b3b60; `0x005abad0` mov [ebp+8],eax | `param_4[2] = FUN_005b3b60(param_3,0,param_4);`, 2 lines to 1 | IDENTICAL-FOR-2724 (32) |
| `0x005ad0b0` | AV read 0x2d2c98fb at blob +0x1f (`mov edx,[eax-0x14]`), eax = 0x2d2c990f read as a list node, N=726 (`c_5ad0b0`) | list insert stores the container: `*piVar2 = iVar4` (iVar4 = `*(param_2 + 0x4c)`), blob +0x20d mov [eax],ebx | `0x005ad2b6` mov ecx,[ebx]; `0x005ad2bf` mov [eax],ecx: node->next = head->next (ebx = [esp+0x18] = list head) | `*piVar2 = *piVar1;`, 2 lines to 1 | IDENTICAL-FOR-2724 (16) |
| `0x005a9f10` | AV write 0xd78 at `0x005a88d0` (original, ebx = 0), blob not on the stack, N=726 (`c_5a9f10r`) | FUN_005ae3a0 declared with 1 argument and called; the 2nd argument it reads is then the blob's own return address | FUN_005ae3a0 reads arg 2: `0x005ae3b7` / `0x005ae3e2` mov ecx,[esp+0x18] after 4 pushes; the original tail-jumps (`0x005a9f3a`) with its param_2 in that slot. Its own source `re/match/s26/005ae3a0.c` has 2 parameters | 2-argument prototype, call with `param_2`, 2 lines (fixed blob tail-jumps like the original) | IDENTICAL-FOR-2724 (16) |
| `0x00481a30` | AV read 0x4 at `0x004c1b5d` (FUN_004c1b40 fmul [edx+4], edx = 0), blob +0x193 on the stack, N=751 (`c_481a30`) | `FUN_004c1b40(FUN_004671a0(0, &sph))`: `&sph` given to the wrong callee | `0x00481bb5` push &sph; push 0; call 0x4671a0; add esp,4 (pops the 0 only); push eax; call 0x4c1b40 = FUN_004c1b40(FUN_004671a0(0), &sph) | that call and the two prototypes, 3 lines | IDENTICAL-FOR-2724 (2719) |
| `0x00440bc0` | EIP 0x56: `call [ebx]` at `0x004b793b` (ret `0x004b793d`), original code, N=624 (`c_440bc0`) | 68 code symbols declared `extern undefined1 FUN_x` and passed by value: `FUN_0047b980("Course_Id", FUN_0047a180)` registers the first code byte (blob +0x0 movzx eax,byte [0x47a180], 66 such loads) | the original registers the handler addresses | `&FUN_x` at the 66 argument sites (mechanical) | IDENTICAL-FOR-2724 (2) |
| `0x005a75b0` | AV read 0x5 at `0x005a6d99` (FUN_005a6d90 mov eax,[esi+4], esi = 1, ret `0x005a6e29`), blob not on the stack, N=2996 (`c_5a75b0r`) | three statement-order defects: (1) exponent read from `u.u` before `u.f = fVar11`; (2) insert sets `piVar12->prev` before `piVar9->prev = piVar12->prev` (node->prev = node); (3) `DAT_007dca1c = param_1` before `*(V **)DAT_007dca1c = param_1` (node->next = node, old tail not linked) | (1) `0x005a7655`..`0x005a7664` read the stored float; (2) `0x005a777b` mov eax,[edx+4] first, then `0x005a7780` / `0x005a7786` / `0x005a7788`; (3) `0x005a77e6` mov eax,[0x7dca1c]; `0x005a77ec` mov [eax],ecx; then `0x005a77ee` mov [0x7dca1c],ecx | reorder the 3 pairs, 6 lines | IDENTICAL-FOR-2724 (54321) |
| `0x005ac210` | EIP 0x8b: `call ebp` in the original FUN_005aa0c0 (ret `0x005aa0ec`), blob +0x132 on the stack, N=676 (`c_5ac210`) | `MGLOBAL(undefined1, FUN_005ac770, ..)` passed by value: the blob pushes the byte 0x8b at 0x005ac770 (blob +0x11d mov cl,[0x5ac770]) as the callback; same for FUN_005aaaa0 (blob +0x203) | `0x005ac324` push 0x5ac770 | `&FUN_005ac770`, `&FUN_005aaaa0`, 2 lines | **CRASH, 2nd fault** (`f_5ac210`): AV read 0x10 at `0x005aded6` (mov ecx,[edx+4], edx = 0xc), ret `0x00462b5e`, N=676, blob not on the stack. Not diagnosed. The source header already lists unported argument types (FUN_005acaa0 / 005aca80 / 005aec00 / 005ae0c0, `code` typedef arities) |
| `0x005b3b80` | AV read 0x1 at blob +0x104, worker thread (tid 20848, main 20712), N=5022 (`c_5b3b80`) | Ghidra's stack alias kept: `local_38[(int)param_1 - 4]` (4 uses). In the DDK frame element -4 is another local ([esp+0x14] = 1) | `0x005b3cd4` mov ecx,[esp+esi*4+0x24]: one array indexed by the param_1 slot | `local_48[(int)param_1]`, 4 lines | **CRASH, 2nd fault** (`f_5b3b80`): AV read 0x74616d67 at blob +0x125, element 0x00630110 whose first dword is 0x74616d5f, N=5020, worker thread. [UNCERTAIN] The call to FUN_005b3760 differs from the original: `0x005b3bd7`..`0x005b3bee` pushes the frame arrays at E+0x34 / E+0x44 and ebp, where the source passes `local_48, local_38, ..., 0`. Missing evidence: a stack-depth trace of `0x005b3cd0`, to fix which array the loop indexes. |
| `0x00470c70` | AV read 0x9e4 at `0x00470680` (FUN_00470670 fld [edi+0x9e4], edi = eax at entry = 0), blob +0x413 on the stack, N=751 (`c_470c70`) | FUN_00470670 takes eax as a register argument. The blob call site (+0x409..+0x40e) sets no eax | `0x00471073` mov eax,ebp before `0x00471076` call 0x470670; `0x00470679` mov edi,eax. The defining source `re/match/s11/00470670.c` models it as a 5th parameter `char *car` | follow-up row: a C cdecl call cannot load eax, so the call needs a thunk or the defining source's convention | not run |

**Defect classes, with a static scan of every linked body:**

| Class | Bodies | Scan / note |
|---|---|---|
| Code symbol declared as one byte and passed by value | 2 (`0x005ac210`, `0x00440bc0`) | `BYTE_FNADDR_SCAN.tsv`: all of `bodies_all.json`, no other body |
| Ghidra negative local index | 1 (`0x005b3b80`) | `NEGINDEX_SCAN.tsv`: no other body |
| Callee output buffer smaller than the callee writes | 2 (`0x005abfa0`, `0x005a7b60`), both FUN_005ab380 | none |
| Callee declared with fewer arguments than it reads | 1 (`0x005a9f10`) | `ARITY_SCAN.tsv`, see below |
| Argument given to the wrong callee | 1 (`0x00481a30`) | none |
| Statement order / uninitialised read | 2 (`0x005aba20`, `0x005a75b0`) | none |
| Wrong value in a list insert | 1 (`0x005ad0b0`) | none |
| Inverted comparison | 1 (`0x005aa0c0`) | none |
| Missing register argument | 1 (`0x00470c70`) | none |

**`ARITY_SCAN.tsv`** (`re/tools/prov/prov_arity_scan.py`) lists every callee prototype whose argument count differs
from the callee's own match source:
- 230 rows over all PROV-INSITU sources, a lead list, not verdicts
- 21 of those rows are in IDENTICAL-FOR-2724 bodies
- in 0x005aba20, FUN_005a7b00 is declared with 1 argument against 2. The blob still passes param_6 as the 2nd
  argument, through the same deferred pop as the original (`push ebp; call 0x5a7b50; push eax; call 0x5a7b00`, blob
  +0x265..+0x26c, original `0x005abc89`..`0x005abc90`). So that row is not a defect.

## `0x004c4eb0` (PROV-WRITESET N = 10 death, raised by the lead)

**Verdict: harness stepping, not the source.** Evidence:
- Without stepping the body ran 877 calls and was IDENTICAL-FOR-2724 (PROV-INSITU `t0_195814_012_1`).
- Under stepping, N = 1 and N = 3 were clean.

What the failing N = 10 run left (`D:/MashedWT/PROV-WRITESET-114_data/ws_s_0x004c4eb0.log`):
- The line `stats failed script has been destroyed` appears before `game exited rc=2147483652` (0x80000004).
- `wlive.jsonl` has 0 lines and no `wcrash.json` was written, so neither handler saw a fault.

[UNCERTAIN] One mechanism that fits:
- The Frida script was destroyed with the trap flag still armed.
- That removes the CModule vectored handler, so the next single step goes unhandled.
- Missing evidence: why the script was destroyed. This needs a run with `--prov-wlog 10` and an out-of-process
  exception catcher (`re/frida/poll_attach_catch_crash.py` attached to the same PID).

## Method

**Catcher** (`re/frida/detorig.py --crash-catch S`, `detorig.js crashArm`): armed at the install, on the menu.
- **Exception handler**, no Interceptor. On the first real fault (not breakpoint / single-step / system) it records:
  - type, address, memory operation and address
  - all registers
  - 64 stack dwords, each classed as blob offset, module + offset, or raw
  - 32 code bytes around EIP
  - which `--prov` blob holds EIP
- **Hang sampler:** a JS timer takes the main thread's context 40 times (`Process.enumerateThreads`) once N
  (main-thread clock calls) has not moved for S seconds.
- **Output:** each record goes to `crash.jsonl` with a synchronous flush before the `send`.
- **Watch chaining:** `--watch` hands non-step exceptions to the same handler.
- **Write-set runs:** not used with `--prov-wlog`, whose handler stays in place.

**Reading:** `re/tools/prov/prov_crash.py <run>` disassembles the blob around the fault, with rel32 targets named from the
link table, and lists the stack dwords that point at code.

**Fix test** (`re/tools/prov/prov_fixlink.py`):
- applies each candidate fix as a text edit to a copy of the committed source (`D:/MashedWT/PROV-CRASH-12_data/fixsrc`)
- links the copy with `provlink.link()`, exactly as PROV-INSITU links the committed file
- writes `bodies_fix.json` and `bodies_fix_5a75b0.json`. The second file exists because that fix was added while a
  batch was reading the first.

**Scenario and references:**
- **Scenario:** the PROV-INSITU replay (`verify/provinsitu_20261111/run_arm.sh` configuration: tick clock,
  `--q0 0xC0000000`, `warp_normalize.tsv`, `--load-sleep-ms 0`, `--warp-clock 2000`), one body per run.
- **Private root:** `D:/MashedWT/PROV-CRASH-12_root`, a robocopy of `PROV-INSITU_root`. The binaries are the same:
  - `MASHED.exe` `B9977DAB...`
  - `mashed_re_dev.asi` `906F8601...`
  - `d3d9.dll` `47901951...`
- **Controls:** `ctl_nobody` and `ctl2_nobody`: catcher armed, no body, both rc 0, 2,724 ticks, IDENTICAL-FOR-2724 to
  each other with 0 allowed blocks.
- **Compare:** `re/frida/prov_compare.py RUN ctl_nobody ctl2_nobody --pair r1,r2 --pair x_ref,x_ref2` (PROV-INSITU
  references), 15 allowed blocks.
- **Why my own references:** against PROV-INSITU's r1, my no-body run differs at tick 0 in blocks `0x00773c00` and
  `0x00911c00`. [UNCERTAIN] One mechanism that fits is the different root path (`PROV-CRASH-12_root` against
  `PROV-INSITU_root`, 2 characters longer). Not traced. The fix verdicts use the two same-root references for this
  reason.

**Runs:** 31 game runs, every one under `test-slot` with `mashed_lock.py run --label PROV-CRASH-12` inside it.
- 6 Frida attach failures (`main thread None` / `attach failed`), each re-run. The first `ctl2_nobody` attempt's log
  was overwritten by its retry.
- No piz-collision signature seen.
- Only PIDs this session spawned. No build (none needed). No run started during a merge-train PRIORITY-HOLD: those
  waited in the lock queue.

## Follow-up rows proposed

1. **PROV-FIX-9:** land the 9 tested fixes in the committed sources: `0x005aa0c0`, `0x005abfa0`, `0x005a7b60`,
   `0x005aba20`, `0x005ad0b0`, `0x005a9f10`, `0x00481a30`, `0x00440bc0`, `0x005a75b0`.
   - The text edits are the `FIXES` entries of `re/tools/prov/prov_fixlink.py`.
   - Re-score each with `match.py` and re-run `equiv.py`.
   - Replay each with the committed file (expected IDENTICAL-FOR-2724, as here).
   - Then the PROVISIONAL / C4-replay legs.
2. **`0x005ac210` 2nd fault:** AV at `0x005aded6`, edx = 0xc, from `0x00462b5e`. Start from the source header's list
   of unported argument types.
3. **`0x005b3b80` 2nd fault:** the FUN_005b3760 argument arrays (`0x005b3bd7`..`0x005b3bee`) and a stack-depth trace
   of the loop at `0x005b3cd0`.
4. **`0x00470c70`:** pass eax = car record to FUN_00470670 (`0x00471073`). Needs a calling form for a register
   argument in a DDK C source.
5. **`0x004c4eb0` stepping death:** harness only. Run under an out-of-process catcher.

## Evidence (this folder)

| File | Contents |
|---|---|
| `CRASH_VERDICTS.tsv` | one row per body: fault, EIP, thread, N, defect class, original citation, fix, fix-run verdict, calls |
| `BYTE_FNADDR_SCAN.tsv`, `NEGINDEX_SCAN.tsv`, `ARITY_SCAN.tsv` | the static scans over every PROV-INSITU source |
| `bodies_fix.json`, `bodies_fix_5a75b0.json` | the fixed blobs that ran |
| `run_arm.sh`, `run_fix.sh`, `run_fix2.sh`, `batch.sh`, `batch_fix.sh`, `batch_fix2.sh` | the run scripts |
| `RAW_SHA256.tsv` | SHA-256 of every cited raw file under `D:/MashedWT/PROV-CRASH-12_data/` |

`RAW_SHA256.tsv` covers:
- the `crash.jsonl` of each first-fault and second-fault run
- each run's log
- the `ticks.bin` of the 9 IDENTICAL fix runs and the 2 references

**Raw data, not committed** (`D:/MashedWT/PROV-CRASH-12_data/`):

| Runs | What |
|---|---|
| `c_*`, `h_5aba20` | first faults |
| `f_*` | fix runs, each with a `*.verdict.json` for the compare |
| `ctl_nobody`, `ctl2_nobody` | references |
| `b1.log`..`b5.log` | batch logs |
| `fixsrc/`, `fixsrc2/` | the edited source copies |
