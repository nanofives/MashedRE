# RESULT D4-WSB-C4-BROAD-TAIL: `0x0055ae50` racediff with the tail jmp as call-out + heap-tracked write set

Verdict: **GREEN (step done), 0 rows promoted.** The racediff tool now records the tail `jmp 0x00565550` at
`0x0055ae66` as a call-out, and with it the canonical racediff is **C4-CLEAN** (6/6 calls, 4 signatures, equal on both
sides, hook installed alone, ON byte `0xE9`). The heap-tracked write-set leg is CLEAN 24/24 but compared **0 heap pages**,
as the PREREG predicted: the whole call tree writes only the 2 entry slots, in the caller's stack window. Under L-38 and
L-40 (a), `0x0055ae50` **stays C3**, with a dated blocker note through re-classify. Default gates unchanged.

Branch `orch/D4-WSB-C4-BROAD-TAIL` off `orch/integration` @ `ece372d27`. Tools + wrapper swap `dec473a30`. **PREREG
`PREREG_D4-WSB-C4-BROAD-TAIL.md` at `215af546d`, committed alone before any arm**, applied as written, no addendum.

## Harness change (`dec473a30`)

- `c4_racediff_v11.py` = `verify/wsbbroadws_20261027/c4_racediff_v10.py` + opt-in `C4_TAILJMP=1` (generator
  `make_tool6.py`, 7 asserted replacements). A `jmp imm` inside the target's own range whose destination leaves the
  target's own ranges is recorded as a call-out. Its args are read one dword above `esp`, because the return address is
  already on the stack at a tail jmp. That shift also applies to the ShadowAB Counter read on the transparent-template
  path. Intra-body jumps are dropped by the existing `inOwn(activeTarget, tu)` test. A jmp has no `add esp,N` after it,
  so its arity comes from the callee's declared arity (here `FUN_00565550(int, uint)` = 2) or its `ret N` walk. Without
  the env the tool is v10.
- `FUN_0055ae50` wrapper `ShadowAB::Run` -> `ShadowAB::RunTracked(..., SHADOW_STACK_WINDOW(), ...)`
  (`RwpSolverBroadphase3.cpp`), the swap BROAD-WS made in `879e1ad65`. Unarmed it calls only `_impl`.

## Static facts (`tail_disasm.txt`, anchored image `BDCAE093A30FBF22`)

`0x0055ae50`: loads `[esp+8]` / `[esp+4]` (`0x0055ae50` / `0x0055ae54`), `mov cx, word ptr [eax+0x20]` (`0x0055ae58`),
`mov eax, [edx]` (`0x0055ae5c`), stores the param_2 slot (`0x0055ae5e`) and the param_1 slot (`0x0055ae62`), and
`jmp 0x00565550` (`0x0055ae66`). `0x00565550`: reads both slots, `and eax, 0xffff` (`0x00565558`),
`mov ax, word ptr [ecx+eax*2+0xb810]` (`0x0056555d`), `ret`. It makes no store. Only bits 0-15 of param_2 are read, which is
the proof for the `C4_ARGW=565550:1:ffff` mask: the original passes CX with the caller's ECX bits 16-31, and the port
passes the zero-extended `ushort` (`movzx` at `.asi` `0x1005f4d4`). The port impl calls `_FUN_00565550` with a real
`call` (`.asi` `0x1005f4df`), so without v11 the ON side has a call-out that the OFF side cannot record.

## Build

`mashed_re.exe` `7961b78760eb357260d4e1df58aaf07f8a9e698faf2bd738c9d212c3aa7518ab`, `mashed_re_dev.asi`
`f40edfd4594646e6844507fd2916e136875f4299b3728a2389042207dcc18b99` (copied to the private root only, hash checked after
copy), `.map` `8ed2c6b4b0ce36f120516e6e2046d3f5262557611672baa61a62dda0e787f8f3`. All three were built at `dec473a30` with
`MASHED_NO_DEPLOY=1` (Build OK). Private root `D:/MashedWT/D4-WSB-C4-BROAD-TAIL_root` (robocopy of `original/`;
`MASHED.exe` `b9977dab...fe87`, `MASHED.exe.unpatched` = anchor `BDCAE093...3C0E`). `original/` not touched, and the
shared `.asi` was not redeployed. Every boot ran under the test slot and `mashed_lock.py run --label D4-WSB-C4-BROAD-TAIL`.

## G: default gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = the private root. The first try
(inside `arms.sh`) booted H_DEF (PID 6100, exited by itself) and then crashed in the runner's copy step, because the
`A4_OUT` folder did not exist. No output was scored from it. It was re-run with the folder created (`gates.sh`). All
four arms gave `exit=0 self_exited=True` (game PIDs 5864, 19376, 19924, 19816).

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## R1: canonical racediff (`run_tail.py`, v11)

Quick Battle, A4 calls 300-1200, `C4_TAILJMP=1 C4_DEREF=1 C4_ARGW=565550:1:ffff`, ON PID 16460, OFF PID 13188, attempt 1.
Return kind `16` (AX), arity 2. Summary `racediff_summary.tsv`.

| side | signatures (count) |
|---|---|
| OFF (original) | `[c565550,A0@0,18] ax=1` x2, `[c565550,A0@0,19] ax=1` x2, `[c565550,A0@0,1a] ax=1` x1, `[c565550,A0@0,1b] ax=1` x1 |
| ON (hook alone) | identical |

**C4-CLEAN**, installed `True` (`0xE9`). The OFF side records the tail jmp as `[c565550,...]`, which is what the tool
change is for. Arg 0 = the dword at `param_1+0` (`*param_1`) on every call of both sides. R2 (census) was not needed.

## H: heap-tracked write set (`run_shadow_tail.py`, `heap_pages.py`)

`MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`, census (track 0, mode 10, 4 cars, hold 60, `--hooks
0x0055ae50`), game PID 10936, killed by the driver at SAMPLES-DONE. Summaries `shadow_heap_summary.tsv`, `heap_pages.tsv`.

| n | ndiff | pages o/n | heap o/n | heap pages compared | stack / noise / ovf / ret | rv | proof | read-out |
|---|---|---|---|---|---|---|---|---|
| 24 | 0 | 0..0 / 0..0 | 0..0 / 0..0 | **0** | 0 / 0 / 0 / 0 | `00000001` x24 | `installed=e9 uninstalled=8b A/B-IS-REAL` | CLEAN-NO-PAGES |

No NOISY / UNTRACKED / OTHER-EXC line. The log also has `fn=FUN_00565550 PATCHBYTE installed=8b uninstalled=8b
SUSPECT-no-restore`: that hook was not installed in this boot (`--hooks` named only `0x0055ae50`). Its port wrapper ran
because the port impl calls it, and its byte `8b` is the original first byte (`mov eax,[esp+8]`). It is not a finding for
this row. The PREREG's NEED_PAGES arm was not run, as pre-registered.

## Decision (re-classify)

C4 gate: the racediff is clean and no stub is in the body (`FUN_0055ae50_impl` is one call). The note's only marker is
filed (U-9102). **The C4 is refused on L-38**: the heap-tracked leg compared 0 heap pages, and none can be compared in
any scenario, because the call tree's only stores are the 2 caller-frame slots (compared by the 4 KB stack window,
`stack=0`). The scope note found no reader of those slots. L-40 (a) (Mariano 2026-10-27) keeps this shape at C3 with no
rule relaxation, so no new decision row is filed.

Transaction (re-classify skill, edits made by hand): `hooks.csv` row `0055ae50` stays `C3,impl`, with a dated
`|| 2026-10-27 (D4-WSB-C4-BROAD-TAIL)` notes segment naming the blocker. `scenario` and `frida_diff` are unchanged.
`re/analysis/CHANGELOG.md` gets one `C3 (no change)` line under `<!-- ENTRIES -->`. No `UNCERTAINTIES.md` or `STUBS.md`
change. `scripts/lint_rva_bodies.py`: OK, 0 new.

## Notes

- The D4-WSB-C230-RECHECK warm-up note (`7845651a4`, not an ancestor of this branch) does not apply here: the impl
  reads no `NarrowFixKnobs` static, and sample 0 was CLEAN.
- `C4_TAILJMP` is generic. `0x0055dff0` (C4, slot writes then `jmp 0x0055b800`) has the same shape, if anyone re-runs it.

## Files

Committed (`verify/wsbbroadtail_20261027/`): `PREREG_D4-WSB-C4-BROAD-TAIL.md`, `make_tool6.py`, `c4_racediff_v11.py`,
`run_tail.py`, `run_shadow_tail.py`, `heap_pages.py` (copies, paths / labels changed), `tail_disasm.py` / `.txt`,
`arms.sh`, `gates.sh`, `racediff_summary.tsv`, `shadow_heap_summary.tsv`, `heap_pages.tsv`, this file. Source:
`RwpSolverBroadphase3.cpp` (wrapper). Trackers: `hooks.csv` (1 row note), `re/analysis/CHANGELOG.md` (1 line).

Raw (not committed), `D:/MashedWT/D4-WSB-C4-BROAD-TAIL_data/`:

| file | SHA-256 |
|---|---|
| `0055ae50/c4_racediff_samples.json` | `09ed39e060698b990161635bc6c0da89f785cc59b0e8edb0f7b844cda30a5f40` |
| `0055ae50/c4_racediff_result.json` | `b70bbde81c1b8885341a54ce4bdb56f23d88cdcde0a5e778db5c8632b496eb07` |
| `0055ae50/transcript.txt` | `9c8e9b0704b6895bfa4770208812b6c101954fd7cc47a570159d3a1e09d3c3a8` |
| `0055ae50_heap/shadow_ab.log` | `f260f6b41e458176a17562332824a4c749d22cd3056ea6e22cf7243cbb2143d6` |
| `0055ae50_heap/shadow_report.tsv` | `dee9da8bd0e2caa34f0eba47338d3327f1ad49cefdf6722889c47890025ac158` |
| `0055ae50_heap/shadow_transcript.txt` | `9f66cc06a22ab165ead20e83038b327b37f2ea845aeefc7fb2ddc8d2a2e1968f` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `arms_run.log` | `17712e94968cd8379eb04f057523ce8692073c7dc730c2b257f3dc030d6eec97` |
| `gates_run.log` | `91d8ee6d88f3db3ee673b44171ccdc71e5111c0df168695cdb856de35f0c0b6e` |
| `build.log` | `b2b85487f6c77b77bda7a8018cdcfc06158583958587ebaa4d1a4f5a858c13ca` |

Game PIDs, all spawned by this step: racediff 16460 / 13188, shadow 10936, gates 6100 (first try), 5864, 19376, 19924,
19816. Each was ended by its driver by PID or exited by itself. No MASHED from this step is running, and no watcher of
this step is left.
