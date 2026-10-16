"""Evidence housekeeping before commit: fix one RESULT reference, drop the MB-sized G-CTRL traces (their sha8 lines are
the evidence, in gctrl_after.log), drop the compile probe's .obj, trim the build logs to their summary + tail."""
import pathlib
import shutil

H = pathlib.Path(__file__).resolve().parent
r = H / "RESULT_dupinstall2.md"
t = r.read_text(encoding="utf-8")
old = "(`nav_probe_*` lines in `bisect_logs/`, `win2/nav_probe_*.txt`.)"
assert old in t
r.write_text(t.replace(old, "(`nav_probe_variants.txt`, four spawn variants, re-run for the record.)"), encoding="utf-8")
if (H / "gctrl").exists():
    shutil.rmtree(H / "gctrl")
(H / "asm_probe" / "probe.obj").unlink(missing_ok=True)
for f in ("build_prime.log", "build_after.log"):
    p = H / f
    x = p.read_text(encoding="utf-8", errors="replace").splitlines()
    if x and not x[0].startswith("[trimmed"):
        w = sum("warning" in l for l in x)
        e = sum(" error " in l for l in x)
        p.write_text("\n".join([f"[trimmed: {len(x)} lines, {w} warning lines, {e} error lines; last 6 kept]"] + x[-6:]) + "\n",
                     encoding="utf-8")
print("total KB:", sum(f.stat().st_size for f in H.rglob("*") if f.is_file()) // 1024)
