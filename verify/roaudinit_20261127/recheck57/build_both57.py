"""Recheck run 57 copy of build_both.py (logs to D:). Both builds from the worktree (VS2022, then MASHED_DDKOBJ=1), each after a mashed_lock status check (RULE 4).
usage: py -3.12 build_both.py <tag> [vs|ddk ...]"""
import os, subprocess, sys, time, datetime
WT = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\RO-AUDIOINIT-45D460-REACH-5'
ORCH = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration'
D = 'D:/MashedWT/RO-AUDIOINIT-45D460-REACH-6_data/recheck57'; os.makedirs(D, exist_ok=True)
tag = sys.argv[1]
which = sys.argv[2:] or ['vs', 'ddk']


def wait_lock():
    while True:
        s = subprocess.run(['py', '-3.12', os.path.join(ORCH, r're\orchestrator\mashed_lock.py'), 'status'],
                           capture_output=True, text=True).stdout
        with open(os.path.join(D, 'lockstatus.txt'), 'a') as f:
            f.write('%s %s %s\n' % (datetime.datetime.now().isoformat(timespec='seconds'), tag, s.strip().replace('\n', ' | ')))
        if 'PRIORITY-HOLD' in s or 'SOLO' in s:
            print('[build] waiting: lock shows PRIORITY-HOLD / SOLO', flush=True)
            time.sleep(60)
            continue
        return


rc_all = 0
for w in which:
    wait_lock()
    env = dict(os.environ, MASHED_NO_DEPLOY='1')
    if w == 'ddk':
        env['MASHED_DDKOBJ'] = '1'
    else:
        env.pop('MASHED_DDKOBJ', None)
    log = os.path.join(D, 'build%s_%s.log' % (tag, w))
    t0 = time.time()
    with open(log, 'w', encoding='utf-8', errors='replace') as f:
        rc = subprocess.run(['cmd', '/c', r'mashedmod\build.bat'], cwd=WT, env=env, stdout=f, stderr=subprocess.STDOUT).returncode
    print('[build] %s rc=%d %.0fs log=%s' % (w, rc, time.time() - t0, log), flush=True)
    rc_all |= rc
sys.exit(rc_all)
