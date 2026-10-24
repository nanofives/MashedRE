# bankpath E arm: one race through the real frontend launch (MASHED_RACE_DEMO=1 + MASHED_GOTO=6 -> GameFlow_RequestRace ->
# RaceSession::Begin), audio on, knob from argv. Private root, explicit PID only.
import os, subprocess, sys, shutil, time
root = r'D:\MashedWT\D4-AUDIO-BANKPATH_root'
exe = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-AUDIO-BANKPATH\mashedmod\build\mashed_re.exe'
name, knob = sys.argv[1], sys.argv[2]
out = r'D:\MashedWT\D4-AUDIO-BANKPATH_data\arms'
e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
e.update(MASHED_ROOT=root, MASHED_WIN_POS='primary-bl', MASHED_RACE_DEMO='1', MASHED_GOTO='6')
if knob == '1': e['MASHED_BANKPATH_CORE'] = '1'
for f in ('mashed_re.log', r'log\mashed_re.log'):
    p = os.path.join(root, f)
    if os.path.exists(p): os.remove(p)
p = subprocess.Popen([exe], cwd=root, env=e)
print('pid', p.pid, flush=True)
try: rc = p.wait(150)
except subprocess.TimeoutExpired:
    print('timeout, killing pid', p.pid); p.kill(); rc = p.wait()
print('exit', rc)
for f, ext in (('mashed_re.log', '.log'), (r'log\mashed_re.log', '.race.log')):
    s = os.path.join(root, f)
    if os.path.exists(s): shutil.copy2(s, os.path.join(out, name + ext))
