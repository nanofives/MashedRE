"""D4-WSH-BODYDIFF: per-function git log -L over the stale-candidate C4 rows.

For each row of stale58.tsv (rva, name, body file, evidence date):
  1. find the function's C++ name (RH_ScopedInstall(X, 0x<rva>) wins, else the
     hooks.csv name) and its definition range at <rev> (signature line .. closing brace);
  2. git log -L <start>,<end>:<file> <rev>, keep commits with committer date > evidence
     date (same-day commits are listed separately, not counted);
  3. classify each kept commit's hunk: comment-only (the -/+ lines are equal once
     comments and whitespace are stripped) or body-changed;
     A kept commit that also touches one of the row's frida_diff paths is tagged
     evidence-commit (the evidence was produced on that body) and not counted;
     a commit whose parent had none of the range is tagged created;
  4. dual-copy rows (hooks.csv file != exe_file, both .cpp): both copies are checked,
     the exe copy through EXE_SITES when it is inline code with no function name;
  5. depth-1 callees: identifiers called in the body that have exactly one definition
     under mashedmod/src at <rev>; run steps 2-3 on each and report callee-changed.
Usage: py -3.12 bodydiff.py <rev> <stale.tsv> <out.tsv> [--detail out.txt]
"""
import re, subprocess, sys
from functools import lru_cache

REV, STALE, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
DETAIL = sys.argv[5] if len(sys.argv) > 5 and sys.argv[4] == '--detail' else None
SRC = 'mashedmod/src'
KEYWORDS = set('''if for while switch return sizeof alignof decltype static_cast
reinterpret_cast const_cast dynamic_cast catch defined __asm _asm asm offsetof
RH_ScopedInstall assert static_assert __declspec __pragma'''.split())


def git(*a):
    return subprocess.run(['git', *a], capture_output=True, text=True,
                          encoding='utf-8', errors='replace').stdout


@lru_cache(None)
def blob(path):
    return git('show', f'{REV}:{path}').splitlines()


def strip_code(lines):
    """Return per-line code text with comments, strings and char literals blanked."""
    out, in_block = [], False
    for ln in lines:
        s, i, buf = ln, 0, []
        while i < len(s):
            if in_block:
                j = s.find('*/', i)
                if j < 0:
                    i = len(s)
                else:
                    in_block, i = False, j + 2
                continue
            c = s[i]
            if s.startswith('//', i):
                break
            if s.startswith('/*', i):
                in_block, i = True, i + 2
                continue
            if c in '"\'':
                j = i + 1
                while j < len(s) and s[j] != c:
                    j += 2 if s[j] == '\\' else 1
                buf.append(c + c)
                i = j + 1
                continue
            buf.append(c)
            i += 1
        out.append(''.join(buf))
    return out


def find_def(path, fname):
    """All (start, end) 1-based line ranges defining fname at top level of path."""
    lines = blob(path)
    code = strip_code(lines)
    pat = re.compile(r'(?<![\w.>])(?:[\w]+::)*' + re.escape(fname) + r'\s*\(')
    ranges, depth, ns_depths = [], 0, []
    # Pass 1: depth at the start of each line, treating namespace / extern "C" braces as 0.
    depth_at = []
    pend_ns = False
    for ln_raw, ln in zip(lines, code):
        depth_at.append(depth - len(ns_depths))
        if re.search(r'\bnamespace\b[^;]*$', ln) or re.search(r'extern\s+"', ln_raw):
            pend_ns = True
        for ch in ln:
            if ch == '{':
                depth += 1
                if pend_ns:
                    ns_depths.append(depth)
                    pend_ns = False
            elif ch == '}':
                if ns_depths and ns_depths[-1] == depth:
                    ns_depths.pop()
                depth -= 1
            elif ch == ';':
                pend_ns = False
    for idx, ln in enumerate(code):
        if depth_at[idx] != 0 or 'RH_ScopedInstall' in ln:
            continue
        m = pat.search(ln)
        if not m:
            continue
        # scan forward from the match: first top-level '{' or ';' decides.
        par, j, k, found = 0, idx, m.end() - 1, None
        while j < len(code) and found is None:
            t = code[j]
            while k < len(t):
                ch = t[k]
                if ch == '(':
                    par += 1
                elif ch == ')':
                    par -= 1
                elif par == 0 and ch == ';':
                    found = 'decl'
                    break
                elif par == 0 and ch == '{':
                    found = (j, k)
                    break
                k += 1
            j, k = j + 1, 0
        if not isinstance(found, tuple):
            continue
        bj, bk = found
        d, end = 0, None
        for jj in range(bj, len(code)):
            t = code[jj][bk:] if jj == bj else code[jj]
            for ch in t:
                if ch == '{':
                    d += 1
                elif ch == '}':
                    d -= 1
                    if d == 0:
                        end = jj
                        break
            if end is not None:
                break
        if end is not None:
            ranges.append((idx + 1, end + 1))
    return ranges


def cxx_name(path, rva, name):
    for ln in blob(path):
        m = re.search(r'RH_ScopedInstall\(\s*([\w:]+)\s*,\s*0x0*' + rva.lstrip('0') + r'\b', ln, re.I)
        if m:
            return m.group(1).split('::')[-1]
    return name.split('::')[-1]


# exe copies that are not a named function at REV (exe_main inlines the two threshold
# helpers, MenuNavSM folds the prompt-strip heads into Nav()).
EXE_SITES = {
    '004325c0': ('fn', 'Anim_Tick'),
    '00432b30': ('fn', 'Nav'),
    '0040b890': ('lines', (4115, 4128)),   # kParticipants/kEa64/rule_now + FUN_0040b890 arm
    '0040b8e0': ('lines', (4115, 4128)),   # same inputs + FUN_0040b8e0 arm
}


def classify_hunk(minus, plus, old):
    if not old and plus:
        return 'created'
    a = ''.join(re.sub(r'\s+', '', l) for l in strip_code(minus))
    b = ''.join(re.sub(r'\s+', '', l) for l in strip_code(plus))
    return 'comment-only' if a == b else 'body-changed'


@lru_cache(None)
def log_L(path, start, end):
    """[(sha, date, subject, cls, old_text)] for every commit git log -L reports on the
    range; old_text is the range as it stood in the commit's parent (context + '-')."""
    out = git('log', f'-L{start},{end}:{path}', '--format=@@C %H %cs %s', REV)
    res, cur, minus, plus, old, in_hunk = [], None, [], [], [], False

    def flush():
        if cur:
            res.append((*cur, classify_hunk(minus, plus, old), tuple(old)))
    for ln in out.splitlines():
        if ln.startswith('@@C '):
            flush()
            _, sha, date, *subj = ln.split(' ')
            cur, minus, plus, old, in_hunk = (sha[:10], date, ' '.join(subj)), [], [], [], False
        elif ln.startswith('@@ '):
            in_hunk = True
        elif not in_hunk or ln.startswith('\\'):
            continue
        elif ln.startswith('-'):
            minus.append(ln[1:])
            old.append(ln[1:])
        elif ln.startswith('+'):
            plus.append(ln[1:])
        elif ln.startswith(' ') or ln == '':
            old.append(ln[1:])
    flush()
    return res


def norm(lines):
    return ''.join(re.sub(r'\s+', '', l) for l in strip_code(list(lines)))


def net(path, s, e, kept):
    """Range at the parent of the oldest kept commit vs the range at REV."""
    if not kept:
        return 'none'
    oldest = kept[-1]                     # git log is newest-first
    if not oldest[4]:
        return 'created'
    a, b = norm(oldest[4]), norm(blob(path)[s - 1:e])
    return 'net-equal' if a == b else 'net-changed'


@lru_cache(None)
def src_files():
    return [p for p in git('ls-tree', '-r', '--name-only', REV, SRC).splitlines()
            if p.endswith(('.cpp', '.h', '.hpp', '.inl'))]


@lru_cache(None)
def def_sites(fname):
    hits = git('grep', '-l', '-w', fname, REV, '--', SRC).splitlines()
    sites = []
    for h in hits:
        p = h.split(':', 1)[1]
        if p.endswith(('.cpp', '.h', '.hpp', '.inl')):
            for r in find_def(p, fname):
                sites.append((p, *r))
    return sites


def callees(path, start, end, self_name):
    code = strip_code(blob(path)[start - 1:end])
    names = set()
    for ln in code[1:]:
        for m in re.finditer(r'(?<![\w.>:])([A-Za-z_]\w*)\s*\(', ln):
            n = m.group(1)
            if n not in KEYWORDS and n != self_name and not n.isupper():
                names.add(n)
    return sorted(names)


def after(entries, ev):
    return [e for e in entries if e[1] > ev], [e for e in entries if e[1] == ev]


@lru_cache(None)
def touched(sha):
    return set(git('show', '--name-only', '--format=', sha).splitlines())


def hooks_rows():
    import csv
    txt = git('show', f'{REV}:hooks.csv')
    return {r['rva']: r for r in csv.DictReader(l for l in txt.splitlines() if not l.startswith('#'))}


HR = hooks_rows()


def check(rva, path, s, e, fn, ev, evid_paths, tag):
    hist = log_L(path, s, e)
    post, same = after(hist, ev)
    kept = []
    detail.append(f'   [{tag}] {fn} {path}:{s}-{e}')
    for c in hist:
        ec = bool(evid_paths & touched(c[0])) if c in post else False
        t = 'POST' if c in post else ('SAME' if c in same else 'pre ')
        detail.append(f'     {t} {c[0]} {c[1]} {c[3]:13s}{" evidence-commit" if ec else ""} {c[2][:90]}')
        if c in post and not ec:
            kept.append(c)
    nb = net(path, s, e, kept)
    body = [c for c in kept if c[3] in ('body-changed', 'created')] if nb != 'net-equal' else []
    com = [c for c in kept if c[3] == 'comment-only' or nb == 'net-equal']
    detail.append(f'     net since evidence: {nb}')
    cchg = []
    for cn in callees(path, s, e, fn):
        sites = def_sites(cn)
        if len(sites) != 1:
            if len(sites) > 1:
                detail.append(f'     callee {cn}: {len(sites)} definitions, skipped')
            continue
        cp, cs, ce = sites[0]
        cpost = [c for c in after(log_L(cp, cs, ce), ev)[0] if not (evid_paths & touched(c[0]))]
        cn_net = net(cp, cs, ce, cpost)
        cb = [c for c in cpost if c[3] in ('body-changed', 'created')] if cn_net != 'net-equal' else []
        detail.append(f'     callee {cn} {cp}:{cs}-{ce} net={cn_net} post-ev body commits: '
                      + (', '.join(f'{c[0]} {c[1]} {c[3]}' for c in cb) or 'none'))
        if cb:
            cchg.append(f'{cn}@' + '/'.join(c[0][:9] for c in cb))
    verdict = ('body-changed' if body else 'callee-changed' if cchg
               else 'comment-only' if com else 'unchanged')
    return verdict, body, com, cchg, same


rows = [l.rstrip('\n').split('\t') for l in open(STALE, encoding='utf-8')][1:]
summary, detail = [], []
for rva, name, path, ev, gd in rows:
    hr = HR[rva]
    evid = {p.strip() for p in (hr['frida_diff'] or '').split(';') if p.strip()}
    asi_file = (hr['file'] or '').strip()
    targets = []   # (tag, path, fn, range-or-None)
    if asi_file.endswith('.cpp') and asi_file != path:
        targets.append(('asi', asi_file, cxx_name(asi_file, rva, name), None))
        if rva in EXE_SITES:
            k, v = EXE_SITES[rva]
            targets.append(('exe', path, v if k == 'fn' else f'inline@{v[0]}-{v[1]}',
                            v if k == 'lines' else None))
        else:
            targets.append(('exe', path, cxx_name(path, rva, name), None))
    else:
        exe_file = (hr['exe_file'] or '').strip()
        tag = ('asi' if not exe_file else 'asi+exe' if exe_file == asi_file
               else 'exe (file=.md)')
        targets.append((tag, path, cxx_name(path, rva, name), None))
    detail.append(f'== {rva} {name} ev={ev} evidence={sorted(evid)}')
    for tag, p, fn, rng in targets:
        if not blob(p):
            last = git('log', '-1', '--format=%h %cs %s', REV, '--', p).strip()
            summary.append([rva, name, tag, p, ev, fn, '', 'FILE-MISSING', '', '', '', ''])
            detail.append(f'   [{tag}] {p} absent at {REV}; last touched by {last}')
            continue
        if rng is None:
            rngs = find_def(p, fn)
            if not rngs:
                summary.append([rva, name, tag, p, ev, fn, '', 'NO-DEF', '', '', '', ''])
                detail.append(f'   [{tag}] definition of {fn} not found in {p}')
                continue
            s, e = rngs[0]
        else:
            s, e = rng
        v, body, com, cchg, same = check(rva, p, s, e, fn, ev, evid, tag)
        summary.append([rva, name, tag, p, ev, fn, f'{s}-{e}', v,
                        ' '.join(f'{c[0][:9]}({c[1]},{c[3]})' for c in body),
                        ' '.join(f'{c[0][:9]}({c[1]})' for c in com),
                        ' '.join(cchg),
                        ' '.join(f'{c[0][:9]}:{c[3]}' for c in same)])

with open(OUT, 'w', encoding='utf-8', newline='') as fh:
    fh.write('rva\tname\tcopy\tbody\tevidence_date\tcxx_fn\tlines\tverdict\tbody_commits\t'
             'comment_commits\tcallee_changed\tsame_day_commits\n')
    for r in summary:
        fh.write('\t'.join(r) + '\n')
if DETAIL:
    open(DETAIL, 'w', encoding='utf-8').write('\n'.join(detail) + '\n')
from collections import Counter
print(Counter(r[7] for r in summary))
