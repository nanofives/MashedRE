# Next session kickoff

> ## START HERE 2026-10-23 audiolink slices 1 + 2 (UNMERGED, branch `work/audiolink`, pushed): **11 Audio files (34 functions) linked into the exe, 2 latent defects fixed.** Next = slice 3 (RwsStream) or Mariano's merge / twin decisions.
>
> - Slice 2: AudioMemory, raw alloc / free = CRT malloc / free in the exe (RwStreamCore precedent), the original trampolines in the .asi; bytes match, path1 5/5, gates unchanged on exe b2f7fba6. `RESULT_slice2.md`.
>
> - Survey: `verify/audiolink_20261023/SURVEY.md` (link order, (a)/(b)/(c)). Result: `RESULT_slice1.md`.
> - Fixed from the bytes: 0x005b4060 truncates (Ghidra ROUND() wrong, path1 vectors never had a fraction), 0x005baf00
>   quiets SNaN; 4 width / order fixes. Controls RED on the old bodies, path1 29/29, gates unchanged on exe 71587212.
> - `music_group_volume_set` runs through `early_window_leaf_diff.py` (no diff_template.js handler for its arg_type).
> - Decisions for Mariano: merge `work/audiolink`; the two job twins 0x004625b0 (CharacterBankPath) and 0x005aeb90
>   (VolToDs); the two precedent files AudioQueuePop / AudioVecLength still reach MASHED addresses in the exe.
> - Next slice per the survey: RwsStream (disassemble 0x005ab380 for its arity and 0x005abf80 for CALL vs JMP first), then the 0x005adf30 split + RwsFmt.

> ## START HERE 2026-10-23 next lane = **audiolink** (Mariano's pick after savelink2). Kickoff below; the savelink2 block follows.
>
> ```
> # Kickoff: Mashed D4, Audio TU linking (lane audiolink)
>
> Read first: re/NEXT_SESSION.md (this block and the savelink2 block under it), ROADMAP.md §D0.7 (Audio/ is "mixed":
> AudioDSound 8 rows + AudioRws 20 rows are genuine intent, the other ~18 files are hook-shaped drift) and §D4
> "Link the Audio TUs", then the pattern files: Render/RwStreamCore.cpp, Save/GameSaveVFS.cpp, Save/SettingsConfig.cpp,
> Save/FileWrappers.cpp (one body, #ifdef MASHED_STANDALONE host thunks + private storage, .asi trampolines + sweep).
>
> ## State (2026-10-23)
> - race/first-frame-parity at 51fd9304e (pushed). Gates on the current tree: G-CTRL bacfc747/8dd1c004/6f9932b6,
>   B f9deb9b9/d3601fcc/9da9a26a. Lint 0 NEW (8 allowlisted).
> - Audio rsp membership: 21 files .asi-only (AtomicExchangeStore, AudioChainWalk, AudioCharacterBankPaths, AudioDSound,
>   AudioLeaves_ab1..ab4, AudioListSearch_wfb0f, AudioList_ag, AudioMemory, AudioMusic, AudioPoolFreeGuards, AudioRws,
>   AudioTableDispatch, AudioVoiceQueueSet, MixedC3Sweep, RingHeaderInit, RwsFmt, RwsStream, TimerGetters_ah5),
>   2 in both (AudioQueuePop, AudioVecLength), 2 exe-only (AudioEngine, RwsBank = the exe's own audio implementation).
> - ROADMAP gate: add-backs need NO MASHED ADDRESS IN ANY exe CODE PATH (the exe arm of each file).
>
> ## Step 0: worker survey first (delegate.ps1 -Repo Mashed, opus), no code before it
> Per .asi-only Audio file: functions + RVAs + hooks.csv level, every address tunnel, every callee by address and
> whether a shared/exe body exists, and OVERLAP with the exe's AudioEngine.cpp / RwsBank.cpp (same RVA or same job:
> a link that creates a CROSS-TARGET dual copy is a stop-and-ask). Output: a link order, smallest closure first,
> split into "dead code in the exe" vs "would replace an AudioEngine path" (the latter is a decision for Mariano).
>
> ## Per slice (the savelink2 recipe)
> 1. Disassemble every function (py -3.12 re/tools/disasm_fn.py <start> <end>, from the main checkout) and compare
>    with the port BEFORE linking: savelink2 found latent defects in 4 of 5 slices, all on paths a race never runs.
> 2. One shared body, exe arm = own CRT / private storage / no address; .asi arm = trampolines replaying the
>    original's first bytes (check BOTH .exe and .exe.unpatched: a boot patch on a target changes what to replay,
>    and HookSystem's kBootPatched guard refuses overlapping hooks).
> 3. PREREG committed alone (check git diff --cached first). Sweep via MASHED_LEAF_SELFTEST(_SWEEP) or an exported
>    sweep run by nav_champ_probe.py --plan exportsweep --export NAME --hooks RVAS; live A/B in scenario_launch.py
>    --hooks. Compare everything the function writes, including the RW error slot where relevant.
> 4. Build (MASHED_NO_DEPLOY=1), COPY the fresh mashed_re_dev.asi into the private root, run, then G-CTRL + B on the
>    final exe (verify/savelink_20261023/run_arms.py with A4_OUT / A4_ROOT), lint.
> 5. Trackers via re-classify (file column / notes; no level moves without evidence). RESULT, commit, push. Merge only
>    when Mariano says.
>
> ## Hard-won rules
> - Every game launch needs the tool sandbox DISABLED (sandboxed, the original AVs at 0x005baf48; not a compat problem).
> - Private root only (D:/MashedWT/<lane>_root, robocopy original/, own AppCompat layer
>   "~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP"); D:/MashedWT/savelink_root is ready to reuse.
> - Audio-specific: launches use MASHED_MUTE=1; audio state can be timing-dependent, so expect noo != 0 on live A/B of
>   anything that reads a clock or a voice position and classify it before calling it a defect.
> - The main checkout's index is polled by a git diff --numstat every few seconds: retry git add/commit/merge a few
>   times, or commit from the worktree (its own index) and push.
> - Edit C++/Python with Edit or a patch script written to a file (raw strings); heredoc Python broke on quoting.
> - Kill only your own PIDs. Commits end with Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>.
> ```

> ## START HERE 2026-10-23 savelink2 (MERGED: `race/first-frame-parity` at `0f8d6f31a`, pushed): **the knob's all-unlocked load is not visible, and the Save TU link is done to the dialog boundary.** Next = Mariano picks option B, the dialog, or another D4 lane.
>
> Every slice has a PREREG committed before its arms, gates unchanged (G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a) and
> lint 0 NEW (8 allowlisted). Evidence in `verify/savelink2_20261023/`, raw data in `D:/MashedWT/savelink2_data/`.
> - **(a) knob side effect** (`RESULT_knob_progress.md`): with `MASHED_SAVE_ORIGINAL=1`, progress loads 13/13 from the shipped save, but screen 6 draws
>   the same as knob OFF (S0 vs S1 GREEN; the only other divergence is the wall-clock pulse quad, draw 158, which also differs OFF vs OFF). Cause: the
>   boot restore (`exe_main.cpp:9445`) puts the shipped col 4 = 2 into 0x007f0a40 either way, and the cup reads that table. The load side does not
>   block a flip. The write target (the diffing reference) and the col 5 trophy cell still do.
> - **(b) Save TU link**, one shared body per function in both rsp files (exe: dead code), each read from the disassembly. Five latent defects fixed:
>   - RwStreamWrite `0x004cbe80` -> Render/RwStreamCore.cpp (Save/RwStream.cpp removed). The old copy's 4 error paths called `FUN_004d8480` with no block
>     and dropped `FUN_004d7ff0`'s 2nd arg. The 2026-10-11 negctl missed it because it never read the RW error slot. C4 kept (Mariano).
>   - GameSaveVFS: VfsFileExists `0x00550b00` (keep the ':' in the prefix, resume the scan after a miss, NULL default -> error cb(6)), AutosaveTrigger
>     `0x004099a0` (printf on 0x005ccb34, the old copy used wprintf on a wrong address).
>   - Video-config cluster `0x004963e0 0x00496400 0x00498910 0x00498950 0x004989b0` -> Save/SettingsConfig.cpp (SettingsCfg.cpp folded in). No
>     functional defect. v1 PREREG failed (4 ndiff) on the boot-patch guard. v2 registered guard off 22/0/0 plus a guard-on control with ndiff 4 exactly
>     at disable_log's RET. **Standing facts:** the C4 hook `0x00496400` is REFUSED by the guard in every default run. `0x004963e0` installs over
>     disable_log's JE (invisible: the opener 0x00496490 is patched, handle NULL).
>   - Option C: FileReadWrapper `0x004b3b70` / FileWriteWrapper `0x004b3bb0` -> Save/FileWrappers.cpp (out of SettingsAndIO.cpp). Sweep 12/0/0, live
>     .dff and gamesave reads 0 diff. **From the bytes:** RwStreamClose `0x004cc160` has no NULL guard, so a write that opens and then fails crashes the
>     original in the close (the wrapper closes the write RESULT).
> - Harness: in-process sweeps (`MASHED_LEAF_SELFTEST(_SWEEP)`) in RwStreamCore / GameSaveVFS. Exported sweeps run through `nav_champ_probe.py --plan
>   exportsweep --export NAME --hooks RVAS` (sets the `--hooks` env; `settingssweep` preset). `verify/savelink2_20261023/run_knob_arms.py` (cup-screen
>   drawstream arms; creates ROOT/log first, because GameFlow's log fopen fails silently without it).
> - Lessons:
>   - A private root's `mashed_re_dev.asi` is NOT refreshed by `MASHED_NO_DEPLOY` builds. Copy the fresh one in, or the A/B runs old code (one void run).
>   - A staged `git rm` rides along into the next commit (it landed in two PREREG commits). Check `git diff --cached` before committing a PREREG.
>   - Grow / first-alloc A/Bs must compare only the written prefix (uninitialized heap past it gave noo=1).
>
> **Decisions for Mariano:**
> 1. Option B: make the file wrappers the exe's save path. It needs exe bodies for RW stream open/close (0x004cc230 / 0x004cc160) and the VFS open
>    0x005507b0 / 0x00550910, plus a call on the failed-write crash (keep it, or a declared guard).
> 2. SettingsAndIO (3 dialog helpers) + SettingsDialog (3): leave asi-only? Dialog 0x65 is unreachable in the patched reference (skip_selector) and
>    the exe has no resources.
> 3. `0x00496400` stays C4 although its hook never installs by default?
> 4. The knob default: still blocked by the write target, not by the load.
>
> **Paste-ready kickoff:**
>
> ```
> # Kickoff: Mashed D4, next lane after savelink2
>
> Read first: re/NEXT_SESSION.md (START HERE 2026-10-23 savelink2), verify/savelink2_20261023/RESULT_filewrappers.md.
> Base: race/first-frame-parity at 0f8d6f31a. Step 0: ask Mariano which of decisions 1-4 to take, or which other D4 item
> (dual-copy allowlist 8 findings, D1-residue R1-R2, Audio TU linking).
>
> Rules: game launches need the tool sandbox DISABLED; private roots only (D:/MashedWT/<lane>_root with its own AppCompat
> layer), copy the fresh .asi into the root; PREREG before each arm; rerun G-CTRL + B (verify/savelink_20261023/run_arms.py,
> A4_OUT / A4_ROOT) on the final exe after any code change; kill only your own PIDs; trackers only via re-classify;
> read-only surveys to the worker (delegate.ps1 -Repo Mashed); commits end with Co-Authored-By: Claude Opus 5.5.
> ```

> ## START HERE 2026-10-23 savelink, ghost leg (UNMERGED, branch `work/savelink`, pushed): **U-3560 narrowed: the "profile block" is the 13-slot TimeTrial ghost buffer, and a real ghost round-trips.** Next = Mariano's merge decision, then savelink2.
>
> Added after the block below was written (all on `work/savelink`, PREREGs before each arm):
> - `re/analysis/save_ghost_block_20261023.md`: file `0x4..0x24A3F` = `*DAT_008a94a8`, 13 ghost slots sized by the table `0x005f29d0` (13 {id, float}
>   pairs, sum 670.0 s; `13 x 0x19c + 4020 x 0x24 = 0x24a3c`). Written only by `ReplaySave 0x004117b0` (`FUN_00483ca0`), armed only by the TimeTrial tick
>   `0x00429310 -> FUN_00411870(1)` on a finished run under the table float; autosave only if the save counter `DAT_008a95ac != 0` (shipped: 0).
>   Shipped save: 0 non-zero bytes in the block by explicit count.
> - `verify/savelink_20261023/RESULT_ghost_roundtrip.md`: Q0-Q3 + R1-R4 held. A real TimeTrial record (id 0x1e, 120 frames), copied by the game's own
>   `FUN_00483ca0` outside its gate, survives `SaveWrite`, a fresh original's menu-path reload (150,076/150,076 bytes) and the standalone's knob-ON
>   in-place write. On a TimeTrial over the reloaded save it becomes the time-attack record, and `Replay::Load 0x00483d10` discards the stored heap
>   pointer (rebases to a fresh allocation); frames equal outside the rebuilt `+0x1c` links; no crash in 40 s.
> - Trackers (re-classify `dec4b5641`): U-3560 NARROWED (still open: header fields beyond [0..2,5,0x5b,0x174], the loader's `(n+2) x 0x24` read, the
>   frontend caller of the restore); U-0177 decimal corrected; `0x2443C -> 0x24A3C` in `save_gamesave_d3/00404e80.md` and `structs/gamesave_layout.md`;
>   stale U-1567 citations -> U-3560. No C-level moved.
> - Harness additions: `scenario_launch.py --ghost-capture <dir>`, `SAVELINK_GHOST_SNAP_ONLY=1`, `SAVELINK_PRELOAD=1` (runs `SaveLoad` +
>   `DeserializeFromBuffer` at the menu before the warp, because the warp skips the restore: without it the in-race block is all zero);
>   `nav_champ_probe.py --plan ghostdump` (`MASHED_GHOST_OUT`).
>
> **Decisions for Mariano** (unchanged from the block below, plus): merge `work/savelink` with the knob OFF? It now also carries the U-3560 tracker edits.
>
> **Kickoff for savelink2:** the block below's options (a) all-unlocked measurement and (c) the next Save TU group are still open; option (b) U-3560
> is done to the limit of this harness. A natural (non-synthetic) ghost needs a TimeTrial lap under the table float (20 s for id 0x1e), which no
> unattended harness here drives.

> ## START HERE 2026-10-23 savelink (UNMERGED, branch `work/savelink`, HEAD `73799d98e`): **the original accepts the standalone's save, and the exe can write it behind a default-OFF knob.** Next = Mariano's decisions below, then the rest of the Save TUs.
>
> `verify/savelink_20261023/` (PREREGs committed before each run):
> - `RESULT_savelink_roundtrip.md`: P0-P4 held. Using `re/frida/nav_champ_probe.py --plan dumptable` (boots the original to the mode-select
>   menu and dumps the live table `0x007f0a40`; the old `scenario_launch.py` route never loads the save), a save built by the standalone's
>   `BuildImage` gives the same 156 span cells as an edited shipped save, a positive control (r2c1 0 -> 1) moved exactly that cell, and the
>   original's own writer (`0x00404ee0` then `0x00404f50`, `--plan savewrite`) produces a file `ParseImage` accepts. Span, counter and magic only.
>   The profile block (`U-3560`) is not covered.
> - `RESULT_savelink_write.md`: K0-K4 held. `MASHED_SAVE_ORIGINAL=1` (default OFF, `Save/GameSave.cpp` + `Race/GameFlow.cpp`) makes the exe write
>   `original/gamesave.bin`, patching the existing image in place (col 4 / col 5 of the tracked rows + counter; the from-zero rebuild would wipe
>   the launch gates, savedata gate and profile). First write leaves `original/gamesave.bin.preexe_bak`. Knob OFF: G-CTRL `bacfc747 / 8dd1c004 /
>   6f9932b6` and B `f9deb9b9 / d3601fcc / 9da9a26a` unchanged on exe `1ed749b6`, lint 8 known NEW=0 (only G-CTRL and B rerun; (b), (e), D2 and the
>   13 tracks were not). Knob ON: self-test 4 of 4 (new G-MERGE), and the original loads the merged file on 156 cells.
> - Tooling added: `re/tools/save_roundtrip.py`, `re/tools/savefmt/savefmt_cli.cpp`, `MASHED_ORIG_DIR` + `--plan savewrite` in `nav_champ_probe.py`.
>
> **Lessons that cost time (also in the RESULT files):**
> - The tool sandbox makes the original (and any game exe) AV at boot (`0xC0000005` at `0x005baf48`, NULL audio object). Run every game launch with
>   the sandbox disabled. It looks exactly like a compat-shim problem; `setup_mashed_compat.ps1` does not help and it drops `DWM8And16BitMitigation`
>   from the exe's layer value.
> - A private root needs its own AppCompat layer (`~ RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP`) on its own `MASHED.exe` path.
> - `GameSave_SelfTest` ends with `std::remove(path)`; under the knob that path is the reference save. It now refuses. Treat any default-OFF self-test
>   that deletes or rewrites its target as a hazard once a knob can point it at `original/`.
> - Bash heredoc Python with backslash-n inside strings failed to match twice; write the patch script to a file with raw strings, or use Edit.
>
> **Decisions for Mariano (nothing below was done):**
> 1. Merge `work/savelink` into `race/first-frame-parity` with the knob OFF? (Merging is what carries this block and the tooling onto the race branch.)
> 2. Flip the default? NOT proposed: every default run would overwrite the diffing reference and leave a backup next to it.
> 3. With the knob ON and the shipped file, the shipped col 4 is `2` on every row, so the standalone's progression loads as all-unlocked
>    (`ApplyProgressFromSpan`). Not measured in a campaign flow. Col 5 (trophy) is the standalone's private value in a cell whose original meaning is
>    `[UNCERTAIN]`; the original loads it, but what it does with it is untested.
>
> **Paste-ready kickoff for the next session:**
>
> ```
> # Kickoff: Mashed D4, Save TUs leg 3 (lane savelink2)
>
> Base: work/savelink at 73799d98e, or race/first-frame-parity if Mariano merged it. Children on claude2 (verify the model id with read_session).
> Game launches need the sandbox disabled (see the lessons above).
>
> ## State
> Round trip and write leg done (RESULT_savelink_roundtrip.md, RESULT_savelink_write.md). Allowlist 8 findings, lint 0 NEW. The 5 exe-linked Save TUs
> (GameSave, GameSaveBuffer, FsOpen, VfsStream, ReplayTimeFormat) are in exe_sources.rsp; 11 more are .asi-only and call MASHED.exe by address.
>
> ## Task (pick with Mariano): 
> a. Measure finding 3: boot the exe with MASHED_SAVE_ORIGINAL=1 and the shipped save in a private root; log "progress loaded: N/N tracks unlocked" and
>    check the cup screens. PREREG first. This decides whether the knob can ever be a default.
> b. U-3560: the profile block (file 0x4..0x24A3F, 150,076 bytes). Everything so far leaves it zero in standalone saves. Worker survey of the writers
>    of *DAT_008a94a8, then a round trip that fills it (the original's own SerializeToBuffer from a live profile).
> c. Link the next Save TU group (SettingsConfig / SettingsCfg / SettingsDialog / SettingsAndIO / GameSaveVFS / RwStream): all C3/C4 .asi-only with
>    address tunnels; plan the closure first (GameSave.cpp's header counts a 12-TU closure for the three file wrappers).
>
> ## Caveats
> Private roots: robocopy original/ to D:/MashedWT/<lane>_root, then set that path's AppCompat layer. Kill only your own PIDs. D2 arms run alone.
> Never write original/gamesave.bin outside a private root (MASHED_SAVE_ORIGINAL points at it).
>
> ## Lane protocol
> Worktree .worktrees/<lane>, MASHED_NO_DEPLOY=1, evidence D:/MashedWT/<lane>_data, runner copied from verify/savelink_20261023/ (run_arms.py, write_leg.py).
> Commits end with Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>. Do not merge: report verdicts, the gate table, decisions.
> ```

> ## START HERE 2026-10-23 dcstream + kfdot: **allowlist 11 -> 8, no knob-free dual-copy entry left.** Next = a decision, or the other D4 items (Save / Audio TU linking).
>
> Two lanes merged into `race/first-frame-parity` (HEAD `116c4f4fe`, local; origin was at `f0cf8fa6b` when this was written, so push is pending):
> - `verify/dcstream_20261022/RESULT_dcstream.md` (merged `6ddf1e1d5`): `0x004cbd30` RwStreamRead and `0x004cc050` RwStreamSkip are one
>   body each in `Render/RwStreamCore.cpp` (both rsp files), knob-free, C2 -> C3. In-process A/B 0/0 (track 0 + Warzone), sweep 116/0/0.
>   The exe's only stream producer (`RwWorldLoad_FromBytes`) has no caller, so that path is dead code on the default build.
> - `verify/kfdot_20261022/RESULT_kfdot.md` (merged `116c4f4fe`): `0x00532b80` is one body in `Track/KfInterpolate.cpp`, both targets.
>   Mariano flipped it 2026-10-23: the Dump sampler's legacy copy and `MASHED_KF_INTERP` are deleted, C2 -> C3. P0-P3 held: every
>   natural Dump call takes the plain-lerp branch, so no gate and none of the 13 tracks moved (draws and frames byte-identical).
> **References unchanged since rpct** (compare against these): G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc /
> 9da9a26a`; (b) 9/9, (e) 3/3 launch 1425.7 / 2052.5 / 2055.0, D2 3/3, 13 tracks 878 rows 0 RED. Final exe `d70eb347`.
> Allowlist `re/tools/dual_copy_allowlist.txt`: 7 lines, 8 findings (`rva-lint`: CROSS-TARGET=3, DUP-IN-TARGET=5, NEW=0).
>
> **What is left in the allowlist is all decision-gated** (none is a mechanical merge):
> - Math pairs that cost D2 or a draw file: `0x004c39b0` (LUT vs exact normalize), `0x004c4680` (ortho, Warzone pontoon vs body
>   orientation), `0x004c4d20` (`RwMatrixRotate` vs `RotateAboutAxis`). Route: knob default OFF + PREREG first, flip only on Mariano's
>   decision (the r7keep / rpct / kfdot pattern). Mariano picks which, if any.
> - Deliberate splits: `0x004282a0`, `0x004177b0` (next AI pair for the AiBankSwitch recipe once its inputs have producers),
>   `0x00410510` (input plumbing and end-block side effects in the race layer).
> - `0x00408ad0`: stays only for the opt-out `MASHED_NO_RACEPCT_SLOT` stand-in.
>
> **Other D4 items (ROADMAP §D4), not started:** link the Save TUs (`Save/`, 16 files, 28 C4, unlinked and RVA-tunneled; the standalone
> must read and write a `gamesave.bin` the original accepts), link the Audio TUs (585 rows hook-shaped, needs a plan before any
> promotion), D1-residue R1-R2 (owed before D5), WS-F data formats, WS-J audio remainder, WS-H C4 verification.
>
> **Paste-ready kickoff for the next session (pick one; the Save lane is the recommended default):**
>
> ```
> # Kickoff: Mashed D4, link the Save TUs (lane savelink)
>
> Base: race/first-frame-parity at the commit carrying this block (dcstream + kfdot merged). Children on claude2 (verify the model id
> with read_session).
>
> ## State
> Default and every reference as in the 2026-10-23 START HERE block above. Allowlist 8 findings, lint 0 NEW.
>
> ## Task: ROADMAP §D4 "Link the Save TUs". Plan first, no code until the plan is read.
> 1. Worker survey (read-only, delegate.ps1 -Repo Mashed): inventory Save/ (files, RVAs, C-levels in hooks.csv, which are
>    RVA-tunneled .asi-only vs linkable into mashed_re.exe), who in the exe calls save/load today, and the gamesave.bin layout notes
>    in re/analysis/. memory save-subsystem-two-paths: GameSaveBuffer.cpp is a separate path, read it first.
> 2. Decide the shape with Mariano: which functions the exe links, what stays hook-shaped, how the standalone proves a gamesave.bin
>    the original accepts (round-trip: standalone writes, original loads, and the reverse).
> 3. Evidence plan before any promotion: path1 where a registry entry exists, in-process A/B on the original where the .asi installs
>    it, exe gates against the references above. Anything that changes what the exe computes is NOT knob-free: knob default OFF +
>    PREREG, flip only on Mariano's decision.
>
> ## Caveats
> - Heredocs eat backslashes, and sed -i can strip CRLF: edit C++ / rsp / runner files from Python with chr(92) for backslashes, and
>   check line endings after.
> - A G-CTRL run can drop the dump rows of one frame (rpct H_DEF-1, frame 5759). Rerun before calling P0 failed.
> - Private roots: robocopy original/ to D:/MashedWT/<lane>_root and <lane>_root13 (exit 1 = copied). D2 arms run alone. Kill only
>   your own PIDs. git merge -m, never -F -. Never apply unlock_* patch scripts to original/MASHED.exe.
>
> ## Lane protocol
> Worktree .worktrees/savelink, branch work/savelink, MASHED_NO_DEPLOY=1, evidence D:/MashedWT/savelink_data (MOVED_OFF_GIT.txt),
> runner copied from verify/kfdot_20261022/ (run_arms.py + vsum.py). Commits end with
> Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>. Do not merge: report per-entry verdicts, the gate table, decisions.
>
> ## Housekeeping
> - The shared original/mashed_re_dev.asi was not redeployed after dcphys, dcphys2, dcleaf, dcmix, r7keep, rpct, dcstream or kfdot;
>   deploy only when Mariano says.
> - Push race/first-frame-parity (origin lags by the dcstream and kfdot merges).
> ```

> ## START HERE 2026-10-22 dcmix + r7keep + rpct: **allowlist 18 -> 11, two default flips, G-CTRL REBASED.** Next = D4 dual-copy burn-down, the render streams (lane `dcstream`).
>
> Three lanes merged into `race/first-frame-parity` (HEAD `b82d25315`):
> - `verify/dcmix_20261022/RESULT_dcmix.md`: `0x0055dec0`, `0x0045baa0` (C2 -> C3), `0x0040e180` (C2 -> C4, U-9788 coverage) one
>   body each; `0x0055e200` and `0x0042d3e0` were false findings (scanner `rva_body_scan.py` now drops declarations, test 8/8).
> - `verify/r7keep_20261022/RESULT_r7keep.md`: rule 7 keeps the score-scan winner (`0x00410790`), **default ON**, opt-out
>   `MASHED_NO_RULE7_KEEP`. Moved no hash (unreachable in natural play: rule 7 never moves a score).
> - `verify/rpct_20261022/RESULT_rpct.md`: `RacePct` / `RaceCamCar::race_pct` load the slot `0x008a96ec` (`0x00408ad0`), **default
>   ON** on P1 (Mariano), opt-out `MASHED_NO_RACEPCT_SLOT`. Round 4 of the G-CTRL race now eliminates car 1 instead of car 3
>   (no original reference exists for that round).
> **New default references (compare against these, not the p2flip trio):** G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 /
> d3601fcc / 9da9a26a`; (b) 9/9, (e) 3/3 launch 1425.7 / 2052.5 / 2055.0, D2 3/3, 13 tracks 878 rows 0 RED (unchanged).
> `MASHED_NO_RACEPCT_SLOT=1` gives the old trio `c3bfcef3 / 4ed4849b / 4c3548a1`.
>
> **Paste-ready kickoff for the next session:**
>
> ```
> # Kickoff: Mashed D4, dual-copy burn-down, the render streams (lane dcstream)
>
> Base: race/first-frame-parity at the commit carrying this block (dcmix + r7keep + rpct merged and pushed).
>
> ## State
> Default G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a (rebased by rpct). Allowlist
> re/tools/dual_copy_allowlist.txt: 11 findings, lint 0 NEW.
>
> ## Task: 0x004cc050 + 0x004cbd30 CROSS-TARGET (exe D3d9Render/RwWorldStream.cpp vs asi Render/TextureLoader_q6.cpp).
> Shape named in the dcmix kickoff: one shared memory-stream core in a TU listed in both rsp files, per-target thunks only under
> #ifdef MASHED_STANDALONE. Static diff first (worker), every claim checked against original/MASHED.exe.unpatched
> (re/tools/disasm_fn.py); scan .text for E8 call sites. If the faithful core changes what the exe loads or draws, it is NOT
> knob-free: knob default OFF + PREREG first (the r7keep / rpct pattern), flip only on Mariano's decision.
> Evidence: path1 where a registry entry exists (early_window_leaf_diff.py for arg types run_diff.py refuses), in-process A/B on
> the original where the .asi installs it (MASHED_LEAF_SELFTEST pattern, trampoline re-running the clobbered prologue), exe
> gates against the NEW references above, 13 tracks (render: the draw files are the sharp witness here).
>
> Left for decisions (report only):
> - Math pairs with a D2 / draw-file cost: 0x004c39b0 (exe+asi), 0x004c4680, 0x00532b80, 0x004c4d20. Each one now has a proven
>   route (knob default OFF + PREREG, as r7keep / rpct did); Mariano picks which, if any.
> - Deliberate splits: 0x004282a0, 0x004177b0. 0x00410510: the exe body now has the original's rule-7 law; the rest of the split
>   is input plumbing (live callees vs the host snapshot) and the end-block side effects in the race layer.
> - 0x00408ad0 stays allowlisted only for the opt-out MASHED_NO_RACEPCT_SLOT's stand-in.
>
> ## Caveats (from dcmix / r7keep / rpct)
> - Heredocs eat backslashes, and sed -i can strip CRLF: edit C++ / rsp / runner files from Python with chr(92) for backslashes,
>   and check line endings after (a backslash-n in a format string became a real newline in r7keep; two rsp lines lost their backslash).
> - New run_arms.py stacks: grep the name first. 'S', 'NOK' already exist; overwriting one breaks a later stack built on it.
> - A G-CTRL run can drop the dump rows of one frame (rpct H_DEF-1, frame 5759: rows missing, sim identical, ptrace equal).
>   Rerun before calling P0 failed.
> - Round-level outcomes do not repeat on the original and its rounds end early in recipe F: a per-round victim cannot be scored
>   against an original capture (memories round-level-outcomes-do-not-reproduce, o-rg4-recipe-round-ends-early).
> - Private roots: robocopy original/ to D:/MashedWT/<lane>_root and <lane>_root13 (exit 1 = copied). run_arms.py does not
>   create its output dir. D2 arms run alone. Kill only your own PIDs. git merge -m, never -F -.
>
> ## Lane protocol
> Worktree .worktrees/dcstream, branch work/dcstream, MASHED_NO_DEPLOY=1, evidence D:/MashedWT/dcstream_data (MOVED_OFF_GIT.txt),
> runner copied from verify/rpct_20261022/run_arms.py (+ vsum.py). Commits end with
> Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>. Do not merge: report per-entry verdicts, the gate table, decisions.
>
> ## Housekeeping
> - The shared original/mashed_re_dev.asi was not redeployed after dcphys, dcphys2, dcleaf, dcmix, r7keep or rpct; deploy only when
>   Mariano says.
> ```

> ## START HERE 2026-10-22 dcleaf: **vehicle-side dual copies: none left.** Next = D4 dual-copy burn-down, the mixed residue (lane `dcmix`).
>
> `verify/dcleaf_20261022/RESULT_dcleaf.md` (merged `2c097364f`, pushed `a5e5cf885`). `0x0046dbe0`, `0x00468b40`, `0x00442c80` +
> `0x00442ce0`, `0x0046b1c0` are one body each in both targets; `0x0040e350` was a false anchor. `0x00442ce0` C2 -> C4 (Mariano accepted,
> U-9787 open: tier branches seeded only), `0x0046b1c0` C3 -> C4. Exe gates knob-free, every reference unchanged.
> **Default and references unchanged from dcphys2 / p2flip** (G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1`; B `4985eb3b / b6e3d073 / 6717c15e`,
> (b) 9/9; (e) 3/3 launch 1425.7 / 2052.5 / 2055.0; D2 3/3; 13 tracks 39 frames 878 rows 0 RED). Score files in `verify/dcleaf_20261022/`.
> Allowlist 18 lines / 18 findings, lint 0 NEW. Honest measure (`hooks.csv` C3/C4 with `exe_file != file`): 160 (28 with a .cpp `file`).
>
> **Paste-ready kickoff for the next session:**
>
> ```
> # Kickoff: Mashed D4, dual-copy burn-down, the mixed residue (lane dcmix)
>
> Base: race/first-frame-parity at a5e5cf885 (merged + pushed dcleaf). No worktree left from dcleaf.
> Children on claude2 (verify the model id with read_session).
>
> ## State
> Default and every reference as in RESULT_dcleaf.md section 3 (unchanged since p2flip). Allowlist re/tools/dual_copy_allowlist.txt:
> 18 lines = 18 findings, lint 0 NEW.
>
> ## Task: the knob-free part of what is left (ROADMAP §D4 item (4)). Easy first.
> 1. 0x0055dec0 DUP-IN-TARGET [exe] + [asi]: Collision/RwpBuildExterns.cpp:67 FUN_0055dec0 vs Util/UtilLeaves_ab6.cpp:233
>    UtilDeref55dec0 (audit=UNREVIEWED). Never compared body to body. 2 findings.
> 2. 0x0055e200 DUP-IN-TARGET [exe] + [asi]: Collision/RwpIntegrator.cpp:422 RwpSolverContextSet_impl vs
>    Collision/RwpSolverCore23.cpp:140 RwpSolverContextSet (not in the audit). 2 findings.
> 3. 0x0040e180 CROSS-TARGET: exe Race/RaceCamera.cpp vs asi Race/CameraClusterHooks.cpp (audit=DIFFERS-COSMETIC).
> 4. 0x0042d3e0 DUP-IN-TARGET [exe]: Frontend/MenuInit.cpp:73 MenuEntryArrayInit vs Frontend/MenuNavSM.cpp:386 RecordsZero
>    (DIFFERS-BEHAVIOUR). Memory store-leaf-c3-needs-the-disassembly-store-list: compare against the full store list, 14 stores.
> 5. 0x00408ad0 DUP-IN-TARGET [exe]: D3d9Render/TrackRenderer.cpp:4007 TrackRenderer::RacePct vs
>    Frontend/SmallLeaves_t1.cpp:42 RaceScoreFloatGetBySlot (DIFFERS-BEHAVIOUR).
> 6. 0x00410510 CROSS-TARGET: exe Race/RuleEngine.cpp vs asi Race/ScoringHooks.cpp (DIFFERS-BEHAVIOUR; the rule engine
>    feeds the race-metric consumer, memory race-metric-consumer-is-rule-engine-not-fd0).
> 7. 0x0045baa0 CROSS-TARGET: exe Powerup/PowerupSystem.cpp vs asi Util/PromoLoop_sessionB.cpp (DIFFERS-BEHAVIOUR).
> Any pair whose faithful body changes what the exe computes is NOT knob-free: knob default OFF + PREREG first, flip only
> on Mariano's decision. Report it and move on rather than forcing it into this lane.
>
> Out of this lane (report only, each needs a decision or its own lane):
> - DELIBERATE splits: 0x004282a0 (MenuMenusB naked copy vs exe MeasureMenuMessage), 0x004177b0 (AiPreTick vs RuleEngine
>   finish-order fragment; next AI pair for the AiBankSwitch recipe).
> - Render streams: 0x004cc050, 0x004cbd30 (RwWorldStream vs TextureLoader_q6: shared memory-stream core + per-target thunks).
> - Math pairs with a D2 / draw-file cost: 0x004c39b0 (LUT vs exact normalize, D2), 0x004c4680 (ortho, D2), 0x00532b80
>   (KfInterpolate dot order, moves the Dump draw files), 0x004c4d20 (RwMatrixRotate vs RaceCamera RotateAboutAxis;
>   memories rw-matrix-rotate-pads-are-stack-garbage, standalone-fastinvsqrt-is-exact-not-lut).
>
> Per entry (dcleaf recipe): worker static diff of the two bodies -> check every claim against original/MASHED.exe.unpatched
> (re/tools/disasm_fn.py; association from the FADD order; scan .text for E8 call sites) -> one body in a TU listed in
> both rsp files where both targets need it, target thunks only under #ifdef MASHED_STANDALONE -> evidence: path1
> (hooks_registry.py) and, where the .asi installs it, an in-process A/B on the original (MASHED_LEAF_SELFTEST pattern in
> Vehicle/ForceIntegratorLeaves.cpp: trampoline re-running the clobbered prologue, original twice + shared body, every
> mismatch logged) -> exe gates knob-free (every reference unchanged) -> delete the duplicate, drop the key, re-classify,
> lint 0 NEW, scripts/test_rva_body_scan.py passes.
>
> Read first: verify/dcleaf_20261022/RESULT_dcleaf.md, ROADMAP §D4, re/CONFIDENCE.md "Which copy the evidence covers",
> memories inline-asm-call-hides-xmm-clobber, sim-trajectory-is-code-layout-sensitive, exe-stub-arrays-shadow-live-globals.
>
> ## Caveats (from dcleaf)
> - Private roots: robocopy original/ to D:/MashedWT/dcmix_root/original and D:/MashedWT/dcmix_root13/original (13-track
>   replay runs in parallel from its own root). From PowerShell; robocopy exit 1 = copied.
> - run_arms.py does not create its output dir: mkdir it first (the first arm otherwise dies after the race).
> - path2 (run_verify_hook.py) from a worktree maps the SHARED deployed .asi first; use the private-root in-process run
>   (scenario_launch.py --hooks, MASHED_ROOT) as the install evidence instead.
> - The scanner's second pass binds `// 0x<rva> Name` / `// FUN_<rva> Name` header comments to ANY definition of Name in
>   the TU: a call-through helper with such a comment is a false finding (0x0040e350).
> - Bash heredocs eat backslashes (a \r\n in a C string literal became a raw CR LF): write scripts with the Write tool.
> - log/ is gitignored but some files in it are tracked: stage them with git add -u.
> - The main checkout's .git/index.lock is taken by another process now and then: wait and retry, never delete it.
> - Rebuild = new exe hash (link timestamp). D2 arms run alone. Kill only your own PIDs. git merge -m, never -F -.
>
> ## Lane protocol
> Worktree .worktrees/dcmix, branch work/dcmix, MASHED_NO_DEPLOY=1, evidence D:/MashedWT/dcmix_data (MOVED_OFF_GIT.txt), runner
> copied from verify/dcleaf_20261022/run_arms.py (+ ab_leaf.sh, vsum.py). PREREG before any knob arm; new knobs default OFF;
> flips only on Mariano's decision. Commit the final default's score files. Commits end with
> Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>. Do not merge: report per-entry verdicts, the gate table, decisions.
>
> ## Housekeeping
> - The shared original/mashed_re_dev.asi was not redeployed after dcphys, dcphys2 or dcleaf; deploy only when Mariano says.
> ```

> ## START HERE 2026-10-22 dcphys2 + p2flip: **physics A-chain dual copies: none left.** Next = D4 dual-copy burn-down, vehicle residue (lane `dcleaf`).
>
> `verify/dcphys2_20261022/RESULT_dcphys2.md` (merged `574716565`). A6b `0x00468980`, A5 `0x0046ddb0`, A4 `0x00470670` are one TU
> each in both targets, all C4; stack `P2` default ON (Mariano's decision).
>
> **Default now:** FAITH stack + u9774 five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` + `MODE_LIVE` + `SLOT_TYPE_LIVE` +
> `SUBSTEP_OUTER` + `A6A_DOT_ORDER` + `P2` (`A6B_ACOS_CRT A6B_NORM_LUT A5_RING_ORDER A5_DOT_ORDER A5_AIR_F32 A5_CONTACT_INT A5_SPIN_FILD
> A4_DOT_ORDER`), `Core/FaithDefaults.cpp`. Recipe F: `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`.
> **References** (score files in `verify/dcphys2_20261022/`): G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1` (opt-out the eight `MASHED_NO_*`
> -> `23ac657a / 7bf2f27f / 3186dd09`; NO_FAITH `997f4117 / e6288905 / a63bdcfa`); B K0/K1/K26 `4985eb3b / b6e3d073 / 6717c15e`, (b) 9/9
> (`vsum_b_def.txt`); (e) 3/3 launch 1425.7 / 2052.5 / 2055.0, ft_median_m0 2551.6 / 2052.5 / 2278.1 (`e_check_def.txt`); D2 3/3
> (`score_d2_def.txt`); 13 tracks 39 frames 878 rows 0 RED (`tracks_def_score.txt`).
> Allowlist 22 entries / 23 findings, lint 0 NEW. Honest measure (`hooks.csv` C3/C4 with `exe_file != file`): 164 (31 with a .cpp `file`).
>
> **Paste-ready kickoff for the next session:**
>
> ```
> # Kickoff: Mashed D4, dual-copy burn-down, vehicle residue (lane dcleaf)
>
> Base: race/first-frame-parity at 606db1dfd (merged dcphys2). Worktree .worktrees/dcphys2 is merged; remove it only with
> py -3.12 scripts/diag.py wt-remove .worktrees/dcphys2. Children on claude2 (verify the model id with read_session).
>
> ## Task: the vehicle-side entries left on re/tools/dual_copy_allowlist.txt (ROADMAP §D4 item (4))
> Easy first, then the two with inputs:
> 1. 0x0046dbe0 DUP-IN-TARGET [exe]: Vehicle/ForceIntegratorLeaves.cpp CarContactCount vs Vehicle/VehicleState.cpp
>    VehicleRacePositionGet (C4, audit IDENTICAL). Keep one body, call it from the other site, drop the key.
> 2. 0x00468b40 DUP-IN-TARGET [exe]: Collision/CarWorldContacts.cpp ContactHistoryLookup vs Vehicle/VehicleState.cpp (C3,
>    audit IDENTICAL). Same.
> 3. 0x00442c80 CROSS-TARGET: asi Util/UtilMid.cpp ModeGatedPlayerCheck (C4, plate file) vs exe ForceIntegratorLeaves.cpp
>    RubberBandGate; and 0x00442ce0 (C2, exe RubberBandGrip; the .asi calls the original). One shared TU each with thunks for
>    the game-mode word (U-9784), DAT_007f0fd0 and the refdist 0x008989b0 / ref car 0x008989c8 (exe: Race/SpectatorDistances.cpp
>    fills them, A5_RUBBER_LIVE).
> 4. 0x0040e350 DUP-IN-TARGET [asi]: Ai/PromoLoop_round1.cpp GetRenderSubMode vs Vehicle/PhysicsChainHooks.cpp:113 (an inline
>    live-call helper; check whether it is a false anchor). The exe's Fi_GameMode stand-in is U-9784: a producer is in scope only
>    if the static read shows it is cheap; otherwise file it and leave the stand-in.
> 5. 0x0046b1c0 CROSS-TARGET (U-9155, deliberate split): asi Vehicle/VehicleSlotAabbExpand.cpp (naked, C3) vs exe
>    Vehicle/VehicleContactHull.cpp. dcphys noted it folds the same way (one TU + record-base thunk); do it last.
>
> Per entry, the dcphys2 recipe: worker static diff (read-only) -> check every claimed difference against
> original/MASHED.exe.unpatched (re/tools/disasm_fn.py; association from the FADD order) -> one shared TU in both rsp files,
> only target thunks under #ifdef MASHED_STANDALONE -> the row's own evidence path (leaves: re/frida/hooks_registry.py path1 +
> path2; record bodies: in-process A/B with full compare + noo + MASHED_PHYS_SELFTEST_CAP=20) -> exe gates (knob-free: every
> reference above unchanged; a behaviour change needs a knob default OFF and a PREREG first) -> delete the duplicate, drop the
> allowlist key, re-classify, lint passes.
>
> Read first: verify/dcphys2_20261022/RESULT_dcphys2.md, ROADMAP §D4, re/CONFIDENCE.md "Which copy the evidence covers",
> memories inline-asm-call-hides-xmm-clobber, rw-matrix-rotate-pads-are-stack-garbage, phys-selftest-quotas-sample-only-the-early-race,
> exe-stub-arrays-shadow-live-globals, sim-trajectory-is-code-layout-sensitive.
>
> ## Caveats (from dcphys2)
> - Private root: robocopy original/ to D:/MashedWT/dcleaf_root/original (original\ INSIDE the root; scenario_launch.py
>   needs <root>/original/MASHED.exe). From PowerShell.
> - Any .asi helper whose __asm block CALLs into MASHED.exe must be __declspec(noinline) (XMM clobber; dcphys2's A4 noo failure).
> - A no-op control (noo) failing means the harness first: check snapshot coverage (record, scratch, PRNG cursor) and inlining.
> - Bash heredocs eat backslashes in Python scripts: write scripts with the Write tool to the scratchpad.
> - The main checkout's .git/index.lock is taken by another process now and then: retry, never delete it.
> - Rebuild = new exe hash (link timestamp). D2 arms run alone. Kill only your own PIDs. git merge -m, never -F -.
>
> ## Lane protocol
> Worktree .worktrees/dcleaf, branch work/dcleaf, MASHED_NO_DEPLOY=1, evidence D:/MashedWT/dcleaf_data (MOVED_OFF_GIT.txt),
> runner copied from verify/dcphys2_20261022/run_arms.py (+ ab.sh, selftest_sum.py). PREREG before any knob arm; new knobs
> default OFF; flips only on Mariano's decision. Commit the final default's score files. Commits end with
> Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>. Do not merge: report per-entry verdicts, the gate table, decisions.
>
> ## Housekeeping
> - The shared original/mashed_re_dev.asi was not redeployed after dcphys or dcphys2; deploy from the main checkout only when
>   Mariano says. race/first-frame-parity is not pushed past 72ec8a7d0.
> ```

> ## START HERE 2026-10-21 d3close: **D3 is CLOSED** (user decision, Mariano). Next = D4.
>
> `verify/d3close_20261021/RESULT_d3close.md`; the closure block and the carried-forward list are at the top of `ROADMAP.md` §D3.
> Every D3 criterion re-scored on the default: AI (b) 9/9 (A 3/3, B 3/3, C 3/3), (e) 3/3 equal to the reference at 0.1,
> powerups decision CLEAN 11/11 (g3 R_FLAME contact residue named), modes oracle GREEN on all 11 rules, D2 3/3.
>
> **Default now:** FAITH stack + u9774 five + `WCS_TRINORMAL` + `WCS_COMBINE_PC24` + `A5_RUBBER_LIVE` + `MODE_LIVE` +
> `SLOT_TYPE_LIVE` + `SUBSTEP_OUTER` + `A6A_DOT_ORDER` (`Core/FaithDefaults.cpp`). **Canonical recipe F:** `MASHED_ROUND_RULE=0 MASHED_GAME_MODE=10`
> (older lane runners still set rule 4 on purpose so they reproduce their own results). **G-CTRL `23ac657a / 7bf2f27f / 3186dd09`**,
> B K0/K1/K26 `0e3790c4 / 55b84dd8 / 2324715b`, 13 tracks 875 rows 0 RED (dotflip, `work/dcphys`, 2026-10-22; before it
> `f78de789 / 43bbe0fb / 161b1a6a`, B `9bd86ffe / 433abcd2 / 1a1ff727`, 877 rows = `MASHED_NO_A6A_DOT_ORDER=1`).
> Opt-outs and older trios: memory `default-ctrl-hashes-after-b2-flip.md`.
>
> **dcphys (D4 item (1), `verify/dcphys_20261021/RESULT_dcphys.md`, branch `work/dcphys`, not merged):** A6a `0x00467650` and
> A3 `0x0046b540` are one TU each in both targets (allowlist 24 -> 22), both C4; `MASHED_A6A_DOT_ORDER` default ON. Next in the
> same item: A6b `0x00468980`, A5 `0x0046ddb0`, A4 `0x00470670` (`exe_file != file`, not on the allowlist). Open: U-9783, U-9784.
>
> The 2026-10-20..21 lanes, in order: u9777 (A5 rubber-band gate read a zero stand-in) -> rbflip; u9778 (FUN_004709a0 steps all cars per
> substep; fiber form) -> substep (faithful `SUBSTEP_OUTER` restructure of StepCar) -> soflip (default ON, fiber deleted); u9779 (round end at
> call 220; `ELIM_RESOLVE` / `ROUND_END7` / `RACE_TAIL_LATE` default OFF); brecipe -> fflip (recipe F, live mode / rule / slot-type words);
> d3close. Each has `verify/<lane>_2026102x/RESULT_*.md`.
>
> **D4 (ROADMAP §D4):** link the Audio TUs, link the Save TUs (`gamesave.bin` the original accepts), burn the dual-copy allowlist
> (`re/tools/dual_copy_allowlist.txt`, physics A-chain first), D1-residue R1-R2 (owed before D5). Carried D3 items (non-gating):
> U-9779 / U-9781, U-9782, U-9780, U-9775, snapshot / restore after relocation, box-state producers, R_FLAME `FUN_0045ac40`,
> unreached modes exits, D-11078 / D-11079.

> ## START HERE 2026-10-17 dflip: three user-decided defaults are ON. `verify/dflip_20261017/RESULT_dflip.md`, branch `work/dflip`.
>
> Steer-0 heading assist FUN_0046c7d0 (AI slots) default ON (`MASHED_NO_HEADING_ASSIST=1`), Warzone pontoon CARRY+STATE+ANIM default ON
> (`MASHED_NO_PONTOON_CARRY` / `_STATE` / `_ANIM`), draw-list I-NAME counts a `#WxH:<fmt>` raster drawn on both sides of a row as named.
> **New default G-CTRL `4c81db89 / aa03f7ae / 7d3af534`**; `MASHED_NO_HEADING_ASSIST=1` gives `d3b90ee0 / 80f01f35 / 56d62ae7`.
> 13 tracks 870 rows / 0 RED / I-NAME PASS (+3 C27 car-part rows from the assist's AI wheel state). D2 3/3 in bounds. D3 (b) A 1/3, B 2/3,
> C 0/3, no passing car lost. `MASHED_AI_LUT=5` stays OFF (it fails A car 2). Open: U-9743 (the assist's player form, a D2 input),
> D-11078 (U-9730) and D-11079 (U-9741) deferred by the user.

> ## START HERE 2026-10-16 d2close: **D2 is CLOSED** (user decision, Mariano). Next = D3.
>
> `verify/d2close_20261016/RESULT_d2close.md`, branch `work/d2close` (`b61e14a49` + `f3163e6a6`). Fixes W (BSP sphere walk `FUN_00538c80` + collector `FUN_00468b80`) and K (classifier z-first
> summation) are default ON, the port-only `ProduceTerrainBatch` stand-in is deleted, L has been default since d2bsp. Plain default, 3 runs, unchanged `d81a8df6` bounds: slip 0.1890 x3 /
> 0.2483 0.2482 0.2482, driving-median 1914.84 1914.84 1913.06, launch L=0 at 0.19 %, H1 398/400. **New default G-CTRL `d3b90ee0 / 80f01f35 / 56d62ae7`**; opt-outs in the RESULT
> (`MASHED_NO_D2CHUNK`, `MASHED_NO_D2CLS`; W has none after `f3163e6a6`, `MASHED_NO_D2BSP=1` exists only in builds of `b61e14a49` and earlier). Any older trio needs these opt-outs on top of its own.
> D3 (b) per regime on this default: **A 1/3, B 2/3, C 0/3**, unchanged by d2close. Collateral 39 of 94, unchanged.
>
> **What remains for D3** (nothing here is a D2 gate any more; a regression of the three D2 metrics reopens D2, ROADMAP.md §D2 CLOSED block):
> 1. (b) is the open D3-AI criterion: regime A cars 1 and 3 (mutual contacts at steps 64 / 130 the original never has, U-9185 / U-9186), regime B car 2 (c1# / steer#), regime C 0/3
>    (the 2026-10-13 and 2026-10-14 blocks below list the candidates: mutual contacts, the post-contact AI brake U-9684, car 2 refdist-0 entries U-9685 / U-9683).
> 2. U-9684 stays blocked on the parked player at rest (progress `[0x008a96e8]` 0.0 in the exe, 29.999977 in the original; the first-step settle). It is a player-physics change (the 2026-10-14 block below
>    asks for its own row), so re-score the three D2 metrics as its collateral.
> 3. U-9712 (countdown-hold solver-entry wheel fields, port side not captured) is the one open item the closure accepted. Its path: a port-side dump beside `MASHED_BATCH_DUMP`, compare frames < 1000.
> 4. The walk is compared with the original on Training only (G-WALK 4764/4764). A second-track `--batch-probe` capture of the original would extend that; only replica build and non-empty batches are
>    checked on the other 12 tracks.

> ## START HERE 2026-10-14 U-9684 (post-contact AI brake): producer found, kept default-OFF, **next = user decisions below.**
>
> `verify/d3_u9684_20261013/RESULT_U9684.md` (merged 88cacb0c, evidence only, no code change). The brake is FUN_00414a70
> returning 1 -> FUN_00416250 mode 3 (0x00416394) -> band rules (c1/c4/c5 = 255 at 0x00416863..0x00416885, c0 ramp
> 0x00416738..0x00416758). It is ALREADY ported (Ai/AiTargetClosest.cpp) behind MASHED_TGT3 + MASHED_PATHPROG, default OFF.
> G-SRC 16/16, G-REPLAY 5490/5490 exact, but G-LIVE FAILS: the exe's car 2 targets the player from its 2nd grid call and
> never makes the contact, because the parked player's progress [0x008a96e8] is 0.0 in the exe and 29.999977 in the
> original (the original's player settles +0.0000921 z on its first step, the exe's never moves; counterfactual: swapping
> only player progress flips 790/791 calls). With TGT on, (b) gets worse (A 0/3, B 0/3) and collateral 41/97 vs 39/94.
> Default unchanged (G-CTRL 991a5fff / be0d47ea / bc75b405).
> Decisions: open a row for the player at rest (first-step settle, +0x9e4 reads 0.638 at rest vs 0.0); it is a
> player-physics change on the D2 watch list. Do not run more U-9684 arms until that is fixed. MASHED_ROUND_GRID fixes
> the round-1 grid bit-exactly but not car 2's early targeting.

> ## START HERE 2026-10-13 D3 results (items 1+2 run): **decisions taken; car-2 opening-pickup miss fixed and merged; next = the candidates in the last bullet.**
>
> - **U-9219 premise REFUTED** (`verify/d3_u9219_20261013/RESULT_U9219.md`, merged 2026-10-13, no code-behaviour
>   change). The port's 0x00442a60 matches the original on its own inputs (220/220 selections, 880/880 refdist). What
>   keeps the original's car 2 out of mode 7 is the held power-up word: FUN_00414490 refuses mode 7 once
>   `(&DAT_0088fc88)[v*0x2d]` != 0, and the original's car 2 picks up at driving call 27 every run while the exe's
>   holds nothing until call 1742 (the known car-2 opening-pickup miss). Car 3's 43 entries are after the original's
>   round ended. Found, not fixed: the port computes refdist before the AI tick, the original's AI reads the previous
>   tick's value (TrackRenderer.cpp:5836).
> - **(b) per regime, default build** (`verify/d3_regimes_20261013/RESULT_REGIMES.md`): A 2/3 (car 2 abs_steer),
>   B 1/3 (cars 1, 2 fail), C 0/3. Labelling: first non-zero DAT_0089a368 in the car's 220-call window (none=A, 1=B,
>   2=C; FUN_004177b0). The original's noise inside a regime is zero on every metric. In regime C the original's cars
>   1/2 get only 196 calls in 4/4 runs, so C at 220 can score car 3 only. U-9204's "flag 2 inert" is out of date.
>   Raw .msd binaries were lost with the worktree (gitignored), the per-run aistep CSVs, labels and provenance are committed.
> - **Decisions taken (Mariano, 2026-10-13), applied:** U-9219 RESOLVED (refuted premise). New U-9682 = the car-2
>   opening-pickup miss (the mode-7 carrier for car 2). New U-9683 = the one-tick refdist ordering, not tested (user did
>   not request it). B envelope adopted into re/tools/b_envelopes.json (RESULT_REGIMES §4 command). Regime C is scored
>   at 196 calls PER CAR (A/B stay at 220, cars pooled; `ai_ctrl_window.py` REGIME_N / REGIME_PER_CAR). U-9204 rewritten.
> - **(b) per regime, default build, decided rule** (`verify/d3_regimes_20261013/SCORE_REGIMES_C196.txt`):
>   **A 2/3, B 1/3, C 0/3.** A car 2 fails abs_steer~ (0 vs 7..23). B car 1 fails c1#/c1~/abs_steer~ (57/49/49),
>   car 2 fails c0#/c1#/steer# (43/47/89). C at 196 per car: car 1 fails 5 items, car 2 4, car 3 5 (envelopes width 0).
>   A and B verdicts are unchanged by the C rule. All 31 original runs PASS their own envelopes (93/93 car-windows).
> - **Car-2 opening-pickup miss FIXED, merged default ON (user decision), U-9682 RESOLVED** (`verify/d3_car2pickup_20261013/RESULT_CAR2PICKUP.md`). Two port
>   defects in VehiclePhysicsRun.cpp: the adapter IN erased the other car's FUN_00469df0 impulse, and the +0xaec A4 coast
>   gate (FUN_00470c70 0x0047101b..0x00471048) was unported. Opt-outs MASHED_NO_RECORD_VEL_KEEP / MASHED_NO_A4_CONTACT_GATE.
>   **New default G-CTRL `991a5fff / be0d47ea / bc75b405`.** **(b) default build now A 1/3, B 2/3, C 0/3** (A cars 1/3
>   fail abs_steer~, B car 2 fails c1#/steer#). Residues: car 2 collects 2 ticks early (5 on the mode-7 stack).
> - **Next candidates:** (1) the regime-A car-1 / car-3 mutual contacts at steps 64 / 130 that the original never has
>   (they cost A its third car, cause upstream in the car 1 / car 3 paths, U-9185 / U-9186); (2) the post-contact AI
>   brake, `c1` 255 x16 in the original, absent in the exe (U-9684); (3) car 2's two refdist-0 mode-7 entries at
>   frames 16-17, before the contact (U-9685: grid pose `MASHED_ROUND_GRID` or the U-9683 ordering).

> ## START HERE 2026-10-13 D3 survey (back to the roadmap): superseded by the results block above.
>
> Worker survey (account2, claude-opus-5-5). D3 powerups and modes thirds are met. The AI third is open only on
> criterion (b), the ctrl-byte bands per regime (ROADMAP.md:2177-2235, amendment :2190): default build regime A 2/3,
> car 2 fails (`abs_steer` median 0, `verify/d3_b2flip_20261009/RESULT_B2FLIP.md`), regime B unscored (no labelled
> original run), regime C 0/3 on one reference run. Car-1 residue D3-R1 owed before D5.
> Top 3 unblocked:
> 1. U-9219: the mode-7 refdist bridge gives car 2 refdist 0 on 74/201 frames (original 0/890). Capture the original's
>    per-car refdist (`--c30-probe` refdist column), fix the bridge or port FUN_00442a60, re-score mode 7 and (b).
>    `verify/d3_mode7_m4_20261010/RESULT_M4.md` §3.1.
> 2. A labelled original regime-B run and a second regime-C run (extend `capture_breg.sh`, then the `MASHED_RING_SKIP`
>    sweep). Without them (b) cannot pass in 2 of 3 regimes.
> 3. Round-grid re-classify row + C3 leg for 0x00409290 (shared-TU body, run_diff, path2).
>    `verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md:110-118`.
> Pending user decisions: MASHED_ROUND_GRID (+_ORDER) default-ON (needs PATHPROG, which alone drops (b) to 0/3),
> MASHED_AILEAF stack or default-ON, mode-7 knob defaults, G-EVQ D2-reopen candidate, the non-D3 bulk lane.

> ## START HERE 2026-10-13 (standalone screen-19 settings widget GREEN): **next = the follow-ups below.**
>
> - No stale copies in the standalone: 0x00428610 is built into the exe (exe_file set), 0x00427ad0 / 0x00430b90 are
>   .asi-only and the exe draws those rows with its own code.
> - Screen-19 draw-list diff vs the original: widget RED 14 -> 0 (strict, default, 0.01 px, both original captures).
>   Bar box at rowY-7, fill gate from 0x005cc320, insults arrow by the original formula with a new exe
>   `MeasureMenuMessage` (0x004282a0 twin, bit-equal 9/9), value text x 376 / scale 0.7. Screen 30 GREEN as a side effect.
> - New knob `MASHED_SETTINGS_DEFAULTS=1` (default OFF): ignore `mashed_re_settings.bin` for parity runs, never saves.
>   `parity_tooling.md` now says `--scale-b 1`.
> - Notes: `re/analysis/negctl/session_20261012b/scr19_{parity,fix,arrow}_RESULT.md`, evidence `verify/scr19_parity_20261012/`.
>
> - Text renderer: the +0.055*cell_h glyph y bias is gone (original law confirmed on 33 screens), plus six per-site
>   text fixes (prompt shadow 3.0, scr6/7 cup names, scr15 tabs, scr18/24 rows). All measured sites dy 0.

> **Open**
> - DONE 2026-10-13 (text2): scr18/24 label/value x, value arrows, plates; scr4/15/16 text sites. Left: scr18/24 arrow
>   draw order and scr4 row count DONE (text3). Left: U-9681 contcfg slot merge (low), scr18/24 frames DONE (text4, s24 174/174). Frontend parity is paused: next session returns to D3 (see the D3 survey below).
> - DONE 2026-10-13: screen-32 toggle arrow + value text ported from FUN_00431240 (widget GREEN at 0.01 px). The parity knob
>   now pins autosave Off like the original. Left: scr32 value text 1 px low (y 240.0), L fixed at 356 (longer
>   non-English labels would move it), and DECIDED (user 2026-10-13): the shipping autosave default stays On (only the parity knob pins Off).
> - DONE 2026-10-16: U-9680 derived (exe_main.cpp DeriveMenuMeasureDiv, verify/fe_residuals_20261016/u9680/).
> - Fade alpha / early-out in the standalone widget (fix 3, not requested). The exit fade is uncaptured.
> - DONE 2026-10-16: `after_fix/sumdiff.py --map` keys the checker on the LogoOverlayDraw map symbol.
> - DONE 2026-10-16 (no code change): scr32 value text y 240.0 and scr19 insults value text y 285.0 match the original to 0.0000 px on all 71 / 103 glyphs, fresh original captures (verify/fe_residuals_20261016/text/). The earlier 1 px / 284-vs-285 notes predate the 2026-10-13 text-y anchor fix.

> ## START HERE 2026-10-12c (triage B: 8 of 9 re-earned C3)
>
> Four children (bregargs, bnormalise, bworld, bnoreturn), path2 by the parent on the merged deployed build.
> Notes: `re/analysis/negctl/session_20261012b/<item>_RESULT.md`, path2 reports in `re/analysis/negctl/session_20261012b/path2/`.
> - 0x00427840 FontText_UTF16WidenCopy: C2 -> C3, reg_args warm lane, movzx width defect fixed, path2 PASS.
> - 0x00495530 CreateDInputObject: C2 -> C3, IAT stand-in + import_ret, port unchanged, path2 PASS.
> - 0x004b3bb0 FileWriteWrapper_i3: C2 -> C3, callee recorders with seeded returns, port unchanged, path2 PASS.
> - 0x004921d0 DisplayInit: C2 -> C3, 10 callee recorders (deref 'l', outs, stub_outs), port unchanged, path2 PASS.
> - 0x00428610 ViewportScaledRectDraw: C2 -> C3, re-ported as an x87 TU (3 defects), file Frontend/ViewportScaledRectDraw.cpp, path2 PASS.
> - 0x00426dc0 FrontendRaycastForward: C2 -> C3, race_sentinel + live-anchored rays, return type fixed, path2 PASS.
> - 0x004a31b1 CrtExitProcess_j5: C2 -> C3, noreturn import stand-in, CorExitProcess convention fixed, path2 PASS.
> - 0x004a332b CrtExitNoReturn_j5: C2 -> C3, noreturn stand-in + seeded atexit tables, path2 PASS.
> - **0x00493540 thunk_LaunchLangGate: STAYS C2.** path1 GREEN 14/14, but the original is a native 5-byte E9 thunk and the
>   HookSystem dual-install guard (Core/HookSystem.cpp:131) refuses any E9 target, so path2 FAILs by design. U-9679 filed.
> - New instruments: warm-lane reg_args, import_ret stand-in returns, string deref, callee recorders (deref 'l', outs,
>   stub_outs), race_sentinel, {f32_at, add} seeds, observe_raw, noreturn import stand-in, standin_fakes, import_rets,
>   NegCtl_NoReturn, never-freed gate stubs, fixed generic stand-in wrapper.
>
> **Open**
> - DECIDED (user 2026-10-12): U-9679 option (b). 0x00493540 stays C2 by design, no HookSystem change.
> - 0x00404320 rules 5/8/9 arms and the 0x0041c300 caller coverage need forced inputs (not requested).
> - Triage B is fully drained except 0x00493540.

> ## START HERE 2026-10-12b (U-9675, C4 x7, crash 0xe66db, U-9676, draw-stream ALL DONE): **next = the two user decisions below, then the remaining triage B rows.**
>
> Five children in worktrees, merged and removed; path2 by the parent on the merged deployed build. Notes:
> `re/analysis/negctl/session_20261012b/<item>_RESULT.md`. Trackers: 19766af5 (C4 162 -> 165, C2 4702 -> 4699).
> - **U-9675 RESOLVED**: 0x0041bc50 re-ported from the bytes (29 pairs in byte order, field call not vtable, plus a
>   missing pop). C2 -> C3, run_diff / self-baseline GREEN 11/11, old port RED 11/11 x2 boots, path2 PASS.
>   HudDispatchSlot2 now calls the port (port moved to HudDispatch.cpp so the exe links).
> - **C4 earned (3)**: 0x00492270 SubsystemInit (boot, spawn-suspended .asi load), 0x0042f8d0 MenuMenusBC (natural
>   Race Results, ~2,860 calls, controls RED on every call), 0x004464c0 CameraEntry::DispatchAll (attract demo, native
>   counter stub, control RED 2/2; covers the .asi copy only). New harness: `re/frida/c4_front_scenario.py` +
>   `c4_front_compare.py`; `c4_hud_scenario.py` rows 4464c0 / 40d440 / 41c2d0.
> - **0x004026d0**: C4 (GREEN 2/2, control RED 2/2). The user cleared passthrough STUBS S-0171..S-0175, and U-0170
>   is resolved (6d575bb9).
> - **Forced input decided by the user 2026-10-12, all four now C4**: 0x004026d0 C4 (6d575bb9, S-0171..S-0175
>   cleared, U-0170 resolved). 0x00404320 PerModeRenderMachine, 0x0040d440 and 0x0041c2d0 C4 on the approved
>   save-copy Silver event 2 route (group H install-copy save, 26 launch-gate dwords, rule 10), c4save_RESULT.md,
>   a56f9f55. Remaining coverage: the rules 5/8/9 arms of 0x00404320 and the 0x0041c300 caller need further forced
>   inputs (open, not approved).
> - **Crash MASHED.exe+0xe66db**: same collision class as U-9677, different file. A VEHICLES piz held by another
>   process makes the DFF load (0x0042a5d0) return 0 into RpClumpForAllAtomics (0x004e66d0). 2/2 with a held vehicle
>   piz, 2/2 0x1509b4 with a held track piz, 0/3 solo. CLAUDE.md re-run bullet now names both signatures.
> - **U-9676 RESOLVED**: the camera pads' last writer is `rep movsd` at 0x004c4cf1 in FUN_004c4a50 (RwMatrixRotate's
>   helper), copying stack slots that nothing writes. New U-9678 (low): does the rw_matrix_rotate diff compare them.
> - **Triage B, draw-stream capture built** (run_diff call_log recorders + string/float deref, float returns, `--x87-cw`,
>   `log/drawstream_<key>.csv`). 0x00427ad0 MenuMenusBB and 0x00430b90 C2 -> C3, path2 PASS. Both ports had real
>   defects (SSE2 rounding in BB; 8 listing mismatches in ProgressBarSetA); both now x87 TUs in `x87_tus.txt`.
>
> **Open**
> - DECIDED (user 2026-10-12): install-copy save edit + Silver event 2 route allowed. 0x00404320 / 0x0040d440 / 0x0041c2d0 C4 (a56f9f55). Rules 5/8/9 of 0x00404320 and the 0x0041c300 caller still need further forced inputs (open, not approved).
> - DECIDED (user 2026-10-12): S-0171..S-0175 cleared, 0x004026d0 C4 (6d575bb9), U-0170 resolved.
> - 9 of the 11 triage B rows left (TRIAGE_2026-10-12.md B list; heavyweights 0x00427840, 0x004b3bb0, 0x00426dc0,
>   0x004921d0, 0x00428610), each needs its own instrument.
> - Side finding, not acted on: hooks.csv classes 0x004b3fc0 as CRT, but it calls RpClumpForAllAtomics with a game callback.

> ## START HERE 2026-10-12 follow-ups (A2 x7, U-9671, U-9673, crash 0x1509b4 ALL DONE): **next = the user calls below, then C4 scenarios for the new C3 rows and U-9675.**
>
> Six children in worktrees, merged and removed; path2 + integration run_diff + self-baseline by the parent on the
> merged deployed build. Notes: `re/analysis/negctl/followup_20261012/<item>_RESULT.md`.
> - **8 rows C2 -> C3** (22c62412), path2 PASS for all 8: 0x00498d20 ReadModeFromCombo_j5 and 0x0042f8d0 MenuMenusBC
>   (ports matched the bytes; new fixtures put a decoy in the stack slot, since the `eax_arg` trampoline also
>   leaves the value on the stack and a stack-reading port passed), 0x00492270 SubsystemInit / 0x004026d0 /
>   0x00404320 PerModeRenderMachine (real port defects fixed from the bytes), 0x0040d440 (unchanged),
>   0x004464c0 CameraEntry::DispatchAll (16- vs 12-byte frame; [UNCERTAIN] callee attribution closed: no callee port
>   involved), 0x0041c2d0 FUN_0041c2d0 (U-9671 re-port; HudDispatchMode10/Mode5 now call the port, GREEN).
> - 0x00498d20 is fixture-only C3: C4 not earnable while skip_selector keeps dialog 0x65 closed (added to U-9674).
> - **U-9673 RESOLVED** (956b9d2a): nav harness 75 OK / 0 FAIL, 30/30 runs, reachability 12/33. First break was
>   b83b01e0, not 2528c8b3. Test-only .bss arena fix.
> - **U-9677 RESOLVED (explained)** (f5878050): the original's 0x005509b4 AV is a track-piz sharing race between two
>   MASHED processes (CreateFileA share 0 at 0x004b6738, unguarded seek at 0x0047b917), NOT the Vehicle setting.
>   5/5 with a concurrent reader, 0/15 without. Standalone avoids it. **Harness rule: that crash signature in an
>   original-side run means another session loaded the same track at the same moment; re-run it.**
> - Harness gained: per-callee `call_log` recorders (scalars_to_scattered_globals, installed inside freeze_main),
>   per-test `dialog`, c4_pb2_scenario crash/barrier/private-root options (env-gated, default off).
> - Lost evidence: the a2-dialog / a2-stubat children named CSVs `diff_*.csv`, which .gitignore drops; they vanished
>   at wt-remove. Committed CSVs are the parent's integration re-runs (`evidence/integ_*`); the stubat witness counts
>   live only in its RESULT note. Name evidence CSVs without a `diff_` prefix (memory diff-csv-evidence-is-gitignored).
>
> **Open**
> - DECIDED (user): do NOT pin the nav reachable set `{0,1,2,3,4,6,8,15,19,30,31,32}`; it is the port's set, not the original's.
> - DECIDED (user): U-9677 mitigation is the re-run rule only; written into CLAUDE.md MASHED process hygiene.
> - U-9675: the disabled 0x0041bc50 port has the same vtable misreading as U-9671 plus flag-order / 28-vs-29-pair
>   errors. Re-port before any install.
> - U-9676 (low): which callee instruction writes the camera entry's RwMatrix pad slots.
> - C4 for the 7 non-dialog new C3 rows needs canonical scenarios (MenuMenusBC is natural on screens 15/16/5).
> - Lead: group R ctl1 crash at MASHED.exe+0xe66db during track load may be the same collision, unexamined.

> ## START HERE 2026-10-12 night (A1 real-C4 pass DONE: 13 of 19 at C4, 6 stay C3): **next = the A2 rows, U-9671, U-9673.**
>
> The 19 triage A1 rows got real canonical C4 scenarios (game-driven calls into the live port, a RED old-defect
> control, original arms run twice for noise; memory install-observe-may-never-execute-the-body):
> - 11 back to C4: Championship::Complete (forced P1-won flag), MenuButtonDetectA/B/C, TimerSlotTickDispatcher
>   (substituted control), FrontendPlayerSlotCheck (save edit), 0x00403160 / 0x0041db80 / 0x004295a0 (save edit),
>   PlayerBlock2Field08Get / Field50Get (natural Team Play Race Results).
> - 2 to C4 with a user-accepted LIVE forced input: PlayerBlock2Field00Get / Field04Get (fields 0 in every natural run).
> - 6 stay C3: 5 settings-dialog rows (skip_selector / skip_controller_dialog keep the dialogs closed, U-9674),
>   GameStateSlotsFill (never called on a reachable route).
> - New scenario harnesses: re/frida/c4_champ_complete_scenario.py, c4_pb2_scenario.py, and the group H/M scripts.
> - Side finding (EXPLAINED 2026-10-12, U-9677: concurrent-session .piz race, not the Vehicle setting): the UNMODIFIED original crashed at MASHED.exe+0x1509b4 at race start after the Vehicle
>   setting 0x0067ea94 changed 0 -> 1 (one stock run, group R exploration).
> - Never use shared %TEMP% filenames for commit messages: another session overwrote $TEMP/m.txt (fixed a3545a59).

> ## START HERE 2026-10-12 late (triage follow-ups DONE): **next = the user calls in U-9672, then the A1 C4 rows.**
>
> - Triage of the 50 rows below their old level: `re/analysis/negctl/reearn_20261011/TRIAGE_2026-10-12.md` (verified).
> - Policy calls settled: config-log C4 grandfathered (U-9655/9656, 503186dd); 5 structurally-blind rows accepted
>   (4 at C3, CrtPreInitLoop at C2; 4d72c503); FsopenSafe C2 by design (U-9662).
> - MenuButtonDetectE back to C4 on a REAL canonical scenario (natural Race Results, 5d382e9c). Lesson: the old
>   boot-to-menu install-observe never ran its body (memory install-observe-may-never-execute-the-body). The 19
>   triage A1 rows (C4->C3) each need the same treatment: caller's screen, game-driven call count, RED control.
> - 4 HUD dispatch rows ported and C3 (22bd72fb). U-9671: the existing FUN_0041c2d0 port would crash if installed.
> - U-9667 RESOLVED (5a058f2d): one MenuEntryWalk body for .asi + standalone, cursor -1 path ported.
>
> **Open**
> - U-9672 user calls DONE: screen 4 now pushes 6 (38a9e0f3); the challenge row runs the original movers
>   FUN_004322c0 / FUN_004323c0, directive removed (c2914196, 7ff381c1). Residuals in U-9672: 8 cup tracks vs rows
>   0..12, refusal sound / _DAT_0067e840, Team Play heading, popup, key repeat, the screen-4 arm side writes.
> - U-9673: nav harness crashes at R2-2 (pre-existing 2528c8b3), reachability pass not running.
> - 7 A2 rows fixable with the existing harness; 11 B rows need new harness (draw-stream capture covers 2).

> ## START HERE 2026-10-12 (negctl re-earn port fixes DONE, follow-ups closed): **next = the open items below.**
>
> 47 rows demoted by the negctl re-check were re-ported from the bytes and re-earned C2->C3 (8 parallel clusters:
> replay, menuinput, audio, menudraw, hud, fstate, bootio, settings, plus PlayerBlock2Field*Get). Each has run_diff +
> self-baseline GREEN, negctl mutants RED, a witness restoring the old defect alone going RED, and path2 PASS.
> Per-cluster write-ups: `re/analysis/negctl/reearn_20261011/fix_<cluster>_RESULT.md`. Last commit 2e6704cf.
>
> **Follow-ups closed the same day:** branches deleted; U-9662 DECIDED (keep fix_fopen, FsopenSafe C2 by design,
> 999000af); U-9660 RESOLVED (MenuEntryGet fixture, install re-enabled, 63b70364); U-9661 RESOLVED (row repointed to
> the live AudioListDrain, dead AudioListDrain2 removed, fc8a5a41); U-9663 RESOLVED (ControllerConfigLoad_j5 back to
> C3, fd426588). New: U-9667 (the standalone runs ItemActionCode for 0x0042ac90; it differs from the bytes).
>
> **Open**
> - DECISION: 0x004a4541 FsopenSafe is fixed but stays C2. The HookSystem guard refuses to install over the
>   `fix_fopen` boot patch, so it never runs (U-9662). Keep the patch, or move its behaviour into the port.
> - 0x004971b0 ControllerConfigLoad_j5 went C3->C2: its read path changed and no fixture reaches it (U-9663).
> - MenuButtonDetectE 0x0042b770 went C4->C3. C4 needs a new canonical-scenario run with the fixed code.
> - Broken fixtures, levels kept: U-9660 (0x0042ac90 MenuEntryGet), U-9661 (0x005ade90 AudioListDrain2).
> - Coverage gaps: U-9664 (WindowMsgPump WaitMessage branch masked by no_focus_pause), U-9665, U-9666.
> - ~26 demoted rows remain that need new harness work or cannot be measured against the patched reference
>   (`decisions/*.tsv`, verdicts NEEDS_HARNESS / not re-earnable).
> - Harness gained this session: build.bat `MASHED_NO_DEPLOY=1`; path2 pointer args, `recorder`, `dialog`, `eax_arg`;
>   run_diff `dialog` + `eax_arg`; diff_template `stub_at` for scattered globals.
> - Path2 transient seen 2/~45 runs: interceptor 0/1 with rel32 OK and the call returning; passes on the next boot.
> - Fleet lesson: each worktree is ~3.4 GB; 8 at once filled C: (memory `worktree-fleet-fills-the-disk`).

> ## START HERE 2026-10-11 (D3 bulk G-AILEAF Phase 2 DONE: port faithful, knob NOT accepted): **next = user decision.**
>
> Read [`aileaf/RESULT_AILEAF.md`](../verify/d3_bulk_20261011/aileaf/RESULT_AILEAF.md) (Phase 2 section).
>
> **What happened with the knob ON**
> - FUN_00414570 returned 1 on 19 / 17,466 calls: car 3, step frames 2021..2039, committed as mode 1.
>   So G-AILEAF-INERT FAILED as registered.
> - AL_ON = AL_ON2 = AL_ON53 (`7018f1b0 / 8e5cdccb / 5589b6f8`), against AL_PP `4e250237 / 22cceec1 / dc72ab09`.
> - The exe's own inputs through the original bytes (G-AILEAF-LIVE): 368/368 exact, including the 19.
>   The arm fires because the exe's player drives above 2500 next to car 3. The original captures behind
>   "0 of 6,696" have a near-parked player.
> - The blank AI factor (U-9653) is not the cause (AL_SEED).
> - (b) unchanged (0/3 on both PATHPROG arms). Collateral: car 3 from frame 2021, car 2 from 3231, car 1 none, D2 watch clean.
> - Trackers recorded with no promotion; U-9652 / U-9653 filed.
>
> **Options**
> 1. DONE (Phase 2b + 2c): with `--statediff-pursuit -1 --pursuit-wait-go --pursuit-boost`, the ORIGINAL returns 1
>    on 105 calls (all 3 AI cars, frames 926..967, straight). Validated 4646/4646 through the original bytes. The exe's
>    19 commits are the original's behaviour in kind (RESULT_AILEAF.md Phase 2c). The counts differ because launch
>    dynamics are upstream.
> 2. User decision (evidence now complete): make MASHED_AILEAF available for stacking, or default-ON?
> 2b. Launch difference (Phase 2d): NOT launch physics. AI launch curves match within 1 unit. The exe's 19 events are
>     car 3 ramming the parked player at the exe's ROUND-2 launch (ptrace sim step 4139, 624 -> 2548). The original
>     recipe never reaches round 2 (freeze ~frame 1500). Owed: a dense undriven original push capture (`o_al_dense`,
>     3 boot failures under 4 concurrent instances).
> 2c. Round 2 on the original (Phase 2e): `--continue-rounds --continue-rounds-states 7` gets 8 rounds. The original
>     restarts each round at the SURVIVOR's progress node (FUN_00410860 -> DAT_0063ba80 0x004108c5 -> FUN_00409290 ->
>     FUN_00408b00); the exe's StartRound (TrackRenderer.cpp:5148) restarts at gates_[0] every round. That is a port gap
>     in the modes third, with a concrete producer chain (needs MASHED_PATHPROG progress).
>     FIX BRIEF written: [`roundgrid/FIX_BRIEF.md`](../verify/d3_bulk_20261011/roundgrid/FIX_BRIEF.md). FUN_00426bc0/bd0 are the
>     LAPDATA Safe_Start_Lines table, which the exe already builds under PATHPROG. Original nodes per round: 0,7,11,14,21,24,28,1
>     (`o_rounds7b`). Next: Stage A (survivor node write + FUN_00409290 verbatim + StartRound anchor) behind MASHED_ROUND_GRID,
>     G-RG-SNAP / G-RG-NODE / G-RG-GRID first. Tracker row still owed.
>     STAGE A DONE (knob OFF): `roundgrid/RESULT_STAGE_A.md`. SNAP 18504/18504, GRID 5 nodes worst 0.0048, NODE 3/3,
>     G-CTRL unchanged. KNOB-ON DONE (`roundgrid/RESULT_KNOB_ON.md`): TRACE / DET / LOCAL pass. Round 2 starts at node 21
>     (not the start line); the exe snap repeats the original's 13 -> 14. (b) moved 0/3 -> 1/3 from the round-1 grid shift
>     alone (<= 0.0236): sensitivity, not a fix.
>     STAGE B PORTED (knob OFF, `roundgrid/stageb/RESULT_STAGE_B_PORT.md`): slot order from the round's score deltas,
>     1296/1296 vs the original bytes, oracle vs live original 7/7. KNOB-ON DONE (`stageb/RESULT_STAGE_B_KNOB_ON.md`):
>     TRACE / DET / LOCAL pass; identical to Stage A through the round-2 restart, first difference 48 steps into round 2.
>     Next: the tracker row (re-classify) and the user's call on default-ON for MASHED_ROUND_GRID (+ _ORDER).
> 3. Back to the PLAN.md ranking. Every other D3 group is producer-blocked. The non-D3 lane is not decided.
>
> Not started: U-9219, the car-2 pickup miss, default flips.

> ## START HERE 2026-10-11 (D3 bulk Phase 0 DONE, plan only): **next = Phase 1 (APPROVED 2026-10-11): P0 + G-EVQ + G-AILEAF per [`PLAN.md`](../verify/d3_bulk_20261011/PLAN.md) §4.**
>
> **Census**
> - New first-hit census: `scenario_launch.py --firsthit-probe` (entry-only, capped, self-detaching), 2 batches.
> - Controls pass: 3 zero, 3 live.
> - 359 C2 rows execute in the Training race, 0 C1 rows. 288 of the 359 have no body.
>
> **Integration facts (PLAN §2.1)**
> - Car records live at `g_vehicleArrayBase`, not 0x008815a0. Absolute reads need the `PtrCompute881ec8` rebind.
> - The original race loop 0x004111c0 / 0x0040fc00 has no exe body, so each group needs a new call site in the hand-written step.
> - The live peek shows 132 of 170 group inputs are written in the original and 0 in the exe.
>
> **Ranked for Phase 1**
> - (1) G-EVQ: 0x004252c0 + 0x00422ba0 event queue. It produces its own ring. It needs producer P0 (`0x007f1030`, cited 0x0040fe24) first. Touches (b) through `0x0089a4fc` and RNG draws.
> - (2) G-AILEAF: 0x00414570 / 0x00415880 / 0x0046d570. Predicted inert. C3 path.
> - Everything else is producer-blocked, D2 watch (0x00470c70), or outside D3.
>
> **Open**
> - Non-D3 render/particle/audio lane: not decided, out of scope.
> - Worker account2 OAuth expired 2026-10-11 (delegate.ps1 down).

> ## START HERE 2026-10-10 (mode 7 M4 DONE: D-11074's four legs are ported): **next = user decision / U-9219.**
>
> Read [`RESULT_M4.md`](../verify/d3_mode7_m4_20261010/RESULT_M4.md). `FUN_00414c30` + `FUN_00414490` run verbatim behind
> `MASHED_MODE7` (on top of the M3 set), replay 10,777/10,777 at PC_24/PC_53. The original's AI reads the previous frame's
> complete publish (`cc` 0 at every call) and the exe reproduces it. Knob ON: car 1 mode 7 on pool 0 from frame 97 (25 vs 49),
> but cars 2/3 also (52/43 vs 0) because the refdist bridge gives them refdist 0 (U-9219); (b) 0/3 under the knob.
>
> **Options for the next session:** (1) U-9219: capture the original's per-car refdist per frame and fix the bridge (or port
> `FUN_00442a60` faithfully), then re-score mode 7 and (b) with the full knob stack; (2) the car-2 opening-pickup miss (0.065)
> from RESULT_M3; (3) decide whether any of `MASHED_REGISTRY / PICKUPS_ORIG / REG_CARS / REG_CALLBACKS / MODE7` goes default-ON
> (user decision; the M3b respawn seed is already default). Kill only your own MASHED PIDs; build via PowerShell.

> ## START HERE 2026-10-10 (mode 7 M3 DONE, + M3b): **next = M4, the mode-7 consumer `FUN_00414c30` + `FUN_00414490`.**
>
> Read [`RESULT_M3.md`](../verify/d3_mode7_m3_20261010/RESULT_M3.md) (prereg `PREREG_M3.md` incl. §5 addendum). With
> `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS + MASHED_REG_CALLBACKS` the exe collects pickups through the verbatim
> `FUN_00458920` (replay 6/6 live + 3,888/3,888 synthetic, every arm); PickupField only draws. M3b: Training respawn is 5.0,
> written by the Lua glue's `FUN_00458fa0` (now called from `InitReal`; C3 body moved to the shared `PickupPoolSpawn.cpp`).
> Knob ON: 10 collects + 12 bumps, respawns 300 ticks, (b) unchanged (2/3), nothing diverges before step frame 1400.
> Corrections: the bump arm is taken when the car ALREADY holds a powerup (slot `+0xa8 != 0`); the exe's car 2 misses the
> opening pickup by 0.065 (1.0983 vs 1.0334). Open: U-9215, U-9217 (original bump still owed), U-9218.
>
> **Kickoff for M4 (paste):** Mashed RE, D3, D-11074 leg M4: the mode-7 consumer. Read `RESULT_M3.md` §5 (findings 4 and 6)
> and `../d3_modes37_20261009/FINDINGS.md` (car 1: 49 mode-7 calls) first. Measure before code: (1) the order of
> `FUN_00485070` (`0x0040fd07`) vs the AI tick inside `FUN_0040fc00`, since `FUN_00414c30` reads the published count `d8`
> while slots `[0, cc)` are being rewritten; (2) what `FUN_00414490` reads at `0x004144b3` (slot `+0xa8`, `0x0088fc88 + v*0xb4`):
> the exe keeps it in `PowerupSystem` and three AI sites read the raw address (`AiControlStep.cpp:208`,
> `AiStandalone.cpp:1207/1327`, all 0 today); decide bind vs mirror with evidence. Pre-register, port verbatim naked
> (PC_24, fixed PcScope), gate on an original-side replay, then score car 1's mode-7 calls vs the original, (b) per regime,
> collateral with the D2 watch list. Kill only your own MASHED PIDs; build via PowerShell.

> ## START HERE 2026-10-10 (mode 7 M2 DONE, M1+M2a+M2b): **next = M3, callbacks live (the pickup collection takes over).**
>
> Read [`RESULT_M2B.md`](../verify/d3_mode7_m2_20261010/RESULT_M2B.md), [`RESULT_M2A.md`](../verify/d3_mode7_m2_20261010/RESULT_M2A.md),
> [`RESULT_M1.md`](../verify/d3_mode7_20261010/RESULT_M1.md). With `MASHED_REGISTRY + MASHED_PICKUPS_ORIG + MASHED_REG_CARS` the exe's
> registry holds the original's 4 car entries (positions bitwise) and 5 pickups (bitwise) every frame; callbacks are recorded,
> not run. All inert (G-CTRL unchanged). Open: U-9215 (grid), U-9217 (spring arm), U-9218 (car radius writer).
>
> **Kickoff for M3 (paste):** Mashed RE, D3, D-11074 leg M3: dispatch the registry callbacks. Read `decomp_registry.txt`
> (`FUN_00458920`) and `Powerup/PowerupSystem.cpp` (the `Activate` host path, the not-ported `FUN_0045c010` / `FUN_0045c330`)
> first. Pre-register before code: which of `FUN_00458920`'s callees port verbatim (`FUN_00405890`, `FUN_00407580`, `FUN_0045c330`,
> `FUN_00458570`, `FUN_004588c0`, `FUN_0045c010`, `FUN_00465ca0`) and which bind to the exe's PowerupSystem; how `PickupField`'s
> own collection stands down under the knob; gates = collection events (frame, car, pickup type) and respawn timing vs an original
> capture (`--reg-probe` callback witnesses already log them), plus G-CTRL knob OFF. Expect knob ON to change the race (powerups now
> collected the original's way): score (b) and collateral as report.

> ## START HERE 2026-10-10 (mode 7 M2a DONE): **next = M2b, the car registrant `FUN_0041f710`.**
>
> Read [`RESULT_M2A.md`](../verify/d3_mode7_m2_20261010/RESULT_M2A.md) and [`PREREG_M2.md`](../verify/d3_mode7_m2_20261010/PREREG_M2.md) §4.
> The pickup tick `FUN_00459000` runs verbatim on the original-layout pool behind `MASHED_PICKUPS_ORIG` (+ `MASHED_REGISTRY`):
> replay 1,890/1,890 exact, and the exe registers the 5 Training pickups bit-identical to the original. Inert. U-9217: the
> spring re-seed arithmetic is unexercised (no bump in 240 s).
>
> **Kickoff for M2b (paste):** Mashed RE, D3, D-11074 leg M2b: the car registrant. `FUN_0041f710` (EAX = `0x0063d9e0 + v*0x2ac`)
> registers `{+0x258, +0x25c, +0x260, +0x264, +0x290 + 0xff00, 0, 0, 0}`; `+0x258..+0x260` come from `FUN_004c0ed0(car frame)+0x30`
> in `FUN_00420870` (called per car from `FUN_0040da50`). Measure before code: extend `--reg-probe` to read, at `FUN_00485070`
> entry, each car record's `+0x958..+0x960` (`0x008815a0 + v*0xd04`) beside the 4 car entries; decide whether the exe's record
> position reproduces the registered one (bitwise or not); census the writer of `+0x264` (0.533 on every frame). Pre-register the
> bridge or port, gate on per-frame car entries vs the original, knob-OFF G-CTRL `ae67b13a / c8317ce2 / 31db9384`. Callbacks stay
> record-only until M3.

> ## START HERE 2026-10-10 (mode 7 M1 DONE, USER DECISION full broker M1..M4): **next = leg M2, the registrants.**
>
> Read [`RESULT_M1.md`](../verify/d3_mode7_20261010/RESULT_M1.md) and [`PREREG_MODE7.md`](../verify/d3_mode7_20261010/PREREG_MODE7.md).
> The original's world-object registry is a per-frame collision broker; pickups are collected through it (`FUN_00458920`).
> M1: registrar / publish / pair passes verbatim behind `MASHED_REGISTRY`, replay byte-exact on 3,162 frames, callbacks
> 3,003/3,003 (2 real pickup collections), inert. Grid path unexercised on Training (U-9215).
> **Also fixed:** every x87 `PcScope` (P1..M1) left the game thread at PC_24 (U-9216, resolved; P2/P3 numbers re-measured, unchanged).
>
> **Kickoff for M2 (paste):** Mashed RE, D3, D-11074 leg M2: the registry's registrants on the exe. Read RESULT_M1.md §1, §5 and
> `decomp_registry.txt` (`FUN_00459000`) first. Pre-register before code: (1) port the logic half of `FUN_00459000` (state 1/2
> timers, respawn, type re-roll `FUN_00458d00`/`FUN_00458dd0`, registration type 7 r 0.5 cb `0x00458920`) against a pickup pool at
> the original layout `0x0068b198` x 25 x 0x50 (`PickupPoolSpawn.cpp` keeps its own copy today); sprite calls `FUN_004769xx` no-op;
> (2) decide the source of the pickup position `*(obj+4)+0x40` (RW frame translation; no exe equivalent) with evidence;
> (3) port the car registrant `FUN_0041f710` (type `0xff00+v`, r 0.533). Gate: per-frame registry contents vs `o_rg4.msd.reg.tsv`
> (counts per type, radii, callbacks; positions on matching frames), knob-OFF G-CTRL `ae67b13a / c8317ce2 / 31db9384`. Callbacks stay
> record-only until M3. Kill only your own MASHED PIDs; build via PowerShell.

> ## START HERE 2026-10-10 (D-11072 P3 body DONE, score FAILS on window alignment): **next = re-score P3 with the countdown on.**
>
> Read [`RESULT_P3.md`](../verify/d3_modes37_20261009/RESULT_P3.md). `FUN_00414a70` + `FUN_00414300` are verbatim naked
> (`Ai/AiTargetClosest.cpp`, spec `target_p3.json`) behind `MASHED_TGT3` (needs `MASHED_PATHPROG`); wired in `ControlStep`
> (mode 3 on 1 + LOS, brake return on 2; branch 2 now gated on `mode == 0` as in the original). G-P3-REPLAY 5,490/5,490 exact
> at PC_24 (new `scenario_launch.py --tgt-probe`, capture `o_tg1`). G-CTRL unchanged. **G-P3-COUNT FAIL** (port 38/30, 27/16,
> 30/82 vs 16/0, 35/10, 12/26): the port's window starts on the grid at frame 0, the original's at 882 (U-9214). (b) regime A 0/3,
> as with PATHPROG alone. Next: re-score with `MASHED_SUBSTATE` + `MASHED_MODE5_RESET` + PATHPROG + TGT3 (pre-register first).
> Open: U-9212 dispatcher speed tail `0x004710c8` (unported, D2 REOPEN CANDIDATE, report), U-9213 `FUN_0046d510` store, mode 7.

> ## START HERE 2026-10-10 (D-11072 P2 DONE): **next = leg P3, `FUN_00414a70` + `FUN_00414300` on the now-live progress.**
>
> Read [`RESULT_P2.md`](../verify/d3_modes37_20261009/RESULT_P2.md). `FUN_00408610` + 17 callees are verbatim naked
> (`re/tools/gen_naked_asm.py`) behind `MASHED_PATHPROG`; replay of 11,996 original calls is byte-identical at x87 PC_24
> (29% at PC_53): **the original's race loop runs at PC_24** (D2 lead, user decision). Knob ON changes the AI windows
> (step `4f675870`) via the faithful race_pct; not scored. P3: port `FUN_00414a70` (+ `FUN_00414300`, the `+0xbc` speed write
> `0x004710c3..c8`) with the same generator where possible, pre-register, score vs the original window counts (car 2: 35
> mode-3 / 10 brake), then (b) per regime + collateral. Open: per-round `FUN_00407a60`, player grid 0.0009 past the line.

> ## START HERE 2026-10-09 (D-11072 faithful route, USER DECISION; leg P1 DONE): **next = leg P2, the per-frame `FUN_00408610`.**
>
> Read [`FINDINGS.md`](../verify/d3_modes37_20261009/FINDINGS.md), [`PREREG_PATHPROG.md`](../verify/d3_modes37_20261009/PREREG_PATHPROG.md),
> [`RESULT_P1.md`](../verify/d3_modes37_20261009/RESULT_P1.md). Mode 3 comes from `FUN_00414a70` (car 2's carrier: 35 mode-3 + 10 brake
> calls), mode 7 from `FUN_00414c30`'s pickup path (car 1 only). `FUN_00414a70` reads per-car path progress `0x008a96e8`, written only by
> `FUN_00408610`. P1 (`MASHED_PATHPROG`, default OFF) now writes the original's course block byte-identical (Training + Arctic).
> P2 to do: `FUN_00408610` + `FUN_00407e20` + walkers + `FUN_00407a90` + the sphere(0.6)-vs-AI-BSP query (node id = material colour)
> + per-car block init; pre-register first. Gate: `0x008a96e8` grid values 30.00/29.73/29.45/29.18 at frame 0, wrap at the line.
> Then P3 = `FUN_00414a70` + `FUN_00414300` + the `+0xbc` speed write. Owed via `re-classify`: D-11072 row, U-items for the exe
> exact-sqrt fallback (global, user decision, touches D2), track-load PC_24, the `RwMatrixInvert` exe fix.

> ## START HERE 2026-10-09 (B2 settings DEFAULT-ON): **new G-CTRL hashes: step `ae67b13a`, gates `c8317ce2`, ptrace `31db9384`.**
>
> Read [`RESULT_B2FLIP.md`](../verify/d3_b2flip_20261009/RESULT_B2FLIP.md). `MASHED_A364_RESET` / `REFDIST` / `RACEPCT_BRIDGE` / `WIRE_B2` are now
> default-ON (opt-outs `MASHED_NO_*`; all four together = the old default `c741a4c5` / `642af0ac` / `05984e30`). `MASHED_NO_ELIM` NOT flipped.
> (e) PASS 3/3 unchanged; (b) regime A 2/3 with car 2 failing 1 item (|steer| median 0; was 5). Player physics identical until the
> old default's round-1 end; no D2 reopen candidate. Open: car 2's |steer| median 0; a labelled original regime-B run.

> ## START HERE 2026-10-09 (FUN_00442a60 bridge FIXED): **the reference is the pair's LEADING car; the bridge took the rearmost.**
>
> Read [`RESULT_REFDIST_FIX.md`](../verify/d3_bscore_20261009/RESULT_REFDIST_FIX.md). With the fix (behind `MASHED_REFDIST`, default OFF) start
> distances match the original (0 / 1.550 / 1.823 / 2.739 vs 0 / 1.553 / 1.833 / 2.750) and branch 2 first fires 117 / 119 / 192 (orig
> 149 / 216 / 180; was 643). Under the full B2 set + fix, (b) regime A: car 2 fails 1 item (was 5). Default build unchanged. U-9205 was
> mis-stated (the player input already matched) and is resolved. Open: making the B2 set default (user decision); car 2's
> |steer| median 0 under B2.

> ## START HERE 2026-10-09 ((b) is now regime-conditioned, user decision (iii)): **port regime A 2/3 (car 2 fails); B and C not scored yet.**
>
> Read [`RESULT_BREGIME.md`](../verify/d3_bscore_20261009/RESULT_BREGIME.md). The difficulty roll `DAT_0089a368` has THREE outcomes
> (flag 0 / 1 / 2 = regimes A / B / C). 7 labelled original runs: 6 A (all identical), 1 C, 0 B. Checker:
> `py -3.12 re/tools/ai_ctrl_window.py <step.csv> --check-regime re/tools/b_envelopes.json`. Owed: a labelled original B run
> (extend `capture_breg.sh` past 8), a port ring phase in C (extend the `MASHED_RING_SKIP` sweep), and car 2's regime-A failure.

> ## START HERE 2026-10-09 ((b) scoring MEASURED, decision PENDING): **(b)'s RNG sensitivity is exactly the one-shot difficulty roll `DAT_0089a368`.**
>
> Read [`RESULT_BSCORE.md`](../verify/d3_bscore_20261009/RESULT_BSCORE.md). Over 12 ring phases (`MASHED_RING_SKIP`, default build) every car
> takes 2 outcomes, fully set by the roll: regime A (10/12) passes 2/3 cars, regime B (2/12) passes 0/3. The original has the same
> 2-regime structure (3 of 4 runs identical; `o_spread3` = regime B by its accel ~102). **User decision owed:** (i) keep, (ii) sweep
> pass-fraction, (iii) regime-conditioned envelope (recommended), (iv) (iii) + per car.

> ## START HERE 2026-10-09 (U-9201 RESOLVED, partial): **the phase-3 length is the system QPC clock; with a run's clock replayed the port reproduces it (693/690, 742/740) and G-CURVE passes within 1 %.**
>
> Read [`RESULT_U9201.md`](../verify/d3_gatefire_20261008/RESULT_U9201.md) §0, §3. The camera sway input `DAT_007f1030` is
> QueryPerformanceCounter at 3 MHz (`FUN_00493390` 0x00493459), so the original's phase-3 length varies per run by design.
> Residuals filed: **U-9202** (every port car +0.040 higher on the grid, x +0.008 x index: the steady camera offset to 4
> decimals; D2 watch item) and **U-9203** (camera transient over the first ~60 frames). Diagnostic knob:
> `MASHED_CAM_TICKS_REPLAY`. Decision (A) stands; the knob-default question still waits on how (b) is scored.

> ## START HERE 2026-10-09 (D-11073 leg 2 DONE): **phase 3 ported behind `MASHED_SUBSTATE` (default OFF), byte-identical default. 4 bodies C3. Next: U-9201 (the camera target) or the (b) scoring question.**
>
> Read [`RESULT_LEG2.md`](../verify/d3_gatefire_20261008/RESULT_LEG2.md) §0, §2, §5 (prereg `PREREG_LEG2.md`).
> - `Race/CameraEntries.cpp` (both targets, x87): `0x00441b30`, `0x00441c80`, `0x00442600`, `0x00445aa0`, all run_diff GREEN
>   5/5. Written from the bytes: the decomp misread a double (57.2958) as -2.0; ST-resident values must be `double`.
> - Knob ON: phase 3 released by site (a) after **764** frames = the pre-registered model prediction; then 4, 5x112, 6.
>   The original's 652 is not reproduced: the entry dynamics are exact, the camera TARGET differs (U-9201). G-CURVE
>   fails at f480/f540 exactly as pre-registered.
> - Decision (A) stands: seed kept, knob OFF. (b) moves with the RNG ring phase (L2: 5 items, P414: 10).
>
> **Kickoff options for the next session:** (1) U-9201: capture the original camera struct from state 0 through phase 3,
> port `FUN_00442a20`, run the camera chain from state 0, re-score G-CURVE; or (2) decide how criterion (b) is scored
> given its ring-phase sensitivity, before any knob-default decision.

> ## START HERE 2026-10-09 (FUN_00414180 DONE, C3): **the AI start-boost chain is fully ported behind `MASHED_SUBSTATE` (default OFF), byte-identical default. Next: USER DECISION on seed vs chain, then leg 2.**
>
> Read [`RESULT_414180.md`](../verify/d3_gatefire_20261008/RESULT_414180.md) §0 and §3 (prereg `PREREG_414180.md`, every gate PASS).
> - `SubStateRollCountdown414180` at `0x00414180`: round 1 cd = 3,1,4,2 and G-EXIT 0/0, 1100/1, 1400/1, 1200/1, exactly as
>   pre-registered; rounds 2-4 = 1000+100*cd, cd matching the verbatim ring. The decomp/plates were wrong (`FUN_004a2c48` is
>   `_ftol2`; ST0 return and the fd0 {4,8,9} -> 9.9f arm were dropped). `MASHED_SUBSTATE_CD` is now only an override.
> - **Round 1: the start-boost seed masks the chain** (P414 == P414_CD3 until step frame 1986). Rounds 2+ boost only from the chain.
> - **(b) depends on RNG ring phase:** the body's 4 draws alone move (b) 5 -> 10 failing items. Fix how (b) is scored before trusting any (b) delta from the knob.
> - bias374 reversals 2 / 1 (first non-zero, symptom (a)), cause not isolated. U-9198 (original cd 3,3,3,3 = 1/64), U-9199 (path1 coverage).
>
> **DECIDED 2026-10-09: (A)** keep the start-boost seed, knob stays OFF (not (B) chain-only, not (C) default flip). Next: D-11073 leg 2 (phase 3, gate on the release rule, `FUN_00442600` via `aib_track_index()`).

> ## START HERE 2026-10-09 (D-11073 leg 1b DONE): **sub-states 4 + 5 ported behind `MASHED_SUBSTATE` (default OFF), byte-identical default. Next: port `FUN_00414180` (the cd producer), then leg 2.**
>
> Read [`RESULT_LEG1B.md`](../verify/d3_gatefire_20261008/RESULT_LEG1B.md) §0 and §6 (prereg `PREREG_LEG1B.md`, all pass gates PASS).
> - `Race/SubStateMachine.cpp`: `0x0040dbd0` `SubStateEnter4` and `0x004103a0` `SubStateHeldCountdown5`
>   (EAX shim), both C3. Knob ON: 1 x state 4, 112 x state 5, exit read `0x3feeeee2`, 12000 seed -50 per frame,
>   cars held bit-exactly, every round (`StartRound` resets `DAT_0063ba8c`).
> - **The AI start-boost writer is located:** `FUN_00414180` writes cd (= 3 on the original) at
>   `0x0089a4f8+v*0x74`, then Branch A presses accel on frames 107..112, then `FUN_0046d7f0` charges 300, then
>   `FUN_0046d780` arms 1300/1, which is the measured seed exactly. Shown with `MASHED_SUBSTATE_CD=3`
>   (measurement only).
> - Use `MASHED_SUBSTATE` **with** `MASHED_MODE5_RESET`. Alone, it gives the over-rev bog in rounds 2+.
> - D2 REOPEN CANDIDATE (report only): player post-GO speed -1.2% at f=31 under the knob (RESULT §5).
>
> **Kickoff for the next session (paste):**
> Mashed RE, D3, D-11073 follow-up: port `FUN_00414180` (154 B, C2, `0x00414180..0x0041421a`) at its RVA
> behind `MASHED_SUBSTATE`, called from the state-2 tail emulation in `TrackRenderer.cpp` (replacing
> `MASHED_SUBSTATE_CD`). Read RESULT_LEG1B.md §0, §3, §6 and PREREG_LEG1B.md §2.5 first. Predict before
> porting:
> (1) the port's `ftol(DAT_0089a360)` idx and the `0x005f2d80` row it selects;
> (2) what the port's `FUN_00472650` stand-in draws for 4 cars, so cd is predicted per car;
> (3) the blast radius of its `DAT_0089a368 = 0` / `DAT_0089a370 = 0` stores (the AI one-shot 20 % roll at
>     `0x00417c43`) on (b);
> (4) the `0x0089a4c0+v*0x74` store (`= 0x005f2eb4[idx]`): census its readers.
> Gates: knob OFF byte-identical (`c741a4c5` / `642af0ac` / `05984e30`, run_h4.ps1 `L1B_CTRL`); knob +
> `MASHED_MODE5_RESET`: G-EXIT per car = `1000 + 100*cd` (cd >= 0: Branch A presses on the last `2*cd` phase-5 frames,
> charge `100*cd`, so cd 1..4 gives 1100..1400; cd = 3 gives 1300), predicted per car before running; run_diff + run_verify_hook. Then decide whether the start-boost seed yields to the real
> chain (user decision; the seed is one-shot per race, the chain re-arms per round). If `FUN_00414180`
> blocks, go to D-11073 leg 2 (phase 3, gate on the release rule).

> ## START HERE 2026-10-09 (D-11073 leg 1a DONE): **exe substrate in, default build byte-identical. Next: leg 1b (`FUN_0040dbd0` + `FUN_004103a0`).**
>
> Read [`RESULT_LEG1A.md`](../verify/d3_gatefire_20261008/RESULT_LEG1A.md) (prereg `PREREG_LEG1A.md`, all gates PASS).
> - `MASHED_SUBSTATE` (default OFF): step store `0x0040fc57` in `Ai_AdvanceClock` + `FUN_0041d930` tail partial
>   `HudSlideBillboardTimerTail`. `DAT_0063d588` bit-exact float32 running sum on 1288/1288 samples. Controls
>   `c741a4c5` / `642af0ac` / `05984e30` unchanged with the knob OFF **and** ON.
> - Seven callees moved to `Race/SubStateCallees.cpp` (E+A), one body per RVA, path1 GREEN + path2 E9 on all.
> - Original: `DAT_0067ea64` = 0 everywhere, so phase 4 calls **`FUN_0041b520`**. `0x00882194+v*0xd04` = 0,250,250,250
>   (last phase 5) / 0,1100,1100,1100 (first phase 6); `+0x2198` 0 -> 0,1,1,1. Records `0x0063c8d0+k*0x74` have
>   `+0x64` = k in the original; the standalone's BSS reads 0, so read it before porting (RESULT_LEG1A §4).
> - **Harness blocker FIXED same night:** the full auto-hook boot AV was the VideoGetRender{Width,Height}
>   hooks undoing `fix_camera_res` (live 1680x1050). `HookSystem.cpp` now refuses hooks over boot-patch bytes;
>   full-set `run_verify_hook` works again. Leg-1b facts added to `re/BRIEF_D11073.md` before the 1b section.

> ## START HERE 2026-10-08 (D-11073 re-scoped after leg 0): **Leg 1 split into 1a (exe substrate, inert) and 1b (phases 4+5). Start with 1a.**
>
> Brief: [`re/BRIEF_D11073.md`](BRIEF_D11073.md) §2. Leg 0 (`RESULT_LEG0.md`) showed a faithful phase-5 port
> never exits today, because the timer `DAT_0063d588` and its step `_DAT_007f100c` read 0.0 in the standalone.
> 1a supplies the timer step, the `FUN_0041d930` tail (partial), exe homes for six asi-only callees and two owed
> original columns, with the default build byte-identical. 1b then ports `FUN_0040dbd0`/`FUN_004103a0` with
> the 12000 seed and alive routing as a named deviation. Phase 3 (leg 2) gates on the release rule, not a length
> (651/652/730 frames). The kickoff prompt for 1a is in the brief's §4.

> ## START HERE 2026-10-08 (D-11073 leg 0): **Leg 1 CANNOT start as scoped. A faithful `FUN_004103a0` on the port's current inputs holds phase 5 FOREVER.**
>
> Read [`RESULT_LEG0.md`](../verify/d3_gatefire_20261008/RESULT_LEG0.md) (prereg `PREREG_LEG0.md`, 9/9 gates PASS).
> - **The phase-5 exit timer `DAT_0063d588` reads 0.0 in the standalone, and so does its step
>   `_DAT_007f100c`.** The original advances it 0.0166667/frame: `FUN_0041d930` tail at
>   `0x0041da76`, step stored by `FUN_0040fc00` at `0x0040fc57`. The exe has neither write, and
>   `HudSlideBillboardTick` cannot be wired as-is because it makes raw calls outside the mapped text.
> - **Phase 5 = exactly 112 frames.** The timer runs through phases 3, 5 and 6 and is zeroed only on
>   the phase-4 frame.
> - **Track id `0x1e` confirmed live, and entry 0 initial = row 0 of `0x005f8d50`** (to 0.002).
>
> Leg 1 scope additions (RESULT_LEG0 §4):
> 1. the timer step + tail;
> 2. the `DAT_005f29b8 = 12000` seed (standalone reads 0);
> 3. alive routed through `s_host.car_alive` (`0x008815a4` reads 0/0/0/0 in the exe vs 1/1/1/1 original);
> 4. six asi-only callees need exe homes.
>
> **Leg 2 caution:** phase-3 length does NOT reproduce (651 / 652 / 730 frames; the target drifts per
> run). Gate on the release rule, not a length. `DAT_00644158` reads 0 in the exe, so route
> `FUN_00442600` through `aib_track_index()`.
> **Owed on the next original capture:** `DAT_0067ea64` and `0x00882194/98 + v*0xd04`.

> ## START HERE 2026-10-08 (D-11073 scoped): **Port brief written: [`re/BRIEF_D11073.md`](BRIEF_D11073.md). The pre-race machine is THREE phases, not one.**
>
> 3 (camera hold, ~652 frames) -> 4 (1 frame) -> 5 (held-car countdown, ~110 frames, released at
> `DAT_0063d588 >= 1.86f`). `bias374` and the mode-5 branch need **phase 5**, not phase 3, so the
> brief orders the work as leg 0 (measure, no code), then leg 1 (phases 4+5), then leg 2 (phase 3).
> The camera-path clip code is NOT needed: `FUN_00405540` early-outs on `DAT_00639d78 == 0`.
> The kickoff prompt for leg 0 is in the brief's §4.

> ## START HERE 2026-10-08 (task 4 / H4): **`MASHED_SLOT_PLAYER` is DEFAULT-ON, measured inert on ALL FOUR cars. `MASHED_WIRE_B2` stays OFF.**
>
> Read [`RESULT_H4.md`](../verify/d3_gatefire_20261008/RESULT_H4.md) (prereg `PREREG_H4.md`).
> Car 0 is measured for the first time (`MASHED_PLAYERTRACE`). With and without the stores,
> car 0's trace and the stepdump are **byte-identical**, and post-flip default = B,
> opt-out = A, on all three hashes. Branch 2 firing (18x, car 2) moves cars 1-3 on 38,481
> rows but leaves car 0 **byte-identical**, while car 0 is moving. `WIRE_B2` stays OFF on its
> prerequisites (`A364_RESET`, `REFDIST`, `RACEPCT_BRIDGE` undecided; `NO_ELIM` is a harness
> override).
>
> **CARRY:** default gates-dump control is now **`642af0ac`** (step `c741a4c5` and ptrace
> `05984e30` unchanged). `MASHED_SLOT_PLAYER` is a no-op, so use `MASHED_NO_SLOT_PLAYER` for the old
> behaviour. Branch 2 now needs `A364_RESET + REFDIST + RACEPCT_BRIDGE + NO_ELIM + WIRE_B2`.

> ## START HERE 2026-10-08 (tasks 1-3): **The exit flag HOLDS for 652 frames and ends on a DISTANCE test, not input, not the clip module. Clip handles are ZERO live. D-11073 row corrected.**
>
> Read [`RESULT_CADENCE.md`](../verify/d3_gatefire_20261008/RESULT_CADENCE.md) (prereg `PREREG_CADENCE.md`).
>
> **1. `FUN_00445aa0` cadence.** Runs every sub-state-3 frame (`FUN_004111c0` case 3 -> `FUN_0040fc00`
> -> `FUN_0040d470(0)` -> `FUN_00448220` -> `FUN_00446520` -> `0x004468f9` -> `FUN_004464c0`), with
> `param_2 = &DAT_00897fe0`, i.e. the flag itself. Both clears are CONDITIONAL: (a) entry within
> **0.02f** of the target, (b) any input byte `0x007f1042+k*0x4c`. **Live, no injected input: flag
> = 1 on 628/630 samples, 652 frames, cleared by (a)** (entry 0 29.4 -> 0.0207, all inputs 0).
> So the hold survives. **But its END is not cheap.** Without site (a) a no-input port holds
> forever. Site (a) needs `FUN_00445aa0`'s flag-set branch, `FUN_00442600`, and the target copy at
> `0x004481c9` in `FUN_00446520` (computed base, 0 static refs).
>
> **2. D-11073 row corrected and then UPDATED, both via re-classify. Cost is now listed in the row.**
> The standalone carries NONE of clear site (a) (`SURVEY_RACECAM.md`). Live census: 7 entries,
> types 0/1/1/1/2/2/3. Only entry 0 (type 0) can clear the flag, and types 1/2 serve visual parity only
> (RESULT_CADENCE.md s6). To port: seed + coordinator + flag, `FUN_00442600`, a real entry record,
> the `FUN_00445aa0` flag-set branch, the `0x004481c9` copy (split `pos_out_`), the sub-mode != 6
> path with `FUN_004464c0`, and an offset-0 flag on `RaceCamera`.
>
> **3. `DAT_00657448` / `DAT_00639d70` / `d78` = 0 on 656/656 live sub-state-3 samples.**
>
> **HARNESS TRAP, NEW:** every `scenario_launch.py` run WITHOUT `--statediff-out` pulses control 4
> (`:3347`, "skip the start intro"), which **truncates phase 3 to ~25 frames**. Any phase-3 or
> early-race measurement needs `--statediff-out`. `orig_rampwatch.py --cam` is the new opt-in
> column set; default output unchanged.
>
> Still open, independent: (4) `MASHED_SLOT_PLAYER` with `MASHED_WIRE_B2` in a car-0 leg,
> U-9191 (b), U-9195, the `0x00409b0e` jumptable.

> ## START HERE 2026-10-08 (tasks W-SHAPE / B / C): **W-SHAPE is CLOSED NEGATIVE. D-11073 is CHEAPER than its row says. The slot seed is now DEFAULT-ON.**
>
> Read [`RESULT_WSHAPE.md`](../verify/d3_gatefire_20261008/RESULT_WSHAPE.md) →
> [`SCOPE_SUBSTATE2.md`](../verify/d3_gatefire_20261008/SCOPE_SUBSTATE2.md) →
> [`RESULT_H2.md`](../verify/d3_gatefire_20261008/RESULT_H2.md).
>
> **1. `W-SHAPE` port side: closed NEGATIVE.** Added `c4_in`/`c5_in` to the stepdump, ran it, and
> `WS-LIVE` **FAILS**: entry pose is 0 on **42,654/42,654** port rows and — checked for the first
> time — **5,318/5,318** original `o_t3` rows. Both sides zero `ctrl` before every call, so the
> quantity is degenerate. The **probe is sound** (it disagrees with the output on 99.2% of rows,
> which is what proves it samples pre-write). The `ctrl[4]` claim stands on the **static**
> transcription only. Columns kept and marked DEAD. This **corrects `RESULT_FIRESHAPE.md` §4**.
>
> **2. D-11073 scoping (task B), and it LOWERS the cost.** Both open reads answered:
> `FUN_004430a0(0)` — the clear — comes from **`FUN_00445aa0`** (2 sites); `FUN_004102f0` passes
> **1**, it *sets* the flag. And `FUN_004053d0`/`FUN_00405400` **load nothing** — both are pure
> setters; the handle is `DAT_00657448`, which has **no writer anywhere in the image**.
> **The row's inference is wrong:** phase 3's hold does **not** need the camera-path module. The
> exit test is `FUN_00405460()==0 && FUN_004430b0()==0`, so the **flag alone blocks it**, and
> `FUN_004102f0` sets the flag itself under `DAT_005f29b8 == 100000` — a write that is already
> **inside `FUN_004111c0`** at `0x004100d3`. Hold ≈ one seed write + the 172 B coordinator;
> the camera module governs the *motion*, not the hold. **Owed `re-classify` correction to the
> D-11073 row** (not hand-edited). Check `FUN_00445aa0`'s cadence first — it could clear the flag
> immediately, and that is unread.
>
> **3. `H2` DECIDED and SPLIT (task C).** `MASHED_SLOTSTATE_SEED` is **DEFAULT-ON**, five gates
> PASS: the value `0x005f2728` is what the image itself carries at file offset `0x1f2770`;
> it moves exactly **3 of 84** columns (its own witnesses); (b)/(e) digit-identical;
> `MASHED_NO_SLOTSTATE_SEED` restores the old default **byte-identically**.
> **`MASHED_SLOT_PLAYER` stays OFF** — it is a partial bridge at a knowingly-unfaithful site that
> exists only to enable branch 2, which is itself OFF, and the metrics never cover car 0.
>
> **CARRY THIS:** the knob-off reference has **moved**. `86b7b2bb` (80 col) and `7ced2aa3`
> (84 col) are **pre-seed**. The current default control is **`H2_def.step.csv`, `c741a4c5`**.
> Diffing a new default capture against the old ones shows `ss_base`/`ss_v`/`ss_raw` differing
> **by design** — do not read that as a regression. Byte-hash controls are dead anyway; use
> `det_prefix.py --common-cols` (the schema is now 84 columns).

> ## START HERE 2026-10-08 (task A): **The later-anchored window is the WRONG instrument — the two sides' firings are at DISJOINT offsets. Scored call-wise instead: branch 2's output pose AGREES 82/82.**
>
> Read [`RESULT_FIRESHAPE.md`](../verify/d3_gatefire_20261008/RESULT_FIRESHAPE.md). Scoping +
> re-scoring of committed captures only — no run, no build, nothing default-ON, no C-level.
>
> **Task (A) was declined on measurement, not skipped.** Moving the window later does make it see
> the port's firings, but it moves it off the original's: original fires at call offsets
> **149-219**, port at **558-660**. No single offset window holds both. And a late window has **no
> reference** — at `skip=558` only **2 of the original's 12 observations** are full-length and
> live (7 short, 3 frozen), and those 2 disagree on every statistic.
>
> **What replaced it.** `re/tools/ai_fire_shape.py` (new) scores the calls where the branch
> actually fires, each side in its own regime, with a control arm. Original **64/64** and port
> **18/18** firing calls both give `(c0,c1,c4,c5) = (0,0,0,255)` — **82/82**, against control arms
> steering across 13-75 distinct values. First behavioural agreement measured for branch 2.
>
> **W-SHAPE partially closed.** `o_t3` carries `c4_in`/`c5_in` after all: entry is `(0,0)` on all
> 64 original firing calls, so the capture **cannot** separate "branch zeroes `c4`" from "`c4` was
> already 0". Port side still open — it needs that column pair in the stepdump.
>
> **Still open, same root (`D-11073`):** branch 2's **timing** (the 423-call offset *is* the `i0`
> anchor split) and its **car distribution** (original 31/4/29, port 18 on one car). The pose
> agrees; when and to whom does not.
>
> `ai_ctrl_window.py` gained `--skip K` plus a short-window refusal and a `degenerate` flag.
> **skip=0 output is byte-identical to the previous `HEAD`** (diffed over four captures); the
> committed (b)/(e) criteria are untouched. Nothing was re-baselined.

> ## START HERE 2026-10-08 (commits `c41cd645`..`67515c93`): **GATEFIRE ran end to end. Branch 2 is PORTED, WIRED and FIRES. The lane is now blocked on ONE ROOT - the race sub-state machine, opened as D-11073.**
>
> Read [`RESULT_GF1.md`](../verify/d3_gatefire_20261008/RESULT_GF1.md) -> [`RESULT_WIRE2.md`](../verify/d3_gatefire_20261008/RESULT_WIRE2.md) -> [`RESULT_NOELIM.md`](../verify/d3_gatefire_20261008/RESULT_NOELIM.md) -> [`RESULT_CALLWISE2.md`](../verify/d3_gatefire_20261008/RESULT_CALLWISE2.md). Scope: [`SCOPE_SUBSTATE.md`](../verify/d3_gatefire_20261008/SCOPE_SUBSTATE.md).
>
> **What landed.** Branch 2 (`FUN_004148b0 && FUN_00416060`) has a standalone body as a SHARED TU
> (rva-lint NEW=0, allowlist unchanged); `FUN_00414060` step 6 is a partial port fixing `idx364`;
> `MASHED_SLOT_PLAYER` bridges the player slot-state cell; the `FUN_00418560` mode-5 branch is
> ported. **All default-OFF. The default build is bit-identical across TEN builds (`86b7b2bb`).**
>
> **Branch 2 FIRES** - 18x on v2 under `MASHED_NO_ELIM`, LOS passing on all. All four
> `PREREG_WIRE` gates PASS. **But `W-NOREG` passes BLIND**: the firings are at frames 643-660 and
> the scored window is 1-220, so **0 of 18 are inside**. Do not report it as "the wiring does not
> regress (b)/(e)".
>
> **`GF1-CALLWISE` RAN: 0 vs 64.** Both windows are each side's first 220 mode-6 calls, but the
> original reaches mode 6 after 683 calls of phase 3 and the port is in mode 6 from frame 0.
>
> ### THE ONE ROOT - D-11073
>
> `aib_game_sub_mode() { return 6; }` (TrackRenderer.cpp:100) is upstream of **three** measured
> symptoms: the `bias374` ramp, the inert mode-5 branch, and the CALLWISE anchor split. Phase 3 is
> a **camera-path keyframe fly-in** over per-frame-re-initialised cars, chain **3 -> 4 -> 6**.
>
> **Do NOT cost it as a state-function port.** The clip handles `DAT_00639d70`/`d78` and the exit
> flag `DAT_00897fe0` are in `.data`'s **uninitialised tail** - zero at load on BOTH sides - so
> porting `FUN_004102f0` alone exits 3->4 on frame 1 and produces **no hold**. It needs the
> camera-path module at **0x004053d0..0x00405540** that writes them.
>
> ### Standing cautions earned here
>
> - **`.data` is ~90% BSS** (VSize `0x32a704` vs RawSize `0x4d000`). "Is this address initialised
>   in the image?" is a **per-address** question: `offset_in_section < SizeOfRawData`. A flat
>   `VA - 0x400000` read is valid only inside the file-backed part. `DAT_005f29b8` IS file-backed
>   (`0xff`); the three phase-3 globals are not.
> - **`hooks.csv`'s `exe_file` is not ground truth** - `.rsp` membership is. Two rows named TUs
>   holding only thunks or comments; I corrected one and walked into the other myself.
> - **The `MASHED_GF1` probe MUTATES state** (`TimerAt`/`RankAt`). It must be OFF for any (b)/(e)
>   scoring.
> - **Arm every prerequisite knob.** Branch 2 needs `SLOTSTATE_SEED` + `SLOT_PLAYER` +
>   `A364_RESET` + `REFDIST`; an under-specified arm produced a false "the wiring is inert".
>
> ### Next, in order
>
> 1. **D-11073** is the highest-value item in this area. Open reads if taken: who calls
>    `FUN_004430a0(0)` to clear the exit flag, and what `FUN_004053d0`/`FUN_00405400` load the
>    clip from.
> 2. A **later-anchored window** so (b)/(e)-style statistics can see branch 2 at all. Scorer
>    change, needs registering.
> 3. Independent: **H2** (`MASHED_SLOTSTATE_SEED` default-ON, now joined by `MASHED_SLOT_PLAYER`,
>    both measured inert), **U-9191** item (b), **U-9195**, the `0x00409b0e` jumptable.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.


> ## START HERE 2026-10-08 (commits `c41cd645` → `170245be`): **GATEFIRE leg 0 RAN and the lane narrowed to ONE open question — `bias374`. Branch 1 is DEFERRED (input-blocked). `idx364` is FIXED via a PARTIAL port of `FUN_00414060` step 6, default-OFF behind `MASHED_A364_RESET`. Default build bit-identical across SIX builds.**
>
> Read [`RESULT_GF0.md`](../verify/d3_gatefire_20261008/RESULT_GF0.md) →
> [`RESULT_IDX364.md`](../verify/d3_gatefire_20261008/RESULT_IDX364.md) →
> [`RESULT_A360.md`](../verify/d3_gatefire_20261008/RESULT_A360.md) →
> [`RESULT_A364.md`](../verify/d3_gatefire_20261008/RESULT_A364.md).
>
> - **Branch 1 (36 calls) is DEFERRED, input-blocked.** `GF0-PROG1`: `0x008a96e8 + v*0x30c` is
>   `0.0` on 100% of rows. The coverage check is what makes it a finding — `racepct_ec` at
>   `0x008a96ec`, 4 bytes higher, same stride, same loop iteration, is live with up to 11,620
>   distinct values. Needs a producer; that is a `DEFERRED.md` row, not a leg.
> - **`idx364` RESOLVED and FIXED.** Writers are `FUN_00414060` step 6 (unconditional `-1` reset)
>   and `FUN_00414220` step 2 (early-returns all race). Step 6 is ported bit-faithfully; all five
>   gates PASS; INERT by design (the only consumer, `AiLeaderTimer.cpp:94`, is `.asi`-only).
> - **Porting `FUN_00414060` WHOLE is measured-blocked** — every input of steps 1-5 reads `0`, so a
>   faithful port would regress `0x0089a360` from `2.5` to `0`. Blocker is upstream of all four
>   callees: `U-1305` (`0x0067ea7c`) plus `0x0067e9fc`. `0x005cd088` reads exactly **2.5**, so the
>   original's value is almost certainly `TiebreakFlag * 2.5` and the port's seed has been the right
>   number for the wrong reason. Which arm the original takes is **[UNCERTAIN]** — a Frida entry
>   probe on `0x00414060` would close it.
>
> ### START HERE
>
> 1. **`bias374` is the ONLY thing left before branch 2 is measurable.** Port `{0,1,2,3,4}` vs
>    original `0`; `4` is new since the 2026-10-03 witness and is the majority at 72.9%. With
>    `bias374=4, iVar1=2` the limit-table index is **14**, not the original's **10**. Writer is
>    `FUN_004177b0`, exe copy C2-demoted 2026-09-29 (`Race/RuleEngine.cpp`) — start there.
>    NOTE `AiStandalone.cpp:1577` **does** write `0x0089a374`, so this has a DIFFERENT shape from
>    `idx364` and must not be assumed to resolve the same way.
> 2. **`re-classify` transactions owed** (all flagged, none hand-edited): `hooks.csv` `00414060` →
>    C2 "partial (step 6), steps 1-5 blocked", `exe_file = Race/AiDifficultyReset.cpp`;
>    `hooks.csv` `004148b0`'s `exe_file` is **stale** (claims `Ai/AiStandalone.cpp`, which has no
>    body); U-9186's standing instruction is superseded by U-9187 (`PREREG_GATEFIRE.md` §1).
> 3. **Cheap and open:** should `MASHED_A364_RESET` go default-ON? Inert today, so it can ride with
>    **H2**'s `MASHED_SLOTSTATE_SEED` question. Also **U-9191** item (b), **U-9195**, `0x00409b0e`.
>
> **Standing note earned here:** a gate can be registered as *instrumentation* when the thing it
> counts has no body and no live substrate on the measured side — census per branch BEFORE
> registering a counter. And measure a function's **inputs** before porting it: `FUN_00414060` was
> authorized whole and the measurement, taken before a line was written, showed that porting it
> whole was a regression.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.

> ## 2026-10-08 (commit `1cb89a5a`): **U-9186 `GATEFIRE` is PRE-REGISTERED, UNRUN, both branches, by USER DECISION. Read [`PREREG_GATEFIRE.md`](../verify/d3_gatefire_20261008/PREREG_GATEFIRE.md) and run LEG 0 FIRST.**
>
> **Do not re-derive the scope.** Three prior measurements already answer most of "what is
> missing", and re-deriving them is the recorded failure mode
> (`read-the-tracker-row-before-starting-its-work`): `UNCERTAINTIES.md:61` (U-9186, per-branch
> blockers), `UNCERTAINTIES.md:63` + `verify/d3_leader_20261003/RESULT_WITNESS.md` (U-9187 — the
> `.rsp` wiring is **unsafe AND inert**), and H1a/H1b/H3.
>
> - **The scope shrank.** Of the witness's six missing inputs, `FUN_0040e470` (exe body exists),
>   `FUN_0046d4a0` (H1b) and `FUN_00442a60` (H3 — the one it called "a real port, new reversing")
>   have landed; the limit table and the four thresholds `6.5/5.5/6.0/4.0` were resolved *by* that
>   witness. **Only `FUN_00442cc0` is open, and it reads `0x008989b0[v]`, which H3 made live.**
> - **The mechanism is a standalone body, NOT an `exe_sources.rsp` addition.** `exe_main.cpp:56`
>   leaves `0x00400000..0x004fffff` unmapped, so adding those TUs access-violates — measured.
>   U-9186's standing instruction says otherwise and is **flagged for `re-classify`**; its
>   *ordering* (branch 2 then branch 1) is kept.
> - **Leg 0 is a RUNTIME census and gates everything.** `AMEND_H3.md`'s census was static. Its
>   decision rules are pre-committed: `GF0-PROG1` zero → branch 1 deferred with its blocker named;
>   `GF0-TIMER` dead → the body still lands and `GF1-FIRE` is reported structurally-0. **Seeding a
>   global is a non-goal.**
> - **Two upstream disagreements are the real branch-2 risk**: `idx364` (original `-1` on all 512
>   calls, port `0` — it decides whether `E470` runs at all) and `bias374` (original `0`, port
>   `{0,1,2,3}`; writer `FUN_004177b0`'s exe copy is C2).
> - **Branch 1 carries a plate conflict**: `_DAT_005cc320` is **`+1.0f`** per
>   `bucket_ai_00407a40_00415880/0x00414300.md:33`; `ai_update_d3/0x00414300.md:33`/`:73` says
>   `-1.0` and is **wrong**. Calling convention is plate-silent for both functions.
> - Acceptance is **per-call**: `o_t1`/`o_t2`/`o_t3` carry a live `ret14a70` column, so all 149
>   diverging calls are checkable one by one.
>
> **Tracker defect to fix via `re-classify`:** `hooks.csv` row `004148b0` claims
> `exe_file = Ai/AiStandalone.cpp`, which holds no body for it (comments only at `:82`/`:846`/
> `:968`/`:1086`). Ground truth for exe-side coverage is **`.rsp` membership**, not that column.
>
> Still open and independent: **H2** (`MASHED_SLOTSTATE_SEED` default-ON), **U-9191** item (b),
> **U-9195**, the `0x00409b0e` jumptable. **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.**

> ## UPDATE 2026-10-08 (U-9186 leg H3 stepdump arm RAN, commit `e06c9451`): **the three gates H3 left unrun all PASS — the default build is bit-identical across three builds (one SHA-256 for all four captures). `H3-GATEFIRE` is WITHDRAWN: it is a PORT leg, not instrumentation, and the two branches are NOT equally blocked. NO C-level, nothing default-ON, `original/` untouched, `.asi` untouched.**
>
> Read [`RESULT_H3_STEP.md`](../verify/d3_u9186_20261008/RESULT_H3_STEP.md), then
> [`AMEND_H3.md`](../verify/d3_u9186_20261008/AMEND_H3.md) §3.1 for the census.
>
> - `H3-KNOBOFF` **PASS** and **not** the near-tautology H1a's was (`H1-CALLERS = 0`): H3's call
>   site runs 14,400 times in the default build and still changes nothing. `E2off_1`, `H1step`,
>   `H3step`, `H3step_r2` all hash to `86b7b2bb`. `--common-cols` dropped no column (no schema
>   drift). `H3-NOREG-E` 3/3. `H3-DET` PASS. `G-BANDS-UNEDITED` empty before and after.
> - `H3-NOREG-B` **PASS as UNCHANGED**, which is not "(b) passes": `v2` still fails the same five
>   bands with the same numbers (`c0_distinct=39`, `c1_distinct=75`, `steer_distinct=113`,
>   `c1_median=5.5`, `abs_steer_median=44.5`). That failure is the point of U-9186.
> - **`H3-GATEFIRE` withdrawn.** `ControlStep` calls none of the three predicates —
>   `AiStandalone.cpp:844` is a hardcoded `mode = 0` with the chain elided at source level, not
>   stubs returning 0. `FUN_00414a70` has **no body anywhere** (`hooks.csv:645`); `FUN_004148b0` /
>   `FUN_00416060` are `.asi`-only and RVA-saturated.
>
> ### START HERE — the branch-scope question is a USER call
>
> 1. **The 64-branch is the cheap half and the registration did not know it.** Its LOS input is
>    already live: `TrackRenderer.cpp:359` loads the `.AI` tile grid to `0x007f1a9c` and
>    `AiStandalone.cpp:282-288` already implements `FUN_00416060`'s exact tile test. What is left is
>    `FUN_004148b0`'s substrate — `0x0089a4c4` / `0x00442cc0` / `0x0040e470` have exe-side
>    references, **`0x0089a4c8` and `0x005f2dd8` have none**. `0x005f2dd8` is the `0x005f2770`
>    class: extract it from `MASHED.exe.unpatched` as `H3-CONSTS` did.
> 2. **The 36-branch is a port from zero** — `FUN_00414a70` plus its unported callee `FUN_00414300`.
> 3. **[UNCERTAIN], and it gates both:** the census above is **static**. An exe-side reference
>    proves the address is addressed by compiled code, **not** that the value is live in a race.
>    `RESULT_STEP2.md:223`'s risk that `LeaderTimer` reads `.bss` zeros is narrowed, not closed.
>    **Open whichever leg wins with a RUNTIME substrate census**, or the counter reports a number
>    with no meaning (the `_abs`-column failure mode).
> 4. Still open and independent: **H2** (`MASHED_SLOTSTATE_SEED` default-ON, priced by `H1-ALL` at
>    75% of rows), **U-9191** item (b), **U-9195**, the `0x00409b0e` jumptable.
>
> **Standing note earned here:** a gate can be registered as *instrumentation* when the thing it
> counts does not exist on the measured side. Before registering a counter, check the predicate has
> a body **and** a live substrate on the side being measured — the same discipline as "the
> instrument must touch the changed path", one level earlier.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.

> ## UPDATE 2026-10-08 (U-9186 leg H3 RAN = DEFERRED leg B, commits `1560986e` + the decision record): **the `0x008989b0` producer WORKS — live per-car distances for the first time (26,994/53,992 rows non-zero, 19,562 distinct) — and is INERT at the behaviour level. The leg-B prohibition was LIFTED by USER DECISION (Mariano 2026-10-08). Default-OFF behind `MASHED_REFDIST`. NO C-level, `original/` untouched, `.asi` untouched.**
>
> Read [`RESULT_H3.md`](../verify/d3_u9186_20261008/RESULT_H3.md), then
> [`VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md`](analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md).
>
> - `H3-CONSTS` PASS — 80.0 / 20.0 / 100.0 / 0.8 read from `MASHED.exe.unpatched` (`.rdata`
>   `0x1cc730`/`0x1ccd6c`/`0x1cc568`/`0x1cc9bc`). `race_pct` is 0..100 per lap, so ">80 and <20"
>   is the pair straddling the line and `-100.0` un-wraps it.
> - `H3-PAIR` live (7 distinct `(ref,other)` pairs vs 1 on OFF). `H3-DET` identical per arm.
> - `H3-WROTE` **FAILED as registered** (100% required, 74.9963% measured). Cause is the scorer:
>   13,497 of 13,500 mismatches are car 0, which the gate chain correctly skips while the probe
>   computed a distance for every car. On the gate-passing denominator: **40,491/40,494 =
>   99.9926%**. The 3 residual rows are all **frame 0** and the port is the side that is right.
>
> ### START HERE — three things are owed before anything new
>
> 1. **`H3-GATEFIRE` was registered as "the one that matters" and was NOT BUILT.** Nothing is
>    claimed about whether U-9186's branches fire. Build the default-OFF counter on
>    `FUN_00414a70 == 2` and `FUN_004148b0 != 0 && FUN_00416060 != 0` and confirm they approach the
>    original's **36 / 64** calls. The committed captures `o_t1`/`o_t2`/`o_t3` carry the exact 149
>    diverging calls.
> 2. **`H3-INERT` is expected, not a defect.** `ControlStep` still hardcodes `mode = 0`
>    (`AiStandalone.cpp:844`), so the branches cannot fire whatever `0x008989b0` holds. H3 removed
>    the *input* blocker, not the stub. Do not read inertness as the port failing.
> 3. **`H3-KNOBOFF` / `-NOREG-E` / `-NOREG-B` were not run** — the gates dump is a different schema
>    from the stepdump those scorers take. Add a stepdump arm to `run_h3.ps1`.
>
> Only after 1-3 does wiring the `FUN_00416250` branches to `ctrl` make sense; that is a separate
> registered leg. **H2** (the `MASHED_SLOTSTATE_SEED` default-ON question) is still open and is
> cheap. Also still open and independent: **U-9191** item (b), **U-9195**, the `0x00409b0e`
> jumptable.
>
> **Standing notes earned this session.** (a) A gate's instrument must touch the path the change
> alters — `G-TOOK`, `H1-CONVERGE` and `H3-WROTE` all failed on this, each costing a rebuild and a
> re-run for a subject that had passed (`PREREG_H1.md` §A2). (b) `scripts/lint_rva_bodies.py`
> binds a function to the **first RVA token in the comment above it**; a comment that merely
> *mentions* an RVA will be read as a body declaration. (c) The lint's allowlist is a burn-down
> list — prefer consolidating a pair into one shared TU over adding a new one.
>
> Blocks below are previous headlines, left as history.

> ## UPDATE 2026-10-08 (U-9186 leg H1a RAN, commits `714a97e1`/`6e805552`): **the vehicle-record rebind LANDED, all gates pass. `VehicleSlotGetter`/`VehicleCarStateRead` now return the port's live record standalone (100% of 53,992 rows), and the default build is byte-for-byte unchanged. H1a is a PRECONDITION for H3, not a behaviour change. NO C-level, nothing default-ON, `original/` untouched, `.asi` unedited.**
>
> Read [`RESULT_H1.md`](../verify/d3_u9186_20261008/RESULT_H1.md), then the scope
> [`VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md`](analysis/VEHICLE_TABLE_BIND_SCOPE_2026-10-08.md).
>
> - `H1-CONVERGE` **100.0000%** (3 arms), `H1-GATE2` `veh_type_fn == 1` **100%** (was `0`),
>   `H1-ALL` **75.0000%** with `MASHED_SLOTSTATE_SEED=1` — the three AI cars.
> - `H1-KNOBOFF` byte-identical to `E2off_1.csv`; `H1-NOREG-E` 3/3; `H1-NOREG-B` unchanged.
> - **Control held**: the `_abs` columns stayed dead, so the rebind moved the *binding*, not memory.
> - **`H1-CALLERS` = 0.** Neither function has a standalone caller (`ScoreMasks_ah3.cpp` is
>   `.asi`-only), so `H1-KNOBOFF`'s byte-identity is near-tautological and **weak evidence**. The
>   evidence the rebind works is `H1-CONVERGE` + `H1-GATE2`.
>
> ### START HERE
>
> 1. **H1b** — `0x0046d4a0` exe body. `.asi`-only today with an empty `exe_file`, so it needs a
>    second body at one RVA: duplicate-RVA hazard, `hooks.csv` entry through `re-classify`.
> 2. **H2** — the `MASHED_SLOTSTATE_SEED` default-ON question. `H1-ALL` now shows what it buys.
>    It stops being inert once H3 exists, so the scope's §5 gates run at that point, not before.
> 3. **H3** (`FUN_00442a60`) is **still under the `DEFERRED.md:15` "DO NOT start leg B"
>    prohibition.** H1a does not lift it. The scope argues the prohibition's risk basis has changed
>    — it assumed synthesis was needed, which G1 run 2 refuted — but lifting it is a USER call.
> 4. Also still open and independent of this lane: **U-9191** item (b), **U-9195**, the
>    `0x00409b0e` jumptable.
>
> **Gate-writing note, earned twice this session** (`PREREG_H1.md` §A2): `G-TOOK` and
> `H1-CONVERGE` both failed because the instrument did not touch the thing under test. Before
> registering a gate, name the code path the change alters and the path the instrument exercises,
> and require them to be the same.
>
> **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** Blocks below are previous headlines.

> ## UPDATE 2026-10-08 (U-9186 leg G1 RAN, commits `ef420bfc`/`b8f6d1de`): **`FUN_00442a60` is unportable-to-effect today and the blocker is the PER-VEHICLE TABLE SUBSTRATE (`0x008815a4` / `0x00881f90` / `0x00881ec8`), not any one function. The registered control `G1-POS` FAILED; no port was written, by the pre-registration's own decision rule. NO C-level, nothing default-ON, `original/` untouched.**
>
> Read [`RESULT_G1.md`](../verify/d3_u9186_20261008/RESULT_G1.md).
>
> - **Measured, 53,992 rows per arm, three arms.** `G1-ALL` = **0** on all three: no row exists
>   where a ported `FUN_00442a60` would write a non-zero distance.
> - **`G1-POS` FAILED.** The per-vehicle record at `0x00881ec8 + v*0xd04 + t*0x40` reads **one
>   (x,z) for all four cars on every row**, in every arm. So the gate counts indict the substrate,
>   not `FUN_00442a60`.
> - **Three gates, three states.** Slot-state: **solvable today** via `MASHED_SLOTSTATE_SEED`
>   (3 of 4 cars read state `2`, player slot 0 stays `0`). `veh_type` (`0x008815a4 + v*0xd04`):
>   **`0` on 100% of rows in every arm, seed-independent** — the hard gate, and it had never been
>   measured before. Position: dead, no knob.
> - **`GATE3`'s 100% pass is vacuous** — a blank table passes an `== 0` test for the wrong reason.
>   Any future gate chain over this substrate must not count it.
> - **The probe is sound**: on `G1both` it reads 17,924 distinct values from `0x008a96ec` (bridge
>   on) while reading one constant from the vehicle record in the same run.
>
> ### START HERE — read the standing prohibitions first
>
> `DEFERRED.md:15` (D-11072) says **"DO NOT start leg B"** (the `FUN_00442a60` port) and
> `RACE_POSITION_RECON_SCOPE_2026-10-06.md` §4 says "Risk: high. Do not start Leg B first."
> `UNCERTAINTIES.md:61` had already recorded the static blockage on 2026-10-06. G1 re-confirmed it
> in-game and added the two gates that record never tested. **Do not re-derive this a third time.**
>
> Options, in priority order:
>
> 1. **Decide the substrate question, which is a USER call, not an analysis one.** Can the
>    standalone synthesize `0x008815a4` / `0x00881ec8` from `race_[]` + `ai_cars_`, the way
>    `0x008a96ec` was synthesized from `arcprog`? That is a bridge of the same shape and the same
>    fidelity trade-off Mariano already decided YES on for race position (`RACE_POSITION_RECON_SCOPE`
>    §1). If YES, U-9186 becomes reachable; if NO, D3 criterion (b) has no live route and should be
>    re-scoped. **[UNCERTAIN]** — nothing measures this yet.
> 2. **U-9191** item (b): car-1 body-heading residual, independent of this substrate.
> 3. **U-9195** participant-count; the `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm.** The blocks below are
> previous headlines, left as history.

> ## UPDATE 2026-10-07 (D-11072 leg E2 RAN, commits `ae906890`/`ee98e0ab`/`3dcfa32e`): **the monotone metric reaches the rule engine, reorders the cars on 29.50% of frames, and changes NOTHING else — `rmetric` is the only column that differs between arms across all 19,418 rows. The swap is INERT at the outcome level. The race-position STATE lane is FINISHED; D3's criterion (b) blocker does NOT recede into this consumer. NO C-level, nothing default-ON, `original/` untouched.**
>
> Read [`RESULT_E2.md`](../verify/d3_consumer_20261007/RESULT_E2.md). Gates:
> `PREREG_CONSUMER.md` §4 as amended by [`AMEND_E2.md`](../verify/d3_consumer_20261007/AMEND_E2.md).
>
> - **Every gate scored.** `E2-WROTE` PASS (11671/11671 in-scope rows, both arms). `E2-DIFF` PASS
>   (318/1078 3-car frames = 29.4991% vs a 1% bar). `E2-KNOBOFF` PASS — the new build's OFF arm is
>   identical to `F2a` over all 77 pre-existing columns, so the knob perturbed the default path by
>   exactly nothing. `E2-DET` PASS. `E2-NOREG-E` PASS 3/3. `E2-NOREG-B` unchanged (`v2`'s 5 bands
>   are the pre-existing baseline, guaranteed by `E2-KNOBOFF` — not a regression).
> - **`E2-EFFECT` INERT, with stronger evidence than the gate asked for.** A cell-for-cell diff of
>   the arms names `rmetric` as the *only* differing column, on all 19,418 rows; all six
>   `mashed_re.log` copies share one hash. `UpdateFinishOrder`'s output is read by nothing that
>   affects state under `rule=4`. **[UNCERTAIN]** for rules 0 and 10 — untested.
> - **`E2-LAPAGREE` NON-ZERO: 214/19418 = 1.1021%.** `AMEND_E2.md` A4's registered hazard is real —
>   the pre-registered formula pairs `race_[].laps` with a fraction derived from the separate
>   `race_[].arclaps`. Implemented as registered rather than quietly corrected. No consequence while
>   the swap is inert, but it **blocks any future ship of the ON arm**.
> - **Run 1 failed `E2-WROTE` at 60.10% on a defective witness** (a frozen slot read as a formula
>   error) and is preserved as `*_r1`. A7's `rtick` counter replaced an inferred denominator with a
>   measured one. Worth reading before writing the next witness.
>
> ### START HERE
>
> The D-11072 lane is closed as a route to criterion (b). **The named carrier is unchanged: the
> three unported `FUN_00416250` branches (U-9186).** Options, in priority order:
>
> 1. **U-9186 / `FUN_00442a60`.** E2 removes the rule engine as an alternative route, so the
>    progress-producer port is now the only live path to the AI over-speed COMMAND defect.
>    Everything it needed from the race-position substrate exists and is proven consumed.
> 2. **Re-run E2 under `rule=0` and `rule=10`** if the inertness needs generalising. Cheap: the
>    harness is built (`run_e2.ps1`, `check_e2.py`), one knob change, ~12 min for six runs.
> 3. **Redirect**: U-9191 item (b) car-1 body-heading residual; U-9195 participant-count; the
>    `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** **Do NOT ship the `MASHED_RACEMETRIC_ARC` ON arm** — inert, and A4's
> formula inconsistency is unresolved. The blocks below are previous headlines, left as history.

> ## UPDATE 2026-10-07 (D-11072 legs F1+F2 RAN, commits `79e7e2eb`/`11eea794`/`87433a24`/`e832eaf8`): **the reproducibility blocker is CLEARED. A 240 s standalone race is bit-reproducible under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` — three repeats gave BYTE-IDENTICAL stepdumps. E1's blocker was a missing harness knob, not a code defect: `run_e1.ps1` never set it. Leg E2 is UNBLOCKED. NO C-level, zero source change, `original/` untouched.**
>
> Read [`RESULT_F2.md`](../verify/d3_determinism_20261007/RESULT_F2.md), then
> [`RESULT_F1.md`](../verify/d3_determinism_20261007/RESULT_F1.md).
>
> - **F1 (`79e7e2eb`) reported a control failure, not a localization.** `R-PREFIX`'s registered
>   control `L4`-vs-`L10` came out at 1306, equal to the identical-knob pairs. Cause, measured:
>   `L4c` and `L10` differ in 38 of 7411 shared rows while identical-knob `L4b`/`L4c` differ in
>   4488. `MASHED_ROUND_RULE` is near-inert on driving state, so nothing paired against it is an
>   early-divergence control. F1 also found `L4` shares **zero** rows with `L4b`/`L4c` between
>   frames 1306 and 2306 — per-car dump membership is itself a run-to-run variable.
> - **F2 (`87433a24`) passed every gate.** `F2a`/`F2b`/`F2c` are byte-identical (SHA-256
>   `506A210F…`) across 103 s / 183 s / 91 s of wall clock. `R-ROUND` identical, 5 rounds each,
>   where E1's three identical-knob runs gave 5/3/3 rounds and three different matches.
> - **`R-PREFIX` is rehabilitated.** The control F1 lacked now exists: `MASHED_SIM_HZ=59` changes
>   `kSimStep` itself, gives `R-PREFIX` **0** against full overlap, and diverges at frame 0 on
>   exactly the predicted quantity — `round(3000/59)=51` vs `round(3000/60)=50`.
> - **One registered deviation, on the record.** `G-TOOK` FAILED as written: it read the stepdump's
>   `frame` column, which is a dump-local static (`TrackRenderer.cpp:3994`), not `g_det_frame`.
>   Corrected witness: `step_1008` is exactly 50 on all 19,418 rows of every repeat. Reading the
>   remaining gates after that failure departs from `PREREG_F2.md` §4; see `RESULT_F2.md` §1.
> - **Scope limit that carries forward.** Standalone side, scripted-capture regime only.
>   `MASHED_DETERMINISTIC` stays OFF by default and suppresses live input. **E1's captures are not
>   comparable to deterministic ones** — any baseline must be re-taken under the same knobs.
>
> ### START HERE — run leg E2 (`PREREG_CONSUMER.md` §4)
>
> E2's precondition is now satisfied by measurement. Two things must happen first:
>
> 1. **`MASHED_RACEMETRIC_ARC` does not exist in source yet** — grep finds it only in
>    `PREREG_CONSUMER.md` and this file. Implement the default-OFF knob at
>    `TrackRenderer.cpp:5132-5134` (`rc.metric[i] = race_[i].laps + race_[i].arcpct * 0.01f`),
>    rebuild, then run both arms × 3 repeats under the F2 knobs.
> 2. **`E2-KNOBOFF` needs an amendment, pre-registered before running.** As written it compares the
>    knob-OFF arm against "the committed baseline stepdump" — that baseline is E1's
>    non-deterministic capture and is **not** comparable (`RESULT_F2.md` §4). Take a fresh OFF-arm
>    baseline under `MASHED_DETERMINISTIC=1 MASHED_DET_FRAMES=14400` and compare against that. This
>    is a correction to the reference, not a loosened threshold — state it in the amendment.
>
> `E2-EFFECT` is now measurable and must NOT be declared unmeasurable. Reuse
> `verify/d3_determinism_20261007/run_f2.ps1` (waits for the `DET_FRAMES` self-exit, stops only the
> PID it spawned) and `check_f2.py` / `re/tools/det_prefix.py` for the determinism gates.
>
> **Do NOT start leg B.** The block below is the previous headline, left as history.

> ## UPDATE 2026-10-07 (D-11072 leg E1 RAN, commits `bcdd3fcb`/`4a0efcc2`/`6ade0f85`): **the race-position metric already has a LIVE consumer and it was never behind `0x007f0fd0` — it is the rule engine, gated on `TrackRenderer::rule_`, reachable today with `MASHED_ROUND_RULE` and no code change. Swapping it to leg A's monotone `arcpct` is NOT inert (28.6–38.6 % ordering change). NEW BLOCKER: round-level match outcomes do not reproduce across repeats, so E2's registered outcome gate is unmeasurable as written. NO C-level, zero source change, `original/` untouched.**
>
> Read [`verify/d3_consumer_20261007/RESULT_E1.md`](../verify/d3_consumer_20261007/RESULT_E1.md). Pre-registered UNRUN `bcdd3fcb`, RAN `4a0efcc2`.
>
> - **The finding.** The port never writes `DAT_007f0fd0` (`aib_game_mode_fd0()` is a hard
>   `return 0`, `TrackRenderer.cpp:96`), but it carries the same rule as `TrackRenderer::rule_`
>   (`exe_main.cpp:2185` → `RaceModes::RaceRule`, the verbatim `FUN_0043dfd0` cup-event table).
>   The rule engine runs default-ON every frame (`TrackRenderer.cpp:5119`) and feeds
>   `UpdateFinishOrder`/`SegmentCheck`/`EvaluateResult` a metric built at `TrackRenderer.cpp:5132`
>   from the **non-monotone** `race_[].progress` — exactly what leg A's `arcpct` replaces.
>   `MASHED_ROUND_RULE=<n>` (`exe_main.cpp:8508-8515`) reaches rule 4/10 on the existing Training
>   recipe.
> - **It executes.** 5 `RULE-EVAL` evaluations per 240 s run, rule-specific: `L0` (rule 0)
>   concludes the match (`r=4`), `L4` does not, `L10`'s timer varies per round.
> - **The swap has content.** Ordering of the 3 AI cars differs between `lap+racepct/100` and
>   `lap+arcpct/100` on 37.75/38.64/38.31/35.41/28.60 % of 3-car frames across the five runs
>   (max per-car delta 0.0499), vs the pre-registered `E2-DIFF` bar of 1 %.
> - **Two gates failed, reported as written.** `E1-EVAL` failed because the control discriminated
>   in the opposite direction to the prediction. `E1-DET` failed outright: three rule-4 runs with
>   identical knobs gave three different matches (round-2/3 scores, 5 vs 3 rounds, max lap 8/1/4).
>   Aggregate scorers stay stable — it is **round-level outcomes** that are wall-clock-sensitive.
> - **`0x007f0fd0` mirror sub-lane is cheap if wanted:** a worker survey found only three readers
>   that execute in a standalone race, all in `AiPreTickRubberBand`
>   (`AiStandalone.cpp:1469/1501/1541`), plus one frontend-only `ModeCodeLookup`
>   (`Frontend/BatchAA_s4.cpp:64`, `[UNCERTAIN]` reachability). It reaches the mode-4/9
>   `GearConstSet` AI-speed scaling, which bears on criterion (e).
>
> ### START HERE — pick one; the fork is real
>
> 1. **Make long-run outcomes reproducible**, then run E2 with its registered outcome gate.
>    This is the blocker E1 found and it unblocks every future behaviour claim over a full match.
> 2. **Run E2's measurable subset now** (`MASHED_RACEMETRIC_ARC=1`, `PREREG_CONSUMER.md` §4):
>    `E2-WROTE`, `E2-DIFF`, `E2-KNOBOFF`, `E2-NOREG-E/-B`, `E2-DET` on the deterministic 60 s
>    scenario. `E2-EFFECT` must be declared unmeasurable, not restated.
> 3. **The `0x007f0fd0` mirror** (`PREREG_CONSUMER.md` §5) — small blast radius, touches (e),
>    which has a deterministic scorer. Note it scales AI speed and could move (e) either way.
> 4. **Redirect**: U-9191 item (b) car-1 body-heading residual; U-9195 participant-count;
>    the `0x00409b0e` jumptable.
>
> **Do NOT start leg B.** The block below is the previous headline, left as history.

> ## UPDATE 2026-10-07 (D-11072 race_pct bridge write RAN, commit `275d9b26`): **the bridge is CORRECT and consumed by a LIVE reader, but behaviourally INERT at `fd0=0`. The race-position STATE problem is SOLVED (state live + proven consumed); the next blocker is the game-mode gate on the consumer, NOT the progress substrate. NO C-level, nothing default-ON, only `0x008a96ec` written, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_BRIDGE.md`](../verify/d3_racepos_20261006/RESULT_BRIDGE.md). Pre-registered UNRUN `257fcd39`, RAN `275d9b26`.
>
> - **What landed:** a default-OFF knob `MASHED_RACEPCT_BRIDGE` writes the monotone `arcpct` into
>   the original per-car race_pct slot `*(float*)(0x008a96ec + v*0x30c)` in `UpdateRace` (which runs
>   before the AI tick, so the read at `AiStandalone.cpp:1468` sees it same-frame).
> - **8/8 gates PASS.** The one that matters, `G-LIVE` (control): `val_880` (= `AiPreTickRubberBand`'s
>   `ra*0.01+lap` store) tracks `arcpct*0.01+lap` at 1.0000 on the Y arm and is **0 on every N row** —
>   the port's live AI code demonstrably consumes the bridged race position. `G-EFFECT`: (e)
>   bit-identical, (b) same 5 bands → inert. `G-INERT` 0 of 374,662 cells.
> - **Why inert:** `ra → val → 0x0089a880` drives behaviour only through `fd0 ∈ {4,7,8,9}` gates
>   (`AiStandalone.cpp:1472/1492/1505`), and the standalone's `fd0` (global `0x007f0fd0`) is 0.
>
> ### START HERE — reach the consumer (this is a game-mode-wiring problem now, not a progress one)
>
> The race position is live and consumed; to make it *do* something, the `fd0`-gated consumer must
> be reached. Two sub-lanes:
> 1. **Set `0x007f0fd0` to a championship/elimination mode (`fd0 ∈ {4,7,8,9}`)** — investigate what
>    the original writes there for a real round and whether a standalone recipe/knob can reach it,
>    then re-run the bridge arm and see if the finish-order slots / mode-4/9 scaling now move. A
>    default-OFF liveness counter on the `:1472` block first (did it fire), per the discipline.
> 2. **The `0x008a96e8` boost-order path (U-9186, a *different* field):** `Fi_UpdateBoostOrder`
>    (`ForceIntegratorStubs.cpp:176`) reads it ungated to pick the start-boost pair — but its
>    lights-window effect is predicted null (equal progress at the grid). Measure before investing.
>
> **Do NOT start leg B.** Alternatives if redirecting (over-speed not urgent): U-9191 item (b)
> car-1 body-heading residual; U-9195 participant-count; `0x00409b0e` jumptable.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (D-11072 leg A monotone metric BUILT, commit `201ab935`): **the monotonicity blocker is CLEARED — the port now has a monotone, physical-arc-length race-progress metric (`race_[].arcprog`/`arcpct`, 0 backward-midlap on all 3 cars vs the old 484/669/1273), deterministic and INERT on (e)/(b). The leg-A scale map is now a registrable near-identity. START HERE: run the §4 bridge-write pre-registration. NO C-level moved, nothing default-ON, `0x008a96ec` not written, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_LEGA_MONOTONE.md`](../verify/d3_racepos_20261006/RESULT_LEGA_MONOTONE.md). Commit `201ab935`.
>
> - **The metric.** New parallel field `race_[].arcprog` = physical arc length along the gate
>   ring: a forward-only `arcseg` pointer advanced at each gate's perpendicular crossing, within-
>   segment along-track projection `t`, weighted by a per-track cumulative-length table built once
>   from `gates_`. Dumped as `arcpct` (0..100 per lap). Read ONLY by `MASHED_AI_STEPDUMP` and the
>   future bridge — it does **not** feed finish order (`progress` still does), so it is inert.
> - **Acceptance met:** backward-midlap **0/0/0** (matching the original's 0), deterministic over
>   3 repeats, criterion (e) **bit-identical** to baseline and (b) the same 5 bands.
> - **One residual, declared:** a small **forward** jump (max 1.9%) per gate crossing from
>   polyline corners (the original's Catmull path is smooth). Monotone-preserving; a fidelity
>   ripple for ordering/threshold consumers, removable only via spline projection — not done.
>
> ### START HERE — run the §4 bridge-write pre-registration (the leg-A deliverable is ready)
>
> `RESULT_LEGA_MONOTONE.md` §4 registers the candidate map UNRUN: `arcpct` and the original
> `race_pct` are both physical-arc 0..100 per lap, so the bridge write is a near-identity
> `*(float*)(0x008a96ec + v*0x30c) = arcpct(v)` for AI cars, behind a default-OFF knob. Its
> inert-first gate (scope-doc §3/§5): a default-OFF probe confirms `FUN_00408ad0(v)` /
> `FUN_00408a50(v)` go non-zero and monotone, and the bridged ordering of the 4 cars matches the
> original's on a matched capture, **before** any consumer is trusted; then car<->car
> no-regression (G-NOREG-E/-B, G-DET, G-KNOBOFF) + modes oracle rule 3 (leg A reaches the
> elimination tiebreak) + the power-up sweep (it reaches the fire gates). Pre-register UNRUN,
> commit, then run. **Do NOT start leg B before the bridge write.**
>
> Alternatives if you'd rather redirect (the over-speed is not urgent): U-9191 item (b) car-1
> body-heading residual; U-9195 participant-count inertness; or the `0x00409b0e` jumptable.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (D-11072 legs C+A RAN, commits `69e207b9..2708befc`): **fidelity decision YES; leg C is DONE and INERT by design (8/8 gates); leg A finds the port's progress metric is NON-MONOTONE and so NOT bridge-able into the original's spline `race_pct` as-is. The new blocking prerequisite is a monotone, arc-length port progress metric. DO NOT start leg B. NO C-level moved, nothing default-ON, no bridge code, `original/` untouched.**
>
> Read [`verify/d3_racepos_20261006/RESULT_LEGC_LEGA.md`](../verify/d3_racepos_20261006/RESULT_LEGC_LEGA.md).
> Pre-registered UNRUN at `69e207b9`; RAN result `2708befc`. New tool `re/tools/racepct_scale.py`.
>
> - **USER DECISION (Mariano):** a bridged, non-bit-identical race-position substrate is
>   acceptable in the default build, knob-gated, **no C-level** follows. Recorded in the scope
>   doc §1 and the prereg §0.
> - **Leg C — DONE, 8/8 gates, behaviourally INERT (as predicted).** `MASHED_SLOTSTATE_SEED=1`
>   seeds `*(u32*)0x005f2770 = 0x005f2728` once at boot; the literal `FUN_0040e470(v)` then reads
>   the poked `2` on every seeded row and the deref-free control `ss_raw` refutes the null. It
>   changes **no** (e)/(b) behaviour because the standalone's only consumer of the concept is the
>   synthesized constant `aib_veh_type` = `v==0?0:2` (`TrackRenderer.cpp:93`). The seed is
>   bit-faithful (it restores a `.data` initializer the binary carries) and safe default-ON, but
>   buys nothing alone. **The behavioural step is C2** — re-point `aib_veh_type` at the table so an
>   unmarked car reads 0 (death-aware) — **not written, not run.**
> - **Leg A — the pre-registered NOT-COMMON verdict stands, via a corrected (stronger) rationale.**
>   The flat-frac gate I registered is degenerate (original flat_frac is 0 exactly, so `5×0` is an
>   impossible threshold) and the staircase prediction was refuted. The real obstruction, measured
>   as declared collateral: **within-lap monotonicity** — the original `race_pct` has **0** backward
>   steps per car; the port `racepct` = `fmod(laps*n+gate+frac, n)/n*100` runs **backward on
>   484/681/1287 moving steps** per car because `frac` keys on euclidean distance to the gate
>   **center** off the racing line. A non-monotone metric can't feed the original's spline
>   `race_pct` consumers.
>
> ### START HERE — the one lane that unblocks D-11072, or pick another
>
> 1. **Build a monotone, arc-length-proportional port progress metric** (project the car onto the
>    AI spline the port already drives, instead of blending distance to gate centers), then re-run
>    `re/tools/racepct_scale.py lega` — if its collateral monotonicity shows ~0 backward steps,
>    leg A's scale map becomes registrable and leg B is unblocked. This is now the gate for the
>    whole race-position bridge (and its wider payoff: the AI powerup fire-gate chain + mode-3/7).
> 2. **Ship step C2** (re-point `aib_veh_type` at the seeded table, default-ON knob) — small, and
>    it is the only behavioural content leg C has. Gate it on (e)/(b) + determinism like car<->car.
> 3. **Or a different lane entirely** (the over-speed is not urgent — (e) MET, (b) 5 bands):
>    U-9191 item (b) car-1 body-heading residual; U-9195 participant-count inertness re-measure;
>    or recover the `0x00409b0e` jumptable and finish save-acceptance.
>
> The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (U-9185 re-baseline + U-9186 measure-inert-first, commits `19e6b2e1..dea053ba`): **car<->car kept default-ON; the ship already cut the over-speed ~a third; porting the over-speed COMMAND branches is INERT, AND so is porting their producer — the fix recedes two layers into a race-position reconstruction. START HERE: pick another lane, or scope that reconstruction deliberately.**
>
> No C-level moved anywhere in this run. No code shipped by the U-9185/U-9186 work (the
> car<->car KEEP was a decision only; the knob already defaulted ON).
>
> - **KEEP decision (Mariano):** `MASHED_CARCAR_CONTACT` stays default-ON (ROADMAP §D3,
>   `19e6b2e1`). No code change — it already shipped that way.
> - **U-9185 re-baselined** (`verify/d3_overspeed_20261006/REBASELINE.md`, `a9fce015`): the
>   shipped car<->car contact cut the AI window over-speed from +41/+40/+34 % to **+26/+28/+24 %**
>   — ~a third, the largest single reduction on this metric, a side effect of the steering gain.
>   U-9185's `3421/3346/3496` is stale. The **COMMAND attribution is reconfirmed on the shipped
>   build** (`ai_speed_onset.py`): onset COMMAND on all 3 cars, physics exonerated <1 % pre-onset.
> - **U-9186 measure-inert-first** (`verify/d3_overspeed_20261006/RESULT_U9186_INERT.md`,
>   `37ed351f`): **porting the two portable `FUN_00416250` branches would be INERT and no code was
>   written.** Both read per-car progress/rank state that has NO standalone writer (BRANCH 1
>   `FUN_00408a50` @ `0x008a96e8`; BRANCH 2 `RefDist`/`FUN_00442cc0` @ `0x008989b0`, leader
>   `0x0089a364`, catch-up `0x0089a4c4/4c8`) — all blank-mapped zeros, documented in the port's
>   own header `AiStandalone.cpp:704-709`.
>
> ### THE PREREQUISITE recedes TWO layers — it is a race-position reconstruction, not a producer port
>
> Checked 2026-10-06 (`verify/d3_overspeed_20261006/RESULT_U9186_PRODUCER.md`, `dea053ba`):
> porting **`FUN_00442a60`** (`Spectator::ComputeDistances`) does **not** unblock it. Its call
> site exists (the race camera `FUN_00446520` = `Race/RaceCamera.cpp` calls it) and its distance
> math uses live positions, but its reference-car **selection** reads state the standalone never
> produces — `FUN_0040e180` reads the `0x005f2770 → 0x005f2728` slot-state table (a load-time
> `.data` pointer the standalone never loads; base 0 → AV/guarded-zero, documented
> `AiStandalone.cpp:1387-1403`) and `FUN_00408ad0` reads per-car progress `0x008a96ec` (writer
> `FUN_00408610`, unported). So it writes **zeros** → `RefDist` stays 0 → the branches still
> never fire. **The real requirement:** reconstruct the standalone's race-position bookkeeping —
> (a) per-car progress `0x008a96ec` (`FUN_00408610`, or a spline-derived standalone progress
> since the AI already drives the splines), AND (b) the `0x005f2728` slot-state table (allocate
> + point `0x005f2770` + fill). A sizeable dedicated effort, pre-registered on its own, with a
> default-OFF gate-fire counter proving `RefDist` goes non-zero before any behaviour wiring.
>
> ### The race-position reconstruction is now SCOPED — DEFERRED D-11072
>
> `re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md` (`33633860`) lays out the effort as
> three legs with per-leg inert-first checkpoints: **C** slot-state table (CHEAP — boot-seed
> `*(u32*)0x005f2770=0x005f2728`), **A** progress bridge (MEDIUM — the port already has
> `race_[].progress`, but it is a gate ordinal vs the original's spline `race_pct`, so a
> pre-registered scale map and a **USER fidelity decision** are needed), **B** reference
> distance (EXPENSIVE — build on A+C). Payoff is wider than the over-speed: live RefDist
> unblocks the whole AI powerup fire-gate chain and the mode-3/7 producer. **To execute it:**
> make the §1 fidelity decision, then start with leg C's liveness probe + leg A's scale
> measurement (one session, no behaviour risk) before committing to leg B.
>
> ### Or pick a different lane — the over-speed is not urgent
>
> The command defect is blocked two legs deep behind D-11072, and the car<->car ship already
> took a third out of it ((e) MET, (b) 5 bands). Alternatives:
> 1. **U-9191 item (b)** — car 1's body-heading physics residual (~0.89 deg at matched
>    position), the genuinely-physics path left on (b).
> 2. **U-9195** — is the car<->car change inert outside 4 participants? (a bounded re-measure).
> 3. **Recover the `0x00409b0e` jumptable** and finish save-acceptance.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (U-9196 RESOLVED, commits `94485a1c`+): **the car<->car (b) movement is GENUINE behaviour, not a shallow coincidence — verdict PARTIAL. The car<->car lane is COMPLETE.**
>
> **No C-level moved** — `0x00469df0` stays C2. No game run, no build. Read
> [`verify/d3_carcar_20261005/RESULT_U9196.md`](../verify/d3_carcar_20261005/RESULT_U9196.md).
>
> - The feared failure (low median WITHOUT the original's correction tail — "smooth-but-not-
>   cornering") is **REFUTED on both cars**. The original steers bimodally (low median + fat
>   rail-correction tail); both port cars reproduce that shape. New tool `re/tools/ai_steerdist.py`.
> - Verdict **PARTIAL**: **car 1 GENUINE** (exact median match 7=7, both tail gates pass);
>   **car 3 SHALLOW on `G-SHAPE-MED` alone** (median 11 vs 23) — but its tail passes in full,
>   so it corners for real and merely under-steers its reference by ~half. Car 1 also jitters
>   more between corrections (distinct 83 vs 33). Position-matched collateral (R=0.12): median
>   steering diff 7.0 (car 1) / 10.0 (car 3).
> - **Does NOT close (b)**: car 2 still fails entirely, one Training recipe, U-9195's
>   participant-count caveat untouched, C2 unchanged.
>
> ### THE ONE OWED ITEM — a USER DECISION, not more analysis
>
> **Keep or revert `MASHED_CARCAR_CONTACT` default-ON.** It is a genuine step toward the
> original's AI steering on 2 of 3 cars (exact on car 1), regresses no gated (e) statistic, and
> costs car 2 one band (4 → 5). It ships in the default build today. The RESULT lays out the
> trade; the call is the user's.
>
> ### The car<->car lane is otherwise COMPLETE — pick a new lane
>
> Legs 1–3 + U-9196 all landed (`3404612d..94485a1c`). The remaining D3/physics options:
> 1. **U-9185 — the 1.3258x distance over-run** (`verify/d3_noboost_20261003`), the +15..31 %
>    AI over-speed inside the window. The only substantive physics path left on (b), and it is
>    a separate carrier from the steering U-9196 just addressed.
> 2. **U-9195 — is leg 2/3 inert outside 4 participants?** Re-run the car<->car arms at a
>    participant count outside {4,8,9}, with a default-OFF counter on the drafting branch first
>    (so "did not fire" separates from "fired and was overwritten").
> 3. **Recover the `0x00409b0e` jumptable** and finish the save-acceptance question.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (CAR<->CAR LEG 3, commits `523cc5d9`+): **the pair loop is WIRED, 8 of 8 gates PASS, and the contact deterministically moves (b) from 13 failing bands to 5.**
>
> **No C-level moved** — `0x00469df0` is still C2; a call site is not a behavioural diff.
> **`original/` untouched.** 73 insertions / 0 deletions in one file. exe SHA-256
> `9C09212FD227F6906C546DE9117EF57E4464A4F8C6F0AC5E639B6A5DD853CFDC`. Default-ON, revert
> `MASHED_CARCAR_CONTACT=0`. Read
> [`verify/d3_carcar_20261005/RESULT_LEG3.md`](../verify/d3_carcar_20261005/RESULT_LEG3.md).
>
> - **8 gates PASS:** G-CALLED (358 entries, 24 non-zero returns), G-PAIR (j>i 358/358),
>   **G-NOOPP (exactly 0 under `MASHED_MEASURE_NOOPP=1`** — opponents unstepped → their
>   `[+0x9a8]` slot stays zero → proximity fails), G-NOREG-E (gated (e) digit-identical 3/3),
>   G-NOREG-B (5 bands, ≤13), G-BANDS-UNEDITED, G-KNOBOFF (reverts to 13 bands + baseline
>   digits), G-DET (3 ON runs identical).
> - **THE SURPRISE:** (b) 13→5 bands, deterministic. Cars 1 and 3 enter the original's
>   `abs_steer_median` envelope `[0,23]` (52.5→7.0, 49.0→11.5); car 2 stays out (57.5→44.5,
>   4→5 bands). **Not a stuck-car artifact** — window speed 3057/3254, near-stopped 2/1 of 220,
>   same as OFF; the cars race ~90% speed while steering ~7x less (cleaner line).
> - **It is NOT a (b) claim yet (U-9196).** Mechanism unknown; a median in the envelope is not
>   the per-frame distribution matching it; the 2026-10-02 matrix tested AI *command* levers,
>   never this *physics* lever. **The one thing that decides it: a cross-side per-frame
>   comparison of the ON arm's steering series against the original's
>   `verify/d3_elim_20261003/o_t*.msd.aistep.csv`.**
>
> ### START HERE — close U-9196, which is the live question behind a possible (b) win
>
> 1. **Cross-side steering-series check.** Compare the ON arm (`verify/d3_carcar_20261005/L3on1.csv`,
>    cars 1 & 3) per-frame against the original's `o_t*.msd.aistep.csv` at matched position
>    (`ai_posmatch.py`), not just the median. If the *distribution* matches, this is a real (b)
>    movement on two cars from the car<->car physics; if only the median coincides, it is not.
>    Memory `score-the-bands-from-endpoint-first`, `a-band-scored-off-regime-is-not-a-measurement`.
> 2. **Mechanism probe.** A default-OFF probe on which record field the contact writes
>    (`+0x144/+0x148/+0x14c` angular, `+0x9b0..` linear) that changes the heading the AI steers
>    from — reasoned story is not a measurement.
> 3. **Then the keep/revert decision on `MASHED_CARCAR_CONTACT` default-ON is the user's**, given
>    a possible (b) improvement on 2 of 3 cars with car 2 slightly worse.
>
> **Open caveat from leg 2 (U-9195):** the ring-slot publication leg 3 relies on is inert on
> the 4-participant recipe only; outside `g_playerCount` ∈ {4,8,9} the drafting term is live.
> Leg 3 ran at 4 participants, so that is untested here too.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-06 (CAR<->CAR LEG 2, commits `283758b7`+): **ring slot `[+0x9a8]` is published, 6 of 6 gates PASS, (e) and (b) did not move a digit — and LEG 3 IS NOW UNBLOCKED.**
>
> **No C-level moved. No band moved. `original/` untouched.** 49 insertions / 0 deletions in
> one file; the pre-existing `[+0x9ac]` write is byte-for-byte unchanged. exe SHA-256
> `ABE7BA36035B94C8294158B2071FE2E7FD6D3160D99EC6EB10AECD8F8D31E3A2`. Read
> [`verify/d3_carcar_20261005/RESULT_LEG2.md`](../verify/d3_carcar_20261005/RESULT_LEG2.md).
>
> - **Gates:** `G-NOREG-E` PASS (`launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0`
>   2550.6 / 2053.0 / 2278.2 identical on every digit, 3 of 3 cars x 3 of 3 runs);
>   `G-NOREG-B` PASS (**exactly 13 of 30**, 5/4/4, same band names); `G-BANDS-UNEDITED`,
>   `G-KNOBOFF`, `G-DET` PASS; **`G-SLOT0-LIVE` PASS** — slot 0 goes from **0 of 7775** rows
>   agreeing with `own_x`/`own_z` to **0.999618 / 0.999624 / 0.999625**. The knob
>   (`MASHED_RING_SLOT0=0`, one read site) is a true revert.
> - **My pre-registered "this is NOT inert" expectation FAILED, and that is the real output.**
>   The prereg named three readers; there are **four**. Two (`PhysicsChainHooks.cpp:536`,
>   `:2749`) are in an **asi-only TU** and never execute in the exe. One
>   (`VehicleControl.cpp:103`) forwards the pointer to `Vehicle_Integrate2`, whose exe body
>   declares it **unused** (`Integrate2.cpp:123`). The fourth — **`ForceIntegrator.cpp:112-118`,
>   A5 Phase 2's drafting grip term, never named in the prereg** — is live and its `delta`
>   really did go from identically `(0,0,0)` to real inter-car vectors, but its only output
>   `local_70` is unconditionally overwritten by the original's own law at
>   `ForceIntegrator.cpp:137` (`if (g_playerCount == 4) local_70 = 1.0f`), and this recipe has
>   `participants=4`. **So the no-change gates are predicted, not lucky.**
> - **New row U-9195**: outside `g_playerCount` ∈ {4, 8, 9} that fourth reader is LIVE and leg 2
>   is **not** inert. It does **not** block leg 3 (same 4-participant recipe). Its path to
>   resolution starts with a default-OFF counter on the `kDraftDot <` branch, so "did not fire"
>   is distinguishable from "fired and was overwritten".
>
> ### START HERE — run LEG 3, the pair loop. It is pre-registered and unrun.
>
> [`PREREG_CARCAR.md`](../verify/d3_carcar_20261005/PREREG_CARCAR.md) §4. Insert the pair loop
> **immediately before the `break;` that ends the `for (int pass = 0; pass < 2; ++pass)` retry
> in `Vehicle/VehiclePhysics_StepCar`** — it was `Vehicle/VehiclePhysicsRun.cpp:1093` before
> leg 2 and is **`:1142`** after it (re-locate it by the `// Not-contacted path` comment two
> lines above rather than by the number), calling
>
> ```
> Collision::VehicleCarCarContact(recJ, recI, pass)   // 0x00469df0 via 0x00470bcd
> ```
>
> **with `recJ` FIRST** — the other car, `in_EAX`; `recI` second — the self car, `param_1`.
> Default-ON with the revert `MASHED_CARCAR_CONTACT=0`. The five gates of `0x00470b23..0x00470bc1`
> in order, `Fi_GameMode()` returns 6 which is in `{6,7,10,0xb}`, and `g_participantCount` is
> the count. Gates: `G-CALLED` (>= 1 entry **and** >= 1 non-zero return, with the `(i,j)`
> histogram), **`G-NOOPP` (exactly 0 entries under `MASHED_MEASURE_NOOPP=1` — the control that
> can fail)**, `G-PAIR`, then `G-NOREG-E` / `G-NOREG-B` / `G-BANDS-UNEDITED` / `G-KNOBOFF` /
> `G-DET` re-run against **leg 2's** numbers, not the pre-leg-2 ones. **Registered: a call site
> is not a behavioural diff, so NO C-level moves on `0x00469df0`.**
>
> **Do not assume it closes (b).** The 2026-10-02 matrix had no arm passing (b) on any car, and
> `ROADMAP.md:1929-1932` records the opponents already moving the player's median speed
> **2538 → 691** with `0x00469df0` never running — shared mutable state, a different question.
> And read `RESULT_LEG1.md` §4 / this block before suspecting the C2 transcription: memory
> `broadphase-standin-plane-test-was-the-sink`.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (CAR<->CAR CALL SITE, 3 commits `3404612d..749f60ab`): **the call site is fully decoded, leg 1 passed 6 of 6, and the wiring is blocked on ONE unpublished ring slot.**
>
> **No C-level moved. No band moved. No (e)/(b) run. `original/` untouched.** The only
> source change that ships is 19 **appended** columns on the default-OFF
> `MASHED_AI_STEPDUMP` plus comment corrections. Both builds clean.
>
> Read, in order: [`re/analysis/CARCAR_CALLSITE_2026-10-05.md`](analysis/CARCAR_CALLSITE_2026-10-05.md),
> then [`verify/d3_carcar_20261005/PREREG_CARCAR.md`](../verify/d3_carcar_20261005/PREREG_CARCAR.md)
> and [`RESULT_LEG1.md`](../verify/d3_carcar_20261005/RESULT_LEG1.md). **Do not re-derive
> what they establish**, in particular do not hunt the hull producer again.
>
> ### Settled, with citations
>
> - **One call site**: `FUN_004709a0` @ `0x00470bcd`. `--callers` and `--datarefs` agree
>   (both were run; `--callers` returns `(none)` for Ghidra-missed callers).
> - **Argument order is the trap.** `ECX` is the `__thiscall` `this` and holds the **self**
>   car; `EAX` holds the **other** car. The port binds `vehA` to `in_EAX`, so the call is
>   **`VehicleCarCarContact(rec(j), rec(i), pass)` — other car FIRST.** `param_2` (the `j`
>   index) is unused. The TU header and both plates said `in_EAX` was `this`; corrected.
> - **Insertion point: one statement**, `Vehicle/VehiclePhysicsRun.cpp:1093`'s `break;`.
>   Both of the original's fall-through paths into the `j` loop converge there; its third
>   path already matches the port's `continue;` at `:1088`.
> - **The hull chain is ALREADY LIVE in `mashed_re.exe`** and is exact. `+0xa28..+0xa54` is
>   world points 4..7 of the 18-point array `FUN_00469aa0` transforms into `+0x9f8`,
>   body-sourced from `FUN_0046b1c0`'s AABB top face — so its three edge lengths are
>   invariants of the 6-float box: `0.437600 / 0.977100 / 1.070616`. New tool
>   `re/tools/hull_invariants.py`: **10,278 of 10,278** original frames (4 captures, 2 cars)
>   and **7772 of 7772** port rows within `1e-3`, median deviations `<= 1.9e-07`.
>   `Collision/ContactStubs.cpp`'s "producer is NOT yet identified" was a **stale comment**
>   and is fixed; **U-9155's own row is a different producer (the box) and stays OPEN.**
>
> ### THE BLOCKER, and it is the whole next session
>
> **Ring slot `[+0x9a8]` is never published in the port.** Written once at
> `Vehicle/VehicleInit.cpp:252`; `SyncContactRingMatrix` publishes through `[0x9ac]`
> instead. Measured: slot 0 all-zero on **7775 of 7775** rows of `P1.csv` (and `rec_958`
> was already 0.0 on 7705 of 7705 rows of `verify/d3_arm_20261005/A1.csv`;
> `re/tools/ai_yawrate.py:71-73` already says so, so **U-9191 leg 3 is unaffected — do not
> re-file this**). Consequence: the proximity gate at `0x00470b44..0x00470ba9` compares two
> `(0,0,0)` centroids and reduces to `0 < radSum²` — **true for every pair at every
> substep** — and `CarCarContacts.cpp:93`/`:97`'s angular lever arms become absolute world
> positions. **So leg 3 is not startable before leg 2, and that is a result, not a
> scheduling note.**
>
> ### START HERE — run LEG 2, which is pre-registered and unrun
>
> `PREREG_CARCAR.md` §3. Publish the ring into **both** slots in `SyncContactRingMatrix`,
> default-ON with the revert `MASHED_RING_SLOT0=0`, **and gate it ALONE with no car<->car
> call added.** It is **registered in advance as NOT inert**: `Vehicle/VehicleControl.cpp:103`,
> `Vehicle/PhysicsChainHooks.cpp:536` and `:2749` all read `[0x9a8]*0x40 + 0x928` and
> currently receive a **zero matrix**, so A4's and A6b's `wheelBlock` input changes inside
> the D2-certified player solver (`Vehicle/BodyOrientationIntegrate.cpp:171-179` already
> flags that reconciliation as owed).
>
> Five gates plus the inverse control `G-SLOT0-LIVE`, whose "before" value leg 1 has
> already established: `G-NOREG-E` (`launch` 1426.4 / 2053.0 / 2055.2 and `ft_median_m0`
> 2550.6 / 2053.0 / 2278.2, 3 of 3 cars), `G-NOREG-B` (<= 13 of 30 bands), `G-BANDS-UNEDITED`,
> `G-KNOBOFF`, `G-DET` (3 repeats). **Registered FAIL clause: if `G-NOREG-E` or `G-NOREG-B`
> fails, revert the commit and report — do not flip the default or widen a threshold.
> Leg 3 does not run.** Port capture recipe is `re/tools/sa_capture.py` as used for
> `P1`/`B1` in `RESULT_LEG1.md` §1.
>
> **Do not assume any of this closes (b).** The 2026-10-02 matrix had no arm passing (b) on
> any car, and `ROADMAP.md:1929-1932` records the opponents already moving the player's
> median speed **2538 → 691** with `0x00469df0` never running — that coupling is shared
> mutable state and is a different question.

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (LONG SESSION, 38 commits, `cc79f0ba..da9ac1ea`): **two things shipped, four retracted or voided, and one strategic map built.**
>
> **No C-level moved all session. No band moved. `original/` clean throughout.** The one
> behaviour-changing edit is in the exe's save path and passed 5 of 5 gates.
>
> ### What SHIPPED, and is the only thing to build on
>
> 1. **`Save/GameSave.cpp` is in `mashed_re.exe` and its four functions are LIVE** —
>    `0x004099e0` `SaveStatusClear`, `0x00404e50` `SaveLoad`, `0x00404f50` `SaveWrite`,
>    `0x00404f80` `SaveFileExists`. `RESULT_GAMESAVE_EXE.md` (4/4 gates) then `RESULT_WIRE.md`
>    (5/5). `Race/GameFlow.cpp` now routes its save/load through them; the standalone had **two**
>    save images and now has one. **G-LIVE proved they are called**, not merely linked —
>    `GameSave_LastReadBytes() == 151456`.
>    **Deviation, registered:** the exe build substitutes CRT file ops and private storage for three
>    callees that live in unmapped space. **The four rows keep the C4 they earned on the `.asi`; no
>    C4 is claimed for the exe copy.**
> 2. **A live bug fixed in `re/tools/gamesave_parse.py`:** `PROFILE_SIZE` was `0x2443C` (148,540)
>    with a comment saying 150,076. The right value is **`0x24A3C`**. The sizes still summed to the
>    file size, so nothing failed — the profile/tail boundary just sat `0x600` too low and every
>    caller got 600 hex bytes of profile prepended to `tail`. 16/16 tests pass.
>
> ### THE STRATEGIC FINDING — read this before planning any port work
>
> **`re/analysis/ASI_ONLY_TRIAGE_2026-10-05.md`.** Of 888 C3/C4 rows with a source file, **619 build
> only into the dev `.asi`**, which never ships. By subsystem the unshipped verified work is
> render 247, audio 142 (1 shipping), gameplay 129 (7 shipping), save 29 (5).
>
> **But do NOT bulk-add them to `exe_sources.rsp`.** `RH_ScopedInstall` is a **no-op in the exe**
> (`Stubs/HookSystemNoOp.cpp:19`), so a linked body is a **dead export unless something calls it by
> name** — and `ROADMAP.md:235-240` already rejected the bulk-link idea. **The lever is CALL SITES.**
> Proof it is real: `Collision/CarCarContacts.cpp` is in the exe with a byte-faithful `0x00469df0`
> and **zero call sites**.
>
> ### What was RETRACTED or VOIDED, so it is not rebuilt
>
> - **U-9194 (omega-arm divergence) — RETRACTED the same day it was filed.** The port's arm is
>   **correct**: `+0x10 == 0` on **222 of 222 moving frames** across three captures, and **220 of
>   220** inside the (b) window. The "70.60 %" came from a denominator dominated by **2558 frames of
>   a parked, eliminated car**. `RESULT_ARMRETRACT.md`.
> - **U-9191's generated-vs-accumulated: still OPEN**, and now for a measured reason — the position
>   join's **median** induced heading error is **0.087865 deg**, so any gate on it needs a threshold
>   above ~0.88 deg. Leg 3's 0.9866 deg still clears at 11.2x.
> - **The save-acceptance question is UNANSWERED after eight steps.** Bounded to one named unknown:
>   the **`0x00409b0e` jumptable** (`switchdataD_00409e40`, "Too many branches") **and its caller**.
>   `RESULT_TICKCHAIN.md` has the full chain; the save flow is a **six-variable state cluster**
>   (`DAT_008a9584/9588/958c/9590/9594/9598`), not one selector.
>
> ### Harness and tracker changes you can rely on
>
> - `scenario_launch.py --no-warp` (boot to menu, hold) and `--poke-u32 rva=val` (contrived, C3).
> - `orig_nav_hold.py` honours `MASHED_ROOT`, has a `peek`, and a `--scan`. **Nav IS validated**
>   (depth 1→2 on push); the earlier "push doesn't navigate" note was my missing-baseline error.
> - `re/tools/`: `ai_armrate.py`, `ai_armregime.py`, `asi_only_triage.py`, `gamesave_spandiff.py`,
>   `stale_uncertain_refs.py`.
> - **`UNCERTAINTIES.md`: 34 struck-ID rows moved to Resolved**, 6/6 gates including a byte-identical
>   multiset check. Active 3099→3065. **The "1362 marker-resolved" figure I published mid-session was
>   a regex artefact** — the real Type-cell count is **18**, and **no automatic rule is safe**
>   (`U-9191`'s Type cell matches "RESOLVED" while the row is open).
>
> ### START HERE — pick one
>
> 1. **Give an already-ported body a call site.** The highest-leverage item in the triage, and
>    `0x00469df0` is the worked example sitting in the exe doing nothing.
> 2. **Close the 1.3258x distance over-run** (U-9185) — the only substantive physics path left on
>    (b), and the reason U-9191's comparisons return opposite signs under different controls.
> 3. **Recover the `0x00409b0e` jumptable** and finish the save-acceptance question.
>
> **Do not assume any of these closes (b).** The 2026-10-02 counterfactual matrix had **no arm
> passing (b) on any car**.
>
> ### The methodological thread, because it cost most of the day
>
> Four separate confident claims of mine needed retracting, and **every one was caught by a second
> instrument or a control, never by re-reading**: a denominator over the wrong population; a gate
> whose LIVE branch was unsatisfiable at the instrument's sampling rate; a control whose expected
> value was indistinguishable from the default; and a regex matching anywhere in a row.
> **Write the control that can fail, and run it before believing the main arm.**

The block below is the previous headline, left as history.

> ## UPDATE 2026-10-05 (D3 ARM RETRACTION, commit `95c0db13`): **U-9194 is RETRACTED hours after being filed. The port's omega arm is CORRECT for a driving car — 222 of 222 moving frames on three captures.**
>
> **Read [`verify/d3_arm_20261005/RESULT_ARMRETRACT.md`](../verify/d3_arm_20261005/RESULT_ARMRETRACT.md)
> BEFORE the ARM block below, which it supersedes on every arm claim.** No C-level moved, no band
> moved, no code behaviour changed (the only source edit is comment-only). Tool
> `re/tools/ai_armregime.py`.
>
> - **`+0x10` is NOT a per-frame fork. It has exactly ONE transition.** 0 for frames 0..1064, then 1
>   for 1065..3622. `+0x4` is its exact complement, `+0x2c`/`+0x30` go `0 -> 50`, four per-wheel pairs
>   go `(0.15, 0.0125) -> (0.25, 0.25)`, and that single step moves **135 of 833** record dwords with
>   an **identical fingerprint on all three captures**.
> - **The 70.60 % figure measured a PARKED CAR.** The capture is `--mode 10`; car 1 is **eliminated**
>   at the transition and never moves again — 23.548 of distance in frames 800..1199, then **0.000**
>   across the remaining 2423 frames. `+0x10` flips to 1 exactly when it stops.
> - **The right denominator, with a held-out confirmation** (criterion fixed in writing before
>   `o_t2`/`o_t3` were opened): `+0x10 == 0` on **222 of 222 = 100.000 %** of moving frames on
>   `o_t1`, `o_t2` **and** `o_t3`. **Zero** moving frames take the other arm. Inside the (b) window
>   (frame_idx 845..1064): **220 of 220**. So where U-9191's 0.9866 deg is measured, **the arms agree.**
> - **Do NOT port a `+0x10` producer.** An always-zero `+0x10` selects exactly the arm the original
>   uses while the car moves. `BodyOrient_OmegaFromAngVel` having no call site is **not** a defect.
> - **KEEP THIS:** **do not lengthen the (b) window past frame_idx 1064.** The existing refusal was
>   statistical; it now has a mechanism — a longer window averages a driving car with a parked one.
> - **The lesson, and it is the session's real output:** *stating a denominator is not the same as
>   checking it is the right population.* G-ARM spelled its denominator out and still chose the wrong
>   one.
>
> ### START HERE — U-9191 is unchanged, and neither remaining path is "add a column"
>
> 1. **Close the 1.3258x distance over-run** (U-9185 / `verify/d3_noboost_20261003`, the +15..31 %
>    speed defect), so frame-matched and distance-matched comparisons stop returning opposite signs.
>    **This is now the only substantive physics path of the two.**
> 2. **Or build a frame-marked join** (memory `next-sample-pairing-needs-a-frame-marker`). This is
>    instrument work: the position join's measured **median** induced heading error is **0.087865 deg**,
>    which caps any admissible claim at ~0.88 deg and so rules out the sub-degree tercile test.
>
> **Do not assume closing either closes (b).** The 2026-10-02 counterfactual matrix had **no arm
> passing (b) on any car**.

The block below is this session's EARLIER headline. **Its arm claims are RETRACTED** by the block
above; its G-INERT, G-ONSET and G-SPAN results stand.

> ## UPDATE 2026-10-05 (D3 ARM, commit `d0ce578e`): ~~The port and the original take DIFFERENT omega arms — 70.60 % vs 0 %~~ — **RETRACTED, see above.**
>
> **No C-level moved, no band moved, no hooks.csv change, no D2 WATCH.** Read
> [`verify/d3_arm_20261005/RESULT_ARM.md`](../verify/d3_arm_20261005/RESULT_ARM.md);
> `PREREG_ARM.md` was committed **unrun** at `5f3d2bdd` and **amended unrun** at `87531150` (G-PORTARM
> added after G-ARM failed and before the capture ran). New row **U-9194**; **U-9191 amended**.
> New tool `re/tools/ai_armrate.py`. Anchor verified before arming.
>
> - **G-ARM FAIL, and the failure is the finding.** `FUN_0046e9e0` forks on record `+0x10` (the
>   plate's `ESI[4]`). The **original** takes the non-zero **torque-seed** arm on **2558 of 3623**
>   frames (**70.60 %**) and the zero **steer** arm on 1065 of 3623 (29.40 %), measured offline on the
>   committed `o_t1.msd`, car 1, an AI car. `VehiclePhysicsRun.cpp:1001` calls
>   `BodyOrient_OmegaFromSteer` **unconditionally**, so the **port** takes the zero arm on **100 %** of
>   substeps — and `BodyOrient_OmegaFromAngVel` has **ZERO call sites anywhere**.
> - **G-PORTARM MISMATCH-UPSTREAM — do not "fix" this at the caller.** The port's `rec_10` is `0` on
>   **220 of 220** windowed rows and **no file under `mashedmod/src/` writes record `+0x10`** (both
>   sites are reads: `Integrate2.cpp:527`, `PhysicsChainHooks.cpp:2139`). Wiring the gate against an
>   always-zero field reproduces exactly what the port already does. **The missing port is the `+0x10`
>   PRODUCER, and that is new reversing.** `o_t1.msd` gives a per-frame expected series to validate a
>   candidate producer against: **2558 / 1065 of 3623**.
> - **G-ACC was NOT run**, per G-ARM's own registered FAIL clause. **Do not compare `+0x144`/`+0x148`/
>   `+0x14c` cross-side until U-9194 is resolved** — the arms provably differ, so it is the
>   torque-vs-rate error in another costume.
> - **The position join is 3.5x too coarse for a sub-degree gate, and this is now measured.** Its
>   **median** induced heading error is **0.087865 deg** over the 105 matched pairs (0.088920 on the
>   103 that move, so not a construction zero). **Any future gate on this join needs a threshold above
>   ~0.88 deg.** Leg 3 is unaffected — its 0.9866 deg clears it at **11.2x**. Recorded but **not
>   adjudicated**: `PREREG_LEG3.md` §5c's *"median 0.0000 deg"* for the same quantity lands within
>   2.5 % of `RESULT_LEG3`'s own **p90 of 0.0902**.
> - **G-SPAN passed its clause and must NOT be quoted as a finding.** 57.6269 deg against >= 0.50,
>   frame-count matched — but the port covers **1.3258x** the arclength in the same 220 frames, and
>   over the common spatial span the difference is non-monotonic and **ends at the opposite sign**
>   (-9.7987 deg). Per-tercile ratios swing **2.09 / 12.77 / 0.53** because at the same tercile the two
>   sides are on **different stretches of road**. **COMBINED = INCONCLUSIVE**, as the pre-registered
>   rule requires.
> - **Instrument inert:** four appended columns (`rec_144`, `rec_148`, `rec_14c`, `rec_10`),
>   **0 mismatching cells of 323022** (42 cols x 7691 rows) against the committed `L1.csv`.
> - **Chased and NOT filed:** the exactly-90.0000 deg start-heading gap is `atan2(0,0)` on the port's
>   first windowed row; its second reads -90.2997 against -90.0000. Guarded, nothing affected.
>
> ### ~~START HERE~~ — SUPERSEDED. Item 1 below is RETRACTED; use the block at the top of this file.
>
> U-9191 cannot be settled by another column or a tighter radius. The two viable paths:
>
> 1. ~~**Port the `+0x10` producer (U-9194).**~~ **RETRACTED 2026-10-05 — DO NOT DO THIS.** The arms
>    agree on every frame where the car drives (222 of 222, three captures), so an always-zero `+0x10`
>    is already the correct arm and no producer is owed. The "2558 / 1065 of 3623 series" named here as
>    a validation target is a **parked, eliminated car**. See `RESULT_ARMRETRACT.md`.
> 2. **Or close the 1.3258x distance over-run first** (U-9185 / `verify/d3_noboost_20261003`, the
>    +15..31 % speed defect), so frame-matched and distance-matched comparisons stop returning
>    opposite signs and G-SPAN-shaped evidence becomes readable at all.
>
> **A frame-marked join** is the third option and it is instrument work, not physics: it is what a
> sub-degree generated-vs-accumulated test needs, since the position join caps admissible claims at
> ~0.88 deg.
>
> **Do not assume closing any of these closes (b).** The 2026-10-02 counterfactual matrix had **no
> arm passing (b) on any car**.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 RATEFIELD, commit `38450632`): **U-9193 RESOLVED — `+0x9c0` is the yaw TORQUE. The yaw rate is `+0x148`, and it was documented here since 2026-05-12.**
>
> **No C-level moved, no band moved, no code changed, no game run, no D2 WATCH.** Read
> [`verify/d3_omega_20261005/RESULT_RATEFIELD.md`](../verify/d3_omega_20261005/RESULT_RATEFIELD.md).
>
> - **`+0x9bc`/`+0x9c0`/`+0x9c4` is the TORQUE triple. `+0x144`/`+0x148`/`+0x14c` is the angular
>   velocity.** Already stated in `re/analysis/vehicle_promote_c2/0046e9e0.md:27-28` (committed
>   **2026-05-12**) and in `vehicle_dynamics/0046e9e0.md:30,49-51`. Confirmed by an independent static
>   read of `FUN_0046e9e0`.
> - **READ THE PLATE BEFORE BUILDING A PROBE.** Two of this session's legs (leg 3's `.msd` KA-1 and the
>   Frida entry-hook probe) tested `+0x9c0` as a rate and were measuring the wrong field. One grep of
>   `re/analysis/**/0046e9e0.md` would have prevented both.
> - **It explains everything that looked strange:** `+0x9c0` tracks turning exactly but is not
>   proportional to Δheading because a **torque is not a rate**; `+0x9bc`/`+0x9c4` being exactly 0.0 is
>   the **pitch/roll torque** on flat ground — **U-9175's physics was right, only its label was wrong.**
> - **U-9175's naming is corrected, scoped:** its label and mechanism are wrong; **all its numbers and
>   its conclusion stand.** Same mislabel in `BodyOrientationIntegrate.cpp:63` (`kAngVel`) — owed a
>   **comment-only** fix, because the port's **behaviour** is faithful (both omega arms, the `+0x144`
>   accumulator with the right gate, the y-term omission).
> - **For U-9191 this REMOVES a candidate, it does not supply one.** The port's rate construction is
>   structurally faithful, so car 1's **0.9866 deg** divergence is **not** a wrong-input bug.
>
> ### START HERE — compare the accumulator, but establish the arm first
>
> 1. **Add `+0x144`/`+0x148`/`+0x14c` to the port's `AiStepDump`** (one more triple; the inert-knob
>    pattern is proven twice over). The original's `.msd` **already carries them**, so **no new
>    original-side run is needed**.
> 2. **First establish which omega arm each side takes per frame** — `ESI[4]` (`+0x10`) `== 0` vs
>    `!= 0`. `BodyOrientationIntegrate.cpp:30` already records this fork as unresolved. **Comparing the
>    accumulator without knowing the arm would repeat exactly the error of comparing a torque to a
>    rate.**
> 3. **Then** answer U-9191: is car 1's 0.9866 deg **GENERATED** or **ACCUMULATED**?
> 4. **Do not assume closing it closes (b).** The 2026-10-02 matrix had **no arm passing (b) on any
>    car**.
>
> **Ghidra MCP was unreachable this session.** `re/tools/decomp_pc.py` is the sanctioned no-MCP path
> (`-readOnly` against a pool clone, still Ghidra on the anchored binary). Constants were read from
> `original/MASHED.exe.unpatched` bits-first, not from Ghidra's `_DAT_` rendering.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 OMEGA session, commit `0af828b1`): **`+0x9c0` is NOT the yaw rate — proved at a known program point.** — the field it *is* was identified the same day, see above.
>
> **No C-level moved, no band moved, no port code changed, no D2 WATCH.** Read
> [`verify/d3_omega_20261005/RESULT_OMEGA.md`](../verify/d3_omega_20261005/RESULT_OMEGA.md);
> `PREREG_OMEGA.md` was committed **unrun**. Anchor verified before arming.
>
> - **Reading `+0x9c0` at A6a ENTRY changed NOTHING.** Matched denominators, car 1: active-drive
>   **312 of 437 = 71.40 %** at the hook against **312 of 436 = 71.56 %** on the `.msd`; all rows
>   **317/2433** against **317/3623**. The **same 312 and the same 317**. So U-9193's
>   "zeroed phase" candidate is **dead** and **`+0x9c0` is not `omega.y`** — do not use it as a rate
>   anywhere.
> - **G-RATEID fails at the known phase too**: Pearson **0.057699** against 0.068251 from the
>   snapshot. 288 turning pairs, `+0x9c0` zero on **0** of them (support matches turning perfectly),
>   ratio median **-6.8962**, p10 -1.276e4, p90 +73.73, sign-inverted.
> - **G-ZERO extends U-9175:** `+0x9bc` and `+0x9c4` are **exactly 0.0 on all 2433** A6a-PRE rows, so
>   "the original's omega.x/z are exactly 0" holds for an **AI car at a known phase** and is not a
>   phase artifact.
> - **`--axis-probe` was NOT player-only** — it is parameterised by `--statediff-car`. It now also
>   logs `wx`,`wy`,`wz`,`px`,`pz`,`esi`, appended so existing consumers are unaffected.
> - **KA-B FAILED — a defect in the inherited probe.** `a6bEsiRec = 0 of 9672`: ESI does **not** hold
>   a record pointer at A6b, so its POST rows are **redundant duplicates of one record** (3.98x the
>   PRE count). Harmless for U-9175's one-car arm; it would **4x over-weight any per-row POST
>   statistic on a multi-car arm**. Fix or document before using POST rows.
>
> ### START HERE — stop probing offsets, read the writer
>
> 1. **The body forward row `+0x9d4`/`+0x9dc` demonstrably DOES rotate, so the rate exists
>    somewhere.** `BodyOrientationIntegrate` (`FUN_0046e9e0`) is **already named as its writer**.
>    **Read what that function actually READS** in Ghidra (use the `ghidra-pool` skill) and let the
>    disassembly name the rate input. Do **not** probe more offsets by trial — that is what just
>    failed twice.
> 2. That same read settles U-9193's second question: **what drives the original's orientation when
>    `omega.x`/`omega.z` are identically zero**, and whether the port's `BodyOrientationIntegrate`
>    inputs correspond to the original's at all.
> 3. **Only then** return to U-9191's open question — is car 1's confirmed **0.9866 deg** heading
>    residual **GENERATED** at the matched instant or **ACCUMULATED** before it? The port side already
>    carries the fields in its dump.
> 4. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **Two drafting failures of mine today, both recorded in the results:** leg 3's KA-1 was mis-scoped
> (it gated a leg that did not use the field it tested) and this session's **G-OMEGA was ill-posed** —
> its PASS clause used the active-drive denominator while the figure it was to be compared against was
> computed over all frames, so its nominal PASS is **void**. Check a gate's denominator against the
> number it will be compared to **before** committing it.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 LEG3 session, commits `52599239` .. `d717a713`): **U-9191 is CONFIRMED by an independent instrument. The rate legs failed their KA, so generated-vs-accumulated is still OPEN.**
>
> **No C-level moved, no band moved, no D2 WATCH.** New row: **U-9193**. Read
> [`verify/d3_leg3_20261005/RESULT_LEG3.md`](../verify/d3_leg3_20261005/RESULT_LEG3.md);
> `PREREG_LEG3.md` was committed **unrun** at `52599239` and amended **unrun** at `01ada11e` and
> `4429c0ec`.
>
> - **G-DIRECT PASS — U-9191's residual is real.** Measured **directly** from the record's forward
>   row on both sides, no `err` inversion, no poll: **0.9866 deg (n 105)** against U-9185's published
>   **0.9796** on the same-size population. Two different derivations, **0.7 %** apart. The gate was
>   written so it could kill the parent row; it confirms it. **U-9192's identity finding still stands
>   as a methodological correction — the number it produced just happened to be right.**
> - **Only medians are admissible from leg 3.** The pairing's p90 induced error (0.0902 deg) sits on
>   the registered 10x line (0.0892). No tail statistic is reported, and none should be quoted.
> - **KA-1 FAILED (corr 0.068251).** `+0x9c0` is **not** a per-frame yaw rate at snapshot phase, so
>   **the rate legs are abandoned and no cross-side rate number exists.** Finding: the original's
>   `+0x9bc` and `+0x9c4` are **exactly 0.0 on all 3623 frames** (extends U-9175 from the player to
>   an AI car), `+0x9c0` is non-zero on only **317**, its support matches turning exactly but
>   `Δheading / +0x9c0` scatters over six orders of magnitude, sign-inverted. **U-9193.**
> - **The port's dump now carries seven more record fields** (`rec_958`, `rec_960`, `rec_9d4`,
>   `rec_9dc`, `rec_9bc`, `rec_9c0`, `rec_9c4`), appended so nothing moved, proven inert at
>   **6479/6479** byte-identical pre-existing columns.
> - **Do not re-use these two assumptions:** the port writes `+0x958`/`+0x960` as **identically 0.0**
>   (it keeps position in `a.pos[]`), and a per-frame `.msd` snapshot cannot be paired to a per-call
>   dump without a measured phase term.
>
> ### START HERE — the open question needs a Frida entry hook, not an offline pass
>
> 1. **Read the original's AI-car yaw rate at a known program point.** `scenario_launch.py`'s
>    `--axis-probe` and `--lat-bracket` already sample `+0x9d4`/`+0x9dc` and
>    `+0x9bc`/`+0x9c0`/`+0x9c4` **at entry hooks** and are the proven shape — but **both target the
>    PLAYER** and need an AI-slot filter. Entry hooks only (memory
>    `frida-interceptor-is-entry-only`).
> 2. **Settle what `+0x9c0` is before using it** — U-9193's two candidates are "not `omega.y`" and
>    "sampled at a zeroed phase". Do not guess between them.
> 3. **Then answer U-9191's question:** is car 1's 0.9866 deg **generated** at the matched instant or
>    **accumulated** before it? The port side already has the fields in its dump.
> 4. **A D2 WATCH becomes owed only if the rate turns out to diverge** — the angular velocity is a D2
>    surface. Nothing is owed yet.
> 5. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **Three assumptions in my pre-registration were wrong** (`+0x9c0` as a rate, the port writing
> `+0x958`, and pairing without a phase term) and **one gate was mis-scoped by me**. All are recorded
> in the result. A better-informed registration would check the port's **field coverage** before
> registering a pairing key — do that next time.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 HEADATTRIB session, commits `225ee06f` .. `b204e34d`): **item (b)'s heading share is RE-ATTRIBUTED and it splits per car. Car 1 is REAL at the physics basis.**
>
> **No C-level moved, no band moved, no game code, no build, no game run** — legs 1, 2 and 2b are
> entirely offline on committed captures. New rows: **U-9191**, **U-9192**. Read
> [`verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md`](../verify/d3_headattrib_20261005/RESULT_HEADATTRIB.md);
> `PREREG_HEADATTRIB.md` was committed **unrun** at `225ee06f` and **amended unrun** at `d5da030e`.
>
> - **U-9185's framing of the heading share as one phenomenon across "2 of 3 cars" does not hold.**
>   `d_body_heading` median_abs, matched position → + identical `(c0,c1)` → + speed within 5 %:
>   **car 1 0.9796 (n 105) → 0.9417 (n 36) → 0.8918 (n 31) = REAL**; car 2 0.9085 → 0.1067 → 0.1064
>   (n 17) = **NO-VERDICT**; car 3 0.6506 → 0.2328 → 0.208 (n 18) = **NO-VERDICT**.
> - **Car 1 is a genuine physics divergence.** Its residual is stable across the whole registered
>   tolerance ladder (0.790 / 0.8918 / 0.9764 at 1/5/10 %) and does **not** collapse under speed
>   matching. Staleness, command and speed are each excluded on car 1. Carried by **U-9191**.
> - **Cars 2 and 3 look like U-9186's command defect** — their `d_err` collapses alongside their
>   heading term (1.2964 → 0.0468, 0.4661 → 0.0878) — but both are below the pre-registered n floor of
>   30 and are **NO-VERDICT, not artifact**. Do **not** loosen the match radius or lengthen the window
>   to reach a verdict on numbers already seen; either needs its own pre-registration.
> - **`ai_posmatch.py`'s `d_body_heading` is an ALGEBRAIC IDENTITY** on the other two reported
>   quantities (`:411`; max residual **5.684e-14** deg). It is **not** an independent measurement of
>   the body heading, so stop citing it as evidence that a residual is *in the physics*. The tool and
>   its committed numbers are fine and reproduce exactly — only the inference was wrong. **U-9192**.
> - **Staleness was tested and refuted.** U-9186's 100-of-660 frozen calls land almost entirely
>   outside the matched population: cars 1 and 2 have **zero** frozen pairs on either side, and
>   excluding car 3's 26 moves it **up** to 0.7269.
>
> ### START HERE — leg 3, already scoped, promoted by the registered gates
>
> 1. **Add `+0x9d4`/`+0x9dc` and `+0x9bc`/`+0x9c0`/`+0x9c4` to the port's `AiStepDump`** — five more
>    `VehiclePhysics_RecordF32` calls beside the `+0x9e4` and `+0xb0c` it already reads. The
>    inert-knob pattern is proven (`MASHED_AI_YAWW`, **6480/6480** byte-identical rows).
> 2. **Compare against `o_t1.msd` / `o_t2.msd`** via `re/tools/statediff/msd_fields.py` — they carry
>    the same offsets for **car slot 1, an AI car**, so **no new original-side run is needed**.
> 3. **Compare the yaw RATE, not the heading.** This is the entire point: a heading is an **integral**
>    of past yaw rate, and matched position + command + speed still does not match **history**, so car
>    1's 0.8918 deg may be **accumulated before** the matched instant rather than generated at it.
> 4. **VOID condition, not a caveat:** the `.msd` is one snapshot per render frame while the aistep
>    CSV has several AI calls per frame. Leg 3 must report its pairing residual and is **VOID** unless
>    that residual is at least **10x smaller** than the difference it claims.
> 5. **Do not assume closing this closes (b).** The 2026-10-02 counterfactual matrix had **no arm
>    passing (b) on any car**.
>
> **My registered prediction (H-STALE) was WRONG** — excluding the staleness made the residual
> slightly worse. Two defects in my own work are disclosed in the result: the tool first conflated "no
> `ret14a70` column" with "no qualifying row" (fixed, numbers unaffected), and the delegated capture
> inventory wrongly reported that `o_t3` does not exist.
>
> **No D2 WATCH** — this is a D2 surface seen through the AI, but C1 was not reached and no D2 code
> was read or changed.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-05 (D3 YAWW session, commits `49fc1ca0` .. `927304d9`): **U-9188 is RESOLVED and its answer is RETRACTED. U-9185 item (b) is REOPENED.**
>
> **No C-level moved, no band moved, no scorer edited.** Only game-code change is one default-OFF
> knob. New rows: **U-9189**, **U-9190**. Read
> [`verify/d3_yaww_20261005/RESULT_YAWW.md`](../verify/d3_yaww_20261005/RESULT_YAWW.md);
> `PREREG_YAWW.md` was committed **unrun** at `49fc1ca0` and **amended unrun** at `5f6f49c4`.
>
> - **The 2026-10-04 heading residual does not exist.** Measured in-process at two program points
>   with no poll, the port's published `g_aib.fwd[v]` against the record's forward is **0.000000 deg,
>   max 2e-6, on all three AI slots on every frame a car is stepped**. `post_rec` is 0.000000 too, so
>   the sync is exact and the bridge has **no drift term**.
> - **What U-9188 measured was the per-frame heading TURN**, banked by a non-atomic
>   `ReadProcessMemory` poll straddling the record-write / `a.yaw`-sync boundary. Its claimed
>   **0.5214 / 1.1430 / 0.4300** against this run's per-frame turn **0.491309 / 0.491309 / 0.421121**
>   (cars 1 and 3 to 5.7 % and 2.1 %), and its own reported cap of **2.5564** against this run's turn
>   maxima **2.556407 / 2.556403**. Right cap, wrong quantity.
> - **A-REACH PASSED.** `:3393` = `:3423` = `:3683` = `:3717` = **0** with `faithful_nav=1` and
>   `phys=1` witnessed in the same dump. **Both of U-9188's conditional remedies are dead.** Items 1
>   and 2 of the previous handoff are **done and void** respectively.
> - **The writer enumeration was wrong at five — there are NINE.** `:2690` (grid placement, witnessed
>   1/1/1) and `:3304` were missing. And **`VehiclePhysics_ResetOrientation` never touches the
>   record** (`VehiclePhysicsRun.cpp:518-523`) — it resyncs the body **basis**.
> - **A real, separate defect exists: U-9189.** An eliminated-while-spinning car publishes a
>   permanently stale heading, pinned at **66.828354 deg** for ~1300 frames. It **cannot** carry (b):
>   `ai_ctrl_window.py:25-29` scores the first 220 calls with `c4 != 0` and this starts at frame 1232
>   on a non-racing car. Do not fix it to move (b), and each candidate repair needs its own
>   pre-registration.
> - **U-9190: `re/tools/sa_headwatch.py` is not fit for this question.** Its `H-JITTER` control bounds
>   a position while the claim is an angle, and its slot-0 floor agrees by construction. **Do not use
>   that floor as an acceptance target.** Retire it or add a frame marker, and re-derive or withdraw
>   any other result taken from it.
>
> ### START HERE — item (b) goes back to the physics basis
>
> 1. **Re-attribute U-9185 item (b)'s matched-position heading share** (`0.9796 / 0.9085 / 0.6506`) at
>    the **physics basis**, which the 2026-10-04 session recorded as **NOT REACHED**. Item (b)'s two
>    candidates are now **one**: the `(cos, sin)` reconstruction is exonerated, leaving the physics'
>    own `io.yaw`.
> 2. **Use the in-process pattern, not a poller.** A default-OFF knob reading both operands at known
>    program points costs one build and has **no sampling term**. `MASHED_AI_YAWW` is the worked
>    example and is proven inert (**6480/6480** byte-identical AI-step rows knob ON vs OFF).
> 3. **Do not assume closing anything here closes (b).** The 2026-10-02 counterfactual matrix had
>    **no arm passing (b) on any car**.
>
> **Three self-corrections, all registered before the run so none is hindsight:** candidate 5 (frame
> order) was my own registered leading hypothesis and was refuted by static reading; PREREG
> Correction 1 reused U-9188's misreading of `ResetOrientation`; and the writer enumeration was
> corrected from five to nine before counting, which is why `:2690` was instrumented at all.
>
> **No D2 WATCH row** — no D2 code was read or changed.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-04 (D3 HEADING session, commits `ed2eba34` .. `ef09fe40`): **U-9185 item (b) is ANSWERED — the body-heading residual is a PORT-ONLY BRIDGE defect, not a physics one.** — **RETRACTED 2026-10-05, see above.**
>
> **No C-level moved, no band moved, no game code / `.rsp` / build.** New row: **U-9188**.
> Read [`verify/d3_heading_20261004/RESULT_HEADING.md`](../verify/d3_heading_20261004/RESULT_HEADING.md);
> `PREREG_HEADING.md` was committed **unrun** at `ed2eba34`.
>
> - **The port carries TWO heading values per AI car and they disagree inside the same
>   frame.** `g_aib.fwd[v]` (what `SteerAngleErrorFwd` reads) vs the record's
>   `+0x9d4`/`+0x9dc` (the field the ORIGINAL's `FUN_0046d510` returns): median
>   **0.5214 / 1.1430 / 0.4300** deg on cars 1/2/3, against a **same-instrument floor of
>   exactly 0.0000 deg on the player at every lag**. Same band as U-9185's matched-position
>   heading share (0.9796 / 0.9085 / 0.6506), and **car 2 is the largest on both**.
> - **Car 2 is qualitatively different**, not just larger: **327 of 933** samples above
>   10 deg and **16 above 90** (max 176.65), where cars 1 and 3 never exceed **2.5564** and
>   share a near-identical hard cap with only 5 distinct values above 2.5 — a quantised
>   rate limit.
> - **Staleness is REFUTED** by an integer-lag fit: the minimum is at L=0 and never
>   approaches the floor at any lag 0..4.
> - **Mechanism.** `TrackRenderer.cpp:3334` sets `a.yaw = io.yaw` straight out of
>   `VehiclePhysics_StepCar`, which wrote `+0x9d4`/`+0x9dc` from that **same** `io.yaw` — at
>   that instant they **agree**. `a.yaw` is then rewritten by port-only scaffold code before
>   `:3729` rebuilds the bridge value. `:3351` **resyncs** the record
>   (`VehiclePhysics_ResetOrientation` at `:3355`); `:3283`, `:3393`, `:3423`, `:3717` do
>   **not**. The disagreement **persists** rather than snapping back, so the firing writer
>   is a non-resyncing one.
> - **U-9185's limiter claim is HALF-CONFIRMED.** Correct for `:3683` (inside
>   `AiOptionBStep`, reached at `:3313` only when `!phys`), but a **second copy exists at
>   `:3423` inside `UpdateCar` itself**, in the gate-ribbon block.
>
> ### START HERE — cheap, and it is a count, not a port
>
> 1. **Add one default-OFF counter at each of `:3283` / `:3393` / `:3423` / `:3717`** and
>    count per AI slot over the (b) window. No behaviour change. **Watch car 2 separately.**
> 2. If **`:3423`** fires → stop the gate-ribbon limiter running on the ported path.
>    If **`:3717`** fires → add the `VehiclePhysics_ResetOrientation` its sibling at `:3351`
>    already does.
> 3. Re-run `py -3.12 re/tools/sa_headwatch.py` after any fix. **The slot-0 floor of
>    0.0000 is the acceptance target** and the tool prints it every run.
> 4. Only then re-score (b) — and **do not assume closing this closes (b)**. The 2026-10-02
>    counterfactual matrix had **no arm passing (b) on any car**.
>
> **Two self-corrections from this session, recorded so they are not repeated:** the
> pre-registered prediction that the bridge/record pair agrees "by construction" was **wrong
> by five orders of magnitude**, and the pre-registered `H-JITTER` gate was **ill-posed**
> (position units against degrees) and was replaced by the player-slot floor. A third,
> methodological: a lag fit that includes zero vectors silently agrees, because
> `atan2(0,0) = 0` — filter first.
>
> **C1 (the physics basis) was NOT reached**, so there is **no D2 WATCH row** from this
> session.

The block below is the PREVIOUS session's headline, left as history.

> ## UPDATE 2026-10-03 (D3 LEADER-WITNESS session, commits `3988dbf0` .. `27041abb`): **item 1 below was tested BEFORE being built, and it is INERT. The wiring was NOT performed, and its recorded scope was wrong. START HERE.**
>
> **No C-level moved, no band moved, no game code and no `.rsp` was edited.** New rows:
> **U-9187**. Read
> [`verify/d3_leader_20261003/RESULT_WITNESS.md`](../verify/d3_leader_20261003/RESULT_WITNESS.md);
> the pre-registration `PREREG_WITNESS.md` was committed **unrun** at `3988dbf0`.
>
> - **W-GATE = INERT.** Adding `AiLeaderTimer.cpp` / `AiTargeting.cpp` / `AiLineOfSight.cpp`
>   to `exe_sources.rsp` would be **both unsafe and inert**. **UNSAFE:** the body calls
>   `0x0040e470` (`:94`,`:104`), `0x00442cc0` (`:101`,`:106`) and `0x0046d4a0` (`:109`), all
>   inside `0x00400000..0x004fffff`, which `Compat/StandaloneRvaThunks.h:7` records as
>   **entirely unmapped** in the standalone and `MenuButtonDetect.cpp:71` records as an AV;
>   no thunk covers them. **INERT:** even thunked, the limit table `0x005f2dd8` reads
>   **0 of 64 non-zero** on the port against **14 of 64** on the original, so `:99`'s
>   `limit (0) <= RankAt (0)` returns 0 **before `Prog` is read at `:101`**.
> - **Root cause, and it generalises:** `exe_main.cpp:56` VirtualAlloc-maps
>   `0x00500000..0x009fffff` **BLANK**. So in `mashed_re.exe` every **runtime-written**
>   global is populated and **every initialised-image value reads zero**. Expect this for
>   any future port that reads a `_DAT_005*` table or constant.
> - **Measured both sides.** `Prog[0..3]` is **0.0 on all four slots in all 1089 port
>   samples** against non-zero on **464 / 125 / 486 / 461 of 512** original calls;
>   `TimerAt` **0** vs a live **0..1250 stepping by 50**; thresholds **0.0 ×4** vs
>   **6.5 / 5.5 / 6.0 / 4.0**. W-BASE passed every leg, so "all zeros" is a finding and not
>   a bad base.
> - **THE RECORDED SCOPE WAS WRONG.** "NO new reversing is needed" misses
>   **`FUN_00442a60`** (`0x00442a60`, `Spectator::ComputeDistances`), the producer of
>   `0x008989b0`, which is **C2 `new` with no body anywhere**. `AiStandalone.cpp:707` had
>   already named it unported.
>
> ### START HERE — the corrected order for the 64-call branch (U-9187)
>
> 1. **Port `FUN_00442a60` first.** Without the `0x008989b0` producer every other item stays
>    inert, and it is the **only** one needing new reversing.
> 2. Give `0x0040e470`, `0x00442cc0`, `0x0046d4a0` exe-side bodies (all three are **C3
>    `impl`** with an **empty `exe_file`** — no reversing, just a home the exe links).
> 3. **Do not re-measure these two — they are resolved in U-9187.** Limit table: only index
>    **10** is ever used and its value is **1** (head of 64
>    `[2,1,1,0,0,1,1,0,0,0,1,0,0,0,0,0]`). Thresholds: **6.5 / 5.5 / 6.0 / 4.0**.
> 4. Then settle the two upstream disagreements that survive all of the above, because they
>    decide whether the 64 calls are **reproduced** or merely made **reachable**:
>    `idx364` (**-1** orig vs **0** port) and `bias374` (**0** vs **{0,1,2,3}**).
> 5. Check call-by-call against the committed `o_t1`/`o_t2`/`o_t3`, which carry the exact 64.
>
> **Do not seed a global to make the arm fire** —
> `verify/d3_modes37_20261002/RESULT_STEP2.md`: *"seeding the globals would not be a port."*
>
> New read-only tooling: `re/tools/sa_leaderwatch.py` (`ReadProcessMemory`, no injection)
> and `scenario_launch.py --leader-probe` (**one entry hook** on `0x004148b0`, with a
> threshold known-answer check).
>
> Items **2, 3 and 4** of the previous START-HERE block below are **unchanged and still
> open**.

The block below is the PREVIOUS session's headline, left as history.

Updated 2026-10-03 at the close of the **D3 start-boost A/B** session.
Branch `race/first-frame-parity`. Nothing is pushed. **No game code was edited this
session**; the only code changes are analysis tools.

## The headline

> **UPDATE 2026-10-03 (D3 ELIMINATION + OVER-SPEED session, commits `e0b35ff9` ..
> `46cd19df`): the confound is ZERO and the over-speed is a COMMAND defect with all 149 of
> its calls accounted for. START HERE.**
>
> **No function changed C-level, no band moved, no scorer was edited.** The only game-code
> change is a default-OFF measurement knob (`MASHED_NO_ELIM`).
>
> - **The elimination confound explains 0 % of the arm A vs arm B delta.** Arm C
>   (`MASHED_NO_ELIM=1`) vs arm A: **0 of 2640 scored byte-slots** differ over the window,
>   **0 of 660** on speed/position, all six arms print the same (b)/(e) digits. U-9185 item
>   (a) RESOLVED; every number in `verify/d3_noboost_20261003/RESULT.md` stands.
> - **Two corrections.** `VehicleStep(0)` runs in **neither** port arm and not on the original
>   either. **Both** port arms eliminate the player (arm A window call 112, arm B 146). On the
>   **original** the player is alive through the whole window and dies on the first frame
>   after it, when the camera-zoom gate `FUN_00442df0() == 10.0` saturates — so the
>   elimination time is a **readout of the over-speed**, not an independent variable.
> - **A PRE-REGISTERED GATE FAILED** (`PREREG_STEP2.md` §2.0: the accel/brake model
>   reproduces the **port** 220/220 and the **original** only 0.636/0.936/0.750), so §2.1–§2.5
>   **did not run and are reported nowhere**. `PREREG_STEP2B.md` replaced them unrun.
> - **THE OVER-SPEED IS A COMMAND DEFECT.** Pre-onset, the port's whole force chain
>   reproduces the original's speed to **0.98 / 0.23 / 0.23 %** — drive force, A5 drag, A6a's
>   clamps incl. grip-clamp #6, the contact solver and `+0xb0c` are jointly **exonerated**.
> - **All 149 of 660 diverging calls = three unported branches of `FUN_00416250`**, by live
>   targeting-return signature, identical across three runs:
>   **36** `FUN_00414a70 == 2` → immediate return `0x00416405`;
>   **49** `FUN_00414c30 == 2` → mode 7 → `ctrl[4] = 0x40`;
>   **64** `FUN_004148b0 != 0 && FUN_00416060 != 0` → a second immediate return.
>   **0 unexplained.** (448 calls have no producer firing and the port's mode-0 tail is
>   bit-exact; 63 are mode 3 and agree.)
>
> ### START HERE — the next session's first job, in order
>
> 1. **Wire `FUN_004148b0` into the standalone — 64 of 149 calls, and NO new reversing is
>    needed.** `0x004148b0` is **C3 `impl` `Ai/AiLeaderTimer.cpp`** and `0x00416060` is
>    **C3 `impl` `Ai/AiTargeting.cpp`** with a committed `frida_diff`. What is missing: all
>    three TUs (`AiLeaderTimer.cpp`, `AiTargeting.cpp`, `AiLineOfSight.cpp`) are in
>    **`asi_sources.rsp` only**, and `AiStandalone.cpp:846` carries the comment
>    *"FUN_004148b0 / FUN_00415020 are stubbed (return 0)"* where the call belongs. The
>    decompiled arm to transcribe is quoted verbatim in
>    `verify/d3_elim_20261003/RESULT_STEP2.md`.
>    **MANDATORY FIRST: a pre-registered knob-took witness proving it is NOT inert.**
>    `LeaderTimer` reads per-car rank/progress tables that may be `.bss` zeros in the
>    standalone — the exact trap `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203` already
>    measured for `FUN_00484c70` (*"seeding the globals would not be a port"*). Memory:
>    `verify-the-harness-knob-actually-took`.
>    The committed captures `o_t1`/`o_t2`/`o_t3` carry the **exact 64 calls** the fix must
>    change, so it is checkable call-by-call rather than only through a band.
> 2. Then `FUN_00414a70` (C2 `mapped`, no body) + its C2 callee `FUN_00414300` — 36 calls.
> 3. Mode 7 (49 calls, **car 1's entire carrier**) is **structurally blocked** until the
>    standalone owns a world-object list: `FUN_00414c30` iterates `FUN_00484c70` objects.
> 4. U-9185's item (b) — separate the two port-side heading candidates
>    (`TrackRenderer.cpp:3327` physics yaw vs the `(cos, sin)` reconstruction at `:3729`) at
>    matched position on car 2 — is untouched and still open.
>
> Trackers: **U-9186** filed, carrying the three sources. **U-9185** amended, keeping only
> item (b). Read `verify/d3_elim_20261003/RESULT_STEP1.md` and `RESULT_STEP2.md`.
>
> **One gate failed in each step and both are stated prominently rather than re-thresholded:**
> `PREREG_STEP2.md` §2.0 (above) and `PREREG_STEP2B.md`'s **G2B-LIVE**, whose registered
> *"≥ 99 % of mode-7 calls carry `c4 == 0x40`"* measures **61.3 %**; the exact relation is the
> converse, **49/49** on four runs.

The block below is the PREVIOUS session's headline, left as history.

> **USER DECISION 2026-10-03 (Mariano) — the decision the block below left open is MADE:
> KEEP the AI start boost.**
>
> `mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:703-707` stays in the default
> build, because it reproduces the original's launch: **(e) is MET 3/3, within 0.05 %** of
> the original's own 1425.7 / 2052.5 / 2055.0. Removing it is **rejected** — that trades a
> MET criterion for a still-NOT-MET one. `MASHED_NO_START_BOOST` remains a measurement knob
> only.
>
> The work that follows from it, in order:
> 1. **Control the player-elimination confound** (`race_[0].alive` differing at
>    `rt = 1.8667 s`, inside the scored window) so that every (b)/(e) number is single-cause.
> 2. **Attack the surviving +15..31 % AI over-speed** with the boost ON — the only route
>    that does not trade (e) against (b). Counterfactually it moves car 1's `c1_distinct`
>    80 → 50 and `steer_distinct` 86 → 61, both into band. Car 2's residual is `err`, not
>    speed.
>
> Recorded in `ROADMAP.md` §D3 ("D3 USER DECISION 2026-10-03"). No band moved, no C-level
> moved.

> **UPDATE 2026-10-03 (D3 START-BOOST A/B session, commits `2b3e2f47` .. `c79a3619`):
> the `MASHED_NO_START_BOOST=1` A/B that the START-HERE block below asked for is DONE.
> It did NOT close (b) and it REGRESSED (e) 6/6. There is now a USER DECISION open.**
>
> Arm A = default, arm B = `MASHED_NO_START_BOOST=1`, three repeats each, every repeat
> digit-identical, no failed boot, recipe/scorers/bands unchanged and both band files
> proven unedited. Fresh arm A reproduces `r1/r2/r3` on every printed digit.
>
> - **(b) 13 → 6 failing bands.** Car 1 5→4, car 2 4→2, **car 3 4→0 (passes outright)**.
>   `c1_median` 52.5/38.0/49.0 → **0.0** and `abs_steer_median` 52.5/57.5/49.0 →
>   **6.0/21.0/9.0**, all in band on all three cars. **But car 1 gains TWO NEW failing
>   bands**, `accel_distinct` and `brake_distinct`: it takes only `c4 = 255` and only
>   `c5 = 0` over the window. **(b) is still NOT MET on 2 of 3 cars.**
> - **(e) regresses 6/6, −46 % to −90 %.** `launch` 1426.4/2053.0/2055.2 → **200.5** on
>   every car, against the **ORIGINAL's own** 1425.7/2052.5/2055.0. **The original
>   demonstrably HAS a launch and arm A reproduces it to 0.05 %** — removing the seed
>   removes the port's only reproduction of a real original behaviour.
> - **Window speed +41/+40/+34 % → +31/+21/+15 %.** The seed is worth about HALF the
>   over-speed. A sustained **+15..31 %** survives the knob and is a **separate carrier**.
> - **The knob is proven live in flight**, not inferred: slots 1..3 read
>   `g_startBoosted` 0→1 with `+0xbf8 == 1` and `+0xbf4` decaying
>   **1100/900/700/500/300/100**; arm B never seeds them; **slot 0 is never seeded in
>   either arm**. `re/tools/sa_boostwatch.py`, `ReadProcessMemory`, no injection.
> - **Player physics is BIT-IDENTICAL between arms** over 4198 `player_trace` lines —
>   **no D2 REOPEN CANDIDATE on player physics.** One confound IS filed: `race_[0].alive`
>   differs at `rt = 1.8667 s`, **inside** the scored window, so the arm A vs arm B deltas
>   are the seed PLUS that divergence and this step did **not** separate them.
>
> Read [`verify/d3_noboost_20261003/RESULT.md`](../verify/d3_noboost_20261003/RESULT.md).
> **One gate was replaced and is stated prominently there**: the pre-registered base
> self-check `record[v] + 0x000 == v` is FALSE in the port and voided the first witness
> run; it was replaced by three stronger legs.

The block below is the PREVIOUS session's headline, left as history.

> **UPDATE 2026-10-02 (D3 OFFLINE session, commits `31fa2fe3` .. `077fc43c`): both
> START-HERE measurements below are DONE. `U-9183` and `U-9182` are RESOLVED, every
> pre-registered gate PASSED, and the recommended next target has CHANGED.**
>
> - **`curv` is faithful.** Position-matched, median `|dcurv|` is **0.0197 / 0.0131 /
>   0.0296** deg on n = 105 / 68 / 104. The 2-8x was a position artefact, on top of a
>   conditioning artefact (`mode == 0` selects the original's low-curvature calls; the
>   FULL-window `curv` medians are **94.14 / 11.40 / 38.28** original vs **52.04 / 52.81
>   / 50.08** port, and on car 1 the ORIGINAL is higher). **Do not port the curvature
>   chain, and `SelectSpline` is not a lead** — the original's `spline` argument equals
>   `0x801aa0 + ai_spline_idx*0x204` on 5463 of 5463 rows.
> - **`c0`'s arithmetic is bit-faithful** (recompute matches the logged byte **586/586**
>   on the port, **148/151** on the original). `c0_distinct` comes from ONE branch, MAG
>   `0x00416697`; everything else contributes only `0`. Car 1 split: **MAG 72 vs 24**,
>   MAGHI 62 vs 171, DEAD 54 vs 22, CTR1 32 vs 3, FF30 **0 vs 0**, CTRHI **0 vs 0**.
> - **What is left is a SUB-DEGREE residual across a HARD split.** At matched position the
>   median `|d signed err|` is only **0.945 / 1.296 / 0.466** deg, but the LO/HI split is a
>   discontinuity at `err = 0/360` and `|err|` is a 1-2 deg oscillation about it, so that
>   bias flips the band on **43 % / 24 % / 13 %** of matched calls. On car 1 it persists at
>   MATCHED SPEED (2606.0 vs 2606.8). New row **U-9185** carries it.
>
> Read [`re/analysis/D3_B_OFFLINE_2026-10-02.md`](analysis/D3_B_OFFLINE_2026-10-02.md)
> and [`verify/d3_offline_20261002/RESULT.md`](../verify/d3_offline_20261002/RESULT.md).
> **One gate was replaced, declared BEFORE the run** (`PREREG.md` A.0): the known-answer
> check could not run offline because the spline point array is runtime memory.

The paragraph below is the PREVIOUS session's headline, left as history.

> **D3 does NOT close, and the 2026-09-29 closure path is REFUTED by its own
> pre-registered gate.** Porting behaviour modes 3 and 7 would not close AI criterion (b),
> so **the port was not written**. (b) is now **decomposed into four measured carriers**
> rather than attributed to one cause, and the **dual-copy hypothesis turned out to be the
> same hypothesis**, already measured.

Read, in order and none of it long:
[`verify/d3_rebase_20261002/RESULT_STEP1.md`](../verify/d3_rebase_20261002/RESULT_STEP1.md),
[`verify/d3_modes37_20261002/RESULT_STEP2.md`](../verify/d3_modes37_20261002/RESULT_STEP2.md),
[`.../RESULT_STEP2B_DUALCOPY.md`](../verify/d3_modes37_20261002/RESULT_STEP2B_DUALCOPY.md).
Pre-registrations `PREREG_STEP1.md` (`867de577`) and `PREREG_STEP2.md` (`a317d25c`), both
committed **unrun**. New rows **U-9182**, **U-9183**, **U-9184**.

## Standing user decisions (recorded 2026-10-02, commit `1cbd4678`)

1. **D2 is PARKED, not closed, and stays re-openable.** 2 of 3 metrics inside their
   unchanged `d81a8df6` bounds; `driving-median` ~1.3 % under with its carrier **not
   identified**. Residuals are `DEFERRED.md` **D-11071**. **Re-pickup trigger:** any
   finding touching `+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6
   (`0x004687f0..0x0046897b`), the substep/chunk loop (`0x00470c70`), `ReassertContacts`,
   or player-car speed on Training → report a **"D2 REOPEN CANDIDATE"** row and do not
   change D2 code in that session.
2. **D3 is UNBLOCKED.**

## What is MEASURED and must not be re-derived

- **(e) PASSES 3/3** on the post-attempt-20 build: `launch` **1426.4 / 2053.0 / 2055.2**,
  `ft_median_m0` **2550.6 / 2053.0 / 2278.2** (n = 100 / 23 / 39). Bands unchanged.
- **(b) FAILS 3/3**, 13 bands, and **all five failing bands are STEER bands**
  (`c0_distinct`, `c1_distinct`, `steer_distinct`, `c1_median`, `abs_steer_median`).
  **accel, brake and `c0_median` PASS on all three cars.**
- **D2 attempt 20 moved the AI window by EXACTLY ZERO.** `MASHED_D2_BATCHMODE=plane`
  reproduces the default arm on every printed digit of both scorers. **The knob is proved
  live** — the full-CSV diff first differs at AI-step call **455**, 235 calls after the
  window closes (2823 rows of `rec_9e4`, 1765 of `c1`).
- **The port is fully deterministic** on this recipe: `r1`/`r2`/`r3` are byte-identical
  over their whole common prefix. Any `collateral.py` floor derived from such a pair on
  this data is **too generous**; use the direct diff.
- **The window provably runs `ControlStep` (`FUN_00416250`)** — the original's window
  carries `ai_mode == 7` on 80 of car 1's 220 calls, and mode 7 is committed **only** at
  `0x0041642f` inside that function.

## THE OPEN QUESTION — (b), decomposed

| carrier | best evidence | next move |
|---|---|---|
| **`int mode = 0;`** (`Ai/AiStandalone.cpp:844`) — the `0x0041665c` multiplier never switches off. The original's `mode != 0` calls carry `curv` medians **125.8 / 91.3 / 132.9**, i.e. it stops amplifying exactly where curvature is extreme | best single arm on cars **1 and 3**: `abs_steer_median` 52.5→15.0 and 49.0→24.5 | **real, large, NOT sufficient — do not re-test it** |
| ~~**`curv` 2-8x high**~~ — `FUN_00443440` @ `0x004162b0` (`AiStandalone.cpp:837`) | **CLOSED 2026-10-02.** Position-matched median `|dcurv|` **0.0197 / 0.0131 / 0.0296** deg (n = 105 / 68 / 104); full-window medians **94.14 / 11.40 / 38.28** orig vs **52.04 / 52.81 / 50.08** port | **U-9183 RESOLVED — do NOT re-open, and do not port the curvature chain** |
| **speed — SPLIT IN TWO 2026-10-03.** A linear multiplier of steer magnitude via `m = err*speed*0.0030034` (`0x00416656`) | **(i) the start seed** (`VehiclePhysicsRun.cpp:703-707`) is worth about HALF: removing it takes the window medians from **3421.7 / 3346.2 / 3495.9** (+41/+40/+34 %) to **3169.4 / 2904.7 / 3011.0** (+31/+21/+15 %) vs the original's **2419.5 / 2397.0 / 2617.6**. **(ii) the remaining +15..31 % is a DIFFERENT, UNIDENTIFIED carrier** | **(i) MEASURED — the A/B is DONE, see the headline; removing it costs (e) 6/6. (ii) is now the open speed lead. U-9185** |
| **sub-degree steering-error bias across a HARD split** — `SteerAngleErrorFwd` `0x00416596`; LO/HI split at `err = 0/360` (`0x004165c0` / `0x004166cf`) | at matched position median `|d signed err|` **0.945 / 1.296 / 0.466** deg (target dir 0.28/0.48/0.45, **body heading 0.98/0.91/0.65**); flips the band on **43 / 24 / 13 %** of matched calls, and persists on car 1 at **matched speed** (2606.0 vs 2606.8) | **U-9185** — second, after the start-boost A/B |
| ~~**`c0_distinct = 6`** on car 1~~ | **ANSWERED 2026-10-02.** `c0` comes from ONE branch, MAG `0x00416697`; car 1 split **MAG 72 vs 24**, MAGHI 62 vs 171. The arithmetic reproduces the logged byte **586/586** (port), **148/151** (orig) | **U-9182 RESOLVED — do NOT port the `c0` magnitude path, it is bit-faithful** |

### START HERE — the ONE next move, and it is NOT a port

> The two offline measurements this section used to list are **DONE** (`U-9183` and
> `U-9182`, both RESOLVED 2026-10-02), and **the start-boost A/B below is DONE too**
> (2026-10-03, `verify/d3_noboost_20261003/RESULT.md`). Do not re-run any of the three.
> The old text is kept below the rule as history.

### THE DECISION THAT IS NOW THE USER'S, and nothing should be ported until it is made

The A/B measured a real and **adverse** trade. Three options, none taken:

1. **Keep the seed (status quo).** (e) MET 3/3, (b) 13 failing bands. The AI third stays
   blocked on (b).
2. **Remove the seed.** (b) 6 failing bands with car 3 passing outright, (e) fails 6/6 by
   46-90 %. **This trades a MET criterion for a still-NOT-MET one.**
3. **Keep the seed and attack the surviving +15..31 % over-speed instead.** Per the
   counterfactual matrix (`ai_band_sim.py`, validation 220/220 = 1.000 on all three cars)
   closing it would move car 1's `c1_distinct` 80→50 and `steer_distinct` 86→61 **into
   band**. **The only option that does not trade one criterion against the other**, and it
   is not costed yet.

### THE NEXT MOVE, if the user wants more measurement before deciding

**1. Separate the seed from the player-elimination confound. Cheapest thing left.**
With the seed ON the player is eliminated at `rt = 1.8667 s`, **inside** the scored window
(`race_[0].alive` 0 vs 1, written at `TrackRenderer.cpp:4664` via `RE::SegmentCheck` +
`EliminationCheck`, which read the **AI cars'** positions). `round_mode_` is true,
`g_aib.alive[0]` is cleared at `TrackRenderer.cpp:3722`, and the AI tick loop at
`AiStandalone.cpp:1708` runs `for (v = 0; v < 4; ++v)` gated on `car_alive(v)` — so
`VehicleStep(0)` runs in arm B and does not in arm A. **Until that is controlled, no arm A
vs arm B delta is single-cause.**

**2. Then the body heading, and car 2 is the car to probe.** It is still the larger median
share on all three cars in both arms, and on **car 2** it **GROWS** 0.9085 → **1.6059** deg
when the seed is removed — car 2 is also the car whose remaining (b) failure the
counterfactual attributes to `err`, not speed. The two separable candidates are unchanged
(see item 2 of the history block below).

**3. Separately: find what carries the +15..31 % over-speed that survives the knob.** It is
**not** the seed. This is option 3 above.

<details><summary>History: the start-boost A/B as it was briefed, now DONE</summary>

**1. Run the start-boost A/B first. One environment variable, no code change.**
`VehiclePhysicsRun.cpp:703-707` applies a **port-only start boost to `slot != 0`, i.e. to
exactly the cars criterion (b) scores** — `+0xbf8 = 1`, `+0xbf4 = 1300`, guarded by
`MASHED_NO_START_BOOST`. The port's window speed is **3421.7 / 3346.2 / 3495.9** against
the original's **2419.5 / 2397.0 / 2617.6** (**+41 / +40 / +34 %**), and
`m = err*speed*0.0030034` (`0x00416656`) makes speed a linear multiplier on the steer byte
while the extra distance travelled pushes the port into higher-curvature track inside the
same 220 calls. Suspect the port-only scaffold before re-suspecting a byte-faithful
transcription.

> Re-capture the port side with `MASHED_NO_START_BOOST=1` and re-score **BOTH (b) and
> (e)**. (e) passes today **with** the boost, so the seed cannot be removed on (b)'s
> evidence alone — if (e) regresses, that is a trade-off for the user, **not** a fix.
> Pre-register the decision rule before the run, as every D3 session has.

**2. Only if that does not close (b): the AI cars' body heading.** It is the larger median
share of the matched-position residual (**0.9796 / 0.9085 / 0.6506** deg). It reaches
`SteerAngleErrorFwd` as `(cos(a.yaw), sin(a.yaw))` from `TrackRenderer.cpp:3729`, with
`a.yaw` round-tripped through `Vehicle::VehiclePhysics_StepCar` (`:3315` in, `:3327` out)
— **not** from `rec+0x9d4`/`+0x9dc`, which is what `AiStandalone.cpp:939`'s comment claims
and which nothing in the standalone reads. Two separable candidates: the physics' own yaw
(a D2 surface measured through the AI) and the scalar reconstruction standing in for the
record's forward basis row (a port-only bridge, cheaper to test). The
`yerr * (6.0f*dt)` turn-rate limiter at `TrackRenderer.cpp:3416` / `:3676` is **not** a
suspect — it is in the legacy "AI v2" `else` branch, which the ported path does not take.

**3. `FUN_00414c30` + producer chain is RULED OUT for this carrier.** Its effect on (b) is
through `ai_mode`, already refuted as sufficient by the modes-3/7 arms, and `curv` and
`c0`'s arithmetic are now proved faithful — so that chain cannot reach what is left.

</details>

**One question for the USER, deliberately not decided.** Criterion (b)'s `c0_distinct`,
`c1_distinct` and `steer_distinct` count which side of a hard discontinuity a 1-2 deg
oscillation lands on; a faithful port can fail them. Whether to re-specify (b) on a
band-invariant statistic is a ROADMAP decision. **No band was moved.**

<details><summary>History: the two offline measurements, now both DONE</summary>


1. **U-9183, position-matched curvature.** The index-matched comparison above is **partly
   circular**: the two sides traverse at different speeds, so at the same within-window
   call index they are at different track positions, and curvature is a property of
   position. Re-bin both sides by `look_idx` / `look_blk` / `ai_spline_idx` (all three are
   already columns in the committed captures) and compare **within bin**. Only if the gap
   survives is `FUN_00443440` a defect — and then `0x00418560` (`SelectSpline`, demoted
   C3→C2 with three missing arms and a missing spline-index reset) is the first suspect.
2. **U-9182, the branch split.** `c0` is written at `0x00416697` (low-err branch) and
   `0x004167b1` (counter-steer, high branch). The decode rule is already in
   `ai_band_sim.py:63-71` (`hist_d8 == 360` → low, `hist_dc == 0` → high), so count the
   per-car split on **both sides from existing captures first**. Only hook the original if
   that is inconclusive.

**Do both before spending a session on the `FUN_00414c30` + producer-chain port.** This
session's evidence says that port would not close (b) on its own.


</details>

### If you do write the modes port, the scope is already measured

`FUN_00484c70` is **already ported and C3 with a GREEN Frida diff** — but in the
**asi-only** `Util/PromoLoop_round20.cpp`, so `mashed_re.exe` has no copy. `DAT_006e70d8`
is written only by `FUN_00484c90` (`0x00484cd4`) and `FUN_00485070` (`0x0048508a`);
`DAT_006dccb8` by `FUN_00484c90` (`0x00484ca4`), indexed by the registrar `FUN_00484cf0`
(`0x00484d2d`/`0x00484d35`), which has **13 callers**, most in the already-ported power-up
range including the dispatcher `0x0045bba0`.
**Porting `FUN_00484c70` alone is INERT** — both globals are `.bss` zeros, the count is 0,
`FUN_00414c30`'s loop never runs, no mode is ever set. **Seeding them is not a port.**
`FUN_00414c30` (704 bytes, `0x00414c30..0x00414ef0`) has no port at all; the only
reference in `mashedmod/src` is the asi-side call-through at `AiControlStep.cpp:92`, which
jumps into `MASHED.exe` and cannot work in the standalone.

### Dead ends — do not re-open

- **`rate1` pinned `0.0f`** (`0x00416a30` `:983`, `0x00417da0` `:1102`) and the
  **velocity-derived heading** (`0x00415e20` `:175`) are **not reachable on this recipe**.
  They live in `ControlStepM49`/`M8`, selected only when `fd0 ∈ {4,8,9}`, and `ControlStep`
  calls the **correct** body-forward `SteerAngleErrorFwd` at `:858`. `U-9184` records this
  and corrects `DUAL_COPY_FIX_2026-09-29.md:177-180`, whose claim that (b) "runs on" those
  two is wrong. **The demotions themselves stand.**
- **`0x00443080`'s exe literal** is harmless: `tgt_7ffc` is within the noise floor in every
  shared band, on top of the committed 6259/6259.
- **Attempt 20 as a cause of any (b) number.** Measured at exactly zero, with a live-knob
  positive control.

## Recipes (unchanged, copy-paste)

```
# standalone (b)/(e) capture
py -3.12 re/tools/sa_capture.py verify/<dir>/<tag> 8,30,60 MASHED_MUTE=1 \
    MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="<label>" \
    MASHED_AI_STEPDUMP=verify/<dir>/<tag>.csv
# original (b)/(e) capture -- one run covers all three AI cars
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/<dir>/o1.msd \
    --statediff-car 1 --statediff-aistep --hold 60
# scorers (BANDS ARE NOT TO BE MOVED)
py -3.12 re/tools/ai_ctrl_window.py --check <csv>      # (b)
py -3.12 re/tools/ai_speed_env.py   --check <csv>      # (e)
# new this session, read-only
py -3.12 re/tools/ai_mode_split.py <csv>               # G2-MODE / G2-JOINT / G2-STEER
py -3.12 re/tools/ai_band_sim.py --orig <o.aistep.csv> --sa <sa.csv> --out <report.txt>
#   now carries modeseq + curvseq counterfactual arms; its >=95% validation gate stands
py -3.12 re/tools/ai_posmatch.py --orig <o.aistep.csv> --port <r.csv> --part A|B|COLL
#   NEW 2026-10-02. Implements verify/d3_offline_20261002/PREREG.md verbatim: the
#   position-matched curvature test (A), the c0 branch split with both known-answer
#   checks (B), and a DESCRIPTIVE collateral/err-decomposition leg (COLL, no pass/fail).
#   Its constants and bands are pre-registered and ARE NOT TO BE MOVED.
# guards
pwsh -NoProfile -File re/tools/pu_replay/sweep.ps1
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --oracle --rule 3 --hold 50
```

## Standing rules

Launch muted; `MASHED_TITLE` on every run; `MASHED_WIN_POS=primary-bl`;
`--poke-ctrl-slots` on race captures; **never** `MASHED_NAV_DEMO`. Track PIDs, kill only
yours. Frida **entry hooks only**. Ghidra pool clones `-readOnly`; never write the master.
Never `unlock_*` on `original/MASHED.exe`. Build via `mashedmod\build.bat` from PowerShell.
Trackers only through `re-classify`, preserving each file's line endings — `DEFERRED.md`,
`UNCERTAINTIES.md`, `hooks.csv`, `STUBS.md` are **CRLF**; `ROADMAP.md`,
`re/analysis/CHANGELOG.md` and this file are **LF**. Pre-register every gate; if one fails,
STOP, and state prominently any gate or rule you replace.

## Still open

- **D3 (b) is still NOT MET** and is the phase's sole blocker. `U-9183` and `U-9182` are
  RESOLVED; **`U-9185`** now carries the remainder (the start-boost scaffold, the
  sub-degree heading residual, the knife-edge band split). `U-9184` stands as filed.
- `[UNCERTAIN]` the `look_z` tail on car 1 (matched-position median **0.7678**, p90
  **5.1128**) — below defect size at the median, but the only matched-position field
  other than speed with a non-trivial tail.
- **D2 parked, D-11071:** `U-9180`, `U-9181`. Two informational **D2 WATCH** rows were
  filed 2026-10-02 (`D3_B_OFFLINE_2026-10-02.md` §6): AI-slot speed is **+34..41 %** on
  track 0 / mode 10 with the start-boost scaffold in place (D-11071's parked metric was
  the **player** 1.3 % **short** on Training — opposite sign, different car class,
  different scenario, so it neither confirms nor refutes that carrier), and the body
  heading is a physics output. **No D2 code was read or changed.**
