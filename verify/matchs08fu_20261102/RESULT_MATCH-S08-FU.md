# RESULT MATCH-S08-FU, 2026-11-02

**Verdict: GREEN.** Branch `orch/MATCH-S08-FU`, stacked on MATCH-S08 `fe620db75` (not yet merged; this branch
carries S08's commits). Three byte-matched bodies swapped into both targets: **0x00442c80** RubberBandGate and
**0x00442ce0** RubberBandGrip (S08's two NOT-SAFE rows, now in the literal-constant form), and **0x00413f90**
TimerGetBasePtr (T07's one sound swap). Both rubber-band bodies are reached from `WinMain` through A5, so they meet the
`match` route; both are **already C4** (frida route), so the tracker leg is a `file`/`notes` update, no promotion. The 0x00434720 residue: unchanged at 57.1%. A 1 h bounded permute reached 72.9%, but all 3 of its rewrites change behaviour and the variant is rejected (section 4). The T07/T11 pause overlay is **not** ported: it is a
feature with a gate-visible branch, so it is a user decision (below), not a swap.

Gates on the final build (exe `22cc3200`): G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = `merge_train.json` refs (unchanged; also unchanged on the rubber-band-only build).

## 1. Swaps

| rva | name | source | match.py (`--toolchain ddk --bytes`) | relocheck | wrapper (both targets) | x87 | old body (removed) |
|---|---|---|---|---|---|---|---|
| 0x00442c80 | RubberBandGate | `re/match/s08/00442c80.c` | 100.0% 20/20, 63 B BYTE-IDENTICAL | OK checked=4 unchecked=0 | `Match/MatchS08_00442c80.cpp` | yes | `Vehicle/ForceIntegratorLeaves.cpp` |
| 0x00442ce0 | RubberBandGrip | `re/match/s08/00442ce0.c` | 100.0% 81/81, 249 B BYTE-IDENTICAL | OK checked=19 unchecked=0 | `Match/MatchS08_00442ce0.cpp` | yes | `Vehicle/ForceIntegratorLeaves.cpp` |
| 0x00413f90 | TimerGetBasePtr | `re/match/s08fu/00413f90.c` | 100.0% 2/2, 6 B BYTE-IDENTICAL | OK checked=1 unchecked=0 | `Match/MatchS08FU_00413f90.cpp` | no | `Util/TimerState.cpp` (deleted) |

**Literal-constant form (the S08 NOT-SAFE reason).** S08's sources read their float constants through `MGLOBAL` from
the original `.rdata` (`_DAT_005cc9b8` = 7.0; `_DAT_005cc320` = 1.0, `DAT_005d757c` = 0.0, `_DAT_005cc318` = 0.6,
`_DAT_005cc32c` = 0.5, `_DAT_005cc564` = 0.25). The exe's copy of the original `.rdata` reads 0, so those sources
would have returned 0 in the exe. Both sources now spell the constants as literals. The DDK emits its own `__real`
constants, the relocations are masked, and the code bytes are unchanged (match.py BYTE-IDENTICAL), and relocheck
resolves each literal against the original's operand value (checked 4 and 19, unchecked 0). Each source's header
comment records the edit.

**Callee binds.** 0x00442c80: `FUN_0040e350` -> forwarder `MatchS08_00442c80_Call0040e350`: exe = the stand-in
`mashed_re::Vehicle::Fi_GameMode()` (`Vehicle/ForceIntegratorStubs.cpp`, U-9784), asi = the original at 0x0040e350.
These are the same calls the old bodies made in each target. 0x00442ce0: `FUN_0040e340` -> `GetLiveCarCount`
(`Match/MatchS02_0040e340.cpp`, both targets, a getter whose result the original discards at 0x00442d06; the old exe
body called the no-op `Fi_GameModeTick`), `FUN_0046dbe0` -> `VehicleRacePositionGet` (`Vehicle/VehicleState.cpp`,
both targets).

**C++ names.** `mashed_re::Vehicle::RubberBandGate(int)` / `RubberBandGrip(int, float, float)` are one-line
forwarders in the wrappers (no RVA comment). Callers are unchanged: `ForceIntegrator.cpp` A5RubberGate /
A5RubberGrip, and the .asi `LeafGateDispatch` / `LeafGripDispatch`. The original never reads Grip's arg 2, and the
forwarder passes its bits through. The .asi installs stay on `RubberBandGateEntry` / `RubberBandGripEntry`
(`Vehicle/ForceIntegratorLeaves.cpp`, the MASHED_LEAF_SELFTEST dispatchers). That TU now holds only the .asi part.
VS2022 inlines each matched body into its forwarder, and `/OPT:REF` drops the unreferenced extern "C" symbol, so the
exe map names the forwarders (`?RubberBandGate@Vehicle@mashed_re@@YAHH@Z` in `MatchS08_00442c80.obj`). The code is
the matched source's.

**0x00413f90.** `re/match/s03/raw_00413f90.c` (raw converter, 100%) left `DAT_005f2b10` as a plain `extern`
(relocheck: unchecked 1). `re/match/s08fu/00413f90.c` is the same function in the S08 port form (preamble +
`MGLOBAL`): relocheck checked=1, unchecked=0. The export name `TimerGetBasePtr` (hooks_registry) and the .asi install
at 0x00413f90 are kept.

Lint (both builds): `[rva-lint] 2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`, `[asm-lint] OK`.
`re/tools/dual_copy_allowlist.txt` unchanged. `rva_body_scan` binds 0x00442c80 / 0x00442ce0 / 0x00413f90 to the
wrappers, one body each, `[EA]`.

## 2. Exe callers (L-68)

`D:/MashedWT/MATCH-S08_data/tools/exe_reach.py` (S07/S08's tool) on the final exe and its map (`exe_reach.txt`):

| rva | exe symbol | direct chain from WinMain |
|---|---|---|
| 0x00442c80 | `mashed_re::Vehicle::RubberBandGate` | <- VehicleWheelForceIntegrate (A5, 0x0046ddb0) <- VehicleControlIntegrate <- VehiclePhysics_StepCarsOuter <- TrackRenderer::UpdateCar <- RenderFrame <- WinMain |
| 0x00442ce0 | `mashed_re::Vehicle::RubberBandGrip` | the same chain |
| 0x00413f90 | `_TimerGetBasePtr` | none (0 direct sites; address taken by the AutoReg install table only). exe-callers=0 |

A5 runs on every racing frame of the default physics path, so the gate scenario executes both rubber-band bodies.
(No per-function trace was taken; this is the direct-call graph plus A5's default-ON status.)

## 3. Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_EXE` = a frozen copy of each build
(`D:/MashedWT/MATCH-S08-FU_data/exe{1,2}/mashed_re.exe`), `A4_ROOT` = `MASHED_ROOT` = `D:/MashedWT/MATCH-S08-FU_root`
(robocopy of `original\`), test slot + `mashed_lock.py run --label MATCH-S08-FU`. `mashed_lock.py status` was
checked first. The set-2 run queued behind the merge train's solo window (05:13), then held the lock. Every arm
exited 0 and self-exited, with no reruns and no crashes.

| set | exe | H_DEF step / gates / ptrace | B_DEF_K0 / K1 / K26 | ref (`merge_train.json`) |
|---|---|---|---|---|
| arms1 (rubber-band swaps) | `3c436b4b` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| arms2 (final, + 0x00413f90) | `22cc3200` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here. The `full_set_paths` are `Vehicle/`, `Ai/` and
`Collision/`, and this branch edits `Vehicle/ForceIntegratorLeaves.cpp`, so the train runs `-Full` on enqueue.

## 4. 0x00434720 residue (T11, 57.1%)

No body swap is possible. 0x00434720 has no port body and no exe caller (T11), and `re/match/s08/00434720.c` is
compiled into neither target, so nothing here can move a gate.

Start: 57.1% (regs 70.5%, ops 97.1%, 2198/2194 ins), the S08 v4 source. The first divergence (`match.py --show`,
`perm/show_v4.txt`) is register assignment and frame layout. The original materialises zero in `ebx` (`xor ebx,ebx`
before `push edi`) and 2 in `edi`, while ours does the reverse. Our frame is 0x10 bytes larger (`[esp+0x88]` vs
`[esp+0x78]` for the first byte stores), with three extra stores (`[esp+0x14c]`, `[esp+0x94] = 3`,
`[esp+0x6c] = 2`). S08's helper note gives the same residual (locals by use count + liveness).

Bounded permute: `re/tools/match/match_permute.py <copy> 0x00434720 --rounds 12 --walk 80 --jobs 6 --time 3600`
(S08's own permute run on this function stopped at 19 min with no gain). Here it ran to the 1 h background cap:
round 1 `move` 67.2%, round 2 `swap` 71.8%, round 3 `cmp` 72.9% (5238 compiles, `perm/permute1.log`). **Rejected:**
each rewrite changes behaviour (diff of `perm/best_00434720.c` against the source):

| rewrite | source line | change | why rejected |
|---|---|---|---|
| move | 504 | `tx = P2.x + 10.0f;` moved below the `FUN_00427f00(bufF8, tx, P2.y + 45.0f, ...)` that reads `tx` | the draw reads the old `tx` |
| swap | 535 | the second row's `FUN_00427f00(bufF8, 270.0f, ty, cB, 0.6f, 2)` moved above its `sprintf` and above `ty = P2.y + 79.0f` | draws the previous row's text at the previous `ty`. The original listing has the source order: `push 0x5cd9b8` / `call 0x4a2b60` (sprintf), then `fld` / `fadd [0x5cd994]` / `fstp`, then `push 0x43870000` / `call 0x427f00` (`perm/show_v4.txt` lines 1120-1137) |
| cmp | 723 | float `end > 580.0f` -> `end >= 581.0f` | not equal on (580, 581) |

The score gain shows only where the instruction alignment lands (ops 97.5%), not the original's source order. Lead
for a later row: the `move` at line 504 gives +10 points, so check whether `tx` already holds `P2.x + 10.0f` there
in the original (an earlier assignment in the same branch). If it does, a behaviour-preserving form of that move
exists. Not done in this budget.

## 5. The T07/T11 pause overlay: a user decision, not a swap

0x0043d7c0 (pause menu, T07) is byte-identical (S08) and 0x0043bf30 / 0x00434720 (T11) are below it in the menu
draw. None has a port body in either target, and none has an exe caller: the exe's pause is
`GameFlow_SetPaused` / `GameFlow_RequestExit` in `mashedmod/src/mashed_re/exe_main.cpp:2249-2264` (ESC toggles the
pause, ENTER while paused quits, and line 2263 states "Restart (-0xe00000 -> FUN_0040de10) is left for the full
port"). The exe frontend draws through RVA-less port code, not through 0x0043c5b0 / 0x0043bf30 / 0x00434720 (T11).

Wiring 0x0043d7c0 into the exe is a feature port, not a body swap. Port bodies for its 19 direct callees
(`rva_body_scan`, this branch):

| status | callees |
|---|---|
| port body in both targets | 0x0040b810, 0x00422b30, 0x004298c0, 0x00429aa0, 0x0042ae10, 0x0042aeb0, 0x0042af50, 0x0042bf30, 0x0046dc00, 0x00413f90 (this branch) |
| .asi only | 0x0040e360 (RaceModeSet), 0x0042b950, 0x0042c150, 0x0042c220, 0x00493570, 0x00493580 |
| no port body | 0x0040ab40, 0x0040ac80, 0x0040de10 (Restart; all three matched 100%), 0x0043d2a0 (54.0%, register-arg callee 0x00432b30 at 74.2%) |

It also reads and writes about 40 frontend words at `0x0067e914..0x0067f1a8` and the control blocks at `0x007f1038` /
`0x007f1a14`. The exe frontend keeps its own state in port objects (T07 table), so the overlay would need its
records set up by the exe. Restart reaches 0x00417750, which writes the live AI-difficulty seed `0x0089a37c` (T07).
That path changes behaviour as soon as Restart is pressed. Per WORKER_RULES this needs a default-OFF knob plus a
PREREG, and the open question is a decision for Mariano:

> **Decision proposed (MATCH-PAUSE-OVERLAY):** port the original pause overlay into the exe as a default-OFF knob
> (`MASHED_PAUSE_OVERLAY=1`)? It needs exe bodies for 0x0040ab40 / 0x0040ac80 / 0x0040de10 (matched), 0x0043d2a0 (not
> matched, 54%), the six .asi-only callees, and a binding of the 0x0067e9xx/0x0067eaxx frontend words to the
> exe's frontend state.
> Options: (a) yes, as its own step chain (bodies first, then the knob and a PREREG with a Restart arm), with Restart
> allowed to write `0x0089a37c` as the original does; (b) yes without Restart (Resume / Quit records only) for now;
> (c) no, leave the 4 T07/T11 rows matched without an exe caller (L-68) until the frontend port reaches the pause
> screen.
> Recommendation: **(c)**. The exe's pause works today, the default gates never open the pause, and (a) is about 10
> bodies plus a state binding for one screen that no gate measures.

## 6. Re-classify text (NOT applied here; the tracker leg is a later text-only row)

All three swapped rows are **already C4** by the frida route (hooks.csv: 0x00442c80 `ModeGatedPlayerCheck` C4
verified, 0x00442ce0 `VehicleRubberBandSpeedModifier` C4 verified, 0x00413f90 `TimerGetBasePtr` C4 verified). The
match route adds a second, static witness and moves the `file` cell. No level changes.

Common evidence: `match.py <source> --toolchain ddk --bytes` BYTE-IDENTICAL under `/O2 /GS /Ob1`, `relocheck.py` OK
with unchecked=0, one body (wrapper, both targets), gates unchanged on this build (section 3).

| rva | level | `file` -> | `notes` += |
|---|---|---|---|
| 0x00442c80 | C4 (keep) | `mashedmod/src/mashed_re/Match/MatchS08_00442c80.cpp` | `match evidence: identical (literal-constant form), swapped, exe-callers via A5 VehicleWheelForceIntegrate (c4=match also holds); verify/matchs08fu_20261102` |
| 0x00442ce0 | C4 (keep) | `mashedmod/src/mashed_re/Match/MatchS08_00442ce0.cpp` | same |
| 0x00413f90 | C4 (keep) | `mashedmod/src/mashed_re/Match/MatchS08FU_00413f90.cpp` | `match evidence: identical, swapped, exe-callers=0 (L-68: no level from the match route); verify/matchs08fu_20261102` |

So the "C4 candidates" for this row are 0 new promotions: the two rubber-band rows already are C4, and they now
satisfy the `match` route as well (exe caller via A5).

## 7. Builds

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, .asi not deployed.

| build | content | exe sha256 | asi sha256 |
|---|---|---|---|
| build1 | rubber-band swaps | `3c436b4baf01893b831666bb0e0e501d622638959f6f6cb72c2ec7cc3b894401` | `b885122370c1bd09c6e90ea2daab32057222c7e543857a0e90d852723e740306` |
| build2 (commit fdfb83e56, final) | + 0x00413f90 | `22cc32007046f82dc99b801798348c2b7f05ba099b323fa8e4151f73efe0bcb0` | `865a35f740cf8aaa1177d131ddb60a201011f5e331474524dc85cf6e0d0f9937` |

## 8. Raw files (not committed; `D:/MashedWT/MATCH-S08-FU_data`)

| file | sha256 |
|---|---|
| arms1/H_DEF.step.csv, arms2/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms1/H_DEF.gates.csv, arms2/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms1/H_DEF.ptrace.log, arms2/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms{1,2}/B_DEF_K0, K1, K26 .step.csv (all six) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| exe1/mashed_re.exe | 3c436b4baf01893b831666bb0e0e501d622638959f6f6cb72c2ec7cc3b894401 |
| exe2/mashed_re.exe | 22cc32007046f82dc99b801798348c2b7f05ba099b323fa8e4151f73efe0bcb0 |
| exe2/mashed_re_dev.asi | 865a35f740cf8aaa1177d131ddb60a201011f5e331474524dc85cf6e0d0f9937 |
| build1.log (Build OK) | 0b2b48690f3142c0810ca530b0c3702c6f3e0ac9ef588f0191965e4f266ee7a2 |
| build2.log (Build OK) | 7a899f1545ffa4ebfe570a4a57a395397b9e254f0456ec25cd6ccaf982c32b55 |
| gates1.log | 4ab6a05025e9c3e34b8e7f4082aeb9886974e0d20e977338f9b20d43f33e7778 |
| gates2.log | a3f7b0c88c5dd20f5523b261bcd0f9ebcd621a662c3e239951274184538a2db9 |
| exe_reach.txt | 4db8bbee7a6dadc0263d4f7e1d80bda9c34804854f3c6904324545a1b5350c5f |
| perm/permute1.log | 4403001f753204f66944a873b8eb91c99c046045f176f05c7c542eb4fd1081b1 |
| perm/best_00434720.c (rejected variant) | e80458c4389a6ba700d2599787593e69060898cc12a5b3a6c4b018c8f76e52f9 |
| perm/show_v4.txt | b6700be6cec1d15b5cf951a2db4ff3ea2efc48707ac470977b38af14369ff5d8 |
| perm/show_best.txt | 77f11e52e20360cdae013085ea40a44d882642917ab2939f577d7b6f587dd8e4 |
| bodies_pause.py (rva_body_scan query for section 5) | 58a2a9ebec0ee156badd545a03c36a8ace698b06c4a6a84e3dafeba75be916cd |
| gates.sh | 6d1bd7d8c97e7dde371ad742ce30131f03a396cf03f509f23c35dd2c36d6729f |
