"""G-WALK / G-WHEEL scorer (PREREG_d2bsp.md section 3 + amendment 1).
usage: py -3.12 g_walk.py <orig .batchprobe.csv> <exe R_ arm .replay.csv>
G-WALK      fix W's batch on the original's query == the original's batch (count, order, every captured dword)
standin     the stand-in's batch on the same query (report only): same count / same set / same order
G-WHEEL-K   replayed classifier +0x1f0 == the original's next solver-entry +0x1f0, per wheel-call
G-WHEEL-F   the original's next-entry (fv, key, normal) == the replay's own wheel OR its axle partner's
Fields per wheel in wstart / O rows / cls: +0x194 fv, +0x1ec key, +0x1f0 key2, +0x200, +0x204, +0x208."""
import collections, csv, sys

csv.field_size_limit(1 << 30)
orig, rep = sys.argv[1], sys.argv[2]
Q, O = {}, []
with open(orig, newline='') as f:
    for r in csv.DictReader(f):
        seq = int(r['seq'])
        if r['kind'] == 'Q':
            Q[seq] = r
        elif r['kind'] == 'O':
            O.append((seq, r['cx'].split(';')))   # O rows: 'cx' column holds the ;-joined wheel dwords
O.sort()
oseqs = [s for s, _ in O]
import bisect
R = {}
with open(rep, newline='') as f:
    for r in csv.DictReader(f):
        R[int(r['seq'])] = r

def ents(s):
    return [] if not s else s.split('|')

PART = {0: 1, 1: 0, 2: 3, 3: 2}
n = walk_ok = 0
st = collections.Counter()
k_ok = {'W': 0, 'S': 0}; f_ok = {'W': 0, 'S': 0}; wc = 0
k_ok_red = {'W': 0, 'S': 0}; wc_red = 0
f_ok_post = {'W': 0, 'S': 0}; wc_post = 0
first_bad = []
cnt_hist_o, cnt_hist_s = collections.Counter(), collections.Counter()
red_q = 0
for seq in sorted(Q):
    q = Q[seq]
    if seq not in R:
        continue
    r = R[seq]
    n += 1
    eo = ents(q['entries'])
    ew, es = ents(r['entW']), ents(r['entS'])
    if int(q['count']) == int(r['nW']) and eo == ew:
        walk_ok += 1
    elif len(first_bad) < 5:
        diff = None
        for i, (a, b) in enumerate(zip(eo, ew)):
            if a != b:
                da, db = a.split(';'), b.split(';')
                diff = (i, [k for k in range(min(len(da), len(db))) if da[k] != db[k]][:6]); break
        first_bad.append((seq, q['count'], r['nW'], diff))
    cnt_hist_o[int(q['count'])] += 1
    cnt_hist_s[int(r['nS'])] += 1
    st['count_eq'] += int(q['count']) == int(r['nS'])
    st['set_eq'] += sorted(eo) == sorted(es)
    st['order_eq'] += eo == es
    st['orig_subset_of_standin'] += set(eo) <= set(es)
    tri = lambda L: [';'.join(e.split(';')[:9]) for e in L]     # vertex dwords only (amendment 2)
    st['tri_set_eq'] += sorted(tri(eo)) == sorted(tri(es))
    st['tri_order_eq'] += tri(eo) == tri(es)
    st['tri_orig_in_standin'] += set(tri(eo)) <= set(tri(es))
    post = int(q['frame']) >= 1000
    has_red = any(e.split(';')[13] == 'ff010101' for e in eo)
    red_q += has_red
    # next solver entry for this car
    j = bisect.bisect_right(oseqs, seq)
    if j >= len(O):
        continue
    out = O[j][1]
    cls = {'W': r['clsW'].split(';'), 'S': r['clsS'].split(';')}
    for w in range(4):
        wc += 1; wc_red += has_red; wc_post += post
        ow = out[w * 6:(w + 1) * 6]
        for arm in ('W', 'S'):
            c = cls[arm]
            own = c[w * 6:(w + 1) * 6]; par = c[PART[w] * 6:(PART[w] + 1) * 6]
            if own[2] == ow[2]:
                k_ok[arm] += 1; k_ok_red[arm] += has_red
            sel = [0, 1, 3, 4, 5]
            if [ow[k] for k in sel] in ([own[k] for k in sel], [par[k] for k in sel]):
                f_ok[arm] += 1; f_ok_post[arm] += post
pct = lambda a, b: f'{a}/{b} = {100.0 * a / b:.2f} %' if b else f'{a}/0'
print(f'queries replayed {n} (orig Q rows {len(Q)}, O rows {len(O)})')
print(f'G-WALK   fix W bit-identical batch: {pct(walk_ok, n)}  -> {"PASS" if n and walk_ok == n else "FAIL"}')
for b in first_bad:
    print(f'   first mismatch seq={b[0]} orig count={b[1]} W count={b[2]} entry/dwords={b[3]}')
print(f'standin  same count {pct(st["count_eq"], n)}, same set {pct(st["set_eq"], n)}, same order '
      f'{pct(st["order_eq"], n)}, orig set contained in standin set {pct(st["orig_subset_of_standin"], n)}')
print(f'standin by triangle (vertex dwords 0..8): same set {pct(st["tri_set_eq"], n)}, same order '
      f'{pct(st["tri_order_eq"], n)}, orig triangles contained in standin {pct(st["tri_orig_in_standin"], n)}')
print(f'queries whose original batch holds a ff010101 entry: {pct(red_q, n)}')
print(f'count histogram orig    {sorted(cnt_hist_o.items())}')
print(f'count histogram standin {sorted(cnt_hist_s.items())}')
for arm in ('W', 'S'):
    print(f'G-WHEEL-K {arm}: {pct(k_ok[arm], wc)}   (wheel-calls on ff010101 queries: {pct(k_ok_red[arm], wc_red)})')
    print(f'G-WHEEL-F {arm}: {pct(f_ok[arm], wc)}   (frames >= 1000, amendment 2: {pct(f_ok_post[arm], wc_post)})')
gk = wc and k_ok['W'] >= 0.99 * wc and f_ok['W'] >= 0.99 * wc
print(f'G-WHEEL (fix W, both parts >= 99 %): {"PASS" if gk else "FAIL"}')
