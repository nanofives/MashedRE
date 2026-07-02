# Known issues catalog — Mashed RE environment

The single place that records every recurring execution failure: its **signature**
(how to recognize it), **root cause**, **self-heal** (auto-fix if any), and the
**manual fallback**. Goal: never re-investigate the same failure twice.

- Source of truth for the machine-readable signatures is `scripts/diag.py` (the
  `SIGNATURES` table). This file is the human narrative. Keep IDs in sync.
- Run `py -3.12 scripts/diag.py doctor` to auto-heal the safe ones + diagnose the rest.
- Wrap any flaky command with `py -3.12 scripts/diag.py run --tag <t> -- <cmd>` to
  capture full context to `re/diag/runs/` and auto-match it against this catalog.

---

## D3D9-NOTAVAIL — standalone can't create a render device
- **Signature:** `InitD3D9: CreateDevice(... -> hr=0x8876086A` (D3DERR_NOTAVAILABLE) or
  `0x8876086C`, on `mashed_re.exe`; the backbuffer `.bmp` is not rewritten.
- **Root cause:** the process was spawned in a context with **no presentable desktop
  surface** — the workstation is locked / display asleep / the session is
  disconnected, or it's a nested background/automation spawn. The GPU adapter is
  queryable but no device is presentable. Confirmed display-state dependent: the same
  exe rendered fine from an interactive desktop session earlier the same day.
- **Self-heal:** NONE possible from automation (you can't unlock the screen in code).
  `diag.py doctor` DETECTS it (via `probe-render`) and reports clearly instead of
  letting a 30 s run fail mysteriously.
- **Fallback:** (1) run the build on an **active, unlocked desktop** — the standalone
  self-writes `verify/r5/car_*.bmp` under `MASHED_DRIVE_DEMO=1`; (2) for renderer
  correctness without pixels, use **offline numeric proof** on the real assets
  (per-vertex N·L spread, color math) as WS-E s2 did. Renderer changes are
  RE-verifiable offline; visual acceptance needs the user's desktop.

## FRIDA-ATTACH-FLAKE — Frida spawn/attach fails transiently
- **Signature:** `attach failed: process with pid N either refused to load
  frida-agent, or terminated during injection` (`run_diff*` rc=4); or a ~61 s hang
  (rc=3 timeout).
- **Root cause:** Frida **spawn** perturbs MASHED's memory layout and trips the
  static-init/heap boot AV (see [[project-boot-crash-static-init-not-heap]]); it is
  intermittent. Record via **attach-after-boot**, not spawn.
- **Self-heal:** retry the single diff up to 3× (a retry usually passes); the pipeline
  collector already does this per-hook (drop-not-abort).
- **Fallback:** `re/frida/record_attach.py` / `poll_attach_catch_crash.py`
  (attach-after-boot). Never blanket-kill MASHED — kill only PIDs you spawned.

## GHIDRA-STALE-LOCK — pool slot LOCKED with no JVM
- **Signature:** `LockException` opening a Ghidra project; `Mashed_poolN.lock~ Device
  or resource busy`; `ghidra_pool.ps1 status` shows `Slot N: LOCKED` while **no
  `java.exe` is running**.
- **Root cause:** a crashed/killed Ghidra MCP session left a stale lock file (the
  embedded JVM died holding the handle).
- **Self-heal:** `diag.py doctor` releases locked slots **only when no `java.exe` is
  running** (proving the lock is stale). If a JVM IS running, it's left alone (another
  session may own it — multi-session rule).
- **Fallback:** read ground truth without Ghidra: `py -3.12 re/tools/console/xtwin.py
  0x<rva>` (Xbox twin) or capstone (`py -3.12 scripts/dump_asm.py 0x<rva>:<size>`).

## WORKTREE-SYMLINK-WIPE — `git worktree remove --force` deleted original/ (INCIDENT 2026-06-27)
- **What happened:** the `worktree` skill **symlinks `original/`** (the immutable game
  install) into each worktree so subagents can read assets (SKILL.md line 40). diag
  `doctor`'s auto-prune ran `git worktree remove --force <path>`, which **followed the
  `original/` symlink and recursively deleted the REAL assets** — anchor
  (`MASHED.exe.unpatched`), every `.piz`, `launch.exe`, the d3d9 shim, `videocfg`.
  Everything else (git-tracked source, trackers, `mashed_pool/`, `log/`) was untouched.
- **Root cause:** force-removing a worktree that contains a symlink/junction to an
  out-of-tree immutable dir recurses through the link on Windows.
- **Prevention (structural, 2026-06-27):**
  1. `diag.py check_worktrees` is **DIAGNOSE-ONLY — never auto-removes**.
  2. **Remove worktrees only via `py -3.12 scripts/diag.py wt-remove <path>`** — it
     strips reparse points (junctions) link-only via `rmdir` FIRST, then
     `git worktree remove` **without `--force`**. NEVER `git worktree remove --force`.
  3. The standalone reads main's assets WITHOUT a junction: it self-locates the repo
     root by walking up from the exe, or honors **`MASHED_ROOT=<abs main repo>`**
     (exe_main.cpp). So worktrees never need an `original` junction — don't create one.
  4. The `worktree` skill was updated (no `--force`, no junction, wt-remove).
  5. `diag.py doctor` now has an **`original` integrity check** that flags an empty/
     missing install as BLOCKED instantly (catches a wipe the moment it happens).
- **Recovery:** `original/` is gitignored (not recoverable from git). Restore a clean
  install into `original/`, then run **`py -3.12 scripts/repatch_original.py`**
  (re-applies the 9 boot patches + canonical videocfg; idempotent) and finish with
  `build_d3d9_shim.bat` + `setup_mashed_compat.ps1`. See [[feedback-never-touch-original-dir]].

## ORPHAN-WORKTREE — dead subagents leave worktrees + branches
- **Signature:** `.claude/worktrees/agent-*` or `.worktrees/*` entries whose owning
  task/agent is no longer running; `git worktree list` grows unboundedly.
- **Root cause:** a stopped/failed/finished subagent's `isolation:'worktree'` (or
  `worktree` skill) checkout isn't cleaned up; nested subagents spawn their own.
- **Self-heal:** `diag.py doctor` prunes a worktree+branch **only if it is SAFE** —
  the branch has **0 commits beyond main** (or is already merged) AND the working copy
  has no uncommitted changes. Worktrees with unmerged work are **flagged, not pruned**
  (could be a live session or unbanked work). Never prune the worktree of a running task.
- **Fallback:** manual `git worktree remove --force <path>` + `git branch -D <b>` after
  confirming the work is merged/disposable.

## WF-CRLF — Workflow launch rejected for control characters
- **Signature:** Workflow tool error: `script contains control characters that would
  be hidden in the approval dialog`.
- **Root cause:** the workflow `.js` picked up Windows CRLF (`\r`) line endings (git
  `autocrlf`); the `\r` are control chars the approval dialog rejects.
- **Self-heal:** `diag.py doctor` normalizes CRLF→LF in `re/pipeline/*.js` /
  `*.workflow.js`.
- **Fallback / permanent:** `.gitattributes` pins `* .js eol=lf` for pipeline scripts
  (added by diag once).

## BUILD-BASH-CMD-NOOP — `cmd /c build.bat > LOG` in Bash silently doesn't compile
- **Signature:** the build "succeeds" (exit 0 or 1) but `mashedmod/build/mashed_re.exe`
  mtime is UNCHANGED and the log file contains only the Windows shell banner
  (`Microsoft Windows [Versión ...]`) — no compile lines. Your code change isn't in
  the running exe (e.g. device stays HAL/SW after a HW-VP change).
- **Root cause:** in Git-Bash, `cmd /c mashedmod\build.bat > LOG 2>&1` mangles `/c` and/
  or the redirect captures only a spawned interactive shell's banner — build.bat never
  actually runs the compiler. The piped form (`cmd //c "...build.bat" 2>&1 | grep`) and
  the PowerShell tool both compile fine; subagents' builds are unaffected.
- **Self-heal:** none (invocation pattern). **ALWAYS build via the PowerShell tool**
  (`& cmd /c "mashedmod\build.bat"`) — see memory project-frame-limiter-speed-fix.
- **Verify a build actually happened:** check the exe mtime is fresh AND, for a device/
  render change, grep mashed_re.log for `CreateDevice(HAL/HW` vs `HAL/SW`.

## VERIFY-SCRATCH-DIRTY — game runs dirty tracked verify screenshots
- **Signature:** `git status` shows only `verify/r5|r6/*.bmp` (or `verify/**/*.png`)
  modified after a `MASHED_DRIVE_DEMO`/race run.
- **Root cause:** the standalone's `DumpBackbufferBMP` overwrites tracked verify
  baselines during demo runs.
- **Self-heal:** `diag.py doctor` restores them (`git checkout -- verify/...`) when
  ONLY verify image files are dirty.
- **Fallback:** `git checkout -- verify/`.

## MASHED-BOOT-AV — original AVs ~4 s into boot
- **Signature:** new `%LOCALAPPDATA%\CrashDumps\MASHED.exe.*.dmp`; Frida attach fails
  `VirtualAllocEx 0x5`.
- **Root cause / heal:** several distinct classes — CRT static-init lock, fopen garbage
  FILE*, joypad device ptr, EMULATEHEAP build-inversion. All handled by the
  `scripts/patch_mashed_*` patches + `setup_mashed_compat.ps1`. Diagnose with
  `scripts/parse_minidump.py`. See CLAUDE.md "Runtime state" + the boot memories.
- **Self-heal:** `diag.py doctor` reports dump presence + points at the patch scripts;
  it does NOT auto-apply binary patches (too consequential to automate).

---

## HKLM-DWM-MITIGATION — boot crash (0xC0000005) from a SYSTEM-level compat layer
- **Signature:** MASHED.exe launches then `game exited while waiting for menu (phase 1)`
  (or a phase-0 timeout) on EVERY boot, even STOCK (`MASHED_RE_NO_AUTO_HOOK=1`, or the
  `.asi` renamed away). `log/fps_limiter.txt` is STALE (game never rendered). `diag doctor`
  still reports `render OK` (that field is the last *successful* CreateDevice, not current).
- **Root cause:** repeated launch/kill crash cycles (e.g. a long Frida/scenario session)
  make Windows PCA add `DWM8And16BitMitigation` to the AppCompat **Layers** — and not only
  under HKCU (which `setup_mashed_compat.ps1` rewrites) but under **HKLM**
  (`HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers`, value name =
  full path to `original\MASHED.exe`, data `$ DWM8And16BitMitigation`). The system-level
  entry takes precedence and breaks the d3d9 shim → CreateDevice AV → the process exits
  before the menu. Related: memory `project_dwm_mitigation_breaks_shim` (HKCU-only case).
- **Self-heal:** NONE. `repatch_original.py`, rebuilding the d3d9 shim, and
  `setup_mashed_compat.ps1` (HKCU layer + PCA Store clear) do NOT touch HKLM.
- **Manual fallback (REQUIRES ELEVATION):** in an **Administrator** PowerShell run
  `Remove-ItemProperty -Path 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers' -Name 'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe' -Force`
  (repeat for any `.worktrees\*` / `.claude\worktrees\*\original\MASHED.exe` names that also
  carry the mitigation). Then re-run `setup_mashed_compat.ps1` (non-elevated) to normalize
  HKCU, and boot. A non-elevated session CANNOT clear the HKLM entry ("Requested registry
  access is not allowed"). First seen 2026-07-01 (WS-R6-D session).
