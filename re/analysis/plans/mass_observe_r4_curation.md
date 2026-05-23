# Mass-observe R4 candidate curation

**Generated:** 2026-05-23 EOD, post-lever3-expansion + U-0448 narrow.
**Pool:** 153 C2 candidates passing c3_filter_v4 (`u0448_check_passed.tsv`),
excluding 30 already promoted in R1+R2+R3 + 9 known hot-path.

## Subsystem distribution

- boot: 82
- frontend: 11
- render: 46
- util: 14

## Critical exclusion: named CRT library functions

Lever-3 expansion promoted ~79 CRT-band C1→C2 via FidDB attestation. Many of these
are named library functions (`_strncpy`, `_malloc`, `__exit`, `__onexit_lk`, etc.).
**Per gta-reversed model, named CRT functions are NOT reimpl candidates** — they stay
as OS-side CRT calls. C2 attestation via FidDB is their final destination; promoting
to C3 adds no value.

R4 must filter out named CRT (regex `^(_|__|___)[a-zA-Z]+$`) before authoring reimpls.

## Top R4 candidates (likely boot-time fires, NOT CRT-library)

These need reimpls authored before observation can promote them:

| RVA | Subsystem | Why included | Risk |
|-----|-----------|--------------|------|
| 0x004026d0 | boot | boot_lowrva candidate | Unknown firing |
| 0x00402f50 | boot | boot_lowrva candidate | Unknown firing |
| 0x004114c0 | boot | boot_lowrva candidate | Unknown firing |
| 0x00404830 | boot | perm-PIZ loader (asset-load boot) | Track-state-only? |
| 0x00412890 | boot | panel-PIZ loader | May not fire at menu |
| 0x00420d00 | boot | panel-PIZ loader | May not fire at menu |
| 0x00425bc0 | boot | perm-PIZ loader sibling | Same |
| 0x00431ae0 | boot | boot_lowrva | Unknown |
| 0x00431af0 | boot | boot_lowrva sibling | Unknown |
| 0x00431b00 | boot | boot_lowrva sibling | Unknown |
| 0x004a2bf7 | boot | CRT-band residue FUN_* (not named CRT) | Pre-OS-resume risk |
| 0x004a31e1 | boot | CRT-band residue | Pre-OS-resume risk |
| 0x004a31ea | boot | CRT-band residue | Pre-OS-resume risk |
| 0x004963d0 | boot | thunk_FUN_00496370 (DInput release thunk?) | Exit-time only? |
| 0x00499cc0 | boot | WindowDestroy — exit-time | Will not fire at menu observation; EXCLUDE |
| 0x00492770 | boot | MainLoopInit — round-1 INCONCLUSIVE (fires pre-spawn) | Same issue; EXCLUDE |

## Curated R4 launch list (15 candidates, boot-only)

Focus on PIZ asset loaders + boot_lowrva game-code candidates. Skip exit-time
(WindowDestroy), skip already-inconclusive (MainLoopInit), skip CRT library.

```
0x004026d0  boot_lowrva FUN_004026d0
0x00402f50  boot_lowrva FUN_00402f50
0x00404830  perm-PIZ loader
0x004114c0  boot_lowrva FUN_004114c0
0x00412890  panel-PIZ loader
0x00420d00  panel-PIZ loader
0x00425bc0  perm-PIZ loader
0x00431ae0  boot_lowrva FUN_00431ae0
0x00431af0  boot_lowrva FUN_00431af0
0x00431b00  boot_lowrva FUN_00431b00
0x004a2bf7  CRT-band residue (game-code in CRT addr range)
0x004a31e1  CRT-band residue
0x004a31ea  CRT-band residue
0x00428320  frontend menus init (from earlier inconclusive — retry with longer idle?)
0x004671a0  KNOWN HOT-PATH — EXCLUDE from Interceptor path; route to behavioral lane
```

## Expected R4 yield

Realistic: 5-8 promotions of 14 attempted (50-60% range).

PIZ loaders (4 candidates): may not fire at menu — they trigger on track-load, not
boot. **Round 2 already saw them count=0**. Probably defer.

boot_lowrva (6 candidates): unknown firing pattern. Some may be init-once at boot,
others may be track-state. Likely 2-3 fire.

CRT-band residue FUN_* (3 candidates): same risk as round-2's ___sbh_heap_init
(fires pre-OS-resume). Likely 0-1 fire.

Frontend retry (1): low yield, already inconclusive.

**Better path forward:** Skip R4 mass-observe of pre-existing-not-firing candidates.
Pivot to:
1. Hot-path behavioral lane (in progress this session)
2. Reimpl-author + targeted-observation for specific high-value boot-time hooks
3. Phase 6 step 2 (link mashed_re.exe with the 277 C3 hooks we have)

## Lessons for future R-N rounds

- **PIZ asset loaders don't fire at boot-to-menu** (4 candidates verified count=0 across R2+R3).
  Need a track-load scenario, currently blocked by `project_runtime_blocked`.
- **CRT-band residue without FidDB names** may fire pre-OS-resume (`___sbh_heap_init` did).
  Try once each; expect ~10-30% yield.
- **Named CRT functions are not C3-reimpl candidates.** They stay at C2 via FidDB.
  This means lever-3 expansion C2 promotions plateau at C2; not all C2 candidates are
  C3-eligible.
- **Frame-state-heavy candidates need the hot-path behavioral lane**, not Interceptor.
  This session's hot-path-behavioral worktree is testing the approach.
