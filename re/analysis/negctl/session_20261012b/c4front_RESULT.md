# C4-FRONT: canonical C4 scenarios for four C3 rows (2026-10-12)

Branch `work/c4front` (base `8da9ecf2`). Evidence only: no tracker edit, no re-classify. The parent promotes.

| row | verdict |
|---|---|
| 0x00492270 SubsystemInit | **C4 EARNED** (boot, game-driven, 2/2 port GREEN, original 2/2 identical, control RED 2/2) |
| 0x004026d0 FUN_004026d0 | **C4 EARNED on the evidence**, with one tracker point for the parent: five live `passthrough` STUBS rows name it as caller (section 6) |
| 0x0042f8d0 MenuMenusBC | **C4 EARNED** (natural Race Results, 4 call sites, 2/2 port GREEN against the pooled original, control RED 2/2 on every call) |
| 0x00404320 PerModeRenderMachine | **NEEDS USER DECISION.** Stays C3. Its arms run only on rules 5/8/9/10, and the stock save cannot launch any of them. Exact forced input in section 5 |

Anchors: `original/MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E` (matches CLAUDE.md). `original/MASHED.exe` is the
ten-patch boot recipe `B9977DAB70607720...`, no unlock patch. Every run spawned its own PID and was killed by that PID.

## 1. Harness

New: `re/frida/c4_front_scenario.py` (+ `re/frida/c4_front_compare.py`). It reuses `statenav.Nav`, the Quick Battle
order and the `FUN_00497310` return override from `c4_detect_e_scenario.py` unchanged. Why the existing harnesses did not
fit as they stand:

- Their `on` arm uses the dinput8 shim's autoload, which loads `original\mashed_re_dev.asi` (the shared deployed build),
  not this worktree's build. Their `late`/`ctl` arm loads at the title, after both boot rows have already run.
- So this harness sets `MASHED_RE_NO_AUTO_HOOK=1` and `Module.load`s the given `.asi` **while the process is still
  suspended** (before MASHED's first instruction). It then calls its `InjectHooks` export with `MASHED_HOOK_ONLY=<rva>`,
  so exactly one row is live, and resumes. Boot code then runs through the installed JMP.

Arms (all three use the same load path; only the `.asi` differs):

| arm | `.asi` | SHA-256 | `MASHED_HOOK_ONLY` |
|---|---|---|---|
| `stock` | none | - | `__none__` |
| `port` | this worktree's build at `8da9ecf2`, `MASHED_NO_DEPLOY=1` | `58D9A4AB2908975E...9ADC` | the row's RVA |
| `ctl` | the same tree with the three old-defect bodies (`c4front_evidence/ctl_patch.diff`) | `1B5CE54027CB9FC8...11C7` | the row's RVA |

The control `.asi` holds all three old bodies at once. That is safe because `MASHED_HOOK_ONLY` installs only the row
under test, and the other two bodies are never reached. The controls:

- SubsystemInit: `Boot/SubsystemInit.cpp` as at `9cde82cb~1`. It calls this TU's own `DisplayInit()` / `ViewportInit()`
  instead of 0x004921d0 / 0x00428590. This is the a2-stubat witness.
- 0x004026d0: `Boot/BootLowRvaCluster.cpp` as at `9cde82cb~1`. 0x004c1bb0 and 0x004c1be0 are called with 1 argument.
  This is the a2-stubat witness.
- MenuMenusBC: `mov bl, al` at `0x0042f8d9` replaced by `mov bl, 0`. This is the a2-dialog witness 1 ("AL ignored").
  MenuMenusBC was not changed by a2-dialog, so its pre-fix bodies date from 2026-05 and were not re-built.

Both controls were built with `MASHED_NO_DEPLOY=1`, copied out, and the sources restored with `git checkout`
(`git status` clean afterwards). Nothing was deployed into `original\`, and nothing was written under it.

**Hook-live witness** (memory hook-live-witness-snap-the-rva-dword): on every `port`/`ctl` run, the 8 bytes at the RVA
are read right after `InjectHooks` and before any Interceptor is attached. The rel32 target is then resolved to module +
offset, plus the export whose address equals it. **Game call count:** an Interceptor on that target (stock: on the
original entry) counts the game's own calls. No arm calls an export directly.

## 2. 0x00492270 SubsystemInit (boot)

**Scenario.** Spawn, resume, wait for the title (`0x0067eca4 == 3`, depth ≥ 1), then 5 s. The game calls 0x00492270
once, from `FUN_00492370` (return address `0x004923ad`, the `call` after `FUN_004924f0`), before
`AppInitialiseOnBootup`.

**Observable.** This is the row's whole output, as the a2-stubat fixture defines it. It is the ordered calls into
0x00493710 (arg + return), 0x004921d0 and 0x00428590 made while inside the row, plus the row's return. Each callee is
Interceptor-attached at its original RVA, and calls are counted only on the thread that is inside the row. The callee
hooks detach once the row returns.

| run | first bytes at 0x00492270 | target | game calls | log | vs stock1 |
|---|---|---|---|---|---|
| stock1 | `6a 00 e8 99 14 00 00 83` | original | 1 | `rwinit(0)=1; display(); viewport(); ret 1` | ref |
| stock2 | same | original | 1 | same | GREEN (original repeat) |
| **port1** | `e9 1b 5a 87 67 00 00 83` | `mashed_re_dev.asi+0x17c90` = export `SubsystemInit` | **1** | same | **GREEN** |
| **port2** | `e9 1b 5a d3 66 ...` | `+0x17c90` `SubsystemInit` | **1** | same | **GREEN** |
| ctl1 | `e9 1b 5a 87 67 ...` | `+0x17c90` `SubsystemInit` | 1 | `rwinit(0)=1; ret 1` | **RED** (3 of 5 entries) |
| ctl2 | same | same | 1 | same | **RED** |

The original's noise is none: both stock runs give the identical 5-entry log. The control departs exactly as the
a2-stubat witness predicts. The calls to 0x004921d0 / 0x00428590 vanish because the old body ran its own C++ bodies. All
6 runs reached the title alive, with 0 AVs (exit `0xDEADBEEF` is the harness's kill).

Coverage, stated plainly: the game runs only the `iVar1 != 0` path (0x00493710 returned 1 on all six runs). The
return-0 arm (`0x0049227e ret`) is covered only by the C3 fixture (return 0 vector).

## 3. 0x004026d0 FUN_004026d0 (boot)

**Scenario.** Same boot. The game calls `FUN_004026d0(4)` once, at the tail of `FUN_00402750` (return address
`0x00402a14`).

**Observable.** The ordered calls made while inside the row:

- 0x004671a0 (arg 0 + return)
- 0x004c1bb0 (3 args; arg 1 dereferenced)
- 0x004c1a00 (arg + return)
- 0x004c19f0 (arg)
- 0x004c1be0 (3 args)

The camera pointer is tokenised (`P0`), because it is a per-boot heap address. 0x004c1a00's return is reduced to
zero/non-zero, because it is a pointer that differs between the two stock boots (`0x460f2dc` vs `0x469f2dc`).

stock1 log (34 entries, 4 identical iterations):
`cam(0)=P0; clear(P0,@ff000000,3); cam(0)=P0; begin(P0)=NZ; cam(0)=P0; end(P0); cam(0)=P0; show(P0,0,1)` x4.

| run | first bytes | target | game calls | vs stock1 |
|---|---|---|---|---|
| stock2 | `51 56 8b 74 24 0c 85 f6` | original | 1 | GREEN 34/34 (original repeat) |
| **port1** | `e9 8b eb 90 67 0c 85 f6` | `mashed_re_dev.asi+0x21260` = export `BootQueueFlush` | **1** | **GREEN 34/34** |
| **port2** | same | same | **1** | **GREEN 34/34** |
| ctl1 | `e9 8b eb c1 68 ...` | `+0x21260` `BootQueueFlush` | 1 | **RED 8/34**: `clear(P0,AV,1afde8)`, `show(P0,0,0)` |
| ctl2 | same | same | 1 | **RED 8/34**, same entries |

The control is RED on exactly the 8 calls the a2-stubat witness names. 0x004c1bb0 gets stack residue for
`&local_4` (unreadable) and for the mode (`0x1afde8` instead of 3), and 0x004c1be0 gets `(…,0,0)` instead of `(…,0,1)`.
The port passes `&local_4` holding `0xff000000` and mode 3, as the original does. All 6 runs reached the title alive,
with 0 AVs.

Coverage: n = 4 and 0x004c1a00 non-zero on every iteration. The `n <= 0` early-out (`0x004026ec jle`) and the
BeginUpdate-fail branch are covered only by the C3 fixture.

## 4. 0x0042f8d0 MenuMenusBC (natural Race Results)

**Scenario.**

1. The statenav Quick Battle route, verbatim from `c4_detect_e_scenario.py`: title → Game Type Select → Load modal →
   Single Player → down → Quick Battle → colour → track → start.
2. In the race, pulse control 4 (player 0, 200 ms, 1 Hz) only while `0x00771968 == 3`. This is the detect_e /
   scenario_launch in-race recipe.
3. Wait for `0x0067eca4 == 3` and `TYPE(depth) == 5` (`0x0067ed3c + depth*0x40`). That is Race Results, entered
   naturally on all 12 runs (both rounds), with no push and no poke.
4. Settle 4 s. Then open a 2.0 s window: an Interceptor on 0x00472c60 records the 5 stack args of every call made while
   the current thread is inside 0x0042f8d0. Close the window, then idle 3 s.

**Observable.** It is the row's whole output, as the a2-dialog fixture defines it: the five 0x00472c60 calls (x, y, w,
h, colour each).

- They are keyed per call by the row's inputs: the 4 stack floats, AL (the alpha, `0x0042f8d9 mov bl,al`), the screen
  type, and the return address.
- The return address is the same on every arm: the JMP sits in the callee, so it is always the original caller's
  `call` + 5.
- Only calls that start and end inside the window are recorded (`edge` = 0 on every round-2 run).

**Where the game calls it.** Every one of the ~2,900 calls per run was made on screen type 5. 0 calls came from the
title, the menus or the race. All came from `FUN_00434720`, at four sites:

- return `0x0043493d` and `0x004361c0` and `0x0043667b`: 1 call/frame each
- return `0x00434ef3`: 2 calls/frame

The 2026-09-05 note names the same four (`0x00434938`, `0x00434eee`, `0x004361bb`, `0x00436676`). AL = `0xff` on every
call.

The original's output is fixed per input. The 5 keys give 5 output tuples, for example (site `0x0043493d`):
input `(350,100,250,26)`, output
`(350,100,250,26, 0x7f146ef0) (348,100,2,26, 0xff1050b4) (348,100,254,2, ..) (348,124,254,2, ..) (600,100,2,26, ..)`.
The fill is `0x{ff>>1}146ef0` and the border `0x{ff}1050b4`, as the a2-dialog bytes say.

**The original's own noise: one INPUT, not an output.** The plate at `0x0043667b` gets width `0x42b6a83c` (91.33) on
some boots and `0x43386c97` (184.42) on others. It follows the race outcome, which does not reproduce (memory
round-level-outcomes-do-not-reproduce): stock1 had 184.42 and stock2 had 91.33. For a given width, both originals give
one fixed output. The reference is therefore the pooled original (stock1 + stock2). A port call whose key is in the pool
must give the pool's output for that key. A call whose key is not in the pool would be "unmatched", and there were 0
such calls on any port or control run.

| run | first bytes at 0x0042f8d0 | target | game calls (whole run) | window calls / quads | vs pooled original |
|---|---|---|---|---|---|
| stock1 | `83 ec 0c 53 55 8b 6c 24` | original | 2890 | 590 / 2950 | ref |
| stock2 | same | original | 2910 | 595 / 2975 | vs stock1 alone: 476 matched, **0 mismatched**, 119 unmatched (the other width) |
| **port1** | `e9 db 65 c1 68 8b 6c 24` | `mashed_re_dev.asi+0x15eb0` = export `MenuMenusBC` | **2855** | 590 / 2950 | **GREEN 590/590**, 0 unmatched (width 91.33, = stock2's output) |
| **port2** | same | same | **2860** | 600 / 3000 | **GREEN 600/600**, 0 unmatched (width 184.42, = stock1's output) |
| ctl1 | `e9 db 65 3f 67 ...` | `+0x15eb0` `MenuMenusBC` | 2900 | 600 / 3000 | **RED 600/600** |
| ctl2 | `e9 db 65 c1 68 ...` | same | 2885 | 590 / 2950 | **RED 590/590** |

The control departs on every call, exactly where the a2-dialog witness predicts. The fill is `0x00146ef0` and the
border `0x001050b4` (alpha 0) where the original has `0x7f146ef0` / `0xff1050b4`. The geometry is identical. The port
matches all 25 dwords of all five calls on both runs, including the call-5 values that the 2026-05 body got wrong:
x = (w+(x-BX))+BX, y = (y+(h-BX))-(h-BX), and the `fst` at `0x0042f98d`. All 6 runs alive at the end, 0 AVs.

**Round 1 (superseded, kept).** The first six runs used a window that recorded a call still in flight when the
0x00472c60 hook was detached. port1 had one call with 1 quad instead of 5, and that quad matched the original's first
quad bit for bit. Round 1 is otherwise the same picture: stock2 / port2 GREEN against stock1, controls RED 550/550 and
595/595, all 6 runs at width 91.33. A quad-count filter would also hide a control that drops quads, so the harness was
fixed (only calls that both start and end in the window are recorded) and all six runs were repeated. Those repeats are
the table above. Round-1 files and its compare CSV: `c4front_evidence/bc_round1_superseded/`.

Coverage, stated plainly: alpha `0xff` only. The body has no branch (95 instructions, no jcc/jmp, a2-dialog), so the
other alpha values differ only in the two colour bytes. Those are covered by the C3 fixture (alpha 0x00 / 0x01 / 0x7f /
0x80 / 0xff).

Not exercised: screens 15/16 (`FUN_0043a610` / `FUN_0043aa30`). They are not on this route, and the existing tool
reaches them only through a synthetic `FUN_0043d2a0` push (`menu_draw_burst.py --screen`), which is a forced input. They
are not needed: the callee is the same branchless body, and only the callers' arguments differ.

## 5. 0x00404320 PerModeRenderMachine: NEEDS USER DECISION (stays C3)

**Why no stock-save scenario can meet the bar.**

- The game calls 0x00404320 every race frame: from `FUN_00492e90` case 3/7 (`0x00771968`), after BeginUpdate.
- But every arm the port holds is gated on the rule `0x007f0fd0` being 5, 8, 9 or 10 (`0x00404540 jne 0x404642`
  for every other value).
- The rule is written at race launch from the cup event table `0x005f65c8` (`Race/RaceModes.cpp`, the
  `kCupEvent` transcription). Rules 5/8/9/10 occur only in the mode-4 (Silver) and mode-5 (Gold) cells. Bronze
  (mode 3) cells give rules 0 and 4, Quick Battle gives 0, and the attract demo gives 6.
- The stock save opens Bronze row 0 only (`c4_a1_h_RESULT.md` §1 and §4; Bronze event 0 measured at rule 0 there).
- Measured here with no forced input. These are Quick Battle runs, `--row pmr`, with 30 s of the 1 Hz control-4
  pulses. Each call samples `0x007f0fd0`, `0x0063ba8c` and `0x00771968` at entry:

  | run | first bytes at 0x00404320 | target | game calls | rule `0x007f0fd0` | sub-states seen (calls) |
  |---|---|---|---|---|---|
  | pmr_stock1 | `a1 d0 0f 7f 00 83 ec 10` | original | 1733 | **0 on all** | 2:3, 3:9, 4:4, 5:448, 6:890, 7:376, 8:3 |
  | pmr_port1 | `e9 eb cc c4 68 83 ec 10` | `mashed_re_dev.asi+0x21010` = export `PerModeRenderMachine` | 1750 | **0 on all** | 2:3, 3:8, 4:4, 5:448, 6:905, 7:379, 8:3 |

  The live port is called by the game 1750 times, but every call is on the rule-0 no-op arm, and no AV occurred.

So a stock-save run can show the live port called by the game, but only on the no-op arm. That is exactly the evidence
the 2026-09-09 demotion (U-9086) rejected.

**The forced input I would use (not applied, the user's call).** It is the same one `c4_a1_h_RESULT.md` §4 used for
0x004295a0:

1. Boot an **install copy** of `original/` (robocopy to the scratchpad) via `MASHED_ROOT`. Its `MASHED.exe`, `d3d9.dll`
   and the dinput8 shim are byte-identical to `original/`.
2. In the copy's `gamesave.bin` only, set 26 dwords to 1: span `0x24A40`, column `+0x08` (FrontendModeIndex(mode 4) = 2)
   and column `+0x14` (FrontendModeIndex(mode 5) = 5) on all 13 rows. These are the launch-gate columns that
   `gamesave_edit.py` documents as propagating to `DAT_007f0a40` on load. `original/gamesave.bin` is never written.
3. Launch through the menus. Single Player, challenge flow, colour select, then control 10 (cup selector 1 → 2, mode 4),
   then downs to the challenge index, then confirm. No memory poke. The rule is written by the game's own launch.

Cells that reach each arm (`kCupEvent`, index = challenge index `0x0067f17c`):

- rule 10: mode 4 index 2 (NEUSTEIN, used by c4_a1_h) or index 12; mode 5 index 5
- rule 8: mode 4 index 6; mode 5 index 11
- rule 9: mode 4 index 10; mode 5 index 2 / 4 / 9
- rule 5: mode 5 index 3 / 7 / 12

`[UNCERTAIN]` Only mode 4 index 2 → rule 10 is measured (c4_a1_h, 9/9 runs). The other rows assume that challenge index
equals the table's track row, as it does at index 2. U-9672 records an open "8 cup tracks vs rows 0..12" residual, so
the first run per cell must log `0x007f0fd0` before anything is compared.

Then idle in the race, and let the sub-state `0x0063ba8c` walk the game's own values. The observable would be this
harness's call-log treatment of the ten callees named in a2-stubat (`end`, `vw` with the float pair, `begin`, `r9`,
`r8`, `r5`, `r10`, `entry` + EAX, `sub6`), keyed by (rule, sub-state).

The control would be `Render/PerModeRender.cpp` at `9cde82cb~1`. Its defect in a live scenario: mode 5 with sub-state 6
calls `FUN_004041c0`, and mode 10 with sub-state 6 calls 0x00403fa0 with a residue EAX. Note that the second one
**writes** a float through that EAX (`0x00403fc1 fstp [eax+0x10]`), so the control may AV rather than read a clean RED.

One more point for the user. Sub-state 6 is entered naturally: 890 / 905 of ~1740 race frames on rule 0 above. So a
rule-10 or rule-5 event can be expected to reach the sub-state-6 arms that hold both a2-stubat defects. That expectation
is still not measured for those rules. A first run per cell should record the (rule, sub-state) histogram before arms
are compared.

## 6. Proposed tracker transactions (for the parent's re-classify; NOT applied)

Evidence pointer for every row: `re/analysis/negctl/session_20261012b/c4front_evidence/c4_front_compare.csv`.

| RVA | hooks.csv row | proposed | append to notes |
|---|---|---|---|
| 0x00492270 | `00492270,SubsystemInit` | **C3 -> C4** | see (a) |
| 0x004026d0 | `004026d0,FUN_004026d0` | **C3 -> C4**, subject to the STUBS ruling (b) | see (b) |
| 0x0042f8d0 | `0042f8d0,MenuMenusBC` | **C3 -> C4** | see (c) |
| 0x00404320 | `00404320,PerModeRenderMachine` | **no change (C3)** | see (d) |

(a) "C4 canonical 2026-10-12 c4front: boot, the game calls 0x00492270 once from 0x004923ad. The worktree .asi was
loaded while suspended, MASHED_HOOK_ONLY=0x00492270, 0xE9 -> mashed_re_dev.asi+0x17c90 (export SubsystemInit), game
calls 1/1. Log `rwinit(0)=1; display(); viewport(); ret 1`: 2/2 port GREEN, original 2/2 identical. Old-defect control
(own DisplayInit/ViewportInit bodies) RED 2/2 (3 of 5 entries). Return-0 arm covered by the C3 fixture only.
re/analysis/negctl/session_20261012b/c4front_RESULT.md §2"

(b) "C4 canonical 2026-10-12 c4front: boot, the game calls FUN_004026d0(4) once from 0x00402a14. 0xE9 ->
mashed_re_dev.asi+0x21260 (export BootQueueFlush). The 34-call camera log matches the original 34/34 on 2/2 runs
(original 2/2 identical up to the BeginUpdate pointer). Old-defect control (1-arg 0x004c1bb0/0x004c1be0) RED 2/2 (8/34).
n<=0 and BeginUpdate-fail arms covered by the C3 fixture only. c4front_RESULT.md §3"

STUBS ruling needed for (b). CONFIDENCE.md:85 says "No stubs in the implementation", and ROADMAP F-DoD says "no stubs".
S-0171..S-0175 are live `passthrough` rows with 0x004026d0 as caller. Each is a call to the original function at its
own RVA, and the C4 run observed all five with the original's exact arguments. Precedent the same day: 0x00403160 went
to C4 with passthrough S-0560 still live. Two clean options for the parent:

1. CLEAR S-0171..S-0175, as calls the bytes make, with live arguments witnessed in c4front §3; then promote.
2. Or keep the row at C3 until the 0x004c1xxx / 0x004671a0 callees are themselves ported.

I recommend 1. A passthrough to the original RVA is the faithful call, not a placeholder. U-0170 (FUN_004671a0 arity,
"Blocks: none") can be resolved in the same transaction from the bytes a2-stubat already cites: 0x004671a0 reads one
stack arg, `[esp+4]` at `0x004671aa`. The extra pushes are caller residue consumed by the next callee, and the live log
shows `cam(0)` returning the same camera every call.

(c) "C4 canonical 2026-10-12 c4front: natural Race Results (statenav Quick Battle, no push or poke). The game calls
0x0042f8d0 ~2,860 times per run, all on screen type 5, from FUN_00434720 sites ret 0x0043493d / 0x00434ef3 (x2) /
0x004361c0 / 0x0043667b. 0xE9 -> mashed_re_dev.asi+0x15eb0 (export MenuMenusBC). All 25 dwords of the five 0x00472c60
calls match the pooled original on 590/590 and 600/600 calls (2/2). The original's noise is one input: the 0x0043667b
plate width follows the race outcome. AL-ignored control RED 600/600 and 590/590 (alpha 0 colours). Alpha 0xff only in
the scenario; other alphas covered by the C3 fixture. c4front_RESULT.md §4"

(d) "2026-10-12 c4front: the game calls the live port (0xE9 -> mashed_re_dev.asi+0x21010) 1750 times in a Quick Battle,
all with rule 0x007f0fd0 = 0, so no ported arm runs. Rules 5/8/9/10 need a Silver/Gold cup cell, which the stock save
cannot launch. C4 waits on a user decision on the c4_a1_h §4 save edit (install copy). c4front_RESULT.md §5"

No new U-rows. No CHANGELOG entry is written on this branch. The parent's re-classify transaction writes one per
promotion.

## 7. Command lines

From the worktree root (PowerShell). `$EV` = `re\analysis\negctl\session_20261012b\c4front_evidence`, and `$PORT` /
`$CTL` = the two `.asi` paths (SHA-256 above):

```
py -3.12 re\frida\c4_front_scenario.py --row subinit --arm stock --tag subinit_stock1 --out $EV
py -3.12 re\frida\c4_front_scenario.py --row subinit --arm port  --asi $PORT --tag subinit_port1 --out $EV
py -3.12 re\frida\c4_front_scenario.py --row subinit --arm ctl   --asi $CTL  --tag subinit_ctl1  --out $EV
#   same with --row bqf, --row bc (Race Results route, --window 2.0 default), --row pmr; n = 1, 2
py -3.12 re\frida\c4_front_compare.py $EV
```

Run times: boot rows about 15 s each. bc about 2.5 min (Race Results after a full Quick Battle match). pmr about 1.5 min.
No run hit the U-9677 collision AV (0 AVs in 26 runs: 12 boot, 12 bc over two rounds, 2 pmr).

## 8. Files

- harness: `re/frida/c4_front_scenario.py`, `re/frida/c4_front_compare.py`
- runs: `c4front_evidence/c4_front_<row>_<arm><n>.json` (subinit, bqf, bc, pmr)
- comparison: `c4front_evidence/c4_front_compare.csv` + `_summary.json` (no `diff_` prefix, so not gitignored)
- control source diff: `c4front_evidence/ctl_patch.diff`
- first BC round (window-edge artefact, superseded): `c4front_evidence/bc_round1_superseded/`
- the two `.asi` binaries are not committed (scratchpad). Their SHA-256 is in every run JSON (`asi_sha256`).
