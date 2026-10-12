# C4-SAVE RESULT: canonical C4 for 0x00404320, 0x0040d440, 0x0041c2d0 on the approved save-copy route (2026-10-12)

Branch `work/c4save`, worktree `.worktrees/c4save` (base f8c1ed99). Evidence only: no tracker edit, no re-classify,
nothing deployed to `original\` (`MASHED_NO_DEPLOY=1`), nothing written under `original\`.

| RVA | row | verdict |
|---|---|---|
| 0x0040d440 | FUN_0040d440 Course::LoadCurrent | **C4 EARNED** (port GREEN 2/2, original 2/2 identical, control RED 2/2) |
| 0x0041c2d0 | FUN_0041c2d0 | **C4 EARNED** (port GREEN 2/2, 1114 / 1109 game calls, control RED 2/2, AV at its first game call) |
| 0x00404320 | PerModeRenderMachine | **C4 EARNED for the rule-10 arm** (port GREEN 2/2 on 499 / 494 captured calls over sub-states 3/4/5/6/11, control RED 2/2 with no crash). The rule 5/8/9 arms and the 0x00403fa0 loop do not run on this route. They are covered by the C3 fixture only (section 5.5) |

## 1. Forced input (user decision 2026-10-12, the only one used)

**Group-H install-copy save edit.** Every run of all three rows boots an **install copy** of `original/`
(`<scratchpad>/gameroot/original` for 40d440 / 41c2d0, `<scratchpad>/gameroot2/original` for pmr, robocopy). Its
`MASHED.exe`, `mashed_re_dev.asi`, `d3d9.dll` and `dinput8.dll` have the same SHA-256 as in `original/`.

- The edit is in the copy's `gamesave.bin` only: span 0x24A40, 13 rows x 0x30, the dwords at +0x08 and +0x14 on
  every row set 0 -> 1 (26 dwords, 26 changed bytes).
- `original/gamesave.bin` (`bd187881...`) was never written.
- The edited save (`80ccb2d4...`, the same SHA-256 as group H's `c4_hud_install_copy.txt`) is copied over the copy's
  save before **every** spawn (`--reset-save`). Every run JSON records `save_sha256 = 80CCB2D4...` at spawn.
- Record: `c4save_evidence/c4save_install_copy.txt`.

The route is then the game's own menus, with no memory poke:

1. Single Player, then the challenge flow (depth-3 cursor 0), colour select and challenge select.
2. Control 10: cup selector `0x0067f184` 1 -> 2 (mode 4).
3. Two downs: `0x0067f17c` 0 -> 2. Confirm.

The race launches with game mode 4, `0x0067f17c` = 2 and rule `0x007f0fd0` = 10 on all 18 runs (pre-launch and
first-call states are in every JSON). No input is sent in the race. The two install copies were deleted after the
runs.

No other forced input was used. Rule 5 (Gold) was not attempted.

## 2. Binaries and arms

Both `.asi` are `MASHED_NO_DEPLOY=1` builds of this worktree. Copies are kept in the scratchpad and not committed.

| file | SHA-256 | content |
|---|---|---|
| `c4save_fix.asi` | `AB541BA7040E2DC4BE6AF093E3EF1B031A54C36FC498CA22D8B2E395A3312A65` | HEAD f8c1ed99 source, unmodified |
| `c4save_ctl.asi` | `3379A0A364E3A3F620A2E0C26AC8F425961A15F94F2762EA077C1869225CE921` | `c4save_evidence/c4save_ctl_patch.diff` (three old-defect bodies, see below) |

The control `.asi` holds all three old bodies, as c4front's did. `MASHED_HOOK_ONLY` installs only the row under test,
so the other two bodies are never reached. The sources were restored with `git checkout` after the control build.

- **0x0040d440:** the c4race witness, reused verbatim. Argument 2 of FUN_0040d270 is read from `0x0063ba78` instead of
  `0x0063ba7c`. Capstone: fix export `+0x80730` `push dword ptr [0x63ba7c]`, ctl `+0x80830` `push dword ptr [0x63ba78]`.
- **0x0041c2d0:** the pre-U-9671 body (`8dd0ce22^`). It derefs a vtable (`mov edx,[ecx]` / `call [edx+0x48]`), never
  pops, and reuses the clobbered EAX. Only the function body was swapped. The `RH_ScopedInstall` line stayed live
  (the old file had it commented out). Capstone of the ctl export `+0x133f0`: `mov ecx,[eax+0xc] | mov edx,[ecx] | push
  ecx | call [edx+0x48] ...`. Fix export `+0x13420`: `push eax | call body | add esp,4 | ret`.
- **0x00404320:** `Render/PerModeRender.cpp` at `9cde82cb~1`, the c4front-named control. Its install line is live.

Arms, with one fresh MASHED per run (own PID, killed by PID through `dev.kill(pid)`):

| harness | original arm | port arm | control arm |
|---|---|---|---|
| `c4_hud_scenario.py` (40d440, 41c2d0) | `stock`: `MASHED_RE_NO_AUTO_HOOK=1`, `MASHED_HOOK_ONLY=__none__` | `late`: as stock with `MASHED_HOOK_ONLY=<rva>`, `Module.load(c4save_fix.asi)` + `InjectHooks` at the title | `ctl`: as late with `c4save_ctl.asi` |
| `c4_front_scenario.py` (pmr) | `stock` | `port`: `.asi` loaded while suspended + `InjectHooks`, `MASHED_HOOK_ONLY=0x00404320` | `ctl` |

Both rows on `c4_hud_scenario.py` are first called after the title (course load, race HUD). So the title-time load
installs the JMP before the game's first call.

## 3. 0x0040d440 Course::LoadCurrent: C4 EARNED

```
py -3.12 re/frida/c4_hud_scenario.py --row 40d440 --arm stock|late|ctl --route cup --root <copy>\original \
    --reset-save <edited gamesave.bin> --secs 10 --out re/analysis/negctl/session_20261012b/c4save_evidence/40d440 \
    --tag 40d440_<arm><n> [--ctl-asi c4save_fix.asi|c4save_ctl.asi]
py -3.12 re/frida/c4_hud_compare.py --dir .../c4save_evidence/40d440 --row 40d440 --pool --csv .../c4save_compare_40d440.csv
```

**The value proving the ported path ran.** At the call, selected `[0x0063ba7c]` = **2** and loaded `[0x0063ba78]` =
**0**. This is the selected != loaded case that c4race could not reach on any stock-save route. The return address is
0x00492a6c, the original caller FUN_004929d0 (call at 0x00492a67). The game calls the body once per run, at race
load (state 2, rule 10, mode 4).

| run | result | ref | game calls | FUN_0040d270 calls | dword at 0x0040d440 | key |
|---|---|---|---|---|---|---|
| stock1 | GREEN | stock2 | 1 | 1 | `63ba7ca1` (original) | `sel=2 pre78=0 ret=492a6c F(OBJ,SEL) post78=SEL` |
| stock2 | GREEN | stock1 | 1 | 1 | `63ba7ca1` | same |
| **late1** | **GREEN** | stock1+stock2 | **1** | 1 | `4e32ebe9` -> `c4save_fix.asi+0x80730` = export `Course_LoadCurrent_40d440` | same |
| **late2** | **GREEN** | stock1+stock2 | **1** | 1 | `4e32ebe9` -> `+0x80730` | same |
| **ctl1** | **RED** | stock1+stock2 | 1 | 1 | `4e33ebe9` -> `c4save_ctl.asi+0x80830` | `sel=2 pre78=0 ret=492a6c F(OBJ,!0) post78=SEL` |
| **ctl2** | **RED** | stock1+stock2 | 1 | 1 | `4e33ebe9` -> `+0x80830` | same |

The original has no noise here: both stock runs give the same single key. The control passes 0 as argument 2 where
the original and port pass 2 (foreign 1/1 call, missing 1/1 key on both control runs). The hook-live check resolves
the export offset from the loaded `.asi` by name (pefile). The dword at the RVA is unchanged at the end of every
port/control run. 0 crashes in 6 runs. A feasibility stock run (`feas/c4_hud_40d440_feas_stock1.json`) gave the same
key.

## 4. 0x0041c2d0 FUN_0041c2d0: C4 EARNED

```
py -3.12 re/frida/c4_hud_scenario.py --row 41c2d0 --arm stock|late|ctl --route cup --root <copy>\original \
    --reset-save <edited gamesave.bin> --secs 40 --out .../c4save_evidence/41c2d0 --tag 41c2d0_<arm><n> [--ctl-asi ...]
py -3.12 re/frida/c4_hud_compare.py --dir .../c4save_evidence/41c2d0 --row 41c2d0 --pool --csv .../c4save_compare_41c2d0.csv
```

**Observable, per game call.** The block pointer (EAX at entry) and the ordered member slots whose `+0x48` function
the body called. The four members' `+0x48` are swapped to a recorder for the duration of each call only, the
row-41db80 method. Also recorded: the return value against the last member call's return, and rule / state at entry.
A counter on 0x0041a3e0 (HudDispatchMode10) records its entries and whether `[0x0063c628]` != 0, and one on
0x0041c300 (Mode5) does the same.

**The value proving the ported path ran.**

- Every call has block `0x0063c610` (the EAX that 0x0041a3e0 passes, tail jmp at 0x0041a3ee), rule 10, game mode 4,
  state 5 or 6.
- 0x0041a3e0 entries = calls = entries with flag `[0x0063c628]` = 1 on every run, and 0x0041c300 had 0 entries.
- So every call came from HudIngameDispatch -> 0x0041a3e0 on rule 10 (0x0040e0bd), as c4race derived statically.
- The first call comes about 20 s after launch, when state `0x0063ba8c` leaves 3 for 5.

| run | result | ref | game calls (= 0x0041a3e0 entries) | keys | dword at 0x0041c2d0 | crash |
|---|---|---|---|---|---|---|
| stock1 | GREEN | stock2 | 1095 | 2 | `8bf08b56` (original) | 0 |
| stock2 | GREEN | stock1 | 1104 | 2 | same | 0 |
| **late1** | **GREEN** | stock1+stock2 | **1114** | 2 | `c5714be9` -> `c4save_fix.asi+0x13420` = export `FUN_0041c2d0` | 0 |
| **late2** | **GREEN** | stock1+stock2 | **1109** | 2 | same | 0 |
| **ctl1** | **RED** | stock1+stock2 | AV inside the body at the 1st call | 0 | `2e711be9` -> `c4save_ctl.asi+0x133f0` | 1 |
| **ctl2** | **RED** | stock1+stock2 | same | 0 | same | 1 |

The keys, identical on all four non-control runs:

```
blk=63c610 order=3,1,0,2 ret=LAST rule=10 st=5     (112 calls on every run)
blk=63c610 order=3,1,0,2 ret=LAST rule=10 st=6     (983 / 992 / 1002 / 997)
```

The order 3,1,0,2 is the byte order 0x0041c2d3 / 0x0041c2da / 0x0041c2e1 / 0x0041c2e7. `ret=LAST` means EAX equals
the last member call's return.

The original's noise is the st=6 call count only, which follows how long the run observed.

The control AVs at `c4save_ctl.asi+0x133f6` (`call dword ptr [edx+0x48]`, read @ `0x01050049`) on the first game call
(`enter` = 1, inside the body) on both boots. That is the old body's vtable deref: `[block[3]]` = `0x01050001` is not a
vtable, and `+0x48` lands on unmapped memory. This is the RED c4race_RESULT.md predicted ("AVs at [vtbl+0x48] per
u9671_RESULT.md"). It is the port's exception, not an original-side piz collision (neither `+0x1509b4` nor
`+0xe66db`).

The return-address field in the JSONs (`rets`) holds a per-boot non-image address, because the caller enters through a
tail jmp. It is not used as evidence. The caller proof is the 0x0041a3e0 counter equality above.

Coverage, stated plainly: rule 10 / block 0x0063c610 only. The 0x0041c300 (rule 5, block 0x0063cda4) caller needs a
Gold event. That would be an additional forced input, not approved, and was not attempted. It is the same body with a
different EAX.

## 5. 0x00404320 PerModeRenderMachine: C4 EARNED for the rule-10 arm

```
py -3.12 re/frida/c4_front_scenario.py --row pmr --arm stock|port|ctl --route cup --root <copy>\original \
    --reset-save <edited gamesave.bin> --out .../c4save_evidence/pmr --tag pmr_<arm><n> [--asi c4save_fix.asi|c4save_ctl.asi]
    (defaults --secs 70 --win 1.5 --period 6)
py -3.12 re/frida/c4_front_compare.py .../c4save_evidence/pmr
```

### 5.1 Observable

The entry hook runs on every call and records a histogram of (rule `0x007f0fd0`, sub-state `0x0063ba8c`). Twelve
capture windows of 1.5 s run every 6 s. Inside a window, every call that starts and ends in the window records the
body's ordered **top-level** callee calls (a callee's own nested calls are not logged):

- `end` 0x004c19f0 and `begin` 0x004c1a00 (camera, return)
- `vw` 0x004c1c80 (camera + the two floats; the restore pair is `PRE` when it equals camera+0x68/+0x6c read at the
  row's entry)
- `sub` 0x0040e350 (return)
- `r9` 0x00403d30, `r8` 0x00403db0, `r5` 0x00403ed0, `r10` 0x00403c60, `sub6` 0x004041c0
- `fa0` 0x00403fa0 (stack index + EAX as table entry)
- `rs`, the render-state function `*(*(0x007d3ff8)+0x20)` = 0x004d7480 (state, value)

The camera is `C` when it equals `*(*(0x007d3ff8))` at entry. A call is GREEN when its log occurs in the pooled
original (stock1 + stock2) for the same (rule, sub-state).

### 5.2 The value proving the ported path ran

Rule `0x007f0fd0` = **10** on every call of every run (histograms below), so the mode-10 arm runs every frame. These
are its prologue, the 5/4/3 chain, the ==6 test and the inlined tail (0x00404546..0x00404639). The windows log it, on
port and original alike:

```
fd0=10,sub=3 | end(C) vw(C,3f4ccccd/3f4ccccd) begin(C)=C rs(6,0) rs(8,0) sub=3 sub=3 sub=3 r10 rs(6,1) rs(8,1) end(C) vw(C,PRE) begin(C)=C
fd0=10,sub=5 | ... rs(8,0) sub=5 r10 rs(6,1) ...
fd0=10,sub=6 | ... rs(8,0) sub=6 sub=6 sub=6 sub=6 rs(6,1) rs(8,1) end(C) vw(C,PRE) begin(C)=C
```

`0x3f4ccccd` = 0.8f, the 0x00404356..0x00404366 view window. The three `sub` calls on sub-state 3 are the bytes'
one-call-per-test chain (cmp 5, cmp 4, cmp 3). Sub-state 6 adds the fourth query at 0x004045c9.

### 5.3 Results

Game calls are whole-run entry counts through the JMP on port/ctl, and through the original entry on stock. "Captured"
is the number of calls compared inside the windows.

| run | result | game calls | captured | mismatch | first 8 bytes at 0x00404320 | target |
|---|---|---|---|---|---|---|
| stock1 | ref | 1910 | 492 | - | `a1 d0 0f 7f 00 83 ec 10` (original) | original |
| stock2 | GREEN vs stock1 | 1908 | 490 | 0 | same | original |
| **port1** | **GREEN** vs stock1+stock2 | **1920** | 499 | **0** | `e9 ab cc c7 68 83 ec 10` | `c4save_fix.asi+0x20fd0` = export `PerModeRenderMachine` |
| **port2** | **GREEN** | **1925** | 494 | **0** | same | same |
| **ctl1** | **RED** | 1944 | 492 | **445** | `e9 1b ce 30 69 ...` | `c4save_ctl.asi+0x21140` = export `PerModeRenderMachine` |
| **ctl2** | **RED** | 1951 | 493 | **461** | `e9 1b ce 43 6f ...` | same |

Per sub-state (matched / mismatch):

| run | sub 3 | sub 4 | sub 5 | sub 6 | sub 11 |
|---|---|---|---|---|---|
| port1 | 149 / 0 | 1 / 0 | 41 / 0 | 269 / 0 | 39 / 0 |
| port2 | 157 / 0 | 1 / 0 | 34 / 0 | 268 / 0 | 34 / 0 |
| ctl1 | 0 / 141 | - | 47 / 0 | 0 / 239 | 0 / 65 |
| ctl2 | 0 / 157 | 0 / 1 | 32 / 0 | 0 / 255 | 0 / 48 |

0 unmatched calls on any run, 0 window-edge calls, 0 crashes, all 6 alive at the harness kill.

**Original noise:** the whole-run sub-state histograms, the same shape on every run:

| sub-state | calls per run |
|---|---|
| 3 | 577..600 |
| 4 | 1 |
| 5 | 112 |
| 6 | 1069 |
| 11 | 137..185 |

The only varying counts are how many sub-3 / sub-11 frames fall inside the 70 s observation. Within a (rule,
sub-state) key, the original produced one log in both runs. The sub-11 restore pair animates, but it is `PRE` (equal
to the camera's own +0x68/+0x6c at entry) on every original call.

**The control departs exactly where its source says.** The old body queries the sub-state once for the 5/4/3 test and
once for ==6. So its log has `sub=3 r10` (1 query) where the bytes make 3, and `sub=6 sub=6` (2) where the bytes make
4. Sub-state 5 is the one case where the chain stops at the first test, and there the control matches (47 / 32 calls).
This is a clean RED with no AV.

### 5.4 Not exercised on this route (stated plainly)

- **0x00403fa0 loop (mode 10, sub-state 6):** 0 `fa0` calls on all 6 runs. No `0x00636b88` entry had `+0x10` above
  `[0x005d757c]` during these runs, so the residue-EAX defect of the old body (c4front §5) was not reachable, and the
  RED comes from the sub-state chain instead.
- **Rules 9, 8 and 5** (0x00403d30, 0x00403db0, 0x00403ed0, 0x004041c0): not run. Per c4front §5, they need Silver
  challenge index 10 (rule 9) or 6 (rule 8), or a Gold event (rule 5).
  - Index 6 and 10 use the same save edit, but the approval names challenge index 2 only, so they were not run.
  - The Gold event would be a further forced input.
- **BeginUpdate failure** (`je 0x404642`): `begin(C)=C` on every captured call.

These arms are covered by the C3 fixture only. This matches the precedent of c4front §2 (SubsystemInit return-0 arm)
and §4 (MenuMenusBC alpha 0xff only). **Parent decision:** promote 0x00404320 to C4 on the rule-10 arm with this
coverage note, or hold it at C3 until a rule 9/8/5 route is approved. I recommend promoting with the note. The
dispatcher, prologue and tail are shared by all four arms, and they ran.

## 6. Proposed tracker transaction (for the parent's re-classify; NOT applied)

Forced-input statement for every row's notes: *"forced input (user-approved 2026-10-12): group-H install-copy save
edit, 26 launch-gate dwords at 0x24A40 (+0x08/+0x14 x 13 rows) set to 1 in a copy's gamesave.bin, original/ untouched;
route Silver cup event, challenge index 2, rule 10."*

**hooks.csv 0x0040d440 FUN_0040d440: C3 -> C4.** frida_diff ->
`re/analysis/negctl/session_20261012b/c4save_evidence/c4save_compare_40d440.csv`. Append to notes:

> c4save 2026-10-12 C3->C4: [forced-input statement]. The game calls the live port once at race load from 0x00492a67
> (ret 0x00492a6c) with selected [0x0063ba7c] = 2 vs loaded [0x0063ba78] = 0. Hook live: 0x0040d440 = E9 ->
> c4save_fix.asi+0x80730 (export Course_LoadCurrent_40d440), MASHED_HOOK_ONLY. FUN_0040d270(OBJ, 2) and post78 = 2 on
> 2/2 port runs, identical to the original's 2 runs. Injected-arg control RED 2/2 (FUN_0040d270(OBJ, 0)).
> re/analysis/negctl/session_20261012b/c4save_RESULT.md §3

**hooks.csv 0x0041c2d0 FUN_0041c2d0: C3 -> C4.** frida_diff -> `.../c4save_evidence/c4save_compare_41c2d0.csv`.
Append to notes:

> c4save 2026-10-12 C3->C4: [forced-input statement]. The game calls the live port 1114 / 1109 times per run from
> HudIngameDispatch -> 0x0041a3e0 (rule 10, states 5/6, block 0x0063c610; 0x0041a3e0 entries = port calls). Hook
> live: 0x0041c2d0 = E9 -> c4save_fix.asi+0x13420 (export FUN_0041c2d0). Member dispatch order 3,1,0,2 with EAX = the
> last member's return on every call, identical to the original's 2 runs. Pre-U-9671 control (vtable deref) RED 2/2:
> AV at c4save_ctl.asi+0x133f6 on the first game call. Rule-5 caller 0x0041c300 not exercised (Gold event).
> c4save_RESULT.md §4

**hooks.csv 0x00404320 PerModeRenderMachine: C3 -> C4 (rule-10 arm), subject to the parent decision in §5.4.**
frida_diff -> `.../c4save_evidence/pmr/c4_front_compare.csv`. Append to notes:

> c4save 2026-10-12 C3->C4 (rule-10 arm): [forced-input statement]. The game calls the live port ~1920 times per 70 s
> run, all with rule 0x007f0fd0 = 10, sub-states 3/4/5/6/11. Hook live: 0x00404320 = E9 -> c4save_fix.asi+0x20fd0
> (export PerModeRenderMachine). The top-level callee log (end/vw/begin/rs/sub/r10 + restore) matches the pooled
> original on 499/499 and 494/494 captured calls. The 9cde82cb~1 control is RED 2/2 (445/492, 461/493): one
> sub-state query where the bytes make 3/4. Not exercised: the 0x00403fa0 loop (no active 0x00636b88 entry) and the
> rule 9/8/5 arms (C3 fixture only). c4save_RESULT.md §5

**UNCERTAINTIES.md:** if c4race's optional row "C4 for 0x0040d440 / 0x0041c2d0 needs a forced route" was added, close
it with this note. No STUBS / DEFERRED change. No new U-row. If the parent holds 0x00404320 at C3, a
DEFERRED/UNCERTAINTIES row would name "rule 9/8/5 route (Silver index 6/10 or Gold) not approved".

## 7. Harness changes (extended, no one-offs)

- `re/frida/c4_hud_scenario.py`:
  - `--route cup` now works for any row (needs `--root <copy>\original`, refuses the real install).
  - `--reset-save FILE` restores the edited save before each spawn.
  - `--pulse` gives the qb in-race pulses on the cup route (not used here).
  - Row 41c2d0 has a per-call member-dispatch recorder (block, order, ret vs last, rule/state) in place of
    counters-only.
- `re/frida/c4_hud_compare.py`:
  - `--pool` compares port/control arms against the union of both original runs, and a key is "missing" only when both
    originals produced it.
  - Row 41c2d0 is supported.
  - The hook-live export offset is resolved from the loaded `.asi` by export name (pefile).
- `re/frida/c4_front_scenario.py`:
  - `--route cup --root --reset-save` for row pmr.
  - `cup_event()` (the same menu order as c4_hud's cup route).
  - pmr per-call callee logs in capture windows (`--secs --win --period`).
  - `state()` also returns mode / `0x0067f184` / `0x0067f17c`.
- `re/frida/c4_front_compare.py`: a pmr branch (pooled original, matched / mismatch / unmatched per (rule, sub-state)).

## 8. Files and incidents

- runs: `c4save_evidence/{40d440,41c2d0}/c4_hud_<row>_<arm><n>.json`, `c4save_evidence/pmr/c4_front_pmr_<arm><n>.json`
- comparisons: `c4save_compare_40d440.csv`, `c4save_compare_41c2d0.csv`, `pmr/c4_front_compare.csv` (+ `_summary.json`)
- control source diff: `c4save_ctl_patch.diff`. Install copy record: `c4save_install_copy.txt`.
- `feas/`: one stock feasibility run per row, taken before the arms. `feas/c4_front_pmr_feas_stock1.json` predates the
  `PRE` normalisation, so its sub-11 restore pairs are raw.
- 18 arm runs + 3 feasibility runs. 0 original-side AVs, so no piz-collision re-run was needed (each row used its own
  install copy, so two of this session's games never opened the same piz file). The only AVs are the two 0x0041c2d0
  control runs, inside the control body.
