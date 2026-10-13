# scr32 autosave toggle right arrow: ported from the bytes, widget GREEN (2026-10-13)

## Verdict: screen-32 widget RED 1 -> 0 at 0.01 px (default and strict stay 0). Screens 19 and 30 stay widget GREEN.

The standalone now places the autosave row's right arrow the way `FUN_00431240` does:
`x = (short)W * (ftol(FUN_004282a0(id, 0.7)) + L + 26.0) * 0.0015625`, all float ops, with `L = 356.0`.
`FUN_004282a0` is the exe `MeasureMenuMessage` from the scr19 fix. It returns the original's value **bit
for bit** for "Off", "On" and the "Autosave" label at 0.6, 0.7 and 0.8 (9/9). The value text moved to the
same function's x `L + 20 = 376.0` and scale 0.7. Branch `work/scr32arrow`, evidence
`verify/scr19_parity_20261012/scr32_fix/` (PROVENANCE.txt there has every command and PID).

The fix needed a second change. The original boots this toggle at **Off**, and the parity knob
`MASHED_SETTINGS_DEFAULTS=1` kept the standalone at **On**. With the faithful formula, "On" puts the arrow
at 408 against the original's 411 for "Off". That is a 3 px RED, worse than the old 0.93 px. The knob now
pins `autosave = 0`. The compiled default (1, user review #23) and every run without the knob are
unchanged. This is a separate commit, so it can be dropped on its own.

| Run (vs standalone, knob ON) | Before (`arrow_fix/`) | Formula only, autosave=1 | After (`scr32_fix/`) |
|---|---|---|---|
| scr32 default (`--tol-anim 4`) | widget RED 0 | n/a | widget RED **0** |
| scr32 strict | widget RED 0 | n/a | widget RED **0** |
| scr32 0.01 px | widget RED **1** (411.00 vs 410.07) | widget RED **1** (411.00 vs 408.00) | widget RED **0** (411.00 vs 411.00) |
| scr19 orig1 default / strict / 0.01 | 0 / 0 / 0 | n/a | 0 / 0 / 0 |
| scr19 orig2 default / strict / 0.01 | 0 / 0 / n/a | n/a | 0 / 0 / 0 |
| scr30 default / strict / 0.01 | 0 / 0 / 0 | n/a | 0 / 0 / 0 |

Non-widget rows are the same set as in `arrow_fix`. On every screen they are the 42 checker rows
(`_LogoOverlayDraw` sine phase, caller `0x42e89c`). Screen 30 adds the two preview quads at caller
`0x42e80c` (`0x474bd3` / `0x474d0c`), the same two as before. Screen 32 at 0.01 px adds the half-pixel
512x512 background pair (`0x473ebd` vs the standalone's -0.5 offset), also present before.

## Step 1: what FUN_00431240 computes (bytes, `MASHED.exe.unpatched`, capstone)

Stack: `sub esp,0x20` (`0x00431245`) plus four pushes, so locals are `[esp+0x10..0x2f]`.

- Row Y: `FUN_0042ac50` with `ecx = 0x1e` (`0x004312c4`, call `0x004312de`), `fild`/`fstp` to `[esp+0x18]`
  (`0x004312ed..0x004312f1`).
- Left-arrow base L (`[esp+0x14]`): `esi = FUN_0042ac00() * 0x34` (`0x004312bf`, `imul 0x004312e3`). Label id
  `[esi+0x898ab4]` when `edi == 1`, else `[esi+0x898ab8]` (`0x004312f7` / `0x004312ff`).
  `FUN_004282a0(label, 0.8)` (push `0x3f4ccccd` at `0x00431305`, call `0x0043130b`), then `fadd [esi+0x898aa4]`
  (`0x00431310`), then `fcom [0x005cd8e0]` (356.0, `0x00431319`). After `test ah,5` / `jp`, **below 356.0**
  stores `0x43b20000` = 356.0 (`0x00431328`). Otherwise it stores the sum `+ 20.0` (`[0x005ccd6c]`,
  `0x00431332`).
- Left arrow: `x = W * L * 0.0015625` (`[0x005cd5a8]`, `0x00431350`),
  `y = H * (Y - 8.0) * 0.0020833334` (`0x0043136e..0x00431376`), size `W * 0.025` (`0x00431390`). Sprite
  `FUN_0040bb50(0x5cd828)`, draw `call 0x004739f0` (`0x004313ed`).
- Value text: `esi = (FUN_0040ad20() != 0) + 0x59` (`neg`/`sbb`/`neg`/`add esi,0x59` at
  `0x00431411..0x0043141d`). `FUN_00427e00(esi, L + 20.0 [fadd 0x004313fe], Y, colour, 0.7 [push 0x3f333333 at
  0x0043140c], 0)` at `0x00431421`. The live text trace on the original (`../after_fix/orig_scr32.text.json`)
  records `(0x59, 376.0, 240.0, ff000000, 0.7, 0)`.
- Right arrow: `FUN_004282a0(esi, 0.7)` (`0x0043142c`), ftol `call 0x4a2c48` (`0x00431434`, truncation), then
  `fild W` (`0x00431449`), `fild ftol` (`0x0043144d`), `fadd L` (`0x00431451`), `fadd 26.0` (`[0x005cd8dc]`,
  `0x00431455`), `fmulp` (`0x00431461`), `fmul 0.0015625` (`[0x005cd5a8]`, `0x00431465`). Draw
  `call 0x00473870` (`0x00431484`), return address `0x00431489`. This is **float** math, unlike the insults
  row's integer `/640` (`0x00430eaa..0x00430ebf`).
- `FUN_0040ad20` is `mov eax,[0x008a95ac]; ret` (`0x0040ad20`). The writers of `0x008a95ac` are
  `0x004098c6` and `0x004098eb` (store 0, in the two reset bodies at `0x004098b0` / `0x004098d0`),
  `0x00404ec1` (copies `[0x00828254]`) and the setter `0x0040ad34` (called from `0x0043fda6` when the
  page is 0x20, plus `0x0043ffce` / `0x0044050d`).

### Measure table (original live return vs exe)

Original: `measure_probe32.py` at screen 32, PID 37696, game thread. Exe: `MASHED_DBG_MEASURE=1` lines in
`mashed_re.log` (`exe_measure_log.txt`). Live `[0x0067d830] = 0x3f7fffff`, the same value the scr19 probe
read, and the exe's constant. Live `[0x008a95ac] = 0`.

| id | text | scale | original FUN_004282a0 | exe MeasureMenuMessage |
|---|---|---|---|---|
| 0x59 | Off | 0.7 | 29.79607582 (`41ee5e5d`) | bit-equal |
| 0x59 | Off | 0.6 / 0.8 | 25.53949356 / 34.05265808 | bit-equal / bit-equal |
| 0x5a | On | 0.7 | 26.91258430 (`41d74cf9`) | bit-equal |
| 0x5a | On | 0.6 / 0.8 | 23.06793022 / 30.75723839 | bit-equal / bit-equal |
| 0x25f | Autosave | 0.8 | 102.15797424 (`42cc50e2`) | bit-equal |
| 0x25f | Autosave | 0.6 / 0.7 | 76.61848450 / 89.38822937 | bit-equal / bit-equal |

- Label term: the label call carried id `0x25f` and scale `0x3f4ccccd`, with ESI `0x34` (row 1). The
  record float `[0x898ad8]` is `0x42800000` = 64.0. 102.158 + 64.0 = 166.16 < 356.0, so L = 356.0. That is
  the 356 the standalone's left arrow already used. The original's left arrow draws at 356.00 in
  `orig_scr32.json`.
- Right arrow: "Off" ftol 29, `640 * (29 + 356 + 26) * 0.0015625f` = **411.000000**, which the original
  draws. "On" ftol 26 gives **408.000000**. The exe logs 411.000000 (bits `43cd8000`) with autosave=0 and
  408.000000 with autosave=1.

**Evidence for "On"**: this table. The original was not driven to "On", because that would need the
setting forced. Its "On" arrow follows from the measured `FUN_004282a0` return through the cited float
sequence. The exe produces the same return and the same sequence. Only "Off" is also covered by a
cross-side draw-list diff.

## Step 2: what changed (`exe_main.cpp`)

1. Commit `d62c8e25`, kToggle branch. Value text `DrawMashedString(val, 376.0f*S, rec.y*S, 0.7*0.0708*480*S)`,
   which was 378 and 0.6. Right arrow `dyn_rax = (W * ((float(int(MeasureMenuMessage(tid, 0.7))) + 356) + 26))
   * 0.0015625f`, which was `378*S + MeasureMashedString(tv, 0.6 law) + 9*S`. Plus a default-off
   `MASHED_DBG_MEASURE` that logs `MeasureMenuMessage` for 0x59/0x5a/0x25f at 0.6/0.7/0.8 and the toggle's
   arrow x once. The old `vth` (0.6 law) had no other user and was removed.
2. Commit 2, `LoadMenuSettings`: under `MASHED_SETTINGS_DEFAULTS=1`, `g_settings.autosave = 0`, citing
   `0x004313f5` / `0x008a95ac` and the live probe. Text y is unchanged.

Value-text pixel check (`value_text_ink.txt`, ink between the arrows). The original's ink is x 377..405,
y 233..246, n=197. The standalone's is x 377..405, y 235..247, n=204. **x extent is identical.** The best
IoU, 0.865, is at a 1 px upward shift of the standalone (dx 0, dy -1). So the value text sits about 1 px low.
Text y was out of scope here (the original passes y 240.0, the standalone uses its row anchor). This is
left as a follow-up, see below.

## hooks.csv / run_diff

`MeasureMenuMessage` is an exe-only body, so no Frida fixture can call it. Its evidence is the two probe
tables (scr19 9/9, scr32 9/9).

Proposed transaction (not applied, trackers untouched):

- `hooks.csv` row `004282a0`: level unchanged. Append:
  `| 2026-10-13 scr32: exe twin also bit-equal 9/9 at screen 32 (ids 0x59/0x5a/0x25f x 0.6/0.7/0.8), [0x0067d830]
  live 0x3f7fffff again; consumed by the autosave toggle arrow (FUN_00431240 0x0043142c..0x00431465),
  verify/scr19_parity_20261012/scr32_fix/measure_table.txt.`
- `hooks.csv` row `00431240` (FUN_00431240, frontend, C2, mapped): level unchanged (the standalone draw path
  is a layout port, not a hooked body). Optional note append: `| 2026-10-13 standalone autosave row arrow +
  value text ported from 0x00431305..0x00431465, scr32 widget GREEN at 0.01 px
  (verify/scr19_parity_20261012/scr32_fix/).`
- The open `[0x0067d830]` UNCERTAINTIES row proposed by the scr19 note gains a second screen (32) with the
  same live value.

## Proposed CHANGELOG line (not applied)

`2026-10-13 scr32 autosave toggle right arrow (standalone exe_main.cpp): x = (short)W*(ftol(FUN_004282a0(id,0.7))+L+26.0)*0.0015625 as float ops (0x0043142c..0x00431465), L=356.0 because label FUN_004282a0(0x25f,0.8)+[0x898ad8] = 102.158+64.0 < 356.0 (0x00431319/0x00431328); value text x L+20=376.0 / scale 0.7 per the 0x00431421 call. Exe MeasureMenuMessage bit-equal to the original's live return 9/9 at screen 32 (Off/On/Autosave x 0.6/0.7/0.8). MASHED_SETTINGS_DEFAULTS now pins autosave=0, the original's live [0x008a95ac] (FUN_0040ad20), and the compiled default stays 1. scr32 widget RED 1 -> 0 at 0.01 px (411.00 vs 411.00), default/strict 0. scr19 and scr30 stay widget GREEN. Evidence verify/scr19_parity_20261012/scr32_fix/, note re/analysis/negctl/session_20261012b/scr32_arrow_RESULT.md.`

## Not done / follow-ups

- The scr32 value text sits 1 px low (best IoU at dy -1, see above). The original passes y 240.0 to
  `FUN_00427e00`.
- L is held at 356.0, which the live label term proves for English. A longer label (other languages, where
  `FUN_004282a0(label,0.8) + 64 >= 356`) would move both arrows and the text to `sum + 20`. The standalone's
  left arrow was already a 356 literal, and that is unchanged.
- Decide whether the shipping default autosave (1) should follow the original's 0. That is a user decision
  (review #23). Only the parity knob changed here.
