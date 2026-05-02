"""Merge tracker files (hooks.csv, STUBS.md, UNCERTAINTIES.md, DEFERRED.md)
across two branches: rebase the incoming branch's monotonic IDs above the
existing maximum, rewrite inline references, write merged output.

Skeleton — implements RVA-keyed merge for hooks.csv only. ID-renumber for
the markdown files is stubbed (raises NotImplementedError) and will be
fleshed out the first time a real conflict appears.

Usage:
    py -3.12 re/tools/merge_trackers.py hooks.csv <ours.csv> <theirs.csv> -o <out.csv>
"""
import argparse
import csv
import sys
from pathlib import Path

LEVELS = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}


def merge_hooks(ours: Path, theirs: Path, out: Path) -> int:
    """Merge two hooks.csv copies by RVA. Higher confidence wins. Conflicts on
    diverging name/subsystem are reported and abort with non-zero exit."""

    def _load(p: Path):
        rows = {}
        with p.open(newline="", encoding="utf-8") as f:
            reader = csv.DictReader(line for line in f if not line.startswith("#"))
            fields = reader.fieldnames or []
            for r in reader:
                rva = (r.get("rva") or "").lower()
                if not rva:
                    continue
                rows[rva] = r
        return rows, fields

    a, fields_a = _load(ours)
    b, _ = _load(theirs)
    keys = sorted(set(a) | set(b))
    merged = []
    conflicts = []
    for k in keys:
        ra = a.get(k)
        rb = b.get(k)
        if ra and not rb:
            merged.append(ra)
            continue
        if rb and not ra:
            merged.append(rb)
            continue
        # both present
        if (ra.get("name") and rb.get("name") and ra["name"] != rb["name"]) \
           or (ra.get("subsystem") and rb.get("subsystem") and ra["subsystem"] != rb["subsystem"]):
            conflicts.append((k, ra, rb))
            continue
        higher = ra if LEVELS.get(ra.get("confidence", "C0"), 0) >= LEVELS.get(rb.get("confidence", "C0"), 0) else rb
        # take other fields' freshest non-empty values
        out_row = dict(higher)
        for fld in ("name", "subsystem", "file", "scenario", "frida_diff", "notes"):
            if not out_row.get(fld):
                out_row[fld] = ra.get(fld) or rb.get(fld) or ""
        merged.append(out_row)

    if conflicts:
        print(f"ERROR: {len(conflicts)} conflicting row(s) in hooks.csv — refusing merge:", file=sys.stderr)
        for rva, ra, rb in conflicts:
            print(f"  RVA {rva}: ours={ra.get('name')}/{ra.get('subsystem')}  theirs={rb.get('name')}/{rb.get('subsystem')}", file=sys.stderr)
        print("Resolve manually, then re-run.", file=sys.stderr)
        return 2

    with out.open("w", newline="", encoding="utf-8") as f:
        f.write("# merged hooks.csv — see re/tools/merge_trackers.py\n")
        writer = csv.DictWriter(f, fieldnames=fields_a)
        writer.writeheader()
        for r in merged:
            writer.writerow({k: r.get(k, "") for k in fields_a})
    print(f"merged: {len(merged)} rows -> {out}", file=sys.stderr)
    return 0


def merge_md_renumber(file_kind: str, ours: Path, theirs: Path, out: Path) -> int:
    """Renumber monotonic IDs in `theirs` to land after the highest in `ours`,
    rewrite inline references in source files, write merged markdown.

    NOT YET IMPLEMENTED — will be authored the first time a real conflict
    arises. Until then this raises so we don't silently produce wrong output.
    """
    raise NotImplementedError(
        f"merge for {file_kind} (monotonic ID renumber + source rewrite) is not yet implemented. "
        f"Hand-merge for now and document the renumber in the commit message."
    )


def main(argv=None):
    p = argparse.ArgumentParser()
    p.add_argument("kind", choices=["hooks.csv", "STUBS.md", "UNCERTAINTIES.md", "DEFERRED.md"])
    p.add_argument("ours", type=Path)
    p.add_argument("theirs", type=Path)
    p.add_argument("-o", "--out", type=Path, required=True)
    args = p.parse_args(argv)
    if args.kind == "hooks.csv":
        return merge_hooks(args.ours, args.theirs, args.out)
    return merge_md_renumber(args.kind, args.ours, args.theirs, args.out)


if __name__ == "__main__":
    raise SystemExit(main())
