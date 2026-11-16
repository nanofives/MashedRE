# RESULT RO-67F0B8-CALLER (run 50)

Row: replay pair NEW block from PORT-TOP-BODY-43DFD0 run 49 (`verify/porttopbody43dfd0_20261114/RESULT_PORT-TOP-BODY-43DFD0.md`
on `orch/PORT-TOP-BODY-43DFD0` @ 6f9a43106): the original writes 0x0067f0b8 = 1 at tick 16 from 0x0042e6bf inside
0x0042e5b0, and the exe never writes it before the warp. This row names the original's caller chain at that tick and the
point where the knob-path exe leaves it. No build and no gates: one original-side run plus reading code and logs.

## Verdict

GREEN on the measured leg. The original's call chain at the tick-16 write is measured (raw stack, every link checked as a
direct CALL whose target contains the next frame's return address). On the exe, run 49's replay arms took the hand
branch of the 0x00492e90 stand-in on every logged frame (runtime log). The claim that the hand branch never writes
0x0067f0b8 rests on reading code plus run 49's write watch (0 writes before the warp), and stays [UNCERTAIN] with its
test below.

## 1. Run

- Tool change (this branch): `re/frida/detorig.py --watch-stack N` and `re/frida/js/detorig.js`. On each watch hit,
  besides the ACCURATE backtrace, the agent sends esp, ebp, a FUZZY backtrace and N raw dwords from esp. Off by default
  (N = 0), so earlier recipes are unchanged.
- Command (`verify/ro67f0b8caller_20261115/run.sh og1ws50`, under `test-slot`, `mashed_lock.py run` inside): the og1
  recipe as in PORT-TOP-BODY-43DFD0 run 49 leg `origi` (`oraclegate/og_arms.sh orig`, root `D:\MashedWT\RO-ORACLE-GATE_root`,
  `--clock tick --boot --q0 0x40000000 --tick-cap 600 --warp-tick 300 --warp-clock 300 --normalize re/frida/warp_normalize.tsv
  --load-sleep-ms 0 --ckpt-ticks 0,1,16,17,18,299,301,600`) with `--watch 0x0067f0b8 --watch-from install --watch-stack 256`.
- Outcome: rc 0, 600 tick records, 23 checkpoints, warp at N=3543, race anchor N=3654. **One watch hit: tick 16 (rec 16),
  value 0x1, pc 0x0042e6c5, main thread (tid 17948)**. Same tick, value and pc as run 49's og1wi49 (`n` 423 here, 411
  there). No piz-collision crash (U-9677).
- Binary run: `D:\MashedWT\RO-ORACLE-GATE_root\original\MASHED.exe`, SHA-256 b9977dab...fe87 (the patched install, same
  hash as `original\MASHED.exe`). Return addresses were decoded with capstone over `original\MASHED.exe.unpatched`
  (SHA-256 bdcae093...3c0e, the version anchor). None of the call sites below lies in a boot-patch cluster (CLAUDE.md
  list: 0x00498dbc, 0x005bc750, 0x004951f0, 0x004951aa, 0x00498bc0/bd0, 0x004963e7/6400/6490, 0x004a4541, 0x00495870,
  0x0040283f, 0x004996d3).

## 2. Caller chain of 0x0042e5b0 at tick 16 (original, measured)

`resolve_stack.py` keeps a stack dword only when the bytes before it decode as a CALL that ends at it. For a direct
CALL it checks that the call target contains the previous frame (target <= previous return < next hooks.csv function
start). Output: `og1ws50_chain.txt`. The FUZZY backtrace from Frida gives the same return addresses in the same order.

| Frame | Function | Return address (stack slot) | Call instruction | Check |
|---|---|---|---|---|
| 0 | 0x0042e5b0 | pc 0x0042e6c5 (after `0x0042e6bf mov [0x67f0b8], edx`) | | watch hit |
| 1 | 0x0043c5b0 | 0x0043c678 ([esp+0x06c]) | `call 0x42e5b0` at 0x0043c673 | chain OK |
| 2 | 0x00492e90 | 0x00493008 ([esp+0x0bc]) | `call 0x43c5b0` at 0x00493003 | chain OK |
| 3 | 0x00492290 | 0x004922bd ([esp+0x0dc]) | `call 0x492e90` at 0x004922b8 | chain OK |
| 4 | 0x00492370 | 0x004923de ([esp+0x0e0]) | `call 0x492290` at 0x004923d9 | chain OK |
| 5 | 0x004a4bb7 | 0x004a4d38 ([esp+0x0e8]) | `call 0x492370` at 0x004a4d33 | chain OK |

Two other dwords decode as returns but fail the containment check. They are stale slots from calls that already
returned: 0x004c2bde ([esp+0xa0], `call 0x45b350` at 0x004c2bd9 in 0x004c2bb0) and 0x00492ffc ([esp+0xac],
`call 0x4c1a00` at 0x00492ff7 in 0x00492e90). The second one is the `FUN_004c1a00(FUN_004671a0(-1))` test that
`re/match/s15/00492e90.c:129` places just before the `FUN_0043c5b0()` call at `:130`. ebp was 0x0 at the hit, so
there is no ebp chain. The ACCURATE backtrace stops after one frame (`0x134fd160|0x0042e6c5`, 0x134fd160 lies outside
the image), as it did in run 49.

Static cross-check (byte scan of `MASHED.exe.unpatched` for E8 rel32 and absolute 32-bit references):
- 0x0042e5b0 has one call site (0x0043c673) and no absolute reference.
- 0x0043c5b0 has three call sites, all in 0x00492e90: 0x00493003, 0x00493281 and 0x004932bb. The measured one is
  0x00493003.

Gate words at the checkpoints (`ckpt20N.data.bin` = .data image from 0x005ea000, read at tick 1 / 16 / 17):

| Tick | DAT_00771968 | DAT_0067eca4 | DAT_0067f0b8 | DAT_0067ecb0 |
|---|---|---|---|---|
| 1 | 0x1 | 0x1 | 0x0 | 0x21 |
| 16 | 0x1 | 0x1 | 0x0 | 0x21 |
| 17 | 0x1 | 0x2 | 0x1 | 0x21 |

DAT_00771968 = 1 at ticks 16 and 17 matches the `case 1:` arm of `re/match/s15/00492e90.c:118`. That arm holds the only
`FUN_0043c5b0()` call before the `case 2:` label (call at `:130`, label at `:152`). 0x0043c5b0 calls 0x0042e5b0 only when 2 <= DAT_0067eca4 < 5
(`re/match/s08/0043c5b0.c:106-118`). DAT_0067eca4 reads 1 at the tick-16 checkpoint and 2 at tick 17, so it reached 2
during tick 16, before the write. [UNCERTAIN] which instruction writes DAT_0067eca4 = 2 during tick 16, and whether that
happens in the same frame as the write: no watch was set on 0x0067eca4. Test: row (b) below.

## 3. Why the knob-path exe does not reach it

The exe's replay arms (PORT-TOP-BODY-43DFD0 run 49: `RO_ROIMG-b43off` and `RO_B43IMG-b43on`, object build, stack ROIMG =
`MASHED_PORTSTATE_IMAGE=1` plus the `run_arms.py` RO env `MASHED_ORIG_TOP=1 MASHED_DETERMINISTIC=1 MASHED_REPLAY=...`)
run the same top three levels from matched sources:
- 0x00492370: `PortTop_00492370.cpp:535`, `#define FUN_00492370 Orig_00492370`, source `re/match/s15/00492370.c` (calls
  `FUN_00492290` at `:63`).
- 0x00492290: `PortTop_00492370.cpp:536`, `#define FUN_00492290 Orig_00492290`, source `re/match/s15/00492290.c` (calls
  `FUN_00492e90` at `:42`).

The chain leaves the original's path at frame 2. In that TU `FUN_00492e90` is `#define`d to `PortTop_StandIn_00492e90`
(`PortTop_00492370.cpp:558`). The stand-in (`:307-321`) runs the matched 0x00492e90 (`RwCore_00492e90`, which calls
`RwCore_0043c5b0` per `RwCore/RwCore_00492e90.cpp:27`) only when `RwCore_FrameRoute()` is true. That function
(`RwCore/RwCore_Knob.cpp:68-76`) requires `RwCore_Enabled()` and `MASHED_RWC_FRAME` set and non-zero. Otherwise the
stand-in runs `PortTop_HandRenderFrame()`.

Runtime evidence (raw logs from run 49):

| Arm | Lines `[ORIG_TOP] frame N rendered (1)` (hand branch, `:317`) | Lines `rendered by 0x00492e90 (matched, RW core)` (`:312`) |
|---|---|---|
| RO_ROIMG-b43off | frames 0, 120, 240, 360, 480 | 0 |
| RO_B43IMG-b43on | frames 0, 120, 240, 360, 480 | 0 |
| vs_frameN49 / vs_frameNT49 / ddk_frameNT49 (frame route, `MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1`) | n/a | 1 each |

So on the replay route the missing call is **frame 2 → 1: 0x00492e90's `call 0x0043c5b0` at 0x00493003**. The exe's
stand-in for 0x00492e90 is the hand RenderFrame there, because the RO env does not set `MASHED_RWC_CORE` /
`MASHED_RWC_FRAME`. The frame route does take the matched 0x00492e90. Run 49 measured `DAT_0067eca4` reaching 3 on that
route with `MASHED_OT_TIMER=1`, so 0x0043c5b0 can call 0x0042e5b0 there. But the frame route is not a replay arm: it
has no tick trace and no compare against og1.

- [UNCERTAIN] the hand branch (`PortTop_HandRenderFrame` and the hand menu code it drives in `exe_main.cpp`) never writes
  0x0067f0b8. This rests on reading code: the `exe_main.cpp` F2 crossfade port (`:628`, `:5506`) keeps its own state.
  It also matches run 49's write watch on both replay arms (`MASHED_REPLAY_TRACE_WATCH=0x0067f0b8`: 2 hits, both at
  tick 300, from `PinNorm+0x71`, the warp normalisation), but that watch only covers that one dword. Test: row (a).
- [UNCERTAIN] a replay arm on the frame route reproduces the original's tick-16 write. This rests on reading the
  sources only. Test: row (c).

## 4. Open tests (each its own row)

| Row | Claim under test | Test |
|---|---|---|
| (a) | the hand branch never writes 0x0067f0b8 | `MASHED_REPLAY_TRACE_WATCH=0x0067f000` (or the 0x0067f0b8 dword plus `MASHED_REPLAY_TRACE_WATCH_FROM` at boot) on RO_ROIMG, 0 writes before the warp |
| (b) | which write moves DAT_0067eca4 from 1 to 2 during tick 16, and whether it comes before the 0x0042e5b0 call in the same frame | `run.sh` with `--watch 0x0067f0b8,0x0067eca4 --watch-from install --watch-stack 256`, compare the `n` order of the two hits at tick 16 |
| (c) | a replay arm on the frame route writes 0x0067f0b8 = 1 at tick 16 | an RO arm with `MASHED_RWC_CORE=1 MASHED_RWC_FRAME=1` (the frame route has `MASHED_RWC_FRAME_N` presents as its end, so the arm needs that cap past 600 ticks), watch 0x0067f0b8, compare with og1 by `detorig_ticks.py` |

## 5. Files

Committed (`verify/ro67f0b8caller_20261115/`): `run.sh` (the run), `resolve_stack.py` (offline resolver),
`og1ws50_watch.jsonl` (the one hit, 2145 bytes), `og1ws50_chain.txt` (resolver output), this RESULT. Tool change:
`re/frida/detorig.py`, `re/frida/js/detorig.js` (`--watch-stack`).

Raw data (`D:/MashedWT/RO-67F0B8-CALLER_data`):

| File | SHA-256 |
|---|---|
| og1ws50.log | 8b05ec7b84eaa74730a984e6e33046eef2f5643983d181a93e26b455ae9ccfa5 |
| og1ws50/watch.jsonl | 5bc7175850c194179abf8c1ce284daa3c624e6349a960003260515e21428ce4d |
| og1ws50/meta.json | 0095078b98fe8bc1bf6649fdf065cde515a438924385f19b57caecda736576b7 |
| og1ws50/ckpt202.data.bin (tick 16) | d2377bed35c998c1891b45bb9f7108ba07334a597e3be15003656770061c903f |
| og1ws50/ckpt203.data.bin (tick 17) | adade77e5a1d6803c3f5f9ae7ccf9b1b4f8b6e5ffcae9c242a3ccfa50a90085b |
| og1ws50/ticks.bin | 9a68280f8cb9bee359d7eb5585a1c538d9fe313ffc8885d9e86731c17c13defe |

Cited from run 49 (`D:/MashedWT/PORT-TOP-BODY-43DFD0_data/r49`):

| File | SHA-256 |
|---|---|
| RO_ROIMG-b43off.log | 2c82a24b1fd32f80a10aaeb8018eb4f4ae7fe0b48c68c482b79f2644cff0c5c2 |
| RO_B43IMG-b43on.log | dc9b1d1f32e54dfd7dfe9eb2c4a40deb9a89ecdf1033552b03ae6b35d995c27f |
