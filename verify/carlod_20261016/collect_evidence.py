"""carlod: copy the small evidence out of the (uncommitted) log/carlod arms into evidence/.

    py -3.12 collect_evidence.py

- evidence/summary_<arm>.txt   per-arm summary (rows, red, matched draws / triangles, pixels)
- evidence/rowdelta_<a>__<b>.txt   rowdelta.py between the arm pairs the PREREG rules read
- evidence/final_diffs/        the nine cited drawlist diffs of the final default build
- evidence/vislog_cited_rows.txt   per-car d2 / state / shade / coefficient at the nine cited rows
- scores.txt                   one table: rows and pixels per arm and frame
"""
import csv
import json
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
LOG = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\log\carlod")
EV = HERE / "evidence"
ARMS = ["base", "vis1_noglass", "vis", "visb", "visb_off", "visb_env", "venv", "venv_nolag", "visb2", "venv2", "final"]   # on b26d9f3c (pre-rebase)
ARMS4 = ["base4", "visx4", "visoff4", "nolag4", "final4"]                                                          # on 04dee0ed (final tip)
ARMS3 = ["base3", "visx3", "visoff3", "nolag3", "final3"]                                                          # on 9420dde2 (merged tip)
ARMS2 = ["base2", "visx", "visoff", "nolagx", "final2"]                                                            # on 0020d521 (rebased)
PAIRS = [("base4", "visx4"), ("base4", "visoff4"), ("visx4", "final4"), ("final4", "nolag4"), ("base4", "final4"), ("base3", "visx3"), ("base3", "visoff3"), ("visx3", "final3"), ("final3", "nolag3"), ("base3", "final3"), ("base2", "visx"), ("base2", "visoff"), ("visx", "final2"), ("final2", "nolagx"), ("base2", "final2"), ("base", "visb"), ("base", "visb_off"), ("visb", "visb_env"), ("visb", "venv"), ("visb", "visb2"),
         ("visb2", "venv2"), ("venv2", "final"), ("visb_env", "venv_nolag"), ("base", "final")]
FRAMES = [("training", 884), ("training", 924), ("training", 959), ("arctic", 1051), ("arctic", 1091), ("arctic", 1126),
          ("city", 761), ("city", 801), ("city", 836)]


def main():
    EV.mkdir(exist_ok=True)
    for a in ARMS + ARMS2 + ARMS3 + ARMS4:
        s = LOG / a / "summary.txt"
        if s.exists():
            shutil.copy2(s, EV / f"summary_{a}.txt")
    for a, b in PAIRS:
        if (LOG / a).exists() and (LOG / b).exists():
            r = subprocess.run([sys.executable, str(HERE / "rowdelta.py"), a, b], capture_output=True, text=True)
            (EV / f"rowdelta_{a}__{b}.txt").write_text(r.stdout)
    fd = EV / "final_diffs"
    fd.mkdir(exist_ok=True)
    for j in sorted((LOG / "final").glob("diff_*.json")):
        shutil.copy2(j, fd / j.name)
    fd2 = EV / "final_diffs_rebased"
    fd2.mkdir(exist_ok=True)
    for j in sorted((LOG / "final2").glob("diff_*.json")):
        shutil.copy2(j, fd2 / j.name)
    fd3 = EV / "final_diffs_final"
    fd3.mkdir(exist_ok=True)
    for j in sorted((LOG / "final3").glob("diff_*.json")):
        shutil.copy2(j, fd3 / j.name)
    fd4 = EV / "final_diffs_tip"
    fd4.mkdir(exist_ok=True)
    for j in sorted((LOG / "final4").glob("diff_*.json")):
        shutil.copy2(j, fd4 / j.name)
    # scores table
    import re
    pat = re.compile(r"^(\w+)\s+(\d+)\s+rows\s+(\d+)\s+red\s+(\d+).*?pix\s+([\d.]+)")
    cols = [a for a in ARMS if (LOG / a / "summary.txt").exists()]
    table = {}
    for a in cols:
        files = [LOG / a / "summary.txt"]
        if a == "base":
            files.append(LOG / a / "summary_arctic_city.txt")   # the first launch of TRAINING wrote nothing (Amendment 2)
        for f in files:
            if f.exists():
                for ln in f.read_text().splitlines():
                    m = pat.match(ln)
                    if m:
                        table[(a, m.group(1), int(m.group(2)))] = f"{m.group(3)}({m.group(4)})/{m.group(5)}"
    out = ["rows (red) / pixel mean abs diff vs the original, per arm and frame ('-' = no record: the launch wrote none)", ""]
    out.append(f"{'frame':<14}" + "".join(f"{a:>14}" for a in cols))
    for t, k in FRAMES:
        out.append(f"{t + '_' + str(k):<14}" + "".join(f"{table.get((a, t, k), '-'):>14}" for a in cols))
    (HERE / "scores.txt").write_text("\n".join(out) + "\n")
    cols2 = [a for a in ARMS2 if (LOG / a / "summary.txt").exists()]
    for a in cols2:
        for ln in (LOG / a / "summary.txt").read_text().splitlines():
            m = pat.match(ln)
            if m:
                table[(a, m.group(1), int(m.group(2)))] = f"{m.group(3)}({m.group(4)})/{m.group(5)}"
    out2 = ["rebased on 0020d521: rows (red) / pixel mean abs diff vs the original", ""]
    out2.append(f"{'frame':<14}" + "".join(f"{a:>14}" for a in cols2))
    for t_, k_ in FRAMES:
        out2.append(f"{t_ + '_' + str(k_):<14}" + "".join(f"{table.get((a, t_, k_), '-'):>14}" for a in cols2))
    (HERE / "scores_rebased.txt").write_text("\n".join(out2) + "\n")
    cols3 = [a for a in ARMS3 if (LOG / a / "summary.txt").exists()]
    for a in cols3:
        for ln in (LOG / a / "summary.txt").read_text().splitlines():
            m = pat.match(ln)
            if m:
                table[(a, m.group(1), int(m.group(2)))] = f"{m.group(3)}({m.group(4)})/{m.group(5)}"
    out3 = ["merged tip (base 9420dde2): rows (red) / pixel mean abs diff vs the original", ""]
    out3.append(f"{'frame':<14}" + "".join(f"{a:>14}" for a in cols3))
    for t_, k_ in FRAMES:
        out3.append(f"{t_ + '_' + str(k_):<14}" + "".join(f"{table.get((a, t_, k_), '-'):>14}" for a in cols3))
    (HERE / "scores_final.txt").write_text("\n".join(out3) + "\n")
    cols4 = [a for a in ARMS4 if (LOG / a / "summary.txt").exists()]
    for a in cols4:
        for ln in (LOG / a / "summary.txt").read_text().splitlines():
            m = pat.match(ln)
            if m:
                table[(a, m.group(1), int(m.group(2)))] = f"{m.group(3)}({m.group(4)})/{m.group(5)}"
    out4 = ["final tip (base 04dee0ed): rows (red) / pixel mean abs diff vs the original", ""]
    out4.append(f"{'frame':<14}" + "".join(f"{a:>14}" for a in cols4))
    for t_, k_ in FRAMES:
        out4.append(f"{t_ + '_' + str(k_):<14}" + "".join(f"{table.get((a, t_, k_), '-'):>14}" for a in cols4))
    (HERE / "scores_tip.txt").write_text("\n".join(out4) + "\n")
    if (LOG / "base" / "summary_arctic_city.txt").exists():
        shutil.copy2(LOG / "base" / "summary_arctic_city.txt", EV / "summary_base_arctic_city.txt")    # vislog extract
    rows = []
    for key, vl in (("city", "vislog_city.csv"), ("arctic", "vislog_arctic.csv")):
        p = LOG / vl
        if p.exists():
            for r in csv.DictReader(open(p)):
                if (key, int(r["row"])) in FRAMES:
                    rows.append(f"{key:<8}{r['row']:>5} slot {r['slot']} d2raw={float(r['d2raw']):9.3f} state={r['state']} "
                                f"shade={float(r['shade']):.4f} coef={float(r['coef']):.4f} byte={r['coefbyte']}")
    (EV / "vislog_cited_rows.txt").write_text(
        "# standalone, MASHED_CAR_ENV=1, per car at the cited rows. city: exe before the previous-camera change\n"
        "# (coef uses this frame's camera); arctic: exe before the BSP layout fix (shade 1.0, original ~0.25).\n"
        "# d2raw / state are unaffected by either change.\n" + "\n".join(rows) + "\n")
    print("evidence collected:", len(list(EV.glob('summary_*.txt'))), "summaries,", len(list(fd.glob('*.json'))), "diffs")


if __name__ == "__main__":
    main()
