# negctl re-earn: cluster SETTINGS (2026-10-12)

Branch `reearn/settings`, port commit `c22ad3de`. Bodies: `mashedmod/src/mashed_re/Save/SettingsDialog.cpp`
(0x00498d60, 0x004991f0) and `Save/SettingsAndIO.cpp` (0x00498f60, 0x00499170, 0x00499740). All five were
re-read from `original/MASHED.exe.unpatched` (capstone), whole function, jump tables included.

| RVA | hook key | verdict |
|---|---|---|
| 0x00498d60 | populate_mode_combo_s2 | FIXED |
| 0x004991f0 | video_settings_dlg_proc_s2 | FIXED |
| 0x00498f60 | video_dialog_init_i3 | FIXED |
| 0x00499170 | subsystem_selection_changed_i3 | FIXED |
| 0x00499740 | set_control_text_from_resource_i3 | FIXED |

## Why every defect was invisible, and the harness that now sees it

The five functions act only through USER32 calls on dialog-control HWNDs. The old entries called them with
hDlg = NULL (or `arg_type: none`), so every GetDlgItem/SendMessageA/SetWindowTextA was a no-op returning 0,
and an HWND read from the wrong register or stack slot changed nothing.

`re/frida/diff_template.js`, handler `scalars_to_scattered_globals`, gains three options (all inert unless named):

- `dialog {controls: [{id, cls, style, text?, check?, items?, sel?}]}`: before EACH arm a hidden STATIC popup
  (id 0) and its WS_CHILD controls (hMenu = id) are created on the A/B thread, so the target's USER32 calls run
  synchronously against them. An arg `{dialog: id}` resolves to that window's HWND. After the call the
  fingerprint folds a readback: per window its text (bytes outside 0x20..0x7e as `\xNN`), BM_GETCHECK for
  buttons, CB_GETCOUNT + each item's text and item data + CB_GETCURSEL for combos, and the focused id. HWND
  values never enter it. The readback is also sent as a `log` line, which is how the counts below were checked.
- `eax_arg k`: both arms go through `mov eax,[esp+4+4k]; jmp target` (0x00498f60 / 0x00499170 take hDlg in EAX).
- `stub_at` entry `{bare: true}`: that stub gets the bare callback returning `ret` even when a recorder exists
  (0x004c2f00 takes no args, so a recorder would read caller garbage).

`re/frida/run_diff.py` forwards `dialog` and `eax_arg` (whitelist). Fixtures:
`re/frida/negctl_fixtures/z_fix_settings.py` (mode tables, subsystem names and checkbox globals seeded through
the scratch, every observe window pre-filled 0xA5, freeze_main).

The fixture keeps every mode's flags word nonzero: the deployed reference carries the show_windowed patch at
0x00498dbc (`75 44` -> `90 90`), so the `flags == 0` filter cannot be diffed against it. The port keeps the
unpatched bytes. That one branch is verified statically only.

Regression (harness and touched files): `file_read_wrapper_i3` (C4, body in SettingsAndIO.cpp) GREEN 5/5;
`replay_save` (stub_at + recorder) GREEN 5/5; `intro_video_dim_getter` (scratch args) GREEN 5/5.
`read_mode_from_combo_j5` (0x00498d20, SettingsDialog.cpp) and `file_write_wrapper_i3` are C2 and were not re-run.

Evidence directory: `re/analysis/negctl/reearn_20261011/evidence/`. negctl run:
`runs/fix_settings/negctl_20261012_051609.tsv`, copied as `evidence/fix_negctl_settings.tsv`
(35 jobs: real GREEN 5/5, baseline GREEN 5/5, null RED 5/5, ret RED 1 + SKIP 4 (void), field_first/last
RED 4 + NA 1, field_none CTRL-GREEN 5/5).

All runs used `MASHED_NO_DEPLOY=1`. Final evidence was re-run after the witness sources were restored, touched,
and the build log showed `(source newer)` for both files.

---

## 0x00498d60 PopulateModeCombo (populate_mode_combo_s2)

**Verdict: FIXED.** arg_type `scalars_to_scattered_globals`, signature `void(pointer)`, cdecl both arms.

Defects (all from the bytes):
1. ABI: __cdecl with the combo HWND as its one stack arg. `0x00498dd9 mov ebx,[esp+0x60]` (after `sub esp,0x48`,
   four pushes and the push of the buffer); callers push it at `0x00499136` / `0x004991ce` and clean it at
   `0x00499149` / `0x004991d4`. The port read it from EAX. **(principal)**
2. Two port-only NULL guards on the heap tables (`*0x007731f8`, `*0x00773408`); the original has none.
3. The original re-reads both tables and DAT_0077340c every pass (`0x00498da0`, `0x00498e02..0x00498e14`);
   the port cached the mode-array pointer.
4. Format buffer: 0x40 bytes at `[esp+0x14]` under the /GS cookie (`0x00498d63..0x00498d72`); the port used 0x7f.

Unchanged and confirmed: signed `jl` filters `cmp [ebx],0x280` (`0x00498dbe`) and `cmp [ebx+4],0x1e0`
(`0x00498dc6`), flags test `0x00498dad..0x00498dbc`, stride 0x18 (`0x00498e18`), `CB_SETITEMDATA(pos, i)`
(`0x00498def..0x00498df9`), `DAT_00773418[i] = pos` (`0x00498dfb`), EBX = mode for `0x00498a00`.

Evidence: `fix_diff_populate_mode_combo_s2.csv` GREEN 8/8; `..._selfbaseline.csv` GREEN 8/8. negctl null RED 6/8,
field_first RED 6/8, field_last RED 6/8, field_none CTRL-GREEN 0/8, ret SKIP (void). Witness (HWND from EAX,
alone): `fix_witness_eaxhwnd_populate_mode_combo_s2.csv` RED 6/8 (the two GREEN vectors have count 0 / -1, so
the loop never runs).

hooks.csv: callers 0x00498f60 VideoDialogInit_i3 C2, 0x00499170 SubsystemSelectionChanged_i3 C2. Callee
0x00498a00 FUN_00498a00 C2 (plus USER32 SendMessageA).

[UNCERTAIN] A vector whose `0x00498a00` string was 67 chars (640x480x32, flags 0x102: "[Window][Interlaced]
[Fullscreen antialiasing mode 0]") made the original's call not return before Frida's script-load timeout, on two
boots (one with freeze_main, one without). The string overruns the 0x40 buffer into the /GS cookie. Where the
original stopped was not captured. The fixture keeps every string at or under 63 chars. Needs a U-row if the
overflow behaviour matters for the standalone.

Plates corrected: `re/analysis/promote_c2_settings_dialog/00498d60.md`, `re/analysis/settings_config_d3/00498d60.md`.

## 0x004991f0 VideoSettingsDlgProc (video_settings_dlg_proc_s2)

**Verdict: FIXED.** arg_type `scalars_to_scattered_globals`, signature `uint32(pointer, uint32, uint32, uint32)`,
stdcall both arms (b04 called the old export mscdecl), fold_ret.

Defects:
1. The four checkbox arms (jump table `0x004993b8`, byte index `0x004993d0`: 0x417 -> `0x004992d0`,
   0x418 -> `0x00499302`, 0x419 -> `0x00499334`, 0x41b -> `0x00499361`) do not test HIWORD(wParam). The port
   gated them on hi == 0. **(principal)**
2. The 0x3e8 arm (`0x00499225 je 0x00499275`) does not test HIWORD either; the port gated it on hi == 1.
3. Every exit is `ret 0x10`; the export returned with a plain `ret`.

Unchanged and confirmed: WM_INITDIALOG -> 0x00498f60 with EAX = hDlg (`0x0049939c..0x004993a6`, now also pushing
the three unused args as the original does), IDOK/IDCANCEL `hi != 0 -> return 1` (`0x00499251` / `0x00499232`),
`EndDialog(hDlg, 1)` / `(hDlg, 0)`, 0x3e9 `hi == 1` (`0x0049929e`), 0x3eb/0x3ec/0x3ed return 1, 0x3ea and 0x41a
return 0, 0x419 writes BM_GETCHECK raw and the other three `== 0` (neg/sbb/inc).

Evidence: `fix_diff_video_settings_dlg_proc_s2.csv` GREEN 31/31 (b04's 13 NULL-hDlg vectors plus 18 on the real
tree, including WM_INITDIALOG, the 0x3e8 arm with hi 0 and 1, IDOK/IDCANCEL with hi 0 and 1); self-baseline
GREEN 31/31. negctl null RED 27/31, ret RED 31/31, field_first RED 18/31, field_last RED 18/31, field_none
CTRL-GREEN 0/31. Witness (hi == 0 gate on the four checkbox arms, alone):
`fix_witness_hiwordgate_video_settings_dlg_proc_s2.csv` RED 5/31, exactly the five checkbox vectors with a
nonzero notification code (0x10417 and 0x5041b on NULL hDlg, 0x10417 / 0x30419 / 0x5041b on the real tree).

hooks.csv: caller 0x00499400 VideoSettingsDispatcher C2 (address taken at 0x00499530, passed to DialogBoxParamA).
Callees 0x00498f60 C2, 0x00499170 C2, 0x00498d20 ReadModeFromCombo_j5 C2 (plus USER32).

Plates corrected: `promote_c2_settings_dialog/004991f0.md` (its BN_CLICKED/CBN_SELCHANGE rows overstate the
gates), `settings_config_d3/004991f0.md`.

## 0x00498f60 VideoDialogInit (video_dialog_init_i3)

**Verdict: FIXED.** arg_type `scalars_to_scattered_globals`, signature `void(pointer)`, `eax_arg 0`.

Defects:
1. The first call `SetControlText(hDlg, DAT_007719e8 + 0x7e4)` (`0x00498f66..0x00498f73`) was missing; it sets
   the dialog title (string 2020 + language base, "Select Video Mode"). **(principal)**
2. `DAT_007731fc` holds a POINTER to the 0x50-stride name table (`0x004990f6 mov edx,[0x7731fc]`; `0x00498ba2`
   frees it). The port passed `0x007731fc + i*0x50`, i.e. bytes of BSS, as the name strings.
3. `CB_SETITEMDATA` wParam is i (`0x0049910a push esi ; push esi`), not the `CB_ADDSTRING` position.
4. `FUN_00498d60(hMode)` is __cdecl with the HWND pushed (`0x00499136`); the port called it __fastcall.
5. Order: both combo HWNDs are fetched before the checkbox block (`0x00499049..0x0049905b`); the subsystem
   count is re-read every pass (`0x00499114`).

Evidence: `fix_diff_video_dialog_init_i3.csv` GREEN 5/5; self-baseline GREEN 5/5. negctl null RED 5/5,
field_first RED 5/5, field_last RED 5/5, field_none CTRL-GREEN 0/5, ret SKIP. Witness (0x7e4 call omitted, alone):
`fix_witness_omit7e4_video_dialog_init_i3.csv` RED 5/5 (original title "Select Video Mode", witness "").

hooks.csv: caller 0x004991f0 C2. Callees 0x00499740 C2, 0x00498d60 C2 (plus USER32).

Plates corrected: `promote_c2_settings_dialog/00498f60.md`, `settings_config_d3/00498f60.md`.

## 0x00499170 SubsystemSelectionChanged (subsystem_selection_changed_i3)

**Verdict: FIXED.** arg_type `scalars_to_scattered_globals`, signature `void(pointer)`, `eax_arg 0`.
`stub_at`: 0x004c2e70 (recorder, 1 cdecl arg, observed at scratch+0x700) and 0x004c2f00 (bare, returns 3).

Defects:
1. `FUN_00498d60(hMode)` is __cdecl with the HWND pushed (`0x004991ce push esi`, `0x004991d4 add esp,4`); the
   port called it __fastcall, so the original body read stack garbage as its combo. **(principal)**

Confirmed unchanged: GetDlgItem 0x3e8 then 0x3e9, CB_GETCURSEL, equality early-out against DAT_0077340c
(`0x004991a1`), CB_GETITEMDATA, `DAT_0077340c = data` and `FUN_004c2e70(data)`, CB_RESETCONTENT,
`DAT_00773200 = FUN_004c2f00()`, CB_SETCURSEL with that value directly (not through DAT_00773418).

Evidence: `fix_diff_subsystem_selection_changed_i3.csv` GREEN 7/7; self-baseline GREEN 7/7 (first self-baseline
boot refused to run, "stub_at did NOT install at 0x4c2e70"; the second boot installed and passed; the normal run
installed on both of its boots). negctl null RED 5/7, field_first RED 5/7, field_last RED 5/7, field_none
CTRL-GREEN 0/7. Witness (__fastcall populate, alone):
`fix_witness_fastcallpopulate_subsystem_selection_changed_i3.csv` RED 4/7.

hooks.csv: caller 0x004991f0 C2. Callees 0x004c2e70 RwEngineSetSubSystem C3, 0x004c2f00 RwEngineGetCurrentMode C4,
0x00498d60 C2.

Plates corrected: `promote_c2_settings_dialog/00499170.md`, `settings_config_d3/00499170.md`.

## 0x00499740 SetControlTextFromResource (set_control_text_from_resource_i3)

**Verdict: FIXED.** arg_type `scalars_to_scattered_globals`, signature `void(pointer, uint32)`, cdecl both arms.

Defects:
1. ABI: __cdecl (HWND, id). The HWND is `[esp+0x108]` (`0x00499778`) and the id `[esp+0x10c]` (`0x00499752`)
   after `sub esp,0x104`. Every caller pushes id then HWND and cleans 8 (`0x00498f71..0x00498f84`, and 25 call
   sites in 0x004974c5..0x00497e89). The port took the HWND from EAX and used arg1 as the id. **(principal)**

Body confirmed: `LoadStringA(DAT_007e9580, id, buf, 0x100)` (`0x00499772`), `SetWindowTextA(hWnd, buf)`
(`0x00499785`), 0x100-byte buffer under /GS.

Evidence: `fix_diff_set_control_text_from_resource_i3.csv` GREEN 8/8 (ids 2000, 2013, 2020, 2021, 2110, 2121,
2240, 2251 from the RT_STRING table, several with non-ASCII bytes); self-baseline GREEN 8/8. negctl null RED 7/8
(the GREEN vector sets text on the CBS_DROPDOWNLIST combo, which keeps no text), field_first/field_last NA (no
memory writes), field_none CTRL-GREEN 0/8, ret SKIP. Witness (HWND from EAX, alone):
`fix_witness_eaxhwnd_set_control_text_from_resource_i3.csv` RED 7/8.

hooks.csv: callers 0x00498f60 C2, 0x004974b0 FUN_004974b0 C2 (sites 0x004974c5..0x004975d0), 0x00497b60
FUN_00497b60 C2 (sites 0x00497cdb..0x00497e89; nearest hooks.csv row at or below each site). Callees USER32 only.

Plates corrected: `promote_c2_settings_dialog/00499740.md`, `settings_config_d3/00499740.md`.

---

## For the parent (path2 and the C3 gate)

- **path2 will not see the dialog.** `run_verify_hook.py` and `verify_hook_install_template.js` do not know
  `dialog`, `eax_arg`, `recorder` or `bare`: a `{dialog: id}` arg resolves to 0 there and EAX is not delivered.
  path2 on these rows will run with hDlg = NULL. That still proves the inline JMP is live, but it is not a
  behavioural measurement. `stub_at` IS forwarded, so 0x004c2e70 stays stubbed in path2.
- The DlgProc export now ends `ret 16`; path2's `orig_calling_convention` is `stdcall` from the fixture.
- Every caller listed above is C2, and so are 0x00498a00 and 0x00498d20. Within the cluster the five call each
  other, so C3 for any one of them leans on the others' rows.
- No tracker was edited. U-row candidate: the 0x00498d60 buffer-overrun behaviour above.
