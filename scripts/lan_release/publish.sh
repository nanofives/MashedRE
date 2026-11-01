#!/usr/bin/env bash
# publish.sh - build mashed_re.exe from a committed ref, stage the LAN release folder and publish it to the
# mini-server (copy of SimGolf's scripts/lan_release/publish.sh, itself from SimCity / TD5RE). Run from LIVING-PC;
# publish_mashed.bat wraps it for a double-click.
#
#   publish.sh                  clean build of race/first-frame-parity, stage, deploy
#   publish.sh --ref <ref>      build another committed ref
#   publish.sh --no-build       stage and deploy the last build again (e.g. after a server reset)
#
# The roadmap loop runs it on its own when release_due.py says DUE (CLAUDE.md "LAN release").
#
# The build is a fresh `git archive <ref>` so uncommitted work in the shared tree never ships. It builds with
# MASHED_NO_DEPLOY=1, so the shared original\mashed_re_dev.asi is never touched. Everything lands under
# mashedmod/build/lan_build and mashedmod/build/lan_release of the MAIN checkout (gitignored), whichever worktree the
# script runs from: original\ only exists there.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$(dirname "$(git -C "$HERE" rev-parse --path-format=absolute --git-common-dir)")" && pwd)"
BUILD="$ROOT/mashedmod/build/lan_build"
STAGE="$ROOT/mashedmod/build/lan_release"
INSTALL="$ROOT/original"
APP="$BUILD/mashedmod/build"
cd "$ROOT"

REF="race/first-frame-parity"
NO_BUILD=0
while [ $# -gt 0 ]; do
    case "$1" in
        --ref) REF="$2"; shift 2 ;;
        --no-build) NO_BUILD=1; shift ;;
        *) echo "[!] unknown argument $1"; exit 1 ;;
    esac
done

sha="$(git rev-parse --short "$REF")"
subject="$(git log -1 --format=%s "$REF" | cut -c1-60)"
label="Mashed RE $sha ($(git log -1 --format=%cd --date=short "$REF"))"

if [ "$NO_BUILD" = 0 ]; then
    echo "[+] Clean build of $REF $sha: $subject"
    rm -rf "$BUILD"
    mkdir -p "$BUILD"
    # What the build reads outside mashedmod/ (../scripts/lint_*.py, the dual-copy allowlist, the re/match/
    # sources the C4-by-match TUs #include) is listed in scripts/lan_release/archive_paths.txt, read AT THE REF.
    # The merge train runs scripts/lan_release/archive_lint.py on the same list (HB-LINT-408A50: the
    # hard-coded list here had no re/match/, so the lint lost a body the train's full worktree still saw).
    paths="$(git show "$REF:scripts/lan_release/archive_paths.txt" 2>/dev/null | sed 's/#.*//' | tr -d '\r' | xargs || true)"
    [ -n "$paths" ] || paths="mashedmod scripts re/tools/dual_copy_allowlist.txt re/match"
    echo "[+] Archive paths: $paths"
    # shellcheck disable=SC2086
    git archive "$REF" $paths | tar -x -C "$BUILD"
    export MASHED_NO_DEPLOY=1
    cmd //c "$(cygpath -w "$BUILD/mashedmod/build.bat")"
fi
[ -f "$APP/mashed_re.exe" ] || { echo "[!] no $APP/mashed_re.exe (build failed?)"; exit 1; }

echo "[+] Staging $label"
py -3.12 "$HERE/stage_lan_release.py" --app "$(cygpath -w "$APP")" --install "$(cygpath -w "$INSTALL")" \
    --out "$(cygpath -w "$STAGE")" --label "$label"

echo "[+] Publishing"
bash "$HERE/deploy_lan_release.sh" "$STAGE"

# history of what went out (release_due.py reads the server's manifest, not this file)
mkdir -p "$ROOT/log/orch"
printf '%s\t%s\t%s\n' "$(date '+%F %T')" "$sha" "$label" >> "$ROOT/log/orch/lan_releases.tsv"
