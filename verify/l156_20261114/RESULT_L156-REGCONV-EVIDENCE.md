# RESULT L156-REGCONV-EVIDENCE (run 46, MATCH section, static evidence row)

Branch `orch/L156-REGCONV-EVIDENCE`, based on `orch/integration` 2c7f97e41. 2026-11-14. No build, no gate, nothing bound,
nothing decided. Answers the L-156 question (log/orch/DECISIONS.md): for each of the five units ddkobj still refuses
(`fail:externalized-static-register-convention`, verify/ddkregconv7_20261113/DDKOBJ_REGCONV_FORWARDERS.txt), does the
VS2022 body the object build runs compute the original's values?

**Verdict: GREEN (all 5 units have a verdict).** No unit is proven (no EQUIV). None differs in a value: the vector
compare of the unit plus its helpers against the original's bytes is same on 300/300 vectors for every unit at both
x87 control words. The exception is one register. For 0x00427f00, 0x00427ff0 and 0x00428140, ECX differs on every
vector at the tail-jump thunk 0x00556ca0. That thunk does not read ECX, but it jumps on through the unpinned
`[DAT_0067d838 + 0x138]` (U-5674).

| unit | verdict | equiv.py, VS2022 compile | unit vector compare (300 vectors each) |
|---|---|---|---|
| 0x00427e00 | **NOT-PROVEN** | `NOT-PROVEN call-sequence: ... 9 vs 10 calls` | same 300/300 at CW 0x037f and at 0x007f, ECX included |
| 0x00427f00 | **NOT-PROVEN** (same if the `[font+0x138]` target ignores ECX) | `NOT-PROVEN call-sequence: ... 7 vs 8 calls` | differs 300/300, only at `call 7 to 0x556ca0 live-in regs: ecx`. With ECX at 0x00556ca0 excluded: same 300/300 at 0x037f and 0x007f |
| 0x00427ff0 | **NOT-PROVEN** (same condition) | `NOT-PROVEN call-sequence: ... 12 vs 13 calls` | differs 300/300, only at `call 11 to 0x556ca0 live-in regs: ecx`. Excluded: same 300/300 at both CWs |
| 0x00428140 | **NOT-PROVEN** (same condition) | `NOT-PROVEN call-regargs: ... call 0x4277a0` | differs 300/300, only at `call 8 to 0x556ca0 live-in regs: ecx`. Excluded: same 300/300 at both CWs |
| 0x00458920 | **NOT-PROVEN** | `NOT-PROVEN branch-condition: ... ld32(M00(),ld32(M00(),0x7f0fd0))` (harness artefact, section 2) | same 300/300 at 0x037f and at 0x007f |

Why NOT-PROVEN and not EQUIV: equiv.py is the only tool that decides an admission (L-126), and it cannot pair these
bodies (section 2). A same tally from random vectors is a cross-check, not a proof. No vector found a value
difference, so DIFFERS does not apply.

## 1. Commands and tool lines

DDK compile (the matched source as ddkobj compiles it). This says nothing about VS2022, and every unit is a 100% match:
```
py -3.12 re/tools/match/equiv.py re/match/s06/00427e00.c re/match/s06/00427f00.c re/match/s06/00427ff0.c re/match/s06/00428140.c re/match/s09/00458920.c
0x00427e00	EQUIV	[BYTE-IDENTICAL]     (same line for 0x00427f00 0x00427ff0 0x00428140 0x00458920)
```
(`equiv_ddk.txt`)

VS2022 compile (`py -3.12 verify/l156_20261114/gen_tus.py`, then
`py -3.12 re/tools/match/equiv.py verify/l156_20261114/tu/single_*.cpp`, `equiv_vc19_single.txt`):
```
0x00427e00	NOT-PROVEN	call-sequence: region ours +0x0 / orig 0x00427e00: 9 vs 10 calls (fallback: argument-area address escapes (passed to a callee))
0x00427f00	NOT-PROVEN	call-sequence: region ours +0x0 / orig 0x00427f00: 7 vs 8 calls (fallback: argument-area address escapes (passed to a callee))
0x00427ff0	NOT-PROVEN	call-sequence: region ours +0x0 / orig 0x00427ff0: 12 vs 13 calls (fallback: argument-area address escapes (passed to a callee))
0x00428140	NOT-PROVEN	call-regargs: region ours +0x0 / orig 0x00428140 call 0x4277a0: (('eax', ret32(call0(0x427780,...))), ('ebx', ebx_in)) | (('eax', ...), ('ebx', (esp0+-0x104)))
0x00458920	NOT-PROVEN	branch-condition: region ours +0x1f / orig 0x00458942: P1('ne',0x5,ld32(M00(),ld32(M00(),0x7f0fd0))) | P1('ne',ld32(M00(),0x7f0fd0),0x5)
```

Unit vector compare (`unit_cmp.py`, this folder). Commands, absolute paths, run from anywhere:
```
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/verify/l156_20261114/unit_cmp.py --runs 300 --cw 0x037f
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/verify/l156_20261114/unit_cmp.py --runs 300 --cw 0x037f --ignore-ecx-556ca0
py -3.12 C:/Users/maria/Desktop/Proyectos/Mashed/.worktrees/L156-REGCONV-EVIDENCE/verify/l156_20261114/unit_cmp.py --runs 300 --cw 0x007f --ignore-ecx-556ca0
```
Outputs: `pass_cw037f.txt`, `pass_cw037f_ignecx.txt`, `pass_cw007f_ignecx.txt`. About 25 min for all three passes
run in parallel. Verdict lines, for example:
```
0x00427e00	cw=0x037f	ignore_ecx_556ca0=0	runs=300	same=300	differs=0	inconclusive=0	calls(orig)=2826
0x00427f00	cw=0x037f	ignore_ecx_556ca0=0	runs=300	same=0	differs=300	inconclusive=0	calls(orig)=2529
  first non-same: differs: call 7 to 0x556ca0 live-in regs: (('ecx', ('stk',)),) | (('ecx', 3268308804),)
0x00427f00	cw=0x037f	ignore_ecx_556ca0=1	runs=300	same=300	differs=0	inconclusive=0	calls(orig)=2529
0x00458920	cw=0x007f	ignore_ecx_556ca0=1	runs=300	same=300	differs=0	inconclusive=0	calls(orig)=930
```
**Negative control** (`negctl_mut_00427e00.txt`): the same harness on a mutant TU, where the helper 0x00427680's
`size * 0.025f` becomes `0.026f` (`tu/mut/`), reports `differs=10` of 10:
`call 8 to 0x556ca0 arg 3: ours ('stk','b','371d40c14c7a813f') | orig ('stk','b','371d40c163ae813f')` (pos[1]).
So the compare sees a one-constant change in a helper's float output.

## 2. Why equiv.py cannot decide these units, and what the harness does instead

* The question crosses a call. The original unit passes its arguments to a private-convention helper in registers
  (0x00427680 ESI/EDI, 0x004277a0 EAX/EBX, 0x00427840 EAX/ECX, 0x00458570 EAX/ESI: verify/ddkregconv7_20261113
  RESULT section 1). The VS2022 unit calls the exe's cdecl body of the helper with stack arguments. equiv.py pairs one
  function with one original function and compares every call's arguments, so it stops at the helper call
  (call-sequence / call-regargs above).
* Under C++, MGLOBAL is a `static T &` whose slot the exe fills at dynamic init. equiv.py maps the slot symbol
  `DAT_007f0fd0` to the original address itself, hence the double dereference `ld32(ld32(0x7f0fd0))` in 0x00458920's
  line. That is an artefact of applying equiv.py to a MATCH_PORT C++ compile, not a property of the body.
* `unit_cmp.py` therefore executes the unit and its helpers on both sides in Unicorn, reusing equiv_dyn.py's layout,
  page filler and run model. Only calls that leave unit + helpers are stubbed. Original side: the unit bytes plus the
  original helper bytes (ranges from re/console/pc_funcs.csv). Ours: `tu/unit_<rva>.cpp` = the unit source plus the
  VS2022 bodies the exe binds for its helpers (re/match/s06/00427680.c, 004277a0.c, 00427840.c as `Port_<rva>`). For
  0x00458920, the exe's adapter (Match/MatchS09_00458920.cpp:40) calls the original 0x00458570. Observables: stub
  records in order, with stack-pointer arguments replaced by the bytes the callee reads there, every non-stack byte
  written, ESP, EBX/ESI/EDI/EBP, fault, x87 TOP. EAX/ECX/EDX at return are not compared (all five are void). Seeded
  inputs: align 0..3 plus a wild value, strings that fit text[128] / text[512], alpha near 0x80 / 0x17f, 0x00458920's
  gate fields, the font globals. Callees that feed values back are modelled: 0x00427780 returns a length-prefixed
  string, RW +0xf4 returns the real strlen, 0x0046d4a0 writes its out pointer, and 0x005554d0 / 0x004c3ac0 / 0x004c39b0
  return a float in ST0.
* The 0x00556ca0 ECX difference: 0x00556ca0 is `mov edx,[esp+0xc]; mov eax,[esp+4]; mov [esp+0xc],edx;
  mov [esp+4],eax; jmp dword ptr [eax+0x138]` (disassembled 0x00556ca0..0x00556cb0, also U-5675). It does not read
  ECX. equiv.summary keeps ECX live-in because the jump target is unknown. U-5674 pins the dispatch base to
  DAT_0067d838 (writers 0x00427632, 0x00427d4a) but not the function stored at +0x138. In neither compile is ECX an
  argument: in the original 0x00428140 it holds `lea ecx,[esp+0x2c]`, pushed as pos (0x0042810c..0x00428113,
  `ecx_at_556ca0_sites.txt`), and in 0x00427f00 it holds a value left by the previous call. [UNCERTAIN] whether the
  `[font+0x138]` target reads ECX. Missing evidence: the writer of font+0x138 (the font object created through
  0x004cc230 in 0x00427ca0) and that function's live-in registers.

## 3. Scope limits, each [UNCERTAIN] for what lies outside

* Flags: the exe compiles these TUs `/EHa /W3 /O2 /DMASHED_STANDALONE /arch:IA32` (build.bat:249, x87_tus.txt
  lines 148 and 162-165) with the default `/GS`. match.py's vc19 toolchain adds `/GS-`. The original units are
  `/GS` builds, so the original's `__security_check_cookie` call (0x004a2be9, `cmp ecx,[0x00616038]`) is not recorded
  as an event. [UNCERTAIN] that the exe's /GS code changes no value. Only the cookie store and check differ, not shown
  by this run.
* Callees outside unit + helpers are stubbed identically on both sides. The exe binds some of them to port bodies
  (FUN_00427780 = FontText_StringTableLookup, FUN_00552840 = FontCtx_SetRotation, raw forwarders for 0x00552d10 /
  0x00552750 / 0x00552d70 / 0x00556ca0 / 0x00556e90). Whether those equal the original is not tested here.
* 0x00458920: the exe's adapter calls the naked PK_458570 (Gameplay/PickupCollect_naked.inc, generated from
  re/tools/naked_specs/pickups_m3.json). The harness runs the original 0x00458570 on both sides. [UNCERTAIN] that
  PK_458570 is byte-equal to 0x00458570, not checked here.
* The RW core copies RwCore_00427e00 / RwCore_00428140 are not the refused bodies: in the run 45 object build they
  link their DDK object, IDENTICAL (verify/ddkregconv7_20261113/BYTECHECK.tsv lines 1219, 1243).
* Random-vector coverage: 300 vectors per unit and CW, 930 to 3722 stubbed calls per tally. Paths these inputs did not
  reach are not covered.

## 4. Files

`gen_tus.py` (writes `tu/single_*.cpp`, `tu/unit_*.cpp`), `unit_cmp.py`, `tu/mut/` (negative control), `equiv_ddk.txt`,
`equiv_vc19_single.txt`, `pass_cw037f.txt`, `pass_cw037f_ignecx.txt`, `pass_cw007f_ignecx.txt`,
`negctl_mut_00427e00.txt`, `ecx_at_556ca0_sites.txt`. No source under `re/match/` or `mashedmod/` changed. No tracker
edit.

## 5. DECISIONS.md

log/ is gitignored (`.gitignore:16`), so the L-156 evidence block cannot live on this branch. It was appended to the
L-156 row of the live `log/orch/DECISIONS.md` in the main checkout. The same text:

> **Evidence 2026-11-14 (L156-REGCONV-EVIDENCE, verify/l156_20261114/RESULT_L156-REGCONV-EVIDENCE.md):** VS2022 body
> vs original, unit + helpers, Unicorn 300 vectors x CW 0x037f/0x007f, negative control RED 10/10. 0x00427e00
> NOT-PROVEN, same 300/300. 0x00427f00 / 0x00427ff0 / 0x00428140 NOT-PROVEN, same 300/300 except ECX at the
> 0x00556ca0 tail-jump into the unpinned [DAT_0067d838+0x138] (U-5674, [UNCERTAIN] whether it reads ECX). 0x00458920
> NOT-PROVEN, same 300/300. No DIFFERS. equiv.py cannot pair them (register-arg helper call, C++ MGLOBAL slot).
