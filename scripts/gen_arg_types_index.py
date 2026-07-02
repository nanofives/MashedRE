#!/usr/bin/env python3
"""Generate re/frida/ARG_TYPES.md — a compact index of every arg_type handler in
diff_template.js plus its usage count in hooks_registry.py.

Why: diff_template.js is ~230 KB and was Read 361 times in June 2026 just to
answer "does an arg_type exist for this signature?". Sessions should Read the
generated index instead, and open diff_template.js only to author a new handler.

Also indexed (argtype-orphans triage 2026-07, re/analysis/argtype_orphans_triage_2026-07.md):
  - arg_types dispatched ONLY by early_window_leaf_diff.py (pure-leaf pre-crash
    lane) — these are NOT run_diff-able but are NOT orphans.
  - recognized marker arg_types (deliberately non-diffable; documented per-entry
    in hooks_registry.py).
Only names in none of those three sets are true orphans.

Usage:  py -3.12 scripts\\gen_arg_types_index.py
Rerun after adding/renaming any handler in diff_template.js or
early_window_leaf_diff.py.
"""
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TEMPLATE = ROOT / "re" / "frida" / "diff_template.js"
EARLY_WINDOW = ROOT / "re" / "frida" / "early_window_leaf_diff.py"
REGISTRY = ROOT / "re" / "frida" / "hooks_registry.py"
OUT = ROOT / "re" / "frida" / "ARG_TYPES.md"

HANDLER_RE = re.compile(r"CONFIG\.arg_type\s*===?\s*['\"]([a-zA-Z0-9_]+)['\"]")
REGISTRY_RE = re.compile(r"['\"]arg_type['\"]\s*:\s*['\"]([a-zA-Z0-9_]+)['\"]")
EW_SET_RE = re.compile(r"PURE_LEAF_ARGTYPES\s*=\s*\{(.*?)\n\}", re.S)
EW_NAME_RE = re.compile(r"'([a-zA-Z0-9_]+)'")

# Marker arg_types: deliberate "cannot be synthetically diffed" sentinels.
# Their evidence lane is documented per-entry in hooks_registry.py comments.
SENTINELS = {
    "harness_limited": (
        "Calling the function synthetically is unsafe or impossible "
        "(process-terminating CRT glue, SEH/stack-probe prologs, live-OS-handle "
        "piz compat shims). Evidence lane documented per-entry."
    ),
    "register_abi_record": (
        "Register-ABI hot-path physics record functions (EAX=0xd04 vehicle "
        "record, >1000 calls/s). C4 lane = installed-hook canonical-race "
        "telemetry (re/frida/phys_c4_telemetry.py + MASHED_PHYS_C4_SELFTEST), "
        "never run_diff."
    ),
}


def main() -> int:
    lines = TEMPLATE.read_text(encoding="utf-8", errors="replace").splitlines()

    # handler name -> (first dispatch line, short comment scraped from the lines above)
    # finditer, not search: shared dispatch lines like
    #   if (CONFIG.arg_type === 'transform_point' || CONFIG.arg_type === 'transform_vector')
    # define a handler for EVERY name on the line.
    handlers: dict[str, tuple[int, str]] = {}
    for i, line in enumerate(lines, 1):
        names = [m.group(1) for m in HANDLER_RE.finditer(line)]
        if not names:
            continue
        # scrape up to 3 consecutive // comment lines immediately above the dispatch
        note_parts: list[str] = []
        j = i - 2  # 0-based index of the line above
        while j >= 0 and len(note_parts) < 3:
            stripped = lines[j].strip()
            if stripped.startswith("//"):
                note_parts.insert(0, stripped.lstrip("/ ").strip())
                j -= 1
            else:
                break
        note = " ".join(note_parts)
        if len(note) > 160:
            note = note[:157] + "..."
        for name in names:
            if name not in handlers:
                handlers[name] = (i, note)

    # early-window pure-leaf lane: PURE_LEAF_ARGTYPES set membership is the
    # dispatch gate (the harness refuses any other arg_type).
    ew_src = EARLY_WINDOW.read_text(encoding="utf-8", errors="replace")
    m = EW_SET_RE.search(ew_src)
    ew_names: dict[str, str] = {}
    if m:
        for raw in m.group(1).splitlines():
            found = EW_NAME_RE.findall(raw.split("#", 1)[0])
            comment = raw.split("#", 1)[1].strip() if "#" in raw else ""
            for n in found:
                # attach the trailing comment only when it is unambiguous
                ew_names.setdefault(n, comment if len(found) == 1 else "")
    ew_only = {n: c for n, c in ew_names.items() if n not in handlers}

    usage = Counter(
        REGISTRY_RE.findall(REGISTRY.read_text(encoding="utf-8", errors="replace"))
    )

    rows = sorted(handlers.items(), key=lambda kv: (-usage.get(kv[0], 0), kv[0]))
    ew_rows = sorted(
        ((n, c) for n, c in ew_only.items()),
        key=lambda kv: (-usage.get(kv[0], 0), kv[0]),
    )
    orphans = sorted(set(usage) - set(handlers) - set(ew_only) - set(SENTINELS))

    out = [
        "# arg_type index (GENERATED — do not hand-edit)",
        "",
        f"Regenerate: `py -3.12 scripts\\gen_arg_types_index.py`",
        f"Handlers: {len(handlers)} in `re/frida/diff_template.js` "
        f"+ {len(ew_only)} early-window-only in `re/frida/early_window_leaf_diff.py` | "
        f"registry uses: {sum(usage.values())} across {len(usage)} distinct arg_types.",
        "",
        "Answer \"does an arg_type exist for this signature?\" HERE. Open",
        "diff_template.js only to author a NEW handler (its header comments,",
        "lines 1-150, document test-vector shapes per family), then rerun this script.",
        "A registry entry naming an arg_type with no handler in EITHER harness",
        "(and not a recognized marker) is FATAL at run_diff pre-flight (see",
        "`worker-invented arg_types` feedback memory).",
        "",
        "| arg_type | diff_template.js line | registry uses | note |",
        "|---|---|---|---|",
    ]
    for name, (line_no, note) in rows:
        out.append(f"| `{name}` | {line_no} | {usage.get(name, 0)} | {note or ''} |")

    if ew_rows:
        out += [
            "",
            "## Early-window-only arg_types (pure-leaf pre-crash lane)",
            "",
            "Dispatched by `re/frida/early_window_leaf_diff.py` (PURE_LEAF_ARGTYPES),",
            "NOT by diff_template.js — `run_diff.py` pre-flight refuses them; that is",
            "correct, their lane is `py -3.12 re\\frida\\early_window_leaf_diff.py <hook>`.",
            "Evidence tag in hooks.csv: `green-earlywindow-rN`.",
            "",
            "| arg_type | registry uses | note |",
            "|---|---|---|",
        ]
        for name, comment in ew_rows:
            out.append(f"| `{name}` | {usage.get(name, 0)} | {comment} |")

    used_sentinels = {n: d for n, d in SENTINELS.items() if usage.get(n)}
    if used_sentinels:
        out += [
            "",
            "## Marker arg_types (deliberately non-diffable)",
            "",
            "Not handlers and never will be: the arg_type field is used as a sentinel",
            "documenting WHY no synthetic diff exists. Evidence lanes are documented",
            "per-entry in hooks_registry.py comments.",
            "",
        ]
        for name, desc in sorted(used_sentinels.items()):
            out.append(f"- `{name}` ({usage[name]} uses) — {desc}")

    if orphans:
        out += [
            "",
            "## Registry arg_types with NO dispatch handler anywhere (true orphans)",
            "",
            "Not in diff_template.js, not in early_window_leaf_diff.py, not a",
            "recognized marker. FATAL at run_diff pre-flight; treat any hook using",
            "one as NOT diffable until the entry is fixed or a handler is authored.",
            "",
        ]
        out += [f"- `{name}` ({usage[name]} uses)" for name in orphans]
    out.append("")

    OUT.write_text("\n".join(out), encoding="utf-8", newline="\n")
    print(
        f"wrote {OUT} ({len(handlers)} JS handlers, {len(ew_only)} early-window-only, "
        f"{len(used_sentinels)} markers, {len(orphans)} true orphans)"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
