# Save-state editor tool — design

Filed as DEFERRED: see `re/DEFERRED.md` (work item to be added with this design as the spec).

## Why this exists

Race-scenario C4 promotion is currently gated on save-unlock state. The user can manually re-enter cheat codes per session, but that's a 5-10 minute tax on every race-scenario test and a friction barrier on iteration. A small tool that flips the unlock bits directly in `original/gamesave.bin` removes the gate.

## Current knowledge (from `re/analysis/structs/gamesave_layout.md`)

```
File size: 0x24FA0 (151,456 bytes)

0x0000 (4B):       magic 0xDEADBEEF              [written by SaveSerializeToBuffer]
0x0004..0x2443B:   profile data (0x2443C bytes)  [REP MOVSD from *DAT_008A94A8, 0x928F dwords]
0x2443F..0x24F9F:  tail (0xB60 bytes, defaults)  [NOT written by Save::SerializeToBuffer; shipped non-zero]

Tail sub-regions (in shipped file):
  0x24A44..0x24C9F: sparse 0x01/0x02 byte run    [championship progression flags — hypothesis]
  0x24CB0..0x24DE7: track ID sequences           [4-byte LE, range 0x07..0x13, 0xFFFFFFFF sentinels]
  0x24E1C..0x24E4F: 13 floats = ~0.9898          [13-track championship completion %?]
  0x24E50..0x24EEF: 160 bytes = 0x01             [unlock/completion flags — hypothesis]
  0x24F00..0x24F0B: 3 floats = ~0.7              [audio mix?]
  0x24F10:          0x02
  0x24F57..0x24F5B: 0x01 × 5
```

**Key uncertainty:** the user reports "everything is locked" on a fresh save, but the shipped tail has 160 bytes of 0x01 at 0x24E50 (the "unlock flags" hypothesis). Either that hypothesis is wrong OR the 0x01s mean something else (max-possible-unlock counts? difficulty defaults?). The actual gating field is in the profile-data region (0x0004..0x2443B) which is zero on a fresh save and gets populated as the player progresses.

**Conclusion: the unlock field hypothesis from the shipped tail is unreliable.** Need primary evidence via observing real saves before/after cheat-unlock.

## Plan — in 3 phases

### Phase 1 — Observation (Frida, ~30 min)

Goal: produce a byte-diff of gamesave.bin between (a) fresh save and (b) save after cheats applied.

Steps:
1. Back up the current `original/gamesave.bin` to `original/gamesave.bin.fresh`.
2. Launch MASHED.exe via Frida with an exception-catcher + write-watch on `DAT_008A94A8` (the profile base) — see `re/frida/poll_attach_catch_crash.py` for the spawn pattern.
3. From the main menu, apply the cheat codes that the user already knows. Save the game.
4. Exit MASHED. Copy the new `original/gamesave.bin` to `original/gamesave.bin.unlocked`.
5. `cmp -l fresh unlocked | python -c "..."` to produce a list of (offset, fresh_byte, unlocked_byte) triples.
6. Document the diff in `re/analysis/structs/gamesave_unlock_diff.md`.

Expected output: a small set of byte ranges (probably ≤ 20 distinct fields) that the cheats flip. Most likely fields:
- A "tracks unlocked" bitmask (1 bit per track, ~20 tracks = 4 bytes)
- A "vehicles unlocked" bitmask
- A "modes unlocked" bitmask
- A championship-progress dword

### Phase 2 — Editor tool (~60 min)

Goal: extend `re/tools/` with a `gamesave_edit.py` CLI that can:
- `gamesave_edit.py show <path>` — print the unlock fields in a readable form
- `gamesave_edit.py unlock-all <path>` — flip every "unlocked" field to "fully unlocked"
- `gamesave_edit.py unlock-tracks <path> --tracks=1,2,5` — selective unlocks
- `gamesave_edit.py round-trip-test <path>` — extract→edit→write→re-read, confirm idempotent

Implementation:
- Use `gamesave_layout.md` as the field map source-of-truth
- Encode field offsets as constants at top of `gamesave_edit.py`
- Preserve the `0xDEADBEEF` magic (don't touch offset 0x0000)
- Preserve unknown-but-non-zero tail regions verbatim (don't speculatively zero them — risk of corrupting non-unlock state)

### Phase 3 — Verification (~15 min)

Goal: confirm the edited save loads cleanly in MASHED.

Steps:
1. Edit a fresh save with `gamesave_edit.py unlock-all`.
2. Launch MASHED.
3. Confirm: main menu reflects unlocked state (tracks/vehicles selectable).
4. Try a quick race; confirm it loads.
5. If anything in the save is corrupted, the game will refuse to load it or crash — that's a Phase-1 hypothesis failure; iterate.

## Acceptance criteria

- `gamesave_edit.py unlock-all original/gamesave.bin` produces a save that MASHED loads and runs without manual cheat entry
- A round-trip test (`read → no-op → write → read`) preserves all bytes byte-identical
- The editor's field-map matches the actual cheat-observed diff (no speculation in field semantics)
- D-???? (the not-yet-filed DEFERRED row tracking this work) is closed

## What's deliberately out of scope

- Editing the profile data (0x0004..0x2443B) beyond unlock fields — saves can have arbitrary progress state we shouldn't speculatively rewrite
- Editing the championship progression byte run (0x24A44..0x24C9F) without per-byte evidence
- Cross-save merging (combining two saves) — niche, defer

## Estimated total effort: 1 session (~1.5–2 hours)

Phase 1 is the critical path. If the cheat-flip diff is messier than expected (e.g. encrypted or checksummed), Phase 2 expands accordingly. Worst case: 2 sessions.
