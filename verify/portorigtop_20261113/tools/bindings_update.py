"""Update verify/porttop_20261108/BINDINGS.tsv from plan.json (run from the worktree)."""
import json, os
S = os.path.dirname(os.path.abspath(__file__))
plan = json.load(open(os.path.join(S, 'plan.json')))
p = 'verify/porttop_20261108/BINDINGS.tsv'
raw = open(p, 'rb').read(); nl = '\r\n' if b'\r\n' in raw else '\n'
lines = raw.decode().split(nl)
if lines and lines[-1] == '': lines.pop()
idx = {l.split('\t')[0].lower(): i for i, l in enumerate(lines) if l.startswith('0x')}
# level / caller by BFS from the three roots (level 3 = callee of a level-2 audio body)
level, caller = {}, {}
q = [('00466b50', 2, '0x004926c0'), ('0045d3f0', 2, '0x004926c0'), ('0045d430', 2, '0x004926c0')]
while q:
    r, lv, c = q.pop(0)
    if r in level: continue
    level[r], caller[r] = lv, c
    pr = plan.get(r)
    if pr and pr[0] == 'bind':
        for cc in pr[3]:
            q.append((cc, lv + 1, '0x' + r))
upd = add = 0
TAG = 'PORT-TOP-AUDIOTICK-STOPS'
for r, pr in sorted(plan.items()):
    if r in ('00466b50', '0045d3f0', '0045d430'):
        continue
    if pr[0] == 'bind':
        kind, sym = 'matched', 'PortTop_Bound_' + r
        note = '%s: %s (%s), Match/PortTop_%s.cpp' % (TAG, pr[1], 'L-126 admitted, re/match/ADMITTED.tsv' if pr[2] == 'admitted' else 'c4=match', r)
    elif pr[0] == 'reuse':
        kind, sym = 'matched', pr[1]
        note = '%s: existing exe body %s' % (TAG, pr[2])
    else:
        kind, sym = 'missing', 'PortTop_AStop_' + r
        note = '%s: named stop (log + exit code = rva, Match/PortTop_AudioStops.cpp). No matched or admitted body' % TAG
    key = '0x' + r
    if key in idx:
        f = lines[idx[key]].split('\t')
        if f[2] in ('missing',) or (f[2] != kind and kind == 'matched' and f[2] in ('missing',)):
            f[2], f[3], f[4] = kind, sym, note
            lines[idx[key]] = '\t'.join(f); upd += 1
        elif f[2] == 'missing' and kind == 'missing':
            f[3], f[4] = sym, note
            lines[idx[key]] = '\t'.join(f); upd += 1
    else:
        lines.append('\t'.join([key, 'FUN_' + r, kind, sym, note, str(level.get(r, 3)), caller.get(r, '')])); add += 1
open(p, 'wb').write((nl.join(lines) + nl).encode())
print('updated', upd, 'added', add)
