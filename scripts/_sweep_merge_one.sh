#!/bin/bash
# Merge one branch with the full f-batch conflict-resolution pipeline.
# Usage: bash scripts/_sweep_merge_one.sh <branch>
#
# Pipeline:
#   1. git merge --no-ff -m "..." <branch>
#   2. For each known conflict file, apply the right strategy:
#       hooks.csv, CHANGELOG.md, build.bat, UNCERTAINTIES.md   -> resolve_sweep_conflicts.py
#       diff_template.js, run_diff.py                          -> take HEAD
#       hooks_registry.py                                      -> merge_hooks_registry.py
#       Audio/AudioRws.cpp, Audio/AudioDSound.cpp, etc.        -> take HEAD + append branch body
#   3. Stage and commit.
# Halts (exit non-zero) on:
#   - any conflict in a file we don't know how to handle
#   - Python syntax error in hooks_registry.py after merge
#   - any unresolved conflict marker still present after pipeline

set -e

branch="$1"
if [[ -z "$branch" ]]; then
  echo "usage: $0 <branch>"
  exit 2
fi

mkdir -p .tmp_sweep

echo "=== merging $branch ==="
if git merge --no-ff "$branch" -m "frida-sweep: merge $branch" >.tmp_sweep/merge.out 2>&1; then
  tail -3 .tmp_sweep/merge.out
  echo "OK clean"
  exit 0
fi
if grep -q "Already up to date" .tmp_sweep/merge.out; then
  echo "OK already up to date"
  exit 0
fi
if ! grep -q "CONFLICT" .tmp_sweep/merge.out; then
  echo "!!! merge failed without CONFLICT marker"
  cat .tmp_sweep/merge.out
  exit 3
fi

# Reject .bak/.orig adds
bak_files=$(git diff --name-only --diff-filter=A | grep -E "\.bak$|\.orig$" || true)
if [[ -n "$bak_files" ]]; then
  echo "    pruning .bak/.orig: $bak_files"
  for f in $bak_files; do git rm -f "$f" >/dev/null 2>&1 || rm -f "$f"; done
fi

# Classify conflicted files
conflicted=$(git diff --name-only --diff-filter=U)
echo "    conflicted: $conflicted"

resolver_files=""
cpp_unions=""
unknown_files=""
for f in $conflicted; do
  case "$f" in
    hooks.csv|UNCERTAINTIES.md|DEFERRED.md|"mashedmod/build.bat"|"re/analysis/CHANGELOG.md")
      resolver_files="$resolver_files $f"
      ;;
    "re/frida/diff_template.js"|"re/frida/run_diff.py")
      git checkout --ours "$f"
      git add "$f"
      ;;
    "re/frida/hooks_registry.py")
      git show :2:"$f" > .tmp_sweep/head_hr.py
      git show :3:"$f" > .tmp_sweep/branch_hr.py
      py -3.12 scripts/merge_hooks_registry.py .tmp_sweep/head_hr.py .tmp_sweep/branch_hr.py "$f"
      git add "$f"
      ;;
    mashedmod/src/*.cpp)
      cpp_unions="$cpp_unions $f"
      ;;
    *)
      unknown_files="$unknown_files $f"
      ;;
  esac
done

if [[ -n "$unknown_files" ]]; then
  echo "!!! unknown conflict file(s):$unknown_files"
  exit 4
fi

# Apply resolver to text-pattern files
if [[ -n "$resolver_files" ]]; then
  py -3.12 scripts/resolve_sweep_conflicts.py $resolver_files
  git add $resolver_files
fi

# Union .cpp files: take HEAD, append branch body (functions + RH_ScopedInstall)
for f in $cpp_unions; do
  base=$(basename "$f")
  git checkout --ours "$f"
  git show :3:"$f" > .tmp_sweep/branch_cpp.cpp
  py -3.12 << EOF
import re
with open('.tmp_sweep/branch_cpp.cpp', 'r', encoding='utf-8', newline='') as ff:
    branch = ff.read()
m = re.search(r"^// ─{10,}", branch, re.MULTILINE)
if not m:
    m = re.search(r"^// 0x[0-9a-f]{8}", branch, re.MULTILINE)
body = branch[m.start():] if m else branch
with open('.tmp_sweep/branch_body.cpp', 'w', encoding='utf-8', newline='') as ff:
    ff.write(body)
print(f"$base body: {len(body)} chars")
EOF
  cat .tmp_sweep/branch_body.cpp >> "$f"
  echo "" >> "$f"
  git add "$f"
done

# Python sanity check
if ! py -3.12 -c "import ast; ast.parse(open('re/frida/hooks_registry.py').read())"; then
  echo "!!! hooks_registry.py syntax error after merge"
  exit 5
fi

# Verify no residual markers
if git diff --staged --check 2>&1 | grep -q "conflict marker"; then
  echo "!!! residual conflict markers"
  git diff --staged --check
  exit 6
fi

git commit --no-edit >.tmp_sweep/commit.out 2>&1
tail -1 .tmp_sweep/commit.out
echo "OK merged $branch"
