import re, sys, statistics as st
rows = {}
alive = []
for l in open(sys.argv[1]):
    m = re.match(r'\[s(\d)\] R=([\d.]+) spd=([-\d.]+) .* skidI=([-\d.]+) skids=(\d+) sparks=(\d+) alive=(\d+)', l)
    if not m: continue
    s = int(m.group(1)); rows.setdefault(s, []).append((float(m.group(3)), float(m.group(4)), int(m.group(5)), int(m.group(6))))
    if s == 0: alive.append(int(m.group(7)))
print('samples/slot', {s: len(v) for s, v in sorted(rows.items())})
print('final skids/slot', {s: v[-1][2] for s, v in sorted(rows.items())}, ' sparks/slot', {s: v[-1][3] for s, v in sorted(rows.items())})
print('max skidI/slot', {s: max(x[1] for x in v) for s, v in sorted(rows.items())}, ' max spd', {s: max(x[0] for x in v) for s, v in sorted(rows.items())})
print('pool alive (of 1200): peak', max(alive), 'median', st.median(alive), 'samples', len(alive))
