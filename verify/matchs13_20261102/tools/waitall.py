import sys, time, pathlib
N = pathlib.Path("D:/MashedWT/MATCH-S13_data/notes")
A = {l.split()[0]: len(l.split()) - 1 for l in open("D:/MashedWT/MATCH-S13_data/helper_assign.txt") if l.strip()}
lim = float(sys.argv[1]) if len(sys.argv) > 1 else 560
def snap():
    o = {}
    for h in A:
        p = N / f"{h}.md"
        o[h] = sum(1 for l in p.read_text(errors="replace").splitlines() if l.startswith("0x")) if p.exists() else 0
    return o
s0 = snap(); t0 = time.time()
while time.time() - t0 < lim:
    s = snap()
    if all(s[h] >= A[h] for h in A): break
    time.sleep(15)
s = snap()
print(" ".join(f"{h}={s[h]}/{A[h]}" for h in A), "total", sum(s.values()), "/", sum(A.values()))
