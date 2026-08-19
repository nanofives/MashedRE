# Acceptance-grade composition check, screen 1 — 2026-08-19

Draw-list diff (the primary verifier per `re/analysis/parity_tooling.md`), original vs
standalone, same screen, same settled state.

## Verdict

```
VERDICT: RED (match=448 mismatch=20 missing=0 extra=0)
```

**`missing=0 extra=0` is the headline.** The standalone emits exactly the same draw set as
the original, in the same order, at the same coordinates, with the same textures and blend
modes. Composition is correct. Every divergence is colour-only.

## The defect: 20 quads drawn at full alpha instead of 0x30

All 20 mismatches share one signature — **identical RGB, wrong alpha**:

| shape | count | original | standalone |
|---|---:|---|---|
| solid | 12 | `col=30131550` | `col=ff131550` |
| gradient | 8 | `col=30131550/00131550/…` | `col=ff131550/00131550/…` |

RGB is `0x131550` on both sides. In the gradient quads the **transparent stop
(`00131550`) matches**; only the opaque end differs. So the standalone is drawing this
quad family at `0xff` (fully opaque) where the original draws at `0x30` (48/255, ~19%).

Emitters: `HudIm2DQuad+0x188` and `HudIm2DQuadCorners+0x1dc`. Geometry places them on the
menu list rows (x 58–270, y 258–282, w 2/100/210, h 2/26) — row separators and the
selection band. Visually the standalone's list rows read solid where the original's are
dimmed.

## Ruled out: this is not animation phase

`parity_tooling.md` warns that unsynced captures can never agree on animated values, so
the alpha was checked frame by frame before calling it a defect. Decoding the ARGB dword
at offset 16 of every 28-byte vertex in the original burst:

| frame | alpha histogram for RGB `0x131550` |
|---|---|
| `scr1_f0` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f1` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f2` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f3` | `{0: 16, 48: 16, 255: 48}` |

**Identical across all four frames.** No ramp. The original holds alpha `0x30` steadily, so
the difference is reproducible and belongs to the port, not to capture timing.

## Two harness parameters that had to be right first

The first attempt returned `RED (match=0 … missing=468 extra=468)` — total alignment
failure, which is a *harness* result, not a rendering one. Two causes, both mine:

1. **`--scale-b` defaults to 0.8**, mapping an 800×600 standalone into the original's
   640×480 space. This standalone runs at 640×480 (`exe_main.cpp:345`), so every B
   coordinate was shrunk by 0.8. Fixed with `--scale-b 1`.
2. **The standalone had not settled.** At `MASHED_DBG_DRAWSTREAM=200:203` it was still
   emitting `LogoGradientQuadPx` / `LogoOverlayDraw` — the logo overlay — while the
   original burst was a settled menu. Moved to `700:703`.

A `match=0` diff should always be read as "the streams did not align", never as "the
renderer is wrong".

## Reproduce

```
py -3.12 re/frida/menu_draw_burst.py --screen 1 --frames 4

MASHED_GOTO=1 MASHED_DBG_DRAWSTREAM=700:703 MASHED_DETERMINISTIC=1 \
  MASHED_DET_FRAMES=900 MASHED_WIN_POS=left-bl mashedmod/build/mashed_re.exe

py -3.12 re/tools/drawlist_diff.py log/menu_draw_burst.json log/drawstream_re.json \
  --exclude-tex 9 --map mashedmod/build/mashed_re.map \
  --rotate-a 0x42e65a --tol-anim 4 --scale-b 1
```

Note the documented baseline is *"settled scr1 is GREEN 118/118 per frame (2026-06-12)"*.
This run captured 117 draws per original frame and 468 aligned pairs across the burst, so
the counts are not directly comparable to that line; whether the baseline has drifted or
the burst framing differs is **[UNCERTAIN]** and not resolved here.


---

# Follow-up, same day: both open questions answered

## 118 vs 117 — RESOLVED, and it is not a regression

All four original frames contain **117 raw draws** (before any `--exclude-tex`), and the
diff aligned **468 pairs = 117 x 4** with `missing=0 extra=0`.

So **both sides independently produce 117 draws per frame and agree completely.** Had the
standalone regressed against a genuine 118-draw original, that would surface as `missing=1`
per frame. It does not. The `118/118` figure in `parity_tooling.md` (2026-06-12) does not
reproduce, but because the two sides agree exactly, this is a measurement or documentation
difference and **not** a rendering regression.

The doc line is deliberately **left as-is**: which count was right in June is not
established here, and overwriting it would swap one unverified number for another.

## The alpha defect localises to the last two rows

The 20 mismatching draws sit at exactly two y positions:

| x | y | w | role |
|---:|---:|---:|---|
| 60 | 258 | 210 | plate / border |
| 270 | 258 | 100 | right-fade gradient |
| 58 | 258 | 2 | left border |
| 60 | 282 | 210 | plate / border |
| 270 | 282 | 100 | right-fade gradient |

Screen 1's rows run y=168, 192, … 258, 282 — so **the original disables the last two items
and the standalone enables them.**

The colour constant in the port is already correct (`exe_main.cpp:3634-3637`):

```cpp
const std::uint32_t border =
    disabled    ? 0x30501513u
  : highlighted ? 0xff1050b4u : 0xff501513u;
```

with the comment *"disabled rows alpha 0x30"*. The bug is that `disabled` never becomes
true for those rows. It comes from `!Nav_ItemEnabled(rec.row_index)`
(`exe_main.cpp:3593-3594`), and `Nav_ItemEnabled` (`MenuNavSM.cpp:1346-1349`) reads
`g_stack[g_nav_depth].avail[row_index] == 1` — so `avail[]` is not carrying the original's
unavailability for those two rows.

**Ruled out: the savedata gate**, which was the obvious suspect. `MenuNavSM.cpp` documents
*"DAT_007f0f2c savedata gate -> +0x4ec (screen 1 item 3)"* and notes a blank save leaves no
savedata. But `original/gamesave.bin` carries magic `0xDEADBEEF` at +0, is exactly
`0x24FA0` bytes, and `Nav_GameStateLoadSave` is wired at `exe_main.cpp:6117`. Both sides
see savedata, so these rows are gated on something else.

**Not fixed here, deliberately.** Finding what the original gates rows 258/282 on is RE work
against the avail-population path, not a constant edit. Guessing a predicate to force the
gate GREEN would be exactly the unfounded change this gate exists to catch — and that GREEN
would be worth less than the RED it replaced. The defect is left localised to a single
predicate, with emitters, geometry and constants all confirmed correct.


---

# Root cause: `DAT_007f0f2c` is runtime state, and the port treats it as save state

## First, the row count was wrong — it is ONE row, not two

A single disabled row emits five quads, and the geometry accounts for both y positions:

| draw | x | y | w | h |
|---|---:|---:|---:|---:|
| top border | 60 | `py2` = 258 | 210 | 2 |
| top right-fade | 270 | 258 | 100 | 2 |
| left border | 58 | 258 | 2 | 26 |
| bottom border | 60 | `py2 + 26 - 2` = 282 | 210 | 2 |
| bottom right-fade | 270 | 282 | 100 | 2 |

That is exactly the 3-at-258 / 2-at-282 split observed, x4 frames = 20 mismatches. So the
original disables **one** row whose plate starts at y=258 — consistent with `avail[3]`.

## The port's switch arm is correct

`FUN_00432800` case 1, decompiled from the anchored binary:

```c
case 1:
    if (DAT_007f0f2c == 0) {
      *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;   // avail[3]
    }
    DAT_007f0fe8 = 0;
    ...
```

`&DAT_0067ed84` is `avail[0]`, so `DAT_0067ed90` is `avail[3]`. The port
(`MenuNavSM.cpp:486`) models exactly this: *"if DAT_007f0f2c==0 -> avail[3]=0"*. The switch
arm, the offset and the constant are all right.

## The defect is where the gate's VALUE comes from

`DAT_007f0f2c` reference-manager xrefs:

| kind | site | in |
|---|---|---|
| READ | `0x004328df` | `FUN_00432800` — the gate, its only reader |
| WRITE | `0x004305ba` | `FUN_00430290` |
| WRITE | `0x00492504` | `FUN_004924f0` — sets it to **0** |
| WRITE | `0x00492991` | `FUN_004927c0` — sets it to **1** |

`FUN_004924f0` carries the annotation *"Zero-fills 0xdce9 (56553 dec) consecutive DWORDs
starting at DAT_007f0f60"* and additionally assigns `DAT_007f0f2c = 0`. `FUN_004927c0`
assigns `DAT_007f0f2c = 1`.

So the flag is **runtime state driven by explicit setters**. The port derives it *only*
from the restored save span at `+0x4ec` (`MenuNavSM.cpp`, `Nav_GameStateLoadSave`), and
models none of the three writers. `original/gamesave.bin` is a real save (magic
`0xDEADBEEF`, `0x24FA0` bytes) whose byte at that offset is non-zero, so the standalone
enables the row, while the original's boot path leaves the flag at 0 and greys it.

The address does fall inside the span the save restore block-copies
(`0x007f0a40..0x007f0f60`, `REP MOVSD` at `0x00404e91`), so the save does write it — but in
the original the runtime setters also run, and the menu reads whatever they last left.
Modelling the restore without the setters is what diverges.

## What would close it

Model the writers rather than the save byte. `FUN_004924f0` and `FUN_004927c0` set the flag
to 0 and 1 respectively; `FUN_00430290`'s value was not read here.

**[UNCERTAIN]** which writer executes before the frontend on a fresh boot. That needs a
call-order trace (Frida on the three writers, or a caller walk), and it is the one fact
required before changing the port. It is deliberately not guessed: forcing `avail[3]=0`
would turn the gate GREEN while leaving the port wrong for any state where the row *should*
be enabled.


---

# Closed by Frida: the gate is 0 all through the menu, and the =1 write is progression-locked

Traced `DAT_007f0f2c` on stock `original/MASHED.exe` (`re/frida/trace_savedata_gate.py`,
hooks off): Interceptor on all three writers plus the reader `FUN_00432800`, logging the
flag's value at each event. 2211 events over a 16 s menu-idle boot.

## Result

- **The reader `FUN_00432800` fired 1056 times, always at `slot=0` (screen 1), and the gate
  was `0` at every single read.** Zero transitions across the whole run. So the original
  greys the row because the flag is genuinely 0 at the menu — the trace matches the RED
  exactly.
- `FUN_004924f0` (the zero-fill) ran twice, early, both before the first read.
- `FUN_004927c0` fired **1152 times and never set the flag to 1**.

## Why the =1 writer never fires — it is a progression gate, not a per-boot init

`FUN_004927c0`'s assignment is guarded:

```c
DAT_00771980 = DAT_00771980 + 1;
if (DAT_00771980 == 0xc) {
    for (i = 0x9c; i != 0; i--) *puVar4++ = 2;   // fills 0x007f0a40..
    DAT_007f0f2c = 1;
}
```

The write to 1 only happens when the counter `DAT_00771980` reaches **12**. At a fresh menu
that counter never gets there, which is why 1152 calls produced zero writes. `DAT_007f0f2c`
is therefore a **progression/unlock flag** — some cup or challenge milestone sets it — not
something a boot establishes.

## Conclusion, now with dynamic proof

The port's model is wrong in kind. `DAT_007f0f2c` is runtime progression state whose
default at a fresh menu is 0 (row greyed), and it flips to 1 only on a gameplay milestone.
The standalone derives it from the restored save byte at `+0x4ec`, which is non-zero in
`original/gamesave.bin`, so it enables a row the original greys.

**The fix is to seed the flag from progression, not from the raw save byte** — or, minimally
and faithfully, default it to 0 at the fresh menu and set it only when the same milestone
`DAT_00771980 == 0xc` is reached. Which is still a port change against the progression
system, not a one-line constant, so it stays out of this evidence-gathering pass. But the
open question from the previous section — *which writer runs before the frontend* — is now
answered: **none of them do; the flag is simply 0.**


---

# RESOLVED: GREEN. It was a save-state mismatch, and the port was correct all along

I was wrong three times about this defect. The record above preserves the wrong turns because
they are instructive; this section is the verified answer.

## The answer

The two builds were compared with **different save files**:

- The original burst (`menu_draw_burst`) ran `original/MASHED.exe`, which reads
  `original/gamesave.bin`. Its byte at file `0x24F2C` (the `DAT_007f0f2c` gate, span+0x4ec)
  is **0**, so item 3 is greyed.
- The standalone reads `original/gamesave.bin` at boot **and then** `Campaign_LoadProgress`
  (`Race/GameFlow.cpp:216`) reads its own progression save `mashed_re_gamesave.bin` and
  calls `Nav_GameStateLoadSave(img)` a second time (`GameFlow.cpp:233`). That file is a real
  progressed save (magic `0xDEADBEEF`, counter 9, 2/13 tracks) whose byte at `0x24F2C` is
  **1**, so `has_savedata` ends at 1 and item 3 is enabled.

Proven by instrumenting the render (`MASHED_DBG_AVAIL`): at screen 1 the standalone reported
`has_savedata=1`, row_index 3 `enabled=1`. The port's `case 1: if (has_savedata==0) av[3]=0`
never fired because the state was legitimately 1 for the save it had loaded.

## The proof: matched state -> GREEN

Parked `mashed_re_gamesave.bin` so `Campaign_LoadProgress` finds no progression save and the
standalone keeps the blank-save default (`has_savedata=0`), matching the original's blank
`original/gamesave.bin`. Re-captured and re-ran the same diff:

```
VERDICT: GREEN (match=468 mismatch=0 missing=0 extra=0)
```

**All four frames, 117/117 draws, zero mismatches.** With the save state matched, the
standalone's draw list is byte-identical to the original's. The 20 alpha mismatches were
entirely a save-state difference. The grey-out port is correct.

## What every earlier root cause got wrong

1. "The port derives the gate from a non-zero save byte." No: `original/gamesave.bin`'s byte
   is 0, and that is the save the original reads too.
2. "It is runtime progression state the port ignores." True of the original's flag, but not
   the divergence: the port does model it, from the save.
3. "The bug is downstream in avail-population." No: avail is populated correctly; the input
   state simply differed.

The single fact that would have refuted all three in 30 seconds was reading both save files'
byte at `0x24F2C` — 0 in the original's, 1 in the standalone's. I did not do that until the
fourth pass. The lesson is the same one that paid off elsewhere this session, applied too
late here: check the artifact before writing the conclusion.

## Is there still a real defect?

Maybe a smaller one, filed rather than chased: the original sets `DAT_007f0f2c` to 1 only at
a specific milestone (`DAT_00771980 == 0xc`, counter 12), while the standalone's save has the
byte set at counter 9. Whether the standalone's `SaveProgress` sets that byte on the same
condition the original would is **[UNCERTAIN]** and untested here. But it is a save-format
fidelity question, not a rendering defect, and the parity gate is GREEN under matched state.

## Harness note

`build.bat` reused a stale `exe_main.obj` across two source edits — the instrumentation only
took effect after `rm mashedmod/build/exe_main.obj`. If a source change seems to have no
runtime effect, delete the object and rebuild before concluding anything about behaviour. The
exe also chdir's into its output dir at boot, so debug files must use the same relative
convention as `kLogPath` (`"foo.txt"`, not `"log/foo.txt"`).
