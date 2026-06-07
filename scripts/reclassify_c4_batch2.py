#!/usr/bin/env python3
"""Second C3->C4 batch via the idle canonical harness — 5 video/display/RW getters.

All are 0-arg uint32(void) getters (read_global / none arg_type, status=impl) — the
same clean class as VideoStateFlagGet/MenuAlphaGet: 0 args => no calling-convention
ambiguity, trivial output identity. Rubric-clean C4:
  - installed: inline-JMP live (0xE9) in the ON run
  - exercised: called during boot-to-menu (canonical_c4_verify OFF-count, per RVA below)
  - no regression: booted to menu & survived with ONLY these installed
  - output identity: prior C3 force-call full-domain GREEN (0-arg getter)
Idempotent (expects C3).
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
SCEN = "boot_to_menu_install_observe_2026-06-06"
# rva -> (name, exercise_count, impl_file)
TARGETS = {
    "00498bc0": ("VideoGetRenderWidth",    3,    "mashedmod/src/mashed_re/Boot/VideoConfig.cpp"),
    "00498bd0": ("VideoGetRenderHeight",   3,    "mashedmod/src/mashed_re/Boot/VideoConfig.cpp"),
    "00498bf0": ("DisplayActiveFlagGet",   1850, "mashedmod/src/mashed_re/Boot/FrameDispatch.cpp"),
    "004c2d70": ("RwPluginRegistryFrozen", 84,   "mashedmod/src/mashed_re/Render/RwPluginHelpers_o3.cpp"),
    "004c2f00": ("RwEngineGetCurrentMode", 4,    "mashedmod/src/mashed_re/Boot/VideoConfig.cpp"),
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
            name, n, impl = TARGETS[key]
            marker = (f"| C3->C4 (canonical install-observe 2026-06-06): inline-JMP LIVE (0xE9) + "
                      f"exercised {n}x during boot-to-menu (canonical_c4_verify OFF-count) + booted "
                      f"to menu & survived with hook installed + C3 0-arg getter full-domain GREEN.")
            fields[3] = "C4"; fields[6] = SCEN; fields[7] = "log/c4_verify_result.json"
            fields[8] = (fields[8] + " " + marker).strip()
            lines[i] = emit(fields) + "\n"; done.append(key)
    missing = set(TARGETS) - set(done)
    if missing: sys.exit(f"not found as C3: {sorted(missing)}")
    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} C3->C4: {sorted(done)}")
    with io.open(REPO + r"\re\analysis\CHANGELOG.md", "a", encoding="utf-8") as f:
        for k in done:
            f.write(f"2026-06-06  {k}  {TARGETS[k][0]}  C3->C4  {TARGETS[k][2]}  {SCEN} ({TARGETS[k][1]}x)\n")
    print("CHANGELOG: appended")

if __name__ == "__main__":
    main()
