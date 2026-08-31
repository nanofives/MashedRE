# `verify/screen_id` — frontend screen catalogue (2026-08-30)

Per the `verify/` convention (`.gitignore` capture-blob block, `verify/EVIDENCE_MANIFEST.md`)
the `.bmp` / `.png` blobs here are **not committed** — they are regenerable. This file is the
tracked verdict; the `.draw3d.json` telemetry is committed alongside it.

## What this proved

Captures of every frontend screen id `0x00..0x21` except `0x1b` (NULL entry in
`PTR_DAT_005f7638`), taken by **force-pushing** each screen rather than navigating to it:
`FUN_0043d2a0(id, 0)` called from a hook on the menu tick `FUN_0043dfd0`, so it runs on the
game's own thread at a frame boundary. 33 screens, six batched processes, no crash.

**These are FORCED pushes.** They establish how a screen *renders*, not that it is reachable in
normal play.

Two uncertainties were resolved from this pool:

- **U-9050 — screen 5 is "Race Results"**, and contcfg action 7 is **Change Stat**. The screen's
  own footer advertises exactly two actions, `Continue` and `Change Stat`.
  Key blobs: `screen_5.png`, `screen_5_footer.png`.
- **U-9049 — action 6 is PAUSE.** Ruled out of the frontend first via the footer catalogue (only
  three prompt icons exist across all 33 screens: green = action 4, red = action 5, square =
  action 7 — no screen advertises a fourth), then confirmed in-race with a negative control.
  Key blob: `race_fire6_settled.png` (the pause menu, "Transmission Interrupted").

## Screen names, read from the header bar

| id | name | id | name |
|---|---|---|---|
| 0 | Transmission Interrupted (pause) | 17 | NOT USED |
| 1 | Game Type Select | 18 | Game Mode |
| 2 | Single Player | 19 | Sound |
| 3 | Multi Player | 20 | Transmission Interrupted |
| 4 | Player Colour Select | 21 | Options |
| 5 | **Race Results** | 22 | (title screen, empty header) |
| 6 | Challenge Select | 23 | Transmission Interrupted |
| 7 | Challenge Select | 24 | Game Mode |
| 8 | Options | 25 | Mashed |
| 9 | Teams | 26 | Position Screen |
| 10 | NOT USED | 27 | *(NULL table entry, not captured)* |
| 11 | Xbox Live | 28 | Vibration |
| 12 | Lobby | 29 | Controllers |
| 13 | Join Existing Game | 30 | Gamma Correction |
| 14 | Lobby | 31 | Bonus Features |
| 15 | Ability Select | 32 | Autosave |
| 16 | Team Select | 33 | Bonus Features |

`NOT USED` (10, 17) and `Xbox Live` (11) are the developers' own labels, left in the PC build.
Screen 29 "Controllers" is a Standard/Shared mode selector, **not** a key rebinder — the original
game has no rebinding screen.

Note the menu-stack entry type is **not** `CURSCREEN` (`0x0067ecb0`): the title screen is entry
type 22 but reads 33 in `CURSCREEN`. Do not conflate them.

## Regenerating

```
py -3.12 re/frida/force_screen.py --screens 0,1,2,3,4,5 --settle 22 --dwell 2.5
```
Batch in groups of ~6 per process (each push grows the menu stack). Skip id 27.
Contact sheets (`footers_*.png`, `headers_*.png`) are built by cropping y=412..446 and
y=30..62 respectively from each capture and stacking them with the id labelled.

Full analysis: `re/analysis/structs/contcfg_record.md`.
