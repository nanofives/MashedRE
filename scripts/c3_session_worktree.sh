#!/bin/bash
# c3_session_worktree.sh — robust per-session worktree setup for C2->C3 (and any
# parallel-fanout) batches. Eliminates the two dominant stop-and-ask causes seen
# in c3_batch_ab (2026-06-04):
#   (1) BRANCH/BATCH-ID COLLISION: a reused batch letter collides with a stale
#       branch of the same name -> `git worktree add -b <branch>` fails or lands
#       on the WRONG base (stale commit, missing recent work). Workers then had
#       to stop and ask the human how to name/rebase.
#   (2) FALLBACK-TO-MAIN: when the worktree-add failed, a session worked in the
#       MAIN repo instead, racing sibling sessions on shared tracked files.
#
# This script makes worktree creation deterministic and SAFE:
#   - Picks a COLLISION-FREE branch name (appends -v2,-v3,... if taken).
#   - Bases the new branch off an EXPLICIT integration ref (default the repo's
#     integration branch), never off a stale same-named branch.
#   - Creates the worktree under .worktrees/ and prints the path to cd into.
#   - NEVER touches the main checkout; exits non-zero with a clear message on
#     any failure so the session STOPS cleanly instead of falling back to main.
#
# Usage:
#   scripts/c3_session_worktree.sh <session-label> [base-ref]
#     <session-label>  e.g. c3-batch-ab-s1   (becomes branch c3/<label> minus the
#                      leading "c3-" normalization -> c3/batch-ab-s1)
#     [base-ref]       integration ref to branch from. Default: $C3_BASE_REF or
#                      feature/standalone-frontend-b15-b16. Pass a SHA to pin all
#                      sibling sessions to the exact batch-plan commit.
#
# Output (stdout, last line): the absolute worktree path to `cd` into.
# On success it also prints BRANCH=<name> and BASE=<resolved sha> for logging.
set -euo pipefail

REPO_ROOT="C:/Users/maria/Desktop/Proyectos/Mashed"
cd "$REPO_ROOT"

LABEL="${1:-}"
if [[ -z "$LABEL" ]]; then
    echo "ERROR: session label required. e.g. $0 c3-batch-ab-s1" >&2
    exit 2
fi

# Branch name: "c3-batch-ab-s1" -> "c3/batch-ab-s1" (first hyphen -> slash).
BRANCH="${LABEL/-//}"

DEFAULT_BASE="${C3_BASE_REF:-feature/standalone-frontend-b15-b16}"
BASE_REF="${2:-$DEFAULT_BASE}"

# Resolve the base to a concrete SHA up front so a moving branch tip can't make
# two siblings diverge mid-run. Fail loudly if the ref doesn't exist.
if ! BASE_SHA="$(git rev-parse --verify --quiet "${BASE_REF}^{commit}")"; then
    echo "ERROR: base ref '${BASE_REF}' does not resolve to a commit." >&2
    echo "       Pass a valid integration branch or SHA as arg 2, or set C3_BASE_REF." >&2
    exit 3
fi

WT_DIR=".worktrees/${LABEL}"

# If this exact worktree already exists and is on a branch off the SAME base,
# it's a resume — reuse it (idempotent). If it exists on a different base, that's
# the duplicate-instance case: do NOT clobber, report and stop.
if git worktree list --porcelain | grep -qx "worktree $(cd "$REPO_ROOT"; pwd)/${WT_DIR}" 2>/dev/null \
   || [[ -d "$WT_DIR" ]]; then
    echo "NOTE: worktree '$WT_DIR' already exists — reusing (resume)." >&2
    echo "BRANCH=$(git -C "$WT_DIR" rev-parse --abbrev-ref HEAD 2>/dev/null || echo '?')" >&2
    echo "$(cd "$REPO_ROOT"; pwd)/${WT_DIR}"
    exit 0
fi

# Collision-free branch name: if BRANCH already exists (e.g. a stale batch reused
# the letter), append -v2, -v3, ... The new branch is ALWAYS based off BASE_SHA,
# never off the stale same-named branch.
PICK="$BRANCH"
n=2
while git show-ref --verify --quiet "refs/heads/${PICK}"; do
    echo "NOTE: branch '${PICK}' already exists (stale/collision) — trying next." >&2
    PICK="${BRANCH}-v${n}"
    n=$((n+1))
    if (( n > 20 )); then
        echo "ERROR: could not find a free branch name after 20 tries for '${BRANCH}'." >&2
        exit 4
    fi
done

# Create the worktree off the pinned base SHA. -b makes the fresh branch.
if ! git worktree add -b "$PICK" "$WT_DIR" "$BASE_SHA" >&2; then
    echo "ERROR: 'git worktree add' failed for $WT_DIR (branch $PICK, base $BASE_SHA)." >&2
    echo "       Do NOT fall back to the main checkout — resolve the error and retry." >&2
    exit 5
fi

echo "BRANCH=$PICK" >&2
echo "BASE=$BASE_SHA (${BASE_REF})" >&2
echo "WORKTREE READY — cd into the path below; do all work there, never in main." >&2
echo "$(cd "$REPO_ROOT"; pwd)/${WT_DIR}"
