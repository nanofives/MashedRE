"""Before the commit: delete the MB-sized G-CTRL traces (the sha8 lines in gctrl_*.log are the evidence, the exes that
made them are reproducible from the tree) and trim the raw build logs to a header plus the last lines.

    py -3.12 finalize_evidence.py
"""
import pathlib
import re
import shutil

HERE = pathlib.Path(__file__).resolve().parent

g = HERE / "gctrl"
if g.exists():
    n = sum(1 for _ in g.iterdir())
    shutil.rmtree(g)
    print(f"removed gctrl/ ({n} trace files)")

for raw in sorted(HERE.glob("build_*_raw.log")):
    x = raw.read_text(encoding="utf-8", errors="replace").splitlines()
    w = sum(1 for ln in x if " warning " in ln)
    e = sum(1 for ln in x if re.search(r"\berror\b|error C\d+|LNK\d+", ln))
    keep = [ln for ln in x if re.search(r"rva-lint|asm-lint|Build OK|^exit=|MASHED_NO_DEPLOY", ln)]
    out = raw.with_name(raw.name.replace("_raw", ""))
    out.write_text("\n".join([f"[trimmed from {raw.name}: {len(x)} lines, {w} warning lines, {e} error lines; summary lines kept]"] + keep) + "\n",
                   encoding="utf-8")
    raw.unlink()
    print(f"{raw.name} -> {out.name}  ({len(x)} lines, {w} warnings, {e} errors)")
