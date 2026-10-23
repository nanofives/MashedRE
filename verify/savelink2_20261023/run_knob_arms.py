r"""savelink2 (a) arm runner: knob-progress arms on the cup screen (PREREG_knob_progress.md).
usage: py -3.12 run_knob_arms.py <arm> [<arm> ...]    arms: S0 S0r S1 S1e S0d S1d
Private root only (A4_ROOT, default D:/MashedWT/savelink_root). Explicit PID; never kills by name.
Restores the root save to the shipped copy after every arm and records its sha before and after.
"""
import hashlib, os, shutil, struct, subprocess, sys, time

W = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
EXE = os.environ.get('A4_EXE', os.path.join(W, r'mashedmod\build\mashed_re.exe'))
ROOT = os.path.normpath(os.environ.get('A4_ROOT', 'D:/MashedWT/savelink_root'))
OUT = os.path.normpath(os.environ.get('A4_OUT', 'D:/MashedWT/savelink2_data/knob'))
SHIPPED = 'D:/MashedWT/savelink_data/shipped.bin'
SAVE = os.path.join(ROOT, 'original', 'gamesave.bin')
LOG = os.path.join(ROOT, 'log', 'mashed_re.log')
DS = os.path.join(ROOT, 'log', 'drawstream_re.json')
SPAN, STRIDE, COL4 = 0x24A40, 0x30, 0x10

NAV = {'MASHED_NAV_KEYS': 'd,d,d,x,x,x,x,x,x', 'MASHED_NAV_KEYS_GAP': '40'}
ARMS = {
    'S0':  dict(knob=False, edited=False, nav=False, win='200:203'),
    'S0r': dict(knob=False, edited=False, nav=False, win='200:203'),
    'S1':  dict(knob=True,  edited=False, nav=False, win='200:203'),
    'S1e': dict(knob=True,  edited=True,  nav=False, win='200:203'),
    'S0d': dict(knob=False, edited=False, nav=True,  win='300:303'),
    'S1d': dict(knob=True,  edited=False, nav=True,  win='300:303'),
}


def sha(p, n=16):
    return hashlib.sha256(open(p, 'rb').read()).hexdigest()[:n]


def edited_save(dst):
    b = bytearray(open(SHIPPED, 'rb').read())
    for r in range(1, 13):
        struct.pack_into('<I', b, SPAN + r * STRIDE + COL4, 0)
    open(dst, 'wb').write(b)


def run(name):
    a = ARMS[name.split('-')[0]]
    os.makedirs(OUT, exist_ok=True)
    shutil.copyfile(SHIPPED, SAVE)
    shipped_copy = os.path.join(OUT, 'shipped_for_boot.bin')
    shutil.copyfile(SHIPPED, shipped_copy)
    if a['edited']:
        edited_save(SAVE)
    os.makedirs(os.path.dirname(LOG), exist_ok=True)   # GameFlow::log fopen()s log/mashed_re.log at boot
    pre = sha(SAVE)
    for p in (LOG, DS, os.path.join(ROOT, 'mashed_re.log')):
        try: os.remove(p)
        except OSError: pass
    e = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    e.update(MASHED_ROOT=ROOT, MASHED_NO_DEPLOY='1', MASHED_MUTE='1', MASHED_WIN_POS='primary-bl',
             MASHED_GOTO='6', MASHED_SETTINGS_DEFAULTS='1', MASHED_DBG_DRAWSTREAM=a['win'])
    if a['knob']: e['MASHED_SAVE_ORIGINAL'] = '1'
    if a['edited']: e['MASHED_SAVE'] = shipped_copy
    if a['nav']: e.update(NAV)
    p = subprocess.Popen([EXE], cwd=ROOT, env=e)
    t0, last, stable, how = time.time(), -1, 0, 'timeout'
    while time.time() - t0 < 120:
        if p.poll() is not None:
            how = f'exit={p.returncode}'; break
        if os.path.exists(DS):
            s = os.path.getsize(DS)
            stable = stable + 1 if s == last and s > 0 else 0
            last = s
            if stable >= 6:
                how = 'drawstream stable'; break
        time.sleep(0.5)
    if p.poll() is None:
        p.kill(); p.wait()
        how += f' (killed pid {p.pid} ONLY)'
    time.sleep(0.5)
    post = sha(SAVE)
    XLOG = os.path.join(ROOT, 'mashed_re.log')   # exe_main kLogPath (R2-2, NAV_KEYS)
    lines = []
    for lp in (LOG, XLOG):
        if os.path.exists(lp): lines += open(lp, encoding='latin-1').read().splitlines()
    prog = [l for l in lines if 'progress loaded' in l or 'R2-2 gamesave load' in l]
    navl = [l for l in lines if l.startswith('NAV_KEYS')]
    ds_out = os.path.join(OUT, name + '.drawstream.json')
    if os.path.exists(DS):
        shutil.copyfile(DS, ds_out)
    for lp, suf in ((LOG, '.gameflow.log'), (XLOG, '.exe.log')):
        if os.path.exists(lp): shutil.copyfile(lp, os.path.join(OUT, name + suf))
    shutil.copyfile(SHIPPED, SAVE)
    print(f'=== {name} pid={p.pid} exe={sha(EXE, 8)} {how} secs={time.time() - t0:.0f}')
    for l in prog: print('   ', l)
    if navl:
        rows = sorted({l.split(' row=')[1].split()[0] for l in navl})
        print(f'    NAV_KEYS lines={len(navl)} rows_seen={rows} last: {navl[-1][:90]}')
    print(f'    drawstream {"sha8=" + sha(ds_out, 8) + " bytes=" + str(os.path.getsize(ds_out)) if os.path.exists(ds_out) else "MISSING"}')
    print(f'    save pre={pre} post={post} {"UNCHANGED" if pre == post else "CHANGED"}; restored root save sha={sha(SAVE)}')


for n in sys.argv[1:]:
    run(n)
print('DONE')
