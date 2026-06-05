#!/usr/bin/env python3
"""First C3->C4 promotion via the validated subset-install canonical harness.

Promotes ONLY 0x00493f70 VideoStateFlagGet. Rubric-clean C4 evidence (no overclaim):
  - installed: inline-JMP live (first byte 0xE9 read at the RVA during the run)
  - exercised: 8583 calls during boot-to-menu (canonical_c4_verify.py OFF-run
    Interceptor count -> proves it is ON the canonical path, not dead code)
  - no regression: ON-run installed ONLY this candidate, MASHED booted to the real
    menu and survived (alive, no crash)
  - output identity: prior C3 read_global force-call A/B 10/10 GREEN over the full
    sentinel domain (a `return DAT_00771a04` leaf cannot diverge on any canonical
    input the C3 diff did not already cover)
  - F-DoD: RVA pinned; C3+; NO [UNCERTAIN] in note; no stubs (leaf); canonical diff.

HELD (not promoted), recorded for the audit trail:
  - 0x00493fc0 AspectRatioGlobalGet: canonical run GREEN (5816 calls, installed,
    survived) BUT U-0814 (possible calling-convention misread, behavioral) is
    unresolved -> F-DoD #3 blocks C4 until resolved. Survival across 5816 calls is
    supporting evidence the convention is __cdecl-compatible, not proof.
  - 6 getters exercised 0x at the IDLE main menu (0x00431b70/00430760/00425ef0/
    00431d80/004241b0/00426bc0): installed+survived but not on the boot-to-menu-idle
    path -> need a menu-navigate / in-game canonical scenario. Stay C3.
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SCEN = "boot_to_menu_install_observe_2026-06-05"
MARKER = ("| C3->C4 (canonical install-observe 2026-06-05): inline-JMP LIVE (0xE9) + "
          "exercised 8583x during boot-to-menu (canonical_c4_verify OFF-count) + booted "
          "to menu & survived with ONLY this hook installed + C3 read_global full-domain "
          "10/10 GREEN. First C4 of the validated subset-install harness.")

TARGETS = {
    "00493f70": ("VideoStateFlagGet",
                 "mashedmod/src/mashed_re/Frontend/SplashGameMode_t5.cpp",
                 "log/c4_verify_result.json"),
}

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
            if len(fields) < 9: sys.exit(f"row {i}: {len(fields)} cols")
            if fields[3] != "C3": sys.exit(f"{key} is {fields[3]!r}, expected C3 (idempotency)")
            name, impl, ev = TARGETS[key]
            fields[3] = "C4"; fields[6] = SCEN; fields[7] = ev
            fields[8] = (fields[8] + " " + MARKER).strip()
            lines[i] = emit(fields) + "\n"; done.append(key)
    missing = set(TARGETS) - set(done)
    if missing: sys.exit(f"not found as C3: {sorted(missing)}")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C3->C4: {sorted(done)}")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-05  {k}  {TARGETS[k][0]}  C3->C4  {TARGETS[k][1]}  {SCEN}\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
