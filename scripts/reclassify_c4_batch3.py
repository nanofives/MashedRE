#!/usr/bin/env python3
"""Third C3->C4 batch via the idle canonical harness — 4 boot/render/util getters.

All 0-arg getters with real reimpls + prior C3 full-domain force-call GREEN; exercised
during boot-to-menu + installed (0xE9) + survived. Notes:
  - 004cd060 AllocatorSlotGet: U-3740 is a catalogued DATA-SEMANTIC uncertainty
    (UNCERTAINTIES.md, type=semantic) -> non-blocking for C4 of a bit-identical leaf
    (F-DoD #3: promoted to UNCERTAINTIES.md with path-to-resolution).
  - 0040e340 GetLiveCarCount: status was stale 'mapped' though the reimpl exists +
    is registered (installed 0xE9); fixed to 'impl' and name FUN_0040e340->GetLiveCarCount.
Idempotent (expects C3).
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SCEN = "boot_to_menu_install_observe_2026-06-06"
# rva -> (name, exercise_count, status, impl_file)
TARGETS = {
    "00494ef0": ("ThunkVideoStateGet", 8150, "impl", "mashedmod/src/mashed_re/Boot/VideoConfig.cpp"),
    "004cd060": ("AllocatorSlotGet",   1,    "impl", "mashedmod/src/mashed_re/Render/RwPluginHelpers_o3.cpp"),
    "0042b8d0": ("StatePhaseIsIdle",   2755, "impl", "mashedmod/src/mashed_re/Util/StateAccessors.cpp"),
    "0040e340": ("GetLiveCarCount",    2755, "impl", "mashedmod/src/mashed_re/Util/UtilLeaves.cpp"),
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
            if fields[3] != "C3":
                sys.exit(f"{key} is {fields[3]!r}, expected C3 (idempotency)")
            name, n, status, impl = TARGETS[key]
            marker = (f"| C3->C4 (canonical install-observe 2026-06-06): inline-JMP LIVE (0xE9) + "
                      f"exercised {n}x during boot-to-menu (canonical_c4_verify OFF-count) + booted "
                      f"to menu & survived with hook installed + C3 0-arg getter full-domain GREEN.")
            fields[1] = name; fields[3] = "C4"; fields[4] = status; fields[5] = impl
            fields[6] = SCEN; fields[7] = "log/c4_verify_result.json"
            fields[8] = (fields[8] + " " + marker).strip()
            lines[i] = emit(fields) + "\n"; done.append(key)
    missing = set(TARGETS) - set(done)
    if missing: sys.exit(f"not found as C3: {sorted(missing)}")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C3->C4: {sorted(done)}")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-06  {k}  {TARGETS[k][0]}  C3->C4  {TARGETS[k][3]}  {SCEN} ({TARGETS[k][1]}x)\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
