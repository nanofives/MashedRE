#!/usr/bin/env python3
"""Tag the broken-loader-window C4 rows as evidence-suspect (KEEP C4 label).

Per the 2026-06-05 C3->C4 planning decision: do NOT mass-demote the 79 C4 rows
whose canonical evidence falls in the broken-loader window (2026-05-15..24) or
cites mass_canonical / hot-path / implicit tooling. Instead append a SUSPECT
marker to the notes column so the tracker stops overstating verified confidence,
and re-validate via the Phase-0 installed-hook canonical harness later.

Why suspect: in that window the .asi was not actually loaded (Interceptor counted
unhooked ORIGINALS), and mass_canonical_observe.py uses Interceptor.attach (it
observes the ORIGINAL function executing, not our INSTALLED inline-JMP). Some
frontend rows were install-reconfirmed post-loaderfix 0526 ('install_only') but
that confirms the JMP landed, NOT a clean canonical DIFF.

Raw-line CSV editor: confidence is UNCHANGED (stays C4); only col[8] notes gets
the marker appended. Idempotent (skips rows already tagged).
"""
import csv, io, re, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
MARKER = ("| C4-EVIDENCE-SUSPECT: canonical evidence in broken-loader window "
          "(2026-05-15..24) or mass_canonical/hot-path (Interceptor.attach observed "
          "ORIGINAL, not installed inline-JMP). Some frontend rows install-reconfirmed "
          "post-loaderfix 0526 but canonical DIFF still pre-fix. C4 RETAINED pending "
          "Phase-0 installed-hook canonical re-validation (decision 2026-06-05).")

SUSPECT_SCEN = re.compile(r"2026-05-1[5-9]|2026-05-2[0-4]|mass_canonical|hot-path|implicit")
SUSPECT_DIFF = re.compile(r"mass_canonical|hot-path")

def parse_line(line):
    return next(csv.reader([line.rstrip("\n")]))

def emit_line(fields):
    buf = io.StringIO()
    csv.writer(buf, lineterminator="").writerow(fields)
    return buf.getvalue()

def main():
    hooks = REPO + r"\hooks.csv"
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()

    tagged = 0
    skipped_already = 0
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"):
            continue
        fields = parse_line(line)
        if len(fields) < 9 or fields[3] != "C4":
            continue
        scen, diff, notes = fields[6], fields[7], fields[8]
        if not (SUSPECT_SCEN.search(scen) or SUSPECT_DIFF.search(diff)):
            continue
        if "C4-EVIDENCE-SUSPECT" in notes:
            skipped_already += 1
            continue
        fields[8] = (notes + " " + MARKER).strip() if notes else MARKER.lstrip("| ").strip()
        lines[i] = emit_line(fields) + "\n"
        tagged += 1

    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"tagged {tagged} suspect C4 rows (already-tagged skipped: {skipped_already})")
    if tagged != 79 and skipped_already == 0:
        print(f"WARNING: expected 79, tagged {tagged} — verify criteria", file=sys.stderr)

if __name__ == "__main__":
    main()
