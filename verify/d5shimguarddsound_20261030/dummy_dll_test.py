r"""D5-SHIM-GUARD-DSOUND dummy-proxy test. Usage: py -3.12 dummy_dll_test.py <label> <exe> <dummy_dir> <frames>
Copies <exe> to ROOT\mashed_re.exe, puts <dummy_dir>\dsound.dll beside it (GUARD_DLLS overrides; the dummy writes
<dll>.LOADED.txt beside itself in DllMain and returns FALSE), boots it with MASHED_ROOT=ROOT on the G-CTRL base env
with audio ON (GUARD_MUTE=1 adds MASHED_MUTE=1) for <frames> deterministic frames, kills only its own PID on timeout,
then reports exit code, markers and the dllguard / audio-init lines of mashed_re.log. Dummies and the exe copy are
removed afterwards.
"""
import hashlib, os, shutil, subprocess, sys, time
ROOT = os.path.normpath(os.environ.get('GUARD_ROOT', 'D:/MashedWT/D5-SHIM-GUARD-DSOUND_root'))
OUT = os.path.normpath(os.environ.get('GUARD_OUT', 'D:/MashedWT/D5-SHIM-GUARD-DSOUND_data/dummy'))
DLLS = tuple(os.environ.get('GUARD_DLLS', 'dsound.dll').split(','))


def sha(p):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()


def main():
    label, exe, dummy_dir, frames = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
    os.makedirs(OUT, exist_ok=True)
    dst = os.path.join(ROOT, 'mashed_re.exe')
    shutil.copy2(exe, dst)
    for d in DLLS:
        shutil.copy2(os.path.join(dummy_dir, d), os.path.join(ROOT, d))
        m = os.path.join(ROOT, d + '.LOADED.txt')
        if os.path.exists(m): os.remove(m)
    log = os.path.join(ROOT, 'mashed_re.log')
    if os.path.exists(log): os.remove(log)
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_TRACK_VIEW='Training', MASHED_CAR='1', MASHED_ROUND='1',
             MASHED_ROUND_RULE='0', MASHED_GAME_MODE='10', MASHED_WIN_POS='primary-bl', MASHED_DETERMINISTIC='1',
             MASHED_DET_FRAMES=str(frames))
    if os.environ.get('GUARD_MUTE', '0') == '1': e['MASHED_MUTE'] = '1'
    t0 = time.time()
    p = subprocess.Popen([dst], cwd=ROOT, env=e)
    print(f'=== {label} pid={p.pid} exe_sha256={sha(dst)}', flush=True)
    try:
        rc = p.wait(600)
    except subprocess.TimeoutExpired:
        print(f'    TIMEOUT - killing pid {p.pid} ONLY', flush=True); p.kill(); rc = p.wait()
    print(f'    exit={rc} (0x{rc & 0xffffffff:08X}) secs={time.time() - t0:.0f}', flush=True)
    for d in DLLS:
        m = os.path.join(ROOT, d + '.LOADED.txt')
        print(f'    dummy {d}: {"BOUND (marker present)" if os.path.exists(m) else "not bound (no marker)"}', flush=True)
    if os.path.exists(log):
        shutil.copy2(log, os.path.join(OUT, label + '.log'))
        lines = open(log, 'r', errors='replace').read().splitlines()
        g = [l for l in lines if l.startswith('dllguard:') or 'DirectSound' in l or 'Init: MASHED_MUTE' in l]
        print(f'    mashed_re.log lines={len(lines)} dllguard/audio-init lines={len(g)}', flush=True)
        for l in g: print('    ' + l, flush=True)
    else:
        print('    mashed_re.log: absent', flush=True)
    for d in DLLS + tuple(x + '.LOADED.txt' for x in DLLS):
        f = os.path.join(ROOT, d)
        if os.path.exists(f): os.remove(f)
    os.remove(dst)


if __name__ == '__main__':
    main()
