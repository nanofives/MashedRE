# AGENTS.md

Operating manual for any agent (Claude Code, sub-agents, future contributors) working in this repo. Pairs with `CLAUDE.md`.

## First-run checklist (do once, in order)

1. **Verify install integrity**
   ```powershell
   Get-FileHash original\MASHED.exe -Algorithm SHA256   # expect BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
   Get-FileHash original\launch.exe -Algorithm SHA256   # expect 694AA949B86AE26E5F1496A48383D1027244752A468BE7F678C54770B973EF79
   ```
   Mismatch → stop. Note the actual hash and ask the user which build they have.

2. **Bootstrap the Ghidra master project (manual; do NOT auto-run)**
   - Open Ghidra GUI from `C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC\ghidraRun.bat`.
   - File → New Project → Non-Shared Project → location `C:\Users\maria\Desktop\Proyectos\Mashed`, name `Mashed`.
   - Drag `original\MASHED.exe` into the project.
   - Right-click → Auto-analyze (default settings, plus *Decompiler Parameter ID* and *Aggressive Instruction Finder*).
   - Wait 10–30 minutes. Save. Exit.
   - (Optional) Repeat with project name `Mashed_headless` if you want a separate headless write target.

3. **Verify the MCP wiring**
   ```powershell
   bash scripts/ghidra_pool.sh status
   ```
   Should print `Master Mashed.gpr: available` and "no slots created yet".

4. **First decomp** — invoke the `ghidra-pool` skill and ask for a small function (e.g., `entry`).

## Build flow (once `mashedmod\src\mashed_re\` has code)

To be defined when the first hook lands. Will mirror TD5RE's `build_standalone.bat`-style script under `scripts\`.

## Diff workflow (once Frida script lands)

1. Run `original\MASHED.exe` clean → trace via `re\frida\diff_<sym>.js` → `log\diff_original_<RVA>.csv`.
2. Run with mod DLL injected → same script → `log\diff_modded_<RVA>.csv`.
3. Diff. First divergent row = bug location. Re-read decomp, fix, repeat.

## Pool etiquette

- Always `read_only=true` on pool slots. Writes go to the master only, in a single supervised session.
- Always close + release a slot before ending a turn.
- After GUI editing, run `ghidra_pool.sh sync` so all unlocked slots see the new symbols/types.
- If two sessions race for the same slot, both should retry — `acquire` is idempotent and skips locked slots.

## Coordinate / numeric conventions (TBD as discovered)

Will be filled in once we confirm Mashed's:
- Position units (RenderWare typically uses 32-bit floats; verify)
- Angle units (radians vs degrees vs fixed-point)
- Color order (RGBA vs BGRA — check via TXD reader)
- Endianness (PC build = LE)

Until verified, every numeric constant in reversed code must be cited against the Ghidra address where it appears, with both raw hex and signed/unsigned decimal forms documented.

## Known unknowns (priority research questions)

1. `.piz` offset semantics — sector × 2048 vs raw bytes? Verify on `Common\AI.piz`.
2. `gamesave.bin` `DEADBEEF` magic — header struct?
3. `videocfg.bin` 512-byte struct — field layout?
4. Lua VM version — confirm 5.0 vs 5.1 from strings inside `MASHED.exe`.
5. Copy protection in `launch.exe` — the SciLor patch shows it's defeatable; document the exact patched bytes.
6. Any RW custom plugins (Mashed-specific RW chunks in `.rws`)?

## Tracker discipline (mandatory)

The four trackers (`hooks.csv`, `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`) are the project's load-bearing bookkeeping. **Mutate them only via the `re-classify` skill.** Direct edits skip the rubric and drift the project state.

- Every function you touch in a session: `re-classify` it before ending the session, even if confidence didn't change (the `notes` field tracks what was looked at).
- Every `[UNCERTAIN]` you drop: filed in `UNCERTAINTIES.md` before the session ends, with a path-to-resolution.
- Every stub you leave: filed in `STUBS.md`, inline `// STUB S-NNNN` comment placed at the call site.
- Every "we'll get to this later": filed in `DEFERRED.md` with a re-pickup condition. Never "later." Never "TODO." Always observable conditions.

## Per-session workflow

1. **Start**: `multi-session` skill loads — run startup protocol (identify worktree, take pool inventory, pull latest trackers, acquire slot).
2. **Work**: `ghidra-pool` for queries, `hook-author` for scaffolds, `diff-original` for verification.
3. **Classify**: `re-classify` after every function is touched, in real time. Don't batch — context is freshest at the moment of work.
4. **End**: `multi-session` shutdown protocol (close programs, commit tracker updates, release slot).

## Things that will get an agent yelled at

- Modifying anything under `original\` without explicit user permission.
- Authoring a hook against an unverified binary version.
- Using "probably", "likely", "seems to" in RE notes.
- Promoting a function to C3+ without satisfying the rubric gate.
- Editing `hooks.csv`/`STUBS.md`/`UNCERTAINTIES.md`/`DEFERRED.md` outside the `re-classify` skill.
- Releasing a Ghidra pool slot that's bound to another worktree's `.pool_slot`.
- Holding the master `Mashed.gpr` writable while another session is active.
- Creating a new MCP server or skill that duplicates one in TD5RE.
- Pulling a new dependency (header, library, build tool) for a single hook.
- Writing comments that describe what code does instead of why a non-obvious decision was made.
