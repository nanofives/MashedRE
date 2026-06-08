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
