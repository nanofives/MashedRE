#!/usr/bin/env python3
"""C4 re-validation outcome: demote 22 non-reimplemented "C4" rows -> C2.

These 22 suspect-C4 rows are NOT registered hooks — they have no active
RH_ScopedInstall in mashedmod/src (either none at all, or one that is
commented-out / MASS-DISABLED). Per re/CONFIDENCE.md, C3 *requires* "a
reimplementation has been written ... hooked through RH_ScopedInstall and
runtime-toggleable", and C4 requires C3 + a canonical diff. With no installable
reimpl there is no "modded" behavior to diff — the prior C4 came purely from
mass_canonical_observe.py Interceptor-OBSERVING the ORIGINAL during boot, which
is the overclaim the C4-EVIDENCE-SUSPECT tagging flagged. Cross-checked against
the registered-hook manifest from canonical_c4_verify.py (log/hook_install_manifest.txt,
477 hooks): none of these 22 appear.

Correct level = C2 (decomp read & mechanically transcribed; the analysis notes
exist) — NOT C3/C4 (no reimpl) and not C1 (more than located). Reversible via git.
The 78 suspects that ARE registered hooks are re-validated separately through the
canonical harness (exercised + installed + survived), not here.
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
MARKER = ("| DEMOTED C4->C2 (re-validation 2026-06-05): NOT a registered hook — no "
          "active RH_ScopedInstall reimpl (absent or MASS-DISABLED) -> fails C3 rubric "
          "'reimplementation written + hooked'. Prior C4 was mass_canonical_observe "
          "Interceptor-observation of the ORIGINAL only (no modded behavior to diff). "
          "Decomp-read analysis note exists -> C2. Needs a reimpl to re-earn C3/C4.")

# 22 non-registered suspect-C4 RVAs (suspect-C4 AND absent from the registered-hook manifest)
TARGETS = [
    "004a4bb7","00492370","004a8a04","004aa3fe","004ac04a","00402750","00493900",
    "00499ba0","004a774d","004aa3e4","004b6540","004b6560","004b6610","00494f20",
    "00499730","004926c0","00493710","004c2fb0","004c2c90","004cc7f0","004950b0","00493480",
]

def parse(line): return next(csv.reader([line.rstrip("\n")]))
def emit(fields):
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(fields); return b.getvalue()

def main():
    hooks = REPO + r"\hooks.csv"
    tset = set(t.lower() for t in TARGETS)
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()
    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"): continue
        key = line.split(",", 1)[0].strip().lower()
        if key in tset:
            fields = parse(line)
            if len(fields) < 9: sys.exit(f"row {i}: {len(fields)} cols")
            if fields[3] == "C2":
                continue  # idempotent
            if fields[3] != "C4":
                sys.exit(f"{key} is {fields[3]!r}, expected C4")
            fields[3] = "C2"
            fields[8] = (fields[8] + " " + MARKER).strip()
            lines[i] = emit(fields) + "\n"; done.append(key)
    missing = tset - set(done)
    if missing:
        # allow already-demoted (idempotent reruns); only fail if a target row is absent entirely
        present = set()
        for line in lines:
            if line and "," in line and not line.startswith("#"):
                present.add(line.split(",",1)[0].strip().lower())
        truly_missing = missing - present
        if truly_missing:
            sys.exit(f"rows not found at all: {sorted(truly_missing)}")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C4->C2 demoted")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-05  {k}  C4->C2  re-validation: no installable reimpl (not a registered hook)\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
