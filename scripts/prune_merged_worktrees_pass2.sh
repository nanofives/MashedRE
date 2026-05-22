#!/usr/bin/env bash
# Second-pass prune: force-delete branches that `git branch --merged main` reports
# as merged but `git branch -d` refused (squash-merge / rebase-merge / branch-tip
# heuristic disagreement). Their changes ARE in main per `--merged main`, so
# `-D` is safe here.
#
# Also removes worktrees that still exist for these stubborn merged branches.

set -u

cd "$(git rev-parse --show-toplevel)"

REMOVED_WT=0
SKIPPED_WT=0
DELETED_BR=0

declare -a TO_REMOVE_PATHS=()
declare -A WT_BRANCH

# Build path->branch map
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

# Merged branches (excluding main)
mapfile -t MERGED < <(git branch --merged main --format='%(refname:short)' | grep -v '^main$')

# Identify worktrees on merged branches
for path in "${!WT_BRANCH[@]}"; do
  br="${WT_BRANCH[$path]}"
  if [[ $br == main ]]; then continue; fi
  for mb in "${MERGED[@]}"; do
    if [[ $br == "$mb" ]]; then
      TO_REMOVE_PATHS+=("$path")
      break
    fi
  done
done

echo "Worktrees still on merged branches: ${#TO_REMOVE_PATHS[@]}"

# Pass A: force-remove the worktrees
for path in "${TO_REMOVE_PATHS[@]}"; do
  if [[ ! -d $path ]]; then
    git worktree prune --quiet 2>/dev/null
    continue
  fi
  if git worktree remove --force "$path" 2>/dev/null; then
    REMOVED_WT=$((REMOVED_WT + 1))
  else
    SKIPPED_WT=$((SKIPPED_WT + 1))
    echo "  could not remove: $path"
  fi
done

# Pass B: force-delete the merged-but-stubborn branches
for br in "${MERGED[@]}"; do
  if git branch -D "$br" 2>/dev/null; then
    DELETED_BR=$((DELETED_BR + 1))
  fi
done

git worktree prune

echo
echo "Pass-2 summary:"
echo "  worktrees force-removed:        $REMOVED_WT"
echo "  worktrees still wedged:         $SKIPPED_WT"
echo "  branches force-deleted:         $DELETED_BR"
echo
git worktree list | wc -l | awk '{print "  worktrees remaining:           "$1}'
git branch --merged main | grep -v 'main$' | wc -l | awk '{print "  merged branches remaining:    "$1}'
git branch --no-merged main | wc -l | awk '{print "  unmerged branches remaining:  "$1}'
