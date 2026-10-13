# text3: screen 18/24 draw order and the screen-4 row count (2026-10-13, claude-opus-5-5)

Worktree `.worktrees/text3`, branch `work/text3`, parent `f1959b30`. Part A commit `54fb2368`, Part B in the
commit after it. Evidence `verify/scr19_parity_20261012/text3/` (provenance in its `PROVENANCE.txt`).

## Verdicts

| part | verdict | before | after |
|---|---|---|---|
| A, s18/s24 arrow order | **FIXED** | 12 arrows `reordered` per screen, every frame, at anim4 / strict / 0.01 px | 0 arrow and 0 plate rows on all 4 frames of s18 and s24 at all three tolerances |
| B, screen-4 row count | **FIXED** (derived, not a constant) | constant 3. Digit site `0x433efc` dy +17.0000 (3 of 4 paired) | count derived from the same inputs as the original: 4 on this machine. Digit site `0x433efc` dx/dy/dh 0.0000 on 4 of 4 |

Remaining RED in the touched widgets: the 6 standalone-only background band tiles (`60ffffff`, w 128) that
text2 already listed in the s18/s24 region box. They are identical before and after. Screen 4's bars and icon
are still ink-fitted (unchanged, text2 follow-up).

## Part A: the original's draw sequence on screens 18 and 24

All listings are from `original\MASHED.exe.unpatched` (`text3/bytes_*.txt`). The draw-list rows cite the fresh
captures `text3/dl_orig{18,24}.json`, which show the same sequence (the capture splits frames at Present, so
`--rotate-a 0x42e65a` puts the backdrop first).

1. **`FUN_0043c5b0`, frame head** (`bytes_43c5b0_head.txt`). Gated on `[0x0067eca4] < 5` (`0x0043c668`):
   - backdrop `FUN_0042e5b0` at `0x0043c673` (video quads + checker `0x42e89c`)
   - header bars `FUN_0042e3a0` at `0x0043c67b`
   - title/watermark text `FUN_00427e00` at `0x0043c6c2` / `0x0043c6f3` (rets `0x43c6c7` / `0x43c6f8`)
2. **`FUN_0043c5b0`, record loop** `0x0043c730..0x0043d266`. `esi` steps `0x34` up to `0x899104`. Per record:
   - fill, fade, top/bottom borders, left border (`0x43cf17..0x43d04e` selected, `0x43ccec..0x43ce1e` idle)
   - selected row Button cap `0x43d0b5`
   - label text (rets `0x43d164` / `0x43d10c`)
   - prompt-strip records, text through `FUN_00428140` at `0x0043d234` / `0x0043d255` (rets `0x43d239` /
     `0x43d25a`)
3. **`0x0043d26c` calls `FUN_0043bf30`** (`bytes_43d200.txt`). This is a flag-gated chain of overlays
   (`bytes_43bf00.txt`):
   - `[0x0067e7a8]` → `FUN_004335f0`
   - `[0x0067e7d8]` → `FUN_0043a610`
   - `[0x0067e7b0]` → `FUN_0042f0c0`
   - **`[0x0067e7f0]` → `FUN_0043af10` at `0x0043bf63`**
   - then `FUN_00434720`, `FUN_00430b90`, `FUN_00431240`, `FUN_004314b0`, `FUN_00431710`, `FUN_0043aa30`,
     `FUN_0042fb70`, `FUN_0042fe90`, `FUN_00430120`
4. **`FUN_0043af10`** (`bytes_43af10_full.txt`):
   - render state, then `[0x0067e7f0] != 0` → `FUN_004368e0([0x0067e7f4])` at `0x0043af8e`
     (`0x0043af79..0x0043af8e`), then `jne 0x0043b1bc` into the row loop
   - **`FUN_004368e0`** (`bytes_4368e0*.txt`):
     - preview frame `FUN_0042e8b0` at `0x00436921` (capture rows 54..61, rets `0x42ea94..0x42ebb9`)
     - panel box `FUN_00472c60` at `0x00436991` (row 62)
     - divider `FUN_00472c60` at `0x004369d3` (row 63)
     - jump table `jmp [ecx*4 + 0x439190]` at `0x00436a9b`, which reaches the car icons (`FUN_004739f0` at
       `0x00438144` / `0x00438179` / `0x004381b1` / `0x004381e6`, rows 64..67) and "vs" (`0x00438242` /
       `0x00438275` / `0x004382a8`, rows 68..70)
     - panel text `FUN_00427e00` at `0x00436b49..`
   - row loop: right arrow (`FUN_004739f0` at `0x0043b3de` s18 / `0x0043b201` s24), value text
     (`0x0043b524` / `0x0043b326`), left arrow (`FUN_00473870` at `0x0043b590`), per row (rows 71..82)
   - text block `FUN_00427e00` at `0x0043b6d5..0x0043b931`
   - power-up row from `0x0043b9c3`: 48x48 frame `FUN_00473870` at `0x0043bae9` and icon at `0x0043bbe5`,
     alternating (rows 83..90)
   - vehicle sprite `FUN_004739f0` at `0x0043bcf3` (row 91), the last quad of the frame

`0x4db7da` and `0x6f3f10cf` in the captured return chains of rows 54..70 are stale stack words, not callers.
The only callers of `FUN_004368e0` are `0x004392d0` and `0x0043af8e`.

**Standalone before:** for each row, plate + Button + label + right arrow + value + left arrow. After the loop
came the vehicle sprite, the car icons, "vs", the separator, then the power-ups.

**Standalone after** (`exe_main.cpp`, same draws and arguments):
- row loop: plate + Button + label
- separator (the divider)
- car icons, "vs"
- value pass: right arrow, value, left arrow per row
- power-ups
- vehicle sprite

The separator moved ahead of the cars because `FUN_004368e0` draws its divider at `0x004369d3`, before the jump
table. No quad overlaps it, so no pixel changes. The value pass keeps the old `g_font.ready()` gate.

**Not drawn by the standalone, and not added** (new draws, out of a draw-order brief): the preview frame
(`0x00436921`, 8 quads), the panel box (`0x00436991`) and the power-up frames (`0x0043bae9`). They stay
`missing` rows in the diff.

### Part A measurements

Fresh original captures `dl_orig{18,24}.json`. Diffs `diff_s<S>_{base,a}_{anim4,strict,tol001}.json`, bucketed
by `../text2/s18sum.py` in `drawlist_partA.txt`. The same against text2's captures is in
`drawlist_partA_t2orig.txt`, with the same result.

| screen | tolerance | base: arrow / plate / region | A: arrow / plate / region |
|---|---|---|---|
| 18 | anim4 | 12 reordered / 0 / 6 | 0 / 0 / 6 |
| 18 | strict | 12 / 0 / 6 | 0 / 0 / 6 |
| 18 | 0.01 px | 12 / 0 / 6 | 0 / 0 / 6 |
| 24 | anim4 | 12 / 0 / 6 | 0 / 0 / 6 |
| 24 | strict | 12 / 0 / 6 | 0 / 0 / 6 |
| 24 | 0.01 px | 12 / 0 / 6 | 0 / 0 / 6 |

All four frames are the same. The 6 `region` rows are the band tiles (`extra`, w 128, `60ffffff`). `mismatched:other`
goes 4 → 3: the divider now pairs.

**Collateral (sweep/):**
- Record multisets base → A are identical on all 33 screens apart from the 42 checker tiles (boot animation)
  and the s6/s7 tex-86 pulse that text2 listed (`multiset_base_vs_a.txt`). s18 is identical in content and
  differs in order only.
- Per-site text dy base → A is unchanged on every site (`sites_table.md`).
- `widget_red` is 0 on all 12 scr19a/19b/30/32 diffs (`drawlist_summary.txt`).

## Part B: `[0x00772ffc]`

### Writers and readers (`bytes_4967c0.txt`, `xref_772f00_773100.txt`)

An all-.text scan for any imm32/disp32 in `0x00772f00..0x00773100` finds two writers of `[0x00772ffc]`, both in
`FUN_004967e0`:

| address | instruction | effect |
|---|---|---|
| `0x0049680d` | `mov [0x772ffc], ebx` (ebx = 0) | zero |
| `0x00496825` | `inc dword ptr [0x772ffc]` | +1 per slot i = 0..3 with `FUN_00497450(i) != 0` (loop `0x00496813..0x0049682f`, `FUN_00496530` called first) |

There are two dword-index aliases:
- `FUN_00496920` reads `[i*4 + 0x772ffc]` at `0x00496924`.
- `0x00773000..0x0077302c` (= index 1..12) are written by the bit loop `0x00496836..0x00496866`.

No computed-base writer reaches index 0. Readers: `FUN_00496910` (`0x00496910`, called once, at `0x0043350c`) and
`FUN_00496920`.

`FUN_004967e0` runs every frame: the main loop `0x004922a0..0x004922ce` calls `FUN_00492d20` at `0x004922ae`,
which calls it at `0x00492d20`. A second call is at `0x004953be`.

### What it counts (mechanically)

`FUN_00497450(i)` (`0x00497450..0x0049746e`) returns 0 if i > 3, else `[0x007e96fc + i*0x200] != 0`.
That is the dword at `+0x13c` of slot i in the array `0x007e95c0`, stride `0x200`.

Writers of slot `+0x13c` (`xref_7e9400_7ea000.txt`, `bytes_498590.txt`, `bytes_4971b0.txt`, `bytes_slot_sites.txt`):

| address | function | writes |
|---|---|---|
| `0x004985c9` | `FUN_00498510` | `FUN_004b6520(0x007e95c0, 0x800)`: all four slots zeroed |
| `0x0049861a` | `FUN_00498510` | slot i < J: `+0x13c` = 1, `+0x140` = i, name via `FUN_00495830(i, slot+4)` |
| `0x0049865f` | `FUN_00498510` | slot i == J: `+0x13c` = 2, name copied from `0x007730b4`, scancodes `0x1f, 0x2d, 0x1e, 0x20, 0x1c, ..., 0xcb, 0xcd, 0xc8, 0xd0` |
| `0x004986cd` | `FUN_00498510` | other slots: `+0x13c` = 0 |
| `0x004971ed` | `FUN_004971b0(i)` | `FUN_004a49cf(slot, 1, 0x200, f)` from `contcfg<i>.bin` (name `FUN_00497190`, open `FUN_004a4541` at `0x004971cd`) |
| `0x00498703` / `0x00498758` | `FUN_00498510` | `rep movsd` save/restore: the file's copy is kept only when its dword `+0` and its name at `+4` equal the defaults (`0x0049870e..0x0049874b`), else the defaults are restored |
| `0x004974a0` | `FUN_00497470` | dialog: the value from two calls through `[0x005cc224]` (messages `0x147`, `0x150`). It is only reached from the dialog that `skip_controller_dialog` disables |

`J` = `FUN_00495790()` = `[0x00772fac]` (`0x004985b6`, `0x00495790`).
- Zeroed by `FUN_00496010` at `0x00496029`.
- Counted by `FUN_00496040`: it calls `[esi+0x10]` on the `FUN_00495520` object with `(4, FUN_00495ee0, &[0x00772fac], 1)`
  (`0x00496055..0x00496069`). That is the IDirectInput8 `EnumDevices(DI8DEVCLASS_GAMECTRL, cb, ref, DIEDFL_ATTACHEDONLY)`
  slot and argument order.
- The callback `FUN_00495ee0` calls `[ebp+0xc]` (CreateDevice) on the instance GUID at `lpddi+4`. It skips on a
  negative result (`0x00495f13..0x00495f15`), otherwise increments `*pvRef` at `0x00495fcb`. It returns 1 always
  (`0x00495fd0`), with no cap.

`FUN_00498510` has one caller, `0x004951b0`. The applied patch `skip_controller_dialog` (`0x004951aa`) replaces
its first argument `[0x006147c0]` with 0. The slot defaults and the `contcfg` merge still run, and only the
final dialog call (`0x00498771..0x0049878d`, gated on that argument) is skipped
(`bytes_495170_patched_vs_unpatched.txt`).

So, mechanically: `[0x00772ffc]` = the number of slots 0..3 whose `+0x13c` is non-zero. With no matching
`contcfg<i>.bin`, that is `min(J + 1, 4)`. J is the number of attached game-controller-class devices for which
CreateDevice succeeds.

### How screen 4 uses it

`FUN_004332a0` (`../text2/bytes_4332a0.txt`):
- base y = `Q_ListBaseY(FUN_00496910(), 0x22)` (`0x0043350c..0x0043351b`).
- It then walks 12 records at `0x0067eaf0`, stride `0xc` (`0x00433570..0x004335ac`). Record i gets x 85.0 / y and
  y += 34 only when `FUN_00496900(i)` (= `FUN_00497450`) is non-zero, else it is set to -1.

So the rows are the active slots, in slot order, and the count also sets the block's base y.

### Live on the original (read-only)

`text3/probe_devcount.py` spawns its own PID, does plain Memory reads 15 s after resume (no Interceptor, no
writes, no nav push), and kills that PID. Three boots, `live/devcount_boot{1,2,3}.json`:

| boot | PID | `[0x00772fac]` J | slot types `+0x13c` | `[0x00772ffc]` |
|---|---|---|---|---|
| 1 | 29896 | 3 | 1, 1, 1, 2 | 4 |
| 2 | 42072 | 3 | 1, 1, 1, 2 | 4 |
| 3 | 41220 | 3 | 1, 1, 1, 2 | 4 |

The three enumerated devices read as "Keychron Link", "Keychron Link" and "Keychron Q6 HE" (product names at
device `+4+0x12c`, trailing bytes past the name are unread garbage). No `contcfg*.bin` exists in `original\`, so
the defaults stand.

### Port

`exe_main.cpp` `DeriveInputSlots()`, called from `InitDirectInput` right after `DirectInput8Create`:
- `EnumDevices(DI8DEVCLASS_GAMECTRL, cb, &J, DIEDFL_ATTACHEDONLY)`. The callback counts each device whose
  `CreateDevice` succeeds and releases it.
- Then slot types `i < J → 1`, `i == J → 2`, else 0, and the active count.
- The standalone logs `input slots: game controllers J, types .., active n`.

Screen 4:
- `nrows4 = g_input_active_slots` (was 3).
- The row glyph comes from the slot type (2 = keyboard, as `FUN_0042bcb0` picks it) instead of "last row".

The standalone on this machine logs `input slots: game controllers 3, types 1 1 1 2, active 4`, the same J,
types and count as the three original boots.

[UNCERTAIN] The `contcfg<i>.bin` merge (`FUN_004971b0` + `0x0049870e..0x0049874b`) is not ported. A
`contcfg<i>.bin` whose dword `+0` and name match the defaults would override the type, and the standalone would
then disagree. Missing evidence: a capture with such a file present. None exists on this machine.

The historical 3-row original capture (`exe_main.cpp` comment, "orange area 106203 -> 141622 px between two
runs") fits J = 2 on that run. [UNCERTAIN] Which device was absent then: no record exists.

### Part B measurements

Build `b` (Part A + B) against `a` (Part A), all 33 screens (0..33 except 27). The references are the same
`../scr32_texty_recheck/orig_frame<S>.json` that text2 used, whose screen-4 capture has 4 rows. Files are in
`sweep/partB/`.

| screen 4, site `0x433efc` | A (n = 3) | B (n = 4 derived) |
|---|---|---|
| digit calls paired | 3 of 4 | 4 of 4 |
| dx / dy / dh | 0.0000 / +17.0000 / 0.0000 | 0.0000 / 0.0000 / 0.0000 on every row (y 189 / 223 / 257 / 291) |
| screen glyphs matched | 85 of 86, mean \|dy\| 0.6000 | 86 of 86, mean \|dy\| 0.0000 |

- Per-screen mean |dy| is 0.0000 on 33 of 33 screens (`quad_diff_sweep.txt`). `0x433efc` is the only site
  whose dy changed (`sites_table.md`). `0x43a8f1` stays unmatched from the `[0x0067e850]` state (text2 B3).
- Record multisets a → b (`multiset_a_vs_b.txt`):
  - Screen 4 gains 5 records: one more row = 3 bar quads + 1 icon + 1 digit glyph. The other 3 rows' bars,
    icons and digits move with the new count (bottom-anchored bars, digits on the law).
  - Every other screen differs only in the 42 checker tiles (s17: 0 records, s7: 1, the tex-86 pulse), with
    no glyph change. So
    the scr19/30/32 widget rows are unchanged from Part A's `widget_red` 0.

## Proposed CHANGELOG line

`2026-10-13 — Frontend s18/s24 draw order + screen-4 row count from the bytes (work/text3): s18/s24 values and
arrows now draw after FUN_004368e0's panel (FUN_0043af10 0x0043af8e before its row loop 0x0043b1bc, divider
0x004369d3 before the car icons, vehicle sprite 0x0043bcf3 last), 12 `reordered` arrows -> 0 on both screens at
anim4/strict/0.01 px; screen-4 rows = [0x00772ffc] derived as FUN_004967e0 counts it (slots 0..3 with type != 0,
FUN_00498510: i<J joypad, i==J keyboard, J = attached DI8DEVCLASS_GAMECTRL devices whose CreateDevice succeeds,
FUN_00495ee0 0x00495fcb), live on the original J=3 -> 4 (3 boots), standalone now 4, digit site 0x433efc
dx/dy/dh 0.0000 on 4 of 4 (was dy +17 at n=3), mean |dy| 0.0000 on 33/33 screens; contcfg merge not ported [UNCERTAIN]. Evidence verify/scr19_parity_20261012/text3/, note
re/analysis/negctl/session_20261012b/text3_RESULT.md.`

## Proposed U-row

`U-<next>: screen-4 input slots, contcfg<i>.bin merge not ported. FUN_00498510 loads contcfg<i>.bin over each
slot (FUN_004971b0, fread 0x200 at 0x004971ed) and keeps it when dword +0 and the name at +4 match the
defaults (0x0049870e..0x0049874b), so the file's +0x13c can change [0x00772ffc] and the row glyphs. The
standalone derives the defaults only (exe_main.cpp DeriveInputSlots). Resolve: a capture with a matching
contcfg file present, then port the merge. Blocks: nothing on a machine without contcfg files.`
