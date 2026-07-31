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

# `// MECHANISM: ...` opt-in marker, preferred over the positional 3-line scrape.
# Continuation lines are the plain `//` lines BELOW it, up to the dispatch.
MECHANISM_RE = re.compile(r"^MECHANISM\s*:\s*", re.I)
# How far above a dispatch to look for the marker. Handler comment blocks in
# diff_template.js run to a few dozen lines; the search stops at the first
# non-comment line anyway, so this is only a runaway guard.
MECHANISM_LOOKBACK = 60
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
        # PREFERRED: an explicit `// MECHANISM: ...` line in the comment block above
        # the dispatch, continued across following `//` lines until a blank comment
        # or the dispatch itself.
        #
        # WHY THIS EXISTS. The 3-line scrape below takes an arbitrary window, so it
        # regularly lands mid-sentence (`ptr_arg_int_get` began "is left Queued,
        # never falsely GREEN") or on a decorative separator (`float_scalar` showed
        # a run of box-drawing characters). Even when it reads cleanly it usually
        # describes the handler's ORIGINAL USE CASE rather than its MECHANISM, and a
        # screen asking "does a handler exist for this shape?" answers from these
        # blurbs. Six consecutive orchestrator runs returned NEEDS_NEW_HANDLER and
        # every one was wrong; `eax_ecx_insert` reads as "cross-link insert" so a
        # screen hunting for "store EAX into *ECX" never matched it, though that is
        # exactly its trampoline. See re/analysis/needs_new_handler_rescreen_20260731.md.
        note = ""
        block_start = max(0, i - 1 - MECHANISM_LOOKBACK)
        k = i - 2  # 0-based index of the line above the dispatch
        tail: list[str] = []   # continuation lines BELOW the MECHANISM: line
        found_mech = False
        while k >= block_start:
            stripped = lines[k].strip()
            if not stripped.startswith("//"):
                break          # left the comment block without finding a marker
            body = stripped.lstrip("/ ").rstrip()
            if MECHANISM_RE.match(body):
                tail.insert(0, MECHANISM_RE.sub("", body).strip())
                found_mech = True
                break
            tail.insert(0, body)
            k -= 1
        # Only a real marker counts. Without this flag, walking off the top of the
        # comment block would hand back the whole accumulated blob as if it were a
        # MECHANISM line — an unbounded version of the very scrape this replaces.
        if found_mech:
            note = " ".join(p for p in tail if p).strip()

        # FALLBACK: up to 3 consecutive // comment lines immediately above the dispatch.
        if not note:
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
        # A MECHANISM line is written to be read in full — it carries the call shape,
        # seed/observe behaviour and config keys, and its "applies more broadly than
        # the name suggests" clause lands at the END, which is exactly what a screen
        # needs and exactly what a 160-char cut would remove. Scraped fallback text
        # stays short: it is an arbitrary window, so extra length adds no signal.
        limit = 420 if found_mech else 160
        if len(note) > limit:
            note = note[: limit - 3] + "..."
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
        "**A `note` describes the handler's MECHANISM, not the function it was",
        "written for — but only where one has been authored.** Notes come from an",
        "explicit `// MECHANISM: ...` line above the dispatch when present, and",
        "otherwise fall back to scraping the 3 comment lines above it — an arbitrary",
        "window that can land mid-sentence or on a separator.",
        "",
        "**A scraped note is not evidence that no handler fits your shape.** Six",
        "consecutive orchestrator runs concluded NEEDS_NEW_HANDLER from these blurbs",
        "and every one was wrong: `eax_ecx_insert` reads as \"cross-link insert\", yet",
        "its trampoline is exactly `mov eax,bufA; mov ecx,bufC; jmp target`. Before",
        "writing a handler, READ THE IMPLEMENTATION, and prefer an additive defaulted",
        "config field over a new handler (precedent: `stub_at`, `null_args`,",
        "`this_reg:'stack'`, `key_off`). Writeup:",
        "`re/analysis/needs_new_handler_rescreen_20260731.md`.",
        "",
        "When you author or touch a handler, add a `// MECHANISM:` line giving (1) the",
        "call shape and how args are delivered, (2) what is seeded and observed, (3)",
        "the config keys, (4) anything making it apply more broadly than its name says.",
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
