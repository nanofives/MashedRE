"""r10: original-side render-site backtraces for the textures of RED rows (camera_probe.py --drawrec-bt).

    py -3.12 verify\r10_20261016\bt_capture.py [--tracks 1,2,...] [--regex REGEX] [--attempts 3]

Writes verify/r10_20261016/bt/<NN_Track>/ (trace.csv, static.json, orig_row_<K>.draws.json with a "bt" return-address
chain per matching draw). A NEW original run per track: the poses are not the capfix ones, so only the render SITES
(MASHED.exe VAs) are used from here, never the draw counts. Absolute paths throughout (capfix cause).
Does NOT touch verify/capfix_20261016 (its run_capture.py wipes the output directory on every attempt).
"""
import argparse, glob, json, re, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
PROBE = ROOT / "re" / "frida" / "camera_probe.py"
TRACKS = {1: "01_Egypt", 2: "02_Neustein", 4: "04_Highway", 5: "05_Sands", 6: "06_SuperG", 7: "07_Roundabout",
          8: "08_Storm", 9: "09_Forest", 10: "10_Dump", 11: "11_Warzone"}


def red_textures(prefix="diff0_"):
    names = set()
    for f in glob.glob(str(HERE / "[0-9][0-9]_*" / f"{prefix}[0-9]*.json")):
        for r in json.loads(Path(f).read_text())["rows"]:
            if "cite" not in r and r["kind"] in ("MISSING", "MOVED", "TEX1") and r["tex"] not in ("-", "*"):
                names.add(r["tex"])
    return sorted(names)


def image_name(pid):
    r = subprocess.run(["tasklist", "/FI", f"PID eq {pid}", "/FO", "CSV", "/NH"], capture_output=True, text=True)
    m = re.match(r'"([^"]+)"', r.stdout.strip())
    return m.group(1) if m else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tracks", default=",".join(map(str, TRACKS)))
    ap.add_argument("--regex", default="")
    ap.add_argument("--attempts", type=int, default=3)
    ap.add_argument("--out", default=str(HERE / "bt"))
    a = ap.parse_args()
    rx = a.regex or "^(" + "|".join(re.escape(n) for n in red_textures()) + ")$"
    out_root = Path(a.out).resolve()
    (out_root).mkdir(parents=True, exist_ok=True)
    (out_root / "regex.txt").write_text(rx)
    res = {}
    for t in [int(x) for x in a.tracks.split(",")]:
        d = out_root / TRACKS[t]
        d.mkdir(parents=True, exist_ok=True)
        ok = False
        for att in range(1, a.attempts + 1):
            cmd = [sys.executable, str(PROBE), "--frame", "--early", "--shots-race", "30,70,105", "--drawrec",
                   "--carframes", "--track", str(t), "--rows", "1600", "--drawrec-bt", rx,
                   "--out", str(d / "trace.csv"), "--static-out", str(d / "static.json")]
            logf = d / "stdout.txt"
            with open(logf, "w", encoding="utf-8", errors="replace") as lf:
                p = subprocess.Popen(cmd, cwd=str(ROOT), stdout=lf, stderr=subprocess.STDOUT)
                try:
                    rc = p.wait(timeout=420)
                except subprocess.TimeoutExpired:
                    p.kill(); rc = "timeout"
            text = logf.read_text(encoding="utf-8", errors="replace")
            m = re.search(r"spawned pid=(\d+)", text)
            pid = int(m.group(1)) if m else None
            if pid and (image_name(pid) or "").lower() == "mashed.exe":
                subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True)
            shots = sorted(d.glob("orig_row_*.draws.json"))
            ok = rc == 0 and len(shots) == 3 and "SHOT CAPTURE FAILED" not in text
            line = f"{time.strftime('%H:%M:%S')} {TRACKS[t]} attempt {att} rc={rc} pid={pid} shots={len(shots)} {'OK' if ok else 'FAIL'}"
            print(line, flush=True)
            if ok:
                break
        res[TRACKS[t]] = ok
    print("RESULT", res)
    return 0 if all(res.values()) else 1


if __name__ == "__main__":
    sys.exit(main())
