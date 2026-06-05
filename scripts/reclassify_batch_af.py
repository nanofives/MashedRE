#!/usr/bin/env python3
"""Central re-classify for c3_batch_af frida-sweep (C2->C3, frontend menu leaves).

Promotes ONLY the 4 deterministic-arg_type hooks that were reproducibly GREEN
across solo + slots-4 + slots-7 warm-pool integration runs. The 3 live-state
observers (gradient_quad_horiz_alpha / border_quad_four_alpha = draw_quad_observe;
entity_table_select_update = void_setter_observe) FLIPPED GREEN<->RED across runs
(attach-timing-dependent on live RW buffer 0x00898a20 / live entity table
0x00636ac0) -> not clean bit-identity evidence -> HELD AT C2, routed to the
canonical-scenario C3->C4 track. See PROMOTION_QUEUE/CHANGELOG.

Raw-line CSV editor: parses ONLY the target lines via csv (handles quoting),
sets name/confidence/status/file/scenario/frida_diff and appends the evidence
marker to notes. Idempotent: refuses if a target is already C3.
"""
import csv, io, sys

REPO = r"C:\Users\maria\Desktop\Proyectos\Mashed"
MARKER = ("| C2->C3 c3_batch_af (Opus frontend menu-leaf harvest, author+verify, "
          "central classify): Frida bit-identical GREEN run_diff_warm "
          "(deterministic arg_type, reproducible across solo+slots4+slots7) + "
          "integration build clean")

# rva (bare 8-hex as in col0) -> (name, status, file, scenario, frida_diff_csv)
TARGETS = {
    "00494f30": ("AspectRatioSnapshot",   "impl",
                 "mashedmod/src/mashed_re/Frontend/SplashGameMode_t5.cpp",
                 "c3-batch-af-s3", "log/diff_aspect_ratio_snapshot.csv"),
    "0043aee0": ("MenuSlotFlagSetCurrent", "impl",
                 "mashedmod/src/mashed_re/Frontend/MenuLeaves_af4.cpp",
                 "c3-batch-af-s4", "log/diff_menu_slot_flag_set_current.csv"),
    "00431b70": ("MenuFlagDat007f0f10Get", "impl",
                 "mashedmod/src/mashed_re/Frontend/MenuLeaves_af5.cpp",
                 "c3-batch-af-s5", "log/diff_menu_flag_dat_007f0f10_get.csv"),
    "00431f30": ("FrontendPageIdDispatch", "impl",
                 "mashedmod/src/mashed_re/Frontend/MenuLeaves_af6.cpp",
                 "c3-batch-af-s6", "log/diff_frontend_page_id_dispatch.csv"),
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
        if not line or "," not in line or line.startswith("#"):
            continue
        col0 = line.split(",", 1)[0].strip().lower()
        key = col0[2:] if col0.startswith("0x") else col0
        if key in TARGETS:
            fields = parse_line(line)
            if len(fields) < 9:
                sys.exit(f"ERROR row {i} has {len(fields)} cols: {line!r}")
            if fields[3] != "C2":
                sys.exit(f"ERROR {key} confidence is {fields[3]!r}, expected C2 (idempotency guard)")
            name, status, impl, scenario, csvpath = TARGETS[key]
            fields[1] = name
            fields[3] = "C3"
            fields[4] = status
            fields[5] = impl
            fields[6] = scenario
            fields[7] = csvpath
            fields[8] = (fields[8] + " " + MARKER).strip() if fields[8] else MARKER.lstrip("| ").strip()
            lines[i] = emit_line(fields) + "\n"
            done.append(key)

    missing = set(TARGETS) - set(done)
    if missing:
        sys.exit(f"ERROR did not find C2 rows for: {sorted(missing)}")

    with io.open(hooks, "w", encoding="utf-8", newline="") as f:
        f.writelines(lines)
    print(f"hooks.csv: {len(done)} rows C2->C3: {sorted(done)}")

    today = "2026-06-04"
    chlog = REPO + r"\re\analysis\CHANGELOG.md"
    with io.open(chlog, "a", encoding="utf-8") as f:
        for key in done:
            name, status, impl, scenario, csvpath = TARGETS[key]
            f.write(f"{today}  {key}  {name}  C2->C3  {impl}  {scenario}\n")
        # held-at-C2 audit trail
        f.write(f"{today}  00473540  GradientQuadHorizAlpha  HELD-C2  "
                f"draw_quad_observe live-state flip (RED in pool runs) -> canonical C3->C4 track  c3-batch-af-s1\n")
        f.write(f"{today}  004736c0  BorderQuadFourAlpha     HELD-C2  "
                f"draw_quad_observe live-state flip -> canonical C3->C4 track  c3-batch-af-s1\n")
        f.write(f"{today}  00401ee0  EntityTableSelectUpdate HELD-C2  "
                f"void_setter_observe live entity-table flip -> canonical C3->C4 track  c3-batch-af-s4\n")
    print(f"CHANGELOG: {len(done)} promote rows + 3 held-C2 audit rows appended")

if __name__ == "__main__":
    main()
