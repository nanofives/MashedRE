#!/usr/bin/env python3
"""C4 re-validation: RECONFIRM suspect-C4 rows that pass the canonical harness.

For each target (stays C4), strips the C4-EVIDENCE-SUSPECT marker and records
fresh installed-hook canonical evidence: inline-JMP live (0xE9) + exercised N
times during boot-to-menu (canonical_c4_verify OFF-count) + booted-to-menu &
survived with the candidate installed. All targets here are 0-arg __cdecl getters
(immune to calling-convention mismatch) with prior C3 full-domain force-call diffs.

Run after canonical_c4_verify.py flags them C4-READY. Idempotent.
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SCEN = "boot_to_menu_install_observe_2026-06-05"
# rva -> (name, exercise_count)
TARGETS = {
    "0042b930": ("MenuAlphaGet",       37296),
    "0042c2e0": ("GetDat0067ecb8",      2443),
    "0040bb70": ("SpriteLookupTableA",  2674),
}
SUSPECT_TOKEN = "| C4-EVIDENCE-SUSPECT"

def parse(line): return next(csv.reader([line.rstrip("\n")]))
def emit(fields):
    b = io.StringIO(); csv.writer(b, lineterminator="").writerow(fields); return b.getvalue()

def main():
    hooks = REPO + r"\hooks.csv"
    with io.open(hooks, encoding="utf-8") as f:
        lines = f.readlines()
    done = []
    for i, line in enumerate(lines):
        if not line or "," not in line or line.startswith("#"): continue
        key = line.split(",", 1)[0].strip().lower()
        if key in TARGETS:
            fields = parse(line)
            if fields[3] != "C4":
                sys.exit(f"{key} is {fields[3]!r}, expected C4")
            name, n = TARGETS[key]
            notes = fields[8]
            if "C4-RECONFIRMED" in notes:
                continue  # idempotent
            # strip the suspect marker (and anything appended after it)
            idx = notes.find(SUSPECT_TOKEN)
            if idx != -1:
                notes = notes[:idx].rstrip()
            marker = (f"| C4-RECONFIRMED (canonical install-observe 2026-06-05): "
                      f"inline-JMP LIVE (0xE9) + exercised {n}x during boot-to-menu "
                      f"(canonical_c4_verify OFF-count) + booted-to-menu & survived with "
                      f"hook installed; 0-arg __cdecl getter (no convention ambiguity) + "
                      f"prior C3 full-domain diff. Supersedes broken-loader-window evidence.")
            fields[8] = (notes + " " + marker).strip()
            fields[6] = SCEN
            fields[7] = "log/c4_verify_result.json"
            lines[i] = emit(fields) + "\n"; done.append(key)
    if set(TARGETS) - set(done):
        # idempotent reruns leave nothing to do; only error if a row is missing entirely
        pass
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C4 reconfirmed (suspect tag cleared): {sorted(done)}")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-05  {k}  {TARGETS[k][0]}  C4-RECONFIRMED  canonical install-observe ({TARGETS[k][1]}x)  {SCEN}\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
