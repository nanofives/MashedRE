# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project: Mashed RE

Reverse-engineering and reimplementation of **Mashed: Fully Loaded** (Supersonic Software, 2004; published by Strategy First / Empire Interactive).

**Project intent: greenfield source port** (re3 / TD5RE shape). The end state is a standalone `mashed_re.exe` that loads Mashed's data files and runs without `MASHED.exe`. The original is **only** used during development as the diffing reference.

Two build targets share one source tree:
- `mashed_re.exe` — shipping standalone (greenfield).
- `mashed_re_dev.asi` — dev-only hook DLL injected into `MASHED.exe` via Ultimate-ASI-Loader, used for Frida-diff verification of new reimplementations against original behavior. Never shipped.

See `re/INJECTION.md` for why we still need a hook harness during dev (it lets us test reversed code against real game state).

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

## Runtime state (what it takes to boot Mashed on this hardware)

Stock `MASHED.exe` does not boot to main menu on Win11 + modern GPUs. A clean checkout becomes bootable only after applying:

1. **Four on-disk binary patches** to `original/MASHED.exe` (idempotent; each script self-checks). `original/MASHED.exe.unpatched` is the backup that matches the SHA-256 anchor — never delete it.
   - ~~`scripts/patch_mashed_skip_powerups.py`~~ — **DO NOT APPLY.** Root-caused 2026-06-01 as boot crash #2 (the NOP at 0x40295d causes a downstream stack-imbalance ret-to-0). Removing it made baseline boot to the real main menu (verify/boot_powerups_removed.png). The script now refuses to apply and un-applies if found.
   - `scripts/patch_mashed_show_windowed.py` — unhides `[Window]` entries in the video selector.
   - `scripts/patch_mashed_skip_audio_com.py` — neutralizes `FUN_005bc750` (real null-deref fix).
   - `scripts/patch_mashed_skip_selector.py` — silent video selector.
   - `scripts/patch_mashed_skip_controller_dialog.py` — silent controller selector.
   - `scripts/patch_mashed_skip_intro.py` — **NOT a binary patch.** Replaces 5 intro .mpg files in `original/toastart/pc/movies/` with empty 1-frame MPEGs (originals backed up to `movies/backup/`). Lets MASHED run its intro player normally — videos just finish in microseconds. Cuts boot-to-menu from ~10s to <3s. Reversible via `--restore` flag. Approach borrowed from SciLor's MashedRunner (re/prior_art/MashedRunner/Intro.cs).
2. **Canonical `videocfg.bin`** copied from `scripts/canonical/videocfg_windowed.bin` (800×600).
3. **One-time per-machine compat shim** via `scripts/setup_mashed_compat.ps1`.
   Must NOT include `DISABLEDXMAXIMIZEDWINDOWEDMODE` while the d3d9 shim is deployed
   (the AcLayers d3d9 hooks deadlock against our proxy at process init).
4. **d3d9 shim** built and deployed via `mashedmod\build_d3d9_shim.bat`. This proxy
   intercepts `IDirect3D9::CreateDevice` to force `Windowed=TRUE`+640×480 — without
   it, MASHED runs fullscreen, and the resolution-mode switch between launches
   causes monitor flicker. 640×480 matches the lowest fullscreen mode so HUD/
   sprite assets render without stretching. The build script auto-copies
   `SysWOW64\d3d9.dll` to `original\d3d9_real.dll` (one-time; loader-dedup
   workaround).

End state: double-click `MASHED.exe` → main menu in a windowed 800×600 D3D9 backbuffer,
no dialogs, no monitor mode switch, no flicker, no audio, ~5 s boot.

If the SHA-256 of `MASHED.exe` no longer matches the version anchor *and* `MASHED.exe.unpatched` exists, the patches are already applied — that is expected; do not "fix" it by reverting. The anchor is preserved on `.unpatched`.

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
├── scripts\                       # ghidra_pool.{sh,ps1}, patch_mashed_*.py (5 patches),
│   └── canonical\                 # known-good videocfg.bin variants
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
- **MSVC Build Tools 2022 (x86)** — installed at `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`. Activated via `VC\Auxiliary\Build\vcvars32.bat`. `cl.exe 19.44+` produces both targets via `mashedmod\build.bat`. Choice locked 2026-05-08: chosen over MinGW for ABI compatibility with the original MSVC-built MASHED.exe (cleaner `__thiscall`, vtable layout, exception handling for hook surfaces).
- **Frida** for runtime tracing / behavioral diffs. Already installed via the user's Python.

## Cross-build reference (Xbox/PS2 twins)

The Xbox build (`toast.exe`) is a second x86 compile of the **same source** as `MASHED.exe`. A static cross-build matcher pairs functions between them so a hairy PC decompilation can be read from its Xbox twin (different optimizer context, often cleaner). See `re/analysis/console_symbol_harvest/REPORT.md`.

- **Lookup**: `py -3.12 re\tools\console\xtwin.py 0x<pc_rva>` prints the Xbox twin's decompilation (~30–60 s; single-writer `Mashed_Console` headless project — don't run two at once).
- **Table**: `re/console/match/xbuild_match_v2.csv` (PC RVA → Xbox VA, `tier`, `scaffold` ∈ {ok-asc, flag, ambiguous}). Trust `scaffold=ok-asc`; treat `prop-weak`/`caller`/`interval` tiers as candidate-grade and confirm before relying on a specific pair.
- **Evidence status**: a twin is a *reading aid and a second static witness only*. It is **NOT** behavioral evidence — it never substitutes for a `diff-original` Frida diff and cannot move a function's C-level. Both consoles are symbol-stripped, so there are no free names to import.
- `re/console/match/platform_candidates.csv` lists PC functions with no Xbox twin; only the `signal=platform-api` rows (DirectShow/D3DX/D3D9) are real PC-only glue — the `recall-gap` rows are matcher misses, not findings.

## Common commands

All commands run from the repo root (`C:\Users\maria\Desktop\Proyectos\Mashed`).

```powershell
# Build both targets (mashed_re.exe + mashed_re_dev.asi). Internally calls vcvars32.bat.
mashedmod\build.bat

# Apply the four binary patches to original\MASHED.exe (idempotent; skip if .unpatched
# is missing because that means a fresh original/ has not been backed up yet).
# NOTE: patch_mashed_skip_powerups.py is RETIRED (causes boot crash #2; refuses to apply).
py -3.12 scripts\patch_mashed_show_windowed.py
py -3.12 scripts\patch_mashed_skip_audio_com.py
py -3.12 scripts\patch_mashed_skip_selector.py
py -3.12 scripts\patch_mashed_skip_controller_dialog.py
py -3.12 scripts\patch_mashed_skip_intro.py            # replaces intro .mpg files; --restore to undo

# Acquire / release a Ghidra pool slot (use the skill rather than calling this directly
# unless you know what you're doing — the skill handles lock hygiene).
pwsh scripts\ghidra_pool.ps1 acquire
pwsh scripts\ghidra_pool.ps1 release <slot>

# Frida — main-menu invocation counter (no Interceptor; behavioral observation only).
py -3.12 re\frida\auto_count_at_menu.py

# Frida — A/B path1 diff for one hook (driven by hooks_registry.py).
py -3.12 re\frida\run_diff.py <hook_name>

# Frida — path2 installer verification for one hook.
py -3.12 re\frida\run_verify_hook.py <hook_name>

# Frida — exception-handler SIGSEGV catcher (use this for any new crash work, NOT
# Interceptor traces — see "Frida overhead on hot paths" below).
py -3.12 re\frida\poll_attach_catch_crash.py
```

### Frida hook verification flow

`re\frida\hooks_registry.py` is the single source of truth for hook test vectors. To verify a new hook:
1. Add an entry to `HOOKS` in `hooks_registry.py` (RVA, export name, signature, arg type, test inputs).
2. Run `run_diff.py <hook_name>` for bit-identity A/B vs the original (path1).
3. Run `run_verify_hook.py <hook_name>` to confirm the inline-JMP redirect actually installed (path2).

Do not add new one-off `.js` or `.py` harnesses for verification — extend the registry instead.

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

For standalone **frontend/visual** work the acceptance analogue is the **parity harness** (`re/analysis/parity_tooling.md`): a draw-list diff (`re/tools/drawlist_diff.py`) between an original-side capture (`re/frida/menu_draw_burst.py`) and the standalone's `MASHED_DBG_DRAWSTREAM` dump — GREEN, or RED with every remaining row cited in the tracker. `re/tools/nav_coverage.py` checks new screens; `re/tools/imgdiff.py` covers what draw lists can't (texture decode, font raster). Screenshots alone are not acceptance for composition fixes.

### Frida overhead on hot paths

Frida `Interceptor.attach` on hot paths (>1000 calls/s) destabilizes Mashed in ~6 seconds — the process drifts and either crashes or hangs. Empirically observed at the main menu where `FastSqrt` fires ~2,700/s and `FastInvSqrt` ~900/s (see `log/auto_count_at_menu.txt`).

Rules:
- For hot-path verification, use **behavioral observation**: `Module.load` the .asi, watch the game for N seconds with **no** `Interceptor`, confirm no visual/crash regression.
- If you must trace a hot function, **sample one function at a time** and keep the run short.
- For crash investigation, use `re/frida/poll_attach_catch_crash.py` (an exception-handler-based EIP catcher), not `Interceptor` traces.

### Confidence promotion (NO overclaiming)

Synthetic Frida A/B with the hook bypassed (env-var disabled) is **C3** evidence at best — it proves the implementation is bit-identical, not that the patcher works. **C4 demands a canonical-scenario run with the hook actually installed.** This rule has been violated before; refuse C4 promotions that lack canonical-scenario evidence with the inline-JMP live.

## Operating principles

**Stop and ask.** When any of the following holds, **halt and ask the user** rather than infer or pick:
- Two interpretations of a request both have material consequences.
- A confidence promotion is missing required evidence (no Frida diff, no analysis note, etc.) — refuse, don't paper over.
- An action is destructive or hard-to-reverse: `rm`, `git push --force`, master Ghidra writes while another session may be active, branch deletion, dependency removal.
- A tracker conflict has substantive disagreement (not just an ID renumber).
- A subsystem boundary is ambiguous when classifying a function.
- A new MCP / dependency / build tool is needed but the user hasn't approved it.
- Architecture-level decisions (greenfield vs hook, MSVC vs MinGW, renderer choice).

**Cite, don't paraphrase.** Every claim about original behavior cites an RVA. Every constant cites its address. NO-GUESSING — `re/CONFIDENCE.md`.

**Trackers are load-bearing.** Mutate `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md` only via `re-classify`. Hand-edits drift.

**Skills are entry points.** Use the skill catalog (below) by name; if no skill fits, ask before inventing a new one.

## Roadmap, DoD, and trackers

**Current phase (ROADMAP v2, 2026-06-09):** the project pivoted to **standalone-first, demand-driven** development — see `ROADMAP.md` v2 (phases R0–R8) and `re/analysis/AUDIT_2026-06-09.md`. Phase R0 (repair & re-baseline) executed 2026-06-09; active work is R1 (C4 re-validation lane, tracker `re/analysis/C4_REVALIDATION.md` — 101 suspect C4 rows incl. the Vec3 trio await installed-hook canonical re-validation) and R2 (menu completion in the standalone). The v1 "Phase 4/5" framing and per-subsystem percentage gates are retired; batch fanout pipelines are demoted to opportunistic use (first-party C1 = 0; C2→C3 batch yield collapsed to 8/48 — do not run a batch with predicted yield under ~30%).

- `ROADMAP.md` — phases (0..6), Definition of Done at function/subsystem/project levels.
- `re\CONFIDENCE.md` — C0..C4 rubric; the only gate for status changes.
- `hooks.csv` — every reverse-engineered function. One row per RVA. Single source of truth for project status.
- `STUBS.md` — placeholder calls into not-yet-reversed functions. Blocks subsystem DoD until cleared.
- `UNCERTAINTIES.md` — explicit knowledge holes with paths to resolution. Required for any `[UNCERTAIN]` marker in source/notes.
- `DEFERRED.md` — work explicitly out of scope for now. Each row has a re-pickup condition.

**Rule:** the four trackers are mutated only through the `re-classify` skill. Hand-edits drift.

## Skills (project-local)

Loaded automatically from `.claude\skills\`:

- **ghidra-pool** — on-demand Ghidra project clones; entry point for any disassembly/decomp work.
- **piz** — extract/inspect `.piz` archives via `re\tools\piz_extract.py`.
- **hook-author** — scaffold a new gta-reversed-style hook with proper RVA citations + binary anchor verification.
- **diff-original** — Frida-based behavioral diff between original and modded binary.
- **re-classify** — promote/demote a function on the C0..C4 ladder; updates `hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `re/analysis/CHANGELOG.md` in one transaction. Refuses promotions that lack evidence.
- **worktree** — git worktrees for isolated per-effort RE work; binds each worktree to a Ghidra pool slot.
- **multi-session** — etiquette and conflict resolution when multiple Claude sessions run concurrently.
- **discover-c1-batch** — generates a Ghidra-side parallel-fanout batch file (`batch_<letter>.txt`) of Claude Code prompts for C0→C1 discovery, C1→C2 mechanical promotion, DEFERRED drains, and struct-extraction sessions (default 6 sessions × 20 RVAs = ~120 RVAs per batch). Pairs with `ghidra-sweep`. Trigger: "make a ghidra batch" / "plan a c1 fanout" / "drain DEFERRED into a batch".
- **ghidra-sweep** — drain all queued rows in `re/SCRIBE_QUEUE.md` into the master Ghidra project (the follow-up Opus session after a `discover-c1-batch` fanout). Trigger: "run the ghidra sweep" / "drain the scribe queue".
- **promote-c3-batch** — generates a Frida-side parallel-fanout batch file (`c3_batch_<letter>.txt`) of Claude Code prompts for C2→C3 promotion sessions (default 6 sessions × 5 candidates = 30 promotions). Pairs with `frida-sweep`. Trigger: "make a c3 batch" / "plan a c3 fanout".
- **frida-sweep** — drain all queued rows in `re/PROMOTION_QUEUE.md` (the follow-up Sonnet session after a `promote-c3-batch` fanout). Merges N worktree branches, resolves the three predictable text-file conflicts, rebuilds the canonical `.asi`, and runs the integration Frida diff against every promoted hook. Trigger: "run the frida sweep" / "merge the c3 batch" / "drain the promotion queue".

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
