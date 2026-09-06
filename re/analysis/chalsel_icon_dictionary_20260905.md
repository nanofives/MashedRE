# The Challenge-Select mode-checklist icons: two dictionaries, not one (2026-09-05)

Filed separately from `re/analysis/race_hud_capture_20260902.md` only because that
file carried another session's uncommitted Finding 37 at the time. It is the direct
successor to that file's **Finding 36**, and it **overturns Finding 36's icon
paragraph in both directions**. Read this note after Finding 36, not instead of it.

## What Finding 36 claimed, and why it was wrong

Finding 36 identified the Challenge-Select detail panel as a per-track mode
checklist drawn by `FUN_00439210`, and left this `[UNCERTAIN]`:

> `FUN_0040bb50(name)` is a plain dictionary lookup: `FUN_004c5c00(DAT_0063b8fc,
> name)`. `INTERFACE.TXD` (source of the Star on the same screen) contains **Lock,
> Star, Tick**; `Frontend.piz/TEXTURES.TXD` contains none of the three. **There is
> no texture named `"check"` in either.** `[UNCERTAIN]` whether the lookup is case-
> or prefix-tolerant enough to reach "Tick", and **which dictionary `DAT_0063b8fc`
> actually holds** — so "an unlocked row draws no icon at all" is supported but not
> proven. Port draws Lock for locked, nothing for unlocked.

Every named fact there is true, and the conclusion is still wrong, because
`INTERFACE.TXD` is not the dictionary `FUN_0040bb50` searches. This is the third
time this lane has paid for the same mistake: **a wrong reference table gives
confident, self-consistent, wrong readings** (`[[two-copies-of-english-dat]]`,
U-9083). As before, what settled it was reading the loader, not comparing more
strings.

## There are TWO named-sprite dictionaries, with two forwarders and two gates

> **SUPERSEDED by this note's own Addendum below: there are FOUR** (`FX.TXD`,
> `BADGES.TXD`, `TrackImages.txd`, `Interface.txd`), read straight out of the
> loader `FUN_0040bbb0`, plus a fifth head at `0x0068b9ac` outside the array. The
> two below are the two this slice needed, and everything said about them holds.
> Do not quote the count from this section.

Both forwarders end in the same search routine, `FUN_004c5c00`, but over different
list heads:

| forwarder | list head | contents | textures |
|---|---|---|---|
| `0x0040bb50` | `DAT_0063b8fc` | `sfx.piz :: BADGES.TXD` | 23 |
| `0x0040bb90` | `DAT_0063b904` | `sfx.piz :: INTERFACE.TXD` | 30 |

The badges head was already established port-side by the R2-5 work
(`FUN_0040bbb0` loads it via `FUN_0042a6b0("badges.txd",0,0)`; `exe_main.cpp`
comments at the `kSlotMenuBadge` definition and in `LoadMenuBadgeSprite`) — it was
simply never carried across to the icon question.

Each forwarder has its own slot-to-name gate, and the two name sets are disjoint:

```
0x0042ee00  ->  0x0040bb50 (BADGES)      0x004391b0  ->  0x0040bb90 (INTERFACE)
  slot 0  "lock"   @0x005cd7b8             slot 0    "Lock"  @0x005cda44
  slot 1  "dot"    @0x005cd7b4             slot 1/3  "Star"  @0x005cd970
  slot 2  "check"  @0x005cd7ac             slot 2    "tick"  @0x005cda3c
  else    0                                else      0
```

`0x0042ee00` is already transcribed byte-for-byte in
`mashedmod/src/mashed_re/Frontend/SpriteGate.cpp`. `0x004391b0` is its sibling,
read here off `MASHED.exe.unpatched`:

```
0x004391b0  test eax,eax
0x004391b2  jne  0x4391c2
0x004391b4  push 0x5cda44            ; "Lock"
0x004391b9  call 0x40bb90
0x004391c2  cmp  eax,1
0x004391c5  je   0x4391f7
0x004391c7  cmp  eax,2
0x004391ca  jne  0x4391f2
0x004391cc  call 0x430760            ; slot 2 is gated
0x004391d5  mov  eax,[0x67e9fc]      ; ...and excluded on screen ids 2 and 0xa
0x004391e4  push 0x5cda3c            ; "tick"
0x004391e9  call 0x40bb90
0x004391f2  cmp  eax,3
0x004391f7  push 0x5cd970            ; "Star"
0x004391fc  call 0x40bb90
0x00439205  xor  eax,eax             ; default: no sprite
```

`0x005cda3c` reads `"tick"` and `0x005cda44` reads `"Lock"` — adjacent in
`.rdata`, four bytes apart.

## The checklist rows use the BADGES forwarder

Read off `MASHED.exe.unpatched` with capstone, first of three identical row blocks
inside `FUN_00439210`:

```
0x004395c6  mov  eax,[edx + 0x7f0a50]   ; the per-mode unlock flag
0x004395cc  test eax,eax
0x004395d1  je   0x4395e0
0x004395d9  push 0x5cd7ac               ; flag != 0  -> "check"
0x004395de  jmp  0x4395eb
0x004395e6  push 0x5cd7b8               ; flag == 0  -> "lock"
0x004395eb  call 0x40bb50               ; BADGES, not INTERFACE
0x004395f0  add  esp,4
0x004395f3  push eax                    ; the resolved texture...
0x004395f4  call 0x473870               ; ...into TextSpriteUVExplicit (C3)
```

repeated at `0x0043966e`/`0x0043967b` (flag `0x007f0a58`) and `0x00439703`/
`0x00439715` (flag `0x007f0a5c`). Argument order into `0x00473870` is the
port's existing 7-param signature — texture last-pushed, then `ebx` = x, the
computed float = y, `ebp` = w, `edi` = h, `esi` = argb, and a literal `1` blend
flag pushed first at `0x004395c1`.

So **the original draws an icon on every row**: `check` when the mode is unlocked,
`lock` when it is not. There is no "draws nothing" arm.

## BADGES.TXD holds exactly the gate's name set

Offline dump of `sfx.piz :: BADGES.TXD` (`re/tools/piz_extract.py` +
`re/tools/txd_format_census.py`'s chunk-0x23 walk), 23 textures:

```
Air, Sv, S2, S1, F3, F2, F1, Stopwatch, boom, cup, flag,
lock(16x16 PAL4), dot(16x16 PAL4), check(16x16 PAL4),
Star(16x16 PAL8), SemiC2, SemiC, Arrow, Box2, Box, Button2, Button, Tritex
```

`lock`, `dot` and `check` are adjacent (indices 11-13), same size, same format —
the whole of `0x0042ee00`'s name set, including `dot`, which no previous analysis
had thought to look for. `INTERFACE.TXD`'s 30 textures include `Lock`, `Star` and
`Tick` at 32x32, and **no `check` and no `dot`**.

## The live read, and why it was load-bearing

The offline dump alone does not settle it, for a reason worth recording:
`FUN_004c5c00` folds case (`0x004c5c3d`/`0x004c5c4a` `add cl,0xe0`), so `"lock"`
would ALSO have matched INTERFACE's `"Lock"`. Under Finding 36's assumption the
locked row would have resolved and the unlocked row returned NULL — which is
exactly the behaviour Finding 36 inferred. Only the runtime value of
`DAT_0063b8fc` distinguishes the two readings.

`re/frida/chal_icon_probe.py`, original at nav screen 6, stock (no ASI hooks),
output in `log/chal_icon_probe.json`:

```
badges_0063b8fc  count=23
  Tritex, Button, Button2, Box, Box2, Arrow, SemiC, SemiC2, Star, check, dot,
  lock, flag, cup, boom, Stopwatch, F1, F2, F3, S1, S2, Sv, Air
iface_0063b904   count=30
  BronzeCup, SilverCup, GoldCup, TimeTrial, QuickRace, MultiPlayer, Freestyle1,
  Freestyle2, Tick, Star, Lock, NFLRed, ... , CupBump, Silver, ENV

direct lookup matrix (0x0 = miss)
  check    bb50/badges=0x4635300   bb90/iface=0x0
  lock     bb50/badges=0x4635240   bb90/iface=0x4636c80
  dot      bb50/badges=0x46352a0   bb90/iface=0x0
  Lock     bb50/badges=0x4635240   bb90/iface=0x4636c80
  Star     bb50/badges=0x4635360   bb90/iface=0x4636ce0
  Tick     bb50/badges=0x0         bb90/iface=0x4636d40
  tick     bb50/badges=0x0         bb90/iface=0x4636d40
  Button   bb50/badges=0x4635600   bb90/iface=0x0
```

The live walk reproduces the offline dump name-for-name and count-for-count in
both dictionaries, so `DAT_0063b8fc` is still the badges dictionary at screen 6 —
it is not re-pointed per screen. `check` resolves through `bb50` and misses
through `bb90`; `Tick` does the reverse.

Calling the lookups directly is safe and was checked before the call, not after:
`FUN_004c5c00` is a pure list walk with no stores anywhere in its 114 bytes
(`0x004c5c00..0x004c5c72`).

**Honest limit on this capture:** the probe observed **no natural
`FUN_0040bb50` calls** — every logged call is one of the eight the probe itself
made. A synthetic `FUN_0043d2a0` nav push does not satisfy the panel's own guard
(`FUN_00430760() == 0 && DAT_0067e9fc == 6`), so `FUN_00439210`'s checklist block
never ran during the window. That is a gap in the natural-call trace, not in the
evidence: the direct matrix queries the game's own live dictionary pointer, which
is the quantity in question. An absent log is not a result
(`[[absent-log-proves-nothing-run-a-control]]`), so it is stated rather than read
as agreement.

## "Tick" was never a candidate

Finding 36 floated `Tick` as a possible intended target for `"check"`. It is not,
on three independent grounds: it is in the wrong dictionary; it is reached by the
other gate under the lowercase spelling `"tick"` at `0x005cda3c`, so it has its
own live user; and `FUN_004c5c00` is **not prefix-tolerant**. At `0x004c5c5a` the
loop, on reaching a NUL in either string, compares the two current characters and
requires them equal — so both strings must terminate together. `"check"` cannot
reach `"Tick"` by any tolerance the routine has.

## Two defects fixed in the port

Both in `mashedmod/src/mashed_re/exe_main.cpp`:

1. **Unlocked rows drew no icon.** They now draw the badges `check`.
2. **Locked rows drew the wrong texture** — INTERFACE.TXD's 32x32 PAL8 `Lock`,
   uploaded in `LoadCarColorSprites`. The original's is BADGES.TXD's 16x16 PAL4
   `lock`. Both icons now load in `LoadMenuBadgeSprite` off the badges dictionary,
   which is the only one the original can reach from this call site.

A third, incidental defect found while allocating slots for the check texture:
`kSlotLock = 61` collided with `kSlotVehPrev0` (61..68) and `kHandleLock = 52`
collided with `kHandleVehPrev0` (52..59) — the same handle-collision class the file
already documents for handle 10 (the 2026-06-12 "Item 13" fix). Moved to slots
93/94 and handles 84/85, which are free below `QuadRenderer::kMaxSlots = 96`.

## Not claimed here

- **The screen-6 Star is NOT affected.** There is a badges `Star` and an interface
  `Star`, and `0x00435f82`/`0x00435fdd` do push `"Star"` into `bb50`. But those two
  sites are inside `FUN_00434720` — the championship/cup screen (screen 5), per
  Finding 37 — not `FUN_00439210`. The screen-6 row star reaches its texture
  through the `bb90` gate, so the port's INTERFACE `Star` stands. The two-star
  split is recorded as an observation; no change was made on it.
- The row-to-area mapping (which track a Challenge-Select row actually launches)
  is still unverified, unchanged from Finding 36.

## A C3 body that does not match its own RVA

`LinkedListStringSearch` (`0x004c5c00`) in `Frontend/SpriteCluster.cpp` is
transcribed wrongly in two places, found because walking the list its way returned
garbage names while the node COUNT came out exactly right:

| | binary | the port's body |
|---|---|---|
| sentinel | the ADDRESS `head + 8` (`0x004c5c05 add eax,8`; loop test `0x004c5c68 cmp ebx,eax`) | `*(head + 8)`, then dereferenced again |
| name | INLINE char array at `node + 8` (`0x004c5c1c lea ecx,[eax+0x10]` off base `node-8`, then `0x004c5c27 mov cl,[esi]`) | `*(const char**)(node + 8)`, chased as a pointer |

The return value (`node - 8`, set at `0x004c5c19`) and the case folding are right.
The body is `RH_ScopedInstall`-commented (MASS-DISABLED 2026-05-24), so it has
never been installed and nothing has mis-executed — the same shape as the
`0x0042f8d0` story in Finding 37: a defective body parked behind a disabled
install, its level never re-tested. **Not corrected in this note** (it is outside
this slice and its `hooks.csv` row is in another session's dirty tree); filed as
`U-9085` so the demotion is decided deliberately rather than in passing.

---

# Addendum: the full forwarder audit (2026-09-05)

Follow-up sweep over **every** call site of the sprite-dictionary forwarders, to
check whether the mis-sourced icon above was an isolated slip or a pattern.
Scanned `MASHED.exe.unpatched` for direct `call`s to each forwarder and walked a
backward instruction window at each site to recover the pushed name literal.
**107 direct call sites** (FX 52, BADGES 34, INTERFACE 19, TrackImages 2).
Reproduce with `py -3.12 re/tools/sprite_forwarder_map.py`.

## There are FOUR dictionaries, not two — read off the loader

`FUN_0040bbb0` opens `d:\toastart\common\sfx.piz` (`0x005ccce4`) and fills four
contiguous heads by calling `FUN_0042a6b0(<name>, 0, 0)` four times:

| head | forwarder | TXD | `0x0040bbb0` store |
|---|---|---|---|
| `0x0063b8f8` | `0x0040bb30` | **FX.TXD** (`0x005cccdc`) | `0x0040bbd2` |
| `0x0063b8fc` | `0x0040bb50` | **BADGES.TXD** (`0x005cccd0`) | `0x0040bbf3` |
| `0x0063b900` | `0x0040bb70` | **TrackImages.txd** (`0x005cccc0`) | `0x0040bc06` |
| `0x0063b904` | `0x0040bb90` | **Interface.txd** (`0x005cccb0`) | `0x0040bc17` |

All four forwarders are byte-identical apart from the head they load
(`0x0040bb30`/`50`/`70`/`90`, 20 bytes each, `.text` padded with `nop`). This is
the authoritative map and it retires the guesswork for good — the main note above
had inferred only two of the four, and the FX one not at all. A **fifth** head
exists outside this array at `0x0068b9ac`, used by `FUN_00458630` (powerup
type-to-name lookup) directly through `FUN_004c5c00`.

## Which dictionary each name comes from

- **FX.TXD** (`bb30`, 52 sites) — the world/effects set: `smoke`, `scorch`,
  `wfall`, `shockwave`, `crosshair`/`crosshair2`, `beam`, `car_shadow`,
  `headlight`/`Headlight`, `lensflare-fs8`, `flash`, `oil`/`oilShine`, `puglow`,
  `tyre`, `flames3`, `animFire`, `exp_cloud`/`exp_cloud2`/`exp_flash`,
  `fireball`, `flatshad`, `shine`, `RWObjShad`, `VehicleIcons`. Four sites take
  the key in a register and are not resolvable statically (`0x00448b1a`,
  `0x0044cc23`, `0x004512fa`, and one more).
- **BADGES.TXD** (`bb50`, 38 sites) — `Arrow` (15 sites), `check` (7), `lock` (3),
  `Star` (2), `Button` (2), `SemiC`/`SemiC2`, `tritex`.
- **Interface.txd** (`bb90`, 19 sites) — `vs` (10), `question` (2),
  `Powerupshadow` (3), plus `Lock`/`Star`/`tick` via the gate.
- **TrackImages.txd** (`bb70`) — **zero** direct call sites; the previews are
  resolved by another path.

## Three arg-rewriting gates, not two

Alongside `0x0042ee00` (bb50) and `0x004391b0` (bb90) there is a third:
**`FUN_0042fab0`**, a 10-case jump table (`0x0042fabd jmp [eax*4 + 0x42fb48]`)
that rewrites its stack argument to an `NFL*` name and tail-jumps to
`0x0040bb90` — so the car-colour badges are INTERFACE, as the port has them.

## Result: no further texture-source defects

Cross-checking every texture the port uploads against the dictionary the original
resolves it from, **all of them now agree**:

| port slot | name | port source | original path | verdict |
|---|---|---|---|---|
| `kSlotMenuBadge` | `Button` | BADGES | `bb50` @`0x0043cbbe` | correct |
| `kSlotMenuArrow` | `Arrow` | BADGES | `bb50` x15 | correct |
| `kSlotCar0..9` | `NFL*` | INTERFACE | `FUN_0042fab0` -> `bb90` | correct |
| `kSlotVs` | `vs` | INTERFACE | `bb90` x10 | correct |
| `kSlotStar` | `Star` | INTERFACE | `bb90` gate slot 1/3, and see below | correct |
| `kSlotLock`/`kSlotCheck` | `lock`/`check` | BADGES | `bb50` @`0x004395eb` etc. | correct (fixed above) |
| track previews | 24 names | TRACKIMAGES | head `0x0063b900` | correct |

The mis-sourced checklist icon was an isolated slip, not a pattern. Recording the
negative result so nobody re-runs this sweep.

**The screen-6 Star question from the main note is now settled, not merely left
alone.** All seven gate calls — `0x00439b7c`, `0x0043a18b`, `0x0043a1c3`,
`0x0043a350`, `0x0043a38f`, `0x0043a523`, `0x0043a562` — are inside
`FUN_00439210`, the screen 6/7/8 renderer, and the screen-6 row star comes
through the `bb90` gate. The two badges-`Star` sites (`0x00435f87`,
`0x00435fe2`) are inside `FUN_00434720` (screen 5). So the two screens genuinely
use different Star textures (16x16 badges vs 32x32 interface), and the port's
INTERFACE Star for screen 6 is right.

## What the sweep DID surface: the row-state icon model (port defect, not fixed here)

`FUN_00439210` draws a per-row icon whose gate slot is the cup-table state
`*(u32*)(0x007f0a40 + idx*4)` — a different quantity from the `0x007f0a50/58/5c`
per-mode flags the detail panel uses. Both gates map that state to a sprite:

| state | `0x004391b0` (INTERFACE 32x32) | `0x0042ee00` (BADGES 16x16) |
|---|---|---|
| 0 | `Lock` | `lock` |
| 1 | `Star` | `dot` |
| 2 | `tick` (gated on `FUN_00430760()` and screen id not in {2, 0xa}) | `check` |
| 3 | `Star` | — (returns 0) |
| other | 0 | 0 |

and one arm additionally skips the draw outright when the state is 1
(`0x0043a174 cmp dword [edx*4 + 0x7f0a40],1 / 0x0043a17c je`), unless
`FUN_0042ef40(..., idx + 0x3e8)` returned non-zero at `0x0043a162`.

**The port draws an unconditional pulsing Star on every row**
(`exe_main.cpp`, `if (g_star_ready) HudIm2DQuad(kHandleStar, ...)` in the
`cup.trackCount` loop) and models none of this: a locked row should show a
padlock, a state-2 row a tick, and a state-1 row nothing at all.

**Deliberately not fixed in this pass.** Which of the two arms runs for which row
depends on branch structure around `0x0043a162`/`0x0043a1ae` that this sweep did
not finish tracing, and `0x0043a1ae` is a jump target reached from elsewhere in
the row loop. Guessing the arm selection would be exactly the mistake this note
exists to record. It needs its own slice: finish the arm trace, then drive the
states with a save that has mixed values (`MASHED_SAVE=<scratch>`) and capture,
rather than inferring from a fresh save where every row reads the same.

---

# Second Addendum: the row state-icon model, traced and measured (2026-09-05)

Follow-up to the "row-state icon model" the forwarder audit flagged. The arm
selection is now **traced in Ghidra and confirmed live**; the icon-selection
model is fully measured; the faithful geometry is not, and is handed off.

## Method

- Headless decomp of `FUN_00439210` and its helpers (read-only, pool slot
  `Mashed_pool15`, `DecompPC.java`, `-noanalysis -readOnly`; slot lock taken and
  released per the parent/child pool-collision rule).
- Live trace + control, original, `re/frida/chal_icon_probe.py --mode 3`. Two new
  probe exports: `armrows` hooks **both** slot gates and logs the slot each
  receives (`0x0042ee00` takes it on the stack, `0x004391b0` in EAX — the reason
  Ghidra prints `FUN_004391b0()` with no arg); `poke` writes distinct values into
  the cup table to break the fresh-save degeneracy. The screen id is driven
  through its producer: `FUN_0042f6b0` maps `DAT_0067f184` → `DAT_0067e9fc` via a
  jump table at `0x0042f724` (`f184=3` → `e9fc=6`), so the probe seeds `f184` and
  calls the mapper rather than poking the derived id — the same
  seed-the-producer discipline as `[[zeroed-granule-vs-minus-one-sentinel]]`.

## Arm selection — CONFIRMED

Per cup row, `FUN_00439210` draws a state icon:

- **selected row** (`iStack_78 == DAT_0067f17c`) → INTERFACE gate `0x004391b0`
  (32x32): `0→Lock 1→Star 2→tick 3→Star`. The `tick` arm (slot 2) is itself
  gated on `FUN_00430760()` and excludes screen ids 2 and 0xa (fine for 6).
  The selected row ALSO draws a category sprite (`MultiPlayer`/`QuickRace`/…)
  through a *third* gate `FUN_0042ee40` at an animated x.
- **other rows** → BADGES gate `0x0042ee00` (16x16): `0→lock 1→dot 2→check
  3→(none)`.

The slot value each gate receives is **cup-table column 3**:
`*(u32*)(0x007f0a40 + row*0x30 + 0xc)`. This is a *different* quantity from the
`0x007f0a50/58/5c` per-mode flags the detail-panel checklist uses (fixed in the
main note) — `FUN_00439210` reads both, for two different UI elements.

Live evidence (fresh save, rows 0-3, `sel=0`), 180 frames:

```
iface32  slot=2   180x        (row 0, the selected row)
badges16 slot=2   540x        (rows 1,2,3, each col3=2)   [pre-poke]
```

Poke `col3[0..3] = {2,1,0,3}` and re-measure:

```
iface32  slot=2   180x        row 0 (selected) -> its own col3 = 2
badges16 slot=1   180x        row 1 -> its own col3 = 1
badges16 slot=0   180x        row 2 -> its own col3 = 0
(row 3 col3=3 -> BADGES gate returns null -> NO draw, hence absent)
```

This is the discriminator that settles it: the slot **tracks each row's own
column-3 value**, not a fixed one, and a value of 3 through the badges gate draws
nothing. So it is genuinely per-row, and the "one icon per row" reading is right;
the earlier "unconditional" phrasing was wrong.

## On a fresh save the original shows tick/check, not Star

Every cup row has `col3 = 2` on a fresh save, so the selected row draws INTERFACE
`tick` and the others draw BADGES `check`. **No row shows a `Star`** — `Star` is
only reached through the INTERFACE gate at slot 1 or 3, which needs a non-2
column-3 value on the selected row.

The port draws a pulsing INTERFACE `Star` on every row. That is wrong on texture
(should be state-dependent tick/check/lock/dot) and on count (a badges slot-3 row
draws nothing).

## Why this is NOT fixed in code yet

The icon **selection** is fully measured, but a faithful **draw** needs two
things this trace did not close, and shipping without them would trade a measured
element for a guessed one:

1. **Geometry.** The icon x is clear (`fVar11 = width * 0xdc/0x280` = width ×
   0.34375), but its y and the two sizes (32x32 selected via `fVar9/fVar10`,
   16x16 others via `fVar7/fVar8`) are built from `_DAT_005cd0f8` (row pitch),
   `_DAT_005cc560`, `_DAT_005cc32c` and several x87 intermediates
   (`extraout_ST*`) the decompiler leaves unresolved. Extracting them faithfully
   is its own capstone pass.
2. **Reconciliation with the port's existing star.** This port's per-row star was
   *measured* against `orig_s6.bmp` star centroids in earlier (Finding-era) work
   and its pitch/count were fitted to the original. So the star is not obviously
   invented the way the A/B letter was — it may correspond to one of these draws
   at a resolved position. Ripping it out for a half-measured icon model would
   regress a measured element. This has to be settled by capturing the original's
   screen-6 icons at known non-uniform states (`MASHED_SAVE=<scratch>` with mixed
   col-3 values) and matching texture + position, not by inference.

Handed off as the top follow-up in `re/NEXT_SESSION.md`. The `[SCAFFOLD]` comment
at the star draw in `exe_main.cpp` now carries the measured model inline so the
next pass starts from evidence, not from the old guess.

---

# Third Addendum: the geometry, measured (2026-09-05)

Extended `chal_icon_probe.py` to hook the sprite draw `FUN_00473870(tex,x,y,w,h,
argb,blend)` and read the actual columns at screen 6 (`--mode 3`), correlating
each draw with the preceding slot-gate call. `FUN_0042b8b0`/`FUN_0042b8c0` return
`DAT_0067ea54`/`DAT_0067ea56` (screen width/height), and the measured x/w/h come
out directly in the port's virtual-640 space (the detail-panel column measured
`x=520 w=24`, which is exactly what the port already draws as `520.0f * kVScale` /
`24.0f * kVScale` — so measured numbers are usable as `N * kVScale` verbatim).

## Screen-6 row list, complete measured model (fresh save, sel=0)

| element | x | y | w×h | colour | gate / source | condition |
|---|---|---|---|---|---|---|
| category sprite | ~208 (anim 197–209) | ~126 + row·pitch | ~45×45 (anim) | `0xffffffff` | `FUN_0042ee40` → INTERFACE (`MultiPlayer` on screen 6) | selected row only |
| state icon | **220** | ~159, pitch **22** | **22×22** | **`0x3f000000`** (stable over 723 draws) | BADGES gate `0x0042ee00` | non-selected rows |
| (selected state icon) | 220 | — | 44×44 | white | INTERFACE gate `0x004391b0` | **suppressed** here: slot-2 `tick` is gated on `FUN_00430760()`, which returns non-zero on this state, so the gate returns 0 and nothing draws |
| detail-panel checklist | 520 | 320 step 16 | 24×24 | `0xffffffff` | BADGES gate (per-mode flags) | already ported (main note) |

So on a fresh save, screen 6 shows: the **selected** row with a big white
`MultiPlayer` category icon and no small badge; the **other** rows each with a
faint (`α=0x3f`) black `check` at x=220. There is **no Star on any row**, and
nothing at all at x≈36.

## What the port draws instead

A bright white pulsing `Star` on **every** row at x≈36 (`stx=20.134`,
`sts=31.616`), plus the orange selection bar and the name. The star column
(x≈36) matches **no** measured original draw on screen 6; the real per-row
element is the x=220 status glyph, and the selected row's real icon is the
`MultiPlayer` category sprite the port does not load. The earlier "star measured
against orig_s6 centroids" note could not be reconciled with this trace — no draw
lands near x=36 — so that measurement was of something else or on another state;
it is not screen 6's row element.

## Why the fix is NOT shipped in this session

The geometry pass is **done** — every column, size, colour, gate and the
selected-row suppression are measured. But turning it into code is a
multi-texture **composition** change:

1. load the category sprites (`MultiPlayer`/`QuickRace`/… from INTERFACE.TXD via
   the `FUN_0042ee40` screen dispatch) — not currently loaded;
2. load `dot` (BADGES) alongside the `lock`/`check` already added;
3. draw the category icon on the selected row and the `α=0x3f` black status glyph
   on the others, at the measured geometry;
4. remove the unmatched pulsing star;
5. model `FUN_00430760()`'s suppression of the selected-row tick.

`re/CONFIDENCE.md` and CLAUDE.md hold composition fixes to more than
compile-and-run (a parity/draw-stream or screenshot check). This headless session
**cannot produce any capture** — the standalone exits on focus loss and there is
no foreground desktop — so shipping a dramatic visual change (removing a
prominent element, adding two new sprite families) with zero verification would
violate that bar and risks a visibly broken screen with no safety net. The
measured model above makes it a ~15-minute implement-and-verify for a session
with a desktop; it is handed off rather than shipped blind. The `[SCAFFOLD]`
comment at the star draw carries the exact numbers inline.
