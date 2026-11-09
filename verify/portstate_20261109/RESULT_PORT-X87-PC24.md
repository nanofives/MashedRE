# RESULT PORT-X87-PC24: the game thread at x87 PC_24 from device creation on (MASHED_X87_PC24)

Verdict: **DECISION.** The knob is in, default OFF, default gates unchanged. Under the PREREG's scoring rule the knob-ON
build is **equivalent** to the default on every race scenario and **closer** on the one row I could score against the
original's own instruction stream. The flip is the open item, and it would move no gate reference.

Mariano's decision L-103 (a). Lead cmw7xu1960f37p41557tefpnd. PREREG `89e392644` (`PREREG_X87PC24.md`, committed alone),
amendment 1 `2068dd0db` (committed alone, before any arm). Knob code `1df613f45`. Exe `55e5c513` (`MASHED_NO_DEPLOY=1`).
Background: `RESULT_PORT-X87-PC24-AUDIT.md` (`orch/PORT-X87-PC24-AUDIT` `dc8b11eb8`, not yet on `orch/integration`).

## 1. Answer

1. **The knob does what the original does, on the right thread, at the right moment.** `ApplyX87Pc24AfterDevice()`
   (`exe_main.cpp`, called on both success returns of `InitD3D9`) sets `_PC_24` on the game thread right after
   `CreateDeviceWatched` returns. Arm log `B_X24LCW_K0.log`: `[x87] MASHED_X87_PC24: tid=21440 pc 0x10000 -> 0x20000`,
   then `[cw] frame=0 pc=0x20000 tid=21440 knob=1 drift=0` and the same at frame 600. The knob thread is the `PumpOnce`
   thread. 0 drift events.
2. **S1 holds, the required check (CW per row).** On the G-CTRL Training recipe, every row the exe reaches after the
   device now runs at 0x007f, as in the original. The pre-device config rows stay 0x027f, as in the original.
   `X87PC24_CW_EXE_OFF.tsv` / `X87PC24_CW_EXE_ON.tsv` (audit tool `x87_cw_probe.py`, same 34 stubs as the audit):

   | RVA | original | exe OFF | exe ON |
   |---|---|---|---|
   | 0x0046c5f0 TriangleFaceNormal | 0x007f | 0x027f x912 | **0x007f** x948 |
   | 0x00441b30 / 0x00441c80 / 0x00442600 / 0x00445aa0 CameraEntries | 0x007f | 0x027f | **0x007f** |
   | 0x004c3b30 FastSqrt | 0x007f | 0x027f x876 | **0x007f** x876 |
   | 0x00402f50 / 0x00407a60 / 0x00409290 / 0x00414060 / 0x0041d910 / 0x00458fa0 | 0x007f | 0x027f | **0x007f** |
   | 0x0040e470 / 0x00442cc0 / 0x00458920 | 0x007f | 0x007f (local scope) | 0x007f |
   | 0x00498910 / 0x00498950 Config | 0x027f (pre-device) | 0x027f | 0x027f |
   | 0x00496400 ConfigLogDebug | attach fail | 0x027f | 0x027f |

   0x00402f50 was `[UNCERTAIN]` in the PREREG: the exe calls it after `InitD3D9`, so it reads 0x007f under ON. Resolved.
3. **No gate hash moves.** P2 is **falsified**: G-CTRL and B are byte-identical with the knob ON.

   | | G-CTRL step / gates / ptrace | B K0 / K1 / K26 |
   |---|---|---|
   | ref (`merge_train.json`) | 8284be46 / 67931834 / 31fe043e | 1d5bf4f9 x3 |
   | knob OFF (H_DEF, B_DEF_*) | 8284be46 / 67931834 / 31fe043e | 1d5bf4f9 x3 |
   | knob ON (H_X24, H_X24-2, B_X24_*) | **8284be46 / 67931834 / 31fe043e** | **1d5bf4f9 x3** |
   | Warzone W_DEF / W_X24 / W_X24-2 (step / pontoon / seek) | eeb92153 / a329ca22 / fa8618df, all three equal | |

   Old = new: the flip needs no new G-CTRL or B reference.
4. **But the knob does change values, and toward the original.** Per-call value probe (`x87pc24_values.py`, the same
   deterministic 1200-frame Training run OFF and ON, inputs identical call by call):
   - **0x0046c5f0 TriangleFaceNormal:** inputs identical on 452/452 calls, outputs differ on **80/452**. Scored against
     a bit-exact replay of the **original's** instruction stream 0x0046c5f0..0x0046c6c1 (`x87pc24_trinormal_model.py`,
     PC_24 = float32 per op, LUT sqrt from the fitted RW table): the original at its own CW (0x007f) reproduces
     **knob ON on 452/452** calls and knob OFF on 372/452. (The original at PC_53 reproduces OFF on only 205/452, so OFF
     is not "the original at PC_53" either: the exe's VS2022 x87 codegen at PC_53 is a third form.) Register map
     resolved by the data: EAX = apex, EDX = p2, ECX = p1, the order `MatchPilot_0046c5f0.cpp` passes.
     The 80 deltas land in the record words `(ring*3 + 0x2cb)*4` (`ForceIntegrator.cpp:367`). No hashed field of G-CTRL,
     B, Warzone or D2 changed, so within these scenarios nothing reads them in a way that reaches a hash. Mechanical
     statement only: which reader consumes those words is not traced here.
   - **0x00445aa0 CamEntryType0Step445aa0:** the camera entry differs after **876/876** calls, in fields +0x3c, +0x40,
     +0x44, +0x68, +0x7c (integer deltas of the float bits up to 122, 21, 12, 512, 122: accumulated rounding). Not
     scored against the original: no original camera-state capture exists for this recipe. These CameraEntries TUs are
     x87 transcriptions whose bit-identity was measured in the `.asi`, which runs at MASHED.exe's PC_24 (memory
     asm-bit-identity-in-asi-not-standalone-pc24). The knob gives the exe the precision they were verified at.
   - 0x00441c80 CamCarMidpoint (1638 calls) and 0x00441b30 CamEntryBuild (7 calls): identical OFF / ON.
5. **D2 (S2): same.** All 6 arms in the O42 band (`X87PC24_D2_SCORE.txt`, interleaved DEF / ON, solo):

   | arm | slip1500 | slip2000 | drvmed |
   |---|---|---|---|
   | D2_DEFH_1 / 2 / 3 | 0.1935 / 0.1930 / 0.1935 | 0.2514 / 0.2515 / 0.2515 | 1928.35 / 1917.10 / 1921.42 |
   | D2_X24H_1 / 2 / 3 | 0.1933 / 0.1935 / 0.1935 | 0.2515 / 0.2515 / 0.2515 | 1919.62 / 1921.98 / 1921.98 |

   d(DEF) = mean |drvmed - 1964.41| = 42.12, d(ON) = 43.22. Noise n = max(half-range DEF 5.63, ON 1.18) = 5.63.
   |d(ON) - d(DEF)| = 1.10 < n, so **same**. The DEF arms' spread is wall-clock (memory d2-arms-are-wall-clock-bound);
   two ON arms produced the byte-identical log `f9fdd52f`.
6. **B (S3): same.** B_X24 hashes = B_DEF hashes, so the per-car envelope (9/9 PASS) and (e) 3/3 with launch
   `1425.7 / 2052.5 / 2055.0` are the default's own values (merge-train run `20261109_080025`).
7. **13 tracks (S4): same.** `run13.py run ... MASHED_X87_PC24=1`, all 13 OK on the first attempt. Scored table
   **identical** to `verify/run13p3_20261029/tracks_score_sub_p3.txt` (the whole score file has the same SHA-256 prefix,
   `2431afb1`), INSTRUMENT GATES PASS.
8. **Verdict by the PREREG rule (section 5): equivalent.** S1 holds, every required check holds, S2 and S4 are same.
   The rule then says: propose the flip on fidelity grounds, flagged as a judgment call. Item 4 adds per-row evidence
   the rule does not score: on the one row with an original-side reference, ON matches the original on every call and
   OFF does not.

## 2. Threads that run game code

Static census (`CreateThread` / `_beginthread*` / `std::thread` / timer-queue grep over `mashedmod/src` and
`mashedmod/deps`) and the arm log:
1. **The game thread** (WinMain, the window procedure via `DispatchMessageA`, sim, AI, physics, render, the audio voice
   model). The only thread that needs the knob, and the one it runs on (tid equality in item 1.1).
2. **The `CreateDeviceWatched` worker** (`exe_main.cpp:3186`): runs only `IDirect3D9::CreateDevice`, already PC_24 from
   D3D9, then exits.
3. **The ShadowTrack watchdog** (`Core/ShadowTrack.h:181`): only with `MASHED_SHADOW_TRACE`, diagnostics, no game float
   code.
The exe has no audio worker thread. The original starts one (the call at 0x005a8328 into FUN_005aef30, listed in
`AudioDSound.cpp:237`); its CW was not measured here. DirectSound / DInput / DirectShow internal threads run no
game code. Side note: `RenderFrame`'s `g_device->Reset` (`exe_main.cpp`, device-lost path) runs on the game thread, so a
device reset already sets PC_24 there today, knob or not.

## 3. Interaction with the per-row precision knobs

`MASHED_ROTATE_PC24`, `MASHED_ROTATE_LUT`, `MASHED_ORTHO_LUT` and `MASHED_WCS_COMBINE_PC24` no longer exist: their ON
paths became unconditional in D5-KNOBS-E3-GEOM-AI / E2. Arms on G-CTRL (H), B K0 and Warzone (W), each knob with the
global knob OFF (`*0`) and ON (`X24*`):

| knob | H OFF / ON | B K0 OFF / ON | W OFF / ON | under global PC_24 |
|---|---|---|---|---|
| `MASHED_MATMUL_PC24=1` (0x005cb2a0 scope, default OFF) | = DEF / = X24 | = / = | = / = | **redundant by construction** (inline x87 `__asm` at the thread's PC, `RwMatrixMultiplyCpu.cpp:67`). Not exercised by any of the three scenarios even with the global knob OFF, so no arm can show more |
| `MASHED_NO_CC_PC24=1` (opts out of the seeded `CC_PC24` body) | 2bb28855/e1a6a325/26a01b06 both | b07b0855 both | 8a73ed74 both | **still needed**: a body switch (LUT + float rounding, SSE2 TU), not a precision scope (amendment 1). Its effect is the same with and without the global knob |
| `MASHED_NO_COLL_LUT=1` | = DEF both | = both | b9f47b5b / 2da4ae07 (step / seek) both | **still needed**: LUT vs `std::sqrt` is an algorithm. Same effect at both PCs. PREREG predicted H != X24: falsified on H (not exercised there), held on W |
| `MASHED_NO_LEAVES1_X87=1` | = DEF both | = both | = both | **not exercised** by H, B or W at either PC. No claim |
| PickupTick PC_53 scopes (`MASHED_X87_PC24_PK53=1` keeps them) | = X24 | | = X24 | **conflicting by construction, inert here.** Under ON the two callbacks now follow the thread baseline (`Core/X87Pc.h` `ThreadBaselinePc()`) |
| local PC_24 scopes (RwMatrixRotate, BodyOrientationIntegrate, RwV3dNormalize, PathNodes, PathProgStep, WorldRegistry, WarzonePontoon, AiTargetClosest `MASHED_TGT3_PC`, `MASHED_MODE7_PC`, AiLeafTargets, CollisionWorld, `MeasureMenuMessage`) | | | | **redundant by construction** (set PC_24 on PC_24, restore to PC_24). Keep them: they also cover the `.asi` and the OFF default |
| `MASHED_A5_DRAFT_LUT`, `MASHED_A6B_NORM_LUT`, `MASHED_NO_A3_LUT`, AI LUT mask 7 | | | | still needed (algorithm, same class as COLL_LUT). No arm |
| `MASHED_A6A_LUT`, `MASHED_A6A_DRIVE_NORM_LUT`, `MASHED_A5_SUSP_LUT`, `MASHED_RW_LUT_ALL` (default OFF) | | | | unaffected (algorithm, report-only). No arm |

Hashes with "=" are the scenario's default hashes (H 8284be46/67931834/31fe043e, B 1d5bf4f9, W eeb92153/a329ca22/fa8618df).
Every pair OFF / ON in this table is byte-identical, which repeats item 1.3: on these scenarios the global PC is
inert at the hash level.

## 4. Predictions against outcome

| | prediction | outcome |
|---|---|---|
| P1 | OFF = refs | **held** |
| P2 | ON moves G-CTRL and B | **falsified**: byte-identical (and on Warzone) |
| P3 | CW per row | **held**, 0x00402f50 resolved to 0x007f |
| P4 | D2 3/3 in band | **held**, 6/6 |
| P5 | B 9/9, (e) 3/3 | **held** (identical hashes) |
| P6 | 13 tracks gates PASS | **held**, table identical |
| Sec. 3 | CC (amended) still needed, MATMUL redundant, PK bites?, COLL_LUT / LEAVES1 still needed | CC held, MATMUL held (vacuous), PK inert, COLL_LUT held on W only, LEAVES1 not exercised |

Why P2 failed, as far as measured: the per-call deltas exist (item 1.4) but none reaches a hashed field in G-CTRL, B,
Warzone, D2 or the 13-track table. The 2026-10-10 probe (`verify/d2_pc24_20261010/RESULT.md`) saw the same on D2.

## 5. Proposed decision for the lead

**Flip `MASHED_X87_PC24` to default ON (the game thread at PC_24 from device creation on)?**
- **Old and new gates:** G-CTRL `8284be46/67931834/31fe043e` -> `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 -> `1d5bf4f9`
  x3. D2 3/3 in band, 13-track table identical. The flip changes no reference in `merge_train.json`.
- **Evidence ON is closer:** S1 (every post-device row at the original's 0x007f, pre-device rows at 0x027f as in the
  original), and on 0x0046c5f0 the original's own instruction stream at 0x007f reproduces ON 452/452 vs OFF 372/452.
  The scored scenarios (S2, S4) are equivalent, not closer.
- **Options.** (a) Flip: default ON, `MASHED_NO_X87_PC24=1` opts out (the FaithDefaults seed pattern or a read-site
  default). (b) Keep OFF. (c) Flip and delete the now-redundant `MASHED_MATMUL_PC24` knob in the same step.
- **Recommendation: (a).** It is the original's own thread state, measured row by row. It costs no gate reference, and it
  makes every future x87 transcription (the 10 matched rows the audit lists as "x87 @ PC_53 would not match once
  called", and 0x00443300 after its DDK-order fix) run at the precision it was verified at. (c) can follow as a
  D5-KNOBS cleanup. Note for the flip step: the dev `.asi` is unaffected (`X87::GameThreadPc24()` is only set by the
  exe's `InitD3D9`).

## 6. Code changed (`orch/PORT-X87-PC24`)

- `mashedmod/src/mashed_re/Core/X87Pc.h` (new): `GameThreadPc24()`, `ThreadBaselinePc()`, `MASHED_X87_PC24_PK53`.
- `mashedmod/src/mashed_re/exe_main.cpp`: `ApplyX87Pc24AfterDevice()` on both `InitD3D9` success returns. `PumpOnce`
  no longer forces PC_24 from frame 0 (the 2026-10-10 form of the same knob name, no ledger owner, D5 knob inventory
  row "c"). It now counts and reasserts drift once the knob is applied, and `MASHED_LOG_CW` logs tid / knob / drift.
- `mashedmod/src/mashed_re/Gameplay/PickupTick.cpp`: `PK_Grant` / `PK_Sfx` use `ThreadBaselinePc()` (PC_53 unless the
  knob applied).
- `verify/savelink_20261023/run_arms.py`: stacks X24, X24H, X24LCW, DEFLCW, X24NCC, X24MM, X24PK, X24NCL, X24NL1, NCC0,
  MM0, NCL0, NL10, NCC0H, X24NCCH.
- Tools: `x87pc24_values.py` (per-call value probe), `x87pc24_trinormal_model.py` (original instruction-stream replay).

No tracker edit, no default flipped, `original\mashed_re_dev.asi` not redeployed.

## 7. Committed extracts (`verify/portstate_20261109/`)

`X87PC24_CW_EXE_OFF.tsv`, `X87PC24_CW_EXE_ON.tsv`, `X87PC24_TRINORMAL_SCORE.txt`, `X87PC24_D2_SCORE.txt`,
`X87PC24_TRACKS_SCORE.txt`, `X87PC24_RUNLOG.txt` (every arm's provenance line, knob list and hashes).

## 8. Raw files (`D:/MashedWT/PORT-X87-PC24_data`, not committed)

| file | sha256 |
|---|---|
| mashed_re.exe (worktree build, `MASHED_NO_DEPLOY=1`) | 55e5c513a4de5a0bc4ef31af1696dbb20189177a90939550ca237739af38bfa3 |
| mashed_re.map | 70f02510bcd7557952274e96623cf37c1b1e253858c13017e5a4bc9f529710fe |
| build.log | e122f7fe8bf8cd23345ff8f7bee4d4fc619913052b51b40add3906129eaa9fb2 |
| values_off.json | baf1afb4690af9baa463993cccb025c04f015e04144f266acb6d0012a2f0960b |
| values_on.json | 568f2c00b82cf569e13e901af4fdc64fac491614d4d2ba58f27247b16db3dc54 |
| cw_exe_off.json | 48b959c03a44d08cf62044c46a6041ab7191b91e0637b3e9c4634bee14330739 |
| cw_exe_on.json | 2982dd0ee6f9f32d9f6469e0185977f8716dac80e7934b99a567d0c68a345790 |
| arms/H_DEF.step.csv = arms/H_X24.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_X24.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_X24.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_X24_K0 / K1 / K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
| arms/B_X24LCW_K0.log (the `[x87]` / `[cw]` lines) | 69ab58b39c13a09ffd10d907df97e113fe31dcf7d8d3e27e6bbec0873e16c851 |
| arms/H_X24NCC.step.csv | 2bb28855bca7fbc0380c28112a111b02560a453d50b874fd7e90559789ed9c90 |
| arms/W_X24.step.csv | eeb921531da3e3086f8c63eb57c2821c163cf4710a67e83d953813ccf84eb955 |
| arms/D2_DEFH_1.motion_diag.log | 8e6108a87472386999df43a313fb7758ee3041131866298216cb1b95aa989910 |
| arms/D2_DEFH_2.motion_diag.log | dea61fb24a373c2cb7500257d669a628bf9f29f123cfeb0296f6c92a1398d1e9 |
| arms/D2_DEFH_3.motion_diag.log | 33652421b6cbf50e0d67611a1d572fa3ffc401a27b0110ea59fb65c85141cef9 |
| arms/D2_X24H_1.motion_diag.log | fe38081b72afd4ea1ad2c44aa451677598a7c7368bffedb4b6099b13c7042d84 |
| arms/D2_X24H_2.motion_diag.log = D2_X24H_3 | f9fdd52fbdcf74992b1972cc5d8f6d6ff8388c61d8be7475fa9c3cdef3f0d06d |
| tracks_score_x24.txt (raw tracks under `tracks_x24/`) | 2431afb1da35f95db9bb01535f071da5400d53fb2cc2550210465c0404be27c3 |
| arms/RUNLOG.txt | 46d09a10b9d3be178ae698e88cd4b0fd66b2b6e98672f71a626e98ad6a2ff61c |

Every boot ran through `mashed_lock.py run` with test-slot outside (D2 and run13 with `--solo`), root
`D:/MashedWT/PORT-X87-PC24_root`. No piz-collision crash, no short D2 arm (all >= 1620 motion_diag lines).
