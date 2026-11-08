"""Bounded permute over a row list (C4R-NEARMISS, 2026-11-07). Fixed budget per row:
match_permute.py --rounds 6 --walk 40 --time 90 --jobs 4, hard kill at 300 s. 3 lanes in parallel.
No --write: the best variant stays in log/permute/<addr>.cpp and is copied to the data dir.
    py -3.12 verify/c4rnearmiss_20261107/permute_driver.py <rows.tsv> <outdir>"""
import concurrent.futures, pathlib, re, shutil, subprocess, sys, threading
ROOT = pathlib.Path(__file__).resolve().parents[2]
rows = [l.split("\t")[:2] for l in pathlib.Path(sys.argv[1]).read_text().splitlines() if l.strip()]
out = pathlib.Path(sys.argv[2]); out.mkdir(parents=True, exist_ok=True)
ARGS = ["--rounds", "6", "--walk", "40", "--time", "90", "--jobs", "4"]
lock = threading.Lock(); done = [0]
RX = re.compile(r"([0-9.]+)%")

def run(row):
    rva, src = row
    log = out / f"{rva}.log"
    cmd = [sys.executable, str(ROOT / "re/tools/match/match_permute.py"), str(ROOT / src), rva] + ARGS
    try:
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=300)
        txt = p.stdout + p.stderr; status = "RC%d" % p.returncode
    except subprocess.TimeoutExpired as e:
        txt = (e.stdout or b"").decode(errors="replace") if isinstance(e.stdout, bytes) else (e.stdout or ""); status = "TIMEOUT"
    log.write_text(txt)
    lines = [l for l in txt.splitlines() if RX.search(l)]
    start = RX.search(lines[0]).group(1) if lines else "?"
    best = max((float(RX.search(l).group(1)) for l in lines), default=None)
    for c in (ROOT / "log" / "permute").glob(f"{int(rva,16):08x}.*"):
        shutil.copy(c, out / f"{rva}_best{c.suffix}")
    with lock:
        done[0] += 1
        print(f"[{done[0]}/{len(rows)}] {rva} {start}->{best} {status}", flush=True)
        with open(out / "summary.tsv", "a") as f:
            f.write(f"{rva}\t{src}\t{start}\t{best}\t{status}\n")

with concurrent.futures.ThreadPoolExecutor(3) as ex:
    list(ex.map(run, rows))
