# negctl follow-up a2-dialog: 0x00498d20 + 0x0042f8d0 (2026-10-12)

Branch `work/a2-dialog`, base `4d6f7038`. Built with `MASHED_NO_DEPLOY=1` only. The shared `original\mashed_re_dev.asi`
was not touched and nothing under `original\` was written. No path2, no tracker edits, no re-classify.

**Which .asi run_diff used.** `run_diff.py` sets `ROOT` from its own location, so in this worktree `ASI_PATH` =
`.worktrees\a2-dialog\mashedmod\build\mashed_re_dev.asi` (Module.load into the reimpl arm). MASHED.exe is found
through the worktree's `original\` junction and spawned with `MASHED_RE_NO_AUTO_HOOK=1` (`run_diff.py:610`), so the
deployed .asi's hooks stay off and the original arm runs the original bytes.

| RVA | hook key | port body | verdict |
|---|---|---|---|
| 0x00498d20 | read_mode_from_combo_j5 | matches the bytes, unchanged | evidence set complete, ready for path2 |
| 0x0042f8d0 | menu_menus_bc | matches the bytes, unchanged | evidence set complete (field mutants NA, see below), ready for path2 |

Neither port body differs from the bytes. Both were re-read from `original/MASHED.exe.unpatched` with
`re/tools/disasm_fn.py`: 0x00498d20 is 21 instructions (0x00498d20..0x00498d54), and 0x0042f8d0 is 95 instructions
(0x0042f8d0..0x0042f9f8, no jcc or jmp). Ghidra boundaries agree: sizes 53 and 297 bytes. The witnesses are
therefore the ABI defect classes the old entries could not see, not a historical defect: the body of 0x00498d20 is
unchanged since `c341ca34`, and the 2026-05 defects of 0x0042f8d0 were already fixed on 2026-09-05.

Comment-only source edits: `MenuMenusB.cpp` said 102 instructions (it is 95) and now records the new path1
evidence; `SettingsDialog.cpp` said the body ends at 0x00498d52 (the `ret` is at 0x00498d54).

## Harness change (one, inert unless used)

`re/frida/diff_template.js`, `scalars_to_scattered_globals`: a test may carry its own `dialog` {controls} tree. Both
arms of that test use it instead of `CONFIG.dialog`. `CONFIG.dialog` must still be set, because it enables the
USER32 bindings. Without a per-test tree, behaviour is unchanged. Regression after the change:
`populate_mode_combo_s2` GREEN 8/8 and `video_dialog_init_i3` GREEN 5/5. No `run_diff.py` whitelist change was
needed, because tests are forwarded whole (`run_diff.py:286`).

## The blind spot in `eax_arg`, and the decoy slot

The `eax_arg` trampoline is `mov eax,[esp+4+4k]; jmp target`, so the register value is ALSO on the stack. A port
that read the argument from the stack instead of the register would get the same value and pass. Both fixtures
therefore put a DECOY stack slot ahead of the register value:

- 0x00498d20: args (decoy, hDlg), `eax_arg 1`. At entry, [esp+4] = decoy and EAX = hDlg.
- 0x0042f8d0: args (x, y, w, h, decoy, alpha), `eax_arg 5`. At entry, [esp+0x14] = decoy, which is where a port
  declaring alpha as a 5th stack arg would read it. EAX = alpha, with garbage in the upper 24 bits on two vectors.

Fixture: `re/frida/negctl_fixtures/z_fix_a2_dialog.py`. Both entries use `__replace__`. The `menu_menus_bc` entry
supersedes b09's and the retired arg_type-none entry.

---

## 0x00498d20 ReadModeFromCombo_j5 (`read_mode_from_combo_j5`)

**Why it was at C2.** It was demoted C4 -> C2 on 2026-10-11 (TRIAGE_2026-10-12.md row 44). arg_type `none` +
`crash_equal_ok` gave GetDlgItem/SendMessageA no window, so DAT_00773200 was 0 on every vector, and a port storing a
constant 0 passed (negctl null BLIND-DEGENERATE).

**Bytes.** `0x00498d27 push eax` (hDlg in EAX) -> `0x00498d28 call [0x5cc20c]` GetDlgItem(hDlg, 0x3e9) ->
`0x00498d40` SendMessageA(hCombo, 0x147 CB_GETCURSEL, 0, 0) -> `0x00498d4b` SendMessageA(hCombo, 0x150
CB_GETITEMDATA, cur, 0) -> `0x00498d4e mov [0x773200],eax` -> `ret` with EAX = the CB_GETITEMDATA result. ESI and EDI
are saved and restored. The port (`Save/SettingsDialog.cpp`: a naked `push eax` bridge into a stdcall C body) does
the same. Both call sites, 0x0049925e and 0x004992ab in FUN_004991f0, read only ESI after the call (`0x00499263 push
1; push esi` and `0x004992b0 mov eax,1`), so the port's ECX/EDX/EFLAGS at return are not consumed.

**Fixture.** `scalars_to_scattered_globals`, signature `int32(uint32 decoy, uint32 hDlg)`, `eax_arg 1`, `fold_ret`,
`freeze_main`, observe DAT_00773200 pre-filled 0xA5, plus the dialog readback. Each vector has its own tree: the
0x3e9 combo with its own items, item data and selection, and a decoy 0x3e8 combo (data 0x111/0x222/0x333, sel 1), so
a wrong control id reads a different value. The 12 vectors cover data 9, data 7, no selection (CB_ERR ->
CB_GETITEMDATA(-1) = CB_ERR), 0xDEADBEEF, 0x80000000, data 0, an empty combo, 10 items at sel 7 and sel 9, and three
NULL-combo paths: EAX = the combo itself, EAX = 0, and EAX = the decoy combo, each with the parent HWND in the stack
slot. 11 distinct fingerprints over 12 vectors (vectors 9 and 10 are both "return 0" on the same tree). The readback
log confirms each per-test tree was built (sel / items differ per vector).

| leg | result | file (`evidence/`) |
|---|---|---|
| run_diff | GREEN 12/12 | `diff_read_mode_from_combo_j5.csv` |
| --self-baseline | GREEN 12/12 | `diff_read_mode_from_combo_j5_selfbaseline.csv` |
| negctl null / ret / field_first / field_last | RED 12/12 each (field flip at 0x00773200 / 0x00773203) | `negctl_a2_dialog.tsv` |
| negctl field_none | CTRL-GREEN 0/12 | same |
| witness: hDlg read from the stack (`push dword ptr [ebp+8]` in place of `push eax`), alone | RED 11/12 | `witness_stackhwnd_read_mode_from_combo_j5.csv` |

The one witness GREEN is vector 5, whose decoy equals hDlg (both the parent). It is the designed control: the stack
slot is the only thing that differs.

**Callers/callees (Ghidra, `decomp_pc.py --callers --callees`, pool slot 0, read-only):** caller FUN_004991f0
(VideoSettingsDlgProc, C3). Callees are USER32 GetDlgItem and SendMessageA only.

**C4 is NOT earnable on the patched reference.** The only caller is 0x004991f0, the handler of dialog 0x65 (its one
reference is `push 0x4991f0` at 0x0049952f, inside FUN_00499400). That dialog opens only from the call site at
0x004951f6, and `skip_selector` writes `xor eax,eax` over its gate at 0x004951f0. With the boot recipe applied, the
game never executes 0x00498d20, so there is no canonical scenario that calls the live port. C3 (fixture-only) is the
ceiling while the patch is in the recipe. This is the same case as U-9674.

## 0x0042f8d0 MenuMenusBC (`menu_menus_bc`)

**Why it was at C2.** It was demoted C3 -> C2 on 2026-10-11 (TRIAGE_2026-10-12.md row 45). b09 passed x/y/w/h but
could not set AL, and it observed only x/y/z/rhw of the LAST quad in VBUF 0x00898a20. Neither colour was observed,
so a port wrong on alpha read GREEN.

**Bytes.** `0x0042f8d9 mov bl,al` is the only read of AL. Fill colour: `0x0042f8e7 shr cl,1`, bytes
0xf0/0x6e/0x14 -> 0x{a>>1}146ef0 (call 1). Border colour: `0x0042f92f mov [esp+0x27],bl` -> 0x{a}1050b4 (calls 2-5).
BX = 2.0f (0x005cc574, bytes 00000040), BY = 4.0f (0x005cc35c, bytes 00008040). There are five `call 0x472c60`
(0x0042f911, 0x0042f950, 0x0042f978, 0x0042f9a0, 0x0042f9e9), cdecl with 5 dwords each: `add esp,0x50` at
0x0042f9b1 cleans the first four calls, `add esp,0x14` at 0x0042f9ee cleans the fifth. The x87 stack is empty at
every call.

**Fixture.** `scalars_to_scattered_globals`, signature `void(float x4, uint32 decoy, uint32 alpha)`, `eax_arg 5`,
`freeze_main`. `stub_at 0x00472c60` installs the `recorder` (5 args, mscdecl, max 6 calls), and the observe window
is the recorder buffer (count plus 6x5 dwords). Nothing reaches RwGlobals or the vertex buffer: the observable is
every argument of every quad call, so x/y/w/h and both colours of all five calls. The port calls through
`g_menuBc_Quad` = 0x00472c60 (absolute), so both arms reach the same recorder. 9 vectors: alpha 0xff/0x80/0x00 on
the same geometry, alpha 0x01/0x7f, EAX 0xDEADBE81 and 0x123456fe (only AL is read), h = 1e-7 and 3.0000002 (the
`fst` at 0x0042f98d keeps h - 2 unrounded in ST0), and the 185/140/120/28 plate geometry.

**Predicted before reading.** The fingerprint was recomputed offline (numpy float32 + FNV-1a over count = 5 and the
five predicted calls) for 6 of the 9 vectors. All 6 match the captured values bit-exactly, including 0xDEADBE81 ->
alpha 0x81. So the recorder holds exactly the five calls the listing predicts, and nothing else.

| leg | result | file (`evidence/`) |
|---|---|---|
| run_diff | GREEN 9/9 | `diff_menu_menus_bc.csv` |
| --self-baseline | GREEN 9/9 | `diff_menu_menus_bc_selfbaseline.csv` |
| negctl null | RED 9/9 | `negctl_a2_dialog.tsv` |
| negctl ret | SKIP (void) | same |
| negctl field_first / field_last | NA (changed=0) | same |
| negctl field_none | CTRL-GREEN 0/9 | same |
| witness 1: AL ignored (`mov bl,0` in place of `mov bl,al`), alone | RED 8/9 | `witness_alignored_menu_menus_bc.csv` |
| witness 2: alpha read from the 5th stack slot (`mov bl,[esp+0x28]`), alone | RED 9/9 | `witness_alphastack_menu_menus_bc.csv` |

Witness 1's one GREEN is vector 2 (alpha 0x00), the only vector where `mov bl,0` is correct. Together the two
witnesses prove AL is read and that the decoy closes the `eax_arg` stack blind spot.

**Why field_first / field_last are NA, and why that is not a gap.** 0x0042f8d0 writes no global memory of its own.
Its stores go to its own frame and to the caller's argument slots (0x0042f933, 0x0042f996). With 0x00472c60
stubbed, its whole output is the argument stream of the five calls, so the write-flip mutants have nothing to flip.
null RED 9/9 plus the two witnesses are the mutation evidence for this row. The parent should decide whether that
satisfies the negctl leg. I consider it complete for a function whose only output is a callee's arguments.

**Callers/callees (Ghidra):** callers FUN_00434720 (C2), FUN_0043a610 AbilitySelectRender (C3) and FUN_0043aa30
TeamSelectRender (C3). Callee FUN_00472c60 ChromeBaseDraw (C3). There are 17 direct call sites in total
(`e8` scan of .text).

**C4.** It is reachable naturally: screens 15, 16 and 5 call it (hooks.csv row, draw-stream A/B 2026-09-05). C4 needs
the live port called in one of those scenarios, a call count, the original run twice, and an old-defect control
reading RED (witness 1 is a ready control). That was not attempted here.

## Ready for path2

`read_mode_from_combo_j5`, `menu_menus_bc`

## Proposed tracker changes (orchestrator applies)

- hooks.csv `00498d20` ReadModeFromCombo_j5: C2 -> C3 after path2. Replace the `frida_diff` pointer with
  `re/analysis/negctl/followup_20261012/evidence/diff_read_mode_from_combo_j5.csv`. Append to notes: "a2-dialog
  2026-10-12: real hidden dialog per vector (z_fix_a2_dialog.py), hDlg in EAX with a decoy stack slot; GREEN 12/12,
  self-baseline 12/12, negctl null/ret/field_first/field_last RED 12/12, field_none CTRL-GREEN; witness hDlg-from-stack
  RED 11/12 (vector 5 = decoy==hDlg control). Port unchanged, matches the bytes. C4 not earnable: the only caller
  0x004991f0 runs only inside dialog 0x65, which skip_selector keeps closed (U-9674)."
- hooks.csv `0042f8d0` MenuMenusBC: C2 -> C3 after path2. Replace `frida_diff` with
  `re/analysis/negctl/followup_20261012/evidence/diff_menu_menus_bc.csv`. Append to notes: "a2-dialog 2026-10-12:
  alpha driven in AL via eax_arg with a decoy 5th stack slot; 0x00472c60 replaced by a recorder, so all five quads'
  x/y/w/h/colour are compared; GREEN 9/9, self-baseline 9/9, negctl null RED 9/9, field mutants NA (no global
  writes), witness AL-ignored RED 8/9 (alpha 0 vector GREEN), witness alpha-from-stack RED 9/9; fingerprints match
  an offline prediction on 6/9 vectors. Port unchanged, matches the bytes (95 instructions)."
- UNCERTAINTIES.md U-9674: add 0x00498d20 ReadModeFromCombo_j5 to the row list (its only caller is 0x004991f0,
  call sites 0x0049925e and 0x004992ab).
- No new U-rows. No STUBS / DEFERRED change.

## Decisions for Mariano

None blocking. The standing U-9674 choice also covers 0x00498d20: accept C3 as the ceiling while `skip_selector` is
in the boot recipe, or measure against `MASHED.exe.unpatched` for these rows only.
