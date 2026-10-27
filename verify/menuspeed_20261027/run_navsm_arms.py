# D4-FU-MENUSPEED PREREG section 3a: navsm_test Piece 3 settle frames. N0 = base snapshot (git archive ebc7b0c36),
# N1 = this worktree default, N2 = this worktree with MASHED_MENU_ANIM_SPEED=6. Run under test-slot.
import os, re, subprocess, sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
DATA = Path('D:/MashedWT/D4-FU-MENUSPEED_data')
BAT = 're\\analysis\\standalone_menu_sm\\harvest\\build_navsm_test.bat'
ARMS = [('N0', DATA / 'n0src', {}), ('N1', WT, {}), ('N2', WT, {'MASHED_MENU_ANIM_SPEED': '6'})]

for i, (name, cwd, extra) in enumerate(ARMS, 1):
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update(extra)
    p = subprocess.run(['cmd', '/c', BAT], cwd=str(cwd), env=env, capture_output=True, text=True, errors='replace')
    out = p.stdout + p.stderr
    (DATA / (name + '.log')).write_text(out, encoding='utf-8')
    m = re.search(r'settled after (\d+) frames=(\d) ; rec1 slide=(-?\d+) \(expect 0\) : (\w+)', out)
    verdict = 'GREEN' if '[navsm_test] GREEN' in out else 'RED'
    print(f"[{i}/{len(ARMS)}] {name} {verdict} " + (f"frames={m.group(1)} settled={m.group(2)} slide={m.group(3)} {m.group(4)}" if m else 'no Piece 3 line'), flush=True)
