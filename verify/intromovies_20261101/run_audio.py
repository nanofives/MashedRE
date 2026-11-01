r"""D5-INTROMOVIES-AUDIO boot arms: record the boot-movie audio of the original and the standalone.
Private root <root> (robocopy of original\) with the REAL movies from movies\backup\ placed in movies\.
Arms: O = <root>/original/MASHED.exe (cwd <root>/original), S = mashed_re.exe (MASHED_ROOT=<root>, no
MASHED_* knob: MASHED_INTRO_MOVIES is default ON since D5-INTROMOVIES-FLIP, MASHED_MUTE not set).
Each launch: audio-endpoint check (a VB-Audio endpoint must be listed, else the run stops BLOCKED),
loopback recording of the default capture device (rec_wave.py) from PRE s before the launch to WALL s
after it, then the spawned PID is killed (only that PID). Run the whole script inside
mashed_lock.py run, so no other game boots while a recording runs.
usage: py -3.12 run_audio.py <exe> <root> <outdir> <arm>[...]     arms O1 O2 S1 S2 ...
Each arm prints '[n/N] <arm> PASS|FAIL' and writes <outdir>/<arm>.wav (+ <arm>.log for S).
"""
import hashlib, os, shutil, subprocess, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from rec_wave import mci

PRE, WALL = 3.0, 115.0
FIVE = ['empire', 'supersonic', 'renderware', 'intro', 'small']


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest().upper()


def audio_check():
    out = subprocess.run(['pwsh', '-NoProfile', '-Command',
                          "Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.Status + ' ' + $_.FriendlyName }"],
                         capture_output=True, text=True).stdout
    return 'VB-Audio' in out, out.strip().replace('\n', ' | ')


def place_real(root):
    mov = os.path.join(root, 'original', 'toastart', 'pc', 'movies')
    for n in FIVE:
        shutil.copy2(os.path.join(mov, 'backup', n + '.mpg'), os.path.join(mov, n + '.mpg'))
    return {n: sha(os.path.join(mov, n + '.mpg'))[:16] for n in FIVE}


def arm(exe, root, out, name):
    ok, devs = audio_check()
    print(f'  {time.strftime("%H:%M:%S")} audio check {"OK" if ok else "NONE"}: {devs}', flush=True)
    if not ok:
        return 'BLOCKED', None, None
    if name.startswith('O'):
        cmd, cwd, env = [os.path.join(root, 'original', 'MASHED.exe')], os.path.join(root, 'original'), dict(os.environ)
    else:
        cmd, cwd = [exe], root
        env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
        env.update(MASHED_ROOT=root, MASHED_WIN_POS='primary-bl')
        lp = os.path.join(root, 'mashed_re.log')
        if os.path.exists(lp):
            os.remove(lp)
    wav = os.path.join(out, name + '.wav')
    mci('open new type waveaudio alias rec')
    try:
        mci('set rec time format ms bitspersample 16 channels 2 samplespersec 44100 bytespersec 176400 alignment 4')
        mci('record rec')
        t_rec = time.time()
        time.sleep(PRE)
        p = subprocess.Popen(cmd, cwd=cwd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        t_launch = time.time()
        while time.time() - t_launch < WALL:
            time.sleep(0.25)
        alive = p.poll() is None
        mci('stop rec')
        mci(f'save rec "{os.path.abspath(wav)}"')
    finally:
        mci('close rec')
    if alive:
        p.kill()   # only the PID this script spawned
    rc = p.wait()
    if name.startswith('S') and os.path.exists(os.path.join(root, 'mashed_re.log')):
        shutil.copy2(os.path.join(root, 'mashed_re.log'), os.path.join(out, name + '.log'))
    with open(os.path.join(out, name + '.meta'), 'w') as f:
        f.write(f'cmd={cmd[0]}\nexe_sha16={sha(cmd[0])[:16]}\npid={p.pid}\nalive_at_wall={alive}\nexit={rc}\n'
                f'launch_offset_s={t_launch - t_rec:.3f}\nwall_s={WALL}\naudio={devs}\n')
    return ('PASS' if alive else 'FAIL'), p.pid, round(t_launch - t_rec, 3)


def main():
    exe, root, out = os.path.abspath(sys.argv[1]), os.path.normpath(sys.argv[2]), os.path.normpath(sys.argv[3])
    arms = sys.argv[4:]
    os.makedirs(out, exist_ok=True)
    print(f'movies placed: {place_real(root)}', flush=True)
    for n, a in enumerate(arms, 1):
        st, pid, off = arm(exe, root, out, a)
        print(f'[{n}/{len(arms)}] {a} {st} pid={pid} launch_offset={off}', flush=True)
        if st == 'BLOCKED':
            return 2
    return 0


if __name__ == '__main__':
    sys.exit(main())
