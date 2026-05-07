# Session PPP — Cheat code detection
**Date:** 2026-05-03  
**Slot:** Mashed_pool9  
**Outcome:** HALTED — no player-facing cheat-code system found

---

## Anchor outcome: false positive, then exhausted

Only one string matched any cheat-related query across all nine queries in the sweep:

| Address | String | Query hit |
|---------|--------|-----------|
| `0x005ce144` | `"cheat lines"` | `cheat` / `Cheat` / `CHEAT` |

All other queries — `UNLOCK`, `Unlock`, `unlock`, `secret`, `combo`, `Konami`, `ABBA`, `UDLR`, `god`, `invincible`, `password` — returned zero results.

---

## What "cheat lines" actually is

**It is not a player cheat code.** It is AI/racing-line terminology.

The string lives at `0x005ce144` inside a named-option array at `0x005ce100`:

```
race\0       0x005ce10f  (= spline 0)
inside lines 0x005ce114  (= spline 1)  [Ghidra label: DAT_005ce114]
clear data   0x005ce11f  [unrelated – clear-save confirmation]
?????        0x005ce125  [unrelated]
save data    0x005ce12f  [unrelated – save-data confirmation]
danger data  0x005ce13b  [unrelated]
cheat lines  0x005ce144  (= spline 3)
slow lines   0x005ce150  (= spline 2)
inside lines 0x005ce15a  (= spline 1)
race lines   0x005ce166  (= spline 0)
```

These are labels for the four AI pathfinding splines embedded in each track:
- **race lines** — the ideal racing line
- **inside lines** — tighter inner arc
- **slow lines** — pre-brake entry line
- **cheat lines** — AI shortcut / overtaking shortcut through off-track or chicane-cut

This is standard racing-game terminology (not a player cheat system).

---

## Debug HUD draw function — `FUN_00444ff0` (`0x00444ff0`–`0x00445a82`)

The only cross-reference to `"cheat lines"` is a DATA ref from `0x00445288` inside `FUN_00444ff0`.

`FUN_00444ff0` is a **developer debug overlay draw function** guarded by flag `DAT_007f1a50`. It draws a multi-page debug menu via vtable-dispatched sprintf + `FUN_005586f0` (text renderer). Menu pages:

| `DAT_007f1a54` | Page name |
|---|---|
| 0 | Main debug menu (9 options including the 4 spline-line types) |
| 1 | "clear data?" confirmation |
| 3 | "save data?" confirmation |
| 4 | Race options (start/stop race, single step, lap frames) |
| 6 | Spline editor ("Edit Spline %d / Edit Point %d") |

---

## Debug menu input handler — `FUN_00423670` (`0x00423670`–`0x00423aeb`)

Handles ENTER/SELECT on each menu item. When page=0 and cursor=3 ("cheat lines"):

```c
// 0x00423758
DAT_007f1a54 = 6;       // navigate to spline editor
DAT_007f1a64 = 2;       // cursor to "Insert Point"
DAT_007f1a68 = iVar2;   // iVar2 == DAT_007f1a64 at entry == 3  → spline #3
```

This simply opens the spline editor for spline index 3. No cheat sequence, no flag set.

---

## Input state layout (observed globals)

| Global range | Role |
|---|---|
| `DAT_007f1042`–`DAT_007f1077` | Per-button current-frame held (byte per button) |
| `DAT_007f1502`–`DAT_007f1535` | Per-button previous-frame held (edge detection) |
| `DAT_007f1a54` | Debug menu page index |
| `DAT_007f1a64` | Debug menu cursor (item index) |
| `DAT_007f1a68` | Current spline type index |
| `DAT_007f1a6c` | Current spline instance index |
| `DAT_007f1a50` | Debug overlay enabled flag |

The input state is a **flat per-frame boolean array** — there is no circular history buffer anywhere in the observed call graph.

---

## CHEAT_FN search result

**Not found.** No function matching the heuristic (circular buffer of recent button presses → compare against static pattern array → set flag on match) was located. There is no evidence of a player-facing cheat-code input system in MASHED.exe.

---

## Possible explanations

1. **Mashed has no button-sequence cheat codes.** It is a 2004 budget racing game; many titles in this class omit runtime cheat codes entirely and instead use a separate unlock/profile system.
2. **Cheats may be activated via Lua scripting or a console command** not yet reverse-engineered.
3. **Cheats may be encoded purely as data** (e.g., a configuration file or registry key read at startup) with no in-game recognition loop.
4. **The developer debug menu** (`DAT_007f1a50` enable flag) is the closest thing to a "cheat system" — it allows editing splines, resetting cars, toggling race start/stop at runtime — but it has no documented activation sequence in the binary.

---

## Functions covered

| RVA | Name | Role | Disposition |
|-----|------|------|-------------|
| `0x00444ff0` | `FUN_00444ff0` | Debug overlay draw (all pages) | **Not hooked** — debug-only draw |
| `0x00448730` | `FUN_00448730` | Debug overlay render wrapper | **Not hooked** |
| `0x00423670` | `FUN_00423670` | Debug menu input handler | **Not hooked** |
| `0x00423040` | `FUN_00423040` | Spline point position adjuster (held-key) | **Not hooked** |

No hooks.csv rows added (halt path — no player cheat system confirmed).

---

## Tracker updates

None. Halt path: no rows added to `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, or `DEFERRED.md`.

---

## Scribe outcome

**n/a — halted.** No commit, no queue entry.
