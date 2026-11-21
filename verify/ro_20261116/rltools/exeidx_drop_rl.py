"""PORT-OT-RACE-BIND-PROV4: drop this generator's own wrappers (Match/PortTop_<rva>.cpp starting with the PORT-OT-RACE
header) from exeidx.json, so a re-run of rl_bind_gen.py plans them as 'bind' again instead of 'reuse' (a 'reuse' plan
entry makes --write delete the wrapper). usage: py -3.12 exeidx_drop_rl.py <wt> <exeidx.json>"""
import json, os, sys
wt, p = sys.argv[1], sys.argv[2]
HDR = '// PORT-OT-RACE (2026-11-16): exe-only port body'
idx = json.load(open(p))
src = os.path.join(wt, 'mashedmod', 'src', 'mashed_re')
n = 0
for rva in list(idx):
    keep = []
    for e in idx[rva]:
        f = os.path.join(src, e[1])
        if os.path.exists(f) and open(f, encoding='utf-8', errors='replace').read().startswith(HDR):
            n += 1
            continue
        keep.append(e)
    if keep:
        idx[rva] = keep
    else:
        del idx[rva]
json.dump(idx, open(p, 'w'), indent=0)
print('dropped', n, 'RL wrapper entries;', len(idx), 'rvas left')
