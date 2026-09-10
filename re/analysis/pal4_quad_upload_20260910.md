# QuadRenderer rejected PAL4, so every BADGES 16x16 sprite silently never drew (2026-09-10)

Found while rescuing the uncommitted Challenge-Select `dot` glyph work off the stale
agent worktree `agent-afe83e3c8e0f2baf6` (base was 37 commits behind HEAD; re-applied
onto HEAD rather than merged, per memory `agent-worktree-forks-stale`).

## The defect

`QuadRenderer::UploadIntoTextureSlot` (`mashedmod/src/mashed_re/D3d9Render/QuadRenderer.cpp:133`)
accepted only `ARGB8888` and `Paletted8` and returned `false` for anything else. The
reason went into `m_last_error` and nowhere else. Meanwhile `TrackRenderer.cpp:382`
(`MakeTexture`) has handled `PAL4` since 2026-07-31, and `Txd::PixelFormat::Paletted4`
has existed in the decoder the whole time (`Txd/TxdDecoder.h:79`).

Measured consequence, from the pre-fix run (`log/build_chalsel_dot_20260910.txt` build,
capture `verify/chalsel_dot_A/`):

```
R2-5: badges.txd 'Button' 16x32 upload OK (dict has 23 textures)
R2-5: badges.txd 'Arrow'  16x16 upload OK
F38:  badges.txd 'lock'   16x16 upload FAILED
F38:  badges.txd 'check'  16x16 upload FAILED
F38:  badges.txd 'dot'    16x16 upload FAILED
```

The same dictionary, the same load path, the names matched — the PAL8 siblings uploaded
and the PAL4 ones did not. So the **entire Finding-38 Challenge-Select status-glyph
family has been dead since it landed**, and so has the detail-panel mode checklist that
draws out of the same three handles. Nothing in the log said why: the call site printed a
bare `FAILED`.

## The fix

1. `UploadIntoTextureSlot` accepts `Paletted4`. PAL4 and PAL8 are **both one byte per
   pixel** — `depth` selects the palette SIZE (16 vs 256), not the storage width
   (`TxdDecoder.h:66-70`; census 2026-07-31 over 5194 mips: `stride == max(4, width*1)`
   for both, no depth-4 byte above `0x0F`). So the expansion loop is shared and only the
   index is masked with `0x0F`. Reading PAL4 as packed nibbles is the bug that corrupted
   every PAL4 track texture (`LibRw/RwRasterBridge.cpp:12`) — not repeated here.
2. The `F38:` log line now prints the source format and `QuadRenderer::last_error()`, so
   this class of silent rejection cannot recur unexplained.
3. Carried over from the rescued worktree: `kMaxSlots` 96 -> 97 and the `dot` arm
   (BADGES gate `0x0042ee00` slot 1, cup-table column 3 == 1) for non-selected rows, per
   the measured model in `re/analysis/chalsel_icon_dictionary_20260905.md`.

Post-fix (`verify/chalsel_dot_B/`):

```
F38: badges.txd 'lock'  16x16 fmt=PAL4 upload OK (ok)
F38: badges.txd 'check' 16x16 fmt=PAL4 upload OK (ok)
F38: badges.txd 'dot'   16x16 fmt=PAL4 upload OK (ok)
```

## Verification (`MASHED_PARITY=1` frontend walk, 17 screens, three runs)

- **Control — scope.** A (pre-fix) vs B (post-fix): **15 of 17 screens byte-identical**
  (mean abs diff 0.00). Only `s6` (Challenge Select, 0.25) and `s7` (0.55) move. So the
  change touches exactly the screens that use the BADGES glyph family and nothing else.
- **Control — animation noise.** B vs a second run of the SAME build (B2) still shows a
  ~1600-pixel cluster at `x 210..259, y 125..178` on s6. That cluster is therefore the
  selected row's size-pulsing category sprite landing on a different phase between two
  processes, **not** an effect of the change. Any A-vs-B reading of that band alone would
  have been a false positive.
- **Signal.** Pixels that differ A-vs-B *and* agree B-vs-B2: **492 on s6, 681 on s7**.
  On s6, 472 of the 492 sit at `x 524..541, y 326..370` where A was pure `(0,0,0)` and B
  is `(255,255,255)` — the detail-panel mode checklist, whose measured geometry is
  `x=520, y=320 step 16, 24x24, 0xffffffff`
  (`chalsel_icon_dictionary_20260905.md`, third addendum table). On s7 the row-glyph
  column at `x 212..254` also carries 201 signal pixels.

So the fix is confirmed as the cause, correctly scoped, and lands the restored draws at
the measured columns.

## [UNCERTAIN U-PAL4-S6] The restored draws cannot be gated against `verify/orig_screens/s6.bmp`

Region diff over the checklist box (`x 518..545, y 318..375`) against that reference gets
**worse**, 48.38 -> 68.25, and the region histogram says why: the original reference is a
uniform dark grey there (`(48,56,56)` x843, `(48,52,48)` x536, …) with **no glyph and no
mode text at all** in the panel's right half, while the port draws the four-line
`Multi Player / Power Ups / Hold the Flag / The Fugitive` list plus a check and two
padlocks at `x~530`.

**That comparison is not admissible as a verdict, because the reference is not
state-matched.** `verify/orig_screens/s6.bmp` (committed `bee91ac3`, 2026-08-19) shows
**four unlocked challenge rows** — Angel Peak, Kharga Temple, Neustein, Timgidski, no
padlocks — whereas this port run shows **one** row. Different save state, so the panel
contents are not comparable, and the third addendum's geometry was taken from a live
`FUN_00473870` draw hook rather than from this still.

What is missing, precisely: an original-side screen-6 capture taken on the **same** save
state the standalone runs on, with the panel's right half in frame. Until that exists,
neither "the port's checklist is invented" nor "the port's checklist is faithful" is
supported. Two separate readings are both consistent with the evidence in hand:
the original's mode list may be state-gated, or the port's may be mispositioned.

Not blocking this fix: the checklist draw and its geometry were already committed code —
the PAL4 rejection only prevented it from ever executing. Making a measured, committed
draw actually run is the correct state; whether that draw is right is the open question.

**Separately, and larger than the glyphs:** the port renders **1** challenge row against
the reference's **4**. That is the dominant s6 divergence and is untouched here.

## Artifacts

- `verify/chalsel_dot_A/parity/` — 17 screens, pre-fix
- `verify/chalsel_dot_B/parity/` — 17 screens, post-fix
- `verify/chalsel_dot_B2/parity/` — 17 screens, post-fix repeat (animation control)
- `log/build_chalsel_dot_20260910.txt`, `log/build_pal4_20260910.txt`
