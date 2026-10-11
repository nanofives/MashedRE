import os, re, csv, sys, glob, collections
ROOT = r"C:\Users\maria\Desktop\Proyectos\Mashed"
KEY = re.compile(r"^callees_depth1:(.*?)(?=^[A-Za-z_][\w]*:|\Z)", re.M | re.S)
edges = collections.defaultdict(set)
for p in glob.glob(os.path.join(ROOT, "re", "analysis", "**", "*.md"), recursive=True):
    try:
        with open(p, encoding="utf-8", errors="replace") as f:
            head = f.read(8000)
    except OSError:
        continue
    if not head.startswith("---"):
        continue
    fm = head[3:].split("\n---", 1)[0]
    m = re.search(r"^rva:\s*(0x[0-9a-fA-F]+)", fm, re.M)
    c = KEY.search(fm)
    if not m or not c:
        continue
    src = int(m.group(1), 16)
    for t in re.findall(r"(?:FUN_|0x)([0-9a-fA-F]{8})\b", c.group(1)):
        edges[src].add(int(t, 16))
import json
for k, v in json.load(open(sys.argv[3])).items():
    for t in v:
        edges[int(k, 16)].add(int(t, 16))
rows = {}
with open(os.path.join(ROOT, "hooks.csv"), encoding="utf-8", errors="replace", newline="") as f:
    for r in csv.DictReader(f):
        try:
            rows[int(r["rva"], 16)] = r
        except Exception:
            pass
roots = [int(x, 16) for x in sys.argv[1].split(",")]
maxd = int(sys.argv[2])
seen = {}
q = collections.deque((r, 0, r) for r in roots)
while q:
    v, d, rt = q.popleft()
    if v in seen:
        continue
    seen[v] = (d, rt)
    if d < maxd:
        for w in sorted(edges.get(v, ())):
            q.append((w, d + 1, rt))
print("plates_with_edges", len(edges), "reached", len(seen), file=sys.stderr)
cnt = collections.Counter()
w = csv.writer(sys.stdout, delimiter="\t", lineterminator="\n")
w.writerow(["rva", "depth", "root", "confidence", "status", "subsystem", "name", "has_plate_edges"])
for v, (d, rt) in sorted(seen.items()):
    r = rows.get(v) or {}
    conf = r.get("confidence", "norow")
    cnt[conf] += 1
    w.writerow(["0x%08x" % v, d, "0x%08x" % rt, conf, r.get("status", ""), r.get("subsystem", ""), r.get("name", ""), int(v in edges)])
print(dict(cnt), file=sys.stderr)
