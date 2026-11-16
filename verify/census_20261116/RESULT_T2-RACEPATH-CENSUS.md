# RESULT T2-RACEPATH-CENSUS: which functions the original runs, and what the top-down route has for each

## Verdict: PARTIAL (boot, audio-init and menu measured; race-load and race-tick not yet)

The census run of the ref1 recipe crashes in the race load (twice, see section 5). The report sent at the warp
survived, so everything up to the warp (tick 300, main menu) is measured. The race-load and race-tick rows are still
open. Section 6 tracks the rerun.

## 1. Method (measured, not inferred)

- **Recipe.** The ref1 argv of `verify/ro_20261116/REFERENCE_RO-REF-NAME.json`, unchanged, through
  `verify/census_20261116/census_arms.py` (a copy of `refname_arms.py` with this row's root, data folder and lock label).
  Root: `D:\MashedWT\T2-RACEPATH-CENSUS_root`, a robocopy of `RO-REF-FIXED-LOAD_root`. `original/MASHED.exe` sha256 is
  `b9977dab...` (the ref1 exe). The run used test-slot outside and `mashed_lock.py run --label T2-RACEPATH-CENSUS` inside.
- **Counters.** `detorig.py --prov-count` puts a native call counter on every entry of `re/console/pc_funcs.csv`
  (Interceptor + CModule, no JS per call, all threads). The counters go live at the PE entry (boot mode, `detorig.js`
  install). 6541 entries:
  - 6514 got a counter.
  - 27 were excluded by `census_list.py`, the rows of `not_instrumented.tsv`:
    - 12 have a `ret` before 5 bytes.
    - 2 have a `jmp` before 5 bytes.
    - 13 have a direct branch target inside Interceptor's patch window.
  - 1 install failed: `0x00497310`, "wrong type". It is the input lookup that `detorig.js` already hooks. Not counted.
- **Phase split.** Counter snapshots:
  - From `detorig.js`: tick 1, the warp, and the race anchor.
  - New in this row, `--prov-snap`: the first enter or leave of named RVAs.
    - `0x004669b0` enter: n=11, tick 0, main thread.
    - `0x005aa3d0` enter: n=13. This is the stop where both exe builds halt today (RESULT_PORT-TOP-AUDIOINIT-SPLIT-5A9970).
    - `0x0045d460` enter: n=17.
    - `0x004669b0` leave: n=45.

  All four fire before tick 1. The phases:

  | phase | from | to |
  |---|---|---|
  | boot | the PE entry | tick 1, minus the audio init |
  | audio-init | `0x004669b0` enter | `0x004669b0` leave |
  | menu | tick 1 | the warp (tick 300) |
  | race-load | the warp | the race anchor |
  | race-tick | the race anchor | the end (600 ticks) |

- **Status join** (`census_join.py`):
  - RE status: `hooks.csv` confidence plus the `c4=` route, `ADMITTED.tsv`, `PROVISIONAL.tsv` (status and score), and
    `PROVISIONAL_RWC.tsv` (score).
  - Binding on the top-down route, in this order of precedence:
    1. `verify/portorigtop_20261113/tools/plan.json`, the audio tree plan: bind match / admitted / provisional /
       replay, reuse, or stop.
    2. `verify/porttop_20261108/BINDINGS.tsv` (kinds mapped as listed in the script).
    3. `ROUTED.tsv`, the rows routed under MASHED_RWC_CORE.
    4. Static reach of today's exe (see "Reach" below): `matched` if C4 by match or libid, else `exe-body`.
    5. Anything else is `unbound`: no exe body on the route.
- **Reach.**
  - `reached_today`: the original ran the function before its first call of `0x005aa3d0`.
    [UNCERTAIN] This rests on the exe following the original's call order up to that stop. It is not measured on the exe.
  - `reach_static` (`census_reach.py`, `reach_top_static.tsv`): the linked exe's call graph walked from `_Orig_00492370@16`.
    - Tool: `scripts/backfill_port.py` build_graph (rel32 calls + base relocations).
    - Exe and map: the PORT-TOP-AUDIOINIT-SPLIT-5A9970 checkpoint-4 build (5cae85f7f, merged into orch/integration at 7a5e5e777).
    - Result: 17,871 exe functions reached, 1448 original RVAs named.
    - Static: a reached symbol may not run.

## 2. Counts per phase x RE status (distinct RVAs that ran in that phase; an RVA counts in every phase it ran in)

Executed before the warp: **1841 distinct functions**. 1031 of them ran before the exe's stop at `0x005aa3d0`.

| phase | C4:match | C4:libid | C4:frida-replay | C4 (no route tag) | C3 | C2 | C1 | not in hooks.csv | total |
|---|---|---|---|---|---|---|---|---|---|
| boot | 940 | 301 | 7 | 6 | 25 | 270 | 43 | 22 | 1614 |
| audio-init | 233 | 22 | 7 | 0 | 21 | 64 | 12 | 10 | 369 |
| menu | 184 | 45 | 5 | 6 | 15 | 22 | 4 | 2 | 283 |
| race-load | pending | | | | | | | | |
| race-tick | pending | | | | | | | | |

Byte-matched (`c4=match`, `c4=libid`) and replay-verified (`c4=frida-replay`) are kept apart, per L-128.

## 3. Counts per phase x top-down binding

| phase | matched | replay | admitted | provisional | stand-in | under-stand-in | hand-port | exe-body | shim | rw-lib | crt-lib | missing | stop | unbound |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| boot | 422 | 4 | 1 | 8 | 41 | 7 | 18 | 60 | 19 | 53 | 18 | 5 | 7 | 951 |
| audio-init | 133 | 3 | 8 | 11 | 6 | 0 | 2 | 11 | 2 | 8 | 8 | 1 | 12 | 164 |
| menu | 116 | 4 | 1 | 2 | 5 | 6 | 9 | 19 | 0 | 6 | 4 | 0 | 3 | 108 |

**`unbound` by hooks.csv subsystem (top 12).** On today's route most boot `unbound` rows sit below a librw, CRT or
stand-in binding: the exe replaces their caller, not them. That is a static reading of who calls them, not a
measurement.

| phase | rows by subsystem |
|---|---|
| boot | renderware 217, directshow-baseclasses 156, render 145, lua-4.0 126, msvc-crt-main 96, util 57, boot 34, libpng 21, audio 15, none 14, frontend 13, gameplay 11 |
| audio-init | audio 85, render 22, msvc-crt-main 15, boot 9, none 9, renderware 8, msvc-crt-tail 5, frontend 5, input 4, msvc-crt 1, hud 1 |
| menu | directshow-baseclasses 41, render 21, audio 13, frontend 9, renderware 8, util 5, input 4, msvc-crt-main 2, none 2, video 1, msvc-crt-tail 1, boot 1 |

## 4. Audio init: what remains before 0x0045d460

**94 functions** are first called after the exe's stop and up to the first call of `0x0045d460` (counter 0 at the
`0x005aa3d0` snap, nonzero at the `0x0045d460` snap), plus `0x005aa3d0` itself.

| group | count | what it needs |
|---|---|---|
| matched | 25 | already bound (plan.json) |
| replay | 2 | already bound |
| provisional | 2 | already bound |
| plan.json stop | 8 | a body |
| unbound, C4 by match or libid | 29 | a binding only |
| unbound, no C4 body | 28 | a body (match, admission or provisional) |

**Stops remaining before 0x0045d460: 65** = 8 plan stops + 29 bind-only + 28 needing a body.

- **The 8 plan stops:** `0x005a6340`, `0x005a9e60`, `0x005a9f10` (provisional 81.0), `0x005aa200`, `0x005aa3d0`,
  `0x005aac00`, `0x005ad420`, `0x005b36b0`.
- **The 28 that need a body:** `0x005a8060`, `0x005a84d0`, `0x005a8f30`, `0x005a91b0`, `0x005ae250`, `0x005aee20`,
  `0x005aef70`, `0x005b6be0`, `0x005b74c0`, `0x005b8680`, `0x005b9f30`, `0x005ba1d0`, `0x005bac00`, `0x005bad30`,
  `0x005baf00`, `0x005bb000`, `0x005bbc10`, `0x005bbd50`, `0x005bbdb0`, `0x005bbfc0`, `0x005bc470`, `0x005bce80`,
  `0x005bf670`, `0x005c1920`, `0x005c6cc0`, `0x005c7a90`, `0x005c7d30`, `0x005c8120`.
- The full list with status is in `census_counts.json` → `window_5aa3d0_to_45d460`.

**Order and thread, measured (run c4).** c4 ran `--prov-snap` on all 94 window RVAs, so each one's first call has a
sequence number and a thread id.
- The window membership is the same 94 in c2 and c4.
- 91 are first called on the thread that entered `0x004669b0`.
- 3 are first called on another thread (tid 21164), the `__beginthread` worker: `0x005c2dd9` (C4 libid), `0x005aef70`,
  `0x005a84d0` (both not in hooks.csv).
- The first stops in call order: `0x005aa3d0` (1), `0x005b36b0` (2), `0x005a6340` (3), `0x005aee20` (4, no C4 body),
  `0x005aa200` (5), `0x005adbe0` (6, C4, 449 calls).

**`audio_window_65.tsv`** has the 65 rows with these columns: class (plan-stop / C4-unbound / no-C4-body),
first_call_order (rank in the 94), calls up to `0x0045d460`, calls in the whole audio init, status and
first_call_thread. Generator: `audio_window.py <prov json> census_counts.json <out>`.

## 5. The race-load crash (c1, c2)

- **What happens.** Both runs exit with `0xC000008F` after the warp, in phase 2 (c2 stats: N=3569, tick 300, phase 2).
  This is not one of the U-9677 piz-collision signatures (those are access violations). No crash dump was written, and
  `--crash-catch` recorded nothing: its handler skips exceptions that Frida types `system`.
- **Boot perturbation.** The counters shift the boot by one clock call: bootlen N=233 tgt=11, against ref1's N=234
  tgt=12. qpc=222 is the same. The census race is therefore not the ref1 race tick for tick. Counts are per function,
  and the function set is what this row measures.
- **Changes for the rerun (c3).**
  - The `--prov-snap` listeners now detach after their snapshot. In c1 and c2 they stayed attached and ran JS on every
    later call of the four RVAs, on any thread.
  - `--crash-system` now logs `system`-typed exceptions (logging only).
  - Partial `prov` reports are sent at the warp and at the race anchor (`prov_warp.json`, `prov_anchor.json`).

- **c3** had both changes and still exits with `0xC000008F` after the warp, so the snap listeners were not the cause.
  Its 16 `system` log slots all went to OutputDebugString (`0x40010006` raised through KERNELBASE RaiseException,
  return address `MASHED.exe+0x987e3`). Those are now filtered.
- **c4** had the filter and the 94 window snaps. It exits the same way, and the exception handler recorded **no**
  exception at all, so the process ends without a first-chance exception Frida sees. It executed 1795 functions before
  the warp, against 1841 in c2: the 94 JS listeners change the boot. c4 is used only for the window order.
- **Patch-window safety.** A rescan with every jump-table entry as a target (the image has no base relocations,
  characteristics `0x10f`) excludes no new entry.
- **c5** records a backtrace at ExitProcess / TerminateProcess / NtTerminateProcess / RtlExitUserProcess
  (`--crash-exit`) to find who ends the process.

## 6. Pending

- c3 (D:/MashedWT/T2-RACEPATH-CENSUS_data/c3): race-load and race-tick counts, and the race-tick list of provisional /
  stand-in / missing functions ranked by calls.

## Raw files (not committed)

| file | sha256 |
|---|---|
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c2/prov_warp.json | b7dc4e935504d93aceeefdb61626304063c1a9608b49e1d0954fe004a130a647 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c2/meta.json | ed6aa726014023c6483fb312fe1b7c7a07d998c482667b8563bc438157e36f25 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c2.log | 406b4200ba23cb6e3b91e75c08fa0c3c52006a9f72d96e1be20195e7b2e13510 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c1.log | b8c8b770510d38fdbbe006faab4b6e2eaddb97daae7a173d33ad3bb9175310ca |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c4/prov_warp.json (window order) | e8307e2d8042b896af8ab4aeeeded0c4d1aa91ac91ba8128825f9fed59148dd7 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c4.log | ca9e7fb9fde3b74da2624d7dddf0695a94004d5bc98486c2f839907501784655 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c3.log | 53a5059238ba3d236ee76a9d83a6239f25368946966d579db4b0a23f3c19ca25 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/c3/crash.jsonl | 642f2dedba35047a33ee434dea1be724d13e9853367e8e2ac4bace35c77f0bc2 |
| D:/MashedWT/T2-RACEPATH-CENSUS_data/count_list.txt | 7226d3ed58553649a73fde527ad75d0cb4b50d2a6b0e0bd60258017d389f2cb4 |
| ../PORT-TOP-AUDIOINIT-SPLIT-5A9970/mashedmod/build/mashed_re.exe (reach) | 7a5af1b34a28c0b6e44038090f1b2aa033c2e5cca98818362879f6d6c92a0df8 |
| ../PORT-TOP-AUDIOINIT-SPLIT-5A9970/mashedmod/build/mashed_re.map (reach) | 0b055c33cc393b620bf582428e4f4e851b9ac07164fe8b8b6848d5fc4c895ed4 |

## Files

| file | content |
|---|---|
| `census_T2-RACEPATH-CENSUS.tsv` | one row per rva x phase: calls, status, binding, binding source, reached_today, reach_static, subsystem |
| `census_counts.json` | the tables above, plus the window list |
| `not_instrumented.tsv` | the excluded entries |
| `reach_top_static.tsv` | the static reach of today's exe |
| `census_list.py`, `census_join.py`, `census_reach.py`, `census_arms.py` | the scripts |

Harness: `re/frida/detorig.py` / `js/detorig.js` (`--prov-snap`, `--crash-system`, partial prov reports).
