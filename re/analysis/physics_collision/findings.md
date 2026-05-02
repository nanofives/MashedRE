# physics_collision — Session findings (2026-05-02)

## Session goal
Find COLLISION_FN: the RWP narrow-phase collision test that queries the BSP track geometry against vehicle rigid bodies.

## Conclusion: COLLISION_FN not found via static analysis

All references to BSP collision globals (`DAT_006bf1c8` = 0x006bf1c8, `DAT_006bf1cc` = 0x006bf1cc) are from
Lua C setup functions in the region `0x0047a000..0x0047b350`. These are track-load-time BSP primitive
constructors/setters called from `COURSE.LUA` execution, not runtime collision tests.

The RWP37Active physics engine is statically linked without symbols. Its per-frame collision dispatch goes
through vtables and function pointers that are not traceable via Ghidra's static call graph. No function with
the signature of a BSP volume test was found in the call graph reachable from the game loop.

## Call chain mapped (new this session)

```
entry (0x004a4bb7)
  → FUN_00492370 (WinMain)
    → FUN_00402750 (AppInitialiseOnBootup)
      → FUN_00492290 (game loop — while not quit: 6 per-frame calls)
        → FUN_004929d0  [already analyzed, game_state/]
        → FUN_00492d20  (10 bytes, timer)
        → FUN_00492d30  (265 bytes)
        → FUN_00492e90  [NEW — game logic tick, state machine — see 00492e90.md]
        → FUN_004926c0  (172 bytes)
        → FUN_00493480  (177 bytes)
```

Track load chain (from game_state state 2 / FUN_0040d440):
```
FUN_0040d440 → FUN_0040d270 → FUN_0040d020 → FUN_00426e10 (track loader)
  → FUN_0047a020 [NEW — BSP collision init — see 0047a020.md]
    → FUN_0047b9b0 ("COURSE.LUA" Lua executor)
      → Lua C functions at LAB_00440bc0 (0x0047a1b0..0x0047ab48)
        — these set BSP collision primitive properties into DAT_0086cac0 array
  → FUN_00479330 (BSP tree builder, already in track_loader/)
  → FUN_004715a0 [NOT yet written — links physics scenarios to BSP objects]
```

## BSP system layout

| Address | Size | Purpose |
|---|---|---|
| `DAT_006bf1c8` | 4 | Global: pointer to BSP struct A (`&DAT_00644378`) |
| `DAT_006bf1cc` | 4 | Global: pointer to BSP struct B (`&DAT_00644158`) |
| `DAT_0086cac0` | 0x2200 (8704) | 32-entry BSP collision primitive table, stride 0x110 (272) |
| `DAT_0086ece0` | ~0x2510 × N | Physics scenario array, stride 0x2510, count at `DAT_00691480` |
| `LAB_00440bc0` | unknown | Lua C function registration table for BSP primitive setters |

## Why COLLISION_FN requires Frida

RWP37Active physics update dispatches collision queries through:
1. A vtable on each physics object (vehicle rigid body → BSP volume test)
2. The BSP object at `DAT_006bf1cc + 0x6c` (object count field, updated per-primitive-add)

The actual narrow-phase function (GJK body, confirmed statically linked via SCCS string
`//Physics/Rwp37Active/src/core/volume/src/RwpGjk.c`) cannot be anchored without a breakpoint on
read to `DAT_006bf1cc` during a live race.

## Recommended next step

Run Frida with a hardware watchpoint on `DAT_006bf1cc` (0x006bf1cc) during a race. The first read that
occurs on-frame (not during track-load) will be the runtime collision test. Capture the full call stack to
identify the RWP dispatch chain.
