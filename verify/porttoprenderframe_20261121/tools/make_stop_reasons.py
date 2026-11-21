"""PORT-TOP-RENDERFRAME-492E90: write re/tools/bind/state/renderframe/stop_reasons.tsv (rva <TAB> reason) from
  - log/orch/run53_bindlists/race_owned.txt and audio_owned.txt (main checkout): rvas another run-53 row owns
  - orch/RACE-BIND-C4-BULK @ d4fda5c93 verify/bindbulk_20261121/tools/plan_cp.json: the bodies cp1 binds (unmerged)
  - PORT-TOP-06's raceframe plan (0x004111c0 closure, unmerged, uncommitted in .worktrees/PORT-TOP-06 at 2026-11-21):
    snapshot tools/porttop06_raceframe_plan_snapshot.json (sha256 in the RESULT). Its binds and the root 0x004111c0
    are PORT-TOP-06's: listed, not rebound.
Run from the worktree root: py -3.12 verify/porttoprenderframe_20261121/tools/make_stop_reasons.py <main-checkout-root>
"""
import json, os, subprocess, sys

main_root = sys.argv[1] if len(sys.argv) > 1 else r'C:\Users\maria\Desktop\Proyectos\Mashed'
out = {}
for fn, tag in (('race_owned.txt', 'owned by RACE rows (run 53, race_owned.txt)'),
                ('audio_owned.txt', 'owned by AUDIO rows (run 53, audio_owned.txt)')):
    for l in open(os.path.join(main_root, 'log', 'orch', 'run53_bindlists', fn), encoding='utf-8'):
        l = l.strip()
        if l.startswith('0x'):
            out.setdefault(l[2:].lower(), tag)
CP1 = 'd4fda5c93'
plan = json.loads(subprocess.run(['git', 'show', CP1 + ':verify/bindbulk_20261121/tools/plan_cp.json'],
                                 capture_output=True, text=True, check=True).stdout)
cp1 = sorted(k for k, v in plan.items() if v and v[0] == 'bind')
for r in cp1:
    out.setdefault(r, 'bound by RACE-BIND-C4-BULK cp1 (orch/RACE-BIND-C4-BULK @ %s, unmerged): listed, not rebound' % CP1)
snap = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'porttop06_raceframe_plan_snapshot.json')
rf = json.load(open(snap, encoding='utf-8'))
rfb = sorted(k for k, v in rf.items() if v and v[0] == 'bind')
out.setdefault('004111c0', 'PORT-TOP-06 root (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound')
for r in rfb:
    out.setdefault(r, 'bound by PORT-TOP-06 (0x004111c0 closure, orch/PORT-TOP-06, unmerged): listed, not rebound')
p = 're/tools/bind/state/renderframe/stop_reasons.tsv'
os.makedirs(os.path.dirname(p), exist_ok=True)
with open(p, 'w', encoding='utf-8', newline='\n') as f:
    f.write('# PORT-TOP-RENDERFRAME-492E90: rvas the renderframe closure does not bind '
            '(verify/porttoprenderframe_20261121/tools/make_stop_reasons.py)\n')
    for k in sorted(out):
        f.write('%s\t%s\n' % (k, out[k]))
print('owned', sum('owned' in v for v in out.values()), 'cp1 bind keys', len(cp1), 'porttop06 binds', len(rfb),
      'rows', len(out))
