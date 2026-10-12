# negctl re-earn, cluster BOOTIO: result (2026-10-12)

Branch `reearn/bootio` (base 606a8bbd). Six rows, all re-ported from the bytes of
`original/MASHED.exe.unpatched` (`re/tools/disasm_fn.py`). Built with `MASHED_NO_DEPLOY=1`; the shared
`original\mashed_re_dev.asi` was never touched. No path2, no tracker edits, no re-classify.

| row | hook key | verdict |
|---|---|---|
| 0x00493480 | `fps_discretise` | FIXED |
| 0x004955b0 | `dinput_init_predicate` (+ `create_dinput_object_bool`, same RVA) | FIXED |
| 0x00499690 | `window_msg_pump` | FIXED |
| 0x004a4541 | `fsopen_safe` | FIXED (path1). path2 cannot install on the patched reference, see row |
| 0x00550980 | `vfs_stream_read` | FIXED |
| 0x00552c10 | `font_sys_init_render_state` | FIXED |

Evidence directory: `re/analysis/negctl/reearn_20261011/evidence/`. Negctl: `fix_negctl_bootio.tsv` (49 rows,
combined from `runs/fix_bootio/negctl_20261012_050009.tsv`, `..._050922.tsv` and `..._051101.tsv`; the
`source_run` column names the run of each row). Witness runs used ONE build with every hook's principal old
defect restored in its own body (each body carried exactly one defect), plus a second build for
`create_dinput_object_bool`. After the witnesses every source was restored with a fresh mtime, the build log showed
all seven TUs recompiled (`(source newer)`), and every `fix_diff_*` CSV was re-taken from that build.

Field mutants marked BLIND below flipped bytes OUTSIDE MASHED.exe's image, reached through entry registers
(NegCtl.cpp window (a)): 0x0e1ee504, 0x0d739088, 0x045c060e, 0x045e342e. None is a byte the port writes, none is
in an observed window. Each hook still has null RED and ret RED.

---

## 0x00493480 FpsDiscretise (`fps_discretise`), Boot/FrameDispatch.cpp

**Verdict: FIXED.**

Defects (disasm 0x00493480..0x00493531):
1. The remainder round-up and zero-snap ran only inside `accum > 49`. The original runs them on every call:
   `cmp ecx,0x32; jl 0x4934f4` (0x004934de) skips only the divide, then `mov [0x7719d4],ecx` (0x004934f7),
   `cmp ecx,0x2f; jle` (0x004934f4/0x004934fd) -> `inc edx` (0x004934ff) + zero (0x00493507),
   else `cmp ecx,3; jge` (0x00493502/0x00493505) -> zero (0x00493507).
2. All compares were unsigned. The original uses signed `jl/jg/jle/jge` (0x0049348d..0x004934cd,
   0x004934e1, 0x004934fd, 0x00493505).
3. The 200 band started at 0xc4. The bytes say `cmp eax,0xc5` (0x004934c1).
4. The snap wrote back to 0x007f1000. The original snaps EAX only (0x00493485..0x004934cf); the global is
   overwritten at 0x00493514. Not observable (the later store always runs), corrected for fidelity.
5. `DAT_007f1004` was `(float)u * (1/3000.0f)` in SSE. The original is `fild [0x7f1000]; fmul [0x5cc948];
   fstp [0x7f1004]` (0x0049351f..0x0049352b), one rounding at the live control word. Now an `__asm` block with the
   constant's bits 0x39aec33e held locally (the .rdata VA reads 0 in the exe, memory
   `naked-asm-rdata-reads-zero-in-exe`).

Fixture: new `z_fix_bootio.py`. b10's entry ran the real callee FUN_00493390, which stores
`[0x7f1000] = 0x32` unconditionally (0x004933d5), so ms was always 50 and no band edge was reachable. The new entry
replaces 0x00493390 with the scalar handler's recorder (0 args, cdecl; call count observed in scratch, so a port
that skips the call is RED) and seeds 0x007f1000 directly: every band edge in and out, negative ms and acc, the
0x2f/0x30, 0x31/0x32 and 2/3 remainder edges, and a wrapping sum. 30 vectors.

Evidence: `fix_diff_fps_discretise.csv` GREEN 30/30 (17 distinct fingerprints),
`fix_diff_fps_discretise_selfbaseline.csv` GREEN 30/30. Negctl (arg_type `scalars_to_scattered_globals`):
real GREEN, baseline GREEN, null RED, ret RED, field_last RED (flip 0x007f1007), field_none CTRL-GREEN,
field_first BLIND (flip 0x0e1ee504, harness memory). Witness (`fix_witness_remgate_fps_discretise.csv`):
remainder tests moved back inside `acc >= 50`, alone: RED 7/30.

Callers/callees (hooks.csv): caller sub_00492290 C2 (sole call site 0x004922c2). Callee FUN_00493390 C2 (stubbed
in the fixture; the port calls it through its VA like the original).

Plate correction: `re/analysis/boot_subsystem_d3/0x00493480.md` pseudo-code rewritten from the bytes (it carried
defects 1-3 and the port followed it), and its caller line corrected from FUN_00492d30 to sub_00492290
(`re/tools/callsites.py 0x00493480` finds one site, 0x004922c2).

Not changed: `Frontend/MenuNavSM.cpp FrameClockUpdate` is a second, standalone-only copy of this body. It already
runs the remainder tests on every call, uses 0xc5 and signed ints. Its float is `static_cast<float>(int) * k`, which
can differ from the x87 single rounding only for tick totals above 2^24.

## 0x004955b0 DInputInitPredicate (`dinput_init_predicate`), Boot/RwEngineInit.cpp

**Verdict: FIXED** (also `create_dinput_object_bool`, the second key at this RVA).

Defects (disasm 0x004955b0..0x004955bb: `call 0x495530; neg eax; sbb eax,eax; neg eax; ret`):
1. `DInputInitPredicate` returned `unsigned char`, so EAX bits 8..31 leaked from the callee. Now `std::uint32_t`
   0/1 (0x004955b5..0x004955b9).
2. `CreateDInputObjectBool` (Input/DirectInput.cpp, same RVA, a second body) called the port `CreateDInputObject`
   directly instead of the game VA 0x00495530, bypassing whatever owns that VA (in the fixture, the recorder),
   and faulted in the logger on every vector. It now forwards to `DInputInitPredicate`, so the RVA has one body.
   The file header lines were reworded so rva-lint does not see a second anchored body (DUP-INSTALL for the two
   RH_ScopedInstall lines stays allowlisted, `dual_copy_allowlist.txt:128`).

Fixture: b10 iteration 2 unchanged (recorder on 0x00495530 returning 0, 1, 2, -1, 0x80000000, 0x100, 0).

Evidence: `fix_diff_dinput_init_predicate.csv` GREEN 7/7, self-baseline GREEN 7/7;
`fix_diff_create_dinput_object_bool.csv` GREEN 7/7, self-baseline GREEN 7/7 (first run of the latter after the
restore build printed nothing, the second boot was GREEN). Negctl (`stub_dispatch_observe`), both keys:
real/baseline GREEN, null RED, ret RED, field_none CTRL-GREEN, field_first/last NA (no memory writes).
Witness: `unsigned char` return alone, `fix_witness_u8ret_dinput_init_predicate.csv` RED 3/7 (idx 3-5, the
demotion's vectors). For `create_dinput_object_bool` the `unsigned char` defect is invisible (its forwarder
zero-extends through the `std::uint32_t` declaration, run GREEN, CSV discarded); its own old defect, the direct
`CreateDInputObject()` call alone: `fix_witness_directcdo_create_dinput_object_bool.csv` RED 7/7 (`system error`).

Distinct fingerprints: 2 of 7 on both keys (the function's whole output is 0 or 1 plus the one recorded call).

Callers/callees: caller FUN_00495150 C2 (sole site 0x00495155). Callee CreateDInputObject 0x00495530 C2.

## 0x00499690 WindowMsgPump (`window_msg_pump`), Boot/Window.cpp

**Verdict: FIXED.**

Defects (disasm 0x00499690..0x004996e8):
1. On WM_QUIT the port returned the quit flag. The original returns 1 (`mov eax,1; ret` at 0x004996b0).
2. After TranslateMessage/DispatchMessageA the port returned without the WaitMessage gate. The original falls
   through to 0x004996cc (`[0x77391c] == 0` -> WaitMessage at 0x004996d5).

Fixture: b11 unchanged (PeekMessageA/TranslateMessage/DispatchMessageA/WaitMessage stand-ins, active flag 1).

Evidence: `fix_diff_window_msg_pump.csv` GREEN 5/5 (5 distinct), self-baseline GREEN 5/5. Negctl
(`standin_object_call`): real/baseline GREEN, null RED, ret RED, field_none CTRL-GREEN, field_first/last NA.
Witness: WM_QUIT returning the quit flag alone, `fix_witness_quitret_window_msg_pump.csv` RED 1/5 (idx 2, the
demotion's vector).

[UNCERTAIN] Defect 2 is not measurable against the reference: boot patch `no_focus_pause` (0x004996d3 JNZ->JMP)
makes the patched original skip WaitMessage at any active-flag value, so only active = 1 is compared. No vector
exercises active = 0. Consequence for the parent: with this hook LIVE in the dev .asi, an unfocused game calls
WaitMessage again (the old port already did so in the no-message branch). Needs a U-row if the parent wants it
tracked; resolution would be a reference without `no_focus_pause` (`--restore` on that one patch).

Callers/callees: caller sub_00492290 C2 (sole site 0x004922a0). Callees are USER32 imports only.

## 0x004a4541 FsopenSafe (`fsopen_safe`), Save/FsOpen.cpp

**Verdict: FIXED for path1.** Measurable against the patched reference: yes, for every return outside 1..0xffff.
path2: the HookSystem `kBootPatched` guard refuses to install over the `fix_fopen` JMP at 0x004a4541, so the hook
can never be live in the dev .asi. That is for the parent's C3 gate to judge; it is not fixable in the port.

Defect (disasm 0x004a4541..0x004a4553: `push 0x40; push [esp+0xc]; push [esp+0xc]; call 0x4a44e5; add esp,0xc;
ret`): the port called the DLL CRT's `_fsopen`, so the dev .asi returned a FILE* from a different CRT than the
game's `_fread`/`_fclose` expect. The .asi now calls the game `__fsopen` at 0x004a44e5 (cdecl, 3 args, popped at
0x004a4550); the standalone keeps its own `_fsopen` under `MASHED_STANDALONE` (no game CRT there).

The reference: patched 0x004a4541 is `jmp 0x0050f6c4`; the cave does the same three pushes and call, then
`cmp eax,0x10000; jae; xor eax,eax` (0x0050f6d6..0x0050f6dd). The port follows the unpatched bytes, so the two
differ only when __fsopen returns 1..0xffff. Fixture vectors use 0, 0x10000 and larger returns only.

Collateral fix, Input/DInput.cpp `ControllerConfigLoad_j5` (0x004971b0, C3): it called the `FsopenSafe` export
and then the DLL's `fread`/`fclose`. With FsopenSafe now returning a game-CRT FILE*, that mix would be wrong, so the
body now makes the original's three calls through their game VAs: `call 0x4a4541` (0x004971cd), `call 0x4a49cf`
_fread (0x004971ed), `call 0x4a4368` _fclose (0x004971f3). Regression diff below.

Fixture: new `z_fix_bootio.py`. The old `int_pair` (0, 0) entry faulted equally on both arms (crash_equal_ok).
A `stub_dispatch_observe` recorder replaces __fsopen 0x004a44e5, so nothing is opened; observed: the recorded
(filename, mode, shflag) triple, normalised to `b0+0,b1+0,0x40`, and the passed-through return. Names are files
the game does not use, modes read-only.

Incident: the first fixture draft had a vector `contcfg0.bin` / `wb`. The witness build's host `_fsopen` opened it
for real and created a 0-byte `original\contcfg0.bin` (CreationTime = LastWriteTime = 2026-10-12 05:02:33, the
witness run; `re/analysis/structs/contcfg_record.md:445` records that no contcfg*.bin lives in original\). I
removed that file and changed the vectors. The fix evidence, witness and negctl were all re-taken on the safe
fixture.

Evidence: `fix_diff_fsopen_safe.csv` GREEN 7/7 (5 distinct), self-baseline GREEN 7/7. Negctl
(`stub_dispatch_observe`): real/baseline GREEN, null RED 7/7, ret RED 7/7, field_none CTRL-GREEN,
field_last BLIND (flip 0x045e342e, harness memory), field_first NA (third boot; first boot ERROR on the stub
install guard, second boot process died). Witness: host `_fsopen` alone,
`fix_witness_hostcrt_fsopen_safe.csv` RED 7/7 (the recorder is never called).

Callers (all `call 0x4a4541`): sub_00496490 C2 (0x004964b3), ControllerConfigLoad_j5 C3 (0x004971cd),
FUN_00497230 C2 (0x0049724d), ConfigLoad C4 (0x00498969), ConfigSave C4 (0x004989c9), FUN_004b6710 C2
(0x004b671f). Callee FUN_004a44e5 C2.

## 0x00550980 VfsStreamRead (`vfs_stream_read`), Save/VfsStream.cpp

**Verdict: FIXED.**

Defects (disasm 0x00550980..0x005509a8):
1. `bytes_read / element_size` was signed. The original is `xor edx,edx; div esi` (0x005509a3..0x005509a5),
   unsigned 32-bit.
2. The port returned 0 for element_size 0. The original has no guard; `div esi` raises #DE. The port's unsigned
   divide now does the same.
3. `element_size * count` was a signed `int` multiply (UB on overflow). Now formed unsigned, the low 32 bits of
   `imul edx,[esp+0x10]` (0x0055098b).

Fixture: b13 unchanged.

Evidence: `fix_diff_vfs_stream_read.csv` GREEN 12/12 (12 distinct), self-baseline GREEN 12/12. Negctl
(`scalars_to_scattered_globals`): real/baseline GREEN, null RED, ret RED, field_first RED, field_last RED,
field_none CTRL-GREEN. Witness: signed divide + zero guard restored alone,
`fix_witness_signeddiv_vfs_stream_read.csv` RED 3/12 (idx 9-11, the demotion's three vectors).

[UNCERTAIN] element_size 0 (#DE on both bodies) is not in the vectors; the harness has no equal-fault comparison
for this handler.

Callers/callees: caller RwStreamWrite 0x004cbe80 C4 (sole site 0x004cbead). Callee is the indirect slot
`[[ctx+0x38]+0x34]` (`add eax,0x28` 0x00550998 + `call [eax+0xc]` 0x0055099d).

## 0x00552c10 FontSys_InitRenderState (`font_sys_init_render_state`), HUD/FontCtx.cpp

**Verdict: FIXED.**

Defect (disasm 0x00552c10..0x00552d0d): `mov [0x912bfc],eax` (0x00552ce1, EAX = 0 after `rep stosd` at
0x00552cd4) had no counterpart. Added. Every other store, value and the two callees were re-read against the bytes
and match (0x912b04, 0x912a84, 0x912be4, 0x912a50 |= 0x20003, 0x912bec, 0x912be0, 0x912bd8, 0x912bf0 = 7,
0x912a44/58/6c = 1.0f, the nine zero dwords 0x912a48..0x912a7c, the 0x100-entry word table at 0x912500, the
0x1f-dword clear from 0x912a88, 0x912c0c, 0x912bf8, 0x913204/08, 0x912c00/04 = 1.0f, return 1).

Fixture: b13 unchanged.

Evidence: `fix_diff_font_sys_init_render_state.csv` GREEN 3/3 (3 distinct), self-baseline GREEN 3/3. Negctl
(`scalars_to_scattered_globals`): real/baseline GREEN, null RED, ret RED, field_first RED, field_last RED,
field_none CTRL-GREEN. Witness: the 0x912bfc store dropped alone,
`fix_witness_no912bfc_font_sys_init_render_state.csv` RED 3/3.

Plate correction: `re/analysis/hud_promote_c2_b/0x00552c10.md` omitted the 0x912bfc store (the port followed it)
and gave the 0x912bf8 store at 0x00552d0b; both lines corrected (0x00552cdc / 0x00552ce1).

Callers/callees: caller FontSys_InitSeq 0x00552b60 C2 (sole site 0x00552b83). Callees FontCtxMatrix_AllocInit
0x004c57a0 C2, FontCtx_ResetTransform 0x00552750 C3.

---

## Regression: other C3/C4 hooks in touched files

Re-run on the final build (log/diff_<key>.csv, not copied to evidence):

| file | key (hooks.csv) | run_diff |
|---|---|---|
| Boot/FrameDispatch.cpp | state_phase_is_idle (C4) | GREEN 10/10 |
| Boot/FrameDispatch.cpp | state_phase_is_final (C3) | GREEN 10/10 |
| Boot/FrameDispatch.cpp | vehicle0_handle_get (C3) | GREEN 10/10 |
| Boot/FrameDispatch.cpp | display_active_flag_get / display_get_cursor_gate (C4) | GREEN 10/10, 10/10 |
| Boot/FrameDispatch.cpp | rw_vtable_slot07_call (C3) | GREEN 4/4 |
| Boot/FrameDispatch.cpp | main_loop_init (C4) | GREEN 4/4 |
| Boot/Window.cpp | window_show (C3) | GREEN 5/5 |
| Input/DirectInput.cpp | get_input_hinst (C3) | GREEN 10/10 |
| Input/DirectInput.cpp | rw_init_null_stub (C3) | INCONCLUSIVE-DEGENERATE 10/10 (bare RET; decision already says not re-earnable) |
| Input/DirectInput.cpp | bit_array_clear (C3) | GREEN 14/14 |
| Input/DirectInput.cpp | joypad_strcpy (C3) | GREEN 8/8 |
| Input/DInput.cpp | controller_config_load_j5 (C3) | INCONCLUSIVE-DEGENERATE 10/10, see below |
| Save/VfsStream.cpp | vfs_stream_get_type (C4) | GREEN 5/5 |
| HUD/FontCtx.cpp | font_ctx_set_scale (C3) | GREEN 10/10 |
| HUD/FontCtx.cpp | font_ctx_set_translation (C4) | GREEN 10/10 |
| HUD/FontCtx.cpp | font_matrix_push (C3) | GREEN 10/10 |

Boot/RwEngineInit.cpp and Save/FsOpen.cpp install no other hook.

**`controller_config_load_j5` is not covered where I changed it.** Its registry entry (`eax_implicit_int`, slots
0..9) returns 0 on every slot on both arms because no `contcfg*.bin` exists in original\, so `call 0x4a4541`
returns NULL and the body takes the "Unable to load" exit (0x0049720f). The file-present branch, where the
collateral edit swapped the DLL's fread/fclose for the game's 0x004a49cf/0x004a4368, never runs. This row has no
entry in the negctl decisions. Covering it needs a contcfg file in the game's CWD (the `re/frida/verify_rebind.py`
install-and-remove pattern) or recorders on 0x004a4541/0x004a49cf/0x004a4368 with an EAX-arg handler; I did
neither, to keep writes out of the shared install. Flagged for the parent.

## Notes

- A build failed once on a transient disk-full (`C1088 ... No space left on device`, C: free = 0). Nothing large
  from this session; free space returned to ~14 GB within a minute, and the retry built clean.
- No tracker edits. Rows needing U-rows: the `window_msg_pump` WaitMessage branch and the `vfs_stream_read`
  element_size 0 fault (both [UNCERTAIN] above). Both are reference/harness limits, not port doubts.
