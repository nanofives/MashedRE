"""BIND-AUDIO-ONE-GENERATOR unit test: one audio generator.
usage: py -3.12 verify/bindaudio_20261121/tools/test_audio_one_generator.py   (from the worktree root)
1. audiotick_bind_gen.py run as a script refuses (exit text names porttop_bind.py --profile audio).
2. Its definitions still load by exec (rl_bind_gen.py / bulk_bind_gen.py read them that way).
3. The audio profile reads roots.txt (83 roots), the force / prov_bind / stop_force / callee_override rules moved here.
4. Its in-memory plan equals state/audio/plan.json; 0x005b10a0 binds re/match/s26/005b10a0.c; 0x005c92c0 /
   0x005c92f0 / 0x005c9310 stay named stops (no PROVISIONAL.tsv row yet), 0x005bf610 binds."""
import io, contextlib, json, os, subprocess, sys

ok = bad = 0
def check(name, cond, info=''):
    global ok, bad
    ok += bool(cond); bad += not cond
    print('[%d] %s %s %s' % (ok + bad, name, 'PASS' if cond else 'FAIL', info))

GEN = 'verify/portorigtop_20261113/tools/audiotick_bind_gen.py'
r = subprocess.run([sys.executable, GEN, '.', 'x.json', '00466b50'], capture_output=True, text=True)
check('generator refuses as a script', r.returncode != 0 and 'profile audio' in r.stderr, r.stderr.strip()[:90])

src = open(GEN, encoding='utf-8').read()
prefix = src[:src.index("plan = {}   # rva ->")]
g = {'__name__': 'audgen', '__file__': GEN}
saved = sys.argv
sys.argv = [GEN, '.', 'verify/portorigtop_20261113/tools/exeidx.json', '00466b50']
try:
    exec(compile(prefix, GEN, 'exec'), g)
    check('generator definitions load by exec', 'msrc' in g and 'PROV_BIND' in g)
except SystemExit as e:
    check('generator definitions load by exec', False, str(e))
sys.argv = saved

sys.path.insert(0, 're/tools/bind')
sys.argv = ['porttop_bind.py']
import porttop_bind as pb
roots = pb.profile_roots('audio').split(',')
check('audio roots from roots.txt', len(roots) == 83 and os.path.exists('re/tools/bind/state/audio/roots.txt'), len(roots))
b = pb.Binder('audio', roots)
check('rules: force 005b10a0', '005b10a0' in b.P['force'])
check('rules: prov_bind 005c92c0/f0/9310', {'005c92c0', '005c92f0', '005c9310'} <= set(b.P['prov_bind']))
check('rules: stop_force 005c91f0 kept, 005bf610 dropped', '005c91f0' in b.P['stop_force'] and '005bf610' not in b.P['stop_force'])
check('rules: callee_override 0045d460', ('0045d460', '004cc230') in b.P['callee_override'])
with contextlib.redirect_stdout(io.StringIO()):
    plan = {k: list(v) for k, v in b.build_plan().items()}
ref = json.load(open('re/tools/bind/state/audio/plan.json'))
check('plan equals state/audio/plan.json', plan == ref, '%d keys' % len(plan))
check('005b10a0 binds the s26 replay source', plan.get('005b10a0', [None, None])[:2] == ['bind', 're/match/s26/005b10a0.c'], plan.get('005b10a0', [])[:3])
for r_ in ('005c92c0', '005c92f0', '005c9310'):
    check('%s named stop (no PROVISIONAL row yet)' % r_, plan.get(r_) == ['stop'], plan.get(r_))
check('005bf610 binds', plan.get('005bf610', [None])[0] == 'bind')
print('RESULT %s: %d passed, %d failed' % ('PASS' if not bad else 'FAIL', ok, bad))
sys.exit(1 if bad else 0)
