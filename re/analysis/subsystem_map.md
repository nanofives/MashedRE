# Subsystem Map

The Phase 1 deliverable. Identifies the major code areas in `MASHED.exe` and their entry points. Filled in as Phase 1 progresses — empty until then.

## Format

For each subsystem: a short prose description of its role, the RVAs of its top entry points (≤ 5), and the strings/imports that fingerprint it.

## Subsystems

### boot
- **Role:** WinMain, command-line parsing, language selection, video config load, RenderWare engine init.
- **Entry points:**
  - `entry` @ `0x004a4bb7` (PE OS entry — MSVC CRT startup; calls into WinMain)
  - WinMain RVA: TBD (downstream of `entry`)
- **Fingerprints:** strings `"RwEngineInit"`, `"videocfg.bin"`, language code constants from MashedRunner; MSVC EH runtime at `0x004a3d7e` / `0x004a8f33`.

### audio
- **Role:** RenderWare audio (RWS) loading, sound playback, music
- **Entry points:** TBD
- **Fingerprints:** RW audio API symbols, file paths to `toastaudio/`

### ai
- **Role:** Driver AI for opponents
- **Entry points:** TBD
- **Fingerprints:** strings/paths referencing `AI*.AI` files inside `AI.piz`

### vehicle
- **Role:** Vehicle loading, physics, control
- **Entry points:** TBD
- **Fingerprints:** vehicle model paths, physics constants

### track
- **Role:** Track loading, geometry, checkpoints, per-player track assignment, Lua scripted course events
- **Entry points:**
  - `FUN_0040d440` @ `0x0040d440` — Course::LoadCurrent (no args; loads `DAT_0063ba7c`-indexed track; sole public entry)
  - `FUN_0040d270` @ `0x0040d270` — Course::Load (table, idx; full orchestrator; called only from FUN_0040d440)
  - `FUN_00426340` @ `0x00426340` — KTCScript.lua loader (registers COURSE.LUA Lua command block at LAB_00440bc0)
  - `FUN_004264d0` @ `0x004264d0` — Track powerups loader (per-track powerups_gold.lua or powerups.lua)
  - `FUN_00420420` @ `0x00420420` — Per-player vehicle+checkpoint init (stride 0x2AC; BSP sectors)
- **Fingerprints:** strings `"toastart/tracks/"` (0x005cd4c0), `"LAPDATA.LUA"` (0x005cd578), `"COURSE.LUA"` (0x005cd584), `"TOTALNUMTRACKS-3=%d\n"` (0x005cd8b0), `"course LoadTime is %u (%.3f)\n"` (0x005ccd48), `"KTCScript.lua"` (embedded in FUN_00426340)

### render
- **Role:** RenderWare → D3D9 rendering pipeline
- **Entry points:** TBD
- **Fingerprints:** RW render API, D3D9 entry points

### hud
- **Role:** In-race HUD, position indicators, lap counter
- **Entry points:** TBD
- **Fingerprints:** strings for digit textures, lap counter constants

### frontend
- **Role:** Main menu, vehicle/track select, options
- **Entry points:** TBD
- **Fingerprints:** strings for menu items, `Frontend.piz` references

### input
- **Role:** Keyboard/joypad/gamepad reading; Lua remap
- **Entry points:** TBD
- **Fingerprints:** strings `"remap.lua"`, joypad button code constants from `remap.lua`

### save
- **Role:** Game save read/write; settings persistence
- **Entry points:** TBD
- **Fingerprints:** `"gamesave.bin"`, `0xDEADBEEF` constant, player profile serialization

### net
- **Role:** Multiplayer (if present in this build) — TBD whether this exists
- **Entry points:** TBD
- **Fingerprints:** WSAStartup, socket calls, multiplayer menu strings

### util
- **Role:** Math/string helpers, container types, RW SDK helpers
- **Entry points:** TBD
- **Fingerprints:** small leaf functions called from many subsystems

### unknown
- **Role:** Anything not yet categorized
- **Entry points:** N/A
