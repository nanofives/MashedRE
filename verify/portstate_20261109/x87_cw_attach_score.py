"""x87_cw_attach_score.py <tag>=<cw json> [...] [--all]

PORT-X87-UNREACHED. Per-RVA CW counts from x87_cw_attach.py runs, one column per scenario (n 0x007f / n 0x027f / n
other[/last other]). Default rows: the audit's unreached rows (X87AUDIT_TABLE.tsv: recommendation 'undetermined (not
reached)', plus the PC-SENSITIVE transcription rows marked 'not reached'). --all prints every RVA reached anywhere.
"""
import json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
args = [x for x in sys.argv[1:] if not x.startswith('--')]
show_all = '--all' in sys.argv
rows = [l.rstrip('\n').split('\t') for l in (HERE / 'X87AUDIT_TABLE.tsv').read_text().splitlines()]
hdr, rows = rows[0], rows[1:]
focus = []
for r in rows:
    d = dict(zip(hdr, r))
    if 'undetermined (not reached)' in d['recommendation'] or (
            d['scope'].startswith('transcription') and d['orig_cw'] == 'not reached' and d['static_orig'] == 'PC-SENSITIVE'):
        if d['rva'] not in [f[0] for f in focus]:
            focus.append((d['rva'], d['tu'], d['port']))
runs = []
for a in args:
    tag, f = a.split('=', 1)
    d = json.load(open(f))
    c = d['snaps'][0]['c'] if d['snaps'] else None
    runs.append((tag, {t: c[i] for i, t in enumerate(d['targets'])} if c else {}))
print('rva\ttu\tport\t' + '\t'.join(t for t, _ in runs) + '\torig_cw')
keys = [f[0] for f in focus]
if show_all:
    seen = set(keys)
    for _, m in runs:
        for k, v in m.items():
            if k not in seen and (v[0] or v[1] or v[2]):
                seen.add(k); keys.append(k)
info = {f[0]: f for f in focus}
for k in keys:
    cols, cws = [], set()
    for _, m in runs:
        v = m.get(k, [0, 0, 0, 0])
        cols.append('%d/%d/%d%s' % (v[0], v[1], v[2], ('/last=0x%04x' % v[3]) if v[2] else ''))
        if v[0]: cws.add('0x007f')
        if v[1]: cws.add('0x027f')
        if v[2]: cws.add('other')
    tu, port = info.get(k, (k, '', ''))[1:]
    print(f'{k}\t{tu}\t{port}\t' + '\t'.join(cols) + '\t' + (' + '.join(sorted(cws)) if cws else 'NOT REACHED'))
