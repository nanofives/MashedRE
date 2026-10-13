# text4: screen 18/24 preview frame, panel box, power-up frames (2026-10-13, claude-opus-5-5)

Worktree `.worktrees/text4`, branch `work/text4`, parent `23d53fed`. Evidence `verify/scr19_parity_20261012/text4/`
(provenance in its `PROVENANCE.txt`).

## Verdict

**FIXED.** s24 is GREEN at anim4: 174 of 174 rows matched on all 4 frames. s18 matches every row except the
animated checker. At strict and 0.01 px the only rows left on either screen are:
- the checker (fsin jitter, the documented animation noise)
- the video quad's −0.5 px offset at 0.01 px, which is on every screen and is outside this brief

Draws in this sequence that the diff showed as original-only: 0 left. Standalone-only: 0 left. RED: 0. Reordered: 0.

What was added or corrected (all byte-cited in `exe_main.cpp`):

| draw | original | before | after |
|---|---|---|---|
| preview frame, 2 crossfade layers × 2 halves + 4 borders | `FUN_0042e8b0` at `0x00436921` | not drawn | ported (`DrawPreviewFrame`, `PreviewPairQuad`) |
| backdrop slide `+= 0x20` | `0x0042ebb9..0x0042ebc7` | not done | done. FUN_0042e5b0's clamp to `0x200` (`0x0042e856..0x0042e865`) added |
| panel box | `FUN_00472c60(32, 300, 576, 96, (2a/3)<<24)` at `0x00436991` | not drawn | drawn |
| divider | `FUN_00472c60(319.0, 308, 2, 80)` at `0x004369d3` | x 319.5 (ink) | x 319.0 (`0x439f8000` @`0x004369c5`) |
| "vs" × 3 | `FUN_004739f0("vs", 72/136/200, 316, 56, 56)` at `0x00438242/75/a8` | 63×63 at 104.5+64c−31.5, 309 (ink) | from the bytes |
| power-up frame × 4 | `FUN_00473870("Powerupshadow", 558−38c, 372, 48, 48, .., 0)` at `0x0043bae9` | not drawn | drawn before each icon, INTERFACE.TXD, point filter |
| vehicle sprite | `FUN_004739f0(car, 456, 144, 152, 152, .., 1, 0)` at `0x0043bcf3` / `0x0043bc77` | 458, 147, 145×141 (ink) | from the bytes, point filter |

**The 6 "standalone-only band tiles" are not extra draws.** The original draws the same `FUN_00473ee0` band strips
on these screens, 544 px further right. `FUN_0042e8b0` adds `0x20` to the slide `[0x008990e0]` on every call.
The backdrop reads `float(slide) + 512` and then steps the slide by `−0x10`, clamped to `0x200`. So on s18/s24 the
slide holds at `0x220`, f = 1056, and the band is at x = 928. That is exactly the captured value. Porting the write
moves all 32 band rows (the 6 in the region box included) onto the original's: 32 missing + 32 extra → 0. Nothing
was removed.

## The bytes

### `FUN_0042e8b0` (preview frame, `0x0042e8b0..0x0042ebd0`), called by `FUN_004368e0` with its alpha byte a

**Phase and crossfade:**
- `t = ftol([0x007f1010] * 60.0 [0x005cc728]) & 0x1ff` (`0x0042e8b3..0x0042e963`; `FUN_004a2c48` truncates).
- `t == 0x1e0` → `[0x0067ea58] += 2` (`0x0042e973`). `t == 0xe0` → `[0x0067f0bc] += 2` (`0x0042e9a5`).
- `[0x0067f0bc]` even → +1 (`0x0042e97f..0x0042e985`).
- Alphas `cA` / `cB` (`0x0042e98b..0x0042e9fd`):
  - `t < 0xe0`: cA 0xff, cB 0
  - `t < 0x100`, d = t − 0xe0: d < 0x10 → cB d<<4, cA 0xff. Else cA (0x1f−d)<<4, cB 0xff
  - `t < 0x1e0`: cA 0, cB 0xff
  - else, d = t − 0x1e0: d < 0x10 → cA d<<4, cB 0xff. Else cB (0x1f−d)<<4, cA 0xff
- Each layer's colour is `(c·a/255)<<24 | 0xffffff` (`0x80808081` multiply).

**Each layer** is `FUN_00474890(FUN_0042e590(n), 330, 96, 270, 180, colour, p, mode, 0)`:
- rect `0x43a50000 / 0x42c00000 / 0x43870000 / 0x43340000` (`0x0042e93e..0x0042e956`)
- `mode = {0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3}[slot & 0xf]` (`0x0042e8f2..0x0042e92e`)
- A: `p = (((t+0x20) wrapped below 0x200) << 8) / 288` (`0x0042ea42..0x0042ea78`)
- B: `p = ((t−0xe0) << 8) / 288` (`0x0042eac2..0x0042eade`)
- `FUN_0042e590(n)` = `FUN_0040bb70(name[0x005f79d8 + (n + [0x0067f17c]·2)·4])` (`0x0042e590..0x0042e5a8`). The table
  holds 26 TrackImages names (Training1 .. City-at-Night2).

**Borders**, `FUN_00472c60` in black with alpha a (`0x0042eb05..0x0042ebb4`): (328, 94, 2, 184), (328, 94, 274, 2),
(328, 276, 274, 2), (600, 94, 2, 182). Then `[0x008990e0] += 0x20` (`0x0042ebc7`). The only other writers of
`0x008990e0` are `FUN_0042e5b0`'s `0x0042e843` / `0x0042e84a` / `0x0042e865`.

**`FUN_00474890` UV law** (decomp on pool slot 1 + listing `0x00474890..0x00474d16`). k = p/2048 (`[0x005cead4]`),
cases from the jump table at `0x00474d18`:

| case | vt | vb | u0 | u1 |
|---|---|---|---|---|
| 1 | 0.125−k | 1−vt | vt | vb |
| 2 | 0.0625 | 0.9375 | 0.125−k | 1−k |
| 3 (j = 0.125−k) | 0.0625 | 0.9375 | 0.125−j | 1−j |
| 4 (j = 0.125−k) | 0.125−j | 1−j | 0.0625 | 0.9375 |
| 5 | 0.125−k | 1−k | 0.0625 | 0.9375 |
| default | k | 1−k | k | 1−k |

The quad is drawn as two halves of width w·0.5 (`[0x005cc32c]`): u0 .. u1·0.5, then u1·0.5 .. 2·(u1·0.5). With the
9th argument 0 there is no V0/V2 alpha fade (`0x00474b4a`).

**Law check against the original** (`uvcheck.py`): every captured preview half-quad is reproduced exactly in
float32 by one integer p.
- `text4/dl_orig18` A mode 0, p 39 (t 12). `dl_orig24` A mode 0, p 41 (t 14).
- `text3/dl_orig18`: A mode 0, p 238 and B mode 1, p 10. Both solve to the same t = 0xEC (e = 268 → 238,
  (0xEC−0xE0)·256/288 = 10), and t = 0xEC gives cB = 12<<4 = 0xC0, the captured B colour.
- The standalone's own captures fit the same law (s18/s24 mode 0 p 137; frames 900..903 mode 4 p 64).

### `FUN_004368e0` head (`0x004368e0..0x004369d8`)

- Alpha a = `[0x0067e7f4]`, pushed at `0x0043af87..0x0043af8d`.
- Its writer `0x0043c206..0x0043c23a` stores `ftol((1−[0x0067e840])·255)` while `[0x0067e7f0]` is 1,
  `ftol([0x0067e840]·255)` while it is 2, and `bl` otherwise. The settled capture's panel is `0xaa000000` =
  (2·0xff)/3, so a = 0xff there. The port uses a constant 0xff. The ramp is not modelled, as for the rest of this
  overlay.
- Panel box: `x 0x42000000 @0x00436974, y 0x43960000 @0x00436968, w 0x44100000 @0x0043695b, h 0x42c00000
  @0x0043694e`, colour bytes 0,0,0, alpha (2a)/3 (`0x00436926..0x00436944`).
- Divider: `x 0x439f8000 @0x004369c5, y 0x439a0000 @0x004369b9, w 0x40000000 @0x004369ac, h 0x42a00000 @0x0043699f`,
  colour ff,ff,ff,a.

### "vs" (`0x004381ee..0x004382ad`)

`FUN_0040bb90("vs" 0x005cda30)`, then `FUN_004739f0(tex, x, 316, 56, 56, colour, 0, 1.0, 0, 1.0, 1, 1)`:
- x `0x42900000` (`0x00438203`), `0x43080000` (`0x00438261`), `0x43480000` (`0x0043829a`)
- y `0x439e0000` (`0x0043820b`), w = h `0x42600000` (`0x004381ee`, `0x004381fb`)

`FUN_004739f0`'s args 7..12 are u0, u1, v0, v1, scale mode, filter. The filter arg is 0 → render state 9 = 1,
else 2 (decomp).

### Power-up frame (`FUN_0043af10` `0x0043b939..0x0043bbfa`)

Per column, under the icon's own gates (`id > −1` at `0x0043b9ee`, sprite ≠ 0 at `0x0043ba0c`). Standard sets the
zoom `[esp+0x1c]` to 0 (`0x0043b9fd`), so:
- `x = W·0.878125 [0x005cda70] − (W·col·0x26)/640 − 4.0 [0x005cc35c]`
- `y = H·0.783333 [0x005cda1c] − 4.0`
- `w = W·0.075 [0x005cd988]`, `h = H·0.1 [0x005cc56c]`

That is (558 − 38·col, 372, 48, 48). It is `FUN_0040bb90("Powerupshadow" 0x005cda04)` (INTERFACE.TXD, head
`0x0063b904`), drawn by `FUN_00473870(.., 0)` at `0x0043bae9`: last arg 0 = render state 9 = 1. The icon follows
at `0x0043bbe5` with the law the standalone already had (562 − 38·col, 376, 32, 32). The texture is 32×32 PAL4 with
a maximum alpha of 191. It loads into slot 97 / handle 88, which needed `QuadRenderer::kMaxSlots` 97 → 98.

### Vehicle sprite (`0x0043bc00..`)

`FUN_004739f0(FUN_0040bb70(name[0x005f6710 + FUN_0042a980([0x0067ea94])·4]), 0x43e40000, 0x43100000, 0x43180000,
0x43180000, colour, 0, 1.0, 0, 1.0, 1, 0)`, for table `0x5f7370` (s24, `0x0043bc77`) and `0x5f72a0` (s18,
`0x0043bcf3`). The last arg 0 = point filter, set on handles 52..59 at load. They are drawn only here.

### Live reads (`probe_chalrow.py`, read-only after the screen push, own PID each)

`[0x0067f17c]` = 0 on s24 (PID 23144) and on s18 (PID 27928). So `FUN_0042e590(0/1)` = "Training1" / "Training2",
which are the standalone's `kHandlePreview0 + 0/1`.

## Measurements

### Draw list, fresh originals `text4/dl_orig{18,24}.json`, frame 0 (all 4 frames in `groupsum.txt`)

Rows grouped by original caller (`groupsum.py`). "m" = moved, "x" = extra.

| screen, tol | build | matched | preview | panel | divider | vs | pu frame | vehicle | band | checker | video quad | extra |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| s18 anim4 | base | 84 | 6 missing | 1 | 0 | 3 m | 4 | 1 | 32 | 36 m | 0 | 33 |
| s18 anim4 | t4 | 131 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 36 m | 0 | 0 |
| s18 strict | base | 78 | 6 | 1 | 0 | 3 m | 4 | 1 | 32 | 42 m | 0 | 33 |
| s18 strict | t4 | 125 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 42 m | 0 | 0 |
| s18 0.01 | base | 76 | 6 | 1 | 1 | 3 | 4 | 1 | 32 | 42 | 1 | 80 |
| s18 0.01 | t4 | 124 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 42 | 1 | 43 (42 checker + video) |
| s24 anim4 | base | 127 | 6 | 1 | 0 | 3 m | 4 | 1 | 32 | 0 | 0 | 33 |
| s24 anim4 | t4 | **174** | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| s24 strict | base | 127 | 6 | 1 | 0 | 3 m | 4 | 1 | 32 | 0 (f1..f3: 10/19/27 m) | 0 | 33 |
| s24 strict | t4 | 174 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 (f1..f3: 8/16/27 m) | 0 | 0 |
| s24 0.01 | base | 83 | 6 | 1 | 1 | 3 | 4 | 1 | 32 | 42 | 1 | 80 |
| s24 0.01 | t4 | 131 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 42 | 1 | 43 |

- 0 `reordered` rows anywhere. The plate, arrow, label and value rows text2/text3 fixed are unchanged (still
  matched, 0 RED).
- Against text3's originals (`diff_t3orig_*`), t4 leaves 4 rows on s18: the backdrop's B layer (`0x42e80c`, 2) and
  the preview's B layer (`0x42eafa`, 2). That capture was taken at t = 0xEC (solved above), inside the B window.
  The standalone's frames 200..203 are at t 123, where both sides draw A only. This is crossfade phase from the
  wall clock: the same rows text3 counted as `other` on scr19a. s24 against text3's original is 0 RED.

### Pixel check (`maskdiff.py`, backbuffer dumps at frame 200)

Only pixels the new draws own opaquely are compared, so the moving video backdrop and the crossfade phase stay out.

| screen | region | px | base mean / >16 | t4 mean / >16 |
|---|---|---|---|---|
| 18 | preview borders | 1816 | 133.23 / 99.39 % | 1.04 / 0.00 % |
| 18 | vehicle, car1 alpha 255 | 17785 | 62.78 / 67.28 % | 3.89 / 0.33 % |
| 18 | power-up frames, alpha 191, minus icons | 684 | 110.22 / 100 % | 2.83 / 0.00 % |
| 24 | preview borders | 1816 | 133.14 / 99.39 % | 1.04 / 0.00 % |
| 24 | vehicle | 17785 | 62.73 / 67.26 % | 4.01 / 0.67 % |
| 24 | power-up frames | 684 | 123.63 / 100 % | 13.50 / 48.83 % |

- At alpha 191 a quarter of the backdrop shows through the power-up frame. The s24 dump's backdrop differs from
  the original's at that moment.
- Inside the preview rect, s18 shows the same "Training1" image on both sides (`pixel/s18_preview_pair.png`). The
  original's s24 dump comes from a different boot than its draw list and shows "Training2" (decoded with
  `txd_peek.py`, `pixel/s24_montage.png`), i.e. the B layer. That is phase, and the texture mapping is the bytes'.
- Unmasked region numbers are in `pixel/` (`regiondiff.py`).

### Collateral (`sweep/`)

- **Record multisets** base → t4 on 33 screens (`multiset_base_vs_t4.txt`, checker split out by symbol):
  - identical on 31 screens
  - s6 / s7 differ by 4 records each, the tex-86 size pulse text2/text3 listed
  - s18 / s24 per frame: +11 (6 preview, 1 panel box, 4 power-up frames) and 37 moved (32 band, divider, 3 vs,
    vehicle)
- **Text**: 338 text calls over 33 screens, `pre == post` on every one, matched/dx/dy/dh/dw included
  (`text_dy_check.txt`, `quad_diff_sweep.txt`, `sites_table.md`).
- **scr19a / 19b / 30 / 32**: `widget_red` 0 on all 12 diffs (`drawlist_summary.txt`). scr19a keeps its 2 `other`
  rows, the backdrop B layer `0x42e80c`, as in text3.

## Remaining, not in this change

1. Checker jitter (strict / 0.01 px) and the video quad's −0.5 px offset at 0.01 px. Both are on every screen and
   pre-date this work.
2. **Crossfade phase.** The standalone's clock starts at its first chrome frame, the original's at boot. So which
   layer is up at a given frame differs between runs. The law is verified on 4 captured original layers (8 half-quads). The B layer was not
   sampled live on the standalone (frames 900..903 landed back in the A window, mode 4).
3. **Not modelled:**
   - the `[0x0067e7f4]` transition ramp (`0x0043c206..0x0043c23a`)
   - the power-up gate `[0x007f0a50 + row·0x30] != 0` (`0x0043b95c..0x0043b96a`), which the existing icon code
     does not test either
   - Chaos power-ups
   - the vehicle index `FUN_0042a980`
4. The standalone loads 24 of the table's 26 preview names. Row 12 (City-at-Night) would draw no preview layer.
5. **Seen in passing, not changed.** The standalone's backdrop crossfade (`exe_main.cpp`, "ShellB runs the preview
   crossfade") picks `kHandlePreview0 + kCycle[slot]·2`. `FUN_0042e5b0` calls `FUN_0042e590(1)` for its second
   layer (`0x0042e7fc`), and in `FUN_0042e8b0` the slot only selects the `FUN_00474890` UV case.
   [UNCERTAIN] whether the backdrop's first layer also passes a constant n. Missing evidence: the listing of
   `0x0042e5b0..0x0042e7f0`. Worth a byte read before any backdrop work.

## Proposed CHANGELOG line

`2026-10-13 — Frontend s18/s24 preview frame, panel box, power-up frames from the bytes (work/text4): FUN_0042e8b0
ported (crossfade t = ftol([0x007f1010]*60)&0x1ff, slot counters [0x0067ea58]/[0x0067f0bc], FUN_00474890 UV law
cases 1..5/default, rect 330,96 270x180, 4 borders, [0x008990e0] += 0x20 at 0x0042ebc7), so the FUN_00473ee0 band
now sits at the original's x 928 (32 band rows 0 RED; the 6 "standalone-only" tiles were this). Panel box 32,300
576x96 (2a/3)<<24 (0x00436991), divider x 319.0 (0x004369c5), "vs" 72/136/200,316 56x56 (0x00438242..), power-up
frame "Powerupshadow" 558-38c,372 48x48 point filter (0x0043bae9), vehicle 456,144 152x152 point filter
(0x0043bcf3/0x0043bc77). s24 174/174 matched at anim4, s18 all but checker; 0.01 px leaves checker + the
all-screen video quad -0.5; UV law exact on 4 captured original layers; collateral 31/33 screens identical (s6/s7
pulse), text 338/338 unchanged, scr19/30/32 widget RED 0. Evidence verify/scr19_parity_20261012/text4/, note
re/analysis/negctl/session_20261012b/text4_RESULT.md.`
