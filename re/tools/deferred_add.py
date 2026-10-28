#!/usr/bin/env python3
"""Add a DEFERRED.md Active row (re-classify skill, "Add DEFERRED row").

Usage: deferred_add.py add --title "<text>" --why "<text>" --when "<observable condition>" --tag "<phase tag>"
                           --reason "<changelog reason>" [--date YYYY-MM-DD]
                           [--rvas-tsv <tsv> --rvas-id <id> [--rvas-id-col id] [--rvas-col rva]]
                           [--dry-run]

add   takes the next monotonic ID (max D-NNNN anywhere in DEFERRED.md, struck rows and Cleared
      included, plus one), appends the row after the last row of the "## Active" table, and
      prepends one line to re/analysis/CHANGELOG.md below the <!-- ENTRIES --> marker.
      With --rvas-tsv/--rvas-id the RVAs of every TSV row whose id column equals --rvas-id are
      appended to the title as `0x<rva>` (address order), so a census group is one row.
Refuses: empty title/why/when/tag, a re-pickup condition that is not observable ("later",
"someday", "tbd"), a title that already exists in the file, a cell containing " | " or a newline,
and a --rvas-id that matches no TSV row. Only the "## Active" section grows. Cell count stays 5.
Line endings of DEFERRED.md and CHANGELOG.md are preserved byte for byte.
"""
import argparse, csv, datetime, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
DEFERRED = ROOT / "DEFERRED.md"
CHANGELOG = ROOT / "re" / "analysis" / "CHANGELOG.md"
MARKER = "<!-- ENTRIES -->"
VAGUE = {"later", "someday", "tbd", "n/a", "na", "when i feel like it"}


def read_raw(path):
    with open(path, encoding="utf-8", newline="") as f:
        return f.read()


def write_raw(path, text):
    with open(path, "w", encoding="utf-8", newline="") as f:
        f.write(text)


def eol_of(text):
    return "\r\n" if "\r\n" in text else "\n"


def next_id(text):
    ids = [int(m) for m in re.findall(r"^\| (?:~~)?D-(\d+)", text, flags=re.M)]
    return (max(ids) if ids else 0) + 1


def rvas_for(tsv, id_col, rva_col, want):
    with open(tsv, encoding="utf-8", newline="") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    if not rows or id_col not in rows[0] or rva_col not in rows[0]:
        sys.exit(f"REFUSED: {tsv} lacks column {id_col!r} or {rva_col!r}")
    hit = sorted({r[rva_col].strip().lower().removeprefix("0x") for r in rows if r[id_col].strip() == want})
    if not hit:
        sys.exit(f"REFUSED: no TSV row has {id_col} == {want}")
    return hit


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("op", choices=["add"])
    ap.add_argument("--title", required=True)
    ap.add_argument("--why", required=True)
    ap.add_argument("--when", required=True)
    ap.add_argument("--tag", required=True)
    ap.add_argument("--reason", required=True)
    ap.add_argument("--date", default=datetime.date.today().isoformat())
    ap.add_argument("--rvas-tsv")
    ap.add_argument("--rvas-id")
    ap.add_argument("--rvas-id-col", default="id")
    ap.add_argument("--rvas-col", default="rva")
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()

    title = a.title.strip()
    n_rvas = 0
    if a.rvas_tsv or a.rvas_id:
        if not (a.rvas_tsv and a.rvas_id):
            sys.exit("REFUSED: --rvas-tsv and --rvas-id go together")
        rv = rvas_for(a.rvas_tsv, a.rvas_id_col, a.rvas_col, a.rvas_id)
        n_rvas = len(rv)
        title = f"{title} ({n_rvas} rows): " + ", ".join(f"`0x{r}`" for r in rv)
    cells = [title, a.why.strip(), a.when.strip(), a.tag.strip()]
    for c in cells:
        if not c:
            sys.exit("REFUSED: empty cell")
        if " | " in c or "\n" in c or "\r" in c:
            sys.exit("REFUSED: a cell contains ' | ' or a newline")
    if a.when.strip().lower().rstrip(".") in VAGUE:
        sys.exit("REFUSED: re-pickup condition must be observable")

    text = read_raw(DEFERRED)
    eol = eol_of(text)
    lines = text.split(eol)
    s = next(i for i, l in enumerate(lines) if l.startswith("## Active"))
    e = next(i for i in range(s + 1, len(lines)) if lines[i].startswith("## "))
    if any(l.startswith("| ") and l.split(" | ")[1:2] == [title] for l in lines[s:e]):
        sys.exit("REFUSED: a row with this exact title is already in Active")
    last = max(i for i in range(s, e) if lines[i].startswith("| D-") or lines[i].startswith("| ~~D-"))
    did = f"D-{next_id(text)}"
    row = "| " + " | ".join([did] + cells) + " |"
    lines.insert(last + 1, row)
    new_text = eol.join(lines)

    entry = f"{a.date}  {did}  DEFERRED  added to Active  {a.reason}" + (f" ({n_rvas} RVAs)" if n_rvas else "")
    ctext = read_raw(CHANGELOG)
    ceol = eol_of(ctext)
    cl = ctext.split(ceol)
    idx = next((i for i, l in enumerate(cl) if l.strip() == MARKER), None)
    if idx is None:
        sys.exit("REFUSED: CHANGELOG marker line missing")
    cl.insert(idx + 1, entry)

    if a.dry_run:
        print(f"dry-run: would add {did} ({n_rvas} RVAs)")
        return
    write_raw(DEFERRED, new_text)
    write_raw(CHANGELOG, ceol.join(cl))
    print(f"add: {did} ({n_rvas} RVAs)")


if __name__ == "__main__":
    main()
