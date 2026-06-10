#!/usr/bin/env python3
"""
c3_batch_j filter: applies the 4 FIXED filters from c3_batch_i orchestrator
report:

1. Inline [UNCERTAIN] regex: matches both bare `[UNCERTAIN]` and catalogued
   `[UNCERTAIN U-xxxx]` forms (use `\[UNCERTAIN[ \]]`).
2. Catalogued-U-ID exemption: if a U-ID is in UNCERTAINTIES.md, don't refuse.
3. Body-grep callee extraction scoped to ## Decompilation / ## Callees /
   ## Mechanical description sections only.
4. Skip `Resolves [UNCERTAIN U-xxx] from <prior_note>` prose patterns.

Plus arg_type compat filter and stale-impl drift filter.
"""

from __future__ import annotations

import csv
import os
import re
import sys
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
HOOKS = ROOT / "hooks.csv"
UNCERT = ROOT / "UNCERTAINTIES.md"
DIFF_TEMPLATE = ROOT / "re" / "frida" / "diff_template.js"

# Filter 1: corrected regex — matches `[UNCERTAIN]` AND `[UNCERTAIN U-xxxx]`
UNCERTAIN_RE = re.compile(r"\[UNCERTAIN[ \]]")

# Filter 4: explicitly skip these prose forms
RESOLVES_PROSE_RE = re.compile(
    r"Resolve[ds]?\s+\[UNCERTAIN", re.IGNORECASE
)

# Filter 3: callee-extraction is restricted to these section headings
ALLOWED_CALLEE_SECTIONS = {
    "## decompilation",
    "## callees",
    "## mechanical description",
    "## mechanical",
}

# RVA pattern in note bodies; matches `0x00ABCDEF` and `00abcdef`
RVA_RE = re.compile(r"\b(?:0x)?([0-9a-fA-F]{8})\b")

# subsystems we're targeting (frontend explicitly excluded)
TARGET_SUBSYSTEMS = {"audio", "save", "vehicle", "input", "boot"}


def load_hooks() -> dict[str, dict]:
    """{lowercase-rva-no-0x: {rva, name, subsystem, confidence, status, file}}"""
    rows: dict[str, dict] = {}
    with open(HOOKS, encoding="utf-8-sig") as f:
        reader = csv.reader(f)
        header = next(reader)
        for r in reader:
            if len(r) < 6:
                continue
            rva = r[0].strip().lower()
            rva = rva[2:] if rva.startswith("0x") else rva
            row = {
                "rva": rva,
                "name": r[1].strip(),
                "subsystem": r[2].strip(),
                "confidence": r[3].strip(),
                "status": r[4].strip(),
                "file": r[5].strip(),
            }
            # multiple rows possible per RVA — keep highest confidence
            existing = rows.get(rva)
            if existing is None:
                rows[rva] = row
            else:
                # prefer C3+ if present
                pri = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}
                if pri.get(row["confidence"], -1) > pri.get(
                    existing["confidence"], -1
                ):
                    rows[rva] = row
    return rows


def load_uncertainties() -> set[str]:
    """Set of catalogued U-IDs (e.g., 'U-2587') in UNCERTAINTIES.md"""
    catalogued = set()
    if not UNCERT.exists():
        return catalogued
    txt = UNCERT.read_text(encoding="utf-8", errors="replace")
    for m in re.finditer(r"\bU-\d{3,5}\b", txt):
        catalogued.add(m.group(0))
    return catalogued


def load_supported_arg_types() -> set[str]:
    """Set of arg_type strings recognised by re/frida/diff_template.js"""
    txt = DIFF_TEMPLATE.read_text(encoding="utf-8", errors="replace")
    return set(re.findall(r"arg_type === '([a-z_0-9]+)'", txt))


def read_note(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def section_iter(note_body: str):
    """Yield (section_heading_lowercased, section_text) tuples."""
    current_heading = "(preamble)"
    buf: list[str] = []
    for line in note_body.splitlines():
        if line.startswith("## "):
            yield current_heading, "\n".join(buf)
            current_heading = line.strip().lower()
            buf = []
        else:
            buf.append(line)
    yield current_heading, "\n".join(buf)


# ------ Filter 1+2+4: inline UNCERTAIN ---------------------------------------


def inline_uncertain_blocks(note_body: str, catalogued: set[str]) -> str | None:
    """
    Returns None if the note passes the filter; else a string describing why.
    """
    for heading, section in section_iter(note_body):
        # Filter 4: skip Resolves-prose lines BEFORE testing
        scrubbed_lines = []
        for line in section.splitlines():
            if RESOLVES_PROSE_RE.search(line):
                continue
            scrubbed_lines.append(line)
        scrubbed = "\n".join(scrubbed_lines)

        # Markers inside ## Uncertainties section are legitimate
        if heading.startswith("## uncertainties"):
            continue

        for m in UNCERTAIN_RE.finditer(scrubbed):
            # Extract surrounding context to get the U-id if any
            start = m.start()
            tail = scrubbed[start : start + 32]
            uid_match = re.search(r"\[UNCERTAIN\s+(U-\d{3,5})\]", tail)
            if uid_match:
                uid = uid_match.group(1)
                # Filter 2: catalogued exemption
                if uid in catalogued:
                    continue
                return f"inline [UNCERTAIN {uid}] not in UNCERTAINTIES.md (heading={heading})"
            else:
                # bare [UNCERTAIN]
                return f"inline bare [UNCERTAIN] (heading={heading})"
    return None


# ------ Filter 3: section-scoped callee extraction --------------------------


def extract_callees(note_body: str, self_rva: str) -> list[str]:
    """
    Return RVAs cited only within allowed-callee sections, AFTER scrubbing
    Resolves-prose lines (filter 4).
    """
    rvas: set[str] = set()
    self_rva_norm = self_rva.lower().lstrip("0x").zfill(8)
    for heading, section in section_iter(note_body):
        if heading not in ALLOWED_CALLEE_SECTIONS:
            continue
        # scrub Resolves-prose lines
        scrubbed_lines = []
        for line in section.splitlines():
            if RESOLVES_PROSE_RE.search(line):
                continue
            scrubbed_lines.append(line)
        scrubbed = "\n".join(scrubbed_lines)
        for m in RVA_RE.finditer(scrubbed):
            rva = m.group(1).lower()
            if rva != self_rva_norm:
                rvas.add(rva)
    return sorted(rvas)


def callee_gate(
    callees: list[str], hooks: dict[str, dict]
) -> tuple[bool, list[str]]:
    """All callees must be C2+ or unmapped-leaf (not in hooks.csv => assume leaf, OK).
    Returns (passes, list_of_blocking_callees).
    """
    blocking: list[str] = []
    pri = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}
    for c in callees:
        row = hooks.get(c)
        if row is None:
            # unmapped; only block if it's clearly an RVA cited as a callee
            # and it doesn't look like a data address (DAT_*) — but the section-
            # scoping already gives us better signal. Skip unmapped (assume
            # leaf or CRT). c3_batch_i's overly broad filter blocked these too;
            # the fix is "if not in hooks.csv, give the benefit of the doubt"
            continue
        conf = row["confidence"]
        if pri.get(conf, -1) < 2:
            blocking.append(f"{c}={conf}")
    return (not blocking), blocking


# ------ stale-impl drift filter ---------------------------------------------


def stale_impl_check(rva: str) -> str | None:
    """
    If an impl exists in mashedmod/src/ AND a GREEN diff CSV exists in log/,
    report as drift (skip from batch).
    """
    src = ROOT / "mashedmod" / "src" / "mashed_re"
    log = ROOT / "log"
    short = rva.lstrip("0x").lower().zfill(8)

    impl_hits: list[str] = []
    if src.exists():
        for p in src.rglob("*.cpp"):
            try:
                txt = p.read_text(encoding="utf-8", errors="replace")
            except Exception:
                continue
            if short in txt.lower():
                impl_hits.append(str(p.relative_to(ROOT)))
                break

    diff_hits: list[str] = []
    if log.exists():
        for p in log.glob(f"diff_*{short}*.csv"):
            diff_hits.append(str(p.relative_to(ROOT)))

    if impl_hits and diff_hits:
        return f"DRIFT impl={impl_hits[0]} diff={diff_hits[0]}"
    return None


# ------ arg_type filter ------------------------------------------------------

# Read the note frontmatter or look for an explicit arg_type marker. Many of
# these C2 notes don't have explicit frontmatter, so we mark "unknown" unless
# the note explicitly says so. The selection algorithm flags unknown arg_type
# candidates as "needs-inspection" rather than auto-rejecting — we just want
# to flag them for the worker session.


ARG_TYPE_RE = re.compile(
    r"^(?:arg_type|signature_arg_type):\s*([a-z_0-9]+)\s*$", re.MULTILINE
)


def arg_type_present(note_body: str, supported: set[str]) -> tuple[str, bool]:
    """
    Returns (arg_type_name_or_'unknown', supported_bool). If the note doesn't
    explicitly declare an arg_type, returns ('unknown', True) — we let the
    worker session decide. Only return False when the note explicitly names
    an arg_type AND that name is NOT in `supported`.
    """
    m = ARG_TYPE_RE.search(note_body)
    if not m:
        return "unknown", True
    name = m.group(1)
    return name, name in supported


def main():
    print("Loading inputs...")
    hooks = load_hooks()
    catalogued = load_uncertainties()
    supported = load_supported_arg_types()
    print(f"  hooks.csv rows: {len(hooks)}")
    print(f"  catalogued U-IDs: {len(catalogued)}")
    print(f"  supported arg_types: {len(supported)}")
    print()

    # Pull C2 candidates from target subsystems
    candidates: list[dict] = []
    for rva, row in hooks.items():
        if row["confidence"] != "C2":
            continue
        if row["subsystem"] not in TARGET_SUBSYSTEMS:
            continue
        if not row["file"].endswith(".md"):
            continue
        candidates.append(row)

    print(f"=== Initial pull: {len(candidates)} C2 candidates ===")
    by_sub = {}
    for c in candidates:
        by_sub.setdefault(c["subsystem"], 0)
        by_sub[c["subsystem"]] += 1
    for sub, n in sorted(by_sub.items()):
        print(f"  {sub}: {n}")
    print()

    # Apply each filter, log eliminations
    elim_counts = {
        "already_c3plus": 0,
        "stale_drift": 0,
        "inline_uncertain": 0,
        "callee_gate": 0,
        "arg_type": 0,
    }
    passed: list[dict] = []
    rejected: list[tuple[dict, str]] = []

    for c in candidates:
        rva = c["rva"]
        note_path = ROOT / c["file"]
        body = read_note(note_path)
        if not body:
            rejected.append((c, "no-note"))
            continue

        # Filter: stale-impl drift
        drift = stale_impl_check(rva)
        if drift:
            elim_counts["stale_drift"] += 1
            rejected.append((c, drift))
            continue

        # Filter: inline UNCERTAIN (with catalogued exemption + Resolves-skip)
        unc = inline_uncertain_blocks(body, catalogued)
        if unc:
            elim_counts["inline_uncertain"] += 1
            rejected.append((c, unc))
            continue

        # Filter: callee gate (section-scoped, Resolves-skipped)
        callees = extract_callees(body, rva)
        passes, blocking = callee_gate(callees, hooks)
        if not passes:
            elim_counts["callee_gate"] += 1
            rejected.append((c, f"callee-gate: {','.join(blocking)}"))
            continue

        # Filter: arg_type
        atype, ok = arg_type_present(body, supported)
        if not ok:
            elim_counts["arg_type"] += 1
            rejected.append((c, f"arg_type={atype} not supported"))
            continue

        c["arg_type"] = atype
        c["callees"] = callees
        passed.append(c)

    print(f"=== Filter eliminations ===")
    for k, v in elim_counts.items():
        print(f"  {k}: {v}")
    print()
    print(f"=== Passed all filters: {len(passed)} ===")

    by_sub = {}
    for c in passed:
        by_sub.setdefault(c["subsystem"], 0)
        by_sub[c["subsystem"]] += 1
    for sub, n in sorted(by_sub.items()):
        print(f"  {sub}: {n}")

    # Write outputs
    out_dir = ROOT / "re" / "analysis" / "plans"
    with open(out_dir / "c3_batch_j_passed.tsv", "w", encoding="utf-8") as f:
        f.write("rva\tname\tsubsystem\targ_type\tnote\tcallees\n")
        for c in passed:
            f.write(
                "\t".join(
                    [
                        c["rva"],
                        c["name"],
                        c["subsystem"],
                        c.get("arg_type", ""),
                        c["file"],
                        ",".join(c.get("callees", [])),
                    ]
                )
                + "\n"
            )

    with open(out_dir / "c3_batch_j_rejected.tsv", "w", encoding="utf-8") as f:
        f.write("rva\tname\tsubsystem\treason\n")
        for c, reason in rejected:
            f.write("\t".join([c["rva"], c["name"], c["subsystem"], reason]) + "\n")

    print(f"\nWrote: {out_dir / 'c3_batch_j_passed.tsv'}")
    print(f"Wrote: {out_dir / 'c3_batch_j_rejected.tsv'}")


if __name__ == "__main__":
    main()
