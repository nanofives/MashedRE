#!/usr/bin/env python3
"""Generate re/frida/ARG_TYPES.md — a compact index of every arg_type handler in
diff_template.js plus its usage count in hooks_registry.py.

Why: diff_template.js is ~230 KB and was Read 361 times in June 2026 just to
answer "does an arg_type exist for this signature?". Sessions should Read the
generated index instead, and open diff_template.js only to author a new handler.

Usage:  py -3.12 scripts\\gen_arg_types_index.py
Rerun after adding/renaming any handler in diff_template.js.
"""
import re
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TEMPLATE = ROOT / "re" / "frida" / "diff_template.js"
REGISTRY = ROOT / "re" / "frida" / "hooks_registry.py"
OUT = ROOT / "re" / "frida" / "ARG_TYPES.md"

HANDLER_RE = re.compile(r"CONFIG\.arg_type\s*===?\s*['\"]([a-zA-Z0-9_]+)['\"]")
REGISTRY_RE = re.compile(r"['\"]arg_type['\"]\s*:\s*['\"]([a-zA-Z0-9_]+)['\"]")


def main() -> int:
    lines = TEMPLATE.read_text(encoding="utf-8", errors="replace").splitlines()

    # handler name -> (first dispatch line, short comment scraped from the lines above)
    handlers: dict[str, tuple[int, str]] = {}
    for i, line in enumerate(lines, 1):
        m = HANDLER_RE.search(line)
        if not m or m.group(1) in handlers:
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
        handlers[m.group(1)] = (i, note)

    usage = Counter(
        REGISTRY_RE.findall(REGISTRY.read_text(encoding="utf-8", errors="replace"))
    )

    rows = sorted(handlers.items(), key=lambda kv: (-usage.get(kv[0], 0), kv[0]))
    orphans = sorted(set(usage) - set(handlers))

    out = [
        "# arg_type index (GENERATED — do not hand-edit)",
        "",
        f"Regenerate: `py -3.12 scripts\\gen_arg_types_index.py`",
        f"Handlers: {len(handlers)} in `re/frida/diff_template.js` | "
        f"registry uses: {sum(usage.values())} across {len(usage)} distinct arg_types.",
        "",
        "Answer \"does an arg_type exist for this signature?\" HERE. Open",
        "diff_template.js only to author a NEW handler (its header comments,",
        "lines 1-150, document test-vector shapes per family), then rerun this script.",
        "A registry entry naming an arg_type with no handler here is FATAL at",
        "run_diff pre-flight (see `worker-invented arg_types` feedback memory).",
        "",
        "| arg_type | diff_template.js line | registry uses | note |",
        "|---|---|---|---|",
    ]
    for name, (line_no, note) in rows:
        out.append(f"| `{name}` | {line_no} | {usage.get(name, 0)} | {note or ''} |")
    if orphans:
        out += [
            "",
            "## Registry arg_types with NO dispatch handler in diff_template.js",
            "",
            "run_diff.py pre-flight refuses these (FATAL) unless the name happens to",
            "appear elsewhere in the JS. Most are historical worker entries that",
            "predate the 2026-06-12 pre-flight and were never run; treat any hook",
            "using one as NOT diffable until a handler is authored.",
            "",
        ]
        out += [f"- `{name}` ({usage[name]} uses)" for name in orphans]
    out.append("")

    OUT.write_text("\n".join(out), encoding="utf-8", newline="\n")
    print(f"wrote {OUT} ({len(handlers)} handlers, {len(orphans)} orphans)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
