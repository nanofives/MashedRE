# `MASHED_*` flag inventory — D0.2 (2026-08-15)

Generated, not hand-listed. Regenerate with the script in the D0.2 changelog entry.

```
distinct MASHED_* tokens      150
live env vars (real accessor) 138
non-env tokens                8
DEAD flag names (comment-only) 4
```

Accessors counted: `getenv`, `GetEnvironmentVariableA`, `EnvSet`, `envSet`. A regex
limited to the first two undercounts by 9 and produced the wrong "128" quoted in an
earlier draft of ROADMAP v3 — `envSet("MASHED_D3D_REF")` at `exe_main.cpp:2140` is a
real read.

## Class A — MIGRATION (selects a port over a scaffold). THE v3 DEBT.

| Flag | Default | Meaning |
|---|---|---|
| `MASHED_RENDER_LIBRW` | **OFF** | librw vs hand-written D3D9 world renderer — port is OFF by default |
| `MASHED_REAL_PHYSICS` | **OFF** | ported RWP-3.7 chain vs kinematic scaffold — port is OFF by default |
| `MASHED_RW_RENDER` | **OFF** | inert even when set (RwWorldRender.cpp:230) |

## Class B — ALREADY INVERTED (the model to copy)

| Flag | Meaning |
|---|---|
| `MASHED_RULE_ENGINE` | port is DEFAULT; flag turns it off for A/B |
| `MASHED_LIBRW_INST` | port is DEFAULT; flag turns it off for A/B |
| `MASHED_GATE_RIBBON_AI` | port is DEFAULT; flag turns it off for A/B |

## Class C — DEAD flag names (referenced in comments, NO accessor anywhere)

A comment naming a flag that does not exist is a false map. Delete the comment or
implement the flag.

- `MASHED_REAL_AI`
- `MASHED_CHASE_FOG`
- `MASHED_AI_LEADER_SELFTEST`
- `MASHED_INTERP_REGISTRY`

## Class D — non-env tokens the raw prefix-grep picks up

- `MASHED_RE_COLLISION_BUILD_DEPS_H` — include guard
- `MASHED_RE_FRONTEND_MENUNAVSM_H` — include guard
- `MASHED_DAT_005` — absolute-address macro
- `MASHED_DAT_007` — absolute-address macro
- `MASHED_PTR_00624058` — absolute-address macro
- `MASHED_PHYS_DIAG` — compile-time #define
- `MASHED_PATCHES` — filename (deps/librw/MASHED_PATCHES.md)
- `MASHED_QOL` — prose prefix

## Class E — verification / debug flags, by target


### `mashed_re` (107)

- `MASHED_A6_DIAG` — mashed_re/Vehicle/Integrate2.cpp
- `MASHED_AI_AB` — mashed_re/Ai/AiControllerAB.cpp
- `MASHED_AI_DIAG` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_AI_DRIVES_PLAYER` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_AI_LOS_SELFTEST` — mashed_re/Ai/AiLineOfSight.cpp
- `MASHED_AI_NAV` — mashed_re/Ai/AiStandalone.cpp
- `MASHED_AI_PUREPURSUIT` — mashed_re/Ai/AiStandalone.cpp
- `MASHED_AI_SPLINE_SELFTEST` — mashed_re/Ai/AiNavHooks.cpp
- `MASHED_AI_SPREAD` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_AI_STEERFLIP` — mashed_re/Ai/AiStandalone.cpp
- `MASHED_AI_WALLAHEAD_SELFTEST` — mashed_re/Ai/AiWallAhead.cpp
- `MASHED_AI_WALLLAT_SELFTEST` — mashed_re/Ai/AiWallLateral.cpp
- `MASHED_ALIGNRATE` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_BQ_AB` — mashed_re/Render/GlobalByteQuadAB.cpp
- `MASHED_CAM_POSE` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_CAR` — mashed_re/exe_main.cpp
- `MASHED_CAR_SEL` — mashed_re/exe_main.cpp
- `MASHED_CFGEDIT_DEMO` — mashed_re/exe_main.cpp
- `MASHED_CHAINSCALE` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_CONFIG_EDIT` — mashed_re/exe_main.cpp
- `MASHED_CONTACT_DIAG` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_COUPLING_DIAG` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_CUP` — mashed_re/exe_main.cpp
- `MASHED_CUP_STANDINGS` — mashed_re/exe_main.cpp
- `MASHED_CUP_TIERS` — mashed_re/exe_main.cpp
- `MASHED_D3D_NOVSYNC` — mashed_re/exe_main.cpp
- `MASHED_D3D_REF` — mashed_re/exe_main.cpp
- `MASHED_D3D_TIMEOUT_MS` — mashed_re/exe_main.cpp
- `MASHED_DBG_BBDUMP` — mashed_re/exe_main.cpp
- `MASHED_DBG_BBDUMP_OUT` — mashed_re/exe_main.cpp
- `MASHED_DBG_BBDUMP_REQ` — mashed_re/exe_main.cpp
- `MASHED_DBG_CONFIGEDIT` — mashed_re/exe_main.cpp
- `MASHED_DBG_DRAWLOG` — mashed_re/D3d9Render/RwIm2DBridge.cpp
- `MASHED_DBG_DRAWSTREAM` — mashed_re/D3d9Render/DrawStreamDump.cpp
- `MASHED_DBG_DRAWSTREAM3D` — mashed_re/D3d9Render/DrawStreamDump.cpp
- `MASHED_DBG_DRAWSTREAM3D_OUT` — mashed_re/D3d9Render/DrawStreamDump.cpp
- `MASHED_DBG_DRAWSTREAM_OUT` — mashed_re/D3d9Render/DrawStreamDump.cpp
- `MASHED_DBG_DUMPBADGE` — mashed_re/exe_main.cpp
- `MASHED_DBG_MENU` — mashed_re/exe_main.cpp
- `MASHED_DBG_MODAL` — mashed_re/exe_main.cpp
- `MASHED_DBG_NO_ARC` — mashed_re/exe_main.cpp
- `MASHED_DBG_NO_PREVIEWS` — mashed_re/exe_main.cpp
- `MASHED_DBG_TEXMATCH` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_DEMO_DRIVE` — mashed_re/exe_main.cpp
- `MASHED_DETERMINISTIC` — mashed_re/exe_main.cpp
- `MASHED_DET_FRAMES` — mashed_re/exe_main.cpp
- `MASHED_DRIVE_DEMO` — mashed_re/exe_main.cpp
- `MASHED_DRIVE_HOLD` — mashed_re/exe_main.cpp
- `MASHED_FONT_GAMMA` — mashed_re/D3d9Render/MashedFont.cpp
- `MASHED_FORCE_SWVP` — mashed_re/exe_main.cpp
- `MASHED_FRAME_PROF` — mashed_re/exe_main.cpp
- `MASHED_FX_DEBUG` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_GAME_LENGTH` — mashed_re/exe_main.cpp
- `MASHED_GAME_MODE` — mashed_re/exe_main.cpp
- `MASHED_GOTO` — mashed_re/exe_main.cpp
- `MASHED_HOOK_HI` — mashed_re/Core/HookSystem.cpp
- `MASHED_HOOK_LO` — mashed_re/Core/HookSystem.cpp
- `MASHED_HOOK_MANIFEST` — mashed_re/Core/HookSystem.cpp
- `MASHED_HOOK_ONLY` — mashed_re/Ai/AiControllerAB.cpp
- `MASHED_HOOK_SKIP` — mashed_re/Core/HookSystem.cpp
- `MASHED_LAPS` — mashed_re/exe_main.cpp
- `MASHED_LAP_DIAG` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_LIBRW_FOGFIX` — mashed_re/LibRw/RwRaceSubmit.cpp
- `MASHED_LIBRW_LIFT` — mashed_re/LibRw/RwRaceSubmit.cpp
- `MASHED_LIBRW_LINEAR` — mashed_re/LibRw/RwRasterBridge.cpp
- `MASHED_LIBRW_NODRAW` — mashed_re/LibRw/RwRaceSubmit.cpp
- `MASHED_LIBRW_ONLYPROP` — mashed_re/LibRw/RwRaceSubmit.cpp
- `MASHED_LIBRW_SMOKE` — mashed_re/LibRw/RwBridge.cpp
- `MASHED_LIBRW_TEXLOG` — mashed_re/LibRw/RwRasterBridge.cpp
- `MASHED_LINK_AB` — mashed_re/Render/RwPluginLinkSetAB.cpp
- `MASHED_MP_RULES` — mashed_re/exe_main.cpp
- `MASHED_MUTE` — mashed_re/Audio/AudioEngine.cpp
- `MASHED_NAV_DEMO` — mashed_re/exe_main.cpp
- `MASHED_NO_COPTERS` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_NO_FOG` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_NO_PARTICLES` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_NO_UVSCROLL` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_NO_WEATHER` — mashed_re/exe_main.cpp
- `MASHED_PARITY` — mashed_re/exe_main.cpp
- `MASHED_PHYS_C4_SELFTEST` — mashed_re/Collision/RwpIntegrator.cpp
- `MASHED_PHYS_NOCONTACT` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_PHYS_PROF` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_PLAYTHROUGH` — mashed_re/exe_main.cpp
- `MASHED_PLAY_DEMO` — mashed_re/exe_main.cpp
- `MASHED_PROP_VDUMP` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_QHULL_OPT` — mashed_re/Collision/b5b_qhull_selftest.cpp
- `MASHED_QHULL_PC` — mashed_re/Collision/b5b_qhull_selftest.cpp
- `MASHED_RACE_DEMO` — mashed_re/exe_main.cpp
- `MASHED_RACE_MODE` — mashed_re/exe_main.cpp
- `MASHED_RENDER_PROF` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_RESULT_DEMO` — mashed_re/exe_main.cpp
- `MASHED_ROOT` — mashed_re/exe_main.cpp
- `MASHED_ROUND` — mashed_re/exe_main.cpp
- `MASHED_RPLIGHT` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_SIM_HZ` — mashed_re/exe_main.cpp
- `MASHED_THROTTLE_K` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_TOPSPEED` — mashed_re/Vehicle/VehiclePhysicsRun.cpp
- `MASHED_TRACK_SEL` — mashed_re/exe_main.cpp
- `MASHED_TRACK_VIEW` — mashed_re/exe_main.cpp
- `MASHED_VERIFY_OUT` — mashed_re/exe_main.cpp
- `MASHED_WORLDVEL` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_FLATSNOW` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_MATID` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_ONLYMAT` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_PRELITONLY` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_REVORDER` — mashed_re/D3d9Render/TrackRenderer.cpp
- `MASHED_WORLD_VDUMP` — mashed_re/D3d9Render/TrackRenderer.cpp

### `d3d9_shim` (12)

- `MASHED_FPS_CAP` — d3d9_shim/d3d9_shim.cpp
- `MASHED_FPS_CAP_RACE` — d3d9_shim/d3d9_shim.cpp
- `MASHED_FPS_LOG` — d3d9_shim/d3d9_shim.cpp
- `MASHED_FPS_OSD` — d3d9_shim/d3d9_shim.cpp
- `MASHED_HIRES` — d3d9_shim/d3d9_shim.cpp
- `MASHED_ORIG_BBDUMP` — d3d9_shim/d3d9_shim.cpp
- `MASHED_ORIG_BBDUMP_REQ` — d3d9_shim/d3d9_shim.cpp
- `MASHED_PARITY_BG` — d3d9_shim/d3d9_shim.cpp
- `MASHED_QOL_BORDERLESS` — d3d9_shim/d3d9_shim.cpp
- `MASHED_RES` — d3d9_shim/d3d9_shim.cpp
- `MASHED_RE_BORDERLESS` — d3d9_shim/d3d9_shim.cpp
- `MASHED_RE_NO_SCREEN1_PIN` — d3d9_shim/d3d9_shim.cpp

### `qol_asi` (11)

- `MASHED_DECOUPLE` — qol_asi/mashed_qol.cpp
- `MASHED_HIDE_SHADOW` — qol_asi/mashed_qol.cpp
- `MASHED_INTERP` — qol_asi/mashed_qol.cpp
- `MASHED_INTERP_OVERLAYS` — qol_asi/mashed_qol.cpp
- `MASHED_INTERP_SHADOW` — qol_asi/mashed_qol.cpp
- `MASHED_INTERP_SUBMIT` — qol_asi/mashed_qol.cpp
- `MASHED_INTERP_WHEELS` — qol_asi/mashed_qol.cpp
- `MASHED_JUMPFIX` — qol_asi/mashed_qol.cpp
- `MASHED_NO_SAVE` — qol_asi/mashed_qol.cpp
- `MASHED_QOL_LOG` — qol_asi/mashed_qol.cpp
- `MASHED_UNLOCK` — qol_asi/mashed_qol.cpp

### `dinput8_shim` (2)

- `MASHED_QOL_DISABLE` — dinput8_shim/dinput8_shim.cpp
- `MASHED_RE_NO_AUTO_HOOK` — dinput8_shim/dinput8_shim.cpp
