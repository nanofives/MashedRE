"""
Resolve the 4 known mechanical conflict patterns produced by frida-sweep merges.

Usage:
    py -3.12 scripts/sweep_resolve_conflicts.py

Behavior:
    Scans git's conflicted files (`git diff --name-only --diff-filter=U`).
    For each, if it matches a known pattern (build.bat, hooks_registry.py,
    re/analysis/CHANGELOG.md, re/DEFERRED.md, DEFERRED.md), apply the
    appropriate mechanical resolution and `git add` it.
    For any conflicted file NOT matching a known pattern, print its path
    and exit non-zero so the caller halts the sweep.

DEFERRED.md collision policy:
    Take UNION of D-rows. For incoming D-IDs that collide with HEAD's,
    renumber the incoming side using the next available D-ID (after the
    global max across both sides). Print renumbering map for audit.
"""

import re
import subprocess
import sys
from pathlib import Path

REPO = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")

CONFLICT_RE = re.compile(
    r"^<<<<<<<.*?$\n(.*?)^=======.*?$\n(.*?)^>>>>>>>.*?$\n",
    re.DOTALL | re.MULTILINE,
)

D_ROW_RE = re.compile(r"\|\s*D-(\d+)\s*\|")
S_ROW_RE = re.compile(r"\|\s*S-(\d+)\s*\|")
U_ROW_RE = re.compile(r"\|\s*U-(\d+)\s*\|")


def run(cmd, **kw):
    return subprocess.run(cmd, cwd=REPO, capture_output=True, text=True, **kw)


def conflicted_files():
    r = run(["git", "diff", "--name-only", "--diff-filter=U"])
    return [Path(p) for p in r.stdout.strip().splitlines() if p]


def read_text(path):
    return (REPO / path).read_text(encoding="utf-8")


def write_text(path, content):
    (REPO / path).write_text(content, encoding="utf-8", newline="\n")


def all_d_ids_in_text(text):
    return set(int(m.group(1)) for m in D_ROW_RE.finditer(text))


def _resolve_id_tracker(path, id_letter, id_regex):
    """Generic union+renumber resolver for D-/S-/U- ID-keyed trackers."""
    text = read_text(path)
    non_conflict = CONFLICT_RE.sub("", text)
    existing_ids = set(int(m.group(1)) for m in id_regex.finditer(non_conflict))

    renumbering = {}

    def resolve_one(match):
        head_block = match.group(1)
        incoming_block = match.group(2)

        head_ids = set(int(m.group(1)) for m in id_regex.finditer(head_block))
        global_max = max(
            existing_ids | head_ids | set(int(m.group(1)) for m in id_regex.finditer(incoming_block)),
            default=0,
        )

        def renumber_incoming(line_match):
            nonlocal global_max
            old = int(line_match.group(1))
            if old in head_ids or old in existing_ids:
                global_max += 1
                renumbering[old] = global_max
                return f"| {id_letter}-{global_max} |"
            return line_match.group(0)

        new_incoming = id_regex.sub(renumber_incoming, incoming_block)
        existing_ids.update(head_ids)
        existing_ids.update(int(m.group(1)) for m in id_regex.finditer(new_incoming))
        return head_block + new_incoming

    resolved = CONFLICT_RE.sub(resolve_one, text)
    write_text(path, resolved)

    if renumbering:
        print(f"  [{path}] renumbered {id_letter}-rows: " + ", ".join(
            f"{id_letter}-{k}->{id_letter}-{v}" for k, v in sorted(renumbering.items())
        ))
    return renumbering


def resolve_deferred(path):
    return _resolve_id_tracker(path, "D", D_ROW_RE)


def resolve_stubs(path):
    return _resolve_id_tracker(path, "S", S_ROW_RE)


def resolve_uncertainties(path):
    return _resolve_id_tracker(path, "U", U_ROW_RE)


def resolve_changelog(path):
    """Union both sides; sort by timestamp (newest first)."""
    text = read_text(path)

    def resolve_one(match):
        head_lines = [l for l in match.group(1).splitlines() if l.strip()]
        incoming_lines = [l for l in match.group(2).splitlines() if l.strip()]
        merged = head_lines + incoming_lines
        # Sort: timestamp desc, then session-id asc.
        def sort_key(line):
            ts_match = re.match(r"^(\d{4}-\d{2}-\d{2})\s+(\S+)", line)
            if ts_match:
                return (ts_match.group(1), -ord(ts_match.group(2)[0]) if ts_match.group(2) else 0, line)
            return ("", 0, line)
        merged.sort(key=lambda l: sort_key(l), reverse=True)
        return "\n".join(merged) + "\n"

    resolved = CONFLICT_RE.sub(resolve_one, text)
    write_text(path, resolved)
    return {}


def resolve_build_bat(path):
    """Union of additions; both before the marker line."""
    text = read_text(path)

    def resolve_one(match):
        head = match.group(1)
        incoming = match.group(2)
        # Collect lines from both, dedupe, keep order
        seen = set()
        result = []
        for block in (head, incoming):
            for line in block.splitlines():
                stripped = line.strip()
                if not stripped:
                    continue
                if stripped in seen:
                    continue
                seen.add(stripped)
                result.append(line)
        return "\n".join(result) + "\n"

    resolved = CONFLICT_RE.sub(resolve_one, text)
    write_text(path, resolved)
    return {}


def resolve_hooks_registry(path):
    """Union dict entries; preserve closing brace."""
    text = read_text(path)

    def resolve_one(match):
        head = match.group(1)
        incoming = match.group(2)
        # Strip trailing closing-brace lines from one side to avoid duplicates.
        # Both sides typically end with `}\n` for the registry dict.
        head_stripped = re.sub(r"\n\}\s*\n*$", "\n", head)
        incoming_stripped = re.sub(r"\n\}\s*\n*$", "\n", incoming)
        # If both blocks ended with `}`, retain only one at the end.
        return head_stripped + incoming_stripped + "}\n"

    resolved = CONFLICT_RE.sub(resolve_one, text)
    write_text(path, resolved)
    return {}


def resolve_hooks_csv(path):
    """Union of row additions, keyed by RVA (first column) to detect collisions."""
    text = read_text(path)

    def resolve_one(match):
        head_lines = match.group(1).splitlines()
        incoming_lines = match.group(2).splitlines()
        seen_rvas = {}
        result = []
        for block_name, lines in (("head", head_lines), ("incoming", incoming_lines)):
            for line in lines:
                if not line.strip():
                    continue
                # First column is RVA
                rva = line.split(",", 1)[0].strip()
                if rva in seen_rvas:
                    # Real conflict on same RVA: keep first occurrence, skip duplicate.
                    print(f"  [{path}] WARNING: duplicate RVA {rva} in conflict; kept {seen_rvas[rva]}, skipped {block_name}")
                    continue
                seen_rvas[rva] = block_name
                result.append(line)
        return "\n".join(result) + "\n"

    resolved = CONFLICT_RE.sub(resolve_one, text)
    write_text(path, resolved)
    return {}


def resolve_promotion_queue(path):
    """Prefer HEAD's curated rows. The orchestrator's PROMOTION_QUEUE.md edits
    are authoritative; if a session branch also touched PROMOTION_QUEUE.md from
    its worktree, discard that side."""
    text = read_text(path)
    resolved = CONFLICT_RE.sub(lambda m: m.group(1), text)
    write_text(path, resolved)
    return {}


HANDLERS = {
    "mashedmod/build.bat": resolve_build_bat,
    "re/frida/hooks_registry.py": resolve_hooks_registry,
    "re/analysis/CHANGELOG.md": resolve_changelog,
    "re/DEFERRED.md": resolve_deferred,
    "DEFERRED.md": resolve_deferred,
    "STUBS.md": resolve_stubs,
    "re/STUBS.md": resolve_stubs,
    "UNCERTAINTIES.md": resolve_uncertainties,
    "re/UNCERTAINTIES.md": resolve_uncertainties,
    "hooks.csv": resolve_hooks_csv,
    "re/PROMOTION_QUEUE.md": resolve_promotion_queue,
    "re/SCRIBE_QUEUE.md": resolve_changelog,
}


def main():
    files = conflicted_files()
    if not files:
        print("No conflicts.")
        return 0

    unknown = [f for f in files if str(f).replace("\\", "/") not in HANDLERS]
    if unknown:
        print(f"HALT: unknown conflict pattern in:", file=sys.stderr)
        for f in unknown:
            print(f"  {f}", file=sys.stderr)
        return 2

    all_renumberings = {}
    for f in files:
        key = str(f).replace("\\", "/")
        print(f"Resolving: {key}")
        renumb = HANDLERS[key](f)
        if renumb:
            all_renumberings.update(renumb)
        # git add
        r = run(["git", "add", str(f)])
        if r.returncode != 0:
            print(f"git add failed: {r.stderr}", file=sys.stderr)
            return 3

    if all_renumberings:
        print("\nD-row renumberings (summary):")
        for old, new in sorted(all_renumberings.items()):
            print(f"  D-{old} -> D-{new}")
        # Persist to a sidecar file for the sweep release commit.
        sidecar = REPO / "log" / "frida_sweep_d_renumber.txt"
        sidecar.parent.mkdir(exist_ok=True)
        with sidecar.open("a", encoding="utf-8") as fh:
            for old, new in sorted(all_renumberings.items()):
                fh.write(f"D-{old} -> D-{new}\n")

    print(f"\nResolved {len(files)} conflict(s).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
