#!/usr/bin/env python3
r"""xtwin.py — show the Xbox-twin decompilation of a PC MASHED.exe function.

Looks up the PC VA in re/console/match/xbuild_match_v2.csv, then decompiles
the matched toast.exe function from the Mashed_Console headless project.
Use when a PC function decompiles badly — the Xbox compile of the same source
often reads cleaner.

Usage:
  py -3.12 re\tools\console\xtwin.py 0x0041e870

Notes:
  - one-shot analyzeHeadless run (~30-60 s).
  - the Mashed_Console project is single-writer: don't run while another
    xtwin/ApplyTwinNames invocation is live.
"""
import csv
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]  # repo root (re/tools/console -> root)
GH = r"C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra_12.0.3_PUBLIC\support\analyzeHeadless.bat"
MATCH = ROOT / "re/console/match/xbuild_match_v2.csv"
PROJ_DIR = ROOT / "re/console/ghidra"
SCRIPTS = ROOT / "re/tools/console/ghidra_scripts"


def main():
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    pc_va = int(sys.argv[1], 16)

    row = None
    with open(MATCH, newline="") as f:
        for r in csv.DictReader(f):
            if int(r["pc_va"], 16) == pc_va:
                row = r
                break
    if row is None:
        raise SystemExit(f"0x{pc_va:08x}: no Xbox match in {MATCH.name} "
                         f"(platform-layer candidate, or below match tiers)")

    print(f"PC 0x{pc_va:08x} ({row['pc_name'] or 'unnamed'}, "
          f"{row['subsystem'] or '?'}) -> Xbox {row['xbox_va']} "
          f"[tier={row['tier']}]")
    if row["tier"] == "prop-weak":
        print("WARNING: prop-weak tier is candidate-grade — "
              "verify the pair before trusting details.\n")

    out = Path(tempfile.gettempdir()) / f"xtwin_{pc_va:08x}.c"
    res = subprocess.run(
        [GH, str(PROJ_DIR), "Mashed_Console",
         "-process", "toast_flat.bin", "-noanalysis", "-readOnly",
         "-postScript", "DecompTwin.java", row["xbox_va"], str(out),
         "-scriptPath", str(SCRIPTS)],
        capture_output=True, text=True)
    if not out.exists():
        sys.stderr.write(res.stdout[-2000:] + res.stderr[-2000:])
        raise SystemExit("decompile run produced no output")
    print(out.read_text())


if __name__ == "__main__":
    main()
