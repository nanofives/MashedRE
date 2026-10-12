# C4-RACE RESULT: canonical C4 scenarios for 0x0040d440, 0x004464c0, 0x0041c2d0 (2026-10-12)

Branch `work/c4race`, worktree `.worktrees/c4race` (base 8da9ecf2). Evidence only: no tracker edit, no
re-classify, nothing deployed to `original\` (`MASHED_NO_DEPLOY=1`), no forced input of any kind.

| RVA | row | verdict |
|---|---|---|
| 0x004464c0 | CameraEntry::DispatchAll | **C4 EARNED** (.asi copy). Natural attract demo, no input. |
| 0x0040d440 | FUN_0040d440 Course::LoadCurrent | **Stays C3: needs a user decision.** The game calls the live port naturally, but every stock-save route loads course 0 with loaded course 0, so the old-defect control reads GREEN (degenerate). |
| 0x0041c2d0 | FUN_0041c2d0 | **Stays C3: needs a user decision.** No stock-save route reaches it (0 calls measured on attract and Quick Battle). It needs rule 5 or 10, which exist only in Silver/Gold cup cells. |

## The bar applied (per row)

1. The game itself calls the live port on its caller's screen (no direct export call).
2. Hook-live witness (the dword at the RVA: `E9` + rel32 into the loaded control/fix module) and a game-driven
   call count > 0.
3. The original arm run twice to measure its own noise, and the port arm compared against it.
4. The old-defect control, installed the same way, must read RED on the same scenario.

## Harness

Extended, not new: `re/frida/c4_hud_scenario.py` (rows `40d440`, `4464c0`, `41c2d0`, option `--route
attract|qb|cup`, `--track-presses`) and `re/frida/c4_hud_compare.py` (rows `40d440`, `4464c0`).

Arms, one fresh MASHED per run, own PID only (killed by the harness's `dev.kill(pid)`):

| arm | setup |
|---|---|
| `stock` | `MASHED_RE_NO_AUTO_HOOK=1`, `MASHED_HOOK_ONLY=__none__`, nothing loaded. The original reference. |
| `late` | `MASHED_RE_NO_AUTO_HOOK=1`, `MASHED_HOOK_ONLY=<rva>`, at the title `Module.load(c4race_fix.asi)` + `InjectHooks`. |
| `ctl` | as `late`, with `c4race_ctl.asi` (the old defect). |

Binaries (both `MASHED_NO_DEPLOY=1` builds of this worktree, copies kept in the session scratchpad, not committed):

| file | SHA-256 | content |
|---|---|---|
| `original/MASHED.exe` | patched reference (`.unpatched` = anchor) | the ten documented boot patches |
| `c4race_fix.asi` | `4EA89BB02759985FC8E0DE2D118B81D9AC2721BD0EFF9708013F9F46E8E92427` | HEAD 8da9ecf2 source, unmodified |
| `c4race_ctl.asi` | `B2A2E1E9758DBE9BEFA89210184E0444BAC80F990C2E14F0E3EF48F9D54BAC71` | `c4race_evidence/c4race_ctl_patch.diff` |

The control patch, then reverted (sources restored from copies, `touch`ed, rebuilt "(source newer)"):

- 0x004464c0: the a2-camera witness, the old 16-byte frame alone: `push ebp` after `push edi`,
  `mov edi,[esp+14h]`, `pop ebp` before `pop edi`. Capstone of the ctl export `+0x3d050`:
  `... push esi | push edi | push ebp | mov edi, dword ptr [esp + 0x14] ...`; fix export: `push esi | push edi |
  mov edi, dword ptr [esp + 0x10]`.
- 0x0040d440: the a2-stubat witness. This port never had a defect (it matched the bytes), so the witness there was
  an injected one, and it is reused here as such: argument 2 of FUN_0040d270 read from `0x0063ba78` instead of
  `0x0063ba7c`. Capstone of the ctl export `+0x80760`: `push dword ptr [0x63ba78]`; fix: `push dword ptr [0x63ba7c]`.

Evidence: `re/analysis/negctl/session_20261012b/c4race_evidence/` (per-run JSON `c4_hud_<row>_<arm><n>.json`,
compare CSVs `c4race_compare_4464c0.csv` / `c4race_compare_40d440.csv`, no `diff_` prefix).

## 0x004464c0 CameraEntry::DispatchAll: C4 EARNED

### Scenario

```
py -3.12 re/frida/c4_hud_scenario.py --row 4464c0 --arm stock|late|ctl --route attract --secs 40 \
    --out re/analysis/negctl/session_20261012b/c4race_evidence/4464c0 --tag 4464c0_<arm><n> [--ctl-asi <asi>]
py -3.12 re/frida/c4_hud_compare.py --dir .../c4race_evidence/4464c0 --row 4464c0 --csv .../c4race_compare_4464c0.csv
```

Boot and touch nothing. The title idle timer starts the attract demo after 60 s (FUN_0043df00: mode `0xb` written at
0x0043df19, rule 6 at 0x0043df0f). No input hook is installed on this route.

### Why no Interceptor on the game thread

The old defect (a2-camera_RESULT.md) is a 4-byte frame shift that changes which uninitialised stack dword the three
callees copy into the camera entries' RwMatrix pad slots (`+0x68/+0x78/+0x88`). An Interceptor frame writes below
ESP, into exactly that garbage. So:

- **Call count:** a native stub at the entry (the E9 target on `late`/`ctl`, 0x004464c0 itself on `stock`). It only
  reads `[esp]` and increments globals, then runs the relocated first instruction (`mov eax,[0x00898994]`, 5 bytes in
  the original, `3E A1` 6 bytes in the naked port) and jumps back. No stack write. One counter for all calls, one
  for calls whose return address is 0x00448710 (the FUN_00448700 100-iteration loop), one for the last return address.
- **Observable:** a JS-thread poll (50 ms) of the entry array: count `0x00898994`, per entry the type `+4` and a
  class for each pad dword: `I<n>` small int, `S-<off>` stack address as distance below the main thread's stack
  top, `E<j>+<off>` pointer into the entry array, `<module>+<off>`, `F` float-shaped, `X` other. Raw values of the
  first 6 distinct samples per slot are kept in each JSON (`dump.raw`).
- **Compare rule:** a run matches its reference when the dominant key is equal and every key holding >= 2% of its
  polls also occurs in the reference.

### Results

| run | result | ref | game calls | seed-loop calls | last return | polls | keys | witness (dword at 0x004464c0) |
|---|---|---|---|---|---|---|---|---|
| stock1 | GREEN | stock2 | 2469 | 100 | 0x004468fe | 788 | 1 | `898994a1` (stock) |
| stock2 | GREEN | stock1 | 2370 | 100 | 0x004468fe | 787 | 1 | `898994a1` |
| **late1** | **GREEN** | stock1 | **2412** | 100 | 0x004468fe | 785 | 3 (dominant 99.75%) | `fb6b8be9` -> `c4race_fix.asi+0x3d050` |
| **late2** | **GREEN** | stock1 | **2351** | 100 | 0x004468fe | 793 | 1 | `b16b8be9` -> `c4race_fix.asi+0x3d050` |
| **ctl1** | **RED** | stock1 | 2392 | 100 | 0x004468fe | 785 | 1 | `b16b8be9` -> `c4race_ctl.asi+0x3d050` |
| **ctl2** | **RED** | stock1 | 2315 | 100 | 0x004468fe | 787 | 1 | `df6b8be9` -> `c4race_ctl.asi+0x3d050` |

The call counts are game calls through the inline JMP: the counter sits at the port entry, and the return
addresses are original code (0x00448710 in FUN_00448700, 0x004468fe in FUN_00446520). `MASHED_HOOK_ONLY=0x004464c0`,
so no other port is live, and FUN_00448700's own port is not installed. 0 crashes on all six runs.

Live state at the first call: mode 11, rule 6, `[0x0063ba8c]` 5, entry count 7.

Dominant key (every original and port run, 100% / 99.75% of polls):

```
n=7 0:I3,I3,F 1:I3,I3,I3 1:I3,I3,I0 1:I3,I3,I0 2:S-4c4,E3+0,E3+4c 2:S-4c4,E3+0,E3+4c 3:I0,I0,I0
```

Control key (ctl1, ctl2, 100% of polls):

```
n=7 0:I3,I3,F 1:I3,I3,I3 1:I3,I3,I0 1:I3,I3,I0 2:S-4c8,E3+0,E3+4c 2:S-4c8,E3+0,E3+4c 3:I0,I0,I0
```

- **e4/e5 +0x68:** `0x001afb3c` on stock1, stock2, late1, late2. `0x001afb38` on ctl1, ctl2 (4 bytes lower, the old
  frame). The main-thread stack sits at the same address on every boot here, so the raw value is reproducible too.
- **e0 +0x88** (class `F` in every arm, so not in the key): stock1/stock2 first samples `be2993fc, be29b3b4,
  be2a0a22, be2aa0c8, be2b8390`, late2 `be2993fc, be29990a, be29b3b4, be2a0a22, be2aa0c8`, ctl1/ctl2 `bfbd530c,
  bfbd52xx`. This is the third slot the a2-camera probe named (e0+0x88, e4+0x68, e5+0x68), so the control
  reproduces exactly the a2-camera RED set on a game-driven run.
- late1's two extra keys hold one poll each (0.25%): `2:S-25c` (the same transient the stock probe
  `probe/c4_hud_4464c0_probe_stock.json` caught once at race start) and `2:I13` once. Both are under the 2% rule.

Original noise: the two stock runs produced the same single key on every poll, and the same first six raw e0+0x88
samples. The port runs share the first sample (`be2993fc`) and differ from the stock runs only in which later float
values the 50 ms poll caught (the slot carries a float, class `F`).

**Verdict: C4 EARNED for the .asi copy.** Game-driven calls through the live JMP on the canonical attract
scenario (2351 and 2412 calls), output identical to the original's two runs, old-defect control RED on 2 of 2 boots
at the predicted slots.

## 0x0040d440 Course::LoadCurrent: stays C3, needs a user decision

### Scenario

```
py -3.12 re/frida/c4_hud_scenario.py --row 40d440 --arm stock|late|ctl --route qb --secs 10 \
    --out re/analysis/negctl/session_20261012b/c4race_evidence/40d440 --tag 40d440_<arm><n> [--ctl-asi <asi>]
py -3.12 re/frida/c4_hud_compare.py --dir .../c4race_evidence/40d440 --row 40d440 --csv .../c4race_compare_40d440.csv
```

statenav Quick Battle (the row-41db80 route). Per call: `[0x0063ba7c]`, `[0x0063ba78]` and `[0x005f2770]` at entry,
every FUN_0040d270 call made while the body runs (Interceptor at 0x0040d270, args), `[0x0063ba78]` after, and the
body's return address.

### Results

| run | result | game calls | FUN_0040d270 calls | witness (dword at 0x0040d440) | key |
|---|---|---|---|---|---|
| stock1 | GREEN vs stock2 | 1 | 1 | `63ba7ca1` (stock) | `sel=0 pre78=0 ret=492a6c F(OBJ,SEL) post78=SEL` |
| stock2 | GREEN vs stock1 | 1 | 1 | `63ba7ca1` | same |
| late1 | GREEN | 1 | 1 | `96331be9` -> `c4race_fix.asi+0x80760` | same |
| late2 | GREEN | 1 | 1 | `d0331be9` -> `c4race_fix.asi+0x80760` | same |
| **ctl1** | **GREEN (degenerate)** | 1 | 1 | `96331be9` -> `c4race_ctl.asi+0x80760` | same |
| **ctl2** | **GREEN (degenerate)** | 1 | 1 | `7b331be9` -> `c4race_ctl.asi+0x80760` | same |

`ret=492a6c` is the original caller FUN_004929d0 (call at 0x00492a67), so legs 1 and 2 hold: the game calls the
live port once per course load. `OBJ` = `[0x005f2770]` = `0x005f2728` on the probe run.

Leg 4 fails. The control reads `[0x0063ba78]` for argument 2, and on this route `[0x0063ba78]` = `[0x0063ba7c]` = 0
before the call, so the control passes the same value as the port. The store `[0x0063ba78] = [0x0063ba7c]` is also
0 -> 0, so a port that dropped it would read GREEN as well.

### Why every stock-save route is degenerate (static, capstone on `MASHED.exe.unpatched`)

- `[0x0063ba78]` has one writer in `.text`: 0x0040d45b, this row's own store. It sits in `.data` BSS, so it is 0 at
  boot.
- `[0x0063ba7c]` has one writer: 0x0040e174 in the setter 0x0040e170. Its callers pass `[0x0067f17c]` (0x0043e94d,
  0x0043f128, 0x0043f180), EBX at 0x0043f36c, and ESI = 0 at 0x0043dfa0 (FUN_0043df00, the attract demo: `xor esi,esi`
  at 0x0043df01). So the attract demo always loads course 0 (measured: probe `c4_hud_40d440_probe_attract2.json`).
- On the stock save, `[0x0067f17c]` stayed 0 on the Quick Battle track-select screen under controls 9, 10, 11 and
  12 (probes `c4_hud_40d440_probe_qb12.json`, `..._qb10.json`): 11/12 moved the menu cursor to -1, 9/10 changed
  nothing. Group H measured that the stock save opens only Bronze row 0 (`c4_a1_h_RESULT.md` section 4).

So the first course load of a boot always has selected = loaded = 0, and no natural stock-save route selects another
course.

### What would have to be forced (user decision)

A route with `[0x0067f17c]` != 0 at the first course load. Two options:

1. **Reuse group H's 0x004295a0 install copy (preferred, one decision covers 0x0041c2d0 too).** An install copy of
   `original/` whose `gamesave.bin` has the 26 launch-gate dwords set to 1 (`c4_a1_h_RESULT.md` section 4, span
   0x24A40), run with `--route cup --root <copy>\original`: Silver cup (mode 4) event 2, `[0x0067f17c]` = 2. Then
   `[0x0063ba7c]` = 2 and `[0x0063ba78]` = 0 before the call, and the control's argument 2 differs (0 vs 2). Group H
   measured this launch on 9 of 9 runs (`TRACKS\NEUSTEIN.PIZ`).
2. A save edit in an install copy that makes a second Quick Battle track selectable (needs a `gamesave_edit.py`
   field I have not identified; not investigated further).

The harness already has the cup route for row 4295a0. Rows `40d440` and `41c2d0` would need it wired (a few
lines: allow `--route cup` with `--root` for them).

## 0x0041c2d0 FUN_0041c2d0: stays C3, needs a user decision

### How the game reaches it (static, capstone on `MASHED.exe.unpatched`)

- Callers 0x0041a3e0 HudDispatchMode10 and 0x0041c300 HudDispatchMode5 only (U-9671 RESULT). "Mode10/Mode5" are
  **rule** values, not game modes. HudIngameDispatch 0x0040dfc0:
  - states `[0x0063ba8c]` 5/6: game mode FUN_0042f6a0 - 2 indexes 0x0040e140 = `[0,1,1,1,2,4,4,4,2,3]`, so modes 3, 4
    and 5 go to 0x0040e08a. There rule `[0x007f0fd0]` - 4 indexes the table at 0x0040e14c: rule 5 -> 0x0040e09e
    `call 0x0041c300`, rule 10 -> 0x0040e0bd `call 0x0041a3e0`.
  - state 7: index 0x0040e104 = `[0,1,1,1,2,3,3,3,2]`, so modes 3-5 go to 0x0040e014. There the table at 0x0040e110
    gives rule 5 -> 0x0040e028 `jmp 0x0041c300`, rule 10 -> 0x0040e043 `jmp 0x0041a3e0`.
  - Game modes 2, 6-11 never reach either caller.
- The callers' gates `[0x0063c628]` / `[0x0063cdbc]` have one writer each: 0x0041a217 in FUN_0041a1e0 (loads
  `BTBPanel.dff`) and 0x0041c14b in FUN_0041c100 (loads `KCPanel.dff`). Both write 1, and both are called only
  from FUN_00402750 (decomp_pc.py `--callers`). Measured at runtime: both read 1 throughout every run.
- Rules 5 and 10 come only from the cup event table 0x005f65c8 (`game_mode_rules_REmap_20260616.md` step 3): rule
  10 at mode 4 event 2 and 12, mode 5 event 5; rule 5 at mode 5 events 3, 7, 12. Mode 3 (Bronze) cells are rules 0
  and 4 only.

So the body runs only in a Silver or Gold cup event with rule 5 or 10. The stock save does not open those.

### Measured feasibility on stock-save routes (no forced input)

| run | route | 0x0041c2d0 calls | (mode / rule / state / flags) seen |
|---|---|---|---|
| `41c2d0/c4_hud_41c2d0_feas_attract.json` | attract | 0 | 11/6/5,6 with flags 1,1 |
| `41c2d0/c4_hud_41c2d0_feas_qb.json` | Quick Battle | 0 | 10/0/3..11 with flags 1,1 |

### What would have to be forced (user decision)

The same install copy as option 1 above (group H's 26-dword launch-gate save edit in a copy, `original/`
untouched). Then the Silver event 2 race (mode 4, rule 10) runs HudIngameDispatch -> 0x0041a3e0 -> 0x0041c2d0 in
states 5/6/7 (0x0040e0bd / 0x0040e043). That covers rule 10 only. Rule 5 (0x0041c300) would need a Gold event (mode 5,
event 3/7/12). The same edit opens the mode-5 gates per group H, but I have not measured that route.

The C4 observable for this row would be the group-H 41db80 method: swap each member's `[m+0x48]` to a recorder for
the call and record the dispatch order (expected 3, 1, 0, 2, from 0x0041c2d3..0x0041c2eb). The control would be the
pre-fix port (vtable deref, no pops), which AVs at `[vtbl+0x48]` per u9671_RESULT.md.

## Proposed tracker transaction (for the orchestrator via re-classify; not applied)

**hooks.csv 0x004464c0 CameraEntry::DispatchAll: C3 -> C4.** frida_diff ->
`re/analysis/negctl/session_20261012b/c4race_evidence/c4race_compare_4464c0.csv`. scenario -> `c4_hud_scenario.py
--row 4464c0 --route attract`. Append to notes:

> c4race 2026-10-12 C3->C4 (.asi copy): canonical attract demo (title idle, no input, mode 11 rule 6), hook live
> (0x004464c0 = E9 -> c4race_fix.asi+0x3d050, MASHED_HOOK_ONLY), game calls through the JMP 2412 / 2351 (100 each
> from the FUN_00448700 loop, rest from FUN_00446520, last return 0x004468fe), counted by a stack-neutral native stub.
> Entry-array pad classes identical to the original's two runs on every poll. Old-defect control (16-byte frame)
> RED 2/2 boots: e4/e5+0x68 = 0x001afb38 vs 0x001afb3c and e0+0x88 differs, the a2-camera slot set.
> re/analysis/negctl/session_20261012b/c4race_RESULT.md

The standalone keeps its own C body for this RVA (a2-camera_RESULT.md), so this C4 covers the `.asi` copy only.

**hooks.csv 0x0040d440 FUN_0040d440: stays C3.** Append to notes:

> c4race 2026-10-12 C4 attempt: game calls the live port once per course load from 0x00492a67 (ret 0x00492a6c),
> hook live, originals and port identical (Quick Battle x2 each). C4 refused: the injected-arg control reads GREEN.
> On every stock-save route [0x0063ba7c] = [0x0063ba78] = 0 at the call (sole writers 0x0040e174 / 0x0040d45b,
> attract passes 0 at 0x0043df9f, QB keeps [0x0067f17c] at 0). Needs a forced route with [0x0067f17c] != 0
> (user decision). re/analysis/negctl/session_20261012b/c4race_RESULT.md

**hooks.csv 0x0041c2d0 FUN_0041c2d0: stays C3.** Append to notes:

> c4race 2026-10-12 C4 attempt: not reachable on the stock save. Reached only from game modes 3-5 with rule 10
> (0x0040e0bd / 0x0040e043 -> 0x0041a3e0) or rule 5 (0x0040e09e / 0x0040e028 -> 0x0041c300), and those rules come
> only from Silver/Gold cup cells. The flags 0x0063c628 / 0x0063cdbc are written once at boot (0x0041a217 /
> 0x0041c14b). 0 calls on attract and Quick Battle. Needs the group-H launch-gate save edit in an install copy
> (user decision). re/analysis/negctl/session_20261012b/c4race_RESULT.md

**UNCERTAINTIES.md:** optional new row "C4 for 0x0040d440 / 0x0041c2d0 needs a forced route: group-H install-copy
save (Silver event 2) or equivalent. Pending user decision." No STUBS / DEFERRED change.

## Incidents

- One stock-arm 40d440 probe on the attract route (`probe/c4_hud_40d440_probe_attract.json`) AV'd inside FUN_0040d270
  at `MASHED.exe+0xe66db` (read @ 0x8, a NULL list argument to 0x004e66d0). Two other sessions' MASHED processes were
  running at the time. The re-run with the same command was clean. **Explained by the sibling CRASH-E66DB child
  (branch `work/crashcam`, its RESULT note documents it):** it is a cross-session **vehicle**-piz collision.
  FUN_0040d440 -> FUN_0040d110 mounts the vehicle-group piz (e.g. `VEHICLES\Advantag.piz`, opened exclusively for
  ~0.56 s during load). When another process holds it, the 0x0042a5d0 DFF loader returns 0, and FUN_004b3fc0 calls
  RpClumpForAllAtomics(NULL) -> 0x004e66db. Holding the track piz instead gives the U-9677 `+0x1509b4` signature.
  Same rule: re-run, not a finding. No other run of this session crashed.
- Two 4464c0 arms (first `ctl1`, `late1`) were ARM-FAILED because the counter did not accept the port's `3E A1`
  encoding. They were fixed and re-run, and the committed JSONs are the re-runs.
- Side effect of the harness change: the attract route no longer installs the 0x00497310 input hook, and that also
  applies to the existing row 403160 (which never sent input).
