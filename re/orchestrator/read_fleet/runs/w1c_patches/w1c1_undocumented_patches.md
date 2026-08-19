All eleven files read. Report follows, strictly from source.

---

# Per-script reports

## 1. `patch_mashed_clean_exit.py`

1. **PURPOSE** ÔÇö "When MASHED quits (WinMain returns), the CRT exit path crashes with eip=0 ... redirect doexit's entry to ExitProcess(code) ÔÇö a clean OS-level terminate that skips the crashing CRT cleanup." (`clean_exit.py:1-15`)
2. **MECHANISM** ÔÇö Single site `0x004a3258` (doexit, `FUN_004a3258`) entry. Overwrites 12 bytes `558bec56576a08e81b460000` (push ebp; mov ebp,esp; push esi; push edi; push 8; call __lock) with `ff742404 ff15b8c15c00 c3 cc` = `push [esp+4]; call [ExitProcess] (IAT 0x5cc1b8); ret; int3`. (`clean_exit.py:37-39`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED. The header argues it *is* benign ("its exit path is never diffed, so dropping CRT exit-cleanup is benign"), giving no note of retirement or supersession. (`clean_exit.py:16-18`)
4. **RISK CLASS** ÔÇö SAFE-DEV: reversible, touches only the exit dispatcher, no gameplay effect; but note it *is* on the shutdown path (see verdict A caveat re: overlap with skip_teardown).
5. **SUPERSEDED?** ÔÇö Overlaps in intent with `skip_teardown` (both redirect a shutdown path to `ExitProcess`). Not superseded by any of the applied nine; none of the nine addresses the CRT doexit crash. `skip_teardown` (also unapplied) targets the same crash class one frame earlier.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.precleanexitpatch`. (`clean_exit.py:44-47`)

## 2. `patch_mashed_fix_movie_uaf.py`

1. **PURPOSE** ÔÇö "Fixes a latent use-after-free in MASHED's DirectShow movie teardown that crashes the game when a degenerate/short movie ... fails video-format negotiation." (`fix_movie_uaf.py:1-3`)
2. **MECHANISM** ÔÇö Two regions. Region 1 redirect at `0x004943b5`: 17 bytes `8b1050ff5208a1141a77008b0850ff5108` ÔåÆ `e9db480700` (JMP 0x508c95) + 12├ùNOP. Region 2 code cave at `0x00508c95`: 39 bytes of `cc` padding ÔåÆ gated+nulled releases of textures `0x771a10`/`0x771a14`. (`fix_movie_uaf.py:40-76`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED. The header presents it as a working fix.
4. **RISK CLASS** ÔÇö SAFE-DEV: a crash-hardening fix on the movie-teardown COM release path, reversible; no gameplay rule change.
5. **SUPERSEDED?** ÔÇö Not superseded by the applied nine. Note the header itself warns the cave `0x508bd5` is used by the applied `fix_joypad`, so it deliberately picks `0x508c95` to avoid collision (`fix_movie_uaf.py:51,56`). It is complementary to, not covered by, any applied patch.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.premovieuaf`. (`fix_movie_uaf.py:94-97`)

## 3. `patch_mashed_force_keyboard.py`

1. **PURPOSE** ÔÇö "Makes MASHED ignore physically connected joysticks, so it never takes the joystick code path that crashes on this Win11 build." (`force_keyboard.py:1-2`)
2. **MECHANISM** ÔÇö Site `0x00496040` (`FUN_00496040`, joypad enum). 16 bytes `5668bcff5c00e86527000083c404e8cd` ÔåÆ `c705ac2f770000000000 b801000000 c3` = `mov [0x772fac],0; mov eax,1; ret` (force count 0, return success, skip enum). (`force_keyboard.py:31-33`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED. It calls itself "the clean fix the joypad-guard patch only band-aided" (`force_keyboard.py:18`), implying it is an alternative to the applied `fix_joypad`, but gives no note of being rejected.
4. **RISK CLASS** ÔÇö GAMEPLAY-ALTERING (borderline BOOT-CRITICAL): it forces keyboard-only by zeroing the joystick count, which changes device-binding behaviour (`FUN_00498510` assigns players by that count) ÔÇö a behavioural divergence from stock, so it would compromise a faithful-behaviour joystick reference.
5. **SUPERSEDED?** ÔÇö Yes, effectively superseded by the applied **`fix_joypad`** (`0x00495870`), which is the guard the header calls the "band-aid." The applied recipe chose the guard over this heavier count-forcing rewrite.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.prekbdonlypatch`. (`force_keyboard.py:38-41`)

## 4. `patch_mashed_heap_reserve.py`

1. **PURPOSE** ÔÇö "Patch ORIGINAL MASHED.exe's PE optional-header SizeOfHeapReserve so ntdll places the default process heap HIGH (off the low 0x30000 region) at process init." (`heap_reserve.py:1-2`)
2. **MECHANISM** ÔÇö No code patch. Rewrites the PE optional-header field `SizeOfHeapReserve` at computed file offset `opt + 0x50` (opt = `e_lfanew + 24`) to a caller-supplied value via `--set 0xHEX`. Touches no RVA, no image base. (`heap_reserve.py:24-35,54-63`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as a retirement note. Note it requires an explicit `--set` value and has no default apply (bare invocation only `--show`s), so it is inert unless driven ÔÇö consistent with it being an abandoned experiment, though the file does not say so. [UNCERTAIN whether it was rejected or simply superseded ÔÇö the header gives no verdict.]
4. **RISK CLASS** ÔÇö SAFE-DEV: header-only, no code/RVA change, fully reversible, no gameplay effect.
5. **SUPERSEDED?** ÔÇö Yes, functionally superseded by the applied **`fix_fopen`** ("ROOT boot fix") plus the compat-shim EMULATEHEAP handling; this heap-reserve approach is an alternative boot-heap-corruption mitigation that the applied recipe did not adopt.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.preheappatch`. (`heap_reserve.py:48-53`)

## 5. `patch_mashed_no_focus_pause.py`

1. **PURPOSE** ÔÇö "On-disk patch (DEV/testing): stop MASHED pausing when its window loses focus." (`no_focus_pause.py:1`)
2. **MECHANISM** ÔÇö Single byte at file offset `0x000996d3` (RVA `0x004996d3`): flip `0x75` (JNZ) ÔåÆ `0xEB` (JMP), so `WaitMessage()` at `0x004996d5` is unconditionally skipped. Guarded by a 15-byte signature at `0x000996cc`. (`no_focus_pause.py:33-41`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED. Header frames it as an accepted dev/testing aid ("Acceptable for a dev/testing patch"). (`no_focus_pause.py:17`)
4. **RISK CLASS** ÔÇö SAFE-DEV, but strictly it *is* GAMEPLAY-ALTERING against a faithful reference: stock Mashed pauses/blocks when unfocused; this makes it run at full speed unfocused. For a diffing reference that distinction matters, but for unattended captures it is exactly the desired behaviour. Reversible; header notes the CPU-spin trade-off.
5. **SUPERSEDED?** ÔÇö No. None of the applied nine touches the focus/WaitMessage gate (`DAT_0077391c`). Unique.
6. **RESTORE PATH** ÔÇö Yes: `--restore` (JMPÔåÆJNZ), with signature verification. (`no_focus_pause.py:52-62`)

## 6. `patch_mashed_skip_intro_logos.py`

1. **PURPOSE** ÔÇö "Auto-skips the 4 boot logo videos (empire/supersonic/renderware/intro) for a fast boot, WITHOUT touching the D3D pump." (`skip_intro_logos.py:1-2`)
2. **MECHANISM** ÔÇö Site `0x00494a80` (`FUN_00494a80`, intro movie-start). 5 bytes `6884000000` (push 0x84) ÔåÆ `c3 90909090` (ret; nop├ù4). (`skip_intro_logos.py:31-33`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as retirement. Header notes it "Complements patch_mashed_skip_movies.py" (`skip_intro_logos.py:17`). Redundant against the applied **DATA** patch `skip_intro` (which replaces the five `.mpg` files), so it is functionally unnecessary ÔÇö but the file does not itself state that as the reason.
4. **RISK CLASS** ÔÇö SAFE-DEV: reversible, cosmetic boot-speed aid, no gameplay effect. (Touches the boot movie loop, so mild BOOT-CRITICAL adjacency, but the header argues it leaves the render pump intact.)
5. **SUPERSEDED?** ÔÇö Yes, by the applied intro handling: the data-level `skip_intro` (five 167,414-byte `.mpg` replacements) already skips these logos without touching `MASHED.exe`. `skip_movies` (applied) covers small.mpg.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.preintrologospatch`. (`skip_intro_logos.py:38-41`)

## 7. `patch_mashed_skip_teardown.py`

1. **PURPOSE** ÔÇö "SKIP-TEARDOWN clean-exit fix ... The real fix for the esp=0x1afe50 crash." ÔÇö at the WinMain teardown entry, call `ExitProcess(0)` so the crashing teardown never runs. (`skip_teardown.py:1,18-23`)
2. **MECHANISM** ÔÇö Site `0x004923de` (WinMain teardown entry). 10 bytes `6800f75c00 e8f83f0000` (push 0x5cf700; call 0x4963e0, the AppDestroy log) ÔåÆ `6a00 ff15b8c15c00 9090` = `push 0; call [ExitProcess] (IAT 0x5cc1b8); nop; nop`. (`skip_teardown.py:37-39`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as retirement. Header presents it as "The real fix," but it is unapplied. It explicitly does **not** fix the ~67% boot-init failure, only makes those exit cleanly (`skip_teardown.py:22-23`) ÔÇö likely why the applied `fix_fopen`/`fix_joypad` (which fix the root and boot 8/8) were chosen instead. [UNCERTAIN ÔÇö inference from the applied recipe, not stated in this file.]
4. **RISK CLASS** ÔÇö BOOT-CRITICAL: it rewrites the WinMain teardown/boot-fail junction to force-exit; a wrong-address or interaction with the applied boot fixes could break shutdown or successful-boot flow. Reversible.
5. **SUPERSEDED?** ÔÇö Yes, by the applied **`fix_fopen`** (ROOT boot fix, boots 8/8) and **`fix_joypad`**, which address the root causes this patch only exits-cleanly around. Overlaps `clean_exit` (both ÔåÆ ExitProcess) at a different frame.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from `MASHED.exe.preteardownpatch`. (`skip_teardown.py:44-47`)

## 8. `patch_mashed_unlock_all.py`

1. **PURPOSE** ÔÇö "makes all vehicles and all tracks available so a race can be started from a fresh save." (`unlock_all.py:1-2`)
2. **MECHANISM** ÔÇö Two getters, each entry overwritten with `B8 01 00 00 00 C3` (MOV EAX,1; RET). File offset `0x2ef40` = `VehicleUnlockFlagGet 0x0042ef40`; file offset `0x30830` = `TrackAvailGet 0x00430830`. (`unlock_all.py:36-42`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as retirement ÔÇö but note its own sibling files declare it insufficient: `unlock_tracks.py:2-8` says "the getter patch in patch_mashed_unlock_all.py is NOT enough" because renderers read `DAT_007f0a40` directly, bypassing the getter.
4. **RISK CLASS** ÔÇö CHEAT/UNLOCK (and GAMEPLAY-ALTERING): unlocks all cars/tracks, corrupting any faithful-behaviour reference.
5. **SUPERSEDED?** ÔÇö Not by the applied nine (unrelated domain). Within the unlock family it is the weakest/incomplete variant, superseded by `unlock_restore` / `unlock_tracks`.
6. **RESTORE PATH** ÔÇö Yes: `--restore` from shared `MASHED.exe.preunlock_bak`. (`unlock_all.py:51-57`)

## 9. `patch_mashed_unlock_restore.py`

1. **PURPOSE** ÔÇö "Unlock-everything (PERMANENT) patch ... patches the SAVE-STATE RESTORE" so mode-entry always leaves the unlock arrays populated. (`unlock_restore.py:1,13-22`)
2. **MECHANISM** ÔÇö Redirect at `0x00404eb4` (6 bytes `8b3da8948a00`, MOV EDI,[0x8a94a8]) ÔåÆ JMP to cave + 1 NOP. Cave at `0x005caf30` fills `0x007f0a40` with 2 (156 int32, tracks) and `0x007f0e50` with 1 (156 bytes, cars), replays the displaced instruction, then JMPs back to `0x00404eba`. (`unlock_restore.py:36-71`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as retirement. It is the most complete unlock variant ("PERMANENT"), correctly identifying that `FUN_00404e80` REP-MOVSDs over both arrays and re-locks them (`unlock_restore.py:3-11`).
4. **RISK CLASS** ÔÇö CHEAT/UNLOCK (and GAMEPLAY-ALTERING): permanently forces all unlocks on every mode-entry ÔÇö maximally corrupting as a faithful reference.
5. **SUPERSEDED?** ÔÇö Not by the applied nine. Within the family it *supersedes* `unlock_all` (whose getter approach it proves ineffective) and overlaps `unlock_tracks`.
6. **RESTORE PATH** ÔÇö Yes: `--restore` (and a `--check`) from shared `MASHED.exe.preunlock_bak`. (`unlock_restore.py:81-86,92-99`)

## 10. `patch_mashed_unlock_tracks.py`

1. **PURPOSE** ÔÇö "Unlock-all-TRACKS patch" ÔÇö appends a fill loop to the state initializer so the championship/track table reads as all-available at boot. (`unlock_tracks.py:1,10-24`)
2. **MECHANISM** ÔÇö Redirect at file `0x9267f` (VA `0x0049267f`): CALL `0x00431d00` (`e87cf6f9ff`) ÔåÆ CALL `0x005caf00` (`e87c881300`). Cave at file `0x1caf00` (VA `0x005caf00`), 35 zero bytes ÔåÆ fill `0x007f0a40` with 2 (156 int32), replay displaced `call 0x431d00`, ret. (`unlock_tracks.py:41-60`)
3. **WHY NOT APPLIED** ÔÇö NO REASON STATED as retirement. Written specifically because `unlock_all`'s getter patch "does nothing for what the screens draw" (`unlock_tracks.py:1-9`).
4. **RISK CLASS** ÔÇö CHEAT/UNLOCK (and GAMEPLAY-ALTERING): unlocks all tracks, corrupting a faithful reference.
5. **SUPERSEDED?** ÔÇö Not by the applied nine. Overlaps `unlock_restore` (which covers both tracks and cars and survives the save-restore); `unlock_restore` is the more complete of the two.
6. **RESTORE PATH** ÔÇö Yes: `--restore` (and `--check`) from shared `MASHED.exe.preunlock_bak`. (`unlock_tracks.py:69-75,81-86`)

## 11. `patch_mashed_skip_powerups.py` (RETIRED)

1. **PURPOSE** ÔÇö Header: "RETIRED PATCH ÔÇö DO NOT APPLY (kept as un-applier / refusal guard)." Original intent was to NOP the 25-byte powerups-load call sequence. (`skip_powerups.py:1,30`)
2. **MECHANISM** ÔÇö Target RVA `0x0040295d..0x402975`, the 25-byte sequence `push 0x5cc3b4 ("...powerups.piz"); call 0x00495280; call 0x0045bae0; call 0x00418980; call 0x004952f0`. The retired patch replaced it with 25├ù`0x90`. (`skip_powerups.py:31-39`)
3. **WHY NOT APPLIED** ÔÇö STATED: "NOPping the 25-byte powerups call sequence at RVA 0x0040295d causes a downstream stack-imbalance ret-to-0 ÔÇö it WAS boot crash #2 (eip=0). Removing the patch made the baseline boot to the real main menu ... The original 'FUN_004b6940 piz reader crash' this patch tried to solve was a Frida phantom." (`skip_powerups.py:3-9`)
4. **RISK CLASS** ÔÇö BOOT-CRITICAL: confirmed to cause boot crash #2.
5. **SUPERSEDED?** ÔÇö Not superseded; simply retired/removed. Its removal is itself the fix.
6. **RESTORE/UN-APPLY PATH** ÔÇö **Refusal guard CONFIRMED**: on the pristine file (PRE_PATCH signature present) it prints the retirement notice and `sys.exit("refusing to apply retired patch")` (non-zero) ÔÇö it never re-applies (`skip_powerups.py:54-58`). If the 25 NOPs are found it un-applies by restoring the original 25 bytes, requiring exactly one occurrence and the `.unpatched` backup to exist (`skip_powerups.py:60-84`). Otherwise it aborts. The guard behaves exactly as CLAUDE.md describes.

---

# Grouped verdicts

## A) Genuinely useful for current work and safe

The project runs **unattended game captures for a remote user**, so focus/keyboard/pause/shutdown behaviour is the operative axis.

- **`no_focus_pause.py`** ÔÇö *Directly on-target.* Stock Mashed calls `WaitMessage()` and freezes when unfocused (`no_focus_pause.py:9-12`); this makes it keep running so background/unattended captures don't stall when the terminal or Frida holds foreground focus (`no_focus_pause.py:20-22`). Single-byte, signature-guarded, reversible. **The single most relevant script to the unattended-capture use case.**
- **`clean_exit.py`** ÔÇö Useful for stopping dirty-exit crash dumps on shutdown; SAFE-DEV, reversible. Caveat: overlaps `skip_teardown` ÔÇö do not apply both; they both hijack the shutdown path to `ExitProcess`.
- **`fix_movie_uaf.py`** ÔÇö A real crash-hardening fix for degenerate-movie teardown UAF; safe and reversible, and specifically relevant because the applied `skip_intro` replaces movies with 167,414-byte stand-ins that could be the "degenerate/short movie" this UAF trips on (`fix_movie_uaf.py:2-3,17-19`). Worth considering if capture runs ever crash around movie transitions.
- **`skip_intro_logos.py`** ÔÇö Safe boot-speed aid, but redundant next to the applied `skip_intro` data patch (see C).

**Focus/keyboard/pause/shutdown call-outs, explicitly:** window-focus/pause ÔåÆ `no_focus_pause.py` (recommended). Keyboard input ÔåÆ `force_keyboard.py` (relevant but behaviour-altering, see B/caveat). Pause-when-unfocused ÔåÆ `no_focus_pause.py`. Clean shutdown ÔåÆ `clean_exit.py` and `skip_teardown.py` (pick one; `skip_teardown` is BOOT-CRITICAL, `clean_exit` is the lighter touch).

## B) Must NEVER be applied to `original/MASHED.exe` (would compromise the diffing reference)

- **`unlock_all.py`**, **`unlock_restore.py`**, **`unlock_tracks.py`** ÔÇö the unlock family. Applying any of them **would invalidate behavioural comparisons**: they force `VehicleUnlockFlagGet`/`TrackAvailGet` and the working arrays `0x7f0a40`/`0x7f0e50` to non-stock values, so any faithful-behaviour Frida diff of car-select, track-select, championship, or save-restore logic would diff modded-vs-modded, not modded-vs-stock. `unlock_restore` is the worst offender (permanent, survives save-restore). Say plainly: **yes, applying the unlock_* family corrupts `MASHED.exe` as the reference ÔÇö keep them off it.**
- **`force_keyboard.py`** ÔÇö should also be kept off the reference for *input/device* diffs: it forces the joystick count to 0 and changes device-binding behaviour (`force_keyboard.py:6-12`), diverging from stock joystick handling. Fine as a temporary A/B toggle, not as a standing state on the reference.

## C) Dead / experimental / superseded ÔÇö consider for deletion or archival

- **`skip_powerups.py`** ÔÇö RETIRED and confirmed harmful; keep ONLY as the refusal guard/un-applier (do not delete ÔÇö the guard is load-bearing).
- **`skip_intro_logos.py`** ÔÇö superseded by the applied data-level `skip_intro`; archival candidate.
- **`heap_reserve.py`** ÔÇö superseded by applied `fix_fopen` + compat-shim EMULATEHEAP handling; experimental boot-heap approach, no default apply. Archival candidate. [UNCERTAIN if formally abandoned ÔÇö no note in-file.]
- **`skip_teardown.py`** ÔÇö superseded by applied `fix_fopen`/`fix_joypad` (root fixes, boot 8/8); BOOT-CRITICAL and now redundant. Archival candidate.
- **`force_keyboard.py`** ÔÇö superseded by applied `fix_joypad` (the guard it calls a "band-aid"); keep as an A/B experiment only.
- **`unlock_all.py`** ÔÇö the incomplete unlock variant, its own siblings declare it insufficient; superseded by `unlock_restore`. Archival candidate (but see B ÔÇö never apply).

---

# Single recommendation

If exactly one of these eleven were to be adopted, adopt **`patch_mashed_no_focus_pause.py`**.

It is the only script whose effect maps directly onto the stated operating mode ÔÇö unattended captures for a remote user. Stock Mashed blocks on `WaitMessage()` and freezes whenever its window is not focused (`no_focus_pause.py:9-12`); flipping the one `JNZ`ÔåÆ`JMP` byte at `0x004996d3` makes the frontend and game loop keep advancing while another window holds focus (`no_focus_pause.py:20-22`), which is exactly what an unattended, background-running capture needs. It is the smallest and most surgical of the eleven (one signature-guarded byte), fully reversible via `--restore`, and it does not touch the boot chain or gameplay rules ÔÇö so it carries none of the reference-corrupting risk of the unlock family, none of the BOOT-CRITICAL risk of `skip_teardown`, and none of the redundancy of `skip_intro_logos`. The only cost the file itself discloses is a spinning CPU core while unfocused (`no_focus_pause.py:17`), which is acceptable for a capture host.
