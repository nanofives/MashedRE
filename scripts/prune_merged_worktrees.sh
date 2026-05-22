#!/usr/bin/env bash
# Prune git worktrees whose branch is already merged into main.
#
# Two passes:
#   1. Remove the worktree (working-tree directory). Skip if uncommitted changes
#      (we trust the merge as the source of truth; refuse to discard unintended
#      uncommitted work).
#   2. Delete the (now-orphaned) branch ref via `git branch -d` (safe-delete;
#      refuses if not merged).
#
# Also cleans the .claude/worktrees/agent-* short-lived worktrees if they are
# locked — those are leftover from background sub-agent runs.
#
# Idempotent: re-running is a no-op.

set -u

cd "$(git rev-parse --show-toplevel)"

REMOVED=0
SKIPPED_DIRTY=0
SKIPPED_MISSING=0
BRANCHES_DELETED=0
BRANCHES_KEPT=0

declare -a TO_REMOVE_PATHS=()
declare -a TO_DELETE_BRANCHES=()

# Build set of merged branches (excluding main itself)
mapfile -t MERGED_BRANCHES < <(git branch --merged main --format='%(refname:short)' | grep -v '^main$')

# Build the path-to-branch map for all worktrees
declare -A WT_BRANCH
WT_PATH=""
WT_BR=""
while IFS= read -r line; do
  if [[ $line == worktree* ]]; then
    WT_PATH="${line#worktree }"
    WT_BR=""
  elif [[ $line == branch* ]]; then
    WT_BR="${line#branch refs/heads/}"
    if [[ -n $WT_PATH && -n $WT_BR ]]; then
      WT_BRANCH[$WT_PATH]=$WT_BR
    fi
  fi
done < <(git worktree list --porcelain)

# Identify worktree paths whose branch is merged
for path in "${!WT_BRANCH[@]}"; do
  br="${WT_BRANCH[$path]}"
  # skip main
  if [[ $br == main ]]; then continue; fi
  for mb in "${MERGED_BRANCHES[@]}"; do
    if [[ $br == "$mb" ]]; then
      TO_REMOVE_PATHS+=("$path")
      TO_DELETE_BRANCHES+=("$br")
      break
    fi
  done
done

echo "Worktrees to remove (merged branches): ${#TO_REMOVE_PATHS[@]}"
echo

# Pass 1: remove worktrees
for path in "${TO_REMOVE_PATHS[@]}"; do
  if [[ ! -d $path ]]; then
    # already gone; just prune the bookkeeping
    git worktree prune --quiet 2>/dev/null
    SKIPPED_MISSING=$((SKIPPED_MISSING + 1))
    continue
  fi
  if git worktree remove "$path" 2>/dev/null; then
    REMOVED=$((REMOVED + 1))
  else
    SKIPPED_DIRTY=$((SKIPPED_DIRTY + 1))
    echo "  SKIPPED (dirty?): $path"
  fi
done

echo
echo "Pass 1 done: removed=$REMOVED skipped-dirty=$SKIPPED_DIRTY skipped-missing=$SKIPPED_MISSING"
echo

# Pass 2: delete branches (safe-delete only; refuses if not fully merged)
for br in "${TO_DELETE_BRANCHES[@]}"; do
  if git branch -d "$br" 2>/dev/null; then
    BRANCHES_DELETED=$((BRANCHES_DELETED + 1))
  else
    BRANCHES_KEPT=$((BRANCHES_KEPT + 1))
    echo "  KEPT (not safe to -d): $br"
  fi
done

echo
echo "Pass 2 done: branches-deleted=$BRANCHES_DELETED branches-kept=$BRANCHES_KEPT"
echo

# Pass 3: clean .claude/worktrees/agent-* (short-lived agent isolation worktrees)
AGENT_CLEANED=0
for path in .claude/worktrees/agent-*; do
  if [[ ! -d $path ]]; then continue; fi
  # unlock if locked
  git worktree unlock "$path" 2>/dev/null
  # remove (force, since these are ephemeral agent dirs)
  if git worktree remove --force "$path" 2>/dev/null; then
    AGENT_CLEANED=$((AGENT_CLEANED + 1))
  else
    echo "  agent worktree could not be removed: $path"
  fi
done

# Also delete their branches (force, since these are throwaway)
for br in $(git branch --format='%(refname:short)' | grep '^worktree-agent-'); do
  git branch -D "$br" 2>/dev/null && echo "  deleted ephemeral branch: $br"
done

echo
echo "Pass 3 done: agent-worktrees-cleaned=$AGENT_CLEANED"
echo

# Final prune of bookkeeping
git worktree prune

echo
echo "Final summary:"
echo "  worktrees removed (merged):  $REMOVED"
echo "  branches safely deleted:     $BRANCHES_DELETED"
echo "  agent worktrees cleaned:     $AGENT_CLEANED"
echo
git worktree list | wc -l | awk '{print "  worktrees remaining:         "$1}'
git branch --no-merged main | wc -l | awk '{print "  unmerged branches remaining: "$1}'
