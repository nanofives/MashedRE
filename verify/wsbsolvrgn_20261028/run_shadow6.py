"""D4-WSB-C4-SOLV-RGN region shadow leg (copy of verify/wsbsolvws_20261027/run_shadow4.py, data path and label changed; that file is a copy of verify/wsbshadowpriv_20261027/run_shadow3.py, data path and label changed; that file is a copy of verify/wsbbvhfu_20261027/run_shadow2.py, paths and label changed;
SHADOW_TAG=<sfx> appends _<sfx> to the row key and the raw folder, e.g. SHADOW_TAG=stw with MASHED_SHADOW_STW=1): one armed shadow A/B boot per RVA, the hook installed alone.

  py -3.12 verify/wsbbvh_20261027/run_shadow.py <rva> [<rva> ...]

scenario_launch.py --hooks 0x<rva> installs only that hook (MASHED_HOOK_ONLY) and the wrapper runs
original and port on the same input at the real call site; MASHED_SHADOW_AB=1 arms it. The rows run
here use ShadowAB::RunTracked (page-level write tracking + the caller's 4 KB stack window, pre-state
restored between the two runs, Core/ShadowTrack.h) or RunRegion (the output buffer), so the WRITES are
compared, not only the return. Scenario = the census one: track 0, mode 10, 4 cars, 60 s hold.
Needs MASHED_ROOT (private root). Raw per RVA: D:/MashedWT/D4-WSB-C4-SOLV-RGN_data/<rva>/shadow_*.
Summary (committed): verify/wsbsolvrgn_20261028/shadow_summary.tsv. Kills only the PID the launcher prints.
"""
import ctypes, hashlib, os, re, shutil, subprocess, sys, threading, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
WT = HERE.parent.parent
LOCK = WT.parent / "orch-integration" / "re" / "orchestrator" / "mashed_lock.py"
LAUNCH = WT / "re" / "frida" / "scenario_launch.py"
REPORT = WT / "re" / "tools" / "shadow_ab_report.py"
DATA = Path("D:/MashedWT/D4-WSB-C4-SOLV-RGN_data")
SUMMARY = HERE / "shadow_summary.tsv"
COLS = ["rva", "verdict", "n", "ndiff", "proof", "detail", "pid", "log_sha256", "utc"]


def alive(pid):
    k = ctypes.windll.kernel32
    h = k.OpenProcess(0x1000, False, pid)
    if not h:
        return False
    c = ctypes.c_ulong(); k.GetExitCodeProcess(h, ctypes.byref(c)); k.CloseHandle(h)
    return c.value == 259


def one(rva):
    tag = ("_" + os.environ["SHADOW_TAG"]) if os.environ.get("SHADOW_TAG") else ""
    out = DATA / (rva + tag); out.mkdir(parents=True, exist_ok=True)
    glog = Path(os.environ["MASHED_ROOT"]) / "original" / "shadow_ab.log"
    if glog.exists():
        shutil.move(str(glog), str(out / f"shadow_stale_{int(time.time())}.log"))
    env = dict(os.environ); env["MASHED_SHADOW_AB"] = "1"; env.setdefault("MASHED_MUTE", "1")
    env["PYTHONUNBUFFERED"] = "1"   # the launcher's pid line must arrive live through the lock wrapper (guards)
    cmd = [sys.executable, str(LOCK), "run", "--label", "D4-WSB-C4-SOLV-RGN", "--", sys.executable, str(LAUNCH),
           "--track", "0", "--mode", "10", "--cars", "4", "--hold", os.environ.get("HOLD", "60"), "--hooks", "0x" + rva]   # HOLD: addendum 1
    # Guard (added before arm P): the game PID the launcher prints is killed once the target row has
    # SAMPLES_DONE samples in the live log, or when the log has not grown for STALL_S seconds (a post-sample
    # STW deadlock held the machine lock for 30 min in leg S). Only that PID is ever killed.
    done_n = int(os.environ.get("SAMPLES_DONE", "24")); stall_s = int(os.environ.get("STALL_S", "300"))
    lines, box = [], {"pid": 0}
    p = subprocess.Popen(cmd, cwd=str(WT), env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                         text=True, errors="replace")
    def drain():
        for ln in p.stdout:
            lines.append(ln)
            mm = re.search(r"scenario_launch\s+pid=(\d+)", ln)
            if mm and not box["pid"]: box["pid"] = int(mm.group(1))
    th = threading.Thread(target=drain, daemon=True); th.start()
    nolog_s = int(os.environ.get("NOLOG_S", "360"))   # no shadow log this long after the PID appeared
    t0 = time.time(); last_size, last_grow, why, pid_at = -1, time.time(), "", 0.0
    srx = re.compile(r"^\[\d+\] fn=\S+ rva=" + rva + r" ", re.M)
    while p.poll() is None:
        time.sleep(2.0)
        if box["pid"] and not pid_at: pid_at = time.time()
        if time.time() - t0 > 1800: why = "TIMEOUT"
        elif pid_at and not glog.exists() and time.time() - pid_at > nolog_s: why = "NO-LOG"
        elif box["pid"] and glog.exists():
            sz = glog.stat().st_size
            if sz != last_size: last_size, last_grow = sz, time.time()
            if len(srx.findall(glog.read_text(encoding="utf-8", errors="replace"))) >= done_n: why = "SAMPLES-DONE"
            elif time.time() - last_grow > stall_s: why = "STALL"
        if why:
            if box["pid"] and alive(box["pid"]):
                time.sleep(3.0 if why == "SAMPLES-DONE" else 0.0)
                subprocess.run(["taskkill", "/PID", str(box["pid"]), "/F"], capture_output=True)
            try: p.wait(60)
            except subprocess.TimeoutExpired: p.kill()
            break
    th.join(10)
    txt = "".join(lines) + (f"\n[run_shadow2] killed game pid {box['pid']}: {why}\n" if why else "")
    (out / "shadow_transcript.txt").write_text(txt, encoding="utf-8")
    pid = box["pid"]
    if pid and alive(pid):
        subprocess.run(["taskkill", "/PID", str(pid), "/F"], capture_output=True); time.sleep(1.0)
    log = out / "shadow_ab.log"
    for _ in range(5):
        try:
            if glog.exists(): shutil.move(str(glog), str(log))
            break
        except PermissionError:
            time.sleep(1.0)
    row = {"rva": "0x" + rva + tag, "verdict": "NO_LOG", "n": "", "ndiff": "", "proof": "", "detail": "", "pid": str(pid),
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
