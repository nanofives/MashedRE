# Frontend screen map + navigation status (2026-06-13)

Goal: implement all frontend screens + navigate through all of them. The nav
SM (MenuNavSM.cpp) ports all 34 kT* descriptor tables (kT0..kT33); the menu
draw loop renders any reached screen. Full GOTO sweep: verify/allscreens/.

## Navigation
- **Natural chain from the main menu** (after the 0xff1d0000->15 wire):
  1 main -> 2 Single Player / 3 Multi Player -> 4 Player Colour Select ->
  15 Ability Select -> 6 Challenge Select; plus 8 Options -> 19 Sound /
  30 Gamma / 32 Autosave; 1 -> 31 Bonus Features. Reachable set:
  {1,2,3,4,6,8,15,19,30,31,32}.
- **Dev screen-cycle (PageUp/PageDown)**: Nav_DevGoto jumps to prev/next
  screen id so EVERY screen — including gameplay/network/NOT-USED ones that
  cannot be reached naturally in a frontend-only build — is navigable and
  inspectable. (exe_main UpdateMenuSelection.)

## Per-screen status (GOTO sweep)
RENDER full menus/content (frontend, done): 1 main, 2 Single Player, 3 Multi
Player, 4 Colour Select (car livery), 8 Options, 18/24 Game Mode, 19 Sound,
29 Controllers, 30 Gamma, 31/33 Bonus Features, 32 Autosave.

RENDER chrome+title, CONTENT pending (frontend setup; content is drawn by the
campaign/player-state dispatchers FUN_00439210 / FUN_0043af10 / FUN_004368e0
keyed on which challenges are unlocked + which players/controllers are
present — state a frontend-only build lacks; the item is prim_id=-1, a
placeholder for that content):
- 6 / 7  Challenge Select  (grid of campaign challenges; needs campaign data)
- 15     Ability Select    (powerup-ability widgets; needs player state)
- 16     Team Select       (team assignment; needs MP player state)

GAMEPLAY-GATED (render chrome; reachable only via dev-cycle — no in-race
context in a frontend-only build): 0 pause, 5 Race Results, 20/23
Transmission Interrupted, 21 pause-Options, 25 Mashed-splash, 26 Position.

NETWORK-GATED (Xbox Live / lobby; need MP networking): 11 Xbox Live, 12/14
Lobby, 13 Join Game.

NOT USED in the retail build (titles literally "NOT USED"): 9 Teams, 10, 17.

## Remaining for full content
The 3 frontend setup screens (challenge/ability/team select) need their
campaign/player state + per-screen drawers — this belongs with the gameplay
/ campaign milestone (track-select -> race), not the frontend round. All are
navigable now (chrome+title render; dev-cycle + natural chain reach them).
