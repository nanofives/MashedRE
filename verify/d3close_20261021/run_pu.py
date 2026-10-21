r"""d3close power-up leg (PREREG_d3close.md section 2.1).
1. Re-capture m1 / m2 / s1 / s2 on the ORIGINAL with their recorded argv (verify/d3_contact_20260928b/<c>.msd.provenance.json),
   as <c>r into D:/MashedWT/d3close_data/pu/. scenario_launch.py spawns and owns its MASHED.exe (explicit PID, never by name).
2. Replay all 11 (7 committed + 4 re-captured) through re/tools/pu_replay/out/pu_replay.exe built from THIS tree, then pu_diff.py.
usage: py -3.12 run_pu.py capture <c> [<c> ...] | py -3.12 run_pu.py sweep
"""
import json, os, re, subprocess, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MAIN = os.path.normpath(os.path.join(W, '..', '..'))
OUT = 'D:/MashedWT/d3close_data/pu'
os.makedirs(OUT, exist_ok=True)
CAPS = [('o3', 'verify/d3_pu_20260926/o3.msd'), ('o4', 'verify/d3_pu_20260926/o4.msd'),
        ('c2', 'verify/d3_contact_20260927/c2.msd'), ('c3', 'verify/d3_contact_20260927/c3.msd'),
        ('g2', 'verify/d3_contact_20260928/g2.msd'), ('g3', 'verify/d3_contact_20260928/g3.msd'),
        ('g4', 'verify/d3_contact_20260928/g4.msd'),
        ('m1r', OUT + '/m1r.msd'), ('m2r', OUT + '/m2r.msd'), ('s1r', OUT + '/s1r.msd'), ('s2r', OUT + '/s2r.msd')]


def capture(c):
    d = 'd3_box_20260928' if c == 'b3' else 'd3_contact_20260928b'   # amendment 1: b3 (--pu-box)
    prov = json.load(open(os.path.join(W, 'verify', d, c + '.msd.provenance.json')))
    argv = list(prov['argv'])
    i = argv.index('--statediff-out'); argv[i + 1] = OUT + '/' + c + 'r.msd'
    argv[0] = os.path.join(W, 're', 'frida', 'scenario_launch.py')
    env = dict(os.environ); env['MASHED_ROOT'] = MAIN; env['MASHED_MUTE'] = '1'
    log = open(os.path.join(OUT, c + 'r.capture.txt'), 'w')
    print('=== capture', c + 'r', ' '.join(argv[1:]), flush=True)
    rc = subprocess.call(['py', '-3.12'] + argv, cwd=W, env=env, stdout=log, stderr=subprocess.STDOUT)
    log.close()
    print('    exit', rc, flush=True)


def sweep():
    exe = os.path.join(W, 're', 'tools', 'pu_replay', 'out', 'pu_replay.exe')
    rows = []
    for n, p in CAPS:
        src = p if os.path.isabs(p) else os.path.join(W, p)
        if not os.path.exists(src + '.puhook.csv'):
            rows.append((n, 'MISSING', 'MISSING')); continue
        port, pcx = OUT + '/' + n + '.port.csv', OUT + '/' + n + '.port.pucontact.csv'
        for f in (port, pcx):
            if os.path.exists(f): os.remove(f)
        env = dict(os.environ); env['MASHED_PU_STEPDUMP'] = port; env['MASHED_PU_CONTACTDUMP'] = pcx
        rep = subprocess.run([exe, src + '.puhook.csv', '0'], env=env, capture_output=True, text=True).stdout
        open(OUT + '/' + n + '.replay.txt', 'w').write(rep)
        dif = subprocess.run(['py', '-3.12', os.path.join(W, 're', 'tools', 'pu_diff.py'), src + '.puhook.csv', port, '--slot', '0'],
                             capture_output=True, text=True).stdout
        open(OUT + '/' + n + '.diff.txt', 'w').write(dif)
        cv = re.search(r'^CONTACT VERDICT: (.*)$', rep, re.M); dv = re.search(r'^VERDICT: (.*)$', dif, re.M)
        rows.append((n, dv.group(1) if dv else '(none)', cv.group(1) if cv else '(none)'))
        print('[%d/%d] %s decision=%s contact=%s' % (len(rows), len(CAPS), n, rows[-1][1], rows[-1][2]), flush=True)
    with open(OUT + '/sweep.txt', 'w') as f:
        for r in rows: f.write('%-4s decision=%s | contact=%s\n' % r)
    print(open(OUT + '/sweep.txt').read())


if sys.argv[1] == 'capture':
    for c in sys.argv[2:]: capture(c)
elif sys.argv[1] == 'sweep':
    sweep()
elif sys.argv[1] == 'b3':   # amendment 1: replay b3r as built and with MASHED_PU_FORCE=nobox
    exe = os.path.join(W, 're', 'tools', 'pu_replay', 'out', 'pu_replay.exe'); src = OUT + '/b3r.msd.puhook.csv'
    for tag, force in (('gate', None), ('nobox', 'nobox')):
        env = dict(os.environ); env['MASHED_PU_STEPDUMP'] = OUT + '/b3r.%s.port.csv' % tag
        env['MASHED_PU_CONTACTDUMP'] = OUT + '/b3r.%s.port.pucontact.csv' % tag
        if force: env['MASHED_PU_FORCE'] = force
        rep = subprocess.run([exe, src, '0'], env=env, capture_output=True, text=True).stdout
        open(OUT + '/b3r.%s.replay.txt' % tag, 'w').write(rep)
        dif = subprocess.run(['py', '-3.12', os.path.join(W, 're', 'tools', 'pu_diff.py'), src, env['MASHED_PU_STEPDUMP'], '--slot', '0'],
                             capture_output=True, text=True).stdout
        open(OUT + '/b3r.%s.diff.txt' % tag, 'w').write(dif)
        cv = re.search(r'^CONTACT VERDICT: (.*)$', rep, re.M); dv = re.search(r'^VERDICT: (.*)$', dif, re.M)
        print('b3r %s decision=%s contact=%s' % (tag, dv.group(1) if dv else '(none)', cv.group(1) if cv else '(none)'), flush=True)
