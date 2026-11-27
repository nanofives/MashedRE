"""Insert the mashed_launch_guard import into the bare launcher scripts of the survey (HB-LOCK-BARE-LAUNCH-SURVEY).

Usage: py -3.12 re/tools/inject_launch_guard.py <survey.tsv> [--apply]
Takes the BARE rows of the survey for *.py under re/ and scripts/, minus the EXCLUDE list (rows that are not game
launches), and inserts one line before the first non-__future__ top-level import. Dry run unless --apply.
Idempotent: a file that already names mashed_launch_guard is skipped. Every edited file is compiled.
"""
import ast
import csv
import os
import py_compile
import sys

ROOT = os.getcwd()
ORCH = os.path.join(ROOT, 're', 'orchestrator')
EXCLUDE = {
    're/tools/bind/test_porttop_bind_lib.py', 're/tools/ddkobj/mathprobe/ddkcrt_rename.py',
    're/tools/gen_portstate_seed.py', 're/tools/spawner_audit.py', 're/tools/veccap/replay_offline.py',
    're/tools/veccap/veccap_registry.py', 're/tools/xref.py', 're/frida/run_diff_parallel.py',
    're/tools/bare_launch_survey.py', 're/tools/inject_launch_guard.py',
}
MARK = 'mashed_launch_guard'


def line_for(path):
    rel = os.path.relpath(ORCH, os.path.dirname(os.path.abspath(path))).replace('\\', '/')
    return ("import os as _gos, sys as _gsys; _gsys.path.append(_gos.path.join(_gos.path.dirname(_gos.path.abspath(__file__)), "
            "'%s')); import mashed_launch_guard  # HB-LOCK-BARE-LAUNCH-SURVEY: game launches take MashedLock\n" % rel)


def insert_at(src):
    tree = ast.parse(src)
    for node in tree.body:
        if isinstance(node, ast.ImportFrom) and node.module == '__future__':
            continue
        if isinstance(node, (ast.Import, ast.ImportFrom)):
            return node.lineno - 1
    return None


def main():
    tsv, apply = sys.argv[1], '--apply' in sys.argv
    files = sorted({r['file'] for r in csv.DictReader(open(tsv, encoding='utf-8'), delimiter='\t')
                    if r['status'] == 'BARE' and r['file'].endswith('.py') and r['file'].split('/')[0] in ('re', 'scripts')
                    and r['file'] not in EXCLUDE})
    done = skipped = failed = 0
    for f in files:
        p = os.path.join(ROOT, f)
        raw = open(p, 'rb').read()
        src = raw.decode('utf-8', errors='replace')
        crlf = b'\r\n' in raw
        if MARK in src:
            skipped += 1
            continue
        try:
            at = insert_at(src)
        except SyntaxError as e:
            print('FAIL parse', f, e)
            failed += 1
            continue
        if at is None:
            print('FAIL no import', f)
            failed += 1
            continue
        lines = src.splitlines(keepends=True)
        ins = line_for(p)
        if crlf:
            ins = ins.replace('\n', '\r\n')
        lines.insert(at, ins)
        if apply:
            open(p, 'wb').write(''.join(lines).encode('utf-8'))
            try:
                compile(open(p, 'rb').read(), p, 'exec')   # syntax check only, writes no .pyc
            except Exception as e:
                print('FAIL compile', f, e)
                failed += 1
                continue
        done += 1
        print(('EDIT ' if apply else 'WOULD ') + f + ':%d' % (at + 1))
    print('files=%d edited=%d skipped=%d failed=%d apply=%s' % (len(files), done, skipped, failed, apply))
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
