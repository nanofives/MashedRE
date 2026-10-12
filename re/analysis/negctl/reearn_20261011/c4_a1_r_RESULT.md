# C4 re-earn, triage A1 group R: Race Results stat-page getters (canonical scenario, hooks live)

Rows: PlayerBlock2Field00Get `0x00423ee0`, Field04Get `0x00423f60`, Field50Get `0x00423ff0`, Field08Get `0x00424070`.

| row | verdict |
|---|---|
| `0x00424070` PlayerBlock2Field08Get | **C4 EARNED** (natural data, no forced state) |
| `0x00423ff0` PlayerBlock2Field50Get | **C4 EARNED** (natural data, no forced state) |
| `0x00423ee0` PlayerBlock2Field00Get | **C4 EARNED with a stated forced state** (input field values poked, §5). On natural data alone: NOT EARNED, the control cannot be distinguished |
| `0x00423f60` PlayerBlock2Field04Get | **C4 EARNED with a stated forced state** (input field values poked, §5). On natural data alone: NOT EARNED, the control cannot be distinguished |

Evidence only. No tracker edit and no re-classify in this branch. The parent promotes, and the f00/f04 forced-state
call is the parent's to accept or refuse.

Date 2026-10-12, branch `work/c4a1-r` (base `225236b5`). Anchor `original/MASHED.exe.unpatched` SHA-256
`BDCAE093...3C0E` (matches CLAUDE.md). `original/MASHED.exe` is the ten-patch boot recipe (`B9977DAB...FE87`), no
unlock patch. Port under test: the deployed `original/mashed_re_dev.asi` (SHA-256 `C5594D6B...3840`, never
overwritten). Static check: its shared body (`.asi+0x1e390`) reads `shl eax,4` / `[eax+0x7f1a18]` and compares
`0x7f1a18/28/38/48`, i.e. it carries 2e3cfead.

## 1. Why neither boot-to-menu nor E's Quick Battle route can call these bodies

Call sites (E8 rel32 scan of `.text`, unpatched image):

| getter | FUN_00424270 (stat sort) | FUN_0042c510 (page advance) | FUN_0042c960 (draw build) |
|---|---|---|---|
| f00 `0x00423ee0` | `0x0042457c` | `0x0042c766` | `0x0042cf31`, `0x0042cf3f` |
| f04 `0x00423f60` | `0x004245c2` | `0x0042c736` | `0x0042cbce` |
| f50 `0x00423ff0` | `0x0042460c` | `0x0042c776` | `0x0042cf61`, `0x0042cf6f` |
| f08 `0x00424070` | `0x0042464f` | `0x0042c746` | `0x0042cbe1` |

FUN_00424270 is called only from FUN_0042c960 (`0x0042caa2`, `0x0042cda8`). FUN_0042c960's only caller is
`0x0043c2d9`. FUN_0042c510 is called at `0x0042c9bf` (the `0x0067ec28` flag path) and `0x0042d01b` (scroll).
All of it is the Race Results stat page.

The getters serve stat types `0xb` (f04), `0xd` (f08), `0xa` (f00), `0xc` (f50) (the `cmp [ebx],N` ladder at
`0x0042c730..0x0042c776`). Those types live on pages 9, 10, 12, 13 of the page table `0x005f7854` (`0x005f77c8`,
`0x005f77e4`, `0x005f781c`, `0x005f7838`). FUN_0042c510 skips pages 7..13 (`0x0042c5f3..0x0042c63a`) unless the
game mode `[0x0067e9fc]` is 6 or 2 (`0x0042c5e9`, `0x0042c5ee`). It also skips a page whose four-player sum is 0
(`test edi,edi` at `0x0042c796`). E's Quick Battle route runs mode 10, so these pages are never visited there.
At the main menu none of the callers runs. The old C4 evidence (boot-to-menu install-observe) therefore never
executed these bodies.

The team branch inside each getter is gated on `FUN_0042f500` = `[0x0067ea64]` (`0x00423ee0`), which only Team Play
sets (action `0xff2e0000`, U-9078). f04 and f08 also return 0 when `FUN_0042f6a0` = `[0x0067e9fc]` == 4
(`0x00423f65`, `0x00424075`). The old defect (team-id stride 4) can only show with the team flag set.

Page messages (Defines.txt names): page 9 = 119/154 "kills overall" (f04), page 10 = 150/151 "unforced errors
overall" (f08), page 12 = 152/153 "aggressive overall" (f00), page 13 = 155/156 "cleanest driving line overall" (f50).

## 2. Scenario (new harness `re/frida/c4_pb2_scenario.py`, reuses statenav's Nav and FUN_00497310 override)

1. Title -> GTS (screen 1). The async Load-Successful modal swallows input: a DOWN that leaves the cursor at 0 means
   the modal is up, so one confirm closes it, then DOWN again. Cursor 1 = Multi Player (msg `0x22`) -> screen 3.
2. Screen 3 cursor 1 = Team Play (msg `0x140`, action `0xff2e0000`) -> screen 4. Measured `[0x0067e9fc] = 6`,
   `[0x0067ea64] = 1` from here on, in every run.
3. Screen 4 (Player Colour Select). Its confirm arm `0xff1d0000` (`0x0043e557`) runs FUN_0042b9e0, which needs at
   least 2 profiles with a colour choice. FUN_004332a0 moves each profile's choice `0x0067eaf0 + 12*p` on its own
   column 1 (`0x00433306`/`0x00433315`), cooked from control 10 (`0x00496655..0x00496664`). Profiles 0/1/2 press
   RIGHT 1/2/3 times: choices `[1,2,3,0]`. Confirm -> screen 16 (Team Select, `0x0043e5f8` when ea64 != 0).
4. Screen 16. FUN_0042fa00 steps `0x0067e938 + 12*p` (0 none, 1 team A, 2 team B) on column 1. RIGHT 1/2/1:
   picks `[1,2,1,0]`. Confirm -> 15 -> 6 -> 18 -> race (player-0 confirms only).
5. Race. Profile 0 holds accelerate (control 0 -> `0x007f103c`, `Ai/AiController.cpp:170`), profiles 0..2 get
   E's 1 Hz control-4 pulse while `0x00771968 == 3`. Profiles 1 and 2 do not drive. The default round-over test
   (`0x00410ee3..`) ends rounds on car separation, so car 0 driving away ends the match.
6. Race Results reached naturally when `0x0067eca4 == 3`, `TYPE(depth) == 5`, `0x00771968 != 3`: 10 of 11 final runs,
   30..35 s after the race start (ctl1 crashed in track load, §6). No push, no poke to get there.
7. Idle 3 s, then 18 action-7 presses for player 0 (the E override), 2.5 s apart. Presses 0..8 = phase **nat**.
   Before press 9: phase **poke** (§5).

Live team records at Race Results, identical in all 10 runs: `0x007f1a14..` = `{0,0,0,0} {1,1,1,0} {2,0,2,0}
{-1,-1,0,0}`. Team ids at the 0x10 stride = `[0,1,0,-1]` (2v1). The stride-4 read gives `[0,0,0,1]`.

Recipes that did NOT end a match (logs in `evidence/c4_pb2/explore/`): player 0 pulsing control 4 only (420 s),
all three pulsing control 4 (480 s), profiles 0 and 1 both accelerating with steering (600 s), profile 0 zig-zagging
(2 x 565 s). Screen 18's Extra Opponents row is not selectable in this setup (cursor 0 -> 2 -> 4).

## 3. Arms

| arm | setup | first byte at the 4 RVAs, pre-attach | getters instrumented at |
|---|---|---|---|
| `off` | shim loads deployed `.asi`, `MASHED_HOOK_ONLY=__none__` | `0xE8` (stock `CALL 0x0042f500` / `0x0042f6a0`) | original entries |
| `on` | shim loads deployed `.asi`, `MASHED_HOOK_ONLY=` the 4 RVAs | `0xE9` | E9 targets |
| `stock` | `MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded | `0xE8` | original entries |
| `late` | NO_AUTO_HOOK, then `Module.load(original/mashed_re_dev.asi)` + `InjectHooks` at the title, HOOK_ONLY = 4 RVAs | `0xE9` | E9 targets |
| `ctl` | as `late`, with the control `.asi` | `0xE9` | E9 targets |

**Hook-live witness.** On every `on`/`late` run the rel32 at each RVA resolved into `mashed_re_dev.asi` at
`+0x1e420` (f00), `+0x1e450` (f04), `+0x1e4d0` (f50), `+0x1e490` (f08), which are exactly the four export RVAs of
that file. On `ctl` it resolved into `ctl_stride4.asi` at `+0x1e410/+0x1e440/+0x1e4c0/+0x1e480`, its export RVAs.
The byte is read before Frida attaches anything (after attach the off/stock arms also show `0xE9`, Frida's own
trampoline). Every call counted on those arms was counted at that reimpl entry, so each is a game call through the
inline JMP.

**Control** `.asi` (SHA-256 `7D6BDB0E...7721`): `k_TeamIdStride = 4u` restored alone in
`Frontend/MenuLeaves_s3.cpp` at `225236b5`, built with `MASHED_NO_DEPLOY=1`, copied out, source restored with
`git checkout`. Diff: `evidence/c4_pb2/c4_pb2_ctl_patch.diff`. Static: its body reads `[ecx*4+0x7f1a18]` and
compares `0x7f1a18/1c/20/24`. The control was never deployed.

## 4. Observables and the original's own noise

Per getter call, at entry: param, return address, and every input the bytes read (`0x0067e9fc`, `0x0067ea64`,
team dwords at both strides, the 4 field dwords). At leave: the return. Two byte models on the same snapshot:
`m_orig` (the bytes `0x00423ee0..0x00423f5e` and the three siblings) and `m_old` (stride 4).

- `mmOrig` / `mmOld`: calls whose return differs from each model. `disc`: calls where the two models differ, i.e.
  the calls that can tell the fix from the defect.
- Consumer, pass-through: FUN_0042c300 `(value,row)` from `0x0042cbfa` (f04/f08) and FUN_0042c3c0
  `(ref,own,row,flag)` from `0x0042cf95` (f00/f50) take exactly the getter returns that fed them.
- Consumer vs original (`corig`): those draw-call value args compared with `m_orig` of the feeding calls.
- Cross-run (`xrun`): the ordered game-call sequence `(getter, param, return address, return)` per phase, compared
  with a setup-matched reference run, plus the page walk.

**Original noise, measured** (off1, off2, stock1, stock2, plus explore7): everything repeats except one value,
f50 of player 0 (`0x00899f70`, "cleanest driving line"): 28, 22, 22, 22, 25. Team records, f08 `[0,4,4,0]`,
f50 players 1..3 = 0, f00 = f04 = all 0, the page walk, and the call counts per site are identical in every run.
The cross-run channel therefore writes a return equal to the run's own f50[0] as `F` before comparing. Two original
runs then match exactly (off2 vs off1 0 mismatches, stock2 vs stock1 0 mismatches, every getter, both phases).

## 5. The forced state (phase poke) and why it is needed for f00/f04 only

In all 10 natural runs f00 ("aggressive") and f04 ("kills") are 0 for every player. A sum of zeros is the same at
either stride, so on natural data these two getters cannot tell the fix from the defect (`disc = 0`), and their
draw pages 12 and 9 are skipped by the zero-sum test at `0x0042c796`.

Phase poke writes `[3,5,7,11]` to `0x00899f20 + k*0x138` (f00) and `[2,6,1,9]` to `0x00899f24 + k*0x138` (f04),
after press 8, on the already-reached Race Results screen. Previous values recorded: all 0. Nothing else is written,
and f50/f08 stay natural. What it does not change: the screen (reached naturally), the call sites (the game's own),
the team data, the mode and team flag, and the getter's instruction path. The bodies have no branch on field values,
and the team compares read the natural team records. What it does change: the values summed, and therefore which
pages pass the zero-sum test. Pages 9 and 12 become visited, which adds the FUN_00424270 sort calls and the
FUN_0042c960 draw calls for f04/f00. That caller-side coverage exists only under the poke, which is why f00/f04
are reported as forced-state C4s.

## 6. Results

Every final run's per-phase call counts and call sites are identical across all arms:

| getter | phase nat: calls (sites) | phase poke: calls (sites) | disc nat / poke |
|---|---|---|---|
| f00 `0x00423ee0` | 4 (`0x0042c76b` x4) | 11 (`0x0042c76b` x4, `0x00424581` x3, `0x0042cf36` x2, `0x0042cf44` x2) | 0 / 10 |
| f04 `0x00423f60` | 4 (`0x0042c73b` x4) | 9 (`0x0042c73b` x4, `0x004245c7` x3, `0x0042cbd3` x2) | 0 / 8 |
| f50 `0x00423ff0` | 11 (`0x0042c77b` x4, `0x00424611` x3, `0x0042cf66` x2, `0x0042cf74` x2) | 11 (same) | 5 / 5 |
| f08 `0x00424070` | 9 (`0x0042c74b` x4, `0x00424654` x3, `0x0042cbe6` x2) | 9 (same) | 8 / 8 |

Per arm (values identical for every run of that arm; full rows in `evidence/c4_pb2/c4_pb2_compare_ref{off1,stock1}.csv`):

| arm (runs) | ref | f08 nat | f50 nat | f00 poke | f04 poke | f08/f50 poke | crash |
|---|---|---|---|---|---|---|---|
| off (off2) | off1 | mmOrig 0, xrun 0/9 | mmOrig 0, xrun 0/11 | mmOrig 0, xrun 0/11 | mmOrig 0, xrun 0/9 | 0 / 0 | 0 |
| stock (stock2) | stock1 | mmOrig 0, xrun 0/9 | mmOrig 0, xrun 0/11 | mmOrig 0, xrun 0/11 | mmOrig 0, xrun 0/9 | 0 / 0 | 0 |
| **on** (on1, on2) | off1 | **GREEN**: mmOrig 0, corig 2/2, xrun 0/9 | **GREEN**: mmOrig 0, corig 2/2, xrun 0/11 | **GREEN**: mmOrig 0, corig 2/2, xrun 0/11 | **GREEN**: mmOrig 0, corig 2/2, xrun 0/9 | GREEN | 0 |
| **late** (late1, late2) | stock1 | **GREEN** (same numbers) | **GREEN** | **GREEN** | **GREEN** | GREEN | 0 |
| **ctl** (ctl2, ctl3) | stock1 | **RED**: mmOrig 8/9, mmOld 0, corig 0/2, xrun 8/9 | **RED**: mmOrig 5/11, mmOld 0, corig 0/2, xrun 5/11 | **RED**: mmOrig 10/11, corig 0/2, xrun 10/11 | **RED**: mmOrig 8/9, corig 0/2, xrun 8/9 | RED | 0 |

On natural data f00/f04 read `NON-DISCRIMINATING` on every arm, the control included (4 calls each, all returns 0
on both sides).

The control fails exactly on the discriminating calls (`mmOrig == disc` and `mmOld == 0` on every getter), and the
wrong value reaches the screen. Draw-call value args at the same rows (stock1, stock2, off2 and on2 all draw the same, ctl2/ctl3 differ):
unforced errors (f08, rows 129/130) `4, 4` vs control `8, 8`. Kills (f04, poke, rows 129/130) `3, 6` vs `9, 9`.
Driving line (f50, rows 142/143) `(0, 0), (0, F)` vs `(F, F), (F, F)` (F = that run's f50[0]). The page walk is the same on all arms (sums stay
non-zero at either stride), so the page-advance consumer does not discriminate here. The draw args do. The `late`
arms use the control's load path and are GREEN, so the RED is the stride, not the load mechanism.

**Byte model validated** on the original arms: `mmOrig = 0` on every call of off1, off2, stock1, stock2 (and
explore7), including 5..16 discriminating calls per getter per run. So `m_orig` is the original's behaviour on these
inputs, and `m_old` is what the defect would return.

**Survival.** Every final `off`/`on`/`stock`/`late` run and ctl2/ctl3 reached the end alive. The exit code
`0xDEADBEEF` is the harness's `dev.kill`. ctl1 died during track load (session phase 2, before any getter call,
0 calls) with an AV at `MASHED.exe+0xe66db` (read @ `0x8`). ctl2 and ctl3 ran the same arm cleanly. Single
observation, before any hooked body ran, recorded and not attributed.

## 7. C4 checklist per row

| | f08 `0x00424070` | f50 `0x00423ff0` | f00 `0x00423ee0` | f04 `0x00423f60` |
|---|---|---|---|---|
| 1. game calls the body on a naturally reached screen | yes | yes | yes (4 calls/run natural) | yes (4 calls/run natural) |
| 2. hook live: E9 pre-attach, target = export, call count > 0 | yes, 18/run (9 nat + 9 poke), 4 hook-live runs | yes, 22/run (11 + 11) | yes, 15/run (4 + 11) | yes, 13/run (4 + 9) |
| 3. behavioural comparison, hook-live vs original, same scenario | GREEN, natural | GREEN, natural | GREEN under the stated poke | GREEN under the stated poke |
| 4. control RED on the same scenario | RED 2/2 runs, natural | RED 2/2 runs, natural | RED 2/2 only under the poke. Natural: not distinguishable | RED 2/2 only under the poke. Natural: not distinguishable |
| 5. survival, both arms | all 8 port/original runs and ctl2/ctl3 alive. ctl1 load-time AV recorded (§6) | same | same | same |

## 8. Side findings (not acted on)

- **Original AV with Vehicle = 1 in a Multi Player Team Play setup** (explore5, stock arm, nothing loaded): a blind
  cursor landed on screen 18 row 4 and set `0x0067ea94` 0 -> 1. The stock original then AV'd at race start,
  `MASHED.exe+0x1509b4` (read @ `0x38`). Single observation, not re-run.
- **A human-only Multi Player match needs a car to drive away.** Only the profiles that pick a colour get car slots
  (`0x007f1a14` slot 3 = -1), and the round-over test is separation-based. A future multi-player harness should
  hold accelerate (control 0) on one profile, not pulse control 4.
- The colour-choice reset value for unused profiles was -1 in one run and 0 in the others (`0x0067eaf0 + 36`,
  explore3 vs final runs). It does not matter here, but it is the `[UNCERTAIN]` EBP reset value d11057 left open.

## Files

- harness: `re/frida/c4_pb2_scenario.py`, `re/frida/c4_pb2_compare.py`, `re/frida/c4_pb2_pair.sh`
- per run: `evidence/c4_pb2/c4_pb2_{off,on,stock,late}{1,2}.{json,txt}`, `c4_pb2_ctl{1,2,3}.{json,txt}`
- comparisons: `evidence/c4_pb2/c4_pb2_compare_refoff1.csv`, `..._refstock1.csv`
- control: `evidence/c4_pb2/c4_pb2_ctl_patch.diff`
- route exploration (failed recipes, nav fixes): `evidence/c4_pb2/explore/`
- off1/on1 predate the per-phase counters. Their phases are rebuilt from the per-call events, split at the first
  call that carries the poked values (rows marked `nat*` / `poke*` in the CSV). All later runs use the
  in-process split.
