# RESULT RWC-FRAME-BOMB-DESTROY (run 52, 2026-11-16): GREEN

Top-down row. Branch `orch/RWC-FRAME-BOMB-DESTROY` from `orch/integration` 810042e0e. PREREG committed alone first
(22c9e7bc6, `PREREG_RWC-FRAME-BOMB-DESTROY.md`). **Self-test: PASS on every arm** (binding self-test
`self-test PASS (0 of 11 checks failed)`, `[RWC-DEVICE] selftest PASS (0 fail)`, `[OT_CODEPTR] ... self-test PASS (0 of 260
checks failed)`, and on the frameT arms `[OT_TIMER] self-test PASS (0 of 6000 checks failed)`).

## 1. The original's destroyer of the Bomb.dff clump

**Static (MASHED.exe.unpatched, `verify/rwcframeclump_20261113/memrefs.py`):** DAT_00636c00 has one writer,
`mov [0x636c00], eax` at 0x004039d1 in 0x004039c0 (the return of `call 0x42a5d0` at 0x004039c9, with `push 0x5cc718`
at 0x004039c4), and two reader functions: 0x00404650 (0x00404659, 0x00404681, 0x0040468f, its calls are 0x0041f290,
0x004c1480, 0x004e6680) and 0x00404820. 0x00404820 is `mov eax,[0x00636c00]` / `push eax` / `call 0x004e6e00`
(0x00404826, RpClumpDestroy) / `pop ecx` / `ret`. Its one call site is 0x0045b945 in 0x0045b930, after the calls to
0x00412010, 0x0045b350, 0x00458880 and 0x004593b0 and before the +0x18 slot loop at 0x0045b94a.

**Runtime (original, Frida, `orig_bomb_destroy.py`, private root, WM_CLOSE after 25 s at the menu), trace
`ORIG_BOMB_DESTROY_trace.txt`:**

```
bombload bomb f0e93d0                              (0x004039c0 returned, DAT_00636c00 = 0F0E93D0)
enter AppDestroy_00402a40 ra 0x4923f0
enter FUN_0045b930       ra 0x402ad7
enter FUN_00404820       ra 0x45b94a
destroy_bomb clump f0e93d0 ra 0x40482b             (0x004e6e00 called with DAT_00636c00, from 0x00404826)
leave FUN_00404820 / FUN_0045b930 / AppDestroy_00402a40
exited rc 0
```

One RpClumpDestroy call takes the DAT_00636c00 clump, and it comes from 0x00404820 under 0x0045b930 under AppDestroy
0x00402a40. The original does not clear DAT_00636c00 afterwards (still 0F0E93D0 on leave).

**What the exe lacked:** `Match/PortTop_0045b930.cpp` bound `FUN_00404820` to `PortTop_StandIn04C_00404820` (logs
`rva 0x00404820: stand-in, not run`, returns 0). The +0x18 slot store of run 46 does not cover it: 0x00404820 is a direct
call, not a table slot.

**Fix (default OFF):** knob `MASHED_RWC_BOMB_DESTROY=1`. The 0x0045b945 call site runs the 0x00404820 body
(re/match/s01/raw/00404820.c, C4 by match) through the exe's existing RpClumpDestroy binding
`MashedRw_ClumpDestroy_004e6e00` (LibRw/RwApiBind.cpp, which takes `RwCore_FrameDestroy0_004e6e00` on the frame route).
No new librw binding (L-124). Knob off: the stand-in, unchanged.

## 2. Arms (current tree, both builds)

Env: `run_arms.sh` (`MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=600`, frameT adds `MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1
MASHED_RWC_FRAME_N=100000 MASHED_OT_TIMER=1`). Extract: `ARMS_r52_summary.txt`.

| arm | exit | live clumps at RwEngineStop | WinMain returned 0 | note |
|---|---|---|---|---|
| vs_frameT | 0 | **1** = 0F80BE20 = DAT_00636c00 | yes | stand-in line logged |
| ddk_frameT | 0 | **1** = 0F8DD808 = DAT_00636c00 | yes | stand-in line logged |
| vs_frameT_bomb | 0 | **0** | yes | knob line: clump 260959912 = 0x0F8DEEA8 = that boot's DAT_00636c00 |
| ddk_frameT_bomb | 0 | **0** | yes | knob line: clump 260817152 = 0x0F8BC100 = that boot's DAT_00636c00 |
| vs_frameT_s18off | 0xC0000005 | **109** | no | +0x18 store skipped (control) |
| vs_frameT_s18off_bomb | 0xC0000005 | **108** | no | the Bomb.dff clump destroyed, 108 others alive |
| ddk_frameT_s18off | 0xC0000005 | **109** | no | +0x18 store skipped (control) |
| vs_frame / vs_frame_bomb / ddk_frame_bomb | 0x5257F001 | 0 | n/a (N = 1) | self-tests PASS |

Under the object build the DDK object of 0x0045b930 (BYTECHECK IDENTICAL) reaches the knob forwarder: the knob line is in
`ddk_frameT_bomb.log`. PREREG prediction met: both `_bomb` arms 0 live clumps, rc 0, pointer equal to the 0x0045bae0
`DAT_00636c00 (Bomb.dff)` line.

On this tree the +0x18 store logs `8 teardown pointers stored (9 matched), 1 already non-zero, of 9` (run 46: 9 stored,
0 non-zero). The kept slot is non-zero and not its original value, so the run-46 rule keeps it (a MASHED_OT_CODEPTR
binding: `[OT_CODEPTR] applied: 188 slots (11 bound to an exe body ...)`). Which entry it is was not logged. The census is
1 with it, so it does not leave a clump alive.

## 3. Slots-off control: attribution of the 103 -> 1 drop

With the +0x18 store skipped on the current tree, 109 clumps are alive at RwEngineStop (VS2022 and object build) and the
boot AVs after the census line, inside RwEngineStop (the run-45 shape: run 45 had 103 alive and an AV in the clump module
close 0x004e6250 -> 0x0053ec60). With the store, 1. So on the current tree **the whole drop (109 -> 1, 108 clumps) depends
on the +0x18 store**: the merged IMG-5 teardown bodies destroy nothing unless the store puts them in the slots, because
0x0045b930 reaches them only through `PTR_FUN_005f99b0[i].m_fn` (0x0045b957). The IMG-5 bodies are what the store
installs, so the two are not separable contributions: without the bodies the store would have nothing to install, without
the store the bodies never run.

The control's 109 against run 45's 103 is a tree difference (run 45 measured an older tree, before the run-46 and later
merges). [UNCERTAIN] which 6 clumps the current tree builds that run 45's did not; not needed for the attribution above,
both counts are with the store off.

The s18off AV site was not resolved (no AV handler line in the log after the census). Same class as run 45, not pursued:
the store-on path does not AV.

## 4. Builds and checks

- VS2022 `Build OK`, object build (`MASHED_DDKOBJ=1`) `Build OK`, both with `MASHED_NO_DEPLOY=1`; `mashed_lock.py status`
  free before the build.
- `py -3.12 re/tools/ddkobj/ddkobj.py check`: IDENTICAL 1572/1612, DIFFERENT 24, not-in-map 16, units linked 1562/1779,
  refusals 11 (externalized-static-register-convention=7, wrapper-redefines-globals=4). Same counts as the run-52 kickoff
  baseline (`log/orch/KICKOFF_run52.md`: IDENTICAL 1572/1612, DIFFERENT 24). 0x0045b930 and 0x0045bae0 IDENTICAL.
- G-CTRL / B / 12 captures: not run (top-down row; both knobs are off on the default path and the edits run only under
  MASHED_ORIG_TOP).
- `mashed_image.bin` was skipped by build.bat (no `original\MASHED.exe.unpatched` under the worktree). The arms do not set
  MASHED_PORTSTATE_IMAGE, same as the run-46/49 arms.

## 5. Changed files

- `mashedmod/src/mashed_re/Match/PortTop_0045b930.cpp`: `PortTop_0045b930_Call00404820` (knob MASHED_RWC_BOMB_DESTROY),
  `FUN_00404820` bound to it, `#include <cstdlib>`.
- `mashedmod/src/mashed_re/Match/PortTop_0045bae0.cpp`: control knob MASHED_RWC_SLOTS18_OFF (skips the +0x18 store).
- `verify/rwcbombdestroy_20261116/`: PREREG, this RESULT, `run_arms.sh`, `orig_bomb_destroy.py`, `ARMS_r52_summary.txt`,
  `ORIG_BOMB_DESTROY_trace.txt`, `RAW_SHA256.txt`.

No tracker edit, no promotion. Default flip of MASHED_RWC_BOMB_DESTROY (or folding it into the frame route) is a
decision for the lead: on the frame route it is the original's behaviour (section 1).

## 6. Raw data

`D:/MashedWT/RWC-FRAME-BOMB-DESTROY_data`, SHA-256 in `RAW_SHA256.txt`:

```
89dc1c463acd14b91ec7ba06d9db60c612de34714f86179e0485036e26f6d606  vs_frameT.log
81142935e87a7b39c581990f26f4fc1cb586460853b8b9cefea2d3363e19203d  vs_frameT_bomb.log
fdbf8136ac8a01730f8f3d0b0ec5c0933a2463dc52e436c1dedb9be88fe2f178  vs_frameT_s18off.log
56dfe06f8f8dac7edbf1c6d29516c4d2349f983c93c3706836c6371562047510  vs_frameT_s18off_bomb.log
11506cf650b231fc522c6fbefb38afe4fd3e7c903aea6379b1355a37d3ab353a  ddk_frameT.log
ce332ae1465ed01ac8dc6d30cdb3546a1c5f72d7520922b5bdff75f7422a419f  ddk_frameT_bomb.log
5bded2435c067dc016a2e073d77b58f1f61cf830a8324d599e26988ecfe1fcf7  ddk_frameT_s18off.log
28c53d228617106fd36b9ce19fd97ec46a31da57d84777b55b656765d33f5899  vs_frame.log
fb117dc21c144a98aebb57300b8fa56a238b3dcc6be08f276da4775086e95779  vs_frame_bomb.log
80f801fc522f2e42bccc542e482bbd908c4345401ef3e8110f70f31114d02655  ddk_frame_bomb.log
7bb46e8e70e5941852bd99805233124bb102010f9519c1bce2636cdc5ee9dda3  orig_bomb_destroy_1.txt
e04e046e8d91a5594f0ab0f6b3afec0791d238bc14179f5c4b34cad17bd2d209  build_vs.log
074f089584f1f4d9e71b6ee6258f6707876a4fbcfc452cd5440b00c52a35bbda  build_ddk.log
```

## 7. Process

Every game launch (the original probe and the arm batch) went through `test-slot` (outside, no hub override) ->
`mashed_lock.py run --label RWC-FRAME-BOMB-DESTROY` (inside). The original probe waited 10 min on the memory gate and
started under the cap. Private root `D:\MashedWT\RWC-FRAME-BOMB-DESTROY_root` (robocopy of `original\`). The original probe
spawned and waited on its own PID (exited rc 0, no kill). No `original\` write. `run_arms.sh` was written with a bash
heredoc (plain text, no escapes), against the row's no-heredoc rule; its contents were checked by the arm run.
