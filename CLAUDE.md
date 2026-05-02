# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project: Mashed RE

Reverse-engineering and reimplementation of **Mashed: Fully Loaded** (Supersonic Software, 2004; published by Strategy First / Empire Interactive). The goal is a gta-reversed-style replacement DLL that reimplements the game's logic function-by-function, while keeping the original `MASHED.exe` as the bootstrap and asset host.

## Engine, anchors, and the "two Ghidra projects" arrangement

- **Engine: RenderWare 3.x** (Criterion). Confirmed via `mashed.log` (`RwEngineInit`, `RwEngineOpen`, `RtFSManagerOpen`). The folder name `TOASTART` / `toastaudio` is asset/middleware naming, **not the engine name**. RenderWare structs/headers from `gta-reversed-modern/source/RenderWare/` apply directly.
- **Other tech**: native PE32 i386 (MSVC-era), DirectX 9.0c, MCI for video, Lua 5.x for joypad remap, MSVBVM60 for the `launch.exe` copy-protection wrapper, custom `.piz` asset format (XeNTaX-documented), RenderWare RWS audio, RenderWare TXD textures.
- **Version anchor — verify before any hook authoring**:
  ```
  original\MASHED.exe   size 2,846,720   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
  original\launch.exe   size   978,944   SHA-256 694AA949B86AE26E5F1496A48383D1027244752A468BE7F678C54770B973EF79
  ```
  If a binary differs, RVAs are invalid; abort and re-anchor before continuing.
- **Two Ghidra projects** (sharing one Ghidra 12.0.3 install, located in TD5RE):
  - `Mashed.gpr` / `Mashed.rep` — GUI-edited master; canonical symbols, types, comments.
  - `Mashed_headless.gpr` / `Mashed_headless.rep` — optional dedicated headless target for the MCP server.
  Pool slots `mashed_pool/Mashed_poolN` are read-only clones, created on demand by the `ghidra-pool` skill.

## Layout

```
Mashed\
├── original\                      # Untouched install — never edit; never delete
│   ├── MASHED.exe                 # Main game (target)
│   ├── launch.exe                 # VB6 launcher with copy protection (anchor #2)
│   ├── TOASTART\                  # Game assets (.piz archives, fonts, panels, powerups)
│   ├── toastaudio\                # RenderWare audio (.rws) per language
│   ├── gamesave.bin, videocfg.bin
│   └── ...
├── mashedmod\                     # Our work
│   ├── src\mashed_re\             # gta-reversed-style C++ replacement DLL
│   └── deps\                      # Toolchain (MinGW), vendored RW headers
├── re\                            # Research artifacts
│   ├── tools\                     # Python: piz_extract.py, rws/txd dumpers, save editors
│   ├── frida\                     # Frida JS hooks for diff-original verification
│   ├── analysis\                  # Per-subsystem RE notes (audio, vehicle, AI, render…)
│   └── prior_art\                 # SciLor's three repos cloned read-only
│       ├── MashedRunner\          # CLI params, language codes, NoCD search-and-replace
│       ├── MashedFileExtractor\   # .piz / .rws parsers (authoritative)
│       └── MashedTrainer\         # Only public memory map (Fully Loaded only)
├── mashed_pool\                   # Ghidra clones (created on demand by the skill)
├── ghidra-headless-mcp\           # NOT here — shared from TD5RE\ghidra-headless-mcp\
├── scripts\                       # ghidra_pool.sh + .ps1 wrapper
├── .claude\                       # Settings, skills, slash commands
├── .mcp.json                      # Wires the Ghidra MCP from the TD5RE shared install
├── log\                           # Build logs, Frida traces, diff CSVs
└── verify\                        # Screenshot/snapshot verification
```

Do not move or delete anything in `original\` without explicit user permission.

## Toolchain

- **Ghidra 12.0.3** — shared install at `C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC` (do not duplicate).
- **Ghidra MCP** — `mrphrazer/ghidra-headless-mcp`, shared install at `C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra-headless-mcp\`. Wired via `.mcp.json`.
- **Python 3.12** invoked as `py -3.12` for tooling and the MCP server.
- **MinGW** (32-bit i686) — to be vendored under `mashedmod\deps\mingw\` (mirroring TD5RE) when DLL build starts. Not yet installed.
- **Frida** for runtime tracing / behavioral diffs. Already installed via the user's Python.

## Workflow conventions

### NO-GUESSING rule (deterministic RE)

Every Ghidra session and every reversed function must follow this:

- Report only what the decompilation literally shows. Never infer, speculate, or "fill in gaps."
- For every constant, offset, or formula reported: cite the exact Ghidra address where it appears.
- If a value's meaning is unclear: report it as a raw hex/decimal constant. Do not assign semantic meaning.
- If a function's purpose is unclear: describe it mechanically (reads offset X, calls Y, writes Z). Do not guess intent or invent a name.
- Never use words like "probably", "likely", "seems to", "appears to", "I think", "presumably."
- If something is uncertain: mark `[UNCERTAIN]` and state exactly what evidence is missing.
- Sign-sensitive values (coordinates, offsets, velocities): report BOTH raw hex AND signed decimal.

### Hook authoring (gta-reversed-style)

- One file per original class under `mashedmod\src\mashed_re\<Subsystem>\<Class>.cpp`.
- Inline comment every RVA: `// 0x00xxxxxx` above the function.
- Naming: `m_` instance, `ms_` static member, `g_` global, `s_` file-static, `PascalCase` types, `CamelCase` methods.
- Register hooks via `RH_ScopedInstall(Method, 0xRVA)` in `InjectHooks()`.
- Hooks must be **runtime-toggleable** so they can be A/B-tested with the `diff-original` skill.
- Track progress in `hooks.csv`: `RVA, ClassName::Method, status, file`.

### Verification

A hook is accepted only when the `diff-original` skill produces a clean Frida diff between original and modded behavior. Compile-passes-and-doesn't-crash is not acceptance.

## Skills (project-local)

Loaded automatically from `.claude\skills\`:

- **ghidra-pool** — manages on-demand Ghidra project clones for parallel sessions; entry point for any disassembly/decomp work.
- **piz** — extract/inspect `.piz` archives via `re\tools\piz_extract.py`.
- **hook-author** — scaffold a new gta-reversed-style hook with proper RVA citations.
- **diff-original** — Frida-based behavioral diff between original and modded binary.

## Prior art (read-only, vendored under `re\prior_art\`)

- **SciLor/MashedRunner** — definitive source for `launch.exe` CLI params, language codes, registry keys, NoCD search-and-replace patterns.
- **SciLor/MashedFileExtractor** — `FileFormats\PIZFile.cs` is the authoritative `.piz` parser; cross-check our Python against it when in doubt.
- **SciLor/MashedTrainer** — Mashed Fully Loaded only. Only public memory map / pointer chains. The 5% asm is inline patches.
- All three are unlicensed (no LICENSE file). **Borrow knowledge and re-implement; do not redistribute their source.**

## What this project deliberately does NOT do

- No Conan/CMake 4.x ceremony — plain MSVC or MinGW + a single build script.
- No ImGui dev-menu until enough hooks land to need it.
- No CI/automation until a second contributor appears.
- No `reversiblebugfixes`-style framework — fork-fix as needed.
- No backwards-compatibility shims when changing internal APIs; this is a solo greenfield codebase.
