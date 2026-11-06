"""Port bodies and installs for every slice RVA. Run from the worktree:
    py -3.12 D:/MashedWT/MATCH-S15_data/tools/bodies.py > D:/MashedWT/MATCH-S15_data/bodies.tsv"""
import sys, pathlib
sys.path.insert(0, 'scripts')
import rva_body_scan as S

BS = chr(92)
s = S.scan_all()
rvas = [int(x, 16) for x in open('D:/MashedWT/MATCH-S15_data/slice_rvas.txt').read().split()]


def rsp(n):
    return set(open(f'mashedmod/{n}_sources.rsp').read().replace('"', '').replace(BS, '/').split())


exe, asi = rsp('exe'), rsp('asi')
done = {int(p.stem, 16) for p in pathlib.Path('re/match/s15').glob('*.c')}
for r in rvas:
    desc = []
    for t, bb in s['bodies'].items():
        if r in bb:
            tt = t.replace(BS, '/')
            tg = ('E' if tt in exe else '') + ('A' if tt in asi else '')
            desc.append(f"{tt}[{tg or '-'}]:{bb[r]}")
    ins = [f"{t.replace(BS, '/')}:{ii[r]}" for t, ii in s['installs'].items() if r in ii]
    print(f"0x{r:08x}\t{'M' if r in done else '.'}\t{';'.join(desc) or '-'}\t{';'.join(ins) or '-'}")
