"""a4order: stage the part-1-only TrackRenderer.cpp (HEAD + the countdown-clock comment edit) without touching the
working tree, which already holds part 2. Asserts the anchor once. usage: py -3.12 stage_p1.py"""
import subprocess

P = 'mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp'
OLD = ("        // [d3bB2 2026-10-17] FUN_0040fc00's clock add on the pre-GO ticks (0x0040fe5e), default OFF\n"
       "        // (MASHED_AI_COUNTDOWN_CLOCK=1). Same gate")
NEW = ("        // [d3bB2 2026-10-17] FUN_0040fc00's clock add on the pre-GO ticks (0x0040fe5e), default ON since\n"
       "        // a4order 2026-10-17 (MASHED_NO_AI_COUNTDOWN_CLOCK=1 opts out). Same gate")
b = subprocess.run(['git', 'show', 'HEAD:' + P], capture_output=True, check=True).stdout
nl = b'\r\n' if b'\r\n' in b else b'\n'
s = b.decode('utf-8')
old, new = OLD.replace('\n', nl.decode()), NEW.replace('\n', nl.decode())
assert s.count(old) == 1
t = s.replace(old, new, 1).encode('utf-8')
sha = subprocess.run(['git', 'hash-object', '-w', '--stdin'], input=t, capture_output=True, check=True).stdout.decode().strip()
mode = subprocess.run(['git', 'ls-files', '-s', P], capture_output=True, check=True).stdout.decode().split()[0]
subprocess.run(['git', 'update-index', '--cacheinfo', f'{mode},{sha},{P}'], check=True)
print('staged', P, sha)
