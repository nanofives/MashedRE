# Subsystem Map

The Phase 1 deliverable. Identifies the major code areas in `MASHED.exe` and their entry points. Filled in as Phase 1 progresses — empty until then.

## Format

For each subsystem: a short prose description of its role, the RVAs of its top entry points (≤ 5), and the strings/imports that fingerprint it.

## Subsystems

### boot
- **Role:** WinMain, command-line parsing, language selection, video config load, RenderWare engine init.
- **Entry points:** TBD (Phase 1)
- **Fingerprints:** strings `"RwEngineInit"`, `"videocfg.bin"`, language code constants from MashedRunner

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
- **Role:** Track loading, geometry, checkpoints
- **Entry points:** TBD
- **Fingerprints:** strings referencing track names (Arctic, City, Egypt…)

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
