# PREREG PORT-STATE-03-FPU: which float model does the original run 0x004325c0 at

Committed before any arm runs. Follow-up to PORT-STATE-03 section 4c and decision L-102 (Mariano 2026-11-08: keep
SSE2 for now, measure the original, whichever of SSE2 and x87 matches the original wins).

## Question

`re/match/s08/004325c0.c` computes its slide counters as `(int)(DAT_007f1004 * speed * K)` with K in
{1000, -2000, 500, -800} (speed 1.0 or 2.0 from DAT_0067eca4 >= 4), truncated by `_ftol` 0x004a2c48, and its default
path as `f8 = DAT_007f1004 * 300.0f + f8`. Three float models exist for the product:

- **F** (float per op): `trunc(f32(f32(dt*speed) * K))`. This is the SSE2 compile of the wrapper by construction, and
  it is also what x87 code produces at precision control PC_24 (each FMUL rounds to a 24-bit mantissa).
- **D** (53-bit or 64-bit): `trunc(dt*speed*K)` held at double or extended precision. This is the /arch:IA32 compile
  run at PC_53 (the standalone's CRT default, memory asm-bit-identity-in-asi-not-standalone-pc24) or PC_64.
- For speed 1 or 2 the product dt*speed is exact in every model, so F and D differ only where f32 rounding of
  dt*speed*K crosses an integer. Offline scan (numpy float32 vs Python double): **dt = 0.02f (0x3ca3d70a)** separates F
  and D for all four K (F: 20 / -40 / 10 / -16, D: 19 / -39 / 9 / -15 at speed 1), 0.015f (0x3c75c28f) for
  K = 1000 / -2000 / -800 (F 15 / -30 / -12, D 14 / -29 / -11), 0.0145f (0x3c6d9168) for K = -2000 (F -29, D -28),
  0.01375f (0x3c6147ae) for K = -800 (F -11, D -10). Control: 0.017f (0x3c8b4396) gives the same value in F and D for
  every K (17 / -34 / 8 / -13). PC_53 and PC_64 agree on all of these dts.

## Arms (all on `original/MASHED.exe` copied into the private root `D:/MashedWT/PORT-STATE-03-FPU_root`, ASIs removed
from that copy so the first phase runs the unmodified game; spawned pid only; `mashed_lock` inside, `test-slot` outside)

1. **CW (natural).** A native (no JS) onEnter stub on 0x004325c0 stores the x87 control word (FNSTCW), MXCSR,
   DAT_007f1004 bits and DAT_0067eca4 for every call at the main menu and through screen pushes driven by
   0x0043d2a0 (the menu_anim_diff.py method). Precision field = CW bits 8..9 (00 PC_24, 10 PC_53, 11 PC_64).
2. **Natural deltas.** For each natural call: the 30 x 0x34 table at 0x00898ac0 before and after the original's own
   tick (Interceptor.replaceFast, the original body called unchanged at the CW read just before it). Offline, each
   int-path record's `cnt` delta is compared to F and D for that call's dt. Reported: number of calls where F and D
   differ, and which one the original matched on each.
3. **Controlled dt.** On the game thread inside the original's own tick call: reload the records (0x0043d2a0(screen, 2)),
   force DAT_007f1004 to each listed dt, run the original body, read the table, restore table and dt, then run the
   port twin (`MenuAnimTickTwin` export of a dev .asi built with the SSE2 wrapper and, in a second boot, the x87
   wrapper) on the same input (a) at the thread's own CW and (b) under a forced PC_53 CW (0x027f with the thread's
   other bits kept), restoring the CW after. dts: 0.02f, 0.015f, 0.0145f, 0.01375f, control 0.017f, plus the call's own dt.
4. **Standalone CW.** Native onEnter stub on the `MenuAnimTickTwin` export of `mashed_re.exe` (both frozen builds)
   during the NAV_KEYS key script, to record the CW the port's tick runs at.

Builds: the frozen PORT-STATE-03 images, SSE2 `exe_l100_sse` (sha256 44b3da06..., the committed wrapper) and x87
`exe_l100` (0a1ab369..., `MatchS08_004325c0` in `x87_tus.txt`), copied to `D:/MashedWT/PORT-STATE-03-FPU_data`.

## What each outcome means

| original result (arm 3 at the discriminating dts) | CW at entry (arm 1) | verdict |
|---|---|---|
| every int-path delta equals F | PC_24 | **SSE2 matches the original.** The x87 compile matches only under PC_24, which the standalone does not run at (arm 4). Keep SSE2, no commit beyond PREREG/RESULT/probe. |
| every int-path delta equals D | PC_53 or PC_64 | **x87 matches.** Commit the `x87_tus.txt` entry and the new frontend state-log references if default gates hold (G-CTRL 8284be46/67931834/31fe043e, B 1d5bf4f9 x3); a moved gate = DECISION with both hash sets. |
| F on some samples, D on others, or neither | any | **neither**; RESULT lists the residual samples (dt, K, original, F, D). DECISION. |
| arm 3 and arm 1 disagree (e.g. F results under a PC_53 word) | | the CW read is not the CW the body ran at; RESULT reports it, verdict from arm 3 (the direct value witness). |

Cross-checks that must hold for the run to count: the SSE2 twin equals the original byte for byte at both CWs if the
verdict is F. The x87 twin equals the original at the thread's own CW if that CW is PC_24, and differs from it under
the forced PC_53 word at dt = 0.02f. A twin result that disagrees with its model voids the arm (harness fault, re-run).
Arm 2 is supporting evidence only: if no natural call has F != D it is reported as "not discriminating".

A piz-collision crash (`MASHED.exe+0x1509b4` / `+0xe66db`) is a re-run.
