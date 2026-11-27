"""Unit test of the binder's `lib` binding kind (BIND-LIB-ROUTE, 2026-11-21), on a fixture library, not real Lua.

  py -3.12 re/tools/bind/test_porttop_bind_lib.py        (from the worktree root; or from anywhere, it chdirs there)

Fixture: re/tools/bind/fixtures/libfix (a two-file C library in the shapes of Lua 4.0's lapi.c) and
re/tools/bind/fixtures/libfix_caller/00400200.c (a matched-source-shaped caller of two library rvas). Cases, one line each
as "[N/M] name PASS|FAIL":
  rules     rules.tsv lib_root / lib lines parse (name and root kept as written, rva lower case, 0x stripped)
  resolve   fix_add (API macro, space before the parameter list) and fix_mul (brace on its own line) resolve
  static    a static definition (static on the same line or the line before) is not linkable: refused
  dup       a symbol defined twice in the globs: refused
  missing   unknown library, missing root, a non-C symbol, a non-rva key: refused
  plan      the planner gives a lib rva ('reuse', symbol, 'lib:...'), ahead of a stop_reason rule
  wrapper   a caller's wrapper #defines FUN_<rva> <symbol> with the exe-body note
  bindings  cmd_bindings writes kind `library` for a lib rva (in memory, BINDINGS.tsv restored)
  link      the wrapper + the fixture library compile and link with the exe's VS2022 flags and the program prints 14
  voidproto BIND-LIB-VOIDPROTO: a (void)-declared lib callee called with arguments keeps them (paste form), the two
            unsupported shapes are refused, and the C unit compiles, links and prints 20
"""
import contextlib, io, os, shutil, subprocess, sys, tempfile, argparse, json

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
os.chdir(ROOT)
sys.path.insert(0, os.path.join(ROOT, 're', 'tools', 'bind'))
import porttop_bind as pb   # noqa: E402

LIB = 're/tools/bind/fixtures/libfix'
CALLER = 're/tools/bind/fixtures/libfix_caller/00400200.c'
ROOTS = {'libfix': (LIB, ['src/*.c'])}
CASES = []


def case(fn):
    CASES.append(fn)
    return fn


@case
def rules():
    d = tempfile.mkdtemp(prefix='libkind_')
    try:
        p = os.path.join(d, 'rules.tsv')
        open(p, 'w', encoding='utf-8').write('# c\nlib_root\tLibFix\t%s\tsrc/*.c src/lib/*.c\nlib\t0x00400100\tLibFix\tfix_add\n' % LIB)
        t = pb.rule_tables(pb.read_rules(p))
        return t['lib_root'] == {'LibFix': (LIB, ['src/*.c', 'src/lib/*.c'])} and t['lib'] == {'00400100': ('LibFix', 'fix_add')}
    finally:
        shutil.rmtree(d, ignore_errors=True)


@case
def resolve():
    ok, errs = pb.resolve_libs(ROOTS, {'00400100': ('libfix', 'fix_add'), '00400110': ('libfix', 'fix_mul')})
    return (not errs and ok['00400100'] == ('fix_add', 'lib:libfix:src/fix_api.c:10')
            and ok['00400110'] == ('fix_mul', 'lib:libfix:src/fix_api.c:15'))


@case
def static():
    ok, errs = pb.resolve_libs(ROOTS, {'00400120': ('libfix', 'fix_helper'), '00400121': ('libfix', 'fix_static2')})
    return not ok and len(errs) == 2 and all('defined 0 times' in e for e in errs)


@case
def dup():
    ok, errs = pb.resolve_libs(ROOTS, {'00400130': ('libfix', 'fix_dup')})
    return not ok and len(errs) == 1 and 'defined 2 times' in errs[0] and 'fix_more.c' in errs[0]


@case
def missing():
    ok, errs = pb.resolve_libs(dict(ROOTS, gone=('re/tools/bind/fixtures/no_such_lib', ['*.c'])),
                               {'00400140': ('nolib', 'fix_add'), '00400150': ('gone', 'fix_add'),
                                '00400160': ('libfix', 'fix-add'), 'zz': ('libfix', 'fix_add')})
    return not ok and len(errs) == 4


def binder():
    b = pb.Binder('raceframe', [])
    b.lib, errs = pb.resolve_libs(ROOTS, {'00400100': ('libfix', 'fix_add'), '00400110': ('libfix', 'fix_mul')})
    assert not errs
    return b


@case
def plan():
    b = binder()
    b.roots = ['00400100', '00400110']
    b.P = dict(b.P, stop_reason={'00400110': 'a stop_reason rule the lib rule must beat'})
    p = b.build_plan()
    return (p['00400100'] == ('reuse', 'fix_add', 'lib:libfix:src/fix_api.c:10')
            and p['00400110'] == ('reuse', 'fix_mul', 'lib:libfix:src/fix_api.c:15') and len(p) == 2)


def caller_wrapper():
    b = binder()
    cs, local, _ = b.callees(CALLER, '00400200')
    b.plan = {'00400200': ('bind', CALLER, 'match', cs, local)}
    for c in cs:
        b.plan[c] = ('reuse',) + b.lib[c]
    return b, b.wrapper_text('00400200')


@case
def wrapper():
    _, (fn, text) = caller_wrapper()
    return (fn.endswith('Match/PortTop_00400200.cpp')
            and '#define FUN_00400100 fix_add   // exe body (lib:libfix:src/fix_api.c:10)' in text
            and '#define FUN_00400110 fix_mul   // exe body (lib:libfix:src/fix_api.c:15)' in text
            and '#define FUN_00400200 PortTop_Bound_00400200' in text)


@case
def bindings():
    d = tempfile.mkdtemp(prefix='libkind_')
    keep_b, keep_sd = pb.BINDINGS, pb.state_dir
    try:
        sd = os.path.join(d, 'state'); os.makedirs(sd)
        json.dump({'00400200': ['bind', CALLER, 'match', ['00400100'], []],
                   '00400100': ['reuse', 'fix_add', 'lib:libfix:src/fix_api.c:10']}, open(os.path.join(sd, 'plan.json'), 'w'))
        bt = os.path.join(d, 'BINDINGS.tsv')
        open(bt, 'w', newline='\n').write('rva\tname\tkind\tsym\tnote\tlevel\tcaller\n')
        pb.BINDINGS, pb.state_dir = bt, (lambda prof: sd)
        with contextlib.redirect_stdout(io.StringIO()):
            pb.cmd_bindings(argparse.Namespace(profile='raceframe', step='LIBTEST', dry=False))
        rows = {l.split('\t')[0]: l.split('\t') for l in open(bt).read().splitlines()[1:]}
        r = rows.get('0x00400100')
        return bool(r) and r[2] == 'library' and r[3] == 'fix_add' and 'lib:libfix' in r[4]
    finally:
        pb.BINDINGS, pb.state_dir = keep_b, keep_sd
        shutil.rmtree(d, ignore_errors=True)


@case
def moved():
    """BIND-MOVED-TSV-AUTO: cmd_moved appends a line with cause + RVA now bound and a totals line, once."""
    d = tempfile.mkdtemp(prefix='moved_')
    keep = (pb.Binder, pb.history_moves, pb.profile_roots)
    try:
        class FakeB:
            def __init__(s, *a, **k): s.sd = d
            def build_plan(s): pass
        mv = [('00400100', '00400100 stop  C2 FUN_00400100', '00400100 reuse PortTop_Bound_00400100 C4 FUN_00400100'),
              ('totals', 'Counter({})', 'Counter({})')]
        pb.Binder, pb.history_moves, pb.profile_roots = FakeB, (lambda b: mv), (lambda p: '00400100')
        ns = argparse.Namespace(profile='bulk', rva=None, cause='TESTROW', dry=False)
        with contextlib.redirect_stdout(io.StringIO()):
            pb.cmd_moved(ns)
            pb.cmd_moved(ns)   # second run adds nothing
        mp = os.path.join(d, pb.PROFILES['bulk']['selftest_history']['moved'])
        lines = open(mp).read().splitlines()
        f = lines[0].split(chr(9))
        return (len(lines) == 2 and f[0] == '00400100' and 'TESTROW: stop -> reuse' in f[3]
                and 'RVA 0x00400100' in f[3] and lines[1].startswith('totals' + chr(9)))
    finally:
        pb.Binder, pb.history_moves, pb.profile_roots = keep
        shutil.rmtree(d, ignore_errors=True)


@case
def link():
    if not os.path.exists(pb.VCVARS):
        print('   link: SKIPPED (no %s)' % pb.VCVARS)
        return None
    _, (fn, text) = caller_wrapper()
    d = tempfile.mkdtemp(prefix='libkind_')
    try:
        text = text.replace('#include "../../../../%s"' % CALLER, '#include "%s"' % os.path.abspath(CALLER).replace('\\', '/'))
        w = os.path.join(d, 'PortTop_00400200.cpp')
        open(w, 'w', newline='\n').write(text)
        m = os.path.join(d, 'main.cpp')
        open(m, 'w').write('#include <cstdio>\nextern "C" int PortTop_Bound_00400200(int);\n'
                           'int main() { std::printf("%d\\n", PortTop_Bound_00400200(3)); return 0; }\n')
        srcs = [os.path.abspath(os.path.join(LIB, 'src', 'fix_api.c')), w, m]
        bat = os.path.join(d, 'b.bat')
        open(bat, 'w').write('@echo off\r\n%scd /d "%s"\r\ncl /nologo /EHa /W3 /O2 /DMASHED_STANDALONE %s /Fe:t.exe\r\n' % (
            '' if os.environ.get('VCINSTALLDIR') else 'call "%s" >nul\r\n' % pb.VCVARS, d, ' '.join('"%s"' % s for s in srcs)))
        r = subprocess.run(['cmd', '/c', bat], capture_output=True)
        exe = os.path.join(d, 't.exe')
        if not os.path.exists(exe):
            print(r.stdout.decode('mbcs', errors='replace')[-1500:])
            return False
        out = subprocess.run([exe], capture_output=True).stdout.decode().strip()
        print('   link: t.exe printed %r, expected 14 = fix_add(3, 2) + fix_mul(3, 3)' % out)
        return out == '14'
    finally:
        shutil.rmtree(d, ignore_errors=True)


VOIDSRC = ('int FUN_00400300(void);\n'
           'int FUN_00400400(int a)\n{\n  return FUN_00400300(7) + FUN_00400300(a);\n}\n')


@case
def voidproto():
    """BIND-LIB-VOIDPROTO: a lib callee the source declares (void) and calls with an argument keeps the argument
    (token-paste form, no drop); a prototype with parameters and a call whose first argument is not a pp-token
    prefix are refused; the C unit compiles and the library reads the kept arguments (7*2 + 3*2 = 20)."""
    ex = pb.call_excess(VOIDSRC, '00400300')
    if ex != (0, 1):
        print('   voidproto: call_excess %r, expected (0, 1)' % (ex,)); return False
    L = pb.lib_keep_args(VOIDSRC, '00400300', 'fix_open', '   // exe body (lib:libfix:src/x.c:1)', ex)
    want = ['#define PT_LIBARG_00400300_void', '#define PT_LIBARG_00400300_7 7', '#define PT_LIBARG_00400300_a a',
            '#define FUN_00400300(...) fix_open(PT_LIBARG_00400300_##__VA_ARGS__)']
    if [l.split('   //')[0] for l in L] != want:
        print('   voidproto: lines %r' % L); return False
    for bad_src, bad_ex in (('int FUN_00400300(int p);\nint f(void) { return FUN_00400300(1, 2); }\n', (1, 2)),
                            ('int FUN_00400300(void);\nint f(int *p) { return FUN_00400300(*p); }\n', (0, 1))):
        try:
            pb.lib_keep_args(bad_src, '00400300', 'fix_open', '', bad_ex)
            print('   voidproto: not refused: %r' % bad_src); return False
        except SystemExit:
            pass
    if not os.path.exists(pb.VCVARS):
        print('   voidproto compile: SKIPPED (no %s)' % pb.VCVARS)
        return True
    d = tempfile.mkdtemp(prefix='libkind_')
    try:
        open(os.path.join(d, 'src.h'), 'w').write(VOIDSRC)
        open(os.path.join(d, 'w.c'), 'w').write('\n'.join(L + ['#include "src.h"', '']))
        open(os.path.join(d, 'lib.c'), 'w').write('int fix_open(int n) { return n * 2; }\n')
        open(os.path.join(d, 'main.c'), 'w').write('#include <stdio.h>\nint FUN_00400400(int a);\n'
                                                   'int main(void) { printf("%d\\n", FUN_00400400(3)); return 0; }\n')
        bat = os.path.join(d, 'b.bat')
        open(bat, 'w').write('@echo off\r\n%scd /d "%s"\r\ncl /nologo /W3 /O2 w.c lib.c main.c /Fe:t.exe\r\n' % (
            '' if os.environ.get('VCINSTALLDIR') else 'call "%s" >nul\r\n' % pb.VCVARS, d))
        r = subprocess.run(['cmd', '/c', bat], capture_output=True)
        exe = os.path.join(d, 't.exe')
        if not os.path.exists(exe):
            print(r.stdout.decode('mbcs', errors='replace')[-1500:]); return False
        out = subprocess.run([exe], capture_output=True).stdout.decode().strip()
        print('   voidproto: t.exe printed %r, expected 20 = fix_open(7) + fix_open(3)' % out)
        return out == '20'
    finally:
        shutil.rmtree(d, ignore_errors=True)


def main():
    bad = 0
    for i, fn in enumerate(CASES, 1):
        try:
            r = fn()
        except SystemExit as e:
            print('   %s: SystemExit %s' % (fn.__name__, e)); r = False
        except Exception as e:   # a crash is a failure, with its text
            print('   %s: %s: %s' % (fn.__name__, type(e).__name__, e)); r = False
        print('[%d/%d] %s %s' % (i, len(CASES), fn.__name__, 'SKIP' if r is None else 'PASS' if r else 'FAIL'))
        bad += r is False
    print('lib kind unit test: %s' % ('PASS' if not bad else 'FAIL %d' % bad))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
