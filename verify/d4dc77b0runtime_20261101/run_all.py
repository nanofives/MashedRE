"""D4-DC-77b0-RUNTIME-PARKED runner. Each boot runs alone under mashed_lock.py (label D4-DC-77b0-RUNTIME-PARKED).

Original arms (scenario_launch.py --valsrc-probe, private root): o_r<rule>_t0 for rule (DAT_007f0fd0) in the list, mode 10,
4 cars, --poke-ctrl-slots, track 0. Before each original boot `Get-PnpDevice -Class AudioEndpoint -PresentOnly` must list
a VB-Audio endpoint. A log with MASHED.exe+0x1509b4 / +0xe66db (U-9677 piz collision) is re-run once.
Exe arms (exe_vsprobe.py): x_r<rule> on the worktree build, H_DEF env.

usage: py -3.12 run_all.py o:0:240 o:2:120 ... x:0:14400
"""
import os, subprocess, sys, time

W = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-DC-77b0-RUNTIME-PARKED'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
ROOT = r'D:\MashedWT\D4-DC-77b0-RUNTIME-PARKED_root'
OUT = r'D:\MashedWT\D4-DC-77b0-RUNTIME-PARKED_data'
PY = sys.executable


def audio_ok():
    r = subprocess.run(['powershell', '-NoProfile', '-Command',
                        'Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.FriendlyName }'],
                       capture_output=True, text=True)
    print('  [audio]', ' | '.join(x.strip() for x in r.stdout.splitlines() if x.strip()), flush=True)
    return 'VB-Audio' in r.stdout


def locked(cmd, log):
    full = [PY, LOCK, 'run', '--label', 'D4-DC-77b0-RUNTIME-PARKED', '--'] + cmd
    env = dict(os.environ, MASHED_ROOT=ROOT)
    with open(log, 'w') as f:
        p = subprocess.Popen(full, cwd=W, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')
        for ln in p.stdout:
            f.write(ln); f.flush()
            if ln.strip(): print('    ' + ln.rstrip(), flush=True)
        return p.wait()


for spec in sys.argv[1:]:
    side, rule, n = spec.split(':')[:3]
    cont = spec.endswith(':c')   # o:<rule>:<hold>:c = --continue-rounds on the standings sub-states 7..0xb
    for attempt in (1, 2):
        name = f'{side}_r{rule}_t0' + ('c' if cont else '') + ('' if attempt == 1 else '_rr')
        log = os.path.join(OUT, name + '.log')
        print(f'=== {name} {time.strftime("%H:%M:%S")}', flush=True)
        if side == 'o':
            if not audio_ok():
                print('  NO VB-Audio endpoint: skipping', name, flush=True); break
            cmd = ['py', '-3.12', r're\frida\scenario_launch.py', '--mode', '10', '--cars', '4', '--rule', rule, '--track', '0',
                   '--poke-ctrl-slots', '--hold', n, '--statediff-out', os.path.join(OUT, name + '.msd'), '--valsrc-probe']
            if cont: cmd += ['--continue-rounds', '--continue-rounds-states', '7,8,9,10,11']
        else:
            cmd = ['py', '-3.12', os.path.join(W, r'verify\d4dc77b0runtime_20261101\exe_vsprobe.py'),
                   '--exe', os.path.join(W, r'mashedmod\build\mashed_re.exe'), '--root', ROOT,
                   '--out', os.path.join(OUT, name), '--rule', rule, '--frames', n]
            if cont: cmd += ['--env', 'MASHED_R7_INPUT_PRESS=150']   # x:<rule>:<frames>:c = the exe's own one-press arm test
        rc = locked(cmd, log)
        txt = open(log, errors='replace').read()
        piz = ('0x1509b4' in txt.lower()) or ('0xe66db' in txt.lower())
        print(f'  [{name}] rc={rc} piz_collision={piz} {time.strftime("%H:%M:%S")}', flush=True)
        if not piz: break
print('ALL DONE', flush=True)
