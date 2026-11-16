# RESULT PORT-TOP-AUDIOSTOP-5A61B0: multi-stop row on the audio init lane

Base: orch/integration 7630e53ca. Branch orch/PORT-TOP-AUDIOSTOP-5A61B0. Top-down row: both builds, ddkobj check, knob
boot, self-tests. Gates: not run (top-down row). No menu walk: no menu code changed.

## Stops passed (one line each)

| # | stop | how it was passed | checkpoint |
|---|---|---|---|
| 1 | 0x005a61b0 | provisional body `re/match/s25/005a61b0.c` 71.4% (NOT-PROVEN, tool limit on the jump table), bound | checkpoint 1 |
| 1 | 0x005b35a0 | provisional body `re/match/s27/005b35a0.c` reshaped to the original store order, 55.0% (EQUIV-IF-CALLERS-IGNORE, EDX unknown at the caller), bound | checkpoint 1 |
| 1 | 0x005ae800 | provisional body `re/match/s26/005ae800.c` 38.2% (NOT-PROVEN), bound; no longer a stop. [UNCERTAIN: the arms do not show that it executed, see 1.3] | checkpoint 1 |

Init arms after checkpoint 1: both builds get through 0x005ad770 and stop at **0x005AD910**, the next named stop.

## 1. Checkpoint 1: 0x005a61b0, 0x005b35a0, 0x005ae800

### 1.1 equiv and shaping
equiv.py is the orch/integration copy (no diff on this branch, base 7630e53ca). Verdicts in
`D:/MashedWT/PORT-TOP-AUDIOSTOP-5A61B0_data/equiv_stops1.txt`.

- **0x005a61b0** `NOT-PROVEN switch-index: region ours +0x13 / orig 0x005a61c3`: the index-byte table address is a
  label in ours and `0x5a6233` (table 0x005a6234 - 1) in the original. That is a tool limit on jump-table addressing,
  not a located difference. Shaping: 720 case-group orders (6 groups, return-4 split in two), goto / break / default /
  if-tree forms, the rw9044 toolchain: best 71.4% (unchanged, regs 71.4%, ops 91.4%, 35/35 ins). The residual is block
  order: the original places `mov eax,4; ret` (0x005a6202) right after `cmp eax,0x400; jne 0x5a621c`, the DDK emits it
  last. Only the order of the 1/2 and 4/8 case groups changes the score (65.7% with 4/8 first). Listed provisional.
- **0x005b35a0** old source 60.0%: `NOT-PROVEN ret-memory` (it stored the `*param_1` increment before the head stores,
  a permuter move). The original order (capstone on `MASHED.exe.unpatched`): head stores `mov [esi+4],ecx` at 0x005b35c7
  and `mov [edx],ecx` at 0x005b35ca, then the increment `mov ecx,[eax]` 0x005b35cc / `inc ecx` 0x005b35cf /
  `mov [eax],ecx` 0x005b35d0. New source: increment last. match.py 55.0% (regs 100.0%, ops 100.0%, 20/20 ins): the
  original loads param_1 into eax and param_1+4 into edx (0x005b35a0, 0x005b35aa), the DDK swaps the two and uses eax
  for the increment temp (scratch rotation). 36 source forms tried (struct forms, increment spellings, local orders),
  none moved the register roles. equiv.py: `EQUIV-IF-CALLERS-IGNORE(EAX,EDX,ECX) [REG,VALUE-FLOW] callers 1, scan says
  ignore: 0/1: 0x005ad893:ignores/unknown/ignores`. EDX is unknown because 0x005ad770 returns at 0x005ad8a2 with edx not
  written after the call at 0x005ad893 (capstone listing 0x005ad893..0x005ad8a2), so the value reaches 0x005ad770's own
  callers. Conditional verdict with an unknown caller, so not admitted: listed provisional.
- **0x005ae800** `NOT-PROVEN call-regargs: region ours +0xed / orig 0x005ae8a8 call 0x5aea00`: ecx differs at the
  call (ours `[p+0xc]+[p+4]*[p]`, orig `[p+8]+[p+0xc]+[p+4]*[p]+8`). 0x005aea00 is `mov edx,[0x7d3ff8]` /
  `jmp [edx+0x108]` (0x005aea00 / 0x005aea06). [UNCERTAIN: whether the RwGlobals slot +0x108 reads ecx is not resolved
  by a tool.] match.py 38.2% (123/106 ins, loop-shape residual). Not shaped in this row. Listed provisional.
- `re/match/PROVISIONAL.tsv` +3 rows (status provisional, evidence this RESULT). None of the three is in ADMITTED.tsv.
  None is C4.

### 1.2 Binding
`verify/portorigtop_20261113/tools/audiotick_bind_gen.py`: `PROV_BIND |= {'005a61b0', '005b35a0', '005ae800'}`.
Roots: PORT-TOP-AUDIO-BODY-45D460's (`roots.txt` sha256 73566306...). `exe_body_index.py` re-run (1981 rvas).
- Plan: 259 bind (226 match, 15 admitted, 11 provisional, 7 replay), 64 reuse, 36 stops. Against the previous row's
  plan the only changes are the three rows stop -> bind; no new stop.
- Wrappers that changed: `PortTop_005ad770.cpp`, `PortTop_005ad6f0.cpp`, `PortTop_005ad730.cpp`,
  `PortTop_005adc10.cpp` (0x005a61b0 bound), `PortTop_005ad770.cpp`, `PortTop_005a7920.cpp`, `PortTop_005ad570.cpp`,
  `PortTop_005ae650.cpp` (0x005ae800 bound), `PortTop_005ad770.cpp` (0x005b35a0 bound). New TUs
  `PortTop_005a61b0.cpp`, `PortTop_005b35a0.cpp`, `PortTop_005ae800.cpp`. `PortTop_AudioStops.cpp`: the three stops
  removed. The generator rewrote the other wrappers with LF line endings only; those were restored and not committed.
- `newfiles.txt` +3, `sync_rsp_allowlist.py` (ARMS_STEP=this row): rsp +3, allowlist +2 (ORIGTOP-ONLY DUP-IN-TARGET
  for 0x005ae800 with `Audio/AudioPool.cpp`, 0x005b35a0 with `Audio/AudioList_ag.cpp`). `bindings_update.py`:
  BINDINGS.tsv updated 39, added 0 (the three rows now `near-match`, `PortTop_Bound_<rva>`).
- Note: binding 0x005ae800 also moves its other bound callers (0x005a7920, 0x005ad570, 0x005ae650) off the stop thunk.

### 1.3 Builds, check, arms
- VS2022 (`MASHED_NO_DEPLOY=1`): Build OK. Object build (`MASHED_DDKOBJ=1`): Build OK. rva-lint 322 known, NEW 0.
- `ddkobj.py check`: IDENTICAL 1561/1599, DIFFERENT 22, not-in-map 16, units linked 1549/1764, refusals 11
  (externalized-static-register-convention 7, wrapper-redefines-globals 4), provisional kept VS2022 204 units + 5 fns.
  The three new provisional units keep their VS2022 body in the object build (204 = 201 + 3).
  DIFFERENT = the previous row's 21 + **0x004938e0** (`_PortTop_Movie_004938e0`, `re/match/s15/004938e0.c`, first
  difference +0x8). That unit came in with RWC-FRAME-MOVIE-BACKDROP (bbdea7609, on the base), whose own message reports
  "ddkobj DIFFERENT = base 21 + 0x004938e0 (80% admitted)". This branch does not touch it. Fresh base check: section 1.4.
- Arms (2026-11-16, test-slot after the memory wait, mashed_lock taken by this run and released, ran local):

  | arm | build | stop | return address | log sha256 (16) |
  |---|---|---|---|---|
  | vs_init_audio | VS2022 | **0x005AD910** | 0x0023C0EC = `_PortTop_Bound_005adc10+0x4c` (`mashedmod/build/mashed_re.map`, symbol at 0x0023c0a0) | 99be67eb3af1bf85 |
  | ddk_init_audio | object | **0x005AD910** | 0x0023A09C = `_PortTop_Bound_005adc10+0x4c` (`mashedmod/build/ddkobj/mashed_re_ddk.map`, symbol at 0x0023a050) | aea954aa8f2fbbaf |

  - Stop line in both: `[OT_TIMER] STOP: the matched 0x00000000 called original 0x005AD910`. No stop at 0x005A61B0 any
    more. The last line before it is still `0x004627f0 sound engine HW init: provisional exe body`.
  - 0x005adc10's source calls 0x005ad910 only in its `== 0x2000` arm; the original's one call is at 0x005adc52
    (capstone listing of 0x005adc10..0x005add15). So the loop ran one entry through 0x005ad770, which returned nonzero
    (the zero return exits the loop), then reached a 0x2000 entry. In 0x005ad770's source 0x005a61b0 (first statement)
    and 0x005b35a0 (last call) are unconditional, so both ran. 0x005ae800 is called only when `param_4 == 0`
    (`re/match/s26/005ad770.c`). [UNCERTAIN: the arms do not show whether 0x005ae800 ran; a probe on the bound body or a
    read of the descriptor 0x005add60 passes would settle it.] The +0x4c offset comes from the maps only.
  - **Self-tests PASS in both arms, both builds:** portstate-image PASS (0 of 8 checks failed, witness PortStateSeed
    272/272), OT_CODEPTR PASS (0 of 367 checks failed; 188 slots, 11 bound to an exe body, 177 to a stop thunk),
    code-trap PASS (0 of 6 checks failed, 459/459 pages).

### 1.4 Fresh base check (classifies 0x004938e0)
See section 3 (run after checkpoint 1).

## Arms command
`ARMS_STEP=PORT-TOP-AUDIOSTOP-5A61B0 ARMS_ROOT=D:/MashedWT/PORT-TOP-AUDIOSTOP-5A61B0_root ARMS_EXTRA="MASHED_PORTSTATE_IMAGE=1 MASHED_CODE_TRAP=1 MASHED_OT_CODEPTR=1" test-slot py -3.12 <ORCH>/re/orchestrator/mashed_lock.py run --label PORT-TOP-AUDIOSTOP-5A61B0 -- py -3.12 verify/portorigtop_20261113/audiotick_stops_arms.py vs_init_audio ddk_init_audio`
Private root: robocopy of `D:/MashedWT/PORT-TOP-AUDIOSTOP-5AD770_root`, its old `mashed_re.log` removed.

## Raw data (`D:/MashedWT/PORT-TOP-AUDIOSTOP-5A61B0_data/`, sha256), checkpoint 1
- `vs_init_audio.log` 99be67eb3af1bf85da1903e47f161eb94deeaa29e89cffcedeb168660daca742
- `ddk_init_audio.log` aea954aa8f2fbbaf770bc6bb5e0cbc7fbe7bb86d5ee0c2b537496cd9cdbf7793
- `arms1.out` 5d321e95c7c4d3243ce9b4339d18a38862f3ee26e78d008ded028c6938f3ac44
- `build_vs.log` f41137c6697de382acbbcccb4180c045e08329cd0f5d78346adcf021aff7f7de
- `build_ddk.log` e82fc4bc588d2d4e9adcf1e61b63a7f2aa140f332be72ce0659f84d8b4bfcfa1
- `ddkcheck.log` eb45e6f890d2eaa3e93c0d888af7d28f1936a09a3af48c24954fd78afbdecb30
- `equiv_stops1.txt` 39cb1631c22c148a0b9ab7151c566e72733cc559103dff4432414ee33933966e
- `plan_dry.txt` 28488980416ca37c89e1043694c2785b9f2673d61c5c804451db42a2a4ee67f4
- `plan_write.txt` 7f9de5d087e2f3e7a26fcac676281590176129265f8d72e7d8360bcef16bc3df
- `roots.txt` 73566306c2eb5600447befbb32c4667ff8465d6d18e268986f50ac1fe77142bb
- `005b35a0_before.c` c21a76ca266742e4ffb451a2fc7ee6d7c32e638224d9d7abd13760048fde32a5
