"""Score D4-DC-INC-0046d4a0-REPLAYORDER (PREREG section 3) from the arms folder. Prints a markdown table.

usage: py -3.12 score.py [<arms_dir>]   (default D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/arms)
"""
import hashlib, json, os, re, sys

A = sys.argv[1] if len(sys.argv) > 1 else 'D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/arms'
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
mt = json.load(open(os.path.join(W, 're', 'orchestrator', 'merge_train.json')))
GREF = mt['gctrl']['ref']; BREF = mt['b']['ref']
WANT = {'rows': 3888, 'exact': 3888, 'entry_bad': 0, 'calls_bad': 0}
WANT_ARMS = {'bump': 60, 'collect': 60, 'no-op': 3522, 'vanish': 246}


def sha8(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'


def replay(name):
    p = os.path.join(A, name + '.replay.txt')
    if not os.path.exists(p):
        return False, 'no report'
    txt = open(p).read()
    m = re.search(r'G-M3-REPLAY rows=(\d+) exact=(\d+) entry_bad=(\d+) calls_bad=(\d+)', txt)
    a = re.search(r'^arms:(.*)$', txt, re.M)
    if not m or not a:
        return False, 'report incomplete'
    got = dict(zip(('rows', 'exact', 'entry_bad', 'calls_bad'), map(int, m.groups())))
    arms = {k: int(v) for k, v in re.findall(r'(\S+)=(\d+)', a.group(1))}
    mism = txt.count('MISMATCH')
    ok = got == WANT and all(arms.get(k) == v for k, v in WANT_ARMS.items()) and mism == 0
    return ok, '%s | arms %s | MISMATCH lines %d' % (m.group(0), a.group(1).strip(), mism)


def reads(name):
    p = os.path.join(A, name + '.pk46.tsv')
    if not os.path.exists(p):
        return False, 'no witness'
    rows = [l.rstrip('\n').split('\t') for l in open(p) if l.strip() and not l.startswith('#')]
    bases = set()
    bad = 0
    for r in rows:
        idx, one_ret, one_ptr = int(r[2]), int(r[8]), int(r[9], 16)
        if one_ret != 1 or one_ptr == 0:
            bad += 1
        else:
            bases.add(one_ptr - idx * 0xd04)
    ok = len(rows) == 60 and bad == 0 and len(bases) == 1
    return ok, 'rows=%d one_ret!=1_or_ptr0=%d distinct(one_ptr-idx*0xd04)=%d %s' % (
        len(rows), bad, len(bases), ','.join('%08x' % b for b in sorted(bases)[:4]))


res = []
for g, n in (('G-RP-OFF', 'RP_FIX_OFF'), ('G-RP-ON', 'RP_FIX_ON'), ('G-RP-FLIP', 'RP_FLIP'), ('G-RP-FLIP53 (diag)', 'RP_FLIP53')):
    res.append((g, n) + replay(n))
for g, n in (('G-READS-ON (OFF arm)', 'RP_FIX_OFF'), ('G-READS-ON (ON arm)', 'RP_FIX_ON')):
    res.append((g, n) + reads(n))
hd = [sha8(os.path.join(A, 'H_DEF' + s)) for s in ('.step.csv', '.gates.csv', '.ptrace.log')]
bd = [sha8(os.path.join(A, 'B_DEF_K%s.step.csv' % k)) for k in (0, 1, 26)]
res.append(('G-DEF', 'H_DEF + B_DEF_K0/K1/K26', hd == GREF and bd == BREF,
            'G-CTRL %s (ref %s)  B %s (ref %s)' % ('/'.join(hd), '/'.join(GREF), '/'.join(bd), '/'.join(BREF))))
print('| Gate | Arm | Result | Detail |\n|---|---|---|---|')
for g, n, ok, d in res:
    print('| %s | `%s` | %s | %s |' % (g, n, 'PASS' if ok else 'FAIL', d))
