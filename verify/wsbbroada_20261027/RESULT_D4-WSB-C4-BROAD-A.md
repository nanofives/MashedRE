# RESULT D4-WSB-C4-BROAD-A: racediff C4 for the BROAD no-store rows

Verdict: **GREEN.** **All 5 rows go C3 -> C4**, each on a canonical-scenario racediff with its hook installed alone (ON byte `0xE9`) and out-memory leg A. Default gates unchanged on this branch's exe. U-9026 (EFLAGS across
`0x0055deb0`) resolved statically.

| row | racediff that decides | result | level |
|---|---|---|---|
| `0x0055deb0` | Quick Battle, A4 calls 300-1200, v8, own body + ECX/EDX leg | C4-CLEAN 6/6, `eax=h0 ecx=kept edx=kept` both sides | **C3 -> C4** |
| `0x0055bb70` | census, `C4_HOLD=400`, A4 calls 300-30000, v8 | C4-CLEAN 6/6, 1 sig each side | **C3 -> C4** |
| `0x0055e050` | Quick Battle, A4 calls 300-1200, v9 field-source tokens (X3) | C4-CLEAN 6/6, 1 sig each side | **C3 -> C4** |
| `0x0055abb0` | census, `C4_HOLD=400`, A4 calls 300-30000, v10, `0x00565200` arg 1 masked (Y1) | C4-CLEAN 6/6, 3 sigs, same counts | **C3 -> C4** |
| `0x0055c230` | census, `C4_HOLD=400`, A4 calls 300-30000, v10, static-init helpers transparent (Z2), control Z1 | C4-CLEAN 6/6, 2 sigs; Z1 CONTROL-SAME | **C3 -> C4** (see the caveat in section 5) |

Branch `orch/D4-WSB-C4-BROAD-A` off `orch/integration` @ `2d75d00d1`; `orch/integration` @ `84a47e645` (the L-38 demote,
trackers and docs only, no source) merged in before the tracker transaction. PREREG `PREREG_D4-WSB-C4-BROAD-A.md`
(`96aa3ff92`, committed alone), addendum 1 (`572ff3c6b`), addendum 2 (`31de21c29`), addendum 3 (`84aa9ccdd`), each
committed alone before the arms it names.

## Build and method

`mashed_re.exe` SHA-256 `8b1ce6b0...7d4c`, `mashed_re_dev.asi` `ca909408...de2b`, built from this worktree at
`2d75d00d1` with `MASHED_NO_DEPLOY=1` (Build OK). No source change on this branch. Private root
`D:/MashedWT/D4-WSB-C4-BROAD-A_root` (robocopy of `original/`, `MASHED.exe.unpatched` = anchor `BDCAE093...3C0E`), this
build's `.asi` copied next to `dinput8.dll` there. `original/` not touched. Every boot under the test slot and
`mashed_lock.py run --label D4-WSB-C4-BROAD-A`.

Racediff = the BVH-FU v7 tool (`verify/wsbbvhfu_20261027/c4_racediff_v7.py`) with opt-in options added by asserted
replacements, each option off unless named for an arm:

- v8 (`make_tool3.py`): `C4_OWN_SYMS` (extra own-body symbols), `C4_TRANSP_SYMS` (extra transparent helpers),
  `C4_REGS` (ECX/EDX at return equal to entry, per sample).
- v9 (`make_tool4.py`): `C4_DEREF` (a pointer equal to the dword at entry-arg k + off is named `A<k>@<off>`),
  `C4_CONTROL` (both boots run the original; verdict CONTROL-SAME / CONTROL-DIFF, never C4-CLEAN).
- v10 (`make_tool5.py`): `C4_ARGW` (declared-width masks, the v6 `0x005646c0` mechanism).

Driver `run_broada.py` (copy of `run_bvh3.py`). Out-memory leg for all five rows: leg A of the scope note (no store
outside the own frame in the original body, `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md` BROAD table). The
port bodies store only to their own locals as well (`RwpIntegrator.cpp:134-137`, `RwpSolverGlue5.cpp:115-121`, `:133-146`,
`:175-201`, `RwpSolverBroadphase3.cpp:518-529`). The racediff compares call-outs, their arguments and the return. A
pointer argument is compared by identity, not by what it points to: `0x0055c230`'s three floats in `loc` reach the
indirect callee by pointer and are not compared here.

## Default gates (`mashed_re.exe` `8b1ce6b0`)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT` = the private root. All four
`exit=0 self_exited=True` (game PIDs 14612, 19388, 5296, 2704).

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

## 1. `0x0055deb0` (register-ABI row)

The hook jumps to the naked shim `RwpWorldSolverHandle_RegAbi` (`RwpIntegrator.cpp:188`), which calls the C body
`_RwpWorldSolverHandle`. On the default path that body calls `b5cSelfTestEnabled` (cached `GetEnvironmentVariableA`,
`RwpIntegrator.cpp:48-53`) and `__security_check_cookie`, both transparent in this arm. Quick Battle (ON PID 13736,
OFF PID 2280): OFF `eax=h0 ecx=kept edx=kept` x6, ON the same x6. ON byte at `0x0055deb0` `0xE9` (the shim installed,
as the 2026-07-28 demotion note requires). ECX and EDX are kept on both sides, so the register contract the demotion
was about holds with the hook live in a race.

**U-9026 (EFLAGS):** `scan_flags_after_call.py` on the anchored image: 22 references to `0x0055deb0`, all direct
`call rel32`, no dword reference. From each return address it tracks the six status flags still holding the callee's
value and follows `jmp imm` and `call imm` (return stack). At all 22, every flag is rewritten before any instruction
tests one: 11 sites rewrite them in the caller (`add esp,N` / `shr` / ...), 11 reach a callee that rewrites them first
(`0x00543df0` returns flag-neutral, then `add esp,0x18` at `0x004b51bc`; `0x005c4bb0` `sub esp,8`; `0x005c4c60`
`sub esp,8`; `0x005c4d30` `test ecx,ecx`; `0x005c4da0` `xor edi,edi`; `0x005c4df0` `imul`). No caller reads flags
across the call, so the shim needs no PUSHFD/POPFD. Output: `scan_flags_after_call.txt`.

## 2. `0x0055bb70` (NOT-EXEC in the 45 s census)

Static callers in the image: `0x00459f0c` (in `0x00459620`), `0x0048185f` (in `0x00481810`), `0x005612b8` (in
`0x00561280`), `0x00561480` (in `0x00561390`, a loop guarded by `jbe 0x0056156e` at `0x0056145f`). PREREG search:

| step | scenario | result |
|---|---|---|
| 1 | Quick Battle, A4 300-1200 | not observed, both sides, 2 attempts |
| 2 | Quick Battle, A4 300-6000 (reached ~5800) | not observed, both sides, 2 attempts |
| 3 | census, `C4_HOLD=400`, A4 300-30000 | **C4-CLEAN** 6/6, 1 sig each side (ON PID 13412, OFF PID 6492) |

Step 3's first attempt used `C4_HOLD=120`: the launcher ended the game at 120 s while the capture was still polling, the
Frida script died with the process (`script has been destroyed`) and the samples were lost for `0x0055abb0`. I stopped
that driver (my PIDs only, tree from 8320, game PID 10140) and re-ran with `C4_HOLD=400`, longer than the tool's own
150 s capture cap, so the tool ends the capture and kills its game by PID. The callee `0x0055bae0` stays C3 (BROAD-WS);
the racediff compares the call-out to it, not its effect.

## 3. `0x0055e050` (addendum 1)

v8 Quick Battle (ON 10536, OFF 2368): HOLD `BEHAVIOR-DIFF`, 4 sigs each side, differing only in the first argument of
the `0x0055ab30` call, `param_1[9]` (`0x0055e063`, port `RwpSolverGlue5.cpp:119`), named by its heap distance from
`param_1`: OFF `A0+ba8/e28/e68/ea8`, ON `A0+1370/1470/16b0/16f0`.

| arm | boots | tokens | result |
|---|---|---|---|
| X1 | original vs original (4980 / 1124) | v8 | **CONTROL-DIFF**: `A0+13b0/1530/1570/16f0` vs `A0+930/b70/bb0/eb0` |
| X2 | original vs original (2556 / 18256) | field-source | **CONTROL-SAME**, `[c57c590] [c55ab30,A0@24,A0+0,Sloc] eax=A0+0` x6 both |
| X3 | hook ON vs original (12120 / 16104) | field-source | **C4-CLEAN**, the same sig x6 both |

The heap distance differs between two boots of the unhooked original, so the v8 token does not compare across boots
and the v8 HOLD is not a port difference. Named by its source field, the argument is `param_1[9]` on every call of
both sides. Addendum 1 rule met (X1 CONTROL-DIFF, X2 CONTROL-SAME, X3 C4-CLEAN).

## 4. `0x0055abb0` (addendum 2)

Quick Battle A4 300-1200 and census A4 1-3000: not observed (both sides, 2 attempts each). Census `C4_HOLD=400`,
A4 300-30000, v8 (ON 16228, OFF 3952): HOLD `BEHAVIOR-DIFF`, only argument 2 of the `0x00565200` call: OFF `h1` x6,
ON `18` / `a`. The original loads it with `mov dx, word ptr [ecx+0x20]` (`0x0055abb7`) and pushes all of EDX
(`0x0055abe6`), so bits 16-31 are the caller's EDX. `0x00565200` reads it as `and ecx, 0xffff` (`0x00565208`). The port
passes the zero-extended `ushort` (`RwpSolverBroadphase3.cpp:522`, `:525`). Y1 (v10, `C4_ARGW=565200:1:ffff`, ON 5076,
OFF 5208): **C4-CLEAN**, `[c565200,h0,18,S+34] eax=S+34` x2, `[c565200,h0,a,S+34] eax=S+34` x1,
`[c565200,h0,a,S+54] eax=S+54` x3 on both sides.

## 5. `0x0055c230` (addendum 3)

Quick Battle A4 300-1200 and census A4 1-3000: not observed (both sides, 2 attempts each). Census `C4_HOLD=400`,
A4 300-30000, v8 (ON 10920, OFF 18028): HOLD `BEHAVIOR-DIFF`. Every signature has the same body part,
`[<indirect>] [c55c0f0,A4+0,A1+0] eax=A4+0`. The first token is the target of `call dword ptr [edx+0x18]` at `0x0055c2a8`
(`edx = [param_1+0x5c]`, `0x0055c2a3`), so it depends on which object the sampled call receives. OFF `c55c680` x3 /
`c55cdd0` x3, ON `c55cdd0` x3 / `c57b420` x3, one ON sample led by `__Init_thread_header` / `_getenv` /
`__Init_thread_footer` (the first-call init of a function-local static in the ON body, harness code, once per boot).

| arm | boots | result |
|---|---|---|
| Z1 | original vs original (20244 / 6360) | **CONTROL-SAME**: `c55cdd0` x4, `c57b420` x2 on both |
| Z2 | hook ON vs original (9756 / 19568) | **C4-CLEAN**: `c55cdd0` x3, `c57b420` x3 on both |

Addendum 3 rule met (Z1 CONTROL-SAME, Z2 C4-CLEAN). **Caveat for the lead:** counting every original boot that reached
this window, 3 of 4 gave `{c55cdd0, c57b420}` (Z1 x2, Z2 OFF) and 1 gave `{c55c680, c55cdd0}` (the v8 OFF boot 18028).
So the original itself does not always give the same sample set this late in the race. Both hook-installed boots
(10920, 9756) gave `{c55cdd0, c57b420}`. The body part after the indirect call is identical on all 6 boots. The
promotion follows the pre-registered rule; it rests on set equality in a window where the original varies 1 time in 4.

## Tracker transaction (re-classify)

`apply_reclass.py 0055deb0 0055bb70 0055e050 0055abb0 0055c230` (run once from the worktree root): `hooks.csv` rows C3/impl -> C4/verified with
scenario, `frida_diff` = `verify/wsbbroada_20261027/racediff_summary.tsv` and a dated notes segment; U-9026 marked
RESOLVED with the scan; one CHANGELOG line per row plus one for U-9026 under `<!-- ENTRIES -->`. No `STUBS.md` change
(no stub in any of these bodies). Rows `0055fea0 0055ff90 00564040 00565260` and all write-set / ShadowTrack code not
touched.

## Harness notes (for `re/HARNESS_BACKLOG.md`, not acted on here)

- v7's `A<k>+<d>` token (a pointer within `0x4000` above an entry arg) is not stable across boots when it names a
  second heap object (X1). `C4_DEREF` field-source tokens fixed it for `0x0055e050`. Worth checking for every earlier
  C4-CLEAN that relied on `A<k>+<d>` of a separate object: a CLEAN there needed the two boots' heap layouts to agree.
- The census driver loses all samples when the launcher's hold ends before the capture loop: keep `C4_HOLD` above
  150 s, or read samples before the hold ends.
- Rows that first run late in the race (`0x0055abb0`, `0x0055bb70`, `0x0055c230`) need the long census window; the
  45 s census (D4-WSB-CENSUS) marks `0x0055bb70` NOT-EXEC, yet it runs within this 150 s capture.
- `0x0055abb0`'s case is the second `push edx` after a 16-bit load seen by this tool (after `0x005646c0`): any caller
  that builds a 16-bit argument this way gives a BEHAVIOR-DIFF without a mask.

## Files

Committed (`verify/wsbbroada_20261027/`): `PREREG_D4-WSB-C4-BROAD-A.md`, `PREREG_ADDENDUM_1.md`, `PREREG_ADDENDUM_2.md`,
`PREREG_ADDENDUM_3.md`, `make_tool3.py` / `c4_racediff_v8.py`, `make_tool4.py` / `c4_racediff_v9.py`,
`make_tool5.py` / `c4_racediff_v10.py`, `run_broada.py`, `scan_flags_after_call.py` / `.txt`, `apply_reclass.py`,
`racediff_summary.tsv`, this file. Trackers: `hooks.csv`, `UNCERTAINTIES.md`, `re/analysis/CHANGELOG.md`.

Raw (not committed), `D:/MashedWT/D4-WSB-C4-BROAD-A_data/`. Every PID killed by this step was spawned by this step
(lock wrapper 11800 orphaned by a driver console-encoding crash, the tree under driver 8320 incl. game 10140; all
other games were ended by the tool by PID). No MASHED from this step is running.

| file | SHA-256 |
|---|---|
| `0055deb0/c4_racediff_samples.json` | `05194e512cbc6089bb39a850b5150123ccd6e58dfa7133f4fc1933766fd30646` |
| `0055bb70_census_long2/c4_racediff_samples.json` | `34e2d894a8c0674de42a8efd8e1815dc6033cff628d5b206c7cf04b700794caf` |
| `0055bb70/c4_racediff_samples.json`, `0055bb70_w6000/...`, `0055abb0_census/...`, `0055c230_census/...` (empty) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `0055e050/c4_racediff_samples.json` (v8 HOLD) | `5d0634191d2cc63b7d9b77fa04dcf8a4056025fda6b58aad4bb981435ed9652b` |
| `0055e050_X1/c4_racediff_samples.json` | `27204a089aa8af6503e61f8a100eac7c06836833758abc1e28b49a6fb4ab266c` |
| `0055e050_X2/c4_racediff_samples.json` | `d3366b1b809364aa5bb6f1198b6e96723e65cf8dce74beec7bdbd276e34798e1` |
| `0055e050_X3/c4_racediff_samples.json` | `d3366b1b809364aa5bb6f1198b6e96723e65cf8dce74beec7bdbd276e34798e1` |
| `0055abb0_census_long2/c4_racediff_samples.json` (v8 HOLD) | `17b7711eecc6b67d890ba3eef890f0e959164a5cc1e200d166a3ea65283dea31` |
| `0055abb0_census_Y1/c4_racediff_samples.json` | `269eabd6ed1b65abf1e8a7c41634e5a2862d9136827a793ce03422f222ec2636` |
| `0055c230_census_long2/c4_racediff_samples.json` (v8 HOLD) | `cf3494823885c525882109bba6423826f7d0906aaeaa9057c52bf61147100bb3` |
| `0055c230_census_Z1/c4_racediff_samples.json` | `4b303c7796437502296f91494370c9aa85870a13ce7ae689e220e4a694b716d0` |
| `0055c230_census_Z2/c4_racediff_samples.json` | `f87dfc0cf2c81003fb6b7266ea8c2a98d36ec78eadb0a05c6dbbd83a619353a2` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |

X2 and X3 hash equal because both files hold the same two sample sets (`{"off": ..., "on": ...}` with one identical
signature x6 on each side).
