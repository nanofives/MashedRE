# RESULT PORT-STATE-03-FPU: the original runs 0x004325c0 at PC_24, so the SSE2 wrapper matches it and x87 does not

Verdict: **GREEN, SSE2 stays.** No code change. PREREG `44287a914` (`PREREG_FPU.md`, committed alone before any arm).
Follow-up to PORT-STATE-03 section 4c and L-102 (Mariano 2026-11-08: whichever of SSE2 and x87 matches the original
wins).

## 1. Answer

| measurement | original `MASHED.exe` | `mashed_re.exe` SSE2 wrapper (`44b3da06`, committed) | `mashed_re.exe` x87 wrapper (`0a1ab369`) |
|---|---|---|---|
| x87 CW at entry of the tick (native FNSTCW) | **0x007f = PC_24**, 731/731 calls in 2 boots (plus 366 in the first boot) | 0x027f = PC_53, 785/785 calls | 0x027f = PC_53, 800/800 calls |
| MXCSR at entry | 0x9fa0 | 0x1fa0 | 0x1fa0 |
| frame dt DAT_007f1004 at the menu | 0x3c888888 (1/60) every call | 0x3c888888 every call | 0x3c888888 every call |
| f8 path at one input: f8 = 0x411fffff (9.999999), dt = 0x3c888888 | **0x416ffffe** | **0x416ffffe** | 0x416fffff |
| model the run equals on every record where F and D differ | **F** (164/164 controlled, 480/480 natural, per boot) | **F** (27/27 keys, 248/248 walk) | **D** (27/27 keys, 248/248 walk) |

Models (`PREREG_FPU.md`): **F** = every float op rounded to f32 (the SSE2 compile, and x87 at PC_24). **D** = the
product or sum held at double (x87 at PC_53, the standalone's CW). The original equals F. The SSE2 wrapper equals F.
The x87 wrapper equals D, because the standalone runs it at PC_53. With the same dt and the same record value as the
original, the x87 build stores a different f8 bit pattern (0x416fffff instead of 0x416ffffe).

The moved PORT-STATE-03 hashes are exactly these records. The calls where F and D differ in the standalone are call
2 (key script) and calls 2, 32, 94, 125, 156, 218, 280, 311, 342, 497 (walk). The NAV_KEYS lines that differ between
the SSE2 and x87 builds are line 3 (key script) and lines 3, 33, 95, 126, 157, 219, 281, 312, 343, 498 (walk): one
moved line per discriminating call, 11 of 11, none elsewhere. Each is the frame where the unused records (type 0, 23 to
27 per call) take the default `f8 = dt * 300.0f + f8` step from 0x411fffff. That is the PORT-STATE-03 observation
"1/185 and 10/619 lines move, then converge".

## 2. Arms (all GREEN against the PREREG table, row 1)

Runs: `test-slot` outside, `mashed_lock.py run --label PORT-STATE-03-FPU` inside, private root
`D:/MashedWT/PORT-STATE-03-FPU_root` (robocopy of `original\` with every `*.asi` left out, so arms 1 and 2 run the
unmodified game with no dev DLL loaded). Spawned pids only, each killed by pid. No piz-collision crash occurred.

**Arm 1, CW (native, no JS).** onEnter stub on 0x004325c0 storing FNSTCW, STMXCSR, DAT_007f1004 and DAT_0067eca4.
From the first frontend call through one screen push (0x0043d2a0(1, 0)): 367 and 364 calls (boots 2 and 3), every one
CW 0x007f (PC_24, round to nearest, all exceptions masked), MXCSR 0x9fa0, DAT_0067eca4 2 then 3. Frida's own JS thread
reads 0x027f, so the game thread's 0x007f is not a probe artefact.

**Arm 2, natural deltas.** Original body called unchanged from Interceptor.replaceFast, records 0..29 at 0x00898ac0
before and after, every 30th call reloads the screen (0x0043d2a0(screen, 2)). 600 calls per boot, CW 0x007f before
every call, dt 0x3c888888. 480 record steps per boot discriminate (all on the f8 path, the int paths' products at
this dt give the same integer in F and D). **480/480 = F** in both boots, 17520 non-discriminating steps equal both.

**Arm 3, controlled dt** (4 rounds x 6 dts x speed 1 and 2 = 48 samples per boot, each from a fresh reload):

| run | boot 2 (SSE2 twin) | boot 3 (x87 twin) |
|---|---|---|
| original at its own CW 0x007f | 164/164 = F | 164/164 = F |
| original under forced PC_53 (0x027f) | 164/164 = D | 164/164 = D |
| twin at CW 0x007f | 164/164 = F | 164/164 = F |
| twin under forced PC_53 | 164/164 = **F** | 164/164 = **D** |

Discriminating records by (K, speed): (-2000, 1) 60, (-2000, 2) 80, (-800, 1) 12, (-800, 2) 12. Per dt for the
original at its own CW: 0.02f, 0.015f, 0.0145f, 0.01375f all F on every discriminating record. The control dt 0.017f
discriminates nothing and every run equals both models there. Non-discriminating records: 1276/1276 equal in every run.
The forced-PC_53 row of the original proves the probe separates the models on the original's own code, so the F result
at its own CW is a measurement and not a blind spot. The cross-checks in the PREREG hold: SSE2 twin = original at both
CWs, x87 twin = original at PC_24 and differs under PC_53. CW after each sample = 0x007f (restored).

Not exercised by arm 3: the positive-K branches (+1000, +500). After the reload every int-path record is in state 0,
so only the -2000 and -800 products ran. Arms 1 and 2 show the whole body runs at PC_24, and the positive branches use
the same `dt * speed * K` FMUL chain feeding `_ftol` 0x004a2c48.

**Arm 4, standalone** (native onEnter/onLeave stubs on the `MenuAnimTickTwin` export, records copied at entry and exit;
PORT-STATE-03's key script `MASHED_GOTO=1,2,4,6 MASHED_NAV_KEYS=d,u,d,u,e` and walk
`MASHED_GOTO=1 MASHED_NAV_KEYS=e,d,e,b,b,d,e,d,e,b,b,d,d,e,b,b,u,u,u`). The probed runs reproduce the PORT-STATE-03
NAV_KEYS hashes exactly: SSE2 `4e5e35a2` (185 lines) / `efc763d4` (619 lines), x87 `2e8cc64f` / `dc862c7e`, so the probe
changes nothing. The SSE2 key-script run did not self-exit within 300 s under Frida and was killed by pid after its log
had all 185 lines (175 of its tick calls were drained before the kill, 184 in the x87 run).

First boot (`orig_sse_1.json`): arm 1 valid (366 calls, CW 0x007f) and arm 2's record values valid (480/480 = F). Its
JS-side CW reads and arm 3 were void: the helper stub's page was garbage-collected and reused by the arm 1 stub. Fixed
(`KEEP` list in both probes) before boots 2 and 3, which are the scored runs.

## 3. Decision for the lead

L-102 is answered: **SSE2 matches the original. Keep `MatchS08_004325c0` out of `x87_tus.txt`** (as committed in
PORT-STATE-03, `20142cd32`). The x87 compile would match only under a PC_24 scope (memory
asm-bit-identity-in-asi-not-standalone-pc24), which SSE2 float code reproduces without one. The `x87_tus.txt`
policy line for matched float code ("MATCH-S08 2026-11-01") does not hold for this TU in the standalone. Nothing to
commit in `mashedmod/`. The frontend state-log references stay `4e5e35a2` / `efc763d4`.

Gates: not run here (no code change). The SSE2 image probed is PORT-STATE-03's committed build, gates there G-CTRL
`8284be46/67931834/31fe043e`, B `1d5bf4f9` x3.

Wider note, not acted on: the original's frontend thread runs at PC_24, and the standalone at PC_53. Any other
x87-compiled TU in `x87_tus.txt` that the standalone runs on the frontend thread with float intermediates can round
differently from the original for the same reason. The race thread is PC_24 too (memory above). A check of the other
MATCH-S08 entries (`MatchS08_004331a0`, `MatchS08_0043c000`, `MatchS08_00442420`, ...) under a PC_24 scope is a
candidate follow-up step.

## 4. Commits (`orch/PORT-STATE-03-FPU`, from `orch/PORT-STATE-03` c374b0673)

| commit | content |
|---|---|
| `44287a914` | PREREG_FPU.md |
| this commit | `fpu_probe.py` (original: arms 1-3), `fpu_exe_probe.py` (standalone: arm 4), `fpu_score.py`, `FPU_score_orig.txt`, `FPU_score_exe.txt`, this RESULT |

## 5. Raw files (`D:/MashedWT/PORT-STATE-03-FPU_data`, not committed)

| file | sha256 |
|---|---|
| exe_l100_sse/mashed_re.exe (SSE2, committed wrapper) | 44b3da068a8dc3f80370c27da58033556015f5a89a48aa07001b2f268875ec1f |
| exe_l100_sse/mashed_re_dev.asi | b1fb259cd26d745089da7979aab5a227b61d38b9254662e92b4a072a9fe218e7 |
| exe_l100/mashed_re.exe (x87 wrapper) | 0a1ab3692fc746ac41d7924af47df61261052b9cf580f1e3e3059f0e213df391 |
| exe_l100/mashed_re_dev.asi | a5bd5b440d17a506dfcaf944472334dad7411672271ca14d293abd8762b6fd55 |
| orig_sse_1.json (first boot, arm 3 void) | 060f136d8eb2404b8dad2132715de4f2e99bee1b8d2097a2e94a8b5dcd4ffbce |
| orig_sse_2.json | 77cf4bc35e1eb14424ec670e6697667d2c1de26249161bb16ab0184f584dc171 |
| orig_x87_2.json | 397f0714862200a3307e1185aee53fca1bf4546d6d29fc5e199baf084e08862f |
| exe_l100_sse_keys.json | dc4265eb229656b240b80609c6fd834c9d3a6cbc07403ef378d2c225fb50fe38 |
| exe_l100_sse_walk.json | bd60abbcb185198f420a73545a5ac2177de12c55284628ce1b8d49fa001104ba |
| exe_l100_keys.json | 44fca95e95096ca508eac2087d828f4176d622c00068eb2f638b4ef095b79751 |
| exe_l100_walk.json | 4772c5b405beb80a4309f1ec072338e744b2c78242ecee52041dba3787835ae0 |
| exe_l100_sse_keys.mashed_re.log | 96d9638ee45013ae9d6befcbb4d997fe47022c209b8923569048a9b8d264a551 |
| exe_l100_sse_walk.mashed_re.log | c23f4c13081ee2e5214f29a717c5c1cddf2c9a1b753500f09409a6f19e937f0b |
| exe_l100_keys.mashed_re.log | 04ad67a274f48d9d1b947a8ca94d813ea7c150e9ab36f4de4432294133724f0e |
| exe_l100_walk.mashed_re.log | 630b63382a04360a05d137c962e269578112e85ca55001ba14b423e3b1a292da |
| runall.log | e2cb6307fbac52e9aa1d6e9b0c9c7febcf9495074772af6f493b6d51dec4d7e3 |
