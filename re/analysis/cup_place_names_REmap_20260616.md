# Cup place-names — RE correction + real strings (WS-G3, 2026-06-16)

Binary anchor: `MASHED.exe` SHA-256 (unpatched) `BDCAE093...`. Read-only via
Ghidra MCP `Mashed_pool9` (2026-06-16). NO-GUESSING.

## The WS-G3 brief's anchors are MIS-CITED

The brief states "Cup track names (`DAT_008a94f0`) are runtime-populated
(`DAT_0067ecbc` membership, `FUN_0040b6c0` name-msgid getter)". The decomp
contradicts this — these are **race-results / standings** globals, per-PLAYER
(4 entries), NOT cup track names:

- `FUN_0040b6c0(i)` = `(&DAT_008a94f0)[i]` — a generic getter (0x0040b6c0).
- `FUN_0040b540(out)` (0x0040b540): reads `(&DAT_008a94e0)[i]` for i=0..3, sets
  `-100` where player slot `DAT_007f1a14[i*4] == -1`, bubble-sorts `out` →
  **ranking order of the 4 players by score**. `FUN_0040b460` (0x0040b460) is the
  twin over `DAT_008a94f0`.
- `FUN_0040b810` (0x0040b810) zeros only `DAT_008a94f0[0..3]` (4 dwords) at
  main-menu entry — a 4-element array, not a 13-track table.
- In the standings screen `FUN_00434720` (0x00434720, "Championship/cup progress
  renderer") the call `FUN_0040b6c0(DAT_0067ecbc[i])` feeds
  `FUN_004a2b60(buf, &DAT_005cd794, value)` — a **`%d` number** draw (the player's
  score), NOT a string-msgid lookup.

⇒ `DAT_008a94f0`/`DAT_008a94e0` = per-player SCORE arrays; `DAT_0067ecbc` = player
finishing-ORDER. Capturing `DAT_008a94f0` via Frida would yield 4 scores, not 13
track names. (Consistent with the other anchor mis-citations the
SESSION_VERIFICATION_AUDIT_2026-06-16 flagged.)

## The REAL place-names (verified, static)

The 13 cup place-names live in **`Font36.piz/USA.DAT`** (the table the frontend
actually resolves through — `g_msg_dat`, NOT the 2.7 KB loose `FONT/USA.dat`
stub, which is empty here). Decoded with the `[u32 off]*` + `[u16 len][UTF-16LE]`
layout (FUN_00427780); decoder validated on `0x40`="Game Type Select",
`0x21`="Single Player", `0x27`="Options":

| msgid | name | msgid | name |
|------:|------|------:|------|
| 0x49 (73) | Angel Peak    | 0x50 (80) | Tierra Piedra |
| 0x4a (74) | Kharga Temple | 0x51 (81) | Koko Bay      |
| 0x4b (75) | Neustein      | 0x52 (82) | Silver Creek  |
| 0x4c (76) | Timgidski     | 0x53 (83) | Redpool Tip   |
| 0x4d (77) | Ventura Blvd  | 0x54 (84) | Nukov         |
| 0x4e (78) | Keister Bay   | 0x55 (85) | Hattan City   |
| 0x4f (79) | Polar Wharf   |           |               |

(The loose `English.dat`/Defines.txt names — timgidski/smokeymountain/superg/
keisterbay/track5..13 — DIVERGE from these and are NOT what ships in USA.)

## The remaining gap — trackID → msgid pairing

The standalone shows AREA names (Arctic/Egypt/…) and a wrong scaffold guess
("Mirage Tunnels" — not a real string). The real per-track name lookup is NOT a
simple static table: searched the image for a consecutive int table
(`49 00 00 00 4a 00 00 00 …`) and a consecutive byte table (`49 4a 4b …`) —
**no hit**. So the trackID→name-msgid map is computed/runtime (or a non-contiguous
descriptor field), and the msgid order (Angel Peak, Kharga Temple, Neustein, …)
does NOT match the `kAreas` order (e.g. "Neustein" is msgid index 2 but kAreas
index 5), so index-pairing would be a guess (forbidden).

**To pin the pairing (no guessing), options:**
1. RE the challenge/track-SELECT draw fn (the one that draws the place-name on
   screen 6/7 — left "[UNCERTAIN — not pinned]" in
   `frontend_config_screens_REmap_20260614.md`); find where it forms the name
   msgid from the track index. ~1–2 Ghidra functions.
2. Parse the per-track descriptor (the .piz / track table) for a name-msgid field.
3. Frida on the booted original — but capture the REAL name table address (TBD),
   NOT `DAT_008a94f0`.

Until the pairing is pinned, wiring real names per track would require guessing
the order — so it stays a documented residual. The verified deliverable here is
the corrected anchors + the 13 real strings.
