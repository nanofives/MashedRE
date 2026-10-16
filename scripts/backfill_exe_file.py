"""Fill the `exe_file` column of hooks.csv.

`exe_file` answers one question: **which TU compiled into `mashed_re.exe` carries
a body for this RVA?**  Empty means the exe has no port of it.

Two evidence channels, unioned (same two the audit used --
`re/analysis/DUAL_COPY_AUDIT_2026-09-29.md` §2):

  A  RVA-anchored definitions found by `scripts/rva_body_scan.py` in a TU that
     `mashedmod/exe_sources.rsp` (or build.bat directly) compiles. Mechanical and
     re-runnable; this is the channel the build guard also uses.
  B  the `exe_side_copy` column of `re/analysis/dual_copy_audit_2026-09-29.csv`,
     for the 107 audited RVAs. This catches ports the exe implements as a NAMED
     standalone method with no RVA comment above it (`VehicleInit`,
     `ForceIntegrator`, `ScoreAward`...), which channel A structurally cannot see.

Where an RVA has more than one exe-side body the cell holds all of them joined by
`|`. That is not a formatting choice -- it is the defect the guard reports.

  py -3.12 scripts/backfill_exe_file.py [--check]
"""
from __future__ import annotations

import csv
import io
import pathlib
import re
import shutil
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rva_body_scan as S            # noqa: E402

ROOT = S.ROOT
CSV_PATH = ROOT / "hooks.csv"
AUDIT = ROOT / "re" / "analysis" / "dual_copy_audit_2026-09-29.csv"
BACKUP = ROOT / "log" / "hooks.csv.pre-exe_file-backfill.bak"


def canon_tu_index() -> dict[str, str]:
    """lowercased relative path -> the real on-disk relative path, for exe TUs."""
    idx = {}
    for rel in S.load_targets()["exe"]:
        idx[rel.lower()] = rel
    return idx


def channel_b(idx: dict[str, str]) -> dict[int, set[str]]:
    """exe_side_copy from the audit CSV, resolved to real exe TU paths."""
    out: dict[int, set[str]] = {}
    if not AUDIT.exists():
        return out
    for r in csv.DictReader(AUDIT.open(encoding="utf-8")):
        try:
            rva = int(r["rva"].replace("0x", ""), 16)
        except ValueError:
            continue
        cell = r.get("exe_side_copy", "") or ""
        for tok in re.split(r"[|;,]", cell):
            tok = tok.strip().split(":")[0].strip().replace("\\", "/")
            if not tok.lower().endswith(".cpp"):
                continue
            real = idx.get(tok.lower())
            if real:                       # only if the exe really compiles it
                out.setdefault(rva, set()).add(real)
    return out


def split_records(text: str):
    """Yield (raw, fields) per CSV RECORD, not per physical line.

    A quoted `notes` cell may hold a newline, so a record may span several lines (hooks.csv has
    none today: 5934 records, 5934 lines, checked 2026-10-16, but the parse must not assume it).
    The line-by-line parse this replaces died at "line 11 has 9 fields" for a different reason:
    record 11 is a QUOTED comment row (`"# exe_file: ...`) that the `startswith("#")` test missed.
    `raw` is the record's exact text including its terminator, so an unchanged record is written
    back byte for byte. io.StringIO(newline="") keeps the file's own CRLF and tell() is exact."""
    sio = io.StringIO(text, newline="")
    pos = 0
    for fields in csv.reader(sio):
        end = sio.tell()
        yield text[pos:end], fields
        pos = end
    if pos < len(text):                  # trailing text with no record (blank lines)
        yield text[pos:], []


def main(argv) -> int:
    check_only = "--check" in argv
    text = CSV_PATH.read_bytes().decode("utf-8")
    records = list(split_records(text))
    cols = records[0][1]
    if "exe_file" not in cols:
        print("FATAL: hooks.csv has no exe_file column; run "
              "scripts/add_exe_file_column.py first")
        return 1
    xi = cols.index("exe_file")

    idx = canon_tu_index()
    scan = S.scan_all()
    a = S.rva_to_tus(scan, "exe")                    # {rva: [rel, ...]}
    b = channel_b(idx)

    merged: dict[int, list[str]] = {}
    for rva, tus in a.items():
        merged.setdefault(rva, []).extend(tus)
    for rva, tus in b.items():
        merged.setdefault(rva, []).extend(tus)
    # Store with the same `mashedmod/src/mashed_re/` prefix the `file` column
    # uses, so `file == exe_file` is a direct string test -- which is exactly
    # what the 2026-09-29 CONFIDENCE.md clause asks a reader to evaluate.
    PFX = "mashedmod/src/mashed_re/"
    for rva in merged:
        merged[rva] = sorted({PFX + t for t in merged[rva]})

    out_lines, filled, multi, overwritten, unchanged = [], 0, 0, 0, 0
    malformed, changes = [], []
    for i, (raw, row) in enumerate(records):
        # comment rows may be quoted (record 11) and then carry commas -> any width; pass them through
        if i == 0 or not raw.strip() or raw.startswith("#") or (row and row[0].startswith("#")):
            out_lines.append(raw)
            continue
        if len(row) != len(cols):
            # a data row whose notes hold an unquoted comma: its columns are shifted, so exe_file cannot be
            # located. Keep it byte for byte and say so; it is a tracker defect, not this script's to rewrite.
            malformed.append((i + 1, row[0] if row else "", len(row)))
            out_lines.append(raw)
            continue
        try:
            rva = int(row[0], 16)
        except ValueError:
            out_lines.append(raw)
            continue
        want = "|".join(merged.get(rva, []))
        if want == row[xi]:
            unchanged += 1
            out_lines.append(raw)
            continue
        if row[xi]:
            overwritten += 1
        changes.append((row[0], row[xi], want))
        if want:
            filled += 1
            if "|" in want:
                multi += 1
        row[xi] = want
        term = raw[len(raw.rstrip("\r\n")):]       # keep the record's own terminator
        buf = io.StringIO(newline="")
        csv.writer(buf, lineterminator=term).writerow(row)
        out_lines.append(buf.getvalue())

    print(f"channel A (RVA-anchored, exe TUs): {len(a)} RVAs")
    print(f"channel B (audit exe_side_copy):   {len(b)} RVAs")
    print(f"union:                             {len(merged)} RVAs")
    print(f"hooks.csv rows given an exe_file:  {filled} "
          f"(of which {multi} name MORE THAN ONE exe TU)")
    print(f"rows already correct: {unchanged}   rows whose old value changed: {overwritten}")
    if check_only:
        P = "mashedmod/src/mashed_re/"
        for rva_s, old, new in changes:
            print(f"  would change {rva_s}: {old.replace(P, '') or '(empty)'}  ->  {new.replace(P, '') or '(empty)'}")
    for rec, rva_s, width in malformed:
        print(f"WARN: record {rec} (rva {rva_s}) has {width} fields, expected {len(cols)} "
              f"(unquoted comma in a cell); left untouched, its exe_file was not evaluated")

    if check_only:
        print("--check: not written")
        return 0
    BACKUP.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(CSV_PATH, BACKUP)
    CSV_PATH.write_bytes("".join(out_lines).encode("utf-8"))
    print(f"written; backup at {BACKUP.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
