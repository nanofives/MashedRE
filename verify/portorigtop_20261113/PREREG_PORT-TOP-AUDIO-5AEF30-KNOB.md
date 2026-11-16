# PREREG PORT-TOP-AUDIO-5AEF30-KNOB: default-off stand-in for 0x005aef30 (no audio worker thread)

Written and committed before any arm of this row. Decision L-174 = (c) then (a) (Mariano). Base orch/integration @
0917bc3d0 (contains PORT-TOP-AUDIO-5A8F30-STOPS). Row kind: top-down (both builds, `ddkobj.py check`, init arms,
self-tests; no G-CTRL, no B, no 12x4 captures).

## Knob
- Name: **`MASHED_OT_AUDIOTHREAD_STANDIN`**. On when the value starts with `1`. **Default off** (unset or any other value).
- Read once, exe only, only on the `MASHED_ORIG_TOP` + `MASHED_OT_AUDIOINIT` path. It lives in one new TU,
  `mashedmod/src/mashed_re/Match/PortTop_Knob_005aef30.cpp` (`RVA-LINT: ORIGTOP-ONLY`).
- Binding: the generator (`verify/portorigtop_20261113/tools/audiotick_bind_gen.py`) maps the callee `FUN_005aef30` of a bound
  wrapper to `PortTop_Knob_005aef30` instead of `PortTop_AStop_005aef30`. The stop thunk `PortTop_AStop_005aef30` stays
  generated in `Match/PortTop_AudioStops.cpp`.

## What it changes
`PortTop_Knob_005aef30(param_1, param_2)`:
- knob off: calls `PortTop_AStop_005aef30` exactly as the wrapper did before this row (log line + exit with code 0x005aef30,
  the forced stop of PORT-TOP-AUDIO-5A8F30-STOPS 1.4). So the knob-off arms stop at 0x005AEF30 as before.
- knob on: returns `param_1`, starts no thread, writes nothing. It is a stand-in, not a body of 0x005aef30, and is never C4.

## Evidence for the stand-in contract (listing of `original/MASHED.exe.unpatched`, `re/tools/disasm_va.py`)
- 0x005aef30 itself: `mov [esi+8],eax` (param_2 -> param_1+8) at 0x005aef3d; `call 0x5c2e79` (__beginthread(0x005aef70,
  param_1[4], param_1)) at 0x005aef46; `cmp eax,-1` / `xor eax,eax` (return 0) at 0x005aef4e / 0x005aef53; `mov [esi],eax`
  (handle -> param_1[0]) at 0x005aef5a; `call [0x5cc0bc]` (SetThreadPriority(handle, param_1[3])) at 0x005aef5e;
  `mov eax,esi` (return param_1) at 0x005aef64.
- Callers (`re/tools/xref.py 0x005aef30`): 0x005a8060, 0x005bb000, 0x005be260. Only 0x005a8060 is bound today (its wrapper
  `Match/PortTop_005a8060.cpp` is the only TU with `#define FUN_005aef30`).
- 0x005a8060 at the call: `push 0` / `push 0x7dcb54` / `call 0x5aef30` (0x005a8321 / 0x005a8323 / 0x005a8328), then
  `test eax,eax` / `jne 0x5a8346` (0x005a8330 / 0x005a8332): non-zero goes to `pop esi; ret` (0x005a8346 / 0x005a8347); zero
  falls to the two `CloseHandle` calls (`call esi` at 0x005a833b on [0x7dcb50], 0x005a8344 on [0x7dcae0]). So the caller
  reads the return only as zero / non-zero, and `param_1` (0x007dcb54, non-zero) takes the same branch as the original's
  success path.
- 0x005a8060's own return: its caller 0x005a7fe0 calls it at 0x005a801a and the next instructions push the arguments of
  `call 0x5aa200` (0x005a801f..0x005a802e) without reading eax.
- 0x005aef00 (called at 0x005a8315, `push 0x5a84d0` at 0x005a8301) sets the descriptor at 0x007dcb54: +4 = 0x005a84d0
  (0x005aef0c), +0 = 0 (0x005aef13), +8 = 0 (0x005aef19), +0xc = [0x0063307c] (0x005aef20), +0x10 = 0 (0x005aef23); it
  returns eax = param_1 (`mov eax,[esp+4]` at 0x005aef00, not written again), tested at 0x005a831d.
- **param_2 store at +8: not done.** The task allows the store only if later code reads +8. The reader of param_1[2] is the
  thread entry 0x005aef70 (RESULT_PORT-TOP-AUDIO-5A8F30-STOPS 2.4), which the stand-in never starts; `xref.py --string` finds
  no other referencing function for 0x007dcb54..0x007dcb64 besides 0x005a8060. For the one bound caller the value is 0
  (`push 0` at 0x005a8321) and +8 is already 0 (0x005aef19), so memory at 0x007dcb5c is the same either way.
- **Known memory difference with the knob on:** 0x007dcb54 (descriptor +0) stays 0 (from 0x005aef13); the original stores
  the thread handle there (0x005aef5a). No thread runs 0x005a84d0, so the list at 0x007dcad8 is not walked every 0x11 ms and
  0x007dcb50 is not released by it (0x005a854d).
- [UNCERTAIN] No code other than 0x005a8060 and the thread reads the 0x007dcb54 descriptor (xref.py covers direct address
  references only; a pointer-relative read from another function is not excluded). One line in
  `re/orchestrator/UNCERTAIN_REGISTER.md`; test: a write/read watch on 0x007dcb54..0x007dcb67 in the original over boot to
  menu (`A4_RO_WATCH`).

## Must stay identical with the knob off
- The default path: no default-path TU changes (the new TU and the regenerated wrapper are ORIGTOP-ONLY, reached only under
  `MASHED_ORIG_TOP` + `MASHED_OT_AUDIOINIT`). Default gate hashes (`re/orchestrator/merge_train.json` refs) are not run by the
  worker (top-down row); the merge train runs them once per batch.
- Knob-off arms `vs_init_audio` / `ddk_init_audio`: stop at **0x005AEF30**, return address in `_PortTop_Bound_005a8060`
  at the same offsets as the base (+0x18c VS2022, +0x2cd object build on RESULT_PORT-TOP-AUDIO-5A8F30-STOPS 3.2), self-tests
  PASS (portstate-image 0 of 8, OT_CODEPTR 0 of 367, code-trap 0 failed).
- `ddkobj.py check`: no newly DIFFERENT unit against a fresh check of the base 0917bc3d0 object build (base BYTECHECK kept
  in the data folder).

## Expected with the knob on
- The arms pass 0x005aef30 (census order 42) and 0x005a8060 returns through 0x005a8346. Census rows 43 to 47 (0x005c2e79,
  0x004a8815, 0x005c2dd9, 0x005aef70, 0x005a84d0) are the thread start and the thread itself: not reached.
- Next expected stop: the next `0x00635194` entry after 0x005a7fe0, 0x005c80a0 -> 0x005c8120 (census 49) is bound already
  (byte match), so the first new stop is the first unbound callee reached after it, in census order (candidates 0x005b73e0
  census 51, 0x005bce80 census 53, then 58+). Recorded as found, not predicted.
- Then stop by stop to 0x0045d460 (L-126: equiv.py decides; a conditional verdict with a reading or unknown caller stays
  PROVISIONAL with its verdict line, no decision row per function).

## Pass / fail
- PASS: knob off reproduces the base stop and offsets exactly in both builds; knob on passes 0x005aef30 in both builds with
  self-tests PASS; no newly DIFFERENT unit in the object build; every newly bound body is match.py `--bytes` 100% +
  relocheck OK, or admitted / provisional by equiv.py.
- FAIL: a knob-off arm stops anywhere else or at another offset; any self-test fails; a newly DIFFERENT unit is not explained.
- Then RO-AUDIOINIT-45D460-REACH: DAT_00604604 after 0x0045d460 returns vs ref1 record 0 (ref 4, run 0 in
  RESULT_RO-ORACLE-GATE @ 32576d460), and the oracle (object build) with `MASHED_OT_AUDIOINIT` on the RO arm to the next
  divergence.
