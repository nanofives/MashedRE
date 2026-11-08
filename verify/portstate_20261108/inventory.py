"""PORT-STATE-01 inventory: original globals and callees read by the matched sources of the port-plan rows.

Rows: every lane-A/B row of verify/portplan_20261108/lanes.tsv, plus the top level (WinMain 0x00492370, the run
function 0x00492290, MainLoopInit 0x00492770) and their level-1 callees (lane TOP).

For each row the body of its RVA is cut out of its matched source (from its `// MATCH: 0x<rva>` line to the next
one), and every MGLOBAL / MGLOBALA the body names is classified:
  section     .rdata / .data / other, from the PE headers of original/MASHED.exe.unpatched
  init        nonzero = the declared bytes are file-backed and not all zero (the exe's image-pad maps them as 0),
              zero = file-backed but all zero, bss = past SizeOfRawData (zero at load in the original too)
  exe         how the exe TUs (mashedmod/src/mashed_re, exe_sources.rsp + headers) name the address:
              lit  = a 0x literal in code (the port reads/writes the original address itself)
              name = an 8-hex-digit identifier in code (a port-private copy named after the address, or a
                     binding cell such as g_rfxDefault00613290 / MATCH_CELL_*)
              cmt  = only in comments (a port global that shadows it, cited by address)
              -    = not named anywhere in the exe
Blockers per row (a row can carry several):
  RDATA   reads a nonzero file-backed .rdata value: the exe's image-pad maps it as 0 and nothing seeds it, so the
          matched body reads 0 in the exe (the exe column shows whether the port keeps its own copy / literal)
  IDATA   same for nonzero initialized .data (the port may write it later at runtime: see the exe column)
  STATE   reads/writes a .bss/zero .data global that the exe keeps only in a shadow (cmt) or names as a private
          copy (name) but never at the original address (no lit)
  NOWRITE a .bss global the exe never names at all and that no matched body of this row set writes [heuristic:
          the body text assigns it] -> the matched body reads state nothing in the exe maintains
  CALLEE  calls a FUN_ whose hooks.csv port column is `none`, that is not one of the 6 exe thunks and not defined in
          the same matched file
The exe classification is textual (regex over source); it is an inventory aid, not proof. Per-row reasons in the
RESULT are the authority.
"""
import csv, os, re, sys, collections
import pefile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
OUT = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re')
THUNKS = {0x0042b8b0, 0x0042b8c0, 0x0042d5a0, 0x00472f40, 0x004730b0, 0x00472c60}
TOP = [0x00492370, 0x00492290, 0x00492770]

ORIG = os.environ.get('MASHED_UNPATCHED', r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched')
pe = pefile.PE(ORIG, fast_load=True)
BASE = pe.OPTIONAL_HEADER.ImageBase
raw = open(ORIG, 'rb').read()
SECS = []
for s in pe.sections:
    SECS.append((s.Name.rstrip(b'\0').decode(), BASE + s.VirtualAddress, s.Misc_VirtualSize, s.SizeOfRawData,
                 s.PointerToRawData))


def classify_addr(a, size):
    for name, va, vs, rs, pr in SECS:
        if va <= a < va + max(vs, rs):
            off = a - va
            if off >= rs:
                return name, 'bss'
            b = raw[pr + off: pr + min(off + size, rs)]
            return name, ('nonzero' if any(b) else 'zero')
    return 'none', 'none'


SIZES = {'char': 1, 'uchar': 1, 'byte': 1, 'undefined': 1, 'undefined1': 1, 'bool': 1, 'sbyte': 1,
         'short': 2, 'ushort': 2, 'undefined2': 2, 'word': 2, 'double': 8, 'undefined8': 8, 'longlong': 8,
         'ulonglong': 8, '__int64': 8, 'float10': 10}


def tsize(t):
    t = t.strip()
    if '*' in t:
        return 4
    t = t.replace('unsigned', '').replace('signed', '').replace('const', '').strip() or 'int'
    return SIZES.get(t, 4)


# ---- matched-source index: rva -> (file, body text) ----
MARK = re.compile(r'^//\s*MATCH:\s*0x([0-9a-fA-F]{8})', re.M)
GDECL = re.compile(r'MGLOBAL\(\s*([^,]+?)\s*,\s*(\w+)\s*,\s*(0x[0-9a-fA-F]+)\s*\)')
ADECL = re.compile(r'MGLOBALA\(\s*([^,]+?)\s*,\s*(\w+)\s*,\s*([^,]+?)\s*,\s*(0x[0-9a-fA-F]+)\s*\)')
srcidx = {}
filedecl = {}


def load_file(path):
    if path in filedecl:
        return filedecl[path]
    txt = open(path, encoding='utf-8', errors='replace').read()
    decl = {}
    for t, n, a in GDECL.findall(txt):
        decl[n] = (int(a, 16), tsize(t))
    for t, n, cnt, a in ADECL.findall(txt):
        try:
            c = int(eval(cnt, {}, {}))
        except Exception:
            c = 1
        decl[n] = (int(a, 16), tsize(t) * c)
    marks = [(m.start(), int(m.group(1), 16)) for m in MARK.finditer(txt)]
    bodies = {}
    for i, (pos, rva) in enumerate(marks):
        end = marks[i + 1][0] if i + 1 < len(marks) else len(txt)
        bodies[rva] = txt[pos:end]
    filedecl[path] = (txt, decl, bodies)
    return filedecl[path]


for dp, dn, fn in os.walk(os.path.join(ROOT, 're', 'match')):
    if os.sep + 'raw' in dp:
        continue
    for f in fn:
        if f.endswith('.c'):
            p = os.path.join(dp, f)
            _, _, bodies = load_file(p)
            for rva in bodies:
                srcidx.setdefault(rva, p)

# ---- hooks.csv port / exe_file ----
hooks = {}
with open(os.path.join(ROOT, 'hooks.csv'), newline='', encoding='utf-8') as fh:
    for r in csv.DictReader(fh):
        try:
            hooks[int(r['rva'], 16)] = r
        except ValueError:
            pass

# ---- exe address mentions ----
rsp = [l.strip().strip('"') for l in open(os.path.join(ROOT, 'mashedmod', 'exe_sources.rsp')) if l.strip()]
exe_files = [os.path.join(SRC, p) for p in rsp]
for dp, dn, fn in os.walk(SRC):
    for f in fn:
        if f.endswith('.h') or f.endswith('.hpp'):
            exe_files.append(os.path.join(dp, f))
ADDR_LIT = re.compile(r'0x0*([5-9][0-9a-fA-F]{5})(?:[uUlL]+)?\b')
ADDR_NAME = re.compile(r'(?<![0-9a-fA-Fx])00([5-9][0-9a-fA-F]{5})(?![0-9a-fA-F])')
mention = collections.defaultdict(lambda: {'lit': set(), 'name': set(), 'cmt': set()})


def strip_comments(lines):
    out, inblk = [], False
    for ln in lines:
        code, cmt, i = '', '', 0
        while i < len(ln):
            if inblk:
                j = ln.find('*/', i)
                if j < 0:
                    cmt += ln[i:]; i = len(ln)
                else:
                    cmt += ln[i:j]; i = j + 2; inblk = False
            else:
                j1, j2 = ln.find('//', i), ln.find('/*', i)
                if j1 >= 0 and (j2 < 0 or j1 < j2):
                    code += ln[i:j1]; cmt += ln[j1:]; i = len(ln)
                elif j2 >= 0:
                    code += ln[i:j2]; i = j2 + 2; inblk = True
                else:
                    code += ln[i:]; i = len(ln)
        out.append((code, cmt))
    return out


for p in exe_files:
    if not os.path.exists(p):
        continue
    rel = os.path.relpath(p, SRC).replace('\\', '/')
    lines = open(p, encoding='utf-8', errors='replace').read().split('\n')
    for code, cmt in strip_comments(lines):
        for m in ADDR_LIT.finditer(code):
            mention[int(m.group(1), 16)]['lit'].add(rel)
        for m in ADDR_NAME.finditer(code):
            mention[int(m.group(1), 16)]['name'].add(rel)
        for m in ADDR_LIT.finditer(cmt):
            mention[int(m.group(1), 16)]['cmt'].add(rel)
        for m in ADDR_NAME.finditer(cmt):
            mention[int(m.group(1), 16)]['cmt'].add(rel)


INC = re.compile(r'#\s*include\s+"([^"]*match/[^"]+\.c)"')
compiled = set()
for p in exe_files:
    if p.endswith('.cpp') and os.path.exists(p):
        for inc in INC.findall(open(p, encoding='utf-8', errors='replace').read()):
            compiled.add(os.path.normcase(os.path.normpath(os.path.join(os.path.dirname(p), inc))))


def exe_kind(a):
    m = mention.get(a)
    if not m:
        return '-', ''
    for k in ('lit', 'name', 'cmt'):
        if m[k]:
            return k, ';'.join(sorted(m[k]))[:200]
    return '-', ''


# ---- rows ----
rows = []
with open(os.path.join(ROOT, 'verify', 'portplan_20261108', 'lanes.tsv'), newline='') as fh:
    for r in csv.DictReader(fh, delimiter='\t'):
        rows.append((r['lane'], int(r['rva'], 16), r['name'], r['match']))
top_seen = set()
for t in TOP:
    p = srcidx.get(t)
    rows.append(('TOP', t, hooks.get(t, {}).get('name', ''), os.path.relpath(p, ROOT).replace('\\', '/') if p else ''))
    top_seen.add(t)
for t in TOP:
    p = srcidx.get(t)
    if not p:
        continue
    body = load_file(p)[2][t]
    for c in sorted({int(x, 16) for x in re.findall(r'FUN_([0-9a-f]{8})', body)}):
        if c in top_seen:
            continue
        top_seen.add(c)
        cp = srcidx.get(c)
        rows.append(('TOP-L1', c, hooks.get(c, {}).get('name', ''),
                     os.path.relpath(cp, ROOT).replace('\\', '/') if cp else ''))

inv, summ = [], []
for lane, rva, name, mpath in rows:
    path = None
    for mp in (mpath.split(';') if mpath else []):
        if mp and os.path.exists(os.path.join(ROOT, mp)):
            path = os.path.join(ROOT, mp); break
    if path is None:
        path = srcidx.get(rva)
    if not path or not os.path.exists(path):
        summ.append(dict(lane=lane, rva=rva, name=name, src='', compiled=0, port=hooks.get(rva, {}).get('port', ''),
                         blockers='NOSRC', n_glob=0, rdata=0, idata=0, state=0, nowrite=0, callee=0, callees=''))
        continue
    txt, decl, bodies = load_file(path)
    body = bodies.get(rva) or txt
    file_rvas = set(bodies)
    names = [n for n in decl if re.search(r'\b%s\b' % re.escape(n), body)]
    cnt = collections.Counter()
    for n in names:
        a, sz = decl[n]
        sec, init = classify_addr(a, sz)
        kind, files = exe_kind(a)
        writes = bool(re.search(r'\b%s\b(\s*\[[^\]]*\])*\s*(=[^=]|\+=|-=|\|=|&=|\^=|\+\+|--)' % re.escape(n), body)) \
            or bool(re.search(r'(\+\+|--)\s*\b%s\b' % re.escape(n), body))
        blk = ''
        if init == 'nonzero':
            blk = 'RDATA' if sec == '.rdata' else 'IDATA'
        elif init in ('bss', 'zero') and kind in ('cmt', 'name'):
            blk = 'STATE'
        elif init in ('bss', 'zero') and kind == '-' and not writes:
            blk = 'NOWRITE'
        if blk:
            cnt[blk] += 1
        inv.append(dict(lane=lane, rva='0x%08x' % rva, name=name, glob=n, addr='0x%08x' % a, size=sz, section=sec,
                        init=init, exe=kind, writes=int(writes), blocker=blk, exe_files=files))
    miss = []
    for c in sorted({int(x, 16) for x in re.findall(r'FUN_([0-9a-f]{8})', body)} - {rva}):
        if c in THUNKS or c in file_rvas:
            continue
        if hooks.get(c, {}).get('port', 'none') in ('none', ''):
            miss.append(c)
    if miss:
        cnt['CALLEE'] = len(miss)
    summ.append(dict(lane=lane, rva=rva, name=name, src=os.path.relpath(path, ROOT).replace('\\', '/'),
                     compiled=int(os.path.normcase(os.path.normpath(path)) in compiled),
                     port=hooks.get(rva, {}).get('port', ''),
                     blockers=','.join(k for k in ('RDATA', 'IDATA', 'STATE', 'NOWRITE', 'CALLEE') if cnt[k]) or 'none',
                     n_glob=len(names), rdata=cnt['RDATA'], idata=cnt['IDATA'], state=cnt['STATE'],
                     nowrite=cnt['NOWRITE'], callee=cnt['CALLEE'], callees=' '.join('0x%08x' % c for c in miss)))

with open(os.path.join(OUT, 'INVENTORY.tsv'), 'w', newline='') as fh:
    w = csv.DictWriter(fh, list(inv[0].keys()), delimiter='\t', lineterminator='\n')
    w.writeheader(); w.writerows(inv)
with open(os.path.join(OUT, 'ROWS.tsv'), 'w', newline='') as fh:
    keys = list(summ[0].keys())
    w = csv.DictWriter(fh, keys, delimiter='\t', lineterminator='\n')
    w.writeheader()
    for s in summ:
        s = dict(s); s['rva'] = '0x%08x' % s['rva']; w.writerow(s)

# ---- summary ----
lanes = collections.OrderedDict()
for s in summ:
    L = lanes.setdefault(s['lane'] + ('/compiled' if s['compiled'] else '/open'), collections.Counter())
    L['rows'] += 1
    b = s['blockers']
    if b == 'none':
        L['clear'] += 1
    for k in ('RDATA', 'IDATA', 'STATE', 'NOWRITE', 'CALLEE', 'NOSRC'):
        if k in b.split(','):
            L[k] += 1
    if b.split(',') == ['RDATA'] or set(b.split(',')) <= {'RDATA', 'IDATA'} and b != 'none':
        L['only_data'] += 1
    if b == 'STATE' or (set(b.split(',')) <= {'STATE', 'RDATA', 'IDATA'} and 'STATE' in b):
        L['state_or_data_only'] += 1
print('lane\trows\tclear\tRDATA\tIDATA\tSTATE\tNOWRITE\tCALLEE\tNOSRC\tonly_data\tstate_or_data_only')
for k, L in lanes.items():
    print('\t'.join([k] + [str(L[c]) for c in ('rows', 'clear', 'RDATA', 'IDATA', 'STATE', 'NOWRITE', 'CALLEE',
                                                 'NOSRC', 'only_data', 'state_or_data_only')]))
addrs = {}
for i in inv:
    if i['blocker'] in ('RDATA', 'IDATA'):
        addrs[i['addr']] = (i['blocker'], i['size'])
print('distinct seed addresses (RDATA+IDATA): %d, bytes %d' % (len(addrs), sum(v[1] for v in addrs.values())))
st = {i['addr'] for i in inv if i['blocker'] == 'STATE'}
print('distinct STATE addresses: %d' % len(st))
