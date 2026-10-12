# negctl re-earn fix: cluster MENUDRAW (2026-10-12)

Branch `reearn/menudraw`. Port commit 94bc1313 (re-port from the bytes, capstone on
`original/MASHED.exe.unpatched`). All five rows FIXED. No tracker edits, no re-classify, no path2
(`run_verify_hook.py` is the parent's, it needs the deployed .asi). Every build used `MASHED_NO_DEPLOY=1`.
`original/gamesave.bin` sha256 `bd18788182b2343e...` unchanged after all runs.

Fixture: `re/frida/negctl_fixtures/z_fix_menudraw.py` (loads last, overrides b05/b08/b09 by key, re-uses their
helpers). negctl run: `re/analysis/negctl/reearn_20261011/runs/fix_menudraw/negctl_20261012_044213.tsv`, copied to
`evidence/fix_negctl_menudraw.tsv` (warm lane, slot 1, classes null,ret,field_first,field_last,field_none).

Harness finding that applies to the parent's path2 run: `menu_menus_ba`, `menu_sprite_dispatch_a` and
`menu_sprite_dispatch_b` need `boot_settle_s=8.0` (now in the fixture). Without it every A/B vector AVs on BOTH
arms (0x8 / 0x138) and every BA vector measures an empty string (string table at 0x0066d828 not filled yet), so
all twelve BA folds came out the same constant. The first BA GREEN of this session was that degenerate case; the
evidence below is from the settled re-run.

Regression: `menu_sprite_dispatch_c` (C3, same file) GREEN 8/8 on the final build; `menu_chrome_shell_a` (C4,
same file) GREEN 4/4; `menu_menus_be` (C3, same file as BA/BD) GREEN 5/5. `menu_menus_bb` / `menu_menus_bc` are C2
(not re-run as acceptance; BC has no path1 entry by design).

---

## 0x0040b460 MenuMenusBD (`menu_menus_bd`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED**

Defects (port `Frontend/MenuMenusB.cpp`):
1. Keys read at stride 0x10. Bytes: `mov edx,[eax*4+0x8a94f0]` @0x0040b470, a DWORD array at stride 4. The
   0x10 stride belongs only to the sign words at 0x007f1a14 (`add ecx,0x10` @0x0040b48b,
   `cmp ecx,0x7f1a54` @0x0040b48f).
2. Sort shape: the original swaps the key copy and the output in parallel (0x0040b4bc..0x0040b4d3) with an inner
   bound of 3, 2, 1 (`mov edi,3` @0x0040b49a, `dec edi` @0x0040b4db). The old port re-indexed fixed keys through
   the output over 3 full passes (same result on these inputs, not the bytes).
3. Mode 4/7 path returned early. Bytes re-read DAT_007f0fd0 after the FUN_00417740 loop
   (`mov eax,[0x7f0fd0]` @0x0040b503) and fall into the `== 9` test @0x0040b508.

Evidence: run_diff GREEN 11/11 `evidence/fix_diff_menu_menus_bd.csv` (b05's 7 vectors + modes 4 and 7 + signed
extremes 0x80000000/0x7fffffff + a 0x100/0x10000/0x1000000 key set); self-baseline GREEN 11/11
`evidence/fix_diff_menu_menus_bd_selfbaseline.csv`; negctl real GREEN 0/11, baseline GREEN 0/11, null RED 11/11
(calls=0), ret SKIP (void), field_first RED 11/11, field_last RED 11/11, field_none CTRL-GREEN; witness (key
stride 0x10 restored alone) RED 7/11 `evidence/fix_witness_stride10_menu_menus_bd.csv`.

Callers (E8 sites, owner = nearest hooks.csv row below the site): 0x0042d185 / 0x0042d1b2 (row 0x0042c960
CameraTransitionStateMachine C2), 0x00434870 (row 0x00434720 FUN_00434720 C2). Callee 0x00417740 C2.

Plate corrected: `re/analysis/frontend_promote_menus_b/0040b460.md` said `DAT_008a94f0` "+0x10 stride", the claim
the port was written from.

## 0x004282a0 MenuMenusBA (`menu_menus_ba`, arg_type `scalars_to_scattered_globals`, float return folded)

**Verdict: FIXED**

Defect: the C body stored the FUN_005554d0 ST0 result in a float and redid `/ DAT_0067d830 * 640.0` in SSE
double, one rounding. The original keeps it in ST0 through `fdiv [0x0067d830]` @0x004282f1 and
`fmul [0x005cd618]` @0x00428309 and returns ST0 (x87 rounding per op at the live precision control). Fix: naked
transcription of 0x004282a0..0x00428315 with the three calls through memory cells (FUN_00427780 returns EAX,
FUN_004277a0 takes EAX/EBX, FUN_005554d0 cdecl returning ST0). `/GS` cookie not ported (slot kept, displacements
are the original's, `jmp 0x004a2be9` becomes `ret`). File is ASI-only (`asi_sources.rsp`), so the absolute
`.rdata` reads in the naked body never run in the exe.

Evidence: run_diff GREEN 12/12 `evidence/fix_diff_menu_menus_ba.csv` (12 distinct folds; b08's 6 vectors plus
widths 800/1024/600/333/1280/720); self-baseline GREEN 12/12; negctl real GREEN 0/12, baseline GREEN 0/12, null
RED 12/12, ret RED 12/12, field_first/field_last NA (no observe window, the return is the only output),
field_none CTRL-GREEN; witness (the pre-fix C body restored alone) RED 4/12
`evidence/fix_witness_oldCbody_menu_menus_ba.csv`, including the demotion vector (id 40, size 8.0, width 800).
A first witness that only rounded ST0 to float inside the naked body stayed GREEN: on the harness thread the
FUN_005554d0 result is already float-exact, so the defect is the single double rounding, not the store.

Callers: 36 E8 sites, among them MenusLapTimeCmp 0x0042ed70 C3 (@0x0042ed7c), MenuSpriteDispatchC 0x0042fe90 C3
(@0x00430042), MenuModalRender 0x00433f40 C3 (@0x00433fd9), AbilitySelectRender 0x0043a610 C3 (@0x0043a882),
TeamSelectRender 0x0043aa30 C3 (@0x0043adfe), MenuSpriteDispatchA/B (@0x0042f2fd / 0x0042fd8b), the rest in C2
rows (0x004288a0 x2, 0x0042c960 x12, 0x00430b90, 0x00431240 x2, 0x004314b0, 0x00431710, 0x004335f0 x2, 0x00434720 x6,
0x0043af10 x2). Callees 0x00427780 FontText_StringTableLookup C4, 0x004277a0 TextCtrlCodeRemap C3,
0x005554d0 C2.

Plate `re/analysis/frontend_promote_menus_b/004282a0.md` already says float10; not changed.

## 0x0042f0c0 MenuSpriteDispatchA (`menu_sprite_dispatch_a`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED** (full re-port, `Frontend/MenuSpriteDispatch.cpp`)

Defects:
1. (6,0)/(8,0) render states skipped before the guard; the original issues them first (@0x0042f0d9/0x0042f0e6)
   and issues (6,1)/(8,1) on the early out (@0x0042f14e/0x0042f15b).
2. Selected row (mode 0) read at 0x0067ed44; bytes `mov eax,[edx+0x67ed40]` @0x0042f0ff with `shl edx,6`.
3. Selected row (mode != 0) read at ed80 + screen*0x10 with the -2 applied to the ROW; bytes subtract 2 from the
   SCREEN index (`add edx,-2` @0x0042f110) then `shl edx,6` / `[edx+0x67ed80]` @0x0042f113/0x0042f116.
4. No Arrow quads; bytes draw FUN_004739f0 (@0x0042f247) and FUN_00473870 (@0x0042f35b) on the selected row, the
   right one at x = ((int16)W * (_ftol(FUN_004282a0(id, 0.7)) + 0x120)) / 640.
5. Every row white; bytes draw the selected row and the Arrows in alpha<<24 with RGB 0 (@0x0042f175..0x0042f17f).
6. Text at x 281.0*scale / y 211.0*scale; bytes push 281.0 raw (@0x0042f2dd), y = 211.0 + 30.0 per row
   (@0x0042f1f0, `fadd [0x5cc72c]` @0x0042f3a1). Arrow y advances by ((int16)H*0x1e)/480 per row (@0x0042f36b..).
7. Steering/camera values other than 0/1 drew nothing; bytes `(v == 1)*8 + 0x61/0x62` (@0x0042f297/0x0042f2b2).
8. Alpha guard unsigned; bytes `jge` (signed) @0x0042f142.
9. Out-of-range difficulty keeps esi, which starts as the 211.0 bits 0x43530000 (@0x0042f1f8); the port matches.

Evidence: run_diff GREEN 9/9 `evidence/fix_diff_menu_sprite_dispatch_a.csv` (8 distinct folds, the two
guard-return vectors equal by construction); self-baseline GREEN 9/9; negctl real GREEN 0/9, baseline GREEN 0/9,
null RED 7/9 (calls=0), ret SKIP (void), field_first RED 6/9 (flip 0x00898a28), field_last RED 6/9 (flip
0x00898a8f), field_none CTRL-GREEN; witness (selected row read from 0x0067ed44 restored alone) RED 2/9
`evidence/fix_witness_sel0067ed44_menu_sprite_dispatch_a.csv`.

Not observable in this harness: the render-state calls themselves (only memory is fingerprinted), so defect 1 is
carried by the bytes, not by a RED.

Caller 0x0043bf30 FUN_0043bf30 C2 (@0x0043bf55). Callees 0x0042b8b0 C3, 0x0042b8c0 C4, 0x0040bb50 C3,
0x004739f0 C3, 0x00473870 C3, 0x00427e00 C2, 0x004282a0 C2 (this cluster), 0x004a2c48 C2.

Plate rewritten from the bytes: `re/analysis/promote_c2_frontend_menus/0x0042f0c0.md` (its
`(&DAT_0067ed80)[DAT_0067e9f8 * 0x10]` is DWORD-indexed and was read as a byte offset; it also listed
0x0067ea44 and a 0/1-only steering/camera table).

## 0x0042fb70 MenuSpriteDispatchB (`menu_sprite_dispatch_b`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED** (full re-port, `Frontend/MenuSpriteDispatch.cpp`)

Defects:
1. FUN_0042ac00 / FUN_0042ac50 called with (0,&dummy) / (0,0); bytes pass EDX = DWORD 0x0067ed38 (mode 0) or
   0x0067ed78 (else) + screen*0x40 (@0x0042fb91/0x0042fba2), then EAX = count, ECX = 0x1e (@0x0042fbb1); y is
   that int (`fild`/`fstp` @0x0042fbc5).
2. Render states issued before the guard; the bytes return before any (`jl 0x42fe29` @0x0042fb83).
3. No Arrow quads; bytes draw a left and a right Arrow on EVERY row (@0x0042fcea, @0x0042fde9), right x =
   ((int16)W * (_ftol(FUN_004282a0(id, 0.7)) + 0x13e)) / 640.
4. Every row white; bytes give the selected row (DWORD 0x0067ed40 / 0x0067ed80 + screen*0x40 == i) RGB 0
   (@0x0042fc1d..0x0042fc27).
5. Text x 311.0*scale, y base+scale; bytes 311.0 raw (@0x0042fd7a), y += 30.0 (@0x0042fdf2); Arrow y =
   (int16)H * (y - 8.0) * (1/480) per row (@0x0042fc63..0x0042fc79).
6. Player count drew only 0/1; bytes `(v != 0) + 0x59` (@0x0042fd31). Music out of range keeps row 1's id.
7. Alpha guard unsigned; bytes `jl` (signed).

Evidence: run_diff GREEN 8/8 `evidence/fix_diff_menu_sprite_dispatch_b.csv` (fixture points the two descriptor
tables at DIFFERENT lists, 2 groups vs 3 groups, so a swapped table shows); self-baseline GREEN 8/8; negctl real
GREEN 0/8, baseline GREEN 0/8, null RED 6/8 (calls=0), ret SKIP (void), field_first RED 6/8 (flip 0x00898a28),
field_last RED 6/8 (flip 0x00898a8c), field_none CTRL-GREEN; witness (descriptor-less calls restored alone:
FUN_0042ac00 EDX=0, FUN_0042ac50 ECX=0) RED 6/8 `evidence/fix_witness_nodesc_menu_sprite_dispatch_b.csv`.

[UNCERTAIN] not a U-row candidate about the port, recorded for the harness: esi (sprite id) starts as DWORD
[esp+0x18] (@0x0042fbe6), a stack slot this function never writes first. A Track value outside 0..3 draws that
indeterminate id on the original; the port draws 0. Every fixture vector keeps Track in 0..3.

Caller 0x0043bf30 C2 (@0x0043bfc5). Callees 0x0042ac00 MenuGroupCount C3, 0x0042ac50 C3, 0x0042b8b0 C3,
0x0042b8c0 C4, 0x0040bb50 C3, 0x004739f0 C3, 0x00473870 C3, 0x00427e00 C2, 0x004282a0 C2 (this cluster),
0x004a2c48 C2.

Plate rewritten from the bytes: `re/analysis/promote_c2_frontend_menus/0x0042fb70.md` (it named no inputs for
FUN_0042ac00/FUN_0042ac50, the claim behind defect 1).

## 0x0042e5b0 MenuChromeShellB (`menu_chrome_shell_b`, arg_type `scalars_to_scattered_globals`)

**Verdict: FIXED** (`Frontend/MenuSpriteDispatch.cpp`)

Defects:
1. phase = FUN_004a2c48() with nothing in ST0; bytes `fld [0x7f1010]` / `fmul [0x5cc728]` (60.0)
   @0x0042e65a/0x0042e660 then `call 0x4a2c48` @0x0042e698: _ftol of the unrounded product. Ported through a
   small asm helper (ChrB_Phase).
2. FUN_00474890 called with 1 argument; bytes `add esp,4` / `push eax` / `call 0x474890` / `add esp,0x24`
   @0x0042e7b1..0x0042e7ba: 9 arguments, the FUN_0042e590 result plus the eight slots FUN_0042e590 was called with.
   Ported through ChrB_ThunkCommit, which reuses the very same stack slots (FUN_0042e590 rewrites its own arg1).
3. Handles DAT_00898ab8 / DAT_008991a0 read once at entry; bytes re-read them before FUN_00473ee0
   (@0x0042e86a/0x0042e870).
4. Slide float = float(int) + 512.0 (two roundings); bytes `fild` + `fadd [0x5cd65c]` (one rounding)
   @0x0042e80f/0x0042e81c. Clamp tests the sign of the subtraction (`jns` @0x0042e848); ported on the wrapped
   uint32 difference.

Evidence: run_diff GREEN 10/10 `evidence/fix_diff_menu_chrome_shell_b.csv` (b08's 6 vectors plus mid-ramp
phases 0xe8 / 0xf6 / 0x1e5 / 0x1fa and slides 0x01000001, 0x0000000f, 0x80000005, 0x7fffffff; 10 distinct folds);
self-baseline GREEN 10/10; negctl real GREEN 0/10, baseline GREEN 0/10, null RED 10/10 (calls=0), ret SKIP
(void), field_first RED 6/10 (flip 0x0086eccc), field_last RED 10/10 (flip 0x008990e3), field_none CTRL-GREEN;
witness (empty-ST0 _ftol restored alone) RED 9/10 `evidence/fix_witness_emptyst0_menu_chrome_shell_b.csv`.

Caller 0x0043c5b0 MenuDrawLoop C4 (@0x0043c673). Callees 0x00473c20 C2, 0x004a2c48 C2, 0x0042e590
SpriteAnimFrameThunk C4, 0x00474890 C2, 0x00473ee0 LogoOverlayDraw C4.

Plate corrected from the bytes: `re/analysis/promote_c2_frontend_menus/0x0042e5b0.md` (its
`uVar2 = FUN_004a2c48() & 0x1ff` hid the ST0 input, its FUN_00474890 row stated no arity, and its
phase 0x000..0x0df row gave both alphas 0 where the bytes give A 0xff).

---

## Not done here (parent's)

- path2 `run_verify_hook.py` for all five keys, against the merged + deployed .asi.
- hooks.csv / CHANGELOG via re-classify. No new U-row is required by these ports; the B esi note above is a
  property of the original, not an open question about it.
