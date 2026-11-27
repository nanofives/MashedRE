"""Survey for direct MASHED.exe / mashed_re.exe launches that bypass MashedLock (HB-LOCK-BARE-LAUNCH-SURVEY).

Usage: py -3.12 re/tools/bare_launch_survey.py [--root <dir>] [--out <tsv>] [--selftest]

A launch site is a line holding a launcher token (Popen, subprocess.run/call, Start-Process, os.startfile,
os.system, frida spawn, device.spawn, .spawn( ) with a game-exe token within WINDOW lines (same call).
A file is LOCKED when it mentions mashed_lock / MashedLock / mashed_lock.py anywhere, else BARE.
Output TSV: status, file, line, launcher, text. Read-only, launches nothing.
"""
import os
import re
import sys

SCAN_DIRS = ['re', 'scripts', 'verify', 'mashedmod']
EXTS = ('.py', '.ps1', '.sh', '.bat', '.cmd')
LAUNCH = re.compile(r'(Popen\(|subprocess\.(run|call|check_call|check_output)\(|Start-Process|startfile\(|os\.system\(|'
                    r'os\.spawn\w*\(|\.spawn\(|frida\.spawn|Invoke-Expression|^\s*&\s)')
GAME = re.compile(r'(MASHED\.exe|mashed_re\.exe|mashed_re_dev|MASHED_EXE|\bMASHED\b|\bmashed_re\b|ORIG_EXE|ORIGINAL_EXE|'
                  r'GAME_EXE|STANDALONE|\bSA_EXE\b|\bEXE\b|\bexe_path\b|\bexe\b)', re.I)
HARD = re.compile(r'(MASHED\.exe|mashed_re\.exe)', re.I)
LOCK = re.compile(r'(mashed_lock|MashedLock|mashed_launch_guard)')
WINDOW = 6
SKIP_PARTS = ('prior_art', 'node_modules', '.git', 'mashed_lock')


def scan_file(path):
    try:
        text = open(path, encoding='utf-8', errors='replace').read()
    except OSError:
        return []
    lines = text.splitlines()
    locked = bool(LOCK.search(text))
    hits = []
    for i, ln in enumerate(lines):
        s = ln.strip()
        if s.startswith('#') or s.startswith('//') or s.startswith('REM '):
            continue
        if not LAUNCH.search(ln):
            continue
        chunk = '\n'.join(lines[i:i + WINDOW])
        if not GAME.search(chunk):
            continue
        # skip taskkill / tasklist style and pure reads
        if re.search(r'(taskkill|tasklist|Get-Process|Stop-Process)', chunk):
            continue
        hard = bool(HARD.search(chunk))
        hits.append(('LOCKED' if locked else 'BARE', i + 1, 'hard' if hard else 'soft', s[:140]))
    return hits


def walk(root):
    for d in SCAN_DIRS:
        base = os.path.join(root, d)
        for dp, dn, fn in os.walk(base):
            dn[:] = [x for x in dn if x not in ('.git', 'node_modules', 'prior_art', '__pycache__')]
            for f in fn:
                if f.endswith(EXTS) and not any(p in f for p in ('mashed_lock',)):
                    yield os.path.join(dp, f)


def main(argv):
    root = os.getcwd()
    out = None
    if '--root' in argv:
        root = argv[argv.index('--root') + 1]
    if '--out' in argv:
        out = argv[argv.index('--out') + 1]
    rows = []
    for p in walk(root):
        rel = os.path.relpath(p, root).replace('\\', '/')
        for st, ln, kind, txt in scan_file(p):
            rows.append((st, rel, ln, kind, txt))
    rows.sort(key=lambda r: (r[0], r[1], r[2]))
    lines = ['status\tfile\tline\tmatch\ttext'] + ['%s\t%s\t%d\t%s\t%s' % r for r in rows]
    if out:
        open(out, 'w', encoding='utf-8', newline='\n').write('\n'.join(lines) + '\n')
    bare = [r for r in rows if r[0] == 'BARE']
    print('sites=%d bare=%d bare_hard=%d locked=%d bare_files=%d' % (
        len(rows), len(bare), sum(1 for r in bare if r[3] == 'hard'), len(rows) - len(bare),
        len(set(r[1] for r in bare))))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
