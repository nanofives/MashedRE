---
session: ai_path_following-20260512
date: 2026-05-12
pool_slot: Mashed_pool2
model: claude-sonnet-4-6
rva_count: 7
hard_cap: 20
---

## Summary

Session 20 — first-pass investigation + sweep of the AI path-following subsystem. Entry-point discovery found candidates immediately (no DEFERRED outcome). Analyzed 7 new RVAs covering the full path-following execution chain from the AI tick loop down through the spline data file loader. No STOP-AND-ASK was triggered: path-following is implemented in native code, not inside .AI scripts.

## Functions analyzed

| RVA | Name | Notes |
|-----|------|-------|
| 0x00418860 | FUN_00418860 | AI tick loop: guards on race-lines count > 3; iterates 4 vehicles; calls FUN_004177b0 then FUN_00418560 per slot |
| 0x004177b0 | FUN_004177b0 | AI pre-tick: updates race-angle array (DAT_0089a880), mode-specific speed rubber-banding (modes 4/9), slow-line toggle timer, probability-table candidate selection |
| 0x00444b60 | FUN_00444b60 | Spline debug visualizer: world→screen projection of spline points + Catmull-Rom curve; calls FUN_00443dc0 to mark lookahead target |
| 0x00444ff0 | FUN_00444ff0 | AI debug overlay: sub-mode labels ("race lines", "inside lines", "slow lines", "cheat lines", "danger data", "save data"); spline index display; lap-frame tracker; tile-map display |
| 0x00423670 | FUN_00423670 | Spline editor input handler: runtime point insert/delete; snaps point XZ to vehicle-0 position; saves/clears AI data; navigation keys cycle spline type/index/point |
| 0x00423480 | FUN_00423480 | AI filename generator: formats "AI%d.AI" using FUN_00426c00() track index; appends to DAT_0064410c path buffer |
| 0x00423540 | FUN_00423540 | AI data saver: opens file for write, writes 0x11884-byte block from DAT_007f1a9c with magic 0x13269902 |

## Key structural findings

### Path-following execution chain

```
FUN_00418860 (AI tick loop, per-frame)
  └─ FUN_004177b0 (pre-tick: race angles + rubber-banding)
  └─ FUN_00418560 × 4 vehicles (per-vehicle AI step — ai_update C1)
       └─ FUN_00417180 (bank selector: race/inside/slow/cheat type × 0-2 index)
       └─ FUN_004161e0 (vehicle XZ → spline target — ai_update_d2 C1)
            └─ FUN_00443dc0 (Catmull-Rom lookahead finder — ai_update_d3 C1)
```

### AI data buffer layout

The entire AI data is a monolithic `0x11884`-byte block loaded from `ai.piz`/`AI%d.AI` into `DAT_007f1a9c`. Four spline line-type arrays reside at fixed BSS addresses within this range:

| Line type | Base address | Count field | Max splines |
|-----------|-------------|-------------|-------------|
| Race lines | `0x00801aa0` | `0x00801ca0` | 3 |
| Inside lines | `0x008020ac` | `0x008022ac` | 3 |
| Slow lines | `0x008026b8` | `0x008028b8` | 3 |
| Cheat lines | `0x00802cc4` | `0x00802ec4` | 3 |

Each spline: up to 64 XZ float2 points at `base[i*8]`/`base[i*8+4]`, count at `base+0x200`, stride 0x204.

### AI data file format

- Source: `d:\ToastArt\Common\ai.piz` (opened by `FUN_004235b0`, already C1 track_loader)
- Filename within piz: `"AI%d.AI"` where `%d` = track index from `FUN_00426c00()`
- Record magic: `0x13269902`
- Total size: `0x11884` bytes (71556)
- Loaded into `DAT_007f1a9c`; saved back by `FUN_00423540`

### Per-vehicle path-following state (stride 0x74, base DAT_0089a4cc)

| Field | Address | Stride | Role |
|-------|---------|--------|------|
| Line type | `DAT_0089a4cc` | `0x1d DWORDs` | 0=race, 1=inside, 2=slow, 3=cheat |
| Spline index | `DAT_0089a4d0` | `0x1d DWORDs` | 0..2 within line type |
| Switch-request | `DAT_0089a500` | `0x74` | Non-zero triggers bank change |
| Switch timer | `DAT_0089a504` | `0x74` | Counts to 9000 |
| Behavior mode | `DAT_0089a52c` | `0x74` | 0..10; written by FUN_00416250 |

## Trackers

| Tracker | New entries | IDs |
|---------|-------------|-----|
| hooks.csv | 7 rows | 0x00418860, 0x004177b0, 0x00444b60, 0x00444ff0, 0x00423670, 0x00423480, 0x00423540 |
| UNCERTAINTIES.md | 14 rows | U-3612..U-3625 |
| STUBS.md | 0 new | — |
| DEFERRED.md | 0 new | — |

## Pre-existing functions cross-referenced (not re-analyzed)

- `FUN_004161e0` (0x004161e0) — already C1 in `re/analysis/ai_update_d2/0x004161e0.md`
- `FUN_00417640` (0x00417640) — already C1 in `re/analysis/ai_update/0x00417640.md`
- `FUN_004235b0` (0x004235b0) — already C1 in `re/analysis/track_loader/0x004235b0.md`
- `FUN_00418560`, `FUN_00417180`, `FUN_00443dc0`, `FUN_00443300` — all C1 in ai_update bucket

## Pool slot

Mashed_pool2 — read-only; no writes; released after session.
