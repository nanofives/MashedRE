# FUN_0043c5b0 — Port Spec (standalone menu draw / state-gated frontend renderer)

Source of truth: read-only Ghidra session on `Mashed_pool13` (Mashed_pool14 had a
leaked project-level lock — `LockException`, no on-disk `.lock`; fell back to
pool13, which opened clean and was confirmed `MASHED.exe`, image_base `0x00400000`,
PE32 x86). Anchor not modified; program_close issued at end.

NO-GUESSING: every constant/offset below was read from the original binary image
in pool13 or cited from the decompiler/listing. `[UNCERTAIN]` marks ambiguous decomp.

> **CRITICAL FRAMING CORRECTION.** The prompt described this as "the menu state
> machine … dispatch/entry table @0x898ac0." The decomp shows FUN_0043c5b0 is the
> **per-frame menu DRAW routine**, gated on the frontend phase variable
> `DAT_0067eca4`. The "table @0x898ac0" is a **runtime-populated** menu-item array
> (in writable `.data`, NOT `.rdata`), built by *other* functions
> (FUN_0043d2a0 push/pop, FUN_004325c0 anim-tick, FUN_0042d3e0). **It is all zeros
> in the static image** — see "Harvested constants/tables". The true "state machine"
> (screen push/pop + which items populate the table) lives in FUN_0043d2a0 /
> FUN_0043bf30 / the per-screen builders, not here. This routine only *renders* the
> table the state machine produced.

---

## Signature

```c
// 0x0043c5b0  (body 0x0043c5b0 .. 0x0043d295, ~3.3 KB)
void FUN_0043c5b0(void);   // no params, no return; calling_convention "unknown" (cdecl, takes nothing)
```

- Stack frame: `SUB ESP,0x48` (0x0043c5b0). Pure local scratch (color/coord temps).
- Only caller: **FUN_00492e90** (0x00492e90, `boot`, C2 — the per-frame game-logic
  tick / frame state machine on `DAT_00771968`). So this is invoked once per frame
  from the main loop when the frontend is active.

---

## State machine (mechanical, per-state)

State variable: **`DAT_0067eca4`** (0x0067eca4, dword, signed; `.data`, =0 in static
image, set at runtime). Read at entry: `MOV EAX,[0x0067eca4]` (0x0043c5b3).
Branch ladder (signed comparisons — `CMP EAX,1` / `CMP EAX,4` / `CMP EAX,5` with
`JGE`):

| `DAT_0067eca4` | Behavior |
|---|---|
| `== 0` | `JZ 0x0043d292` → do nothing, return. (frontend inactive) |
| `== 1` | `FUN_0042aae0(1)`; vtable call `(*(DAT_007d3ff8+0x20))(6,1)`; `(*(...+0x20))(8,1)`; **return**. (minimal/transition phase) |
| `>= 2` (else branch @0x0043c647) | Main draw path: `(*(...+0x20))(6,0)`; `(*(...+0x20))(8,1)`. Then phase sub-gates below. |
| `< 5` (i.e. 2..4) | `FUN_0042e5b0(0)` (MenuChromeShellB); `FUN_0042e3a0()` (MenuChromeShellA, C3); `iVar3=FUN_0042b930()`; if `iVar3 != 0x21`: two `FUN_00427e00(...)` sprite draws (chrome). |
| any `>=2` | `FUN_0042aae0(1)` again. |
| `< 4` (i.e. 2..3) | **Menu-item draw loop** over table 0x00898ac0 (see below), then `FUN_0043bf30()` (sub-menu dispatcher). |
| any `>=2` (tail) | `(*(...+0x20))(6,1)`; `(*(...+0x20))(8,1)`; return. |

`DAT_007d3ff8` (0x007d3ff8) is a renderer/device object pointer; `+0x20` is a vtable
slot called as `(int enable_a, int enable_b)`. `[UNCERTAIN]` exact semantics of the
`(6,..)`/`(8,..)` args — they are raw literals 6 and 8 toggled 0/1 (render-state
enable pair). In the standalone these vtable calls go through the existing fake RW
device at `*(0x007d3ff8)` (already wired — see RwIm2DBridge). For a *pure menu-item*
port they can be treated as begin/end render-state guards.

### The menu-item draw loop (phase 2..3)

```
iStack_3c = 0;                       // running visible-item index
piVar9 = &DAT_00898aec;              // = table base 0x00898ac0 + 0x2c (record field cursor)
do {
    iVar3 = piVar9[-0xb];            // field at record+0x00  (the "color/anim tag")
    ... dispatch on iVar3 (signed) ...
    piVar9 = piVar9 + 0xd;           // stride = 0xd ints = 0x34 = 52 bytes
} while ((int)piVar9 < 0x899104);
```

**Record stride = 0x34 (52 bytes) = 13 ints.** `piVar9` is positioned at record
offset `+0x2c` (so `piVar9[-0xb]` = record+0x00). Field map per record (relative to
record base, derived from `piVar9[-N]`):

| expr | record offset | meaning (mechanical) |
|---|---|---|
| `piVar9[-0xb]` | +0x00 | anim/color **tag** (signed; switch key) |
| `piVar9[-0xa]` | +0x04 | item **type** flag (loop checks `==0`, `==2` vs else) |
| `piVar9[-8]`   | +0x0c | ARGB color (`piVar9[-8]` passed as color arg) |
| `piVar9[-7]`   | +0x10 | slide/anim counter (compared `<0x1ff`, `/10`); also passed as last arg to FUN_00428140 |
| `piVar9[-6]`   | +0x14 | float scale (passed to FUN_00428140 arg5) |
| `piVar9[-5]`   | +0x18 | X coord (int→used as float) |
| `piVar9[-4]`   | +0x1c | Y coord (int→used as float) |
| `piVar9[-2]`   | +0x24 | int flag (FUN_00428140 arg6) |
| `piVar9[-1]`   | +0x28 | secondary sprite/text id (the "right column" item; -1 = none) |
| `piVar9[0]` (`*piVar9`) | +0x2c | primary sprite/text id (-1 = none; 0x224 special-cased) |

Table extent: base **0x00898ac0**, loop runs while cursor `< 0x899104`. With the
cursor starting at `0x898aec` (= base+0x2c) and stepping 0x34, the records span
`0x898ac0 .. 0x899104` = **0x644 bytes = 1604 bytes = 30.8 records**. The loop's
terminate test is on the *field cursor* (`0x898aec + n*0x34 < 0x899104`) → it
processes records 0..30 (cursor 0x898aec..0x899100), i.e. **31 record slots** of 52
bytes. `[UNCERTAIN]` whether the table is exactly 31 slots or the tail slot is a
sentinel — confirm against the writer FUN_0043d2a0 (it WRITEs 0x00898ac0).

Tag dispatch (signed `iVar3 = record+0x00`):

| tag value (hex / signed dec) | branch |
|---|---|
| `-0x1000000` (−16777216) | `LAB_0043d185` (static draw, no slide offset) |
| `-0xf00000` (−15728640) | `LAB_0043d185` |
| `-0xef0000` (−15663104) | `LAB_0043d185` |
| `-0xfc0000` (−16515072) | **slide-in / countdown** path (the big block: highlight quads via FUN_00472c60/FUN_00473540, button bg via FUN_0040bb50, text via FUN_00428140) |
| `-0xee0000` / `-0xed0000` / `-0xdd0000` (−15597568 / −15532032 / −14483456) | fall to `LAB_0043d185` |
| anything else | `LAB_0043d25d` (skip this record) |

At `LAB_0043d185` (static items): if type (`+0x04`) is 0 or 2 → draw primary id
(`*piVar9`, skip if −1) twice via FUN_00428140 — once shadow-offset by
`_DAT_005cc31c` (=3.0f) with color 0xff000000, once at base pos with color
`piVar9[-8]`. Mirror for the secondary id (`piVar9[-1]`) when type != 0 && != 2.

The `-0xfc0000` block additionally consults cursor/selection state:
`DAT_0067e9f8` (current screen-stack depth index, ×0x10 stride),
`(&DAT_0067ed38)[DAT_0067e9f8*0x10]` (per-screen "active list" pointer; compared by
*identity* against screen-table sentinels `&DAT_005f72a0 / 005f7370 / 005f6da0 /
005f6df8`), `(&DAT_0067ed80)[...]` and `(&DAT_0067ed40 + ...*4)` (the per-screen
**selection cursor** arrays — `iStack_3c == *(&DAT_0067ed40 + screen*0x40)` is the
"is this item the highlighted one" test). `DAT_0067e914` and `DAT_0067e844` are mode
flags gating a `uVar7` index bias (0 or 2). `DAT_008990e4` is a global "dim all" flag
(0 = full alpha; nonzero = draw at 0x80 alpha / 0x80000000 color).

Highlight rendering primitives in the selected branch:
`FUN_00472c60` (ChromeBaseDraw, C3 — filled quad), `FUN_00473540` (gradient quad),
`FUN_00472dc0` (filled triangle, for the 0x224 special item), `FUN_0040bb50`
(SpriteLookupC "Button", C3), `FUN_004739f0` (submit), `FUN_004a2c48` (float→int
round, C2 — used to size the highlight to text width), `FUN_00428140` (sprite/text
draw w/ alpha-fade, C2). Loop end: `FUN_0043bf30()` (sub-menu dispatcher, C2).

---

## Dispatch table @0x898ac0 (entries)

**There is NO static dispatch/entry table at 0x898ac0.** The address is a
**runtime-populated array** in writable `.data` (block `.data` 0x005ea000..0x00914703,
read+write). Verified by reading 1604 bytes `0x00898ac0..0x00899104` from the original
image (pool13 `memory_read`): **every byte is 0x00.** Ghidra labels it `DAT_00898ac0`
(undefined4, value 0). Cross-references to 0x00898ac0:

| from RVA | type | meaning |
|---|---|---|
| 0x0043d4ee | WRITE | inside **FUN_0043d2a0** (screen push/pop; populates the table) |
| 0x0042d3e7 | WRITE | inside **FUN_0042d3e0** (a table-init/setter) |
| 0x0042ad19 | DATA  | inside FUN_0042aae0 region (ref) |
| 0x004325f2 | READ  | inside **FUN_004325c0** (menu-slide anim tick) |
| 0x0043c730 | READ  | inside **FUN_0043c5b0** (this draw loop) |

So the "entries" the port must reproduce are produced at runtime by the screen
builders. **The reimplementation must construct the 31×52-byte record array itself**
(populating tag/type/color/coords/ids per active screen), then feed it to a port of
this draw loop. The record layout above is the contract.

---

## Harvested constants/tables (RVA → bytes)

All `.rdata` (block 0x005cc000..0x005e9fff, read-only) float/scalar reads, verbatim
little-endian bytes from pool13:

| RVA | bytes (LE) | value | role |
|---|---|---|---|
| 0x005cc31c (`_DAT_005cc31c`) | `00 00 40 40` | 3.0f | shadow-offset added to X/Y for the drop-shadow draw |
| 0x005cc320 (`_DAT_005cc320`) | `00 00 80 3f` | 1.0f | added in `fStack_14 = piVar9[-4] - _DAT_005cd8d8 + _DAT_005cc320` |
| 0x005cc574 (`_DAT_005cc574`) | `00 00 00 40` | 2.0f | width threshold (`< fStack_1c` / `< local_24`) gating the wide-highlight branch |
| 0x005cd8d8 (`_DAT_005cd8d8`) | `00 00 50 41` | 13.0f | X bias subtracted before FUN_0040bb50 button draw and from Y |

Inline immediates (encoded in the instruction stream of FUN_0043c5b0, harvested from
the decomp + listing — these are float bit-patterns / ARGB / item-ids; the port
hard-codes them):

| immediate | as float / meaning |
|---|---|
| `0x441fc000` (ESP+0x20 init, 0x0043c5ca) | 639.0f — scratch rect right |
| `0x43ef8000` (ESP+0x24 init, 0x0043c5d2) | 479.0f — scratch rect bottom |
| `0x40f8d0e8` (ESP bytes e8 d0 f8 40) | 7.776...f — a color/alpha temp seeded at entry |
| `0x3f4ccccd` | 0.8f — scale arg to the two chrome FUN_00427e00 calls |
| `0x44160000` / `0x42500000` | 600.0f / 52.0f — chrome sprite #1 X/Y |
| `0x44150000` / `0x42400000` | 596.0f / 48.0f — chrome sprite #2 X/Y |
| `0x42700000` | 60.0f — default highlight quad X (`uVar5` default) |
| `0x42680000` | 58.0f — narrowed highlight X |
| `0x43260000 / 0x42a80000 / 0x43620000` | 166 / 84 / 226 — quad metrics, "list A" screens |
| `0x43520000 / 0x42c80000 / 0x43870000` | 210 / 100 / 270 — quad metrics, "list B" screens |
| `0x41500000` | 13.0f — width arg to FUN_0040bb50 |
| `0x43a00000` | 320.0f — center X for the 0x224 special item |
| `0xff1050b4 / 0xff501513 / 0xa0146ef0 / 0x40f8d0e8` | ARGB colors: selected-fill / alt-fill / shadow / idle |
| `0xff000000 / 0x80000000` | opaque-black shadow / dimmed |
| `0x224` (548) | special item id (centered, fixed-pos, triangle-bordered) |
| `0x21` (33) | sentinel from FUN_0042b930 that suppresses the two chrome sprites |
| tags: `0xfc0000 0xef0000 0xf00000 0xee0000 0xed0000 0xdd0000 0x1000000` | the negated tag constants in the loop switch |

Screen-table identity sentinels (compared by ADDRESS, not value — these are
`.data` slot addresses, not constants to harvest): `&DAT_005f72a0`, `&DAT_005f7370`,
`&DAT_005f6da0`, `&DAT_005f6df8`. They select list-A vs list-B quad metrics. In the
standalone the port chooses metrics by which screen is active, not by pointer compare.

Runtime-state globals (all `.data`, =0 in static image; the port must own equivalents):
`DAT_0067eca4` (phase), `DAT_0067e9f8` (screen-stack depth), `DAT_0067e914`,
`DAT_0067e844` (mode flags), `DAT_008990e4` (dim-all), `DAT_007d3ff8` (device ptr),
arrays `(&DAT_0067ed38)` (per-screen active-list ptr, ×0x10), `(&DAT_0067ed40)`
(per-screen selection cursor, ×0x40 = 0x10 ints), `(&DAT_0067ed80)` (per-screen
item-count/cursor, ×0x10).

---

## Callees (RVA, C-level, port-needed?)

| RVA | name (hooks.csv) | subsystem | C-level | port needed? |
|---|---|---|---|---|
| 0x0042aae0 | FUN_0042aae0 | frontend | (not C2+ in csv — C1/below) | yes — render-state setter; or stub via fake device |
| 0x0042e5b0 | MenuChromeShellB | frontend | C2 (re-enabled, diff GREEN) | reuse existing reimpl |
| 0x0042e3a0 | MenuChromeShellA | frontend | **C3** | reuse existing reimpl |
| 0x0042b930 | FUN_0042b930 | frontend | (returns a screen-id; not C2+) | yes — supplies the `!=0x21` gate |
| 0x00427e00 | FUN_00427e00 | frontend | C2 | sprite draw — reuse |
| 0x0042aad0 | FUN_0042aad0 | frontend | (helper; not C2+) | yes — recomputes ECX/EDX cursor; small |
| 0x00472c60 | ChromeBaseDraw | frontend | **C3** | reuse (DrawQuadPrimitives.cpp) |
| 0x00473540 | FUN_00473540 | frontend | C2 | gradient quad — reuse |
| 0x00472dc0 | FUN_00472dc0 | frontend | C2 | triangle (only for id 0x224) — reuse |
| 0x004a2c48 | FUN_004a2c48 | frontend | C2 | float→int round — reuse |
| 0x0040bb50 | SpriteLookupC | frontend | **C3** | reuse (SpriteCluster.cpp) |
| 0x004739f0 | FUN_004739f0 | frontend | (submit; not C2+) | yes — small submit helper |
| 0x00428140 | FUN_00428140 | frontend | C2 | **central** sprite/text-with-alpha draw — reuse |
| 0x0043bf30 | FUN_0043bf30 | frontend | C2 | sub-menu dispatcher (14 conditional calls) — reuse or stub |
| (table writer) 0x0043d2a0 | FUN_0043d2a0 | frontend | C2 | **YES — the state machine** (screen push/pop, builds 0x898ac0). Must port to populate the record array. |
| (anim tick) 0x004325c0 | FUN_004325c0 | frontend | C2 | **YES** — advances slide/countdown counters in the table each frame. |
| (table init) 0x0042d3e0 | FUN_0042d3e0 | frontend | (small; not C2+) | likely yes — seeds the table |
| (caller) 0x00492e90 | FUN_00492e90 | boot | C2 | the per-frame tick that calls this — the standalone RenderFrame is its analogue |

**Net: ~4 functions genuinely need porting** to drive the table (FUN_0043d2a0 state
machine + FUN_004325c0 anim tick + FUN_0042d3e0 init + the 3 tiny inline helpers
0x0042aae0/0x0042aad0/0x004739f0). The heavy draw primitives are already C2/C3
reimplemented in `mashedmod/src/mashed_re/Frontend/`. The draw loop body
(FUN_0043c5b0 itself) is mostly orchestration over those.

---

## Standalone wiring point (files + plug-in location)

The standalone's current hand-rolled menu nav (B19c) lives in
**`mashedmod/src/mashed_re/exe_main.cpp`**:

- `struct MenuItemTex { int handle; std::uint32_t w,h; bool ready; };`
  `MenuItemTex g_menu_items[8]` (line 438-439) — current ad-hoc item array.
- `std::uint32_t g_menu_selected` (line 441) — current selection index (the
  standalone analogue of `(&DAT_0067ed40)` selection cursor).
- `wchar_t g_menu_msgs[8][64]` (line 450) — decoded item strings.
- `void UpdateMenuSelection()` (lines 686-698) — the keyboard nav: DIK_UP/DIK_DOWN
  edge-detect, clamps `g_menu_selected` in `[0, g_menu_item_count)`. **The comment at
  683-685 explicitly names FUN_0043c5b0 + 0x898aec as the work this stands in for.**
- `bool RenderFrame()` (line 834) — per-frame draw; the menu-item draw loop is
  lines **924-945** (`for i in g_menu_item_count` → bright if `i==g_menu_selected`,
  via `DrawMashedString` faithful path or `HudIm2DQuad` fallback).
- `void LoadMenuItems()` (line 1410) — builds `g_menu_items` from USA.DAT strings.

Faithful font + text rasterization sit in
**`mashedmod/src/mashed_re/D3d9Render/MashedFont.cpp`** and
**`mashedmod/src/mashed_re/D3d9Render/TextRenderer.cpp`** (`RenderTextToBGRA`,
`DrawMashedString`).

There is also an existing **`mashedmod/src/mashed_re/Frontend/MenuStateMachine.cpp`**
(hook-side reimpls) and **`MenuChrome.cpp`** — the ported draw loop should land
alongside these, but the standalone *consumes* it through `exe_main.cpp`.

**Plug-in plan:** replace the flat `g_menu_items[8]` / `g_menu_selected` model with a
31×52-byte record array matching the layout above, populate it from a ported
FUN_0043d2a0/FUN_0042d3e0 (per active screen) + tick it with a ported FUN_004325c0,
then have `RenderFrame()` (lines 924-945) call a ported `MenuDrawLoop()` (port of
FUN_0043c5b0's loop) instead of the current hand-rolled `for`. `UpdateMenuSelection()`
becomes the input feeder into the per-screen selection cursor (`DAT_0067ed40` analogue).

---

## Uncertainties

- `[UNCERTAIN]` Exact table slot count: the field-cursor loop bound
  (`< 0x899104`) yields **31 slots** of 52 bytes (0x898ac0..0x899104 = 0x644).
  Confirm the writer FUN_0043d2a0 doesn't treat the last slot as a sentinel/terminator.
- `[UNCERTAIN]` Semantics of `(*(DAT_007d3ff8+0x20))(6,..)` and `(8,..)` — raw
  literals 6/8 toggled 0/1; render-state enable pair. Not decoded to named states.
- `[UNCERTAIN]` `DAT_0067e914` / `DAT_0067e844` exact meaning — mode flags biasing the
  `uVar7` (0 vs 2) index used to pick the previous-screen selection row (slide-out anim).
- `[UNCERTAIN]` Meaning of FUN_0042b930's `0x21` return that suppresses the two chrome
  sprites — screen-id sentinel, not decoded.
- The screen sentinels `&DAT_005f72a0` etc. are compared by pointer identity; their
  pointed-to contents were not harvested (undefined data, runtime-filled). The port
  selects list-A vs list-B quad metrics by active-screen id instead.
- `FUN_0042d3e0` (table init) is small and not C2+; needs its own decomp pass before
  the port can faithfully seed the record array. **Single biggest reimplementation
  risk:** the table at 0x898ac0 is 100% runtime-built and zero in the image, so the
  port's correctness hinges entirely on faithfully reproducing the *populators*
  (FUN_0043d2a0 / FUN_0042d3e0 / FUN_004325c0), not just this draw loop. Those are the
  follow-up RE targets.

---
---

# PART 2 — The populators (2026-06-08 read-only pass, Mashed_pool13)

Source: read-only Ghidra session on `Mashed_pool13` (pool12 / pool13 both had `.lock`;
pool12 refused — `LockException`; **pool13 opened clean** read_only, confirmed
`MASHED.exe` image_base `0x00400000`, PE32 x86, session
`fd72c3abe66540ecb6cc82ac85db8586`, `program_close` issued at end). Anchor untouched.
NO-GUESSING: every offset/const below read from the pool13 image (decomp + listing +
`memory_read`). C-levels cross-checked against `hooks.csv`.

> **Prior-spec correction.** The Part-1 "Uncertainties" said FUN_0042d3e0 "is small and
> not C2+." **It is now `MenuEntryArrayInit`, C3, already implemented**
> (`mashedmod/src/mashed_re/Frontend/MenuInit.cpp`). Likewise FUN_0042ac00
> (`MenuGroupCount`, C3), FUN_0042a9c0 (`ModeCodeLookup`, C3), FUN_00431d90 /
> FUN_00431f30 (both C3) are already reimplemented. The genuinely-unported populator
> logic is **FUN_0043d2a0** (nav stack + record-array builder, C2) and the small
> register-arg table iterators **FUN_0042ad90 / FUN_0042add0 / FUN_0042ac50 /
> FUN_00432800 / FUN_00432b30** (all C2).

> **KEY DISCOVERY — the menu content is STATIC, not runtime-only.** The Part-1 spec
> said the screen sentinels' "pointed-to contents were not harvested (undefined data,
> runtime-filled)." **Wrong — they are static `.data` descriptor tables.** Each screen
> has a static per-screen item-descriptor table; the array of pointers to them is
> `PTR_DAT_005f7638` (indexed by screen id). FUN_0043d2a0 *expands* the active screen's
> static descriptor table into the runtime 0x898ac0 draw-record array via the
> register-arg iterator FUN_0042ad90. So a faithful port harvests these tables verbatim
> from the binary (they survive the image-pad? **NO** — they live at 0x005f6xxx which
> is inside MASHED's zeroed range; the standalone must hard-code the harvested bytes).
> See "## Harvested consts (Part 2)".

---

## FUN_0043d2a0 (push/pop — the navigation centerpiece)

```c
// 0x0043d2a0  body 0x0043d2a0..0x0043d7b6 (~1302 B)   hooks.csv: C2
void FUN_0043d2a0(int param_1 /*=EDI, screen id*/, int param_2 /*dir*/);
//   calling_convention "unknown" (cdecl, 2 stack params at [ESP+0x30]=param_1,
//   [ESP+0x34]=param_2 after the 4 pushes); NOTE Ghidra prints (param_1,param_2)
//   but the listing shows ESI=screen-id=[ESP+0x30], EDI=dir=[ESP+0x34] — i.e. the
//   *first* stack arg is the screen id, second is the direction. (decomp's
//   param_1/param_2 are swapped vs role; trust the listing.)
```

`dir` (the `[ESP+0x34]`/`EDI` arg): **0 = push** (enter child screen), **1 = pop**
(go back), **2 = reload/replace** (rebuild current screen in place).

### Phase 1 — teardown on `dir` (0x0043d2ab..0x0043d31e)
- `dir==0` (push): `FUN_00431f30(screen_id)` (FrontendPageIdDispatch, C3 — page-enter side effects).
- `dir==1` (pop): if `DAT_0067e9f8 - 2 >= 0` → `FUN_00431f30((&DAT_0067ed7c)[(DAT_0067e9f8-2)*0x10])` (re-fire the page we're returning to).
- `dir==2` (reload): `FUN_00431d90()` (FrontendPanelFlagAdvance, C3).
- Always: `FUN_00472640(0xff)` (util setter `DAT_0086ecc8=0xff`, C2).

### Phase 2 — per-screen-id snapshot side effects (0x0043d2cf..0x0043d3e3)
Switch on `screen_id` (signed; raw constants from listing):
| id | action (verbatim from decomp) |
|---|---|
| `0x1a`(26) | `DAT_007f0ef4=DAT_007f0eec; DAT_007f0ef8=DAT_007f0ef0` |
| `0x13`(19) | copy 5 dwords `007f0f00/0c/10/08/04 → 007f0f14/24/20/1c/18` |
| `0x20`(32) | `DAT_008990d8 = FUN_0040ad20()` |
| `0x1c`(28) | loop: copy 10 dwords from `(&DAT_007f105c)[i*0x13]` → `(&DAT_00899160)[i]`, while src `< 0x7f13ec` (stride 0x4c bytes = 0x13 ints) |
| `0x1f`(31) or `0x21`(33) | copy 4 dwords `007f0f30/34/38/3c → 007f0f44/48/4c/50` |
| `1` | `FUN_00414120(); FUN_00422b30(); FUN_0040b810(); DAT_0067ea70=1` |

These are per-screen "snapshot the editable settings on entry" operations. For a
minimal nav port they are **screen-specific and optional** (the standalone owns its own
settings model); port lazily, per screen wired.

### Phase 3 — `FUN_0042d3e0()` (MenuEntryArrayInit, C3) — **zero the whole 0x898ac0 record array** (every push/pop/reload starts from a clean array).

### Phase 4 — stack mutation + slide-context selection (0x0043d3ea..0x0043d4a6)
The **navigation stack depth** is `DAT_0067e9f8` (`.data`, signed int, =0 in image).
Per-depth-slot arrays are indexed `[depth*0x10]` (0x40-byte stride per slot):
| array base | per-slot field | meaning |
|---|---|---|
| `DAT_0067ed78` | `+0x00` | active screen **descriptor-table pointer** = `(&PTR_DAT_005f7638)[screen_id]` |
| `DAT_0067ed7c` | `+0x04` | active screen **id** |
| `DAT_0067ed80` | `+0x08` | **selection cursor** (highlighted item index; -1 = none) |
| `DAT_0067ed84..` | `+0x0c..0x38` | **12 per-item availability flags** (1=enabled, 0=greyed) — written by FUN_00432800 |
| `DAT_0067edb4` | `+0x3c` | **visible item count** for this screen |
| `DAT_0067ed38` | (separate base, `+local_1c`) | a *second* per-slot screen-ptr (used for slide-out compare) |
| `DAT_0067ed74` | (`+local_1c`) | saved cursor used on pop |

- `dir==0` (push): `(&DAT_0067ed78)[depth*0x10] = PTR_DAT_005f7638[screen_id]`;
  `(&DAT_0067ed7c)[depth*0x10] = screen_id`; `(&DAT_0067ed80)[depth*0x10] = 0`;
  **`FUN_00432800(depth)`** (init availability flags + place cursor, see below);
  `local_1c = depth*0x40`. (depth is incremented at the very end, Phase 7.)
- `dir==2` (reload): `local_1c = depth*0x40` (operate on current top).
- `dir==1` (pop): `DAT_0067e9f8 = depth-1`; **if `<1` return** (can't pop past root);
  `local_1c = depth*0x40`.

### Phase 5 — build the **back-button row** (record slot 0; 0x0043d4b5..0x0043d570)
Calls FUN_0042ad90 **twice** with register args set in the listing:
- 1st: `EBX=0xff000000` (tag key −0x1000000), `EDI=0` (occurrence), `EDX=DAT_0067ed38[slot]` ptr → `SI = sVar2`.
- 2nd: `EBX=0xff000000`, `EDI=0`, `EDX=DAT_0067ed74[slot]/ed78 ptr` → `AX = sVar3`.
If either != -1, writes record slot 0 (base **0x00898ac0**) verbatim:
`+0x00=0xff000000`(tag), `+0x0c byte triple=0xff,0xff,0xff`(color rgb),
`+0x14=0x42800000`(64.0f), `+0x18=0x42400000`(48.0f), `+0x10=0x3f19999a`(0.6f scale),
`+0x24=0`, `+0x28=(int)sVar2`, `+0x2c=(int)sVar3`; and type `+0x04`/`slide +0x10`:
`{1,0}` if sVar3==-1 else `{2 or 0,0x1ff}`. Sets `local_10=1` so the main loop's
cursor starts at record slot 1 (`puVar4 = &DAT_00898ad8 + local_10*0xd`).

### Phase 6 — main item-expansion loop (0x0043d630..0x0043d764)
Per record slot (cursor `puVar4`, stride **0xd ints / 0x34 / 52 B**, base 0x00898ad8 = record1+0x18):
- `FUN_0042ad90(EBX=0xff040000 /*tag −0xfc0000*/, EDI=row_index, EDX=DAT_0067ed38[slot])` → primary id `sVar2/EBP`.
- `FUN_0042ad90(EBX=0xff040000, EDI=row_index, EDX=DAT_0067ed78[slot])` → secondary id `sVar3/EBX`.
- **Loop terminates** when BOTH return -1 (`BP==-1 && BX==BP`) → finalize and `return`.
- Otherwise writes that record slot (offsets relative to record base):
  `+0x00 = 0xff040000`(tag), color `+0x0c..0x0f = 0xff,0xff,0xff,0xff`,
  `+0x14 = 0x42800000`(64.0f X), `+0x10 = 0x3f4ccccd`(0.8f scale) — **or `0x3f147ae1`(0.578f)** when the screen ptr is `&DAT_005f72a0`/`&DAT_005f7370` (the two "list" screens) and `local_c!=0`;
  `+0x0c(int) = row_index`(=`iVar6`/EDI), `+0x18(Y) = FILD(local_c) + ST(spacing)`
  with `-_DAT_005cd900`(58.0f) bias if screen ptr == `&DAT_005f7370`, and another
  `-58.0f` if == `&DAT_005f72a0`; `+0x08 = row_index`, `+0x10(=ECX) = (int)sVar2`,
  `+0x14 = (int)sVar3`, `+0x04(type) = 0x1ff` default — **but if `DAT_0067e9f8==0`**
  (root screen) `+0x04 = 1` and slide `+0x10 = depth` (root items start visible).
- Loop step: `row_index++`, `local_c += 0x1e`(30), `local_8 += 0x1c`(28), cursor += 0x34.

### Phase 7 — finalize (0x0043d769..0x0043d7b6)
- `DAT_0067ece0 = local_4` (= count of non-(-1) primaries seen, the running item total) — on push it is written into `(&DAT_0067edb4)[slot]` (visible item count); on pop/reload it is *read back* from `DAT_0067ed74[slot]`.
- `FUN_0042ad90(EBX=0xff080000 /*tag −0xf80000*/, EDI=0, EDX=DAT_0067ed38[slot])` → id for the prompt strip.
- `FUN_00432b30(EAX=screen_kind, [ESP+0x24]=&local_4 /*record index by-ref via ESI*/, prompt_id)` — appends the L/R/back prompt-glyph row(s).
- if `dir==0`: `DAT_0067e9f8 = depth+1` (**commit the push**).
- `DAT_0067e844 = dir` (remember last direction — the Part-1 "mode flag" gating slide-out bias).

### State globals FUN_0043d2a0 owns (the menu "state")
| global | RVA | type | role |
|---|---|---|---|
| `DAT_0067e9f8` | 0x0067e9f8 | int | **nav-stack depth** (0=root). THE core state var. |
| `DAT_0067ed78[d*0x10]` | 0x0067ed78 | ptr | per-depth active descriptor-table ptr |
| `DAT_0067ed7c[d*0x10]` | 0x0067ed7c | int | per-depth screen id |
| `DAT_0067ed80[d*0x10]` | 0x0067ed80 | int | per-depth **selection cursor** (the highlight) |
| `DAT_0067ed84[d*0x10]+i` | 0x0067ed84 | int×12 | per-item availability flags |
| `DAT_0067edb4[d*0x10]` | 0x0067edb4 | int | per-depth visible item count |
| `DAT_0067ed38[d*0x10]` | 0x0067ed38 | ptr | secondary per-depth screen ptr (slide-out src) |
| `DAT_0067ed74[d*0x10]` | 0x0067ed74 | int | saved cursor (pop) |
| `DAT_0067e844` | 0x0067e844 | int | last nav direction |
| `DAT_0067ece0` | 0x0067ece0 | int | scratch: current item total |
| `DAT_0067e914` | 0x0067e914 | int | global "menu is animating" gate (read by anim tick) |
| record array | 0x00898ac0 | 31×52B | the draw records this builds |

### Callees of FUN_0043d2a0 (C-level from hooks.csv, port-needed?)
| RVA | name | C | port-needed? |
|---|---|---|---|
| 0x00431f30 | FrontendPageIdDispatch | **C3** | reuse |
| 0x00431d90 | FrontendPanelFlagAdvance | **C3** | reuse |
| 0x00472640 | (util setter) | C2 | trivial / stub |
| 0x0040ad20 / 0x0040b810 / 0x00414120 / 0x00422b30 | misc per-screen | C0/C1 | per-screen, optional |
| 0x0042d3e0 | MenuEntryArrayInit | **C3** | reuse (zero the array) |
| 0x00432800 | (avail-flag init) | C2 | **YES** — places cursor / greys items |
| 0x0042ad90 | (kv table iterator) | C2 | **YES** — the descriptor-table reader |
| 0x0042ac00 | MenuGroupCount | **C3** | reuse |
| 0x0042ac50 | (Y-centering) | C2 | **YES** — small, pure |
| 0x00432b30 | (prompt-strip builder) | C2 | optional (faithful chrome only) |

---

## FUN_0042d3e0 (init — MenuEntryArrayInit, C3, already ported)

```c
// 0x0042d3e0  body 0x0042d3e0..0x0042d41a   hooks.csv: C3 (MenuInit.cpp)
void FUN_0042d3e0(void);
```

Pure leaf. `puVar1 = &DAT_00898ac4` (= record base 0x898ac0 **+ 0x04**); loop stride
0xd ints (52 B); runs while `< 0x8990dc`. Per slot it writes **0** to 13 fields
covering record+0x00..+0x30 (clears the entire record). Extent
`0x898ac4..0x8990dc` ⇒ ((0x8990dc − 0x898ac4)/0x34) = **30 full slots** zeroed (the
loop is entered at +0x04 so it covers slots 0..29; slot 30's tail is the implicit
sentinel/scratch). This confirms the Part-1 "31 slots" bound: **30 usable record
slots**, slot index 30 is the loop-terminator guard region. **No port work needed** —
already C3. The port just calls its existing `MenuEntryArrayInit` reimpl at the top of
its `FUN_0043d2a0`-equivalent.

---

## FUN_004325c0 (anim — Menu Slide Animation Tick, C2)

```c
// 0x004325c0  body 0x004325c0..0x004327f3   hooks.csv: C2
undefined4 FUN_004325c0(void);   // returns 1 if ALL slots settled, 0 if any still animating
```

Per-frame tick. `piVar6 = &DAT_00898ad0` (= record base **+0x10**, the slide-counter
field); stride 0xd; runs while `<= 0x8990e7`. Field map relative to `piVar6`:
`piVar6[-4]`=record+0x00 (tag), `piVar6[-3]`=record+0x04 (type/phase), `piVar6[-2]`=
record+0x08, `*piVar6`=record+0x10 (**slide counter**), `piVar6[3]`=record+0x1c (Y),
`piVar6[4]`=record+0x20, `piVar6[8]`=record+0x30 (screen ptr).

- Settle gate: if `piVar6[-3] != 0x1000` (not frozen) AND `DAT_0067e914 != 0` → `local_8=0` (still animating).
- Tag dispatch (signed `iVar1 = piVar6[-4]`):
  - `-0xfc0000` (countdown): `*piVar6 += FUN_004a2c48()`; on `<1` underflow → if type==2 set counter=0x1ff & freeze(type=0x1000); else counter=0, type=1, recompute Y via `FUN_0042ac50()` + `piVar6[4]*0x1e` (−58.0f bias for the two list-screen ptrs).
  - `-0xef0000/-0x1000000/-0xee0000/-0xed0000/-0xdd0000/-0xf00000` (slide-in family): `*piVar6 += FUN_004a2c48()`; on `>399` overflow → counter=0x1ff, freeze(0x1000).
  - else (float path): `piVar6[-2] += DAT_007f1004 * _DAT_005cd8fc`(300.0f); if `>= _DAT_005cd09c`(180.0f) → set `piVar6[-2]=0x43340000`(180.0f), freeze(0x1000).
- Callees: `FUN_004a2c48` (float→int round / frame-delta, C2) + `FUN_0042ac50` (Y-centering, C2). **Both must be ported** (small). DAT_007f1004 is a runtime scroll-speed global (=0 in image). **Port-needed: YES** — small, drives the slide-in animation each frame; the standalone calls it every frame on its record array.

---

## FUN_00432800 (per-screen availability-flag init + cursor placement, C2)

```c
// 0x00432800  body 0x00432800..0x00432a88   hooks.csv: C2
void FUN_00432800(int depth);
```

Called from FUN_0043d2a0 Phase 4 on push. Sets 12 per-item availability flags
`(&DAT_0067ed84)[depth*0x10 + i] = 1` (i=0..11). Then a switch on the screen id
(`(&DAT_0067ed7c)[depth*0x10]`) selectively **clears** specific flags based on live game
state (e.g. case 1: calls `FUN_0040e480(0..3,0)`; case 0x12: queries unlock/
player-count globals `DAT_007f0a50/a58`, `FUN_00430b60`, `FUN_0042f500` and disables
rows; case 0x1c: per-vehicle `thunk_FUN_00497450(0..3)` availability + seeds 8 fixed
ids `DAT_00898aa0..` = 0,1,2,3 / `DAT_00898b1c..` = 0x236..0x239). Finally **places the
selection cursor**: starting from the stored cursor (`DAT_0067ed80[depth]`), it scans
forward (wrapping at `DAT_0067edb4[depth]` = item count) to the first item whose
availability flag ==1, and stores it; if none enabled, cursor = -1. **Port-needed:
YES for cursor placement** (the "first selectable item" logic is the nav contract); the
per-screen grey-out cases are state-dependent and ported lazily per wired screen. Heavy
state deps (unlock globals, FUN_00430b60/0042f500/00497450 are sub-C2) make a *faithful*
port of the grey-out cases a **partial blocker** — but a minimal port can flag-all-=1
and just run the cursor-placement tail.

---

## FUN_00432b30 (prompt-strip glyph builder, C2)

```c
// 0x00432b30  body 0x00432b30..0x0043305e   hooks.csv: C2
void FUN_00432b30(int screen_kind /*EAX*/, int* rec_index /*ESI, by-ref*/, int prompt_id);
```

Large branch table (screen_kind × relation-code `iVar3` from `FUN_0042add0`) that
appends one prompt row to the record array at `(&DAT_00898ae8)[*rec_index*0xd]` /
`(&DAT_00898aec)[...]`, picking glyph ids `0x42,0x43,0x48,0x13,0x58,0x133,0x225` (the
back/forward/L-R navigation indicator sprites), then `*rec_index += 1`. Callees:
`FUN_0042add0` (per-depth variant of the kv iterator, C2), `FUN_0042ad10` (HUD-glyph
slot init `0x40,0x1ac,0`, C2), `FUN_0042a9c0` (ModeCodeLookup, C3), `FUN_0042b920`.
**Port-needed: OPTIONAL** — this only draws the on-screen "◄ ► / Back" prompt glyphs;
not required for a functional state-machine-driven menu. Defer until faithful chrome.

---

## Harvested consts (Part 2) — real bytes from original/MASHED.exe (pool13)

`.rdata` floats (block 0x005cc000..0x005e9fff, read-only; verbatim LE):
| RVA | bytes (LE) | value | role |
|---|---|---|---|
| 0x005cd8fc (`_DAT_005cd8fc`) | `00 00 96 43` | 300.0f | float-path slide speed multiplier (×DAT_007f1004) — anim tick |
| 0x005cd900 (`_DAT_005cd900`) | `00 00 68 42` | 58.0f | Y bias subtracted for the two list-screen ptrs (build loop + anim tick) |
| 0x005cd09c (`_DAT_005cd09c`) | `00 00 34 43` | 180.0f | float-path freeze threshold |

Inline immediates baked into the record writes (from listing of FUN_0043d2a0):
| immediate | float / meaning |
|---|---|
| `0x42800000` | 64.0f — record +0x14 (back-button & item X) |
| `0x42400000` | 48.0f — back-button +0x18 (Y) |
| `0x3f19999a` | 0.6f — back-button +0x10 (scale) |
| `0x3f4ccccd` | 0.8f — item +0x10 (scale, normal screens) |
| `0x3f147ae1` | 0.578f — item +0x10 (scale, list screens when local_c!=0) |
| `0xff000000` | tag −0x1000000 — back-button row tag / kv search key |
| `0xff040000` | tag −0xfc0000 — list-item row tag / kv search key |
| `0xff080000` | tag −0xf80000 — prompt-strip kv search key |
| `0x43340000` | 180.0f — anim-tick float-path frozen cap |
| `0x1ff` / `0x1000` / `1` | slide-counter cap / frozen-state code / visible-state code (record +0x04 & +0x10) |
| ids `0x42 0x43 0x48 0x13 0x58 0x133 0x225 0x236..0x239` | navigation-prompt & vehicle-tile sprite/string ids |

**The per-screen descriptor tables (STATIC).** `PTR_DAT_005f7638` (0x005f7638) is a
`void*[~34]` array of pointers to per-screen item-descriptor tables, indexed by screen
id. Harvested values (first 34 entries, LE):
```
[0]=0x005f6860 [1]=0x005f6980 [2]=0x005f6a20 [3]=0x005f6a98 [4]=0x005f6c68
[5]=0x005f6d58 [6]=0x005f6da0 [7]=0x005f6df8 [8]=0x005f6e50 [9]=0x005f6c00
[10]=0x005f7058 [11]=0x005f70d8 [12]=0x005f7190 [13]=0x005f7140 [14]=0x005f71d8
[15]=0x005f6cb8 [16]=0x005f6d08 [17]=0x005f7220 [18]=0x005f72a0 [19]=0x005f7458
[20]=0x005f7598 [21]=0x005f67e8 [22]=0x005f6900 [23]=0x005f75e8 [24]=0x005f7370
[25]=0x005f6944 [26]=0x005f7000 [27]=0x00000000 [28]=0x005f6b68 [29]=0x005f6b00
[30]=0x005f7540 [31]=0x005f6ef8 [32]=0x005f74f0 [33]=0x005f6f90]
```
(trailer at +0x88: `0x005cd760, 0x0000000a` — count/sentinel.) Each table is an array of
`(tag_key:u32, id:u32)` pairs. Example — table[0] @0x005f6860 (verbatim):
```
ff000000 00000047   ff080005 00000000   ff020050 00000000(?)...  [parsed as (tag,id):]
(0xff000000,0x47) (0xff080000,0x05) (0xff020000,0x50) (0xff030000,0x78)
(0xff040000,0x18) (0xff140000,0x00) (0xff150000,0x00) (0xff060000,0x04) ...
```
Example — table[18] @0x005f72a0 (the "list A" sentinel screen):
```
(0xff000000,0x024d) (0xff080000,0x02) (0xff020000,0x50) (0xff030000,0x78)
(0xff040000,0x150) (0xff140000,0x00) (0xff420000,...) ...
```
Iterator (FUN_0042ad90/FUN_0042ac00) walks pairs until terminator **`0xff070000`**
(= −0xf90000); group delimiter is **`0xff060000`** (= −0xfa0000). `0xff040000` entries
are the selectable list items (one per visible row); `0xff000000` is the back-target;
`0xff080000` is the prompt id. **The standalone must hard-code these harvested tables**
(they live at 0x005f6xxx, inside MASHED's image-padded zeroed range). The two screens
compared by identity in the draw/build loops are **table[18]=0x005f72a0** and
**table[24]=0x005f7370** (the wide "list" screens — track-select / car-select style).

---

## Port order + first-wire recommendation

Ranking the four functions by portability × value to make the standalone menu
state-machine-driven:

| rank | function | C | portability | value | verdict |
|---|---|---|---|---|---|
| **1** | **FUN_0043d2a0** (push/pop) | C2 | medium (deps mostly C3) | **highest** — IS the nav | **FIRST WIRE** |
| 2 | FUN_004325c0 (anim tick) | C2 | high (2 small C2 callees) | medium (slide polish) | second |
| 3 | FUN_0042d3e0 (init) | **C3** | done | n/a | already reusable |
| 4 | FUN_0043c5b0 (draw loop, Part 1) | — | high (draws C2/C3 prims) | high but downstream | wire after the array is real |

### Single best first-wire piece: **FUN_0043d2a0** (confirmed — matches the prompt's expectation).

It is the navigation centerpiece: it owns the nav stack, expands the active screen's
static descriptor table into the 31×52-B record array, places the cursor, and is the
single entry point for every push/pop/reload. Its heavy side-effect callees are already
C3 (`FUN_0042d3e0` init, `FUN_00431f30`/`FUN_00431d90` page dispatch, `FUN_0042ac00`
group-count, `FUN_0042a9c0` mode-lookup). The only *new* small ports it needs are the
kv-iterator `FUN_0042ad90` (and its variant `FUN_0042add0`), the Y-centering
`FUN_0042ac50`, and the cursor-placement tail of `FUN_00432800`.

### Exact state the standalone must OWN to wire FUN_0043d2a0
A single nav-stack module owning these (all currently the flat
`g_menu_items[8]`/`g_menu_selected` in `exe_main.cpp` — replace it):

1. **`int g_nav_depth`** ← `DAT_0067e9f8` (0=root).
2. Per-depth slot array (≥8 slots × this struct), the standalone analogue of the
   `[depth*0x10]`-indexed `.data` arrays:
   ```c
   struct NavSlot {
     int          screen_id;        // DAT_0067ed7c
     const u32*   desc_table;       // DAT_0067ed78  ← &harvested table for screen_id
     int          cursor;           // DAT_0067ed80  (highlighted item; -1 = none)
     int          item_count;       // DAT_0067edb4
     int          avail[12];        // DAT_0067ed84..  (1=enabled)
     const u32*   slide_src;        // DAT_0067ed38  (slide-out source table)
     int          saved_cursor;     // DAT_0067ed74
   };
   ```
3. **`MenuRecord g_records[31]`** — the 31×52-B record array (the 0x898ac0 analogue),
   layout exactly as Part-1's field map; zeroed by the existing `MenuEntryArrayInit`.
4. The **harvested static descriptor tables** (the 0x005f6xxx blobs) hard-coded as
   `const u32[]` and a `PTR_DAT_005f7638`-equivalent `const u32* g_screen_tables[34]`.
5. `int g_last_dir` ← `DAT_0067e844`; `int g_menu_animating` ← `DAT_0067e914`.

`UpdateMenuSelection()` (exe_main.cpp ~686) feeds DIK_UP/DOWN into `slot.cursor`
(clamped/wrapped over the `avail[]` mask); Enter/Esc call the ported
`Nav(screen_id, dir)` (= FUN_0043d2a0) to push/pop. `RenderFrame()` ticks the ported
`FUN_004325c0` then runs the ported draw loop over `g_records`.

### Blockers / partial blockers
- **FUN_00432800 grey-out cases** depend on sub-C2 game-state queries
  (`FUN_00430b60`, `FUN_0042f500`, `thunk_FUN_00497450`, unlock globals
  `DAT_007f0a50/a58`). A *faithful* per-screen disable map is blocked on those; a
  **minimal** port (all items enabled + cursor-placement tail only) is unblocked and
  sufficient to make the menu navigable. Flag for follow-up RE.
- **FUN_0042ad90 / FUN_0042add0** use implicit register args (EBX=tag, EDI=occurrence)
  rather than stack params — port them as explicit `kv_lookup(table, tag, occurrence)`;
  the listing (0x0043d4b0, 0x0043d634, 0x0043d77e) pins the exact EBX/EDI values, so
  this is mechanical, **not** a blocker.
- **FUN_00432b30** (prompt strip) is large/branchy and cosmetic — defer, not a blocker
  for state-machine-driven navigation.
- **No hard blocker** prevents wiring FUN_0043d2a0 first with a minimal FUN_00432800.

---

## Uncertainties (Part 2)

- `[UNCERTAIN]` Exact length of each per-screen descriptor table (terminator-scanned at
  runtime via `0xff070000`); the harvest above shows the format and first entries but
  the port must scan each table to its `0xff070000` terminator when transcribing. Not a
  semantic gap — mechanical transcription.
- `[UNCERTAIN]` `DAT_007f1004` (anim-tick float-path multiplier) is a runtime scroll/
  frame-time global (=0 in image); its update site was not traced this pass. The port
  needs a frame-delta source for the float-path slide (the integer slide paths use
  `FUN_004a2c48` which is already C2).
- `[UNCERTAIN]` The `screen_kind` arg to FUN_00432b30 (EAX) — its derivation in the
  caller (`MOV EAX,EBP` where EBP came from the loop) maps a screen to one of
  {1,2,4,5,6,8,10}; the exact screen→kind map was not enumerated (cosmetic prompt strip
  only).
- Confirmed-resolved from Part 1: FUN_0042d3e0 is **C3** (not sub-C2); the screen
  sentinels `&DAT_005f72a0`/`&DAT_005f7370` ARE **static descriptor tables**
  (table[18]/table[24]), not runtime-filled undefined data.

---
---

# PART 3 — Reversed item->child-screen PUSH MAP + full table harvest (2026-06-08, Mashed_pool13)

Source: read-only Ghidra session on `Mashed_pool13` (pool13 opened clean read_only,
confirmed `MASHED.exe` image_base `0x00400000`, PE32 x86, session
`fb56d4689f9a404caee3a4479670e3ec`, `program_close` issued; anchor BDCAE093...
verified before+after). This closes the two faithfulness gaps that Parts 1-2 left
open (the demo `RootChildScreen()` heuristic + only tables[0..3] harvested).

## How SELECT chooses the child screen (the real map)

Confirmed: `FUN_0043bf30` (0x0043bf30) is **NOT** the push map — it is a per-frame
flag dispatcher (14 independent `if (DAT_0067e7xx) FUN_xxx()` calls). The real SELECT
routing lives in **`FUN_0043dfd0`** (0x0043dfd0, the frontend input/update tick, the
3rd caller of FUN_0043d2a0), via this chain:

1. On select, the original reads the highlighted item's **ACTION CODE** with
   **`FUN_0042ac90`** (0x0042ac90): walk the active screen's descriptor table to the
   cursor-th item (cursor = `*(&DAT_0067ed40 + slot*0x40)`), then scan that item's
   group for the action key `0xff050000` (-0xfb0000) **or** `0xff140000` (-0xec0000);
   return the following u32; or `0xffffffff` if the group ends (`0xff060000`) first.
2. `FUN_0043dfd0` dispatches on that action code (`uVar10`) through a big `if/else`
   ladder, each branch ending in `FUN_0043d2a0(child, 0)` (push), `(0,1)` (pop),
   `(0,2)` (reload), a modal `FUN_0042bf30(...)`, or a pure side-effect.
3. **Default rule (LAB_0043fc37, 0x0043fc37):** an action code that matches no
   `0xffXX0000` case is passed *as the screen id*: `FUN_0043d2a0(uVar10, 0)`. This is
   how screen 1's items 0x21/0x22 (action 0x2/0x3) push screens 2/3.

So the action code (a per-item field in the **static** descriptor table), not the
item index, is the key. This is exactly portable: read the code, run the same ladder.

### Reversed action-code -> push-target map (cited to FUN_0043dfd0 decomp lines)

| action code | -> child / behavior | class | line |
|---|---|---|---|
| 0xff430000 | push 0x13 | unconditional | L1116 |
| 0xff490000 | push 7 | uncond | L1138 |
| 0xff4b0000 | push 1 | uncond | L1133 |
| 0xff4d0000 | push 4 | uncond | L941 |
| 0xff500000 | push 8 | uncond | L1187 |
| 0xff710000/0xff720000 | push 4 | uncond | L1198/1204 |
| 0xff730000 | push 0x1e | uncond | L1183 |
| 0xff830000 | push 0x20 | uncond | L1229 |
| 0xff2c0000/0xff2e0000 | push 4 (LAB_0043f1fd) | uncond | L402/816 |
| 0xff2d0000/0xff2f0000 | push 10 | uncond | L287/400 |
| 0xff300000/0xff310000 | push 4 (LAB_0043f203) | uncond | L849/853 |
| 0xff360000 | push 0xb | uncond | L863 |
| 0xff380000 | push 0xd | uncond | L867 |
| 0xff3b0000 | push 6 | uncond | L914 |
| 0xff400000 | push 4 | uncond | L273 |
| 0xff150000/0xff3a0000/0xff4a0000 | reload(0,2) | uncond | L338/907/1145 |
| 0xff450000 | pop(0,1) | uncond | L1120 |
| 0xff240000 | push 7 (primary) | **state-gated** [UNCERTAIN] | L303 |
| 0xff3c0000 | push 0xf when FUN_0042bb60()==0x1000 else modal | **gated** [UNCERTAIN] | L889 |
| 0xff3d0000 | push 4 (also sets DAT_0067f184) | **gated** | L257 |
| 0xff820000 | push 0x1f when FUN_00402f40()==0 else 0x21 | **gated** [UNCERTAIN] | L1212/1215 |
| 0xff4c0000 | push 1 when DAT_0067ed3c==0x17 else pop | **gated** [UNCERTAIN] | L1158/1165 |
| 0xff1e/1f/20 0000, 0xff800000 | modal confirm (FUN_0042bf30) | no nav | L335/392/396/1219 |
| 0xff440000/0xff470000 | side-effect (FUN_00409900/30) | no nav | L951/1124 |
| 0xff810000 | reset cursor | no nav | L1223 |
| 0xff420000 | gated reload/modal (player-count dep) | no nav (deferred) | L953 |
| 0xff260000 | mode-tile sub-handler (internal codes) | no nav | L420+ |
| else (small value) | push `action` (literal screen id) | default rule | L841/fc37 |

For the 5 **state-gated** rows the port takes the primary/default branch and marks
`[UNCERTAIN]`; the secondary branch needs the sub-C2 game-state query ported
(FUN_0042bb60 vehicle-ready, FUN_00402f40, DAT_0067ed3c, DAT_0067ed6c). Non-nav
modal/side-effect rows are correctly no-ops in the standalone (no dialog system yet).

Frontend-enter is `FUN_0043df00` -> `FUN_0043d2a0(0, 2)`, i.e. **screen 0 = root
main menu = table[0]** (confirmed). Root items push: item1 (0xff500000)->screen 8;
items 2/3/4 open modal confirms; item0 (0xff150000) reloads.

## Tables harvested

**All 34** PTR_DAT_005f7638 entries read verbatim (33 non-null; index 27 = 0/null),
each out to its `0xff070000` terminator, now in `MenuNavSM.cpp` as `kT0..kT33` with a
34-entry `kScreenTables[]`. The earlier hand-transcribed `kTable0..3` were **wrong**
(they truncated the action codes: root item 1 action was written `0x50`, the real
little-endian bytes are `0xff500000`) — load-bearing for the push map, so replaced.
Harvest + parser + the C++ table fragments are under
`re/analysis/standalone_menu_sm/harvest/` (blk.b64 = raw bytes; tables.cpp.frag).

Reachable tree from root verified end-to-end: 0 -> 8 -> 0x13(19); plus 1 -> 2 / 3 via
the default rule. (Screens 4-7,9-26,28-33 are reachable through deeper game-state
flows; their tables are all harvested so any reachable push lands on real content.)

## VERIFICATION

- **Logic (deterministic, authoritative):** `harvest/navsm_test.cpp` links
  `MenuNavSM.cpp` and drives Nav_Init/Select/Back. Output confirms every transition:
  root(0)[0x18,0x27,0x1c,0x1d,0x1e] --ENTER item1--> screen 8[0x156,0x234,0x157,0x250,
  0x25f] --ENTER item0--> screen 19[0x158,0x159,0x260,0x15b] --ENTER(0xff450000)-->
  pop to 8 --ESC--> root; and screen 1 item0(action 0x2)--> screen 2 (default rule).
  All record arrays carry the correct real string ids at every level.
- **Visual screenshot: BLOCKED (documented, not faked).** The in-process nav-demo
  (`MASHED_NAV_DEMO=1`, RunNavDemoStep extended to the 3-level path, dumps
  verify/msm_tree_*.bmp) could not capture: in this environment `mashed_re.exe` boots
  to `B17-SUMMARY reached-main-loop chrome=YES` (menu items + faithful font load OK)
  but never creates/shows its render-loop window (`MainWindowHandle==0`), so the
  per-frame loop body (and thus the demo + RenderFrame) never runs. This is a
  pre-existing standalone runtime/boot issue in the post-init render loop, NOT in the
  menu state machine (which lives entirely inside that loop and is proven correct by
  the logic harness). The prior commit 93f0ed4c captured msm_*.png when the window
  did appear; re-run the nav-demo with the window foregrounded once the boot/window
  issue is resolved to capture msm_tree_*.png.

## PROGRESS 2026-06-08 (branch standalone/menu-state-machine)

- **Piece 1 — faithful SELECT routing: DONE** (commit 04b236ad). Ported the gated
  queries FUN_0042bb60 (team comp), FUN_00402f40 (DAT_00636ad8), per-slot
  DAT_0067ed3c, mode flags ecdc/ed6c into a MenuGameState model (fresh-menu
  reset defaults). All 5 gated cases now route faithfully:
  0xff240000→push7, 0xff3c0000→modal/no-nav (teams invalid), 0xff3d0000→push4
  (reclassified: unconditional, never gated), 0xff820000→push0x1f, 0xff4c0000→pop.
  Also fixed a port bug: CountItems/BuildRecords used KvLookup value==-1 as
  end-of-list, zeroing single-item screens whose label id is 0xffffffff
  (4/5/6/20..); now count by 0xff040000 tag. Harness navsm_test.cpp green.
- **Piece 2 — faithful grey-out: DONE** (commit 75c8b8a6). Ported FUN_00432800's
  per-screen disable switch (ids 1,2,8,10,0x12,0x18,0x1c) + queries FUN_00497450/
  00430b60/0042f500/00492d10/00430830. Disabled items render greyed
  (0x60606060) and are unselectable; cursor skips them. Verified
  verify/msm_tree_3_screen8.png (Gamma Collection/Autosave greyed) + harness.
- **Piece 3 — anim tick: DONE (slide motion)** (this commit). Ported FUN_004325c0
  (Nav_AnimTick): per-frame the record slide counter (+0x10) counts 0x1ff→0 and
  freezes; renderer maps it to a horizontal slide-in offset. Verified harness
  (settles in 13 frames, restarts on re-push) + visual (screen8 items mid-slide).
  **BLOCKED (documented):** the full pixel draw loop FUN_0043c5b0 and the prompt-
  strip FUN_00432b30 are NOT ported. Both render via MASHED's RW sprite-atlas
  primitives keyed on **sprite/glyph ids** (FUN_00428140 alpha-fade sprite,
  FUN_00472c60/73540 highlight quads, FUN_0040bb50 SpriteLookup; prompt glyphs
  0x42/43/48/13/58/133/225). The standalone has NO sprite-atlas-by-id pipeline —
  its records carry **text string ids** rendered through the FGDC20 font
  (DrawMashedString). Swapping the readable text path for the unloaded sprite
  path would regress the menu to raw numeric ids. Minimal next step to unblock:
  reimplement an RW sprite-atlas loader (TXD glyph atlas + id→quad lookup) so the
  draw loop has real sprites to emit; then port FUN_0043c5b0's tag-dispatch +
  highlight-quad branch and FUN_00432b30's prompt row. Until then the slide-anim
  + faithful-font text path is the standalone's draw.

## PROGRESS 2026-06-09 (branch standalone/menu-state-machine) — Phase 2 DONE + VERIFIED

- **FUN_0043c5b0 draw-loop port: DONE + screenshot-verified** (commits 013440e3,
  71fad07f). The Part-3 "Visual screenshot BLOCKED" note is now RESOLVED: the
  window/render loop DID run this session (`mashed_re.exe` from repo root, exit 0,
  6 backbuffer BMPs dumped). BuildRecords now writes each record's EXACT
  x/y/scale/color verbatim from the FUN_0043d2a0 record writes (X=64.0, scale 0.8 /
  back-row 0.6, Y = ported FUN_0042ac50 vertical-centering base 0xf0=240 + 0x1e(30)
  per row). The RenderFrame loop reads those exact fields (piVar9[-5]=X / [-4]=Y /
  [-6]=scale / [-8]=color), DrawMashedStrings them left-justified scaled 1.25x to
  800x600 with the +3.0f opaque-black drop shadow (LAB_0043d185), draws the selected
  item's highlight background quad (FUN_00472c60 solid 0xa0146ef0 + FUN_00473540
  gradient overlay) at the original's x=60/y=item.y-12/w=210/h=26, and the 0x224
  triangle-border frame. ChromeBaseDraw itself is NOT callable in the standalone
  (its RVA screen-getters + scale globals are image-pad-zeroed) → HudIm2DQuad
  (absolute px) is the standalone-safe analogue. Verified:
  `verify/msm_tree_1_root.png` / `_2_root_item1.png` (root in FGDC20 with real
  USA/English.DAT labels Continue/Options/Restart Race/Quit Race/Quit Game, the
  blue highlight bar tracking the cursor item0->item1), `_4_screen19.png` (depth-2
  Sound submenu: Music Volume/SFX Volume/Insults Volume/Insults + highlight + the
  "Sound" back title), `_6_back_root.png` (ESC popped to root, saved cursor
  restored). Greyed items render greyed (screen8: Gamma Collection/Autosave).
- **FUN_00432b30 prompt strip: DONE (content) / PARTIAL (special glyphs)** (commit
  bceb9ddf). `Nav_PromptId()` resolves the screen's 0xff080000 prompt id (the value
  FUN_0043d2a0 Phase 7 feeds to FUN_00432b30); RenderFrame draws it through
  MenuStringTable::Decode (control-code remap) at the strip position (virtual
  X=0x40=64, Y=0x1ac=428, scale 0x3f19999a=0.6 per FUN_0042ad10). The full
  screen_kind x relation-code branch table is cosmetic + [UNCERTAIN] (screen->kind
  map un-enumerated) so was NOT transcribed; the prompt id is the deterministic
  faithful content. The strip is visible at the bottom in every screenshot.

## RESUME HERE — ALL FOUR ITEMS + THE ANIMATED LOGO OVERLAY DONE 2026-06-09 (R2)

1. ~~FGDC20 256-entry LUT~~ **DONE (r2-3, commit c3152314).** Not a wider LUT — an
   EXTENDED-charset table: header ext_base u32 @0x24 (=0x80) / ext_count u32 @0x2c
   (=128) / u16 codepoint->glyph map @0x30, read by FUN_00554390 BEFORE the 128-entry
   ASCII table (font struct +0x124/128/12c). MashedFont resolves 0x80..0xFF through
   it; extended glyphs verified rendering (ñ in the prompt strip).
2. ~~badges.txd NAMED-sprite atlas~~ **DONE (r2-5, commit 67be20d6).** badges.txd
   lives in sfx.piz (FUN_0040bbb0 loads it into DAT_0063b8fc; SpriteLookupC =
   FUN_004c5c00(dict,name)). The existing Txd::Dictionary decodes it clean — 23
   named textures (Button 16x32, Arrow 16x16, SemiC 128x256, SemiC2 128x64...,
   probe harvest/badges_test.cpp). Draw site 0x0043cb80..cc: "Button" submitted
   via FUN_004739f0 (decompiled: (tex,x,y,w,h,color,u0,u1,v0,v1,mode,filter)),
   w=13.0, x bias 13.0 (_DAT_005cd8d8). Standalone draws the Button cap on the
   highlight bar (verified in capture).
3. ~~Frame-rate-scaled anim~~ **DONE (r2-1, commit e78a2435).** Verbatim
   FUN_00493480 frame clock (50ms tick quantization + carry; dt = ms/3000) +
   verbatim FUN_004325c0 tick (base 1x/2x on DAT_0067eca4>=4; item -2000/+1000,
   slide-class -800/+500, float path 300->180; settle Y-recompute incl. screens
   18/24; full down-up-freeze lifecycle; FUN_004a2c48 = _ftol2 TRUNCATION, not
   banker's rounding). Logic harness settles at exactly the computed 91 frames.
4. ~~Save-driven MenuGameState~~ **DONE (r2-2, commit 07432ff7).** FUN_00404e80
   step-1 span restore (file 0x24A40 -> live 0x7f0a40..0x7f0f60, 0x148 dwords)
   gated on the caller-side 0xDEADBEEF magic; has_savedata/has_profiles/track/car
   unlock gates re-derived. Shipped gamesave.bin is blank (magic 0) -> fresh
   defaults; harness verifies a written save flips screen-1/screen-18 gates.

PLUS: **animated logo overlay DONE (r2-4, commit 61c97421)** — verbatim
FUN_00473ee0/FUN_004733b0/FUN_00473220 ports; the "wavy grid" is the menu's
animated CHECKERED-FLAG chrome (renders top-right/bottom-right exactly as the
original); wave clock = µs/3e6 (FUN_00493390 @0x0049344b); fade pair
DAT_0086ecc8/cc; x-sine amplitude _DAT_005ce1f8 = 0.0 in the image (READ-only).

Remaining for full R2 phase exit (beyond the five items): side-by-side parity
sweep vs original on the canonical screen set; settings screens mutating
persisted state; all-34-screen reachability pass.
