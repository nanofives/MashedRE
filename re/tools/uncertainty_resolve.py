#!/usr/bin/env python3
"""Resolve an UNCERTAINTIES.md Active row (re-classify skill, "Resolve an UNCERTAINTIES row").

Usage: uncertainty_resolve.py resolve --reason "<why>" --evidence "<repo-relative path>" [--date YYYY-MM-DD]
                                      [--partial] [--dry-run] U-NNNN [U-NNNN ...]

resolve strikes the Type cell of each Active-table row (`~~type~~`, the U-9201 / U-9205
convention), prefixes the Statement cell with `**RESOLVED <date>** (<evidence>): <reason>.`
and keeps the old statement after it, sets "Evidence missing" and "Path to resolution" to
`n/a`, and prepends one line per row to re/analysis/CHANGELOG.md below <!-- ENTRIES -->:
`date  U-NNNN  UNCERTAINTIES  Active->struck  <reason> (evidence: <path>)`.
--partial writes `RESOLVED (PARTIAL)` and leaves the Blocks cell alone (a residual was split out).
Refuses: an unknown ID, an already struck row, a missing or non-existent evidence file, an empty
reason, a row that does not have 7 cells, a cell argument containing a newline or ' | '.
Idempotent: a second run on the same ID refuses and writes nothing. Nothing is written unless
every ID passes. Only the "## Active uncertainties" section is touched. Line endings of each
line are preserved (UNCERTAINTIES.md mixes LF and CRLF).
"""
import argparse, datetime, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
UNC = ROOT / "UNCERTAINTIES.md"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MARKER = "<!-- ENTRIES -->"


def read_raw(path):
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_raw(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def split_eol(line):
    body = line.rstrip("\r\n")
    return body, line[len(body):]


def resolve_rows(text, ids, date, reason, evidence, partial):
    """Return (new_text, changelog_lines). Exits with REFUSED on any problem, before any write."""
    lines = text.splitlines(keepends=True)
    s = next((i for i, l in enumerate(lines) if l.startswith("## Active uncertainties")), None)
    if s is None:
        sys.exit("REFUSED: '## Active uncertainties' section missing")
    e = next((i for i in range(s + 1, len(lines)) if lines[i].startswith("## ")), len(lines))
    tag = "RESOLVED (PARTIAL)" if partial else "RESOLVED"
    log = []
    for uid in ids:
        idx = next((i for i in range(s, e) if lines[i].startswith(f"| {uid} |")), None)
        if idx is None:
            struck = any(lines[i].startswith(f"| ~~{uid}~~") for i in range(s, e))
            sys.exit(f"REFUSED: {uid} is already struck" if struck else f"REFUSED: {uid} has no row in the Active table")
        body, eol = split_eol(lines[idx])
        cells = body[2:-2].split(" | ")
        if len(cells) != 7:
            sys.exit(f"REFUSED: {uid} row has {len(cells)} cells, expected 7")
        if cells[1].startswith("~~") or "**RESOLVED" in cells[1] or "**RESOLVED" in cells[2]:
            sys.exit(f"REFUSED: {uid} is already struck")
        cells[1] = f"~~{cells[1]}~~"
        cells[3] = f"**{tag} {date}** ({evidence}): {reason}. Prior statement: {cells[3]}"
        cells[4] = "n/a"
        cells[5] = "n/a"
        if not partial:
            cells[6] = "nothing"
        lines[idx] = "| " + " | ".join(cells) + " |" + eol
        log.append(f"{date}  {uid}  UNCERTAINTIES  Active->struck  {reason} (evidence: {evidence})")
    return "".join(lines), log


def prepend_changelog(ctext, entries):
    ceol = "\r\n" if "\r\n" in ctext else "\n"
    cl = ctext.split(ceol)
    idx = next((i for i, l in enumerate(cl) if l.strip() == MARKER), None)
    if idx is None:
        sys.exit("REFUSED: CHANGELOG marker line missing")
    cl[idx + 1:idx + 1] = entries
    return ceol.join(cl)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("op", choices=["resolve"])
    ap.add_argument("ids", nargs="+")
    ap.add_argument("--reason", required=True)
    ap.add_argument("--evidence", required=True)
    ap.add_argument("--date", default=datetime.date.today().isoformat())
    ap.add_argument("--partial", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()

    reason = a.reason.strip().rstrip(".")
    evidence = a.evidence.strip().replace("\\", "/")
    if not reason:
        sys.exit("REFUSED: empty reason")
    for c in (reason, evidence):
        if " | " in c or "\n" in c or "\r" in c:
            sys.exit("REFUSED: a cell contains ' | ' or a newline")
    if not (ROOT / evidence).is_file():
        sys.exit(f"REFUSED: evidence file not found: {evidence}")
    if len(set(a.ids)) != len(a.ids):
        sys.exit("REFUSED: duplicate IDs")

    new_text, log = resolve_rows(read_raw(UNC), a.ids, a.date, reason, evidence, a.partial)
    new_ctext = prepend_changelog(read_raw(CHANGELOG), log)
    if a.dry_run:
        print(f"dry-run: would resolve {len(log)} row(s)")
        return
    write_raw(UNC, new_text)
    write_raw(CHANGELOG, new_ctext)
    print(f"resolve: {len(log)} row(s) done")


if __name__ == "__main__":
    main()
