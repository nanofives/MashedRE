import sys, time, pathlib, glob
N = pathlib.Path("D:/MashedWT/MATCH-S12_data/notes")
lim = float(sys.argv[1]) if len(sys.argv) > 1 else 540
def snap():
    return {p.name: sum(1 for l in p.read_text(errors="replace").splitlines() if l.startswith("0x")) for p in N.glob("h*.md")}
s0 = snap(); t0 = time.time()
while time.time() - t0 < lim:
    time.sleep(20)
    s = snap()
    if s != s0 and time.time() - t0 > lim: break
print(" ".join(f"{k[:-3]}={v}" for k, v in sorted(snap().items())), "total", sum(snap().values()))
