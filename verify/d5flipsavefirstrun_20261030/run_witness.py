r"""D5-FLIP-SAVE-FIRSTRUN witness: the default seeds MASHED_SAVE_FIRSTRUN_WRITERS.
Save round-trip harness (Race/GameFlow.cpp SaveRoundTripHarness, MASHED_SAVE_RT_IN=none = first run, no file), the
rt() recipe of verify/spanwriters_20261027/run_spw.py. The exe exits before WinMain: no window, no piz open, no game.
  W_DEF  no MASHED_* env (the new default)
  W_K0   MASHED_SAVE_FIRSTRUN_WRITERS=0
  W_NO   MASHED_NO_SAVE_FIRSTRUN_WRITERS=1
Reads file 0x24efc (dword, 0x00409e30's 1) and 0x24f54..0x24f57 (span +0x514..+0x517, the gather bytes, 0x00496568).
usage: py -3.12 run_witness.py   (A4_EXE / A4_ROOT / A4_OUT as in run_measure.sh)
"""
import hashlib, os, re, struct, subprocess, sys
from pathlib import Path

EXE = Path(os.environ.get('A4_EXE', 'D:/MashedWT/D5-FLIP-SAVE-FIRSTRUN_data/mashed_re_on.exe'))
ROOT = Path(os.environ.get('A4_ROOT', 'D:/MashedWT/D5-FLIP-SAVE-FIRSTRUN_root'))
DATA = Path(os.environ.get('A4_OUT', 'D:/MashedWT/D5-FLIP-SAVE-FIRSTRUN_data')) / 'witness'
ARMS = [('W_DEF', {}), ('W_K0', {'MASHED_SAVE_FIRSTRUN_WRITERS': '0'}), ('W_NO', {'MASHED_NO_SAVE_FIRSTRUN_WRITERS': '1'})]


def rt(tag, extra):
    scratch = DATA / f'{tag}_scratch'
    scratch.mkdir(parents=True, exist_ok=True)
    for f in (scratch / 'mashed_re.log', scratch / 'rt_gamesave.bin', DATA / f'{tag}.bin'):
        if f.exists():
            f.unlink()
    out = DATA / f'{tag}.bin'
    env = {k: v for k, v in os.environ.items() if not k.startswith('MASHED_')}
    env.update(MASHED_SAVE_RT_IN='none', MASHED_SAVE_RT_OUT=str(out),
               MASHED_SAVE_PATH=str(scratch / 'rt_gamesave.bin'), MASHED_ROOT=str(ROOT))
    env.update(extra)
    p = subprocess.Popen([str(EXE)], cwd=str(scratch), env=env)
    try:
        rc = p.wait(timeout=60)
    except subprocess.TimeoutExpired:
        p.kill(); rc = f'TIMEOUT(killed own pid {p.pid})'
    slots = ''
    for lg in (scratch / 'mashed_re.log', ROOT / 'mashed_re.log'):
        if lg.exists():
            m = re.findall(r'input slots:.*|game controllers.*', lg.read_text(errors='replace'))
            if m:
                slots = m[-1]; break
    if not out.exists():
        return f'{tag:6} rc={rc} NO OUTPUT', None
    b = out.read_bytes()
    sha = hashlib.sha256(b).hexdigest()
    ctr = struct.unpack_from('<I', b, 0x24efc)[0]
    gat = ' '.join(f'{x:02x}' for x in b[0x24f54:0x24f58])
    return f'{tag:6} rc={rc} size={len(b)} sha={sha[:8]} 0x24efc={ctr} gather[0x24f54..57]={gat} {slots}', b


def main():
    DATA.mkdir(parents=True, exist_ok=True)
    res = {}
    for tag, extra in ARMS:
        line, b = rt(tag, extra)
        print(line, flush=True)
        res[tag] = b
    if all(res.values()):
        for a, c in (('W_DEF', 'W_K0'), ('W_DEF', 'W_NO'), ('W_K0', 'W_NO')):
            d = [i for i in range(len(res[a])) if res[a][i] != res[c][i]]
            print(f'{a} vs {c}: {len(d)} differ at ' + ' '.join(hex(i) for i in d[:16]), flush=True)
    print('WITNESS DONE', flush=True)


if __name__ == '__main__':
    sys.exit(main())
