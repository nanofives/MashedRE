r"""D2-BAND-42S original-side runner (PREREG_D2-BAND-42S.md section 2). One run at a time, each inside test-slot and
mashed_lock.py run --solo. The band recipe's argv with only --hold changed. scenario_launch.py spawns and ends its own PID.
usage: py -3.12 run_orig.py <arm> <hold_s> <n_from> <n_to> [<suffix>]   e.g. run_orig.py o42 42 1 8, run_orig.py o42 42 3 3 b (= o42_r3b)
Appends one line per run to RUNLOG_orig.txt next to this file. Stops on a missing audio endpoint or the 0x005baf48 AV."""
import json, os, re, subprocess, sys, time, hashlib
W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
HERE = os.path.dirname(os.path.abspath(__file__))
DATA = 'D:/MashedWT/D2-BAND-42S_data/orig'
ROOT = r'D:\MashedWT\D2-BAND-42S_root'
LOCK = r'C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\orch-integration\re\orchestrator\mashed_lock.py'
TESTSLOT = r'C:\Users\maria\Desktop\Proyectos\.claude\scripts\testslot\proyectos_test_slot.py'
PIZ = ('1509b4', 'e66db')


def audio_ok():
    r = subprocess.run(['pwsh', '-NoProfile', '-Command',
                        'Get-PnpDevice -Class AudioEndpoint -PresentOnly | ForEach-Object { $_.Status + " " + $_.FriendlyName }'],
                       capture_output=True, text=True)
    lines = [l for l in r.stdout.splitlines() if l.strip()]
    return lines


def sha(p):
    h = hashlib.sha256()
    with open(p, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


SUF = sys.argv[5] if len(sys.argv) > 5 else ''


def one(arm, hold, n):
    out = f'{DATA}/{arm}_r{n}{SUF}.msd'
    txt = f'{DATA}/{arm}_r{n}{SUF}.txt'
    cmd = [sys.executable, TESTSLOT, sys.executable, LOCK, 'run', '--solo', '--label', 'D2-BAND-42S', '--',
           sys.executable, os.path.join(W, 're', 'frida', 'scenario_launch.py'), '--statediff-out', out,
           '--statediff-drive', '--statediff-drive-late', '--statediff-steer', '1', '--hold', str(hold), '--poke-ctrl-slots']
    env = dict(os.environ, MASHED_ROOT=ROOT)
    t0 = time.time()
    with open(txt, 'w', encoding='utf-8') as f:
        p = subprocess.run(cmd, cwd=W, env=env, stdout=f, stderr=subprocess.STDOUT)
    s = open(txt, encoding='utf-8', errors='replace').read()
    frames = None
    pv = out + '.provenance.json'
    if os.path.exists(pv):
        frames = json.load(open(pv)).get('capture', {}).get('frames')
    m = re.search(r'pid=(\d+)', s)
    status = 'OK'
    if '5baf48' in s.lower():
        status = 'AUDIO_AV'
    elif any(k in s.lower() for k in PIZ):
        status = 'PIZ'
    elif 'game exited' in s or frames is None or not os.path.exists(out):
        status = 'EARLY'
    line = (f'{time.strftime("%Y-%m-%d %H:%M:%S")} {arm}_r{n}{SUF} hold={hold} pid={m.group(1) if m else "?"} rc={p.returncode} '
            f'secs={time.time() - t0:.0f} frames={frames} status={status} '
            f'sha256={sha(out) if os.path.exists(out) else "-"}')
    print(line, flush=True)
    with open(os.path.join(HERE, 'RUNLOG_orig.txt'), 'a', encoding='utf-8') as f:
        f.write(line + '\n')
    return status


if __name__ == '__main__':
    arm, hold, a, b = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])
    os.makedirs(DATA, exist_ok=True)
    n = a
    retried = set()
    while n <= b:
        aud = audio_ok()
        print(f'[{arm}_r{n}] audio endpoints: {aud}', flush=True)
        if not aud:
            print('BLOCKED audio endpoint (none present)'); sys.exit(3)
        st = one(arm, hold, n)
        if st == 'AUDIO_AV':
            print('BLOCKED audio endpoint (AV at 0x005baf48)'); sys.exit(3)
        if st == 'PIZ':
            print(f'[{arm}_r{n}] U-9677 piz collision: re-run'); continue
        if st == 'EARLY':
            if n in retried:
                print(f'[{arm}_r{n}] second failure: RED'); sys.exit(4)
            retried.add(n); print(f'[{arm}_r{n}] early end: re-run once'); continue
        n += 1
