# scr19 row-3 right arrow: ported from the bytes, widget GREEN (2026-10-13)

## Verdict: screen-19 widget RED 1 -> 0 at default, strict and 0.01 px. Screens 30 and 32 unchanged.

The standalone now computes the insults-row right arrow the way `FUN_00430b90` does:
`x = ((short)W * (ftol(FUN_004282a0(id, 0.7)) + 0x17e)) / 640`, with `FUN_004282a0` ported for the exe as
`MeasureMenuMessage` (`exe_main.cpp`). That port returns the original's value **bit for bit** for all three
strings on the row at 0.6, 0.7 and 0.72 (9/9, table below). The value text moved to the same call site's
x 376.0 and scale 0.7. Branch `work/scr19arrow`, evidence `verify/scr19_parity_20261012/arrow_fix/`
(PROVENANCE.txt there has every command and PID).

| Run (vs standalone knob ON) | Before (`after_fix/`) | After (`arrow_fix/`) |
|---|---|---|
| orig1 default (`--tol-anim 4`) | 135/136 matched, widget RED **1** | **136/136** matched, widget RED **0** |
| orig1 strict | 93 matched, widget RED **1**, 42 checker | 94 matched, widget RED **0**, 42 checker |
| orig1 0.01 px | widget RED **1** (arrow) | widget RED **0** |
| orig2 default / strict | widget RED **1** / **1** | **0** / **0** (rest is checker `0x474464` only) |
| scr30 default / strict / 0.01 | widget GREEN | same row groups, frame for frame; widget GREEN |
| scr32 default / strict / 0.01 | 0 / 0 / right arrow 411.00 vs 410.07 at 0.01 | identical groups (the 0.93 px toggle arrow is unchanged, not touched) |

SA-vs-SA (`after_fix` vs `arrow_fix`, 0.01 px) on screens 30 and 32: 127 and 119 rows matched, the only
differences are the 42 checker rows (`_LogoOverlayDraw` sine phase). The toggle and gamma widgets did not move.

## Step 1: what FUN_004282a0 computes (bytes, `MASHED.exe.unpatched`, capstone)

- `0x004282a0`: `FUN_00427780(id)` -> `FUN_004277a0` (remap into a 0x400 stack buffer), then
  `fld [esp+0x414]` (scale) `fmul [0x005cd5fc]` (0.0708, bits `3d90ff97`) at `0x004282d7`, `fstp` to the arg
  slot, `FUN_005554d0(font=[0x0067d838], buf, scale*0.0708)`, `fdiv [0x0067d830]` at `0x004282f1`,
  `fmul [0x005cd618]` (640.0) at `0x00428309`, result left in ST0.
- `FUN_005554d0` (`0x005554d0..0x005555aa`): glyph table = `FUN_005c4d30([ctx+0x134])`; 16-bit codes when
  `[ctx+0x10] & 2` (`0x00555500`); loop while code > 0 (`jle 0x55559f`); code < 0x80 -> `movsx [ctx+code*2+0x24]`
  (`0x00555573`); else code-`[ctx+0x124]` checked against `[ctx+0x128]`, `movsx [[ctx+0x12c]+k*2]`, out of range
  -> -1 (`0x00555552..0x0055556e`); idx >= 0 -> `fld [tbl+idx*32]`, `fadd [ctx+0xc]`, `faddp` (`0x00555585..8b`);
  end `fmul [esp+0x28]` (the scaled scale, `0x0055559f`).
- Caller: `0x0043121a` call, `0x00431222` `call 0x4a2c48` (ftol: `fistp` then a toward-zero correction, so
  truncation), `mov edx,eax`, `jmp 0x430e99`; `0x00430e99` `call 0x42b8b0` (`mov ax,[0x67ea54]`), `movsx ecx,ax`,
  `add edx,0x17e` at `0x00430ea1`, `imul ecx,edx`, `0x66666667` / `sar edx,8` / sign add at
  `0x00430eaa..0x00430ebf` (signed /640), `fild` at `0x00430ecf`. The slider rows enter at `0x00430e94` with
  `edx = 0x66`, which is their 484.
- `[0x0067d830]` is not a constant. The font init zeroes it (`0x00427de4`); `FUN_004275d0` refills
  `0x0067d828..0x0067d834` through `FUN_00553cf0(.., 0x67d828)` (push at `0x004275f2`), a bounds walk over an
  RW-side list. Live value at screen 19: `0x3f7fffff` (0.99999994) on three boots.
- Precision: every return the probe read is exactly representable as a float, which is what x87 PC_24 produces.
  A float-op recompute (numpy float32, one rounding per x87 op) matches all nine returns bit for bit. With the
  divisor taken as 1.0 the recompute is 1 ulp low on all three, so the measured `0x3f7fffff` is required for bit equality,
  though not for these three ftol results.

Existing ports: `Frontend/MenuMenusB.cpp:MenuMenusBA` (asi only, naked copy calling the original's absolute
RVAs, C3) cannot run in the exe. The exe had only `MeasureMashedString`, which multiplies each glyph by the
cell width in pixels rather than scaling the sum, and the call site added it to a fitted 378 + 9.

### Measure table (original live return vs standalone)

Probe: `measure_probe.py` calls the original's own `FUN_004282a0` on the game thread at screen 19
(`orig_measure_probe*.json`, PIDs 21684 / 30380 / 17772, identical). Widths are the runtime record floats
(`ctx+0x0c` tracking = 0). Table text: `arrow_fix/measure_table.txt`.

| id | text | glyph widths (record +0) | original FUN_004282a0(id, 0.7) | port MeasureMenuMessage | ftol | arrow x orig / port | old standalone (0.72 law) |
|---|---|---|---|---|---|---|---|
| 0x59 | Off | .454545, .242424 x2 | 29.79607582 (`41ee5e5d`) | 29.79607582, bit-equal | 29 | **411 / 411** | 417.65 |
| 0x1b2 | Auto | .424242, .393939, .242424, .363636 | 45.17469788 (`4234b2e4`) | 45.17469788, bit-equal | 45 | **427 / 427** | 433.47 |
| 0x1b1 | Manual | .606061, .393939 x4, .212121 | 75.93193817 (`4297dd27`) | 75.93193817, bit-equal | 75 | **457 / 457** | 465.10 |

Also bit-equal at scale 0.72 (30.64739418 / 46.46540833 / 78.10141754) and 0.6 (25.53949356 / 38.72116852 /
65.08451843). The old standalone's 0.72 measure was already within 1e-5 of the original's 0.72 return, so the
whole old error was the formula (378 + 9 base, 0.72 scale, no ftol), not the font data.

**Evidence for "Off" and "Auto"**: this table. The original was not driven to those values (it would need the
insults setting forced). The original's arrow x for them follows from its measured `FUN_004282a0` returns
through the cited integer sequence, and the port produces the same returns and the same integer formula.
Only "Manual" is also covered by a cross-side draw-list diff.

## Step 2: what changed (`exe_main.cpp`)

1. New `MeasureMenuMessage(id, scale)`, header `// 0x004282a0`, next to `MeasureMashedString`. Text from
   `GetMenuMessage` (the existing `FUN_00427780` + `FUN_004277a0` remap), glyphs from `g_font` (the same
   FGDC20 record floats), codes above 0xff skip (ext range ends at 0x80 + 128), float sum of (w + trk), times
   `scale * 0.0708f`, divided by the measured `0x3f7fffff`, times 640.0f. The exe TU is SSE2, so each op rounds
   to float, matching PC_24.
2. kTriText branch: value text `DrawMashedString(val, 376.0f*S, ...)` at height `0.7 * 0.0708 * 480 * S`
   (was 378 and 0.72). Both from the call at `0x004311fd`: `push 0x43bc0000` (376.0) at `0x004311f7`,
   `push 0x3f333333` (0.7) at `0x004311f0`. The live text trace on the original records
   `(0x1b1, 376.0, 285.0, ff000000, 0.7, 0)` (`../orig1_settled.text.json`). Text y is unchanged.
3. Right arrow: `dyn_rax = ((short)kWidth * (int(MeasureMenuMessage(id, 0.7)) + 0x17e)) / 640`.
4. The toggle branch (screen 32) keeps its 378 / 0.6 / +9 law; only its comment was split from the tri-state one.
5. `re/tools/dual_copy_allowlist.txt`: one `CROSS-TARGET:004282a0` line (the lint fails the build without it),
   marked DELIBERATE, same shape as `0046b1c0`.

Value-text pixel check (tex 9 is outside the draw-list diff): backbuffers `orig_scr19_bb.bmp` (shim REQ dump)
and `sa_scr19_bb.bmp` (`MASHED_DBG_BBDUMP`). Ink between the arrows: original x 378..449, y 279..291;
standalone x 377..449, y 279..291. Dark-pixel IoU is highest at zero shift (0.815, next best 0.559 at +1 px).
The standalone draws 450 dark pixels against the original's 392. That is glyph raster weight, not position,
and stays with the font-raster item. The standalone's glyph quads went from 378.00..456.10 to 376.00..451.93
(`sa_knob_on.json`, tex 9).

## hooks.csv / run_diff

`MeasureMenuMessage` is an exe-only body, so no Frida fixture can call it. `hooks_registry.py` `menu_menus_ba`
tests the asi export `MenuMenusBA`, which this change does not touch. That entry is still the old
`arg_type 'none'` / `crash_equal_ok` form. The C3 re-earn used the negctl `scalars_to_scattered_globals`
fixture (`reearn_20261011/fix_menudraw_RESULT.md`). It was not re-run because its subject is unchanged.
The exe body's evidence is the probe table: the original's own function, live inputs, 9/9 bit-equal.

Proposed transaction (not applied, trackers untouched):

- `hooks.csv` row `004282a0` (MenuMenusBA, C3, MenuMenusB.cpp): level unchanged. Append:
  `| 2026-10-13 exe twin exe_main.cpp:MeasureMenuMessage (standalone; scr19 insults arrow). Same computation
  over g_msg_dat/g_font with [0x0067d830] held as the measured 0x3f7fffff (writer FUN_004275d0 ->
  FUN_00553cf0(..,0x0067d828) not ported). Bit-equal to the original's live return 9/9 (ids 0x59/0x1b2/0x1b1 x
  scale 0.6/0.7/0.72), probe verify/scr19_parity_20261012/arrow_fix/measure_table.txt. CROSS-TARGET allowlisted
  (DELIBERATE).`
- `UNCERTAINTIES.md`, new row: `[0x0067d830]` / `[0x0067d834]` derivation. `FUN_004275d0` fills
  `0x0067d828..0x0067d834` via `FUN_00553cf0`. The standalone holds `0x3f7fffff` as a measured constant (3 boots,
  screen 19). Missing evidence: the `FUN_00553cf0` input list and whether the value differs on other screens or
  resolutions. Path: decode `FUN_00553cf0` + the `0x00912a28` list, or probe `[0x0067d830]` on other screens.
  `[0x0067d834]` (the y twin) feeds the text law's y factor that `DrawMashedString` assumes 1.0.

## Proposed CHANGELOG line (not applied)

`2026-10-13 scr19 insults-row right arrow (standalone exe_main.cpp): new MeasureMenuMessage = FUN_004282a0 for the exe (FUN_005554d0 float sum x scale*0.0708 [0x005cd5fc] / [0x0067d830] measured 0x3f7fffff x 640.0 [0x005cd618]), bit-equal to the original's live return 9/9; arrow x = ((short)W*(ftol(w)+0x17e))/640 (0x0043121a..0x00430ecf); value text x 376.0 / scale 0.7 per the 0x004311fd call. scr19 widget RED 1 -> 0 (default, strict, 0.01 px; orig1 and orig2). Off/Auto arrows 411/427 by the measure table. scr30/scr32 unchanged. Evidence verify/scr19_parity_20261012/arrow_fix/, note re/analysis/negctl/session_20261012b/scr19_arrow_RESULT.md.`

## Not done / follow-ups

- Screen 32 toggle arrow 411.00 vs 410.07 (0.93 px, visible only at 0.01 px). Same family at `0x00431489`. Out
  of scope here, and `MeasureMenuMessage` makes it a small follow-up.
- `../after_fix/sumdiff.py` hard-codes the checker return `0xb8202`. Any exe rebuild that moves
  `_LogoOverlayDraw` makes its 0.01 px "other" count meaningless. Key it on the map symbol instead.
