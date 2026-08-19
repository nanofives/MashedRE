# The 21 patch scripts: what is applied, what must never be — 2026-08-18

`scripts/` holds 21 `patch_mashed_*.py`. `CLAUDE.md` documented ten. A byte-diff of
`original/MASHED.exe` against `.unpatched` proves which are live: 111 differing bytes in 14
clusters, all consumed by **nine**. `skip_intro` is a data patch. **Eleven are inert.**

## The guardrail that was missing

**Three scripts must NEVER be applied to `original/MASHED.exe`:** `unlock_all.py`,
`unlock_restore.py`, `unlock_tracks.py`.

`original/MASHED.exe` is not just the game — it is the **diffing reference** every
behavioural verification is measured against. The unlock family forces
`VehicleUnlockFlagGet` / `TrackAvailGet` and the working arrays at `0x007f0a40` /
`0x007f0e50` to non-stock values. Apply one and every Frida diff touching car select,
track select, championship progression or save-restore compares **modded against modded**,
silently, while still reporting GREEN. `unlock_restore` is the worst: it persists across
save-restore.

`force_keyboard.py` belongs in the same caution: it forces the joystick count to 0, so it
diverges from stock input handling. Acceptable as a temporary A/B toggle, never as standing
state on the reference.

## Applied (nine binary + one data)

`show_windowed`, `skip_audio_com`, `skip_selector`, `skip_controller_dialog`,
`fix_camera_res`, `disable_log`, `fix_fopen`, `fix_joypad`, `skip_movies`, plus the
`skip_intro` data patch. Addresses in `re/analysis/binary_claims_audit_20260818.md`.

## The eleven inert scripts

| script | class | disposition |
|---|---|---|
| `no_focus_pause` | SAFE-DEV | **Adopt candidate.** See below |
| `clean_exit` | SAFE-DEV | Useful against dirty-exit crash dumps. **Overlaps `skip_teardown` — never apply both**, they both hijack shutdown to `ExitProcess` |
| `fix_movie_uaf` | SAFE-DEV | Real crash-hardening for degenerate-movie teardown. Relevant *because* `skip_intro` swaps in 167,414-byte stand-ins that may be exactly the degenerate case it guards |
| `skip_intro_logos` | SAFE-DEV | Superseded by the applied `skip_intro` data patch |
| `heap_reserve` | BOOT-CRITICAL | Superseded by `fix_fopen` + the compat shim. [UNCERTAIN] formally abandoned — no note in file |
| `skip_teardown` | BOOT-CRITICAL | Superseded by `fix_fopen`/`fix_joypad`, the root fixes |
| `force_keyboard` | GAMEPLAY-ALTERING | Superseded by `fix_joypad`, which its own comment calls the band-aid's replacement. Keep as A/B only |
| `unlock_all` | CHEAT/UNLOCK | **Never apply to the reference.** Its siblings declare it incomplete |
| `unlock_tracks` | CHEAT/UNLOCK | **Never apply to the reference** |
| `unlock_restore` | CHEAT/UNLOCK | **Never apply to the reference.** Persists across save-restore |
| `skip_powerups` | RETIRED | Keep — the refusal guard is load-bearing, do not delete |

## The one worth adopting: `no_focus_pause`

Stock Mashed blocks on `WaitMessage()` and freezes whenever its window loses focus
(`no_focus_pause.py:9-12`). One signature-guarded byte at `0x004996d3`, `JNZ` → `JMP`,
makes the loop keep advancing while another window holds focus (`:20-22`). Reversible via
`--restore`. Disclosed cost: a spinning core while unfocused (`:17`).

**Scope note, important.** This affects `original/MASHED.exe` only. The standalone does not
need it — `exe_main.cpp:1665` already bypasses the focus gate for the demo drivers, which
is why the 16-shot capture is unattended today. `no_focus_pause` matters for **original-side**
work: Frida diffs, `MASHED_ORIG_BBDUMP` captures, `race_draw_burst.py`. Those are the runs
that can stall when a terminal takes focus.

It is not applied, and this note does not apply it — adopting it changes the reference
binary and is a decision for the human.
