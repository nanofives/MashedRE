"""BIND-BULK-PROFILE: write re/tools/bind/state/bulk/moved.tsv, the lines by which the bulk plan's listing on this tree
moved from RACE-BIND-C4-BULK checkpoint 1's last_plan.txt, each with its cause (selftest_history checks the file).
usage (worktree root): py -3.12 verify/bindbulk_20261121/profile/tools/moved_tsv.py
A cause is derived from the plan entry, never typed per line; an rva no rule explains stops the script (exit 2)."""
import os, re, sys
sys.path.insert(0, 're/tools/bind')
import porttop_bind as pb

b = pb.Binder('bulk', pb.profile_roots('bulk').split(','))
b.build_plan()
rows = []
for k, o, n in pb.history_moves(b):
    if k == 'totals':
        cause = 'plan totals after the moves below'
    else:
        p = b.plan[k]
        old_unclean = ' cpp-unclean: ' in o
        src_tu = p[2] if p[0] == 'reuse' else ''
        if p[0] == 'reuse' and old_unclean and src_tu.endswith('.c'):
            cause = ('bound_wins: cp1 kept the cpp-unclean source as a named stop; the raceload profile binds it as a C unit '
                     '(BIND-CUNCLEAN-AS-C), %s, and a body another profile bound wins over the stop' % src_tu)
        elif p[0] == 'reuse' and src_tu.startswith('Match/PortTop_'):
            head = open(os.path.join(pb.SRC, src_tu), encoding='utf-8', errors='replace').readline().strip()
            m = re.match(r'// ([A-Z0-9-]+(?: [a-z]+ \d+)?)', head)
            cause = 'reuse: exe body %s bound after cp1 by %s (another profile); wins over the cp1 stop' % (
                src_tu, (m.group(1) if m and m.group(1) != 'RVA' else 'the audio profile'))
        elif p[0] == 'stop' and len(p) > 1 and p[1].startswith('outside the bulk cp1 list') and o.split(' ')[1] == 'stop':
            r = b.rows.get(k)
            cause = ('stop text only: hooks.csv now gives 0x%s a matched source (%s, %s), so the stop carries the roots-only '
                     'reason; cp1 read it %s' % (k, p[1].rsplit('source ', 1)[1], r[3] + ' ' + ','.join(re.findall(r'c4=[a-z-]+', r[8])),
                                                o.split(' ')[-2]))
        else:
            sys.exit('no cause rule for %s: %s -> %s' % (k, o, n))
    rows.append((k, o, n, cause))
out = os.path.join(b.sd, 'moved.tsv')
with open(out, 'w', encoding='utf-8', newline='\n') as f:
    f.write('# BIND-BULK-PROFILE: listing lines of `porttop_bind.py plan --profile bulk` on the head that differ from\n'
            '# verify/bindbulk_20261121/tools/last_plan.txt (RACE-BIND-C4-BULK checkpoint 1). Written by\n'
            '# verify/bindbulk_20261121/profile/tools/moved_tsv.py; selftest fails on any moved line not listed here.\n')
    f.write('rva\tcp1 line\thead line\tcause\n')
    for r in rows:
        f.write('\t'.join(r) + '\n')
print(len(rows), 'rows ->', out)
