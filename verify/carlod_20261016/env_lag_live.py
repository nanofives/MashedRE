"""carlod: which camera row reproduces the original's LIVE env coefficient (rec+0x274)?

Input: rtex's --matfx captures (log/rtex/orig_matfx2_<track>/trace.csv.matfx.json, every 100 trace rows;
per car: coef, shade rec+0x284, cached part-0x13 position rec+0x258, camera frame position) and the same
run's trace (M_px..M_pz per row, camera modelling matrix position). FUN_00420290 @ 0x00420290:
coef = ((hi - lo) * shade + lo) * (1 - clamp(d2, 0, 100) / 100), d2 = |rec+0x258 - camera|^2
(FUN_0041f120). The snapshot's camera position is the probe-time pose (trace row r); the question is
whether the coefficient was computed with row r's camera or row r-1's. Rows where the coefficient is 0
(d2 >= 100) say nothing and are skipped.

    py -3.12 env_lag_live.py
"""
import csv
import json

BASE = r"C:\Users\maria\Desktop\Proyectos\Mashed\log\rtex\orig_matfx2_%s\trace.%s"


def main():
    for tr in ("training", "arctic", "city"):
        snaps = json.load(open(BASE % (tr, "csv.matfx.json")))
        trace = list(csv.DictReader(open(BASE % (tr, "csv"))))
        for sn in snaps:
            if "s" not in sn:
                continue
            g = sn["s"]
            lo, hi = g["gloss_35c"], g["gloss_360"]
            r = sn["row"]
            for ci, c in enumerate(g["cars"]):
                pos, camp = c.get("pos258"), c.get("p29c_pos")
                if not pos or not camp or c["coef"] <= 0.0:
                    continue
                res = {}
                for name, rr in (("K", r), ("K-1", r - 1), ("K-2", r - 2)):
                    cam = [float(trace[rr][f"M_p{a}"]) for a in "xyz"]
                    d2 = sum((a - b) ** 2 for a, b in zip(pos, cam))
                    res[name] = ((hi - lo) * c["f284"] + lo) * (1 - min(max(d2, 0.0), 100.0) / 100.0)
                print("%-8s row %4d car%d live=%.5f  K=%.5f (%+.5f)  K-1=%.5f (%+.5f)  K-2=%.5f (%+.5f)  probe-cam-vs-traceK=%.4f" % (
                    tr, r, ci, c["coef"], res["K"], res["K"] - c["coef"], res["K-1"], res["K-1"] - c["coef"],
                    res["K-2"], res["K-2"] - c["coef"],
                    max(abs(camp[i] - float(trace[r][f"M_p{'xyz'[i]}"])) for i in range(3))))


if __name__ == "__main__":
    main()
