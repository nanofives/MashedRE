"""Mechanical conflict resolver for frida-sweep merges.

Known patterns:
  mashedmod/build.bat            -- union of new .cpp lines, one `/link /DLL`
  re/frida/hooks_registry.py     -- union of dict entries, one closing `}`
  re/analysis/CHANGELOG.md       -- union of prepended lines
  hooks.csv                      -- per-RVA: keep highest-confidence row
                                    (C0<C1<C2<C3<C4; tie -> impl-row wins;
                                    tie2 -> branch side wins as newer)
  UNCERTAINTIES.md               -- per-U-ID: keep RESOLVED/C3-pending row over
                                    bare-open row; otherwise prefer branch
                                    side (newer)

Invoke after `git merge` reports CONFLICT.  Exits non-zero if any file has
a conflict marker shape this script does not recognize.
"""

import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONF_RE = re.compile(
    r"^<<<<<<< (?P<head_label>[^\r\n]*)\r?\n"
    r"(?P<head>.*?)"
    r"^=======\r?\n"
    r"(?P<incoming>.*?)"
    r"^>>>>>>> (?P<incoming_label>[^\r\n]*)\r?\n",
    re.MULTILINE | re.DOTALL,
)

CONF_ORDER = {f"C{i}": i for i in range(5)}


def parse_csv_row(line):
    rva = line.split(",", 1)[0].strip().lstrip("0x").lower()
    conf = "C0"
    # CSV cols: rva,name,subsystem,confidence,status,file,scenario,frida_diff,notes
    # Confidence is column index 3.
    parts = line.split(",")
    if len(parts) >= 4:
        c = parts[3].strip().upper()
        if c in CONF_ORDER:
            conf = c
    impl_file = parts[5].strip() if len(parts) >= 6 else ""
    return rva, conf, bool(impl_file)


def resolve_hooks_csv(head, incoming):
    """Return merged content.  Each side is a list of CSV row lines."""
    head_rows = [r for r in head.splitlines(keepends=False) if r.strip()]
    inc_rows = [r for r in incoming.splitlines(keepends=False) if r.strip()]
    # Walk in order of the union of RVAs as they appear in HEAD then incoming.
    seen = {}
    order = []
    for r in head_rows:
        rva, conf, has_impl = parse_csv_row(r)
        key = rva
        if key in seen:
            continue
        seen[key] = (r, conf, has_impl, "head")
        order.append(key)
    for r in inc_rows:
        rva, conf, has_impl = parse_csv_row(r)
        key = rva
        existing = seen.get(key)
        if existing is None:
            seen[key] = (r, conf, has_impl, "incoming")
            order.append(key)
            continue
        prev_r, prev_conf, prev_impl, prev_side = existing
        # Higher confidence wins.
        if CONF_ORDER[conf] > CONF_ORDER[prev_conf]:
            seen[key] = (r, conf, has_impl, "incoming")
        elif CONF_ORDER[conf] == CONF_ORDER[prev_conf]:
            # Same confidence -- prefer row with impl file path; else newer side.
            if has_impl and not prev_impl:
                seen[key] = (r, conf, has_impl, "incoming")
            elif prev_impl and not has_impl:
                pass  # keep head
            else:
                # Both have impl or both don't.  Prefer incoming (newer).
                seen[key] = (r, conf, has_impl, "incoming")
    return "\n".join(seen[k][0] for k in order) + "\n"


U_ID_RE = re.compile(r"^\|\s*(U-\d+)\s*\|")


def parse_u_row(line):
    m = U_ID_RE.match(line)
    if not m:
        return None
    uid = m.group(1)
    # Heuristic for "resolved": cells contain RESOLVED, C3, C3-pending, or `resolved 2026-`.
    resolved = bool(
        re.search(r"\bRESOLVED\b|\bC3-pending\b|\bC3\b|\bresolved \d{4}-", line)
    )
    return uid, resolved


def resolve_uncertainties_md(head, incoming):
    head_rows = head.splitlines(keepends=False)
    inc_rows = incoming.splitlines(keepends=False)
    seen = {}
    order = []
    side_keys = []
    for r in head_rows:
        parsed = parse_u_row(r)
        if parsed is None:
            # Non-row line (e.g. header, blank).  Preserve as-is in order.
            order.append(("nonrow", r))
            continue
        uid, resolved = parsed
        if uid in seen:
            continue
        seen[uid] = (r, resolved, "head")
        order.append(("uid", uid))
        side_keys.append(uid)
    for r in inc_rows:
        parsed = parse_u_row(r)
        if parsed is None:
            # If non-row from incoming, only append if not already in HEAD's structure.
            continue
        uid, resolved = parsed
        existing = seen.get(uid)
        if existing is None:
            seen[uid] = (r, resolved, "incoming")
            order.append(("uid", uid))
            continue
        prev_r, prev_resolved, _ = existing
        if resolved and not prev_resolved:
            seen[uid] = (r, resolved, "incoming")
        elif prev_resolved and not resolved:
            pass
        else:
            seen[uid] = (r, resolved, "incoming")  # prefer newer
    out = []
    for kind, val in order:
        if kind == "nonrow":
            out.append(val)
        else:
            out.append(seen[val][0])
    return "\n".join(out) + "\n"


def resolve_build_bat(head, incoming):
    # Each side: a few CPP lines, possibly ending with `    /link /DLL`.
    # Union of CPP lines; one `/link /DLL` (handled by trailing context, not
    # inside the conflict block).
    head_lines = [l for l in head.splitlines() if l.strip()]
    inc_lines = [l for l in incoming.splitlines() if l.strip()]
    # Filter `/link /DLL` from both halves if present; it lives after the
    # conflict block in our build.bat shape.
    head_lines = [l for l in head_lines if "/link /DLL" not in l]
    inc_lines = [l for l in inc_lines if "/link /DLL" not in l]
    # De-dup while preserving order.
    seen = set()
    out = []
    for l in head_lines + inc_lines:
        if l not in seen:
            seen.add(l)
            out.append(l)
    return "\n".join(out) + "\n"


def resolve_changelog_md(head, incoming):
    head_lines = head.splitlines()
    inc_lines = incoming.splitlines()
    seen = set()
    out = []
    for l in head_lines + inc_lines:
        if l not in seen:
            seen.add(l)
            out.append(l)
    return "\n".join(out) + "\n"


def resolve_hooks_registry_py(head, incoming):
    # Both halves contain dict entries.  We just concatenate, but if HEAD
    # ends without a trailing `},` (because the marker swallowed it) we add one.
    head_t = head.rstrip()
    inc_t = incoming.rstrip()
    # Detect: HEAD's last non-blank line ends with `path2_tests' line and no `},`
    if not head_t.endswith("},") and "path2_tests" in head_t.splitlines()[-1]:
        head_t += "\n    },"
    return head_t + "\n\n" + inc_t + "\n"


def resolve_take_head(head, incoming):
    """HEAD wins verbatim — the caller has pre-consolidated this file and
    the incoming branch only has a stale subset.  Use for diff_template.js
    and run_diff.py.  Note: this still preserves any additions the incoming
    branch made that don't conflict with HEAD's edits; only conflict blocks
    resolve to HEAD."""
    return head


RESOLVERS = {
    "hooks.csv": resolve_hooks_csv,
    "UNCERTAINTIES.md": resolve_uncertainties_md,
    "mashedmod/build.bat": resolve_build_bat,
    "re/analysis/CHANGELOG.md": resolve_changelog_md,
    "re/frida/hooks_registry.py": resolve_hooks_registry_py,
    "re/frida/diff_template.js": resolve_take_head,
    "re/frida/run_diff.py": resolve_take_head,
    "DEFERRED.md": resolve_changelog_md,
}


def resolve_file(rel_path):
    abs_path = os.path.join(REPO, rel_path.replace("/", os.sep))
    with open(abs_path, "r", encoding="utf-8", newline="") as f:
        text = f.read()
    resolver = RESOLVERS.get(rel_path)
    if resolver is None:
        return False, f"no resolver for {rel_path}"

    def sub(m):
        return resolver(m.group("head"), m.group("incoming"))

    new_text, count = CONF_RE.subn(sub, text)
    if count == 0:
        return True, "no conflict markers found (already clean?)"
    # Sanity check: no markers should remain.
    if re.search(r"^<<<<<<< |^=======$|^>>>>>>> ", new_text, re.MULTILINE):
        return False, "residual markers after substitution"
    with open(abs_path, "w", encoding="utf-8", newline="") as f:
        f.write(new_text)
    return True, f"resolved {count} conflict block(s)"


def main():
    rel_paths = sys.argv[1:]
    if not rel_paths:
        sys.stderr.write("usage: resolve_sweep_conflicts.py <rel_path> [...]\n")
        sys.exit(2)
    failed = []
    for rp in rel_paths:
        ok, msg = resolve_file(rp)
        print(f"{rp}: {msg}")
        if not ok:
            failed.append(rp)
    if failed:
        sys.exit(1)
    sys.exit(0)


if __name__ == "__main__":
    main()
