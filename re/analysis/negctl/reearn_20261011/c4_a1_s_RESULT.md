# C4 re-earn, triage A1 group S (settings dialog + Championship::Complete), 2026-10-12

Brief: `brief_c4_a1.md` (group S). Branch `work/c4a1-s`. No tracker edits; verdicts below are
input for `re-classify`, not applied. Evidence: `evidence/c4_a1_s/`.

| RVA | Row | Verdict |
|---|---|---|
| 0x00498d60 | PopulateModeCombo | **NOT EARNABLE**: body unreachable on the patched reference (video dialog 0x65 never opens) |
| 0x00498f60 | VideoDialogInit_i3 | **NOT EARNABLE**: same |
| 0x00499170 | SubsystemSelectionChanged_i3 | **NOT EARNABLE**: same |
| 0x004991f0 | VideoSettingsDlgProc | **NOT EARNABLE**: same (it is the dialog's DLGPROC) |
| 0x00499740 | SetControlTextFromResource_i3 | **NOT EARNABLE**: all 33 call sites are inside dialogs 0x65 / 0x67 / 0x68, none of which opens |
| 0x00430290 | Championship::Complete | **C4 EARNED, with one forced input** (0x007f0fcc, see below). Game-driven call, hook live, original 2/2 self-identical, port GREEN 2/2 + late-load GREEN 1/1, old-defect control RED 2/2, no crash |

## Part 1. The dialog rows: reachability on the patched reference

Static record: `evidence/c4_a1_s/dialog_reachability.txt` (capstone-confirmed direct calls via
`re/tools/callsites.py`, absolute-dword pointer installs via `re/tools/dword_refs.py`, jmp/jcc scan
for tail transfers, gate bytes patched vs unpatched).

### Call graph (anchor image `MASHED.exe.unpatched`)

- **Video dialog 0x65.** `VideoSettingsDlgProc` 0x004991f0 has 0 direct callers and exactly one
  absolute reference: `push 0x4991f0` at 0x0049952f, the DLGPROC argument of
  `DialogBoxParamA(DAT_007e9580, 0x65, DAT_007e9584, 0x004991f0, 0)` (call `[0x5cc288]` at
  0x00499538) inside FUN_00499400. Every other video-dialog body hangs off it:
  - `VideoDialogInit` 0x00498f60: one call site, 0x004993a6, inside 0x004991f0 (WM_INITDIALOG).
  - `SubsystemSelectionChanged` 0x00499170: one call site, 0x00499279, inside 0x004991f0.
  - `PopulateModeCombo` 0x00498d60: two call sites, 0x00499137 (inside 0x00498f60) and
    0x004991cf (inside 0x00499170).
  - No jmp/jcc transfer reaches any of the four, and none has an absolute-dword reference.
- **Gate.** FUN_00499400 has one call site, 0x004951f6 in FUN_004951f0. In FUN_00499400 the
  dialog branch is taken only when the stack argument is non-zero: `cmp [esp+0x74], ebx` /
  `je 0x499555` at 0x004994df..0x004994e3 (decomp: `if (param_1 == 0)` silent path, else
  `DialogBoxParamA`). The argument is loaded at 0x004951f0:
  - unpatched `a1 bc 47 61 00` = `mov eax, [0x006147bc]` (initial value 1 in raw .data; written
    only by the `-vs0` / `-vs1` switches at 0x0049396e / 0x0049398e);
  - **patched reference `33 c0 90 90 90` = `xor eax, eax`** (`skip_selector`). The argument is
    always 0, so the dialog branch, and with it 0x004991f0 and all four bodies above, cannot run.
- **Controller dialogs 0x67 / 0x68.** FUN_00498510 has one call site, 0x004951b0 in
  FUN_00495150. Its dialog is opened only behind `mov eax,[ebp+8]` / `test eax,eax` /
  `je 0x498793` (0x00498771..0x00498776), pushing DLGPROC FUN_00498360 at 0x0049877a (the only
  reference to 0x00498360). The argument is loaded at 0x004951aa: unpatched
  `mov eax, [0x006147c0]` (initial 1, `-cs0` / `-cs1` at 0x004939ae / 0x004939ce), **patched
  `xor eax, eax`** (`skip_controller_dialog`). Dialog 0x68 (DLGPROC FUN_00497fd0, only reference
  `push 0x497fd0` at 0x00498304 in FUN_004982a0) is opened from FUN_004982a0, whose four call
  sites 0x00498419 / 0x0049844d / 0x00498471 / 0x00498483 are all inside FUN_00498360.
- **SetControlTextFromResource 0x00499740**: 33 direct call sites, no jmp, no pointer reference.
  0x004974c5..0x004975d0 (10) are in FUN_004974b0, whose only call site 0x004984e6 is in
  FUN_00498360 (dialog 0x67 WM_INITDIALOG). 0x00497cdb..0x00497e89 (15) are in FUN_00497b60, whose
  only call site 0x004981f1 is in FUN_00497fd0 (dialog 0x68 WM_INITDIALOG). 0x00498f73..0x00499041
  (8) are in 0x00498f60 (dialog 0x65). All three parents are unreachable on the patched reference.

### Verdict and re-pickup condition

NOT EARNABLE for all five, same class as ConfigLogError/Debug: no scenario on the diffing
reference can make the game call these bodies, so no game-driven call count can exist. This is a
property of the reference, not of the ports. Re-pickup: a user decision to run a reference
variant with `skip_selector` / `skip_controller_dialog` reverted (an install copy whose
MASHED.exe keeps the other eight patches), and a way to drive a modal Win32 dialog (the bodies
run on the dialog thread inside `DialogBoxParamA`; statenav's FUN_00497310 input override does
not reach them). Neither exists today.

## Part 2. Championship::Complete 0x00430290

### Caller and scenario

One caller: the tail-jmp `jmp 0x430290` at 0x004111a1 in FUN_00411170 (TimeTrialRecordPlayback,
the state-6 per-frame dispatcher), taken when FUN_00410d10 != 0 (0x0041118f..0x00411196) and
FUN_00410510 != 0 (0x00411198..0x0041119f). `callsites.py` reports 0 direct calls because the
transfer is a jmp. The interceptor's return address is therefore FUN_00411170's own return,
**0x004112d2** (call at 0x004112cd), seen on every call in every run.

Body gates: screen id DAT_0067e9fc in {3,4,5} (0x00430290..), then FUN_0040dd60 =
`(DAT_0063b90c != 1) - 1 & DAT_007f0fcc` (0x0040dd60), i.e. race concluded and P1 won.

Scenario (`re/frida/c4_champ_complete_scenario.py`): fresh MASHED from an **install copy**
(`robocopy original <scratch>\gameroot\original`, gamesave.bin reset to the pristine copy before
every run, SHA-256 `BD187881...` = `original\gamesave.bin`), because the body's tail calls
AutosaveTrigger. statenav Single Player entry, mode row 0 (Challenge), confirm to depth 5,
challenge index 0x0067f17c = 0 (Bronze Cup entry 0, TRAINING, unlocked in the stock save),
confirm to race, then c4_detect_e's in-race recipe (control 4, player 0, 200 ms at 1 Hz while
0x00771968 == 3). The race concludes after about 35 s and the game calls the body once.

**Natural run (no forcing), `explore_off1`:** mode 3, idx 0, 0x0063b90c = 1, **0x007f0fcc = 0**.
P1 does not win under the confirm-pulse driver, so the body returns at the guard: no grid change,
no post, no autosave. The control defect sits after the guard, so this scenario alone cannot
tell port from control (confirmed by the unforced on/control runs, see "Unforced runs").

**Forced input (`--force-win`):** at body entry, when 0x0063b90c == 1 and 0x007f0fcc == 0, the
harness writes 0x007f0fcc = 1, identically on every arm. Why this does not change the measured
path: 0x007f0fcc is read by the body only through FUN_0040dd60 in the guard (0x004302a8). It is
the P1-won flag FUN_00410510 writes on a P1 win (0x004107e3 path `DAT_007f0fcc = 1`), so the
forced state is one the game produces itself on a win. The caller, the call, the return address,
mode, index and the whole 0x007f0a40 grid are the game's own. The write happens before the body
reads anything, and the original and port arms see the same value.

### Hook live

- `on` arms: deployed `mashed_re_dev.asi` (SHA-256 `C5594D6B...`, copied into the install copy,
  identical to `original\mashed_re_dev.asi`) with `MASHED_HOOK_ONLY=0x00430290`. At arm time
  0x00430290 holds `e9 7b 71 c0` (b0 0xE9) and the target 0x71037410 = `mashed_re_dev.asi+0x17410`
  **equals the export `ChampionshipComplete`** (`tgt_is_export: true`).
- Late-load arms (`ctl`): `MASHED_RE_NO_AUTO_HOOK=1`, then Module.load + InjectHooks with
  `MASHED_HOOK_ONLY=0x00430290`. Target = export in `clean_ctl.asi` / `witness_mode3marks.asi`
  (`tgt_is_export: true`) in every run.
- `off` arms: same .asi, `MASHED_HOOK_ONLY=__none__`. First byte at arm time 0xA1 (stock).
- **Game-driven call count on the port: 1 per run** (interceptor entry-only on the E9 target,
  return address 0x004112d2), 1/1 in each of on1, on2, ctlclean1, wit1, wit2.

### Behavioural comparison

Channels per call, entry to return (`re/frida/c4_champ_complete_compare.py`):
- **grid**: every dword of 0x007f0a40..0x007f0cc0 the call changed;
- 0x007f0f2c; PostTrophyEvent cells 0x00898ab0 / 0x00899140 at return;
- AutosaveTrigger 0x004099a0 entries while the body is on the stack;
- PostTrophyEvent ids (orig 0x0042a920; port export). The port's ChampionshipComplete calls its
  PostTrophyEvent inline: 0 hits on the export, so for the port the post is read from the cells.

Original noise: `fw_off1` vs `fw_off2` identical on every channel. Original output (both):
grid `0x7f0a40 0->1, 0x7f0a44 1->2, 0x7f0a48 0->1, 0x7f0a70 0->1, 0x7f0a74 0->1, 0x7f0a9c 0->1`;
0x00898ab0 0->64, 0x00899140 0 (post id 0, `n` matched neither 2 nor 9, 0x004302de..0x00430332);
1 AutosaveTrigger entry; 0x007f0f2c 0.

Excluded channel: the copy's gamesave.bin SHA-256 after the run is `BD187881...` on every arm,
including the original, because AutosaveTrigger was entered but no file write happened before
the harness ended the process. It carries no signal and is not used.

| run | arm | calls | grid / cells / autosave vs fw_off1 | crash | alive at end |
|---|---|---|---|---|---|
| fw_off2 | off (original) | 1 | identical | 0 | yes |
| fw_on1 | on (deployed .asi, E9 live) | 1 | **GREEN** | 0 | yes |
| fw_on2 | on | 1 | **GREEN** | 0 | yes |
| fw_ctlclean1 | ctl, clean HEAD build | 1 | **GREEN** | 0 | yes |
| fw_wit1 | ctl, old mode-3 marks | 1 | **RED** | 0 | yes |
| fw_wit2 | ctl, old mode-3 marks | 1 | **RED** | 0 | yes |

### Control

Witness from `fix_fstate_RESULT.md` (defect 3): mode-3 marks back at +0x1c / +0x18 / +0x24
(`Cell(r + 0x007f0a5c/58/64) = 1` instead of 0x007f0a70 / 6c / 9c, CareerEvents.cpp lines 153..155),
built alone with `MASHED_NO_DEPLOY=1`, source then restored with `git checkout` + touch, rebuild
log shows `Save\CareerEvents.cpp (source newer)`. RED 2/2, on exactly the defect's cells:
grid `0x7f0a58 0->1, 0x7f0a5c 0->1, 0x7f0a64 0->1` present and `0x7f0a70`, `0x7f0a9c` missing
(0x7f0a6c was already 1 before the call, so it does not show). Every other channel matched.
The clean late-load twin (same load path, HEAD source) is GREEN, so the RED comes from the
defect, not from the late-load setup.

### Unforced runs

`evidence/c4_a1_s/unforced_compare.txt`: the unforced scenario (no `--force-win`) on
explore_off1 + nat_off2 (original), nat_on1 (hook live) and nat_wit1 (control). In all four
runs the game calls the body 1/1 from 0x004112d2 with mode 3, 0x0063b90c = 1 and
0x007f0fcc = 0. Every arm returns at the guard: no grid change, no post, 0 AutosaveTrigger
entries, no crash. The control reads **GREEN** there (1/1), so the unforced scenario is too weak
to separate port from control. The forced input is what makes the scenario discriminate.

### Survival

Every run alive at the end with 0 access-violation reports from the in-process exception
handler. The run covers boot, frontend, race, the body call and 12 s after it, when the game is
back on the frontend (depth 1, phase 3). Exit code 0xDEADBEEF is the harness's own
`device.kill` of its spawned PID.

### Scope limits

- Only the mode-3 win path with `n` = 0 marked rows and a single next-row mark (c = 1) ran.
  The 0x264 / 0x267 posts, the 0x7f0c54 bound and the mode-4 / mode-5 paths (0x268 / 0x269 /
  0x26a, 0x007f0f2c) did not run in the scenario. They remain covered by the C3 fixture
  (z_fix_fstate.py, 14 vectors) only.
- One forced input, 0x007f0fcc. A natural P1 win would remove it and needs a P1 driver that
  wins a Challenge race. None exists on the original side.

### Files

- Harness: `re/frida/c4_champ_complete_scenario.py`, compare: `re/frida/c4_champ_complete_compare.py`,
  pointer-install scan: `re/tools/dword_refs.py`.
- Run records: `evidence/c4_a1_s/c4_champ_*.json`, comparison: `evidence/c4_a1_s/forced_compare.txt`.
- Control build SHA-256: `witness_mode3marks.asi` `FC36E8EA...`, `clean_ctl.asi` `12DC4A86...`
  (scratch, not committed; `evidence/c4_a1_s/asi_sha256.txt`, witness build log
  `evidence/c4_a1_s/build_witness_log.txt`). Deployed `original\mashed_re_dev.asi` was not touched: `C5594D6B...`
  before and after.
