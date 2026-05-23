# Replace MASHED's intro/splash .mpg videos with a 1-frame empty MPEG to skip
# intro playback without binary-patching the game.
#
# Approach (credit: SciLor's MashedRunner — see re/prior_art/MashedRunner/Intro.cs):
# the game's HardwareShowIntroVideo (FUN_00495350 at 0x00495350) cycles through
# 4 intro stages, each playing a .mpg via DirectShow. Each stage's loop exits
# when the DirectShow source pin signals end-of-stream. So if we replace each
# .mpg with a tiny (1-frame) empty MPEG, the loop exits in microseconds and
# the function returns normally with all state properly initialized.
#
# This avoids the 3 binary-patch approaches that all crashed MASHED at t+3-6s
# (NOPing the call site, RET at function entry, or .asi hook with setup+
# cleanup but no playback). See re/analysis/intro_skip_investigation.md for
# the failed attempts.
#
# Why it works: the playback LOOP runs once-per-stage (no skip), but each
# iteration finishes immediately because the empty MPEG has no frames after
# the first. DirectShow filter graph + RW renderer state are all properly
# set up + torn down between stages.
#
# Files replaced (5 of 6 .mpg files in toastart/pc/movies/):
#   empire.mpg, intro.mpg, renderware.mpg, small.mpg, supersonic.mpg
# Kept as-is:
#   frontend.mpg — looping menu background video (would be visible empty)
#
# Safety:
#   1. Originals backed up to toastart/pc/movies/backup/ (first run only).
#   2. Re-running is a no-op if backup already exists.
#   3. Restore by running with --restore flag, OR manually move backup/*.mpg
#      back over toastart/pc/movies/.
import shutil
import sys
from pathlib import Path

ROOT          = Path(__file__).resolve().parent.parent
MOVIES_DIR    = ROOT / 'original' / 'toastart' / 'pc' / 'movies'
BACKUP_DIR    = MOVIES_DIR / 'backup'
EMPTY_MPG_SRC = ROOT / 're' / 'prior_art' / 'MashedRunner' / 'tmp' / 'SciLorsEmptyVideo.mpg'

# Per SciLor's MashedRunner Mashed.cs VIDEO_NAMES — 5 intro stage videos.
# frontend.mpg is intentionally excluded (menu background loop).
VIDEO_NAMES = ['empire.mpg', 'intro.mpg', 'renderware.mpg', 'small.mpg', 'supersonic.mpg']


def apply() -> int:
    if not MOVIES_DIR.exists():
        print(f'FATAL: {MOVIES_DIR} not found.', file=sys.stderr)
        return 1
    if not EMPTY_MPG_SRC.exists():
        print(f'FATAL: empty-video resource not found at {EMPTY_MPG_SRC}', file=sys.stderr)
        return 2

    BACKUP_DIR.mkdir(exist_ok=True)
    empty_bytes = EMPTY_MPG_SRC.read_bytes()
    replaced = 0
    already_patched = 0

    for name in VIDEO_NAMES:
        target = MOVIES_DIR / name
        backup = BACKUP_DIR / name
        if not target.exists():
            print(f'  skip: {name} not present in {MOVIES_DIR.name}/')
            continue
        target_size = target.stat().st_size
        if target_size == len(empty_bytes) and target.read_bytes() == empty_bytes:
            already_patched += 1
            print(f'  noop: {name} is already the empty video')
            continue
        # First-run backup (don't overwrite an existing backup).
        if not backup.exists():
            shutil.copy2(target, backup)
            print(f'  backed up: {name} -> backup/{name} ({target_size} bytes)')
        target.write_bytes(empty_bytes)
        replaced += 1
        print(f'  replaced: {name} ({target_size} -> {len(empty_bytes)} bytes)')

    print()
    print(f'Result: {replaced} replaced, {already_patched} already patched.')
    if replaced > 0:
        print(f'Intro skip active. MASHED.exe will boot to main menu in <2s.')
    return 0


def restore() -> int:
    if not BACKUP_DIR.exists():
        print(f'No backup to restore from at {BACKUP_DIR}', file=sys.stderr)
        return 1
    restored = 0
    for name in VIDEO_NAMES:
        backup = BACKUP_DIR / name
        target = MOVIES_DIR / name
        if not backup.exists():
            print(f'  skip: backup/{name} not found')
            continue
        if target.exists():
            target.unlink()
        shutil.move(str(backup), str(target))
        restored += 1
        print(f'  restored: backup/{name} -> {name}')
    # Remove backup dir if empty
    try:
        BACKUP_DIR.rmdir()
        print(f'  removed empty backup/ directory')
    except OSError:
        pass
    print()
    print(f'Result: {restored} restored.')
    return 0


def main() -> int:
    if len(sys.argv) > 1 and sys.argv[1] == '--restore':
        return restore()
    return apply()


if __name__ == '__main__':
    sys.exit(main())
