# RESULT PORT-TOP-AUDIO-RWA (2026-11-13, run 43, section D4 T1)

Verdict: **RED**. Top-down row (knob path only). Branch `orch/PORT-TOP-AUDIOTICK-STOPS` (kept from the handoff, ledger row
PORT-TOP-AUDIO-RWA), merged with `orch/integration` at 8d5f90d40.

**Self-test results:**
- Control arms (AUDIOTICK only, gate closed): **`[OT_TIMER] self-test PASS (0 of 8413 checks failed)`** on both builds,
  600 frames, audio branch 600.
- Both-knob arms (AUDIOINIT + AUDIOTICK): **`[OT_AUDIOINIT] self-test PASS (0 of 12 checks failed)`** on both builds. The
  OT_TIMER self-test prints no summary, because the run faults inside the first 0x004926c0 call.

What landed:
1. 0x005a6ea0 and 0x005a6f30 have L-126 provisional bodies (`re/match/PROVISIONAL.tsv`, knob path only, never C4).
2. Six `c4=frida-replay` rows now bind as exe bodies.
3. The 0x0045cbe0 forced stop is gone.

The both-knob boot gets past the 0x005a6ea0 stop and then faults in 0x005aab00 (access violation, read of 0x0). The cause is
that 0x005aad40 returns DAT_007dcd20 = 0, the RenderWare-audio root object. In the exe nobody creates that object:
0x004627f0, the sound engine HW init (C2), still runs the hand piece 3 (`HandTop_InitAudio`, exe audio) under
`MASHED_OT_AUDIOINIT=1` (`Match/PortTop_004669b0.cpp`). 0x005a6f30 is never reached, because 0x005a60b0 calls it after
0x005aab00. So the brief's bar ("get past 0x005a6ea0 / 0x005a6f30 without exiting") is **not met**. The next blocker is
0x004627f0, not a body in this closure.

## 1. Bodies for 0x005a6ea0 / 0x005a6f30 (L-126: equiv.py first)

| rva | source | match.py (DDK, /O2 /Ob1 /G5) | equiv.py | outcome |
|---|---|---|---|---|
| 0x005a6ea0 | re/match/s25/005a6ea0.c | 22.2% (was 38.9%) | NOT-PROVEN `call-args: ... call 0x5ab040 arg 1: below32(-12) \| below32(-16)` | PROVISIONAL.tsv |
| 0x005a6f30 | re/match/s25/005a6f30.c | 65.8% (was 65.4%) | NOT-PROVEN `call-sequence: region ours +0xe9 / orig 0x005a701c: 2 vs 1 calls` | PROVISIONAL.tsv |

Compiler shaping was not attempted beyond this. Both originals truncate float to int inline, with the sequence
`fnstcw / or ah,0xc / fldcw / fistp / fldcw`:
- 0x005a6ea0: from 0x005a6ee5.
- 0x005a6f30: from 0x005a7056.

The recorded DDK `cl` emits a call to `_ftol` for a C cast. Both residuals are that call.

0x005a6ea0, source change:
- The divide is rewritten to the original's operand types: a zero-extended qword divided by a signed dword (`fild qword` /
  `fidiv dword` at 0x005a6ec7 / 0x005a6ed3). The old text was `(float)uVar3 / (float)uVar2`, an unsigned float divide.
- The DDK now emits `fild qword` / `fidiv dword` / `fmul`, the original arithmetic.
- The score drops because of instruction order, not semantics.

0x005a6f30, source change:
- The body is rewritten by hand from the original disassembly 0x005a6f30..0x005a71e5. The raw g2s text did not compile as
  C++ (`ROUND()`, typed list head) and dropped the `[8]` / `[0xb]` stores after the `goto`.
- Float arguments to 0x005a6dc0 are passed as their dword bit pattern. The original pushes one stack dword with
  `fld dword / fstp dword [esp]`.
- The two truncations use float steps. Under the exe's SSE2 these round where the original rounds at PC_24. That is equal
  for operands below 2^24 and differs only in a double-rounding corner otherwise. This is the reason they are provisional
  and not admitted.

VS2022 code check:
- `PortTop_Bound_005a6ea0` uses `cvtdq2pd`, a `divsd` with the unsigned fix-up on the dividend only, then `mulss` and
  `cvttss2si`.
- `PortTop_Bound_005a6df0` (matched 1-arg source) compiles to a tail `jmp PortTop_Bound_005a6d60`, so the 4 args that
  0x005a6f30 passes reach 0x005a6d60 unchanged. Checked with dumpbin on `mashedmod/build/obj/exe`.

## 2. Closure regenerated (`tools/audiotick_bind_gen.py`)
Generator changes:
- binds `c4=frida-replay` rows (L-128) from their one `re/match/*/<rva>.c`;
- binds provisional rows for the named set `PROV_BIND = {005a6ea0, 005a6f30}` only;
- re-runs regenerate every wrapper of the previous `plan.json`, so a callee that moved from stop to bind is rebound in all
  its callers;
- 0x004671a0 aliases the RW core body (below);
- 0x005ab040 links winmm.lib (`timeGetTime`).

Plan: 132 bind (122 match, 6 replay, 2 admitted, 2 provisional) + 35 reuse + 23 stops. Before this row: 117 + 3 + 30 stops.

Newly bound:
- replay: 0x0045cd30, 0x004647f0, 0x005a6d60, 0x005aab70, 0x005acda0. 0x005aab00 moved from admitted to replay.
- match: 0x0045c720, 0x005ab040, 0x005ab070, 0x005ac650, 0x005ace70, 0x0045cbe0.

New stop: 0x0042b8d0 (C4 hand leaf `StatePhaseIsIdle`, no re/match source).

Remaining stops (23): 0x004075a0 0x0042b8d0 0x0042b900 0x0042b930 0x0045caf0 0x0045d7a0 0x0045dd60 0x0045e990 0x0045efe0
0x00461e90 0x00463640 0x00464ca0 0x0046c7b0 0x0046cbb0 0x00493f70 0x004a34b0 0x004c3ac0 0x005459c0 0x0057c210 0x005a75b0
0x005a9f60 0x005aa560 0x005aac20 0x005b0cf0.

Source edits (C++ compile only; DDK object unchanged, `match.py --bytes` BYTE-IDENTICAL, relocheck OK):

| source | edit | bytes | relocheck |
|---|---|---|---|
| s10/0045c720.c | `(undefined4)(...)` on 6 stores | 196 B BYTE-IDENTICAL | OK checked=14 |
| s10/0045cbe0.c | its copy of 0x0045c4e0 under `#ifndef MATCH_PORT_SHARED_0045c4e0`, its `// MATCH: 0x0045c4e0` line dropped | 0x0045cbe0 105 B BYTE-IDENTICAL | OK checked=6 |

**0x0045cbe0 (forced stop, fixed).**
- The port wrapper defines `MATCH_PORT_SHARED_0045c4e0` and binds the call to the one exe copy,
  `PortTop_Local_0045cb20_0045c4e0`. The DDK compile does not define the macro.
- 0x0045c4e0 stays matched in re/match/s10/0045c4e0.c and 0045cb20.c.
- ddkobj check: 0x0045cbe0 IDENTICAL, 0x0045c4e0 IDENTICAL.

**0x004671a0 (merge collision).** orch/integration (RWC-FRAME-492E90) brought `RwCore/RwCore_004671a0.cpp`, a second
ORIGTOP-only exe copy of the same source as the handoff's `PortTop_004671a0.cpp`. The fix:
- `PortTop_004671a0.cpp` is deleted (rsp line and allowlist line removed);
- callers bind `RwCore_004671a0`.

Merge conflict: `re/tools/dual_copy_allowlist.txt`, resolved as the union. Keys were disjoint.

## 3. Builds and checks
- **VS2022** (`MASHED_NO_DEPLOY=1`): Build OK. rva-lint OK, 278 known, NEW 0, +1 ORIGTOP-ONLY line (0x005ab070 CROSS-TARGET).
  Log `D:/MashedWT/PORT-TOP-AUDIO-RWA_build_vs.log` (8f38a8bf28b8fab1).
- **Object build** (`MASHED_DDKOBJ=1`): Build OK. Log `D:/MashedWT/PORT-TOP-AUDIO-RWA_build_ddk.log` (8284af1f0788d145).
- **`ddkobj.py check`:** IDENTICAL 1311/1345, DIFFERENT 19, not-in-map 15, units linked 1291/1569, 4 refusals
  (`wrapper-redefines-globals`, not this row's). Log `D:/MashedWT/PORT-TOP-AUDIO-RWA_ddkcheck.log` (d071f992adb22c7b).
  - Every newly bound matched body is IDENTICAL: 0x0045c720 0x005ab040 0x005ab070 0x005ac650 0x005ace70 0x0045cbe0
    0x0045c4e0.
  - DIFFERENT, this row's, as expected for non-100% sources: the replay bodies 0x0045cd30 0x004647f0 0x005a6d60 0x005aab70
    0x005acda0 0x005aab00, and the admitted 0x0045c480 0x005a8890.
  - DIFFERENT, not this row's: 0x00401690 0x004332a0 0x0043c5b0 (RwCore, from integration) 0x0045a3a0 0x00475a00
    0x004929d0 0x00493710 0x00493900 0x00498810 0x004c0c20 0x004c0f00.
  - The two provisional units are `skip:provisional` (VS2022 body, no byte check, L-126 (6a)).

## 4. Knob boots
Setup:
- Wrapping: `test-slot` outside (`PROYECTOS_TEST_HUB=off`), `mashed_lock.py run --label PORT-TOP-AUDIO-RWA` inside.
- Root `D:/MashedWT/PORT-TOP-AUDIOTICK-STOPS_root`, the handoff's private root.
- Script `audiotick_stops_arms.py` with `ARMS_STEP=PORT-TOP-AUDIO-RWA`.
- Common knobs: `MASHED_ORIG_TOP=1 MASHED_OT_TIMER=1 MASHED_DET_FRAMES=600`.
- Output `arms_rwa_run2.out`. Run 1 (`arms_rwa_run1.out`) holds the two init arms before the 0x0045cbe0 change: same
  fault, same timing.

| arm | extra knobs | result | log sha256 (16) |
|---|---|---|---|
| vs_audio | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 | 0eaef8a4825c4eb1 |
| ddk_audio | AUDIOTICK | rc 0, 600 frames, OT_TIMER PASS 0/8413, audio branch 600 | 99525cf5e3e9e5ba |
| vs_init_audio | AUDIOINIT + AUDIOTICK | OT_AUDIOINIT PASS 0/12, then 0xC0000005 after 1.8 s | 94368003845379fa |
| ddk_init_audio | AUDIOINIT + AUDIOTICK | OT_AUDIOINIT PASS 0/12, then 0xC0000005 after 2.0 s | a839e6c6fbda58e5 |

Fault site, caught with Frida on the VS2022 exe:
- Scratch copy of `re/frida/catch_standalone_phys_crash.py`.
- Output `D:/MashedWT/PORT-TOP-AUDIO-RWA_data/crash_vs.json`, sha256 cdf6ce0fbc3d54df.
- Symbolized against `mashedmod/build/mashed_re.map`.

Faulting instruction: `mov eax,[esi]` with esi = 0, at `PortTop_Bound_005aab00+0x10`.

Call chain at the fault:
```
PortTop_Bound_005aab00+0x10
<- PortTop_Bound_005a60b0+0x1d (after 0x005a6ea0 and 0x005aad40 returned)
<- PortTop_Bound_0045d3f0+0x25
<- AudioTickAndAvg+0x4a
<- PortTop_Timer_004926c0+0x99
<- Orig_00492290
<- Orig_00492370
<- PortTop_WinMain
```

0x005aab00 dereferences its first argument unconditionally when the second is 0 (re/match/s25/005aab00.c). The argument is
`FUN_005aad40()` = DAT_007dcd20 (re/match/s25/005aad40.c), so it is 0 here.

The ddk arm faults at the same point by timing and exit code. Its EIP was not caught separately.

The menu walk was not run: no menu code changed. No G-CTRL, no B and no captures, because this is a top-down row.

## Next
1. **0x004627f0, sound engine HW init (C2, re/match/s10/004627f0.c).** It has 13 callees, including the RWS engine
   0x005a5f00 / 0x005a6110 and DirectSound 0x005baf40 / 0x005baf60 / 0x005bbb20. hooks.csv:
   "creates RWS engine (0x00632ba0) + DS device (0x0069049c) + output node (0x0068f61c)".
   - Until it runs as a body under `MASHED_OT_AUDIOINIT`, the RW-audio globals (DAT_007dcd20, DAT_007dca34, the 0x007dca18
     list) stay 0, and the both-knob path faults at 0x005aab00.
   - This is an RW-audio bring-up, a DirectSound device on the top-down path. Scope it as its own row, or file a decision
     row if it needs a new device binding.
2. Then re-run `audiotick_stops_arms.py vs_init_audio ddk_init_audio`. The next stop or fault names the next body. The 23
   stops above stay until a run reaches them.
3. The provisional bodies 0x005a6ea0 / 0x005a6f30 need the PROV-INSITU replay (column `replay_prov_insitu` = not run)
   before any admission or C4 talk.

## Files
- `re/match/PROVISIONAL.tsv`: +2 rows.
- `re/match/s25/005a6ea0.c`, `re/match/s25/005a6f30.c`: provisional sources.
- `re/match/s10/0045c720.c`, `re/match/s10/0045cbe0.c`: port-compile edits.
- `mashedmod/src/mashed_re/Match/PortTop_*.cpp`: 13 new wrappers, 28 regenerated (PortTop_AudioStops.cpp among them), `PortTop_004671a0.cpp` deleted.

- `mashedmod/exe_sources.rsp`: +13 TUs, -1.
- `re/tools/dual_copy_allowlist.txt`: +1, -1.
- `verify/porttop_20261108/BINDINGS.tsv`: 28 updated, 7 added.
- Tools under `verify/portorigtop_20261113/tools/`: `audiotick_bind_gen.py`, `sync_rsp_allowlist.py`, `plan.json`,
  `newfiles.txt`, `exeidx.json`.
- `audiotick_stops_arms.py`: `ARMS_STEP` env.

No promotions file: nothing moves to C4 in this row.
