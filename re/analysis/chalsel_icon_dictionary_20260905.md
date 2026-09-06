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
