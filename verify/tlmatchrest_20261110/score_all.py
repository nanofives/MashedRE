"""TL-MATCH-REST: score every in-scope row that has a re/match source (match.py --bytes + relocheck).
usage: py -3.12 verify/tlmatchrest_20261110/score_all.py [rva ...]   -> scores.tsv beside this script"""
import glob, os, re, subprocess, sys, pathlib
HERE = pathlib.Path(__file__).resolve().parent
ROWS = ('004c39b0 004c4a50 004c4d20 004c4dc0 004c4eb0 004c51a0 004c52f0 004cc050 '
        '005a6d60 005a7b60 005aa060 005aa0c0 005aaac0 005ab380 005ab410 005aba20 005abd30 005abfa0 005ac210 005acaa0 '
        '005acd10 005addd0 005ae650 005ae800 005ae920 005aec30 005aeca0 005b3760 005b3a00 005b3b80').split()
rows = [a.replace('0x', '') for a in sys.argv[1:]] or ROWS
srcs = {os.path.basename(p)[:8]: p for p in glob.glob('re/match/*/*.c')}
out = []
for n, r in enumerate(rows, 1):
    s = srcs.get(r)
    if not s:
        print('[%d/%d] %s no source' % (n, len(rows), r), flush=True); out.append((r, '-', '', '', '', '')); continue
    o = subprocess.run(['py', '-3.12', 're/tools/match/match.py', s, '--bytes'], capture_output=True, text=True)
    t = o.stdout + o.stderr
    m = re.search(r'0x%s \S+: (\d+\.\d+)%% \(regs ([\d.]+)%%, ops ([\d.]+)%%\)\s+(\d+/\d+)' % r, t)
    b = re.search(r'(\d+) differing unmasked bytes', t)
    ident = 'BYTE-IDENTICAL' in t
    rc = subprocess.run(['py', '-3.12', 're/tools/match/relocheck.py', s], capture_output=True, text=True).stdout
    rl = next((l.split('\t')[1] for l in rc.splitlines() if l.startswith('0x' + r)), '?')
    row = (r, s.replace(chr(92), '/'), m.group(1) if m else 'ERR', 'IDENT' if ident else (b.group(1) + 'B' if b else '?'),
           m.group(4) if m else '', rl)
    out.append(row)
    print('[%d/%d] %s %s %s %s relocheck=%s' % ((n, len(rows)) + row[0:1] + row[2:5] + row[5:6]), flush=True)
with open(HERE / 'scores.tsv', 'a', encoding='utf-8') as f:
    for r in out:
        f.write('\t'.join(r) + '\n')
