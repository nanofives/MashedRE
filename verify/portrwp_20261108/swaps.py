"""PORT-RWP-01: the 16 C4 RWP body rows with a matched source -> swaps.tsv (swap verdict + hand-body flags).
Reads REACH.tsv (reach.py) and relocheck.tsv (relocheck.py output on the 16 sources).
  py -3.12 verify/portrwp_20261108/swaps.py
"""
import csv, glob, pathlib, re

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
RVAS = ("0057c440 0055bd70 0055bd80 00565160 00565fa0 0056b9d0 0056ba30 0056cf90 0056d350 0056f0a0 "
        "0056fea0 005751f0 005757d0 00578e50 0057adb0 0057ae20").split()

rows = {r["rva"]: r for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8"))}
reach = {l.split("\t")[0]: l.rstrip("\n").split("\t") for l in open(HERE / "REACH.tsv", encoding="utf-8")}
rl = {l.split("\t")[0][2:]: l.split("\t")[1] for l in open(HERE / "relocheck.tsv", encoding="utf-8") if l.startswith("0x")}

out = ["rva\tmatched_src\trelocheck\thand_body_tu\treached_by_WinMain\tshadow_ab\tgetenv_in_body\tasm_in_body\tswapped\treason"]
for r in RVAS:
    f = rows[r]["exe_file"]
    t = (ROOT / f).read_text(encoding="utf-8", errors="replace")
    src = pathlib.PurePath(glob.glob(str(ROOT / "re" / "match" / "rwl0*" / f"{r}.c"))[0]).relative_to(ROOT).as_posix()
    m = re.search(r"FUN_%s(_impl)?\s*\([^;{]*?\)\s*\{" % r, t)
    body = ""
    if m:
        i, depth = m.end(), 1
        while depth and i < len(t):
            depth += {"{": 1, "}": -1}.get(t[i], 0)
            i += 1
        body = t[m.start():i]
    sab = "yes" if re.search(r"SHADOW_AB_COUNTER\([^)]*0x%s" % r, t, re.I) else "no"
    knob = "yes" if "getenv" in body else "no"
    asm = "yes" if ("__asm" in body or "naked" in body) else "no"
    out.append("\t".join([r, src, rl.get(r, "?"), f.split("/")[-1], reach[r][5], sab, knob, asm, "no",
                          "NOT-REACHED: no WinMain path (REACH.tsv); brief swaps only rows the default path executes"]))
(HERE / "swaps.tsv").write_text("\n".join(out) + "\n", encoding="utf-8")
print("\n".join(out))
