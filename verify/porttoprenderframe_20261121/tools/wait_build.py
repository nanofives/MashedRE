"""PORT-TOP-RENDERFRAME-492E90 build etiquette (copied from verify/porttop06_20261121/tools/wait_build.py) runner: wait while another Mashed build.bat / ddkobj.py runs, while mashed_lock shows a
PRIORITY-HOLD or SOLO, or while commit is above 26 GB; then print test-slot + lock status (kept in the data dir) and run
one step. Usage (worktree root):
  py -3.12 verify/porttoprenderframe_20261121/tools/wait_build.py vs|ddk|check <tag>
"""
import datetime, os, re, subprocess, sys, time

DATA = 'D:/MashedWT/PORT-TOP-RENDERFRAME-492E90_data'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
SLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
HERE = os.path.abspath('.')


def out(cmd):
    return subprocess.run(cmd, capture_output=True, text=True, errors='replace').stdout


def others():
    ps = ("Get-CimInstance Win32_Process | Where-Object { ($_.Name -eq 'cmd.exe' -and $_.CommandLine -match 'mashedmod.build\\.bat') -or "
          "($_.Name -eq 'python.exe' -and $_.CommandLine -match 'ddkobj\\.py') } | ForEach-Object { \"$($_.ProcessId) $($_.CommandLine)\" }")
    return [l for l in out(['powershell', '-NoProfile', '-Command', ps]).splitlines() if l.strip() and str(os.getpid()) not in l.split()[0]]


def commit_gb(s):
    m = re.search(r'memory: ([\d.]+) GB committed', s)
    return float(m.group(1)) if m else 0.0


step, tag = sys.argv[1], sys.argv[2]
while True:
    lk, sl, ot = out(['py', '-3.12', LOCK, 'status']), out(['py', '-3.12', SLOT, '--status']), others()
    hold = re.search(r'PRIORITY|SOLO', lk)
    if not hold and not ot and commit_gb(sl) <= 26.0:
        break
    print(datetime.datetime.now().strftime('%H:%M:%S'), 'waiting:', 'hold' if hold else '', '%d other build(s)' % len(ot) if ot else '',
          'commit %.1f GB' % commit_gb(sl), flush=True)
    time.sleep(30)
st = '%s\n== %s %s\n%s\n%s\n' % (datetime.datetime.now().isoformat(), step, tag, sl, lk)
open('%s/status_pre_%s.txt' % (DATA, tag), 'w').write(st)
print(st, flush=True)
env = dict(os.environ, MASHED_NO_DEPLOY='1', PYTHONUTF8='1')
if step == 'ddk':
    env['MASHED_DDKOBJ'] = '1'
if step in ('vs', 'ddk'):
    cmd = ['cmd', '/c', r'mashedmod\build.bat']
else:
    cmd = ['py', '-3.12', 're/tools/ddkobj/ddkobj.py', 'check']
t0 = time.time()
with open('%s/%s_%s.log' % (DATA, step, tag), 'w', encoding='utf-8', errors='replace') as f:
    p = subprocess.Popen(cmd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, errors='replace')
    for line in p.stdout:
        f.write(line)
        if re.search(r'error|Build OK|NEW=|IDENTICAL|DIFFERENT|summary|rva-lint|LNK|refus', line, re.I):
            print(line.rstrip()[:300], flush=True)
    rc = p.wait()
print('%s rc=%d in %.0f s, log %s/%s_%s.log' % (step, rc, time.time() - t0, DATA, step, tag), flush=True)
sys.exit(rc)
