#!/usr/bin/env python3
"""Central re-classify for c3_batch_ae frida-sweep (C2->C3, 5 render leaves).

Raw-line CSV editor (per scripted-ghidra-sweep convention): parses ONLY the 5
target lines via csv to handle quoting, flips confidence C2->C3, appends the
evidence marker to the notes column. Appends CHANGELOG rows + stamps
PROMOTION_QUEUE drained-by. Idempotent: refuses if a target is already C3.
"""
import csv, io, sys, datetime

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SWEEP = "frida-sweep-20260604-ae"
MARKER = ("| C2->C3 c3_batch_ae (Opus render-leaf harvest, author+verify, "
          "central classify): Frida bit-identical GREEN run_diff_warm + "
          "integration build (5/5)")

# rva (lowercased, no-0x form as it appears in col0) -> (reimpl_name, impl_file, branch)
TARGETS = {
    "00492440": ("RenderStatsAccumulate",   "mashedmod/src/mashed_re/Render/RenderLeaves_ae1.cpp", "c3-batch-ae-s1"),
    "004b46b0": ("Vec3Equal",               "mashedmod/src/mashed_re/Render/RenderLeaves_ae1.cpp", "c3-batch-ae-s1"),
    "004b40c0": ("RenderElemArrayCopy",      "mashedmod/src/mashed_re/Render/RenderLeaves_ae2.cpp", "c3-batch-ae-s2"),
    "00478cc0": ("RenderWorldStateZeroFill", "mashedmod/src/mashed_re/Render/RenderLeaves_ae2.cpp", "c3-batch-ae-s2"),
    "004b65e0": ("PizGlobalsZero6",          "mashedmod/src/mashed_re/Render/RenderLeaves_ae3.cpp", "c3-batch-ae-s3"),
}

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

    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line:
            continue
        col0 = line.split(",", 1)[0].strip()
        key = col0[2:] if col0.lower().startswith("0x") else col0
        key = key.lower()
        if key in TARGETS:
            fields = parse_line(line)
            if len(fields) < 9:
                sys.exit(f"ERROR row {i} has {len(fields)} cols: {line!r}")
            if fields[3] != "C2":
                sys.exit(f"ERROR {key} confidence is {fields[3]!r}, expected C2 (idempotency guard)")
            fields[3] = "C3"
            fields[8] = (fields[8] + " " + MARKER).strip() if fields[8] else MARKER.lstrip("| ").strip()
            lines[i] = emit_line(fields) + "\n"
            done.append(key)

    missing = set(TARGETS) - set(done)
    if missing:
        sys.exit(f"ERROR did not find rows for: {sorted(missing)}")

    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} rows C2->C3: {sorted(done)}")

    # CHANGELOG append
    today = "2026-06-04"
    chlog = REPO + r"\re\analysis\CHANGELOG.md"
    with io.open(chlog, "a", encoding="utf-8") as f:
        for key in done:
            name, impl, branch = TARGETS[key]
            f.write(f"{today}  {key}  {name}  C2->C3  {impl}  {branch}\n")
    print(f"CHANGELOG: {len(done)} rows appended")

if __name__ == "__main__":
    main()
