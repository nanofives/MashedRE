# RESULT RO-AUDIOTICK-BODIES (2026-11-12)

Verdict: GREEN. 0x0045d3f0, 0x00466b50 and 0x0045d430 now run their byte-matched sources in the exe, so
`MASHED_OT_AUDIOTICK=1` no longer stops on frame 0. Top-down row (knob path only). `MASHED_OT_AUDIOTICK` stays
default OFF, so the default path is unchanged.

Self-test (run 2, all four arms): `[OT_TIMER] self-test PASS`.
- vs_audio: 0 of 8413 checks failed, 0x004926c0 calls 600, audio branch 600.
- ddk_audio (object build): 0 of 8413 checks failed, audio branch 600.

No stop fired. The bodies ran at 00016C70 / 00016CE0 / 00016CB0, which are the BYTECHECK exe VAs.

## What changed
- New exe-only wrappers (marked `// RVA-LINT: ORIGTOP-ONLY`, `#ifdef MASHED_STANDALONE`), in `exe_sources.rsp`:
  - `Match/PortTop_0045d3f0.cpp`: re/match/s10/0045d3f0.c, `PortTop_Bound_0045d3f0`.
  - `Match/PortTop_0045d430.cpp`: re/match/s10/0045d430.c, `PortTop_Bound_0045d430`.
  - `Match/PortTop_00466b50.cpp`: re/match/s10/00466b50.c, `PortTop_Bound_00466b50`.
  All three are hooks.csv C4 `c4=match`: BYTE-IDENTICAL 53 / 39 / 1208 B, relocheck OK.
- `Match/MatchS15_004926c0.cpp` (exe): FUN_0045d3f0 / FUN_00466b50 / FUN_0045d430 now bind to those bodies instead of
  the RO-BIND-TIMER named stops. The `.asi` keeps its raw-address calls.
- Callees:
  - Three callees of 0x00466b50 bind to their existing exe bodies: 0x00426c00 `GameStateFlagGet`, 0x0045b350
    `RwInitNullStub`, 0x004d8560 `UtilReturnOne4d8560`.
  - The other 52 callees of 0x00466b50, plus 0x00550be0 / 0x005a60b0 (callees of 0x0045d3f0) and 0x005a60e0 (callee
    of 0x0045d430), bind to named stops `PortTop_AStop_<rva>`. Each stop calls `PortTop_AudioCalleeStop` in
    `Match/PortTop_00492370.cpp`, which logs and exits with code = rva.
- Self-test (`PortTop_Timer_004926c0`):
  - When DAT_006147b8 != 0 after the call, the audio branch ran: it checks DAT_006904e8 == 0 and that DAT_006904ec is
    unchanged.
  - On the first audio-branch call it also calls 0x0045d3f0 / 0x0045d430 directly and checks that each returns 1 and
    writes nothing.
- `verify/porttop_20261108/BINDINGS.tsv`:
  - 0x0045d3f0 / 0x0045d430 / 0x00466b50 move from missing to matched.
  - The 0x004926c0 note is updated.
  - 52 new level-3 `missing` rows for the stopped callees. The rvas that already had a row keep it: RVAs are unique
    there, and the six such callees are covered by existing rows.
- `re/tools/dual_copy_allowlist.txt`: one `ORIGTOP-ONLY CROSS-TARGET:0045d3f0` line. The asi-only
  `MatchS10_0045d3f0.cpp` and the exe-only `PortTop_0045d3f0.cpp` hold the same source text.

## Why the callees are not reached on the exe path
All three bodies return at once while `DAT_006904e8 == 0`:
- 0x0045d3f0 tests it at `if ((DAT_006904e8 != 0) && (DAT_006904ec == 0))` (re/match/s10/0045d3f0.c).
- 0x0045d430 tests it the same way.
- 0x00466b50 returns on `if (DAT_006904e8 == 0) return;`.

0x006904e8 is .bss: .data raw size 0x4d000 ends below offset 0xa64e8 in the unpatched MASHED.exe, so it is 0 at load.
Its only writer in re/match is 0x004669b0 (`DAT_006904e8 = 1`, re/match/s10/004669b0.c). The exe runs 0x004669b0
as the hand stand-in piece 3 (`PortTop_Piece_004669b0` -> `HandTop_InitAudio`, exe_main.cpp:9842), and that stand-in
never writes the word.

So on the exe path these bodies are the original's code taking the original's early-return branch, with the
original's gate value for an un-initialised sound engine. The original sets the gate in 0x004669b0. Once a matched
0x004669b0 lands, the stops name the next callees to bind.

## Self-test model correction (RO-BIND-TIMER's DAT_007f1010 check)
Run 1 (`arms.out`):
- ddk_timer: 191 of 6000 failed. This arm has no AUDIOTICK, so the failures are not from this row's code.
- ddk_audio: 229 of 8413 failed.
- Every failure was `390 DAT_007f1010` off by 1 ulp. Example: call 1, now 619086351, got 0x434E5CB4, model
  0x434E5CB3.

The model computed `(float)now * 3.3333333e-07f`, which rounds `now` to float first (SSE, the VS2022 body's form).
The DDK body loads `now` exact with `fild` and rounds the product once.

Exact-fraction check of that call:

| form | result |
|---|---|
| SSE | 0x434e5cb3 |
| single rounding of the exact product (PC_24) | 0x434e5cb4 |
| PC_53 then float | 0x434e5cb4 |
| observed | 0x434e5cb4 |

The two forms differ only when now >= 2^24. RO-BIND-TIMER's run did not reach that range (its ddk_on passed 0 of
6000). The check now accepts either form (`X87MulExact`, integer RNE on the 56-bit exact product) and counts the
calls where only the x87 form held:

| arm | x87-only calls |
|---|---|
| vs_timer | 0 |
| vs_audio | 0 |
| ddk_timer | 107 |
| ddk_audio | 112 |

The object build computes the original's form, as L-125 expects.

## Builds (at the commit below)
- VS2022: Build OK (`D:/MashedWT/RO-AUDIOTICK-BODIES_build_vs.log`, sha256 b973735b101c2b18...). rva-lint OK, 0 NEW.
- Object build (`MASHED_DDKOBJ=1`): Build OK (`D:/MashedWT/RO-AUDIOTICK-BODIES_build_ddk2.log`, sha256
  6917447bff4c1032...).
  - ddkobj check: IDENTICAL 794/881, DIFFERENT 77, not-in-map 10.
  - Units linked 840/850. 9 refusals, all `wrapper-redefines-globals`, none of them this row's.
  - None of the 77 DIFFERENT is this row's (the list is in the log).
- BYTECHECK.tsv for this path, all IDENTICAL with diff_bytes 0:

| rva | exe symbol | exe VA | size |
|---|---|---|---|
| 0x0045d3f0 | `_PortTop_Bound_0045d3f0` | 0x00016c70 | 64 |
| 0x0045d430 | `_PortTop_Bound_0045d430` | 0x00016cb0 | 48 |
| 0x00466b50 | `_PortTop_Bound_00466b50` | 0x00016ce0 | 1216 |
| 0x004926c0 | `_AudioTickAndAvg` | 0x000169e0 | 176 |
| 0x00493480 | `_FpsDiscretise` | IDENTICAL | |
| 0x00493390 | `_PortTop_Bound_00493390` | IDENTICAL | |

## Knob boots (run 2)
`test-slot` outside, `mashed_lock.py run --label RO-AUDIOTICK-BODIES` inside. Script:
`verify/roaudiotick_20261112/audiotick_arms.sh`.
- Root: `D:/MashedWT/RO-AUDIOTICK-BODIES_root` (robocopy of `original\`).
- `MASHED_DET_FRAMES=600`.
- Output: `arms_run2.out` (this folder). Logs: `D:/MashedWT/RO-AUDIOTICK-BODIES_data/*.log`.

| arm | exe | knobs | result | log sha256 |
|---|---|---|---|---|
| vs_timer | VS2022 | ORIG_TOP + OT_TIMER | rc 0, PASS 0/6000, 0x004926c0 calls 0 | a205550d92c930df... |
| vs_audio | VS2022 | ORIG_TOP + OT_TIMER + OT_AUDIOTICK | rc 0, PASS 0/8413, 0x004926c0 calls 600, audio branch 600 | 225a8a057854bcf9... |
| ddk_timer | object build | ORIG_TOP + OT_TIMER | rc 0, PASS 0/6000, x87-only form 107 | 36a45732aa29bfe6... |
| ddk_audio | object build | ORIG_TOP + OT_TIMER + OT_AUDIOTICK | rc 0, PASS 0/8413, audio branch 600, x87-only form 112 | 5e2ffd3ad37a1c63... |

Before this row, RO-BIND-TIMER's vs_audio arm stopped with `rc 0x0045D3F0 after 1.8 s`.

## Not done / notes
- No G-CTRL / B / 12 captures: top-down row (WORKER_RULES "Row kinds"). Nothing on the default path changed:
  - The new TUs compile only under `MASHED_STANDALONE`.
  - Their only caller is reached under `MASHED_ORIG_TOP=1` + `MASHED_OT_AUDIOTICK=1`.
  - The self-test edit is inside the `MASHED_OT_TIMER` / `MASHED_OT_AUDIOTICK` dispatchers.
- hooks.csv `port` column not touched (trackers move through re-classify, not this row).
- No new librw binding (L-124). No lifted or unmatched body (L-126 / L-137): all three sources are byte-matched.
- The first arms run started while `mashed_lock` showed a merge-train PRIORITY-HOLD. The boots queued behind it inside
  `mashed_lock.py run`. Neither build overlapped a HOLD or SOLO (status checked before each).
- Turning `MASHED_OT_AUDIOTICK` on by default on the knob path is a P1-class flip (L-127). It needs the OFF/ON/OFF/ON
  capture comparison, which was not run here.
