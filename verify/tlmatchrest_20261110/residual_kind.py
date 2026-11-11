"""TL-MATCH-REST: classify the residual of a near-miss source from `match.py --bytes --show`.

Kinds, judged over every `!` row of the aligned listing:
  SIB-only       every differing pair has the same mnemonic and operands once the terms inside [..] are sorted
  register-only  every differing pair is equal once registers are written as R (and none is SIB-only by itself)
  structural     any inserted / deleted instruction, or a pair that differs in mnemonic / immediate / layout
Relocated operands are compared as match.py prints them (ours 0-based, original absolute), so pairs that differ only
there are skipped: a differing immediate that is a relocation in ours (ours small, original >= 0x400000) is masked.
usage: py -3.12 verify/tlmatchrest_20261110/residual_kind.py <src.c> [...]   -> one line per source
"""
import re, subprocess, sys

REG = re.compile(r'\b(e?[abcd]x|e?[sd]i|e?[sb]p|[abcd][lh])\b')
HEX = re.compile(r'0x[0-9a-f]+|\b\d+\b')


def sib(s):
    return re.sub(r'\[([^\]]+)\]', lambda m: '[' + ' + '.join(sorted(t.strip() for t in m.group(1).replace('- ', '+ -').split('+'))) + ']', s)


def maskabs(a, b):
    """mask operands that are relocations: ours prints a small offset where the original has an image address"""
    na, nb = HEX.findall(a), HEX.findall(b)
    if len(na) != len(nb):
        return a, b
    for x, y in zip(na, nb):
        if x != y and int(y, 0) >= 0x400000:
            a = a.replace(x, '<A>', 1); b = b.replace(y, '<A>', 1)
    return a, b


def classify(src):
    o = subprocess.run(['py', '-3.12', 're/tools/match/match.py', src, '--bytes', '--show'], capture_output=True, text=True)
    t = o.stdout + o.stderr
    head = re.search(r'(\d+\.\d+)% \(regs ([\d.]+)%, ops ([\d.]+)%\)\s+(\d+)/(\d+)', t)
    if 'BYTE-IDENTICAL' in t:
        return 'IDENT', head
    kinds = set()
    for line in t.splitlines():
        if not line.startswith('   ! '):
            continue
        a, _, b = line[5:].partition('|')
        a, b = a.strip(), b.strip()
        if not a or not b:
            kinds.add('structural'); continue
        if a.split()[0] in ('call', 'jmp') or a.split()[0].startswith('j'):
            if a.split()[0] == b.split()[0]:
                continue          # branch / call target printed relative vs absolute
        a, b = maskabs(a, b)
        if a == b:
            continue
        if sib(a) == sib(b):
            kinds.add('SIB'); continue
        if REG.sub('R', a) == REG.sub('R', b):
            kinds.add('register'); continue
        kinds.add('structural')
    if not kinds:
        k = 'encoding-only'
    elif kinds == {'SIB'}:
        k = 'SIB-only'
    elif kinds <= {'SIB', 'register'}:
        k = 'register-only' if kinds == {'register'} else 'register+SIB'
    else:
        k = 'structural'
    return k, head


if __name__ == '__main__':
    for s in sys.argv[1:]:
        k, h = classify(s)
        print('%s\t%s\t%s' % (s, k, '%s%% regs %s ops %s %s/%s' % h.groups() if h else '-'), flush=True)
