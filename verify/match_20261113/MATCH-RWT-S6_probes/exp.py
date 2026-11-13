"""MATCH-RWT-S6 experiment runner for 0x004ce970.
exp.py <variants.txt> [--tc ddk|rw9044] [--flags "..."] [--show NAME] [--jobs N]
variants.txt: blocks separated by lines '@@ name'; each block is the function text from '// MATCH:' on.
An optional first block (before the first '@@') is shared text placed before every variant.
Prints per variant: score, regs, ops, instruction counts, byte-identity, the register of the first call result
(image), and the original's for reference. Probe files go to <variants dir>/<stem>_<name>.c."""
import os, sys, pathlib, argparse, concurrent.futures as cf
ROOT = pathlib.Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 're' / 'tools' / 'match'))
import match  # noqa: E402

BASE = ROOT / 're' / 'match' / 's20' / '004ce970.c'


def run(p, tc, flags):
    try:
        r = match.compare(pathlib.Path(p), flags=flags, quiet=True, toolchain=tc, bytes_mode=True)[0]
    except SystemExit as e:
        return {'err': str(e)[:300]}
    return r


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('vfile')
    ap.add_argument('--tc', default=None)
    ap.add_argument('--flags', default='')
    ap.add_argument('--show', default=None)
    ap.add_argument('--jobs', type=int, default=8)
    a = ap.parse_args()
    head = BASE.read_text(encoding='utf-8').split('// MATCH:')[0]
    txt = open(a.vfile, encoding='utf-8').read()
    outdir = os.path.dirname(os.path.abspath(a.vfile))
    stem = os.path.splitext(os.path.basename(a.vfile))[0]
    blocks = txt.split('\n@@ ')
    pre = ''
    if blocks[0].startswith('@@ '):
        blocks[0] = blocks[0][3:]
    else:
        pre = blocks.pop(0)
    jobs = []
    for b in blocks:
        name, body = b.split('\n', 1)
        name = name.strip()
        if a.show and name != a.show:
            continue
        p = os.path.join(outdir, f'{stem}_{name}.c')
        open(p, 'w', encoding='utf-8').write(head + pre + '\n' + body)
        jobs.append((name, p))
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        res = list(ex.map(lambda j: (j[0], run(j[1], a.tc, a.flags)), jobs))
    for name, r in res:
        if 'err' in r:
            print(f'{name:14s} ERROR {r["err"]}')
            continue
        img = next((x[0] for x in r['a'][5:9] if x[0].startswith('mov') and x[0].endswith(', eax')), '?')
        nb = len(r['bytes_bad'])
        js = ''.join({'je':'e','jne':'n','jmp':'J','ret':'R'}.get(x[0].split()[0],'') for x in r['a'])
        jo = ''.join({'je':'e','jne':'n','jmp':'J','ret':'R'}.get(x[0].split()[0],'') for x in r['b'])
        print(f'{name:14s} {100 * r["acc"]:5.1f}% regs {100 * r["regs"]:5.1f} ops {100 * r["ops"]:5.1f} '
              f'{r["n_mine"]:3d}/{r["n_orig"]} ins {r["bytes_n"]:3d} B bad {nb:3d}  [{img}] {js}{"=" if js==jo else "/"+jo}  {r["toolchain"]} {r["flags"]}')
        if a.show:
            match.print_diff(r['a'], r['b'], r['sm'], True)


if __name__ == '__main__':
    main()
