"""PORT-TOP-08: one line per knob-path log: rc line, NAV_KEYS count and sha8 (sha256 of the lines joined by LF plus a
final LF, the PORT-TOP-05 method: its qb_mat1.log gives 479d0282), flow counts, the DAT_00771968 transitions, the
input-slot line and the self-test line.
usage: py -3.12 verify/porttop08_20261112/summarize.py <log> [<log> ...]
"""
import collections
import hashlib
import os
import re
import sys

for p in sys.argv[1:]:
    nav, trans, extra = [], [], []
    flow = collections.Counter()
    for ln in open(p, encoding='latin-1'):
        ln = ln.rstrip('\r\n')
        if ln.startswith('NAV_KEYS'):
            nav.append(ln)
            m = re.search(r'\bflow=(\d+)', ln)
            if m:
                flow[m.group(1)] += 1
        elif 'DAT_00771968' in ln and '->' in ln:
            m = re.search(r'frame (\d+) (before|after) 0x004929d0: DAT_00771968 (-?\d+) -> (-?\d+)', ln)
            if m:
                trans.append('f%s %s->%s' % (m.group(1), m.group(3), m.group(4)))
        elif ('input slots:' in ln or 'prov-43dfd0' in ln or 'WinMain 0x00492370 returned' in ln
              or 'rva 0x0042c510' in ln or 'rva 0x004111c0' in ln):
            extra.append(ln.strip())
    sha = hashlib.sha256(('\n'.join(nav) + '\n').encode('latin-1')).hexdigest()[:8] if nav else '-'
    print('%s | nav=%d sha8=%s | flow %s | phase %s' % (os.path.basename(p), len(nav), sha,
          '/'.join('%s:%d' % kv for kv in sorted(flow.items())), ' '.join(trans) or '-'))
    for e in extra:
        print('    ' + e)
