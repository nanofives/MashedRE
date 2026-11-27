"""RACE-LOAD-004E99B0: match.py --bytes + relocheck.py on every source this row binds into the exe or edits (RULE 9).
    py -3.12 verify/raceload_99b0_20261127/scope_match.py   (from the worktree root; writes scope_match.tsv next to it)
"""
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROWS = [
    # (class, rva, source)
    ('match', '004e5280', 're/match/rwl02/004e5280.c'),
    ('match', '005c4d20', 're/match/s28/005c4d20.c'),
    ('match', '005c4e10', 're/match/s28/005c4e10.c'),
    ('match', '0041a5b0', 're/match/s03/raw_0041a5b0.c'),
    ('match', '0041a5d0', 're/match/s03/h3_0041a5d0.c'),
    ('match', '00409300', 're/match/s01/00409300.c'),
    ('match', '00409330', 're/match/s01/00409330.c'),
    ('match', '00563de0', 're/match/rwl05/00563de0.c'),
    ('match', '0057c4b0', 're/match/rwl06/0057c4b0.c'),
    ('match', '004790e0', 're/match/s12/004790e0.c'),
    ('match', '005518f0', 're/match/rwtb/005518f0.c'),
    ('replay', '00558cb0', 're/match/rwl05/00558cb0.c'),
    ('replay', '004783f0', 're/match/pilot/a4_004783f0.c'),   # fix round: the replay-verified source (PROMOTIONS.tsv line 41)
    ('replay', '004b53b0', 're/match/s18/004b53b0.c'),
    ('replay', '00559560', 're/match/rwl05/00559560.c'),
    ('replay', '004785e0', 're/match/s12/004785e0.c'),
    ('provisional', '004e99b0', 're/match/rwl02/004e99b0.c'),
    ('provisional', '004e9e40', 're/match/rwl02/004e9e40.c'),
    ('provisional', '004ea220', 're/match/rwl02/004ea220.c'),
    ('provisional', '004e51e0', 're/match/rwl02/004e51e0.c'),
    ('provisional', '00558df0', 're/match/rwl05/00558df0.c'),
    ('provisional', '004cf2b0', 're/match/rwl01/004cf2b0.c'),
    ('provisional', '004b4550', 're/match/s18/004b4550.c'),
    ('provisional', '004b4eb0', 're/match/tlr/004b4eb0.c'),
    ('provisional', '004c4680', 're/match/s19/004c4680.c'),
    ('edited-caller', '00479330', 're/match/s12/00479330.c'),
]


def run(args):
    p = subprocess.run([sys.executable] + args, capture_output=True, text=True, encoding='utf-8', errors='replace')
    return p.stdout + p.stderr


def main():
    out = ['class\trva\tsource\tmatch\tbytes\trelocheck']
    for cls, rva, src in ROWS:
        m = run(['re/tools/match/match.py', src, '--bytes'])
        pct = re.search(r'_FUN_%s\S*: ([0-9.]+%%)' % rva, m) or re.search(r': ([0-9.]+%) \(regs', m)
        by = re.search(r'bytes: ([^\n]+)', m)
        r = run(['re/tools/match/relocheck.py', src]).strip().splitlines()
        rl = r[-1].split('\t')[1] if r and '\t' in r[-1] else (r[-1] if r else '')
        out.append('\t'.join([cls, '0x' + rva, src, pct.group(1) if pct else '?', by.group(1).strip() if by else '?', rl]))
        print('[%d/%d] %s %s %s' % (len(out) - 1, len(ROWS), rva, pct.group(1) if pct else '?', rl), flush=True)
    with open(os.path.join(HERE, 'scope_match.tsv'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()
