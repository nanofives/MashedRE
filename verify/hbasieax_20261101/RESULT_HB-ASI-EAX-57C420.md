# RESULT HB-ASI-EAX-57C420: GREEN, 0x0057c420 and 0x0055bd80 C2 -> C3, gates equal refs

PREREG `PREREG_HB-ASI-EAX-57C420.md` committed alone at `ef08ec6c2`, before any arm. Branch `orch/HB-ASI-EAX-57C420`
off `orch/integration` @ `6d9d01a7f`, with `orch/D4-WSB-PORT-NARROW-TAIL` @ `6afb1ae11` merged in at `83660e8ad`
(its RESULT is this step's source and it was not yet in integration). Code commit `243488723`.

## What changed

- `FUN_0055bd80` / `FUN_0055bd80_impl` (`Collision/RwpSolverGlue5.cpp`) return `undefined4`, the return of the
  descriptor `+0x10` slot call (`0x0055bdca`). The `RwpVolFnApply` slot type and the wrapper (`return
  ShadowAB::RunTracked(...)`) follow. The header's note 1 said "FUN_0055bd80 is void". It now cites the call /
  `add esp,0x10` / `ret` at `0x0055bdca` / `0x0055bdcd` / `0x0055bdd4`.
- `FUN_0057c420` (`Collision/RwpVtableKV2.cpp`) returns `FUN_0055bd80`'s result (`0x0057c435..0x0057c43d`). On the
  `.asi` it is split into `FUN_0057c420_impl` plus an `.asi`-only `RunTracked` wrapper (`kPhaseAny`), the narrow-tail
  idiom of `0x0057c440`. The exe keeps the unwrapped body.
- The `extern` declarations in `RwpSolverCore18/21/22.cpp` and `RwpVtableKV2.cpp` match. No exe call site reads it.
- **No knob** (PREREG "Change"): the default value did not change, see S0 / S1.

## Static legs

`static_s0_s1.txt` (capstone over the built images and their `.map`, scripts `dis_asi.py` / `unarmed.py`).

| leg | binary | finding |
|---|---|---|
| S0 | narrow-tail `.asi` `343d8591` (before) | `_FUN_0057c420` ends `call _FUN_0055bd80` / `add esp,0x10` / `ret`. `_FUN_0055bd80` ends `call RunTracked` / `add esp,0x1c` / `ret`. `RunTracked<void>`'s un-armed path (`0x1006815a`) calls the impl, then writes only ECX / EDI / ESI / EBP / ESP before `ret` (the cookie check reads ECX). `FUN_0055bd80_impl` ends `call eax` (slot) / `add esp,0x10` / `pop esi` / cookie check / `ret`. So the un-armed default `.asi` already returned the slot's EAX through both ports. It was lost only on the armed `RunTracked` path |
| S1 | this step's `.asi` `056a6929` | Same instruction shape in `_FUN_0057c420` (now a wrapper), `FUN_0057c420_impl`, `_FUN_0055bd80`, `FUN_0055bd80_impl`. Both `RunTracked<undefined4>` un-armed paths (`0x100b04d0`, `0x10068930`) call the impl and do not write EAX after it. **PASS** |
| S1 | this step's exe `171d4d72` | `_FUN_0057c420`, `_FUN_0055bd80`, `FUN_0055bd80_impl` and the `RunTracked` un-armed path have the same instruction shape as the `.asi`. No exe caller reads the value |

The narrow-tail RESULT called the fix a default `.asi` behaviour change. S0 shows it is not one on the un-armed
default path. The value already reached `0x0055ab30` correctly there. The armed shadow path is the one that changes,
and it now returns the port's value.

## Shadow arms

Census scenario (track 0, mode 10, 4 cars, 60 s hold), `--hooks 0x<rva>` (hook installed alone), `MASHED_SHADOW_AB=1`,
`SAMPLES_DONE=24`, private root `D:/MashedWT/HB-ASI-EAX-57C420_root` with this step's `.asi` copied in only
(`original/mashed_re_dev.asi` not touched). `shadow_summary.tsv`, verdicts from `re/tools/shadow_ab_report.py`.

| arm | row | verdict | samples | return compared | proof | NOISY discarded |
|---|---|---|---|---|---|---|
| S-420 | `0x0057c420` | **CLEAN** | 24, ndiff 0 on every one | `ret=0` on all 24, `rv` `001afb1c` x12, `001afb50` x8, `001afb8c` x4 | `PATCHBYTE installed=e9 uninstalled=8b A/B-IS-REAL` | 4 |
| S-D80 | `0x0055bd80` | **CLEAN** | 24, ndiff 0 on every one | `ret=0` on all 24, `rv` `001afdc8` x12, `001afde8` x12 | `PATCHBYTE installed=e9 uninstalled=83 A/B-IS-REAL` | 15 |

- The returns are non-zero and equal between the original run and the port run on every sample, so the compare
  covered the value the narrow-tail held row was about.
- Both legs compared **0 heap pages** (`heap=o:0,n:0`, `pages=o:0,n:0`). The runner env did not set
  `MASHED_SHADOW_PRIVATE=1` (the PREREG fixed the runner as copied, without it). That is enough for C3. It is **not**
  a write-set leg for C4 (`re/CONFIDENCE.md` L-38).
- S-420's log also holds 24 `FUN_0055bd80` samples, all `ndiff=0`. Those come from the nested wrapper inside the
  port's run, with `0x0055bd80` not installed on that boot (`PATCHBYTE installed=83 uninstalled=83
  SUSPECT-no-restore`). Not evidence for `0x0055bd80`. S-D80 is.
- `0x0055bd80` re-arm (D4-WSB-C3-PORTED, 2026-10-27: n=1, then the game exited during race load 2/2 boots): this boot
  reached the running race and took 24 samples in about 4 s, with no exit. One boot, so it does not show what caused
  the 2026-10-27 exits. It is consistent with the armed path's lost EAX reaching `0x0055ab30`'s test
  (`0x0055ab69..0x0055ab6f`) through `0x0057c420`, but that link was not measured.

## Default gates on this build

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, test slot + solo lock,
`A4_ROOT=D:/MashedWT/HB-ASI-EAX-57C420_root`, exe `171d4d72` (built at `243488723`, `MASHED_NO_DEPLOY=1`, rva-lint
0 NEW). All six equal `re/orchestrator/merge_train.json`:

| arm | file | sha8 | ref |
|---|---|---|---|
| H_DEF | step.csv | `8284be46` | `8284be46` |
| H_DEF | gates.csv | `ba407a16` | `ba407a16` |
| H_DEF | ptrace.log | `31fe043e` | `31fe043e` |
| B_DEF_K0 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K1 | step.csv | `1d5bf4f9` | `1d5bf4f9` |
| B_DEF_K26 | step.csv | `1d5bf4f9` | `1d5bf4f9` |

## Run log and lock

- First arm launch (22:54Z): the driver queued a solo window behind other holders. L0 asked for no new solo window while
  a merge train was PRIORITY-PENDING, so I stopped my own driver before any launch. Killing its lock wrapper made the
  driver run its post-check with its endpoint probe killed too, which logged `post endpoints: <none>` / `BLOCKED
  audio endpoint (dropped during the run)` at 23:05:41Z. No game was launched in that attempt. A direct check right
  after listed 3 VB-Cable endpoints. Not an audio finding.
- `drive_hbe.py` then got a train check (`7ac8beb19`, before the arms): it waits while a merge-train PRIORITY-PENDING
  shows and, if the flag appears inside its window, releases without launching.
- S-420 ran in the window opened 20:49:28 local (before the 20:57:50 train flag), per the L0 rule that an open window
  may finish. The driver then queued S-D80's window 2 s after the train took the lock. On the L0 request I withdrew it
  (my driver and lock-wrapper PIDs only, no game), and re-ran S-D80 after train `20261101_205241` had merged
  (00:23:58Z).
- Audio check around both arm windows: endpoints present before and after, 0 MASHED Event-1000 events, no fault at
  `0x005baf48`, no U-9677 signature.

## Tracker transaction (re-classify)

- `hooks.csv`: `0x0057c420` C2 `mapped` -> **C3 `impl`**, `0x0055bd80` C2 `ported` -> **C3 `impl`**, `file` = the port,
  `scenario` `shadow_tracked:track0-mode10-cars4`, `frida_diff` `shadow_ab:verify/hbasieax_20261101/shadow_summary.tsv`.
  - `0x0057c420`: callers `0x0047d180` C2, `0x0047d240` C2, `0x00480100` C2. Callee `0x0055bd80` C2 (C3 here).
  - `0x0055bd80`: caller `0x0057c420` (C3 here), callee `0x004c4600` `RwMatrixMultiply` C3.
- The `0x0055bd80` plate (`re/analysis/bucket_00557fb0/0x0055bd80.md`) held one unfiled `[UNCERTAIN]`, "Caller of
  0x0055bd70 with conditional matrix relativization". Filed as U-9800 and resolved here: `0x0055bd80` does not call
  `0x0055bd70`. Both dispatch the shape descriptor's `+0x10` slot, `0x0055bd70` as an argument-forwarding tail JMP
  (`Collision/RwpSolverBroadphase3.cpp` note 3, `0x0055bd70..0x0055bd7b`), `0x0055bd80` as a CALL after the
  optional `0x004c4600` combine (gate `0x0055bd8b..0x0055bd98`, call `0x0055bdca`). It returns that call's EAX
  (S0 / S1, S-D80 `ret=0` x24).
- `STUBS.md` S-3734 (`0x0057c420` called as a passthrough from `0x0047d240`'s port) sits in the caller's body and is
  not touched. No new S rows.
- No C4. Neither row has a race diff with the hook installed, and neither leg compared heap pages.

## Next

- C4 for both needs a race diff with the hook installed and a heap-tracked shadow leg (`MASHED_SHADOW_PRIVATE=1`)
  with more than 0 heap pages compared, or a scenario where the sampled calls write compared memory (L-40).

## Files

Committed: `PREREG_HB-ASI-EAX-57C420.md`, this RESULT, `shadow_summary.tsv`, `static_s0_s1.txt`, `dis_asi.py`,
`unarmed.py`, `run_shadow11.py` (copy of `verify/d4wsb_portnarrowtail_20261101/run_shadow10.py`, data path and label
changed), `drive_hbe.py` (copy of `drive_nt.py`, runner name, data path and label changed, plus the train check).

Raw (not committed), `D:/MashedWT/HB-ASI-EAX-57C420_data/`:

| file | SHA-256 |
|---|---|
| `0057c420/shadow_ab.log` | `e836f5d50b97456643817f84f8030fc5e43ac9312f233ec6c2ce78affad914d9` |
| `0057c420/shadow_transcript.txt` | `2626c034a4dbd6eff10fefe4c1fdaa436c0150f113d8ed67bd079636e6ac102f` |
| `0057c420/shadow_report.tsv` | `77d9a3462bda5d947feed68925c0292aea7b7ced3595c5fbed0e000ca5316c5e` |
| `0055bd80/shadow_ab.log` | `7e275722e70fc2f0f2bb37a8c12969e8b6fc714ced9f8fa1828e997e7c73e6f9` |
| `0055bd80/shadow_transcript.txt` | `084abf1161f71a443aea3fc5aee58ea64a4a891001a7baf021df837a6103ca7d` |
| `0055bd80/shadow_report.tsv` | `548d53f8c67f068b40dd684e90796a7ab2632af5cd39cbc9dc59a6b4950caf32` |
| `drive_hbe.log` | `5de87c076c2ad272079f5c194a263e18948c3c38fcfb3a26aa7d6b004ef7b02d` |
| `arm_s.out` | `f4f0acb3d01a8df0c02016b60dc8d4b060e3835ab941d922817fbec15303ad99` |
| `build.log` | `ae9d9d4f5f0b97dd11d25480e6e74ae91e8b5152d48d9911935792ba3b1e4990` |
| `gates.out` | `8c7501d2ee83d6c92ce36900faa58f6f178ef52a380ea46fee8c2bc870438737` |
| `gates/H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `gates/H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `gates/H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `gates/B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `gates/RUNLOG.txt` | `0ac47ddc19f42d735e6bdf7f4635ed2b32724e3864c458353f4ffbcfd2946335` |

Game PIDs spawned (each killed only by PID by the runner, or self-exited): S-420 18608, S-D80 18508, gates 22992,
24520, 16080, 24172 (self-exited). None alive at the end. No watcher left running.
