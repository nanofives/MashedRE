# RESULT D4-WSB-C4-SOLV-RGN: GREEN (3 of 5 rows C3 -> C4)

Verdict: **GREEN.** `0x0056bce0`, `0x0056ed60` and `0x0056bb30` pass both legs the PREREG fixes
(`PREREG_D4-WSB-C4-SOLV-RGN.md`, commit `eb1a8b9ce`) and move C3 -> C4 through re-classify. `0x0056cf90` and
`0x0056fad0` pass leg R but the racediff observes 0 calls on both sides in Quick Battle and in the census run, so
they stay C3 (PREREG: C4 only when both legs PASS). Default gates unchanged. No source change on the branch.

Run 20 (2026-10-28/29) replaces the run-19 BLOCKED result: the Philips FTV audio endpoint was back.

## Build, root, boot check

- Branch `orch/D4-WSB-C4-SOLV-RGN` off `orch/integration` @ `0d1285dc7`. `git diff 0d1285dc7` on `mashedmod/` and
  `hooks.csv` is empty on this branch and on `orch/D4-WSB-STACKONLY-C3`, so the shared build is this branch's build:
  `mashed_re.exe` `779bc9c329b6...c447`, `mashed_re_dev.asi` `c6a264a3bde1...3566` (built in the STACKONLY
  worktree; the `.asi` and its `.map` were copied into this worktree's ignored `mashedmod/build/` because
  `c4_racediff_v7.py:277` reads the map from the running worktree). Private root
  `D:/MashedWT/D4-WSB-STACKONLY-C3_root`, its `original/mashed_re_dev.asi` = `c6a264a3...`.
- **Boot check (lead's rule a):** one plain launch of the original from the private root under `mashed_lock.py`,
  `MASHED_HOOK_ONLY=__none__`, audio endpoint `OK Philips FTV (NVIDIA High Definition Audio)` before launch. PID 5640
  ALIVE at 25.0 s, then killed (own PID only). No AV at `0x005baf48`. Log `bootcheck2.log`.
- **Audio rule (b):** `drive_rgn.py` (new, harness only) runs each row in its own solo window and checks
  `Get-PnpDevice -Class AudioEndpoint -PresentOnly` inside the window right before the runner, then again after,
  plus the Application log's MASHED.exe Event 1000 records since the invocation started (fault offset
  `0x001baf48` = BLOCKED, `0x001509b4` / `0x000e66db` = U-9677 re-run). Every check: endpoint present, 0 crash
  events. No U-9677 re-run was needed. Limit: a racediff invocation is an OFF boot and an ON boot inside one tool
  call, so the ON boot's own pre-launch check is the post-check of the pair (the tool was not edited).
- **Lock (c):** every boot under `mashed_machine`. Row 3's first shadow attempt spent its runner's 1800 s timer
  queued on the lock (a D2 solo window opened mid-wait), so that attempt was stopped before any game spawned (no PID
  launched, the row's data dir held only an empty start) and the rows were re-run inside per-row solo windows. The
  merge train ran between rows.

## Leg R: region shadow (4 rows), `run_shadow6.py`

Census, track 0, mode 10, 4 cars, `HOLD=60`, `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`,
`SAMPLES_DONE=24`, hook installed alone. Summary `shadow_summary.tsv`.

| row | region | verdict | n | ndiff | PATCHBYTE | NOISY / OTHER-EXC lines | game PID |
|---|---|---|---|---|---|---|---|
| `0x0056bce0` | param_1, 0x10 | CLEAN | 48 | 0 | installed=e9 uninstalled=83 A/B-IS-REAL | 0 | 19480 |
| `0x0056cf90` | param_1, 0x60 | CLEAN | 48 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | 0 | 20312 |
| `0x0056ed60` | param_1, 0x2c | CLEAN | 48 | 0 | installed=e9 uninstalled=83 A/B-IS-REAL | 0 | 20536 |
| `0x0056fad0` | param_1, 0x3c | CLEAN | 48 | 0 | installed=e9 uninstalled=8b A/B-IS-REAL | 0 | 2980 |

SCOPE region-coverage half of leg R, from `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md`: the highest byte
written equals the declared size for all four (lines 133-135), and `0x0056bce0`'s three param slot spills reach no
reader (lines 142-150). Leg R PASS on all 4.

## Leg A: `0x0056bb30`

SCOPE line 108: Run wrapper, no store outside its own frame, one call (`0x0056ba30`). PASS.

## Racediff (5 rows), `run_bvh6.py` -> `verify/wsbsolvws_20261027/c4_racediff_v7.py` (unchanged)

Quick Battle, A4 calls 300..1200, OFF boot `--hooks __none__` (asi loaded, hook byte not JMP), ON boot with the row
alone (`installed` = ON entry byte `0xE9`, `c4_racediff_v7.py:401`). Summary `racediff_summary.tsv`.

| row | Quick Battle | census (`C4_HOLD=400`, A4 calls 1..3000) | read-out |
|---|---|---|---|
| `0x0056bce0` | C4-CLEAN, 6/6 calls, 1 sig each side (`eax=-`) | not needed | PASS |
| `0x0056cf90` | HOLD orig-not-observed, modded-not-observed, 0/0 calls, 2 boot pairs | HOLD, 0/0 calls, 2 boot pairs, armed (ON jmp 233 = 0xE9, OFF 139 = 0x8B) | not PASS |
| `0x0056ed60` | C4-CLEAN, 6/6 calls, 1 sig each side (`eax=-`) | not needed | PASS |
| `0x0056fad0` | HOLD orig-not-observed, modded-not-observed, 0/0 calls, 2 boot pairs | HOLD, 0/0 calls, 2 boot pairs | not PASS |
| `0x0056bb30` | C4-CLEAN, 6/6 calls, 1 sig each side (`[c56ba30,h0,0..3] eax=A0+0`) | not needed | PASS |

No HOLD with a pointer-label (LAYOUT) difference occurred. Recorded, not interpreted: `0x0056cf90` and `0x0056fad0`
gave 48 shadow samples each in the census `HOLD=60` boot, while the racediff census window (A4 calls 1..3000) saw 0
calls of either on both sides. These runs do not establish which of the two observation paths or windows
accounts for the difference.

## Default gates (same exe as STACKONLY, one run serves both)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_EXE` = the shared `mashed_re.exe`
(`exe=779bc9c3` printed per arm), `A4_ROOT` = the private root, under the lock and the test slot.

- G-CTRL `5943c5f1/364350ae/52052890` (ref `5943c5f1/364350ae/52052890`)
- B `3a03670c/9fcdbd3a/53778664` (ref `3a03670c/9fcdbd3a/53778664`)

Unchanged against `re/orchestrator/merge_train.json` on `orch/integration`.

## Tracker transaction (re-classify, Edit tool on the target rows only)

| row | old -> new | scenario / frida_diff |
|---|---|---|
| `0x0056bce0` | C3 -> C4 | `c4_racediff_bvh:QuickBattle-f300-1200+shadow_region:track0-mode10-cars4` / `racediff_summary.tsv` |
| `0x0056ed60` | C3 -> C4 | same |
| `0x0056bb30` | C3 -> C4 | `c4_racediff_bvh:QuickBattle-f300-1200` / `racediff_summary.tsv` |
| `0x0056cf90` | stays C3 | note appended (C4 attempt) |
| `0x0056fad0` | stays C3 | note appended (C4 attempt) |

- Stub sweep: no `STUB` marker in `RwpSolverIntegrate6.cpp`, `RwpSolverCore8.cpp`, `RwpSolverCore21.cpp`, no
  `STUBS.md` row for the three RVAs.
- Uncertainty sweep: 0 `[UNCERTAIN]` markers in the three plates. `U-9020` (numeric, non-blocking: printed FADD
  order of `0x0056ed60`'s 18 three-term dots) stays open and is named in the row's note. The C4 gate in
  `re/CONFIDENCE.md:85` is a clean diff on a canonical scenario plus no stubs.
- One body per row: each TU is in both `asi_sources.rsp` and `exe_sources.rsp` and holds the only definition, so the
  level covers the shipping exe (`re/CONFIDENCE.md:103-105`).
- `re/analysis/CHANGELOG.md`: 3 lines prepended below `<!-- ENTRIES -->`. No new U-/S- rows.
- hooks.csv: 5 lines changed, CRLF kept. The 37 rows with a column-count mismatch are already there at HEAD.

## Raw data (`D:/MashedWT/D4-WSB-C4-SOLV-RGN_data`, SHA-256)

| file | sha256 |
|---|---|
| `bootcheck2.log` | `25638ba58ee57be60ea4afed0b4e5f168c820a32ed6553ce90fc107aca0f3fc9` |
| `drive_rgn.log` | `41fb6dd4458c85cf785be91eac394ba1fba7211d1b5e97cd8bca4b0d4a9e8715` |
| `0056bce0/shadow_ab.log` | `0da3485a789d51d1473804520fb922b195805a4044f66ea0de2362437bb9eabf` |
| `0056cf90/shadow_ab.log` | `8b93d9f368d22e8ef1b85f642b18ea15775f6fb3d1a827a7c2413718fa11d8bf` |
| `0056ed60/shadow_ab.log` | `9ec0a718bcc6c26cee42d148008d2418f7109a6022c35e6d036cb397c1fb237e` |
| `0056fad0/shadow_ab.log` | `fa9005d0c8331593a5fb0e5475101cae06b94888b7e24600d992c9db8dfc577b` |
| `0056bce0/c4_racediff_samples.json` | `c281a8b63c526b94f6152e2433a923d8a9e4dd9d850daa6e48b33f1d0c1b3a1a` |
| `0056ed60/c4_racediff_samples.json` | `1ba8a93c9aecbd55df996123f2101ac6691514611c87490a6546a56f9acd79c3` |
| `0056bb30/c4_racediff_samples.json` | `1de8892e737e41625589e8849a2216d9da81dd0817a4dc0c286d229aa3aa4354` |
| `0056cf90/`, `0056fad0/` and `_census` `c4_racediff_samples.json` (all empty) | `3968b8b8d5257bbb2c9c5e8ea42e559746d0513df7123f0e1d3c214ebd50c9a8` |
| `0056bce0/c4_racediff_result.json` | `90cbe064a2c6200d03472d42b79a6dc8d4e6d7d7fb11e4e6629229ad6a6b3212` |
| `0056ed60/c4_racediff_result.json` | `ff62e6d15af2ff86ca24c4a5506a4c3658ddae89756be47b3a2afaed897f410d` |
| `0056bb30/c4_racediff_result.json` | `9da0bd69917d977fb7080e22474cf28739218aff48cef88d30f2006ab373d2b2` |
| `0056cf90/` and `0056cf90_census/c4_racediff_result.json` | `7155f396495c2e7310252c9babcf37bb4628eb62f9289a48f2154e31e899a2ea` |
| `0056fad0/` and `0056fad0_census/c4_racediff_result.json` | `efeef79bdf4f4f0b4e23b6355978712e2e98cc8f1a7a214b6352551d3f2e94f0` |
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates.log` | `e8239210e962325db7ec740eff3e530f467fc88984b570860af4f614a095b153` |
| `legR.log` / `legR2.log` | `e3d21825...213c` / `75af23fc...8a88` |
| `bvh2.log` / `bvh_census.log` | `0cb91efd...d510` / `61b34b81...4e7a` |

`bvh.log` (first racediff pass) is superseded: it failed before any boot on the missing `.map` (2 rows NO-RESULT,
then stopped). Its `racediff_summary.tsv` rows were overwritten by `bvh2.log`'s pass (last run wins).

## Re-pickup for `0x0056cf90` / `0x0056fad0`

A racediff scenario where the original calls them inside the capture window. Both have leg R PASS on this build.

## Files

Committed: this RESULT, `drive_rgn.py`, `shadow_summary.tsv`, `racediff_summary.tsv`, `hooks.csv` (5 rows),
`re/analysis/CHANGELOG.md` (3 lines). Spawned game PIDs (all killed or self-exited, own only): 5640 (boot check),
19480, 20312, 20536, 2980 (shadow), the racediff `spawned_pids` in `racediff_summary.tsv`, gate arms 20328 / 5752 /
19032 / 7160 (self-exited). No watcher left running.
