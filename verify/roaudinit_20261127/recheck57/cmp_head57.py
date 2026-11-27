"""RO-AUDIOINIT-45D460-REACH-6: new / gone DIFFERENT rvas against the run 57 drain 2 object-build line (d20fd3984 head).
usage: py -3.12 cmp_head6.py <build_ddk.log>"""
import re, sys
HEAD = r"C:/Users/maria/Desktop/Proyectos/Mashed/log/orch/run57_drain2.log"


def diffs(path, pat):
    for l in open(path, encoding='utf-8', errors='replace'):
        if pat in l and 'DIFFERENT' in l:
            m = re.search(r'IDENTICAL (\d+)/(\d+),?\s+DIFFERENT (\d+) \[([^\]]*)\]', l)
            if m:
                return m.group(1), m.group(2), m.group(3), set(m.group(4).split())
    raise SystemExit('no DIFFERENT line in ' + path)


h = diffs(HEAD, 'ddkobj:')
n = diffs(sys.argv[1], '[ddkobj summary]')
print('head IDENTICAL %s/%s DIFFERENT %s' % h[:3])
print('now  IDENTICAL %s/%s DIFFERENT %s' % n[:3])
print('new DIFFERENT:', sorted(n[3] - h[3]))
print('gone DIFFERENT:', sorted(h[3] - n[3]))
