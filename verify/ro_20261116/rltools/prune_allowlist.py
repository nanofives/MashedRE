"""PORT-OT-RACE: drop this row's allowlist lines (re/tools/dual_copy_allowlist.txt, tagged 'PORT-OT-RACE (L-111 (a), L-126),
2026-11-16') whose rva is no longer bound by rltools/plan_rl.json. Run from the worktree root; the build's rva-lint
must still report NEW=0 afterwards."""
import json, os, re
HERE = os.path.dirname(os.path.abspath(__file__))
plan = json.load(open(os.path.join(HERE, 'plan_rl.json')))
bound = {k for k, v in plan.items() if v and v[0] == 'bind'}
p = 're/tools/dual_copy_allowlist.txt'
raw = open(p, 'rb').read(); nl = chr(13) + chr(10) if b'\r\n' in raw else chr(10)
lines = raw.decode().split(nl)
out, drop = [], 0
for l in lines:
    if 'PORT-OT-RACE (L-111 (a), L-126), 2026-11-16' in l:
        m = re.search(r':([0-9a-fA-F]{8})', l.split('#')[0])
        if m and m.group(1).lower() not in bound:
            drop += 1; continue
    out.append(l)
open(p, 'wb').write(nl.join(out).encode())
print('allowlist -', drop)
