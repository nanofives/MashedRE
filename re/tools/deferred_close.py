#!/usr/bin/env python3
"""Close or rewrite a DEFERRED.md Active row (re-classify skill, "Close DEFERRED row").

Close:   deferred_close.py close --reason "<why>" --evidence "<path>" [--date YYYY-MM-DD] D-0245 D-0246 ...
Rewrite: deferred_close.py rewrite --id D-11073 [--why "<text>"] [--when "<text>"] [--tag "<text>"]
                           --reason "<changelog reason>"

close   strikes the ID and title cells of each Active-table row (the existing struck-row
        convention), replaces the "Why deferred" cell with "RESOLVED <date> <reason>
        (evidence: <evidence>)", sets "Re-pickup when" to "—", and prepends one line per
        row to re/analysis/CHANGELOG.md below the <!-- ENTRIES --> marker.
rewrite replaces cells of a row that stays Active and writes one CHANGELOG line.
Only the "## Active" section is touched. Refuses rows that are missing or already struck.
"""
import argparse, datetime, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFERRED = ROOT / "DEFERRED.md"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MARKER = "<!-- ENTRIES -->"
NL = chr(10)


def active_bounds(lines):
    start = next(i for i, l in enumerate(lines) if l.startswith("## Active"))
    end = next(i for i in range(start + 1, len(lines)) if lines[i].startswith("## "))
    return start, end


def find_row(lines, did):
    s, e = active_bounds(lines)
    for i in range(s, e):
        if lines[i].startswith(f"| {did} |"):
            return i
    sys.exit(f"REFUSED: {did} has no unstruck row in the Active table")


def split(line):
    cells = line.rstrip("\n").split(" | ")
    cells[0] = cells[0][2:]          # drop leading "| "
    cells[-1] = cells[-1][:-2]       # drop trailing " |"
    if len(cells) != 5:
        sys.exit(f"REFUSED: row has {len(cells)} cells, expected 5: {line[:60]}")
    return cells


def join(cells):
    return "| " + " | ".join(cells) + " |\n"


def prepend_changelog(entries):
    with open(CHANGELOG, encoding="utf-8", newline="") as f:
        lines = f.read().split(NL)
    idx = next((i for i, l in enumerate(lines) if l.strip() == MARKER), None)
    if idx is None:
        sys.exit("REFUSED: CHANGELOG marker line missing")
    lines[idx + 1:idx + 1] = entries
    with open(CHANGELOG, "w", encoding="utf-8", newline="") as f:
        f.write(NL.join(lines))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("op", choices=["close", "rewrite"])
    ap.add_argument("ids", nargs="*")
    ap.add_argument("--id")
    ap.add_argument("--reason", required=True)
    ap.add_argument("--evidence", default="")
    ap.add_argument("--date", default=datetime.date.today().isoformat())
    ap.add_argument("--why")
    ap.add_argument("--when")
    ap.add_argument("--tag")
    a = ap.parse_args()

    lines = DEFERRED.read_text(encoding="utf-8").splitlines(keepends=True)
    log = []
    if a.op == "close":
        if not a.ids:
            sys.exit("REFUSED: no IDs given")
        for did in a.ids:
            i = find_row(lines, did)
            c = split(lines[i])
            c[0] = f"~~{did}~~"
            c[1] = f"~~{c[1]}~~"
            ev = f" (evidence: {a.evidence})" if a.evidence else ""
            c[2] = f"RESOLVED {a.date}: {a.reason}{ev}"
            c[3] = "—"
            lines[i] = join(c)
            log.append(f"{a.date}  {did}  DEFERRED  Active->struck  {a.reason}{ev}")
    else:
        if not a.id:
            sys.exit("REFUSED: rewrite needs --id")
        i = find_row(lines, a.id)
        c = split(lines[i])
        if a.why is not None:
            c[2] = a.why
        if a.when is not None:
            c[3] = a.when
        if a.tag is not None:
            c[4] = a.tag
        lines[i] = join(c)
        log.append(f"{a.date}  {a.id}  DEFERRED  rewritten, stays Active  {a.reason}")
    DEFERRED.write_text("".join(lines), encoding="utf-8")
    prepend_changelog(log)
    print(f"{a.op}: {len(log)} row(s) done")


if __name__ == "__main__":
    main()
