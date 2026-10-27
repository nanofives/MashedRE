"""D4-WSB-C4-BROAD-WS copy (label, data path and summary path changed). D4-WSB-BVH-WRITESET write-set leg (copy of verify/wsbbvh_20261026/run_shadow.py, paths and label changed): one armed shadow A/B boot per RVA, the hook installed alone.

  py -3.12 verify/wsbbroadws_20261027/run_shadow.py <rva> [<rva> ...]

scenario_launch.py --hooks 0x<rva> installs only that hook (MASHED_HOOK_ONLY) and the wrapper runs
original and port on the same input at the real call site; MASHED_SHADOW_AB=1 arms it. The rows run
here use ShadowAB::RunTracked (page-level write tracking + the caller's 4 KB stack window, pre-state
restored between the two runs, Core/ShadowTrack.h) or RunRegion (the output buffer), so the WRITES are
compared, not only the return. Scenario = the census one: track 0, mode 10, 4 cars, 60 s hold.
Needs MASHED_ROOT (private root). Raw per RVA: D:/MashedWT/D4-WSB-C4-BROAD-WS_data/<rva>/shadow_*.
Summary (committed): verify/wsbbroadws_20261027/shadow_summary.tsv. Kills only the PID the launcher prints.
"""
import ctypes, hashlib, os, re, shutil, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
LOCK = WT.parent / "orch-integration" / "re" / "orchestrator" / "mashed_lock.py"
LAUNCH = WT / "re" / "frida" / "scenario_launch.py"
REPORT = WT / "re" / "tools" / "shadow_ab_report.py"
DATA = Path(os.environ.get("BROADWS_DATA", "D:/MashedWT/D4-WSB-C4-BROAD-WS_data"))   # BROAD-WS: env override for pilots
SUMMARY = Path(os.environ["BROADWS_SUMMARY"]) if os.environ.get("BROADWS_SUMMARY") else HERE / "shadow_summary.tsv"
COLS = ["rva", "verdict", "n", "ndiff", "proof", "detail", "pid", "log_sha256", "utc"]


def alive(pid):
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)
    if not h:
        return False
    c = ctypes.c_ulong(); k.GetExitCodeProcess(h, ctypes.byref(c)); k.CloseHandle(h)
    return c.value == 259


def one(rva):
    out = DATA / rva; out.mkdir(parents=True, exist_ok=True)
    glog = Path(os.environ["MASHED_ROOT"]) / "original" / "shadow_ab.log"
    if glog.exists():
        shutil.move(str(glog), str(out / f"shadow_stale_{int(time.time())}.log"))
    env = dict(os.environ); env["MASHED_SHADOW_AB"] = "1"; env.setdefault("MASHED_MUTE", "1")
    cmd = [sys.executable, str(LOCK), "run", "--label", "D4-WSB-C4-BROAD-WS", "--", sys.executable, str(LAUNCH),
           "--track", "0", "--mode", "10", "--cars", "4", "--hold", "60", "--hooks", "0x" + rva]
    p = subprocess.run(cmd, cwd=str(WT), env=env, capture_output=True, text=True, errors="replace", timeout=900)
    txt = p.stdout + p.stderr
    (out / "shadow_transcript.txt").write_text(txt, encoding="utf-8")
    m = re.search(r"scenario_launch\s+pid=(\d+)", txt)
    pid = int(m.group(1)) if m else 0
    if pid and alive(pid):
        subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True); time.sleep(1.0)
    log = out / "shadow_ab.log"
    for _ in range(5):
        try:
            if glog.exists(): shutil.move(str(glog), str(log))
            break
        except PermissionError:
            time.sleep(1.0)
    row = {"rva": "0x" + rva, "verdict": "NO_LOG", "n": "", "ndiff": "", "proof": "", "detail": "", "pid": str(pid),
           "log_sha256": hashlib.sha256(log.read_bytes()).hexdigest() if log.exists() else "",
           "utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}
    if log.exists():
        tsv = out / "shadow_report.tsv"
        subprocess.run([sys.executable, str(REPORT), str(log), "--only", "0x" + rva, "--tsv", str(tsv), "--quiet"],
                       cwd=str(WT), capture_output=True, text=True)
        if tsv.exists():
            for ln in tsv.read_text(encoding="utf-8").splitlines()[1:]:
                f = ln.split("\t")
                if f and f[0].lower().replace("0x", "").zfill(8) == rva:
                    row.update(verdict=f[2], n=f[3], ndiff=f[4], proof=f[5], detail=(f[6] if len(f) > 6 else "")[:200])
    return row


def main():
    rvas = [a.lower().replace("0x", "").zfill(8) for a in sys.argv[1:]]
    rows = {}
    if SUMMARY.exists():
        for ln in SUMMARY.read_text(encoding="utf-8").splitlines()[1:]:
            rows[ln.split("\t")[0]] = ln.split("\t")
    for i, rva in enumerate(rvas, 1):
        r = one(rva)
        rows[r["rva"]] = [r[c] for c in COLS]
        print(f"[{i}/{len(rvas)}] {r['rva']} {r['verdict']} n={r['n']} ndiff={r['ndiff']} proof={r['proof']} pid={r['pid']}", flush=True)
        SUMMARY.write_text("\t".join(COLS) + "\n" + "".join("\t".join(rows[k]) + "\n" for k in sorted(rows)), encoding="utf-8")


if __name__ == "__main__":
    sys.exit(main())
