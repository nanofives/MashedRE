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

## GHIDRA-POOL-DOUBLE-ISSUE — `acquire` can hand the SAME slot to two sessions (INCIDENT 2026-09-02)
- **Signature:** a child reports binding `Mashed_poolN`; a later `ghidra_pool.ps1 acquire` in
  another session (e.g. the parent) returns **the same N**. Symptom on the loser's side is a
  headless run reporting the project locked, or — worse — an output file that is **missing or
  truncated while the invocation otherwise looks successful**.
- **Root cause:** the child's binding is not represented by a root-level `mashed_pool\Mashed_poolN.lock`,
  so `acquire` sees the slot as free. Observed on 2026-09-02: pool14 was held by a child while the
  root held stale locks only for pool0/1/10-13. Distinct from GHIDRA-STALE-LOCK, which is the
  opposite failure (a slot reported LOCKED with no JVM).
- **Why it is dangerous rather than merely annoying:** every routine pool script
  (`DecompPC`/`DisasmPC`/`CallersPC`/`XrefRange`) runs `-noanalysis` and read-only, so the project
  is not corrupted — but a **truncated disassembly listing silently becomes a bad verbatim
  transcription**, and verbatim transcription is the whole product. A lost lock does not always
  announce itself: on 2026-09-02 `DisasmPC` emitted no `wrote` line and no file for a valid RVA,
  and a plain retry worked.
- **Mitigation (until fixed):** exchange slot ownership **by message** between parent and child;
  do not infer it from the script. Never call `ghidra_pool.ps1 release N` for a slot you did not
  acquire in that same command. Before transcribing, check each listing starts with
  `# function FUN_xxxxxxxx @ xxxxxxxx` and ends at a `RET` (or tail `JMP`); re-run anything short.
- **Proper fix:** have `acquire` write and honour a root-level per-slot lock stamped with the
  owning session id, so a second `acquire` skips it.

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

## FULL-AUTOHOOK-BOOT-AV — the full .asi hook set AVs MASHED.exe at boot (found 2026-10-09)
- **Signature:** `run_verify_hook.py` (any hook) fails `attach failed: ... VirtualAllocEx returned
  0x00000005`; a plain auto-hook boot exits `0xC0000005` at ~4 s; WER event fault offset `0x000c7785`
  (read of `0x1c`; ret chain `0x004ccefa 0x004ccb4e 0x004d801d 0x004c7853 0x0042d535 0x004923ad`).
  `MASHED_RE_NO_AUTO_HOOK=1` boots fine.
- **Root cause (bisected by registry index, LO/HI, 15 boots):** index 217 `VideoGetRenderWidth`
  (`0x00498bc0`, `Boot/VideoConfig.cpp`) crashes ALONE 3/3, and so does its sibling
  `VideoGetRenderHeight` (`0x00498bd0`) alone. Both are the **`fix_camera_res` boot-patch sites**: on
  disk they are `mov eax,640 / mov eax,480` (coupled to the d3d9 shim's 640x480 backbuffer). The hooks
  restore the UNPATCHED body `mov eax,[0x00616028]` / `[0x0061602c]`, and those globals read
  **1680 x 1050** at runtime on this machine (hooks off, own-PID read), i.e. they follow the display
  configuration. Width/height then disagree with the backbuffer and RenderWare AVs.
- **Not a single-hook defect:** full set minus `0x00498bc0` boots 2/2, full set minus both getters
  boots 1/1, yet the height hook alone crashes. Other installed hooks change the path.
- **Why it passed C4 in June:** [UNCERTAIN] the globals' value then. They track the display, so a
  monitor/topology change is consistent but unmeasured.
- **FIXED 2026-10-09:** `Core/HookSystem.cpp` refuses any hook whose 5-byte JMP overlaps a byte where
  `MASHED.exe` differs from `MASHED.exe.unpatched` (table `kBootPatched`, 17 clusters). Refused today:
  `0x00498bc0`, `0x00498bd0` (fix_camera_res), `0x00496400` (disable_log), `0x004a4541` (fix_fopen, was
  already refused as an E9). Verified: full set installs 1260, boots, and full-set `run_verify_hook` passes
  again (`bounded_table_signclamp_46d7f0`, `update_41b520`). `MASHED_HOOK_ALLOW_BOOTPATCH=1` disables the
  guard. If the boot-patch set changes, re-diff and update the table.

## AUTOHOOK-INLINE-ASM-IMMEDIATE — full hook set kills MASHED.exe during the nav boot-to-menu (found 2026-10-16)
- **Signature:** `run_verify_hook.py` of a `scenario='race'` row (and `run_diff`-style nav drives with the .asi
  auto-hook ON) prints `[nav] booting to menu...` then `frida.InvalidOperationError: script has been destroyed`;
  the process exit code is `0xC0000005`, access violation read at **EIP 0x1** (stack top `0x00494ff4`,
  then `0x00771a0c`). `MASHED_HOOK_ONLY=__none__` drives to the race fine; menu-only path2 runs (no nav) are fine.
- **Root cause (bisected by registry index with `MASHED_HOOK_LO/HI`, 20 boots, `verify/dupinstall2_20261016/bisect_nav.py`):**
  index 1335 of 1354, `Fwd494fd0_DoublePlusHalf` (`0x00495080`, `Render/PromoLoop_round256.cpp:172`). The naked body
  writes `mov esi, dword ptr [771A0Ch]`; **MSVC inline asm assembles a bare `[imm]` as the immediate**, so the built
  export is `be 0c 1a 77 00  mov esi, 0x771a0c`, not `8b 35 0c 1a 77 00  mov esi,[0x771a0c]`
  (`verify/dupinstall2_20261016/disasm_asi_export.py`). `FUN_00494fd0` takes its DirectInput device pointer in ESI
  (`mov eax,[esi]` / `call [eax+0x1c]` at `0x00494fee..0x00494ff1`), so the vtable read lands in the object and the
  call goes to `0x1`. Its sole caller is `FUN_0043dfd0` (hooks.csv); the AV comes during the nav drive's `booting to menu` step.
- **Why nothing saw it:** the C3 A/B stubs the callee and records stack args only; ESI is a register input.
- **Scope:** one site. Every other absolute operand in `mashedmod/src` (about 60 lines) is spelled `ds:[...]`, which
  assembles as a memory operand (probe `verify/dupinstall2_20261016/asm_probe/probe.cpp`: `[771A0Ch]` and
  `[0x771A0C]` give `be ...`, `ds:[771A0Ch]` and `ds:[0x771A0C]` give a memory load).
- **FIXED 2026-10-16 (fwd494fd0), entry closed:** `mov esi, dword ptr ds:[771A0Ch]` (built export now `3E 8B 35 0C 1A 77 00`).
  Re-earn: path1 gained a register-input check (`stub_esi` / `stub_esi_seed` in `diff_template.js`, records ESI at the
  stubbed callee, seeds `0x00771a0c` to a sentinel): **RED 8/8 on the unfixed build** (orig `esi=0xd1d0c0de` vs reimpl
  `esi=0x771a0c`, stack args equal), GREEN 8/8 fixed; path2 PASS 2/2; nav boot to a race with the FULL hook set (1290
  installed, no skip) alive and a 45 s full-set survival; the three race rows pass path2 without the skip. Row C2 -> C3.
  Evidence `verify/fwd494fd0_20261016/`.
- **Guard added:** `scripts/lint_inline_asm_abs.py` runs from `mashedmod/build.bat` and fails the build on any bare
  `[0x...]` / `[NNNh]` operand in inline asm without a segment override (skip: `MASHED_SKIP_ASM_LINT=1`). On the
  unfixed tree it reports exactly this line and nothing else; `--selftest` carries 12 positive/negative samples.
- **Workaround no longer needed:** `MASHED_HOOK_SKIP=Fwd494fd0_DoublePlusHalf`.

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

## RETIRED-POWERUPS-NOPS-LIVE — boot crash eip=0 (stack-imbalance ret-to-0)
- **Signature:** stock boot exits pre-menu every time; crash catcher
  (`re/frida/poll_attach_catch_crash.py`) shows **eip=0x0, esp=0x1afe50,
  ecx=0xff000000**, read AV at 0x0, return addr 0x4a4d38 (CRT band) — the
  long-chased stack-imbalance signature from BOOT_PATCHES.md (skip_movies
  section). No WER dump is written. `mashed_re.exe` (standalone) renders fine.
- **Root cause:** the RETIRED `skip_powerups` 25-NOP block was LIVE on
  `original\MASHED.exe` at 0x295d (VA 0x40295d) — boot crash #2
  (BOOT_PATCHES.md). How it got re-applied mid-session 2026-07-01 is
  [UNCERTAIN] (the script itself refuses to apply); detected 2026-07-02 by
  byte-diffing `MASHED.exe` vs `MASHED.exe.unpatched` and mapping every diff
  run to a known patch site — 0x295d was the one unexplained run.
- **Detection recipe:** byte-diff current vs `.unpatched`; expected sites are
  ~15 small runs (show_windowed/skip_audio_com/skip_selector/
  skip_controller_dialog @0x951xx+0x98dbc, fix_camera_res @0x98bc0/0x98bd0,
  disable_log @0x963e7/0x96400/0x96490, fix_fopen @0xa4541+0x10f6c4,
  fix_joypad @0x95870+0x108bd5, skip_audio_com @0x1bc750). ANY other run =
  foreign bytes.
- **Self-heal:** `py -3.12 scripts/repatch_original.py` — since 2026-07-02 it
  runs the retired script's un-apply guard first ("FIXED retired
  skip_powerups NOPs were LIVE"). Direct fix:
  `py -3.12 scripts/patch_mashed_skip_powerups.py` (un-applies when NOPs found).

---

## HKLM-DWM-MITIGATION — boot crash (0xC0000005) from a SYSTEM-level compat layer
- **⚠️ DIAGNOSIS CORRECTED 2026-07-02:** the 2026-07-01 incident this entry was
  written for was actually RETIRED-POWERUPS-NOPS-LIVE (above). The Layers theory
  was falsified: (a) `__COMPAT_LAYER` decisive test still crashed; (b) a
  junction-path launch (exe path with NO matching Layers entry) still crashed;
  (c) after the NOP un-apply, boot works WITH the HKLM `$ DWM8And16BitMitigation`
  entry AND an HKCU string carrying `DWM8And16BitMitigation` still present.
  The GPU-wedge theory was also ruled out for that incident (`mashed_re.exe`
  D3D9 rendered fine, no reboot needed). The HKLM entry may still matter in
  other configurations — keep the tests below — but it is NOT sufficient to
  break boot on this machine as of 2026-07-02.
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
- **DECISIVE no-elevation test (do this FIRST — from memory `project_dwm_mitigation_breaks_shim`):**
  set `$env:__COMPAT_LAYER="RunAsInvoker WIN98RTM HighDpiAware EmulateHeap"` then launch
  MASHED. This OVERRIDES the registry Layers merge (HKCU *and* HKLM) for that launch, no
  admin needed. If it now boots → AppCompat/DWM was the cause (either always launch with that
  env var, or clear the HKLM entry with elevation for a permanent fix). If it STILL crashes →
  it is NOT AppCompat but the **GPU-driver wedge** (see that memory): ~15+ rapid MASHED
  spawns/session thrash the UMD → identical 0xC0000005 pre-CreateDevice signature, and ONLY a
  **reboot** clears it. The WS-R6-D session did FAR more than 15 spawns, so the GPU wedge is a
  strong candidate here — try the `__COMPAT_LAYER` test before assuming the HKLM entry is the
  culprit. scenario_launch.py does not set `__COMPAT_LAYER`; export it in the shell first.
