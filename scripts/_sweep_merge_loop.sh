#!/bin/bash
# One-shot merge loop for frida-sweep-20260516-0008.
# Reads branch names from stdin (one per line), merges each, auto-resolves
# the 5 known conflict patterns via resolve_sweep_conflicts.py, halts on
# unknown conflicts.

set -e

KNOWN_PATTERNS=(
  "hooks.csv"
  "mashedmod/build.bat"
  "re/analysis/CHANGELOG.md"
  "re/frida/hooks_registry.py"
  "re/frida/diff_template.js"
  "re/frida/run_diff.py"
  "UNCERTAINTIES.md"
)

is_known() {
  local f="$1"
  for p in "${KNOWN_PATTERNS[@]}"; do
    [[ "$f" == "$p" ]] && return 0
  done
  return 1
}

while IFS= read -r branch; do
  [[ -z "$branch" ]] && continue
  echo ""
  echo "=== merging $branch ==="
  if git merge --no-ff "$branch" -m "frida-sweep: merge $branch" >/tmp/merge.out 2>&1; then
    tail -3 /tmp/merge.out
    echo "    OK (clean)"
    continue
  fi
  if grep -q "Already up to date" /tmp/merge.out; then
    echo "    OK (already up to date)"
    continue
  fi
  # Reject any *.bak files the branch tried to add — they're session
  # backups, not real artifacts.
  bak_files=$(git diff --name-only --diff-filter=A | grep -E "\.bak$|\.orig$" || true)
  if [[ -n "$bak_files" ]]; then
    echo "    pruning .bak/.orig files added by branch: $bak_files"
    for f in $bak_files; do
      git rm -f "$f" >/dev/null 2>&1 || rm -f "$f"
    done
  fi
  if ! grep -q "CONFLICT" /tmp/merge.out; then
    echo "!!! merge failed without CONFLICT marker — halting"
    cat /tmp/merge.out
    exit 2
  fi
  conflicted=$(git diff --name-only --diff-filter=U)
  unknown=""
  for f in $conflicted; do
    if ! is_known "$f"; then
      unknown="$unknown $f"
    fi
  done
  if [[ -n "$unknown" ]]; then
    echo "!!! unknown conflict file(s):$unknown — halting"
    exit 3
  fi
  echo "    conflicts: $conflicted"
  if ! py -3.12 scripts/resolve_sweep_conflicts.py $conflicted; then
    echo "!!! resolver failed — halting"
    exit 4
  fi
  # Verify no residual markers.
  if grep -q "^<<<<<<<\|^>>>>>>>" $conflicted; then
    echo "!!! residual conflict markers — halting"
    exit 5
  fi
  git add $conflicted
  if ! git commit --no-edit >/tmp/commit.out 2>&1; then
    echo "!!! commit failed — halting"
    cat /tmp/commit.out
    exit 6
  fi
  echo "    RESOLVED + committed"
done

echo ""
echo "=== loop complete ==="
