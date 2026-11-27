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
BIND-SYNC-REMOVE-STALE (run 57), on temp fixtures with the module globals swapped:
  sync_stale          sync drops the rsp line, allowlist lines and generated file of a TU the profile no longer writes,
                      keeps a hand TU under another tag, a TU another profile writes or reuses, and a bound TU; --dry no-op
  bindings_tu_gone    a non-missing row naming a Match/PortTop_<rva> TU that is gone is rewritten; with the TU it is kept
  bindings_prov_reuse a reuse of a PROVISIONAL.tsv body (0x004ccef0 RwCoreHand_004ccef0) is near-match with REACH-6's
                      note byte for byte; a non-listed reuse stays matched; --rva writes only the rvas named
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
def bindings_foreign_stop():
    """BIND-BINDINGS-BULK-DRIFT: bulk (stop_sym_foreign) keeps a missing row another profile's stop_prefix names, and
    still relabels a missing row no profile's prefix names."""
    d = tempfile.mkdtemp(prefix='fstop_')
    keep_b, keep_sd = pb.BINDINGS, pb.state_dir
    try:
        sd = os.path.join(d, 'state'); os.makedirs(sd)
        json.dump({'00400300': ['stop'], '00400310': ['stop']}, open(os.path.join(sd, 'plan.json'), 'w'))
        bt = os.path.join(d, 'BINDINGS.tsv')
        head = 'rva\tname\tkind\tsym\tnote\tlevel\tcaller\n'
        a_row = '0x00400300\tFUN_00400300\tmissing\tPortTop_AStop_00400300\tAUDIO: named stop\t6\t0x00400000\n'
        o_row = '0x00400310\tFUN_00400310\tmissing\tPortTop_Missing_00400310\tOLD: named stop\t6\t0x00400000\n'
        open(bt, 'w', newline='\n').write(head + a_row + o_row)
        pb.BINDINGS, pb.state_dir = bt, (lambda prof: sd)
        with contextlib.redirect_stdout(io.StringIO()):
            pb.cmd_bindings(argparse.Namespace(profile='bulk', step=None, dry=False))
        rows = {l.split('\t')[0]: l.split('\t') for l in open(bt).read().splitlines()[1:]}
        return (open(bt).read().splitlines()[1] == a_row.rstrip('\n')
                and rows['0x00400310'][3] == 'PortTop_BulkStop_00400310'
                and pb.foreign_stop_owner('bulk', 'PortTop_AStop_00400300') == 'audio'
                and pb.foreign_stop_owner('bulk', 'PortTop_BulkStop_00400300') is None)
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


class _Patch:
    """Swap porttop_bind module globals for one case and put them back."""
    def __init__(self, **kw):
        self.kw, self.old = kw, {}

    def __enter__(self):
        for k, v in self.kw.items():
            self.old[k] = getattr(pb, k); setattr(pb, k, v)

    def __exit__(self, *e):
        for k, v in self.old.items():
            setattr(pb, k, v)


def _sync_fixture(d):
    """Two profiles: tp (audio-shaped header, own_mark null) and other (own_mark). Returns (profiles, state dirs)."""
    src = os.path.join(d, 'src'); os.makedirs(os.path.join(src, 'Match'))
    sd = {p: os.path.join(d, 'state', p) for p in ('tp', 'other')}
    for v in sd.values():
        os.makedirs(v)
    tp = dict(pb.PROFILES['audio'])
    ot = dict(pb.PROFILES['bulk'], own_mark='// OTHER (2026-11-27): exe-only port body', header=None)
    hdr = tp['header']['new'][0].split('%(')[0]
    files = {
        '00400a00': hdr + '00400a00 = re/match/x/00400a00.c\n// body\n',              # tp's, planned reuse: stale
        '00400b00': '// HAND-ROW (2026-11-27): exe-only port body of rva 0x00400b00 = x\n',   # hand TU, another tag: kept
        '00400c00': hdr + '00400c00 = re/match/x/00400c00.c\n',                      # tp's, but other writes it: kept
        '00400d00': hdr + '00400d00 = re/match/x/00400d00.c\n',                      # tp binds it: kept
        '00400f00': hdr + '00400f00 = re/match/x/00400f00.c\n',                      # tp's, but a plan reuses the TU: kept
    }
    for r, t in files.items():
        open(os.path.join(src, 'Match', 'PortTop_%s.cpp' % r), 'w', newline='\n').write(t)
    json.dump({'00400a00': ['reuse', 'Sym_a', 'RwCore/X.cpp'], '00400b00': ['stop'], '00400c00': ['stop'],
               '00400d00': ['bind', 're/match/x/00400d00.c', 'match', [], []], '00400e00': ['stop'],
               '00400f00': ['stop']}, open(os.path.join(sd['tp'], 'plan.json'), 'w'))
    json.dump({'00400f10': ['reuse', 'PortTop_Bound_00400f00', 'Match/PortTop_00400f00.cpp']},
              open(os.path.join(sd['other'], 'plan.json'), 'w'))
    s_ = src.replace('\\', '/')   # newfiles.txt names files as SRC/Match/... (SRC is patched to this folder)
    open(os.path.join(sd['tp'], 'newfiles.txt'), 'w').write(s_ + '/Match/PortTop_00400d00.cpp\n')
    open(os.path.join(sd['other'], 'newfiles.txt'), 'w').write(s_ + '/Match/PortTop_00400c00.cpp\n')
    rsp = os.path.join(d, 'exe_sources.rsp')
    open(rsp, 'wb').write(b'"Core\\A.cpp"\r\n' + b''.join(b'"Match\\PortTop_%s.cpp"\r\n' % r.encode()
                                                          for r in ('00400a00', '00400b00', '00400c00', '00400d00', '00400e00', '00400f00')))
    allow = os.path.join(d, 'allow.txt')
    open(allow, 'w', newline='\n').write(
        'ORIGTOP-ONLY DUP-IN-TARGET:00400a00:Match/PortTop_00400a00.cpp,RwCore/X.cpp   # tp\n'
        'ORIGTOP-ONLY DUP-IN-TARGET:00400b00:Match/PortTop_00400b00.cpp,Y.cpp   # kept\n'
        'ORIGTOP-ONLY DUP-IN-TARGET:00400e00:Z.cpp,Match/PortTop_00400e00.cpp   # tp, file already gone\n')
    return src, sd, rsp, allow, {'tp': tp, 'other': ot}


@case
def sync_stale():
    """BIND-SYNC-REMOVE-STALE (1): sync drops the rsp line, the allowlist lines and the generated file of a TU the profile
    no longer writes (00400a00: planned reuse; 00400e00: file already gone), and keeps a hand TU under another tag, a TU
    another profile writes, a TU the profile binds and a TU another profile's plan reuses. --dry changes nothing."""
    import types
    d = tempfile.mkdtemp(prefix='syncstale_')
    try:
        src, sd, rsp, allow, profs = _sync_fixture(d)
        noseed = types.SimpleNamespace(run=lambda *a_, **k_: types.SimpleNamespace(stdout=''))
        with _Patch(SRC=src.replace('\\', '/'), RSP=rsp, ALLOW=allow, PROFILES=profs, state_dir=lambda p: sd[p], subprocess=noseed):
            r0, a0 = open(rsp, 'rb').read(), open(allow, 'rb').read()
            with contextlib.redirect_stdout(io.StringIO()):
                pb.cmd_sync(argparse.Namespace(profile='tp', step=None, date=None, evidence=None, dry=True))
            dry_ok = open(rsp, 'rb').read() == r0 and open(allow, 'rb').read() == a0
            with contextlib.redirect_stdout(io.StringIO()) as o:
                pb.cmd_sync(argparse.Namespace(profile='tp', step=None, date=None, evidence=None, dry=False))
            with contextlib.redirect_stdout(io.StringIO()):   # a second run is a no-op
                pb.cmd_sync(argparse.Namespace(profile='tp', step=None, date=None, evidence=None, dry=False))
        rsp_ok = open(rsp, 'rb').read() == (b'"Core\\A.cpp"\r\n"Match\\PortTop_00400b00.cpp"\r\n"Match\\PortTop_00400c00.cpp"\r\n'
                                            b'"Match\\PortTop_00400d00.cpp"\r\n"Match\\PortTop_00400f00.cpp"\r\n')
        al = open(allow).read().splitlines()
        files = sorted(os.listdir(os.path.join(src, 'Match')))
        ok = (dry_ok and rsp_ok and len(al) == 1 and '00400b00' in al[0]
              and files == ['PortTop_00400b00.cpp', 'PortTop_00400c00.cpp', 'PortTop_00400d00.cpp', 'PortTop_00400f00.cpp']
              and 'rsp - 2' in o.getvalue() and 'allowlist - 2' in o.getvalue())
        if not ok:
            print('   sync_stale: dry %s rsp %s allow %r files %r' % (dry_ok, rsp_ok, al, files))
        return ok
    finally:
        shutil.rmtree(d, ignore_errors=True)


def _bindings_run(rows, plan, step, rva=None, src=None):
    d = tempfile.mkdtemp(prefix='bindstale_')
    try:
        sd = os.path.join(d, 'state'); os.makedirs(sd)
        json.dump(plan, open(os.path.join(sd, 'plan.json'), 'w'))
        bt = os.path.join(d, 'BINDINGS.tsv')
        open(bt, 'w', newline='\n').write('rva\tname\tkind\texe_symbol\tnote\tlevel\tcaller\n' + ''.join(r + '\n' for r in rows))
        s = src or os.path.join(d, 'src')
        os.makedirs(os.path.join(s, 'Match'), exist_ok=True)
        with _Patch(BINDINGS=bt, state_dir=lambda p: sd, SRC=s.replace('\\', '/')):
            with contextlib.redirect_stdout(io.StringIO()):
                pb.cmd_bindings(argparse.Namespace(profile='audio', step=step, dry=False, rva=rva))
        return open(bt).read().splitlines()[1:], s
    finally:
        shutil.rmtree(d, ignore_errors=True)


@case
def bindings_tu_gone():
    """BIND-SYNC-REMOVE-STALE (2): a non-missing row whose note names an exe TU Match/PortTop_<rva>.cpp that no longer
    exists is rewritten whole from the plan (REACH-5's 0x004ccef0 row before REACH-6); the same row with the TU present
    keeps its text."""
    old = ('0x00400a00\tFUN_00400a00\tnear-match\tPortTop_Bound_00400a00\tOLD-ROW: re/match/x/00400a00.c (L-126 provisional, '
           're/match/PROVISIONAL.tsv, NOT matched, knob path only), Match/PortTop_00400a00.cpp\t3\t')
    plan = {'00400a00': ['reuse', 'Sym_a', 'RwCore/X.cpp']}
    gone, _ = _bindings_run([old], plan, 'NEWROW')
    d = tempfile.mkdtemp(prefix='bindkeep_')
    try:
        os.makedirs(os.path.join(d, 'Match'))
        open(os.path.join(d, 'Match', 'PortTop_00400a00.cpp'), 'w').write('// x\n')
        kept, _ = _bindings_run([old], plan, 'NEWROW', src=d)
    finally:
        shutil.rmtree(d, ignore_errors=True)
    want = '0x00400a00\tFUN_00400a00\tmatched\tSym_a\tNEWROW: existing exe body RwCore/X.cpp\t3\t'
    ok = gone == [want] and kept == [old]
    if not ok:
        print('   bindings_tu_gone: gone %r kept %r' % (gone, kept))
    return ok


@case
def bindings_prov_reuse():
    """BIND-SYNC-REMOVE-STALE (3): a reuse of an exe body that re/match/PROVISIONAL.tsv lists under the same symbol
    (0x004ccef0 RwCoreHand_004ccef0, a hand body) is written near-match with REACH-6's note, byte for byte; a reuse of a
    body not listed there (0x004a2d30 RwCoreCrt_pow) stays matched; --rva writes only the rvas named."""
    miss = '0x%s\tFUN_%s\tmissing\tPortTop_AStop_%s\tPORT-TOP-AUDIOTICK-STOPS: named stop (log + exit code = rva, Match/PortTop_AudioStops.cpp). No matched or admitted body\t3\t'
    rows = [miss % (r, r, r) for r in ('004ccef0', '004a2d30')]
    plan = {'004ccef0': ['reuse', 'RwCoreHand_004ccef0', 'RwCore/RwCore_Hand.cpp'],
            '004a2d30': ['reuse', 'RwCoreCrt_pow', 'RwCore/RwCore_Crt.cpp']}
    want = ('0x004ccef0\tFUN_004ccef0\tnear-match\tRwCoreHand_004ccef0\tRO-AUDIOINIT-45D460-REACH-6: existing exe body '
            'RwCore/RwCore_Hand.cpp (hand body, L-126 provisional, re/match/PROVISIONAL.tsv RwCoreHand_004ccef0; NOT matched, '
            'knob path only; one body for the rva)\t3\t')
    one, _ = _bindings_run(rows, plan, 'RO-AUDIOINIT-45D460-REACH-6', rva=['0x004CCEF0'])
    both, _ = _bindings_run(rows, plan, 'T')
    ok = (one == [want, rows[1]] and both[0].split('\t')[2] == 'near-match'
          and both[1] == '0x004a2d30\tFUN_004a2d30\tmatched\tRwCoreCrt_pow\tT: existing exe body RwCore/RwCore_Crt.cpp\t3\t')
    if not ok:
        print('   bindings_prov_reuse: one %r both %r' % (one, both))
    return ok


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
