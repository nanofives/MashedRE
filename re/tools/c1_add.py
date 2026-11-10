#!/usr/bin/env python3
"""Add a C1 hooks.csv row for a game function that has none (re-classify skill, "C1 row for a game function").

HB-CHANGELOG-MARKER (2026-11-10). POWERUPINIT-B, POWERUPINIT-C and MATCH-40BB10 added game C1 rows with ad-hoc inline
Python that anchored the CHANGELOG insert on the first TEXTUAL match of `<!-- ENTRIES -->`. That first match is the
header prose (line 13: "prepended immediately below the `<!-- ENTRIES -->` marker"), so every insert split that line
and the entries landed in the header. This helper is the shared replacement. Its anchor is the one line EQUAL to the
marker (stripped), and it refuses when there is no such line or more than one.

  py -3.12 re/tools/c1_add.py --rva 0x0040bb10 --subsystem gameplay --file re/match/s29/0040bb10.c \
      --why "<C1 evidence: xref / call site / strings>" --result verify/<...>/RESULT_<ROW>.md [--tag ROW] [--name N]
      [--date YYYY-MM-DD] [--dry-run]

Writes, in one go:
- hooks.csv: appends one 11-field row (rva, name, subsystem, C1, mapped, file, '', '', notes, '', ''), with the file's
  own line ending. exe_file / port stay empty, and scripts/backfill_exe_file.py / backfill_port.py fill them.
- re/analysis/CHANGELOG.md: one `date  0x<rva>  name  (none)->C1  added (<tag>): <why>; evidence: <result>` line right
  below the marker line.
Refuses: a row for the RVA already exists, the RESULT file is missing, the marker line is missing or doubled.
Promotion past C1 goes through c4_reclass.py (match / libid) as before.

Self-test: py -3.12 -m pytest re/tools/tests/test_c1_add.py
"""
import argparse, csv, datetime, io, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
HOOKS = ROOT / "hooks.csv"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MARKER = "<!-- ENTRIES -->"
SUBSYSTEMS = {"boot", "audio", "ai", "vehicle", "track", "render", "hud", "frontend", "input", "save", "net", "util",
              "unknown", "gameplay", "particle", "camera", "physics"}


def marker_index(lines):
    """Index of the one line equal to the marker. A line that merely CONTAINS it (header prose) never counts."""
    hits = [i for i, l in enumerate(lines) if l.strip() == MARKER]
    if len(hits) != 1:
        sys.exit(f"REFUSED: CHANGELOG has {len(hits)} lines equal to {MARKER!r} (need exactly 1)")
    return hits[0]


def prepend_changelog(text, entries):
    eol = "\r\n" if "\r\n" in text else "\n"
    lines = text.split(eol)
    i = marker_index(lines)
    lines[i + 1:i + 1] = entries
    return eol.join(lines)


def norm_rva(s):
    return "%08x" % int(s, 16)


def main(argv=None):
    ap = argparse.ArgumentParser()
    ap.add_argument("--rva", required=True)
    ap.add_argument("--subsystem", required=True)
    ap.add_argument("--file", default="")
    ap.add_argument("--why", required=True)
    ap.add_argument("--result", required=True)
    ap.add_argument("--tag", default="")
    ap.add_argument("--name", default="")
    ap.add_argument("--date", default=datetime.date.today().isoformat())
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args(argv)

    rva = norm_rva(a.rva)
    name = a.name or f"FUN_{rva}"
    if a.subsystem not in SUBSYSTEMS and not a.subsystem.startswith("third-party-library["):
        sys.exit(f"REFUSED: unknown subsystem {a.subsystem!r}")
    result = a.result.strip().replace("\\", "/")
    if not (ROOT / result).is_file():
        sys.exit(f"REFUSED: --result {result} does not exist")
    why = a.why.strip().rstrip(".")
    tag = a.tag or "c1_add"

    raw = HOOKS.read_bytes().decode("utf-8")
    heol = "\r\n" if "\r\n" in raw else "\n"
    if rva in {l.split(",", 1)[0].strip().lower() for l in raw.split(heol)}:
        sys.exit(f"REFUSED: hooks.csv already has a row for {rva}")
    note = f"{tag} {a.date} (none)->C1: {why}; name {name}; {result}"
    buf = io.StringIO()
    csv.writer(buf, lineterminator="").writerow([rva, name, a.subsystem, "C1", "mapped", a.file, "", "", note, "", ""])
    if not raw.endswith(heol):
        raw += heol
    new_hooks = raw + buf.getvalue() + heol

    entry = f"{a.date}  0x{rva}  {name}  (none)->C1  added ({tag}): {why}; evidence: {result}"
    new_cl = prepend_changelog(CHANGELOG.read_bytes().decode("utf-8"), [entry])
    print(f"[1/1] 0x{rva} {name} {a.subsystem} (none)->C1" + (" (dry run)" if a.dry_run else ""))
    if a.dry_run:
        return 0
    HOOKS.write_bytes(new_hooks.encode("utf-8"))
    CHANGELOG.write_bytes(new_cl.encode("utf-8"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
