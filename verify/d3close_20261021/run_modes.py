r"""d3close modes leg (PREREG_d3close.md section 2.2): the live oracle on the stock original, one run per rule.
scenario_launch.py spawns and owns its MASHED.exe (explicit PID). MASHED_ROOT = the main checkout (the worktree has no install);
the oracle JSON lands in this worktree's log/ and is copied next to the run log in D:/MashedWT/d3close_data/modes/.
Crash rule: MASHED.exe+0x1509b4 / +0xe66db = piz collision (U-9677) -> the run is re-taken once automatically.
usage: py -3.12 run_modes.py [tag ...]   (default: all 12)
"""
import os, re, shutil, subprocess, sys
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MAIN = os.path.normpath(os.path.join(W, '..', '..'))
OUT = 'D:/MashedWT/d3close_data/modes'
os.makedirs(OUT, exist_ok=True)
BASE = ['--track', '0', '--mode', '10', '--cars', '4', '--car', '0', '--poke-ctrl-slots', '--oracle', '--hold', '40']
RUNS = [('r0', 0, []), ('r1', 1, []), ('r2', 2, []), ('r3', 3, []), ('r4', 4, ['--poke-lap', '0:3']),
        ('r5done', 5, ['--track', '3', '--poke-collect', '1:1']), ('r6', 6, []), ('r7', 7, ['--poke-lap', '1:3']),
        ('r8', 8, ['--poke-lap', '0:3']), ('r9c2', 9, ['--cars', '2', '--poke-lap', '1:3']),
        ('r10exp', 10, ['--mode', '3']), ('r10win', 10, ['--mode', '3', '--poke-lap', '0:2'])]
want = sys.argv[1:]
todo = [r for r in RUNS if not want or r[0] in want]
for i, (tag, rule, extra) in enumerate(todo, 1):
    for attempt in (1, 2):
        args = list(BASE)
        for j in range(0, len(extra), 2):   # later flags override the base value
            if extra[j] in args: args[args.index(extra[j]) + 1] = extra[j + 1]
            else: args += extra[j:j + 2]
        args += ['--rule', str(rule)]
        env = dict(os.environ); env['MASHED_ROOT'] = MAIN; env['MASHED_MUTE'] = '1'
        log = os.path.join(OUT, '%s_run.txt' % tag)
        with open(log, 'w') as f:
            f.write('argv: scenario_launch.py ' + ' '.join(args) + '\n')
            f.flush()
            rc = subprocess.call(['py', '-3.12', os.path.join(W, 're', 'frida', 'scenario_launch.py')] + args,
                                 cwd=W, env=env, stdout=f, stderr=subprocess.STDOUT)
        txt = open(log, errors='replace').read()
        if re.search(r'0x1509b4|0xe66db|\+0x1509b4|\+0xe66db', txt) and attempt == 1:
            print('[%d/%d] %s piz collision signature, re-taking' % (i, len(todo), tag), flush=True); continue
        break
    js = os.path.join(W, 'log', 'rules_oracle_rule%d.json' % rule)
    if os.path.exists(js): shutil.copy2(js, os.path.join(OUT, 'rules_oracle_%s.json' % tag))
    v = re.search(r'ORACLE VERDICT: (\w+)', txt)
    seg = re.search(r'SegmentCheck .*?calls=(\d+) agree=(\d+) MISMATCH=(\d+) segment-end\(ret!=0\)=(\d+)', txt)
    ev = re.search(r'EvaluateResult .*?calls=(\d+) agree=(\d+) MISMATCH=(\d+)', txt)
    fo = re.search(r'FinishOrder .*?calls=(\d+) agree=(\d+) MISMATCH=(\d+) appends=(\d+)', txt)
    print('[%d/%d] %s rule=%d exit=%d verdict=%s seg=%s eval=%s order=%s' % (
        i, len(todo), tag, rule, rc, v.group(1) if v else 'NONE',
        '/'.join(seg.groups()) if seg else '-', '/'.join(ev.groups()) if ev else '-', '/'.join(fo.groups()) if fo else '-'), flush=True)
print('DONE')
