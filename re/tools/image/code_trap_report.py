"""Join the "[code-trap]" lines of a mashed_re.log with names (IMG-3-TEXT-TRAP, spec section 4).

The exe's trap (mashedmod/src/mashed_re/Compat/CodeTrap.cpp, MASHED_CODE_TRAP) logs original VAs and module+offset
addresses only. This reader adds, per report line:
  target  -> the pc_funcs.csv function containing it (entry, name) and hooks.csv confidence / port of that entry;
  slot    -> whether verify/portorigtop_20261109/CODEPTR_SLOTS.tsv lists it, and the dword the image holds there
             (--image, mashed_image.bin) as a cross-check;
  ret/eip -> the nearest preceding public of mashed_re.map (--map) for mashed_re.exe+0x... addresses.

Usage: py -3.12 re/tools/image/code_trap_report.py <mashed_re.log> [--map mashed_re.map] [--image mashed_image.bin]
       [--tsv out.tsv]
"""
import argparse
import bisect
import csv
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..', '..'))
sys.path.insert(0, HERE)

LINE = re.compile(r'\[code-trap\] (execute-null|execute|read|write) original 0x([0-9A-Fa-f]{8})(.*)$')
SLOT = re.compile(r'slot 0x([0-9A-Fa-f]{8})')
VIA = re.compile(r'via "([^"]*)"')
RET = re.compile(r'ret ([0-9A-Fa-f]{8}) \(([^)]*)\)')
EIP = re.compile(r'by eip ([0-9A-Fa-f]{8}) \(([^)]*)\)')
MODOFF = re.compile(r'mashed_re(?:_ddk)?\.exe\+0x([0-9A-Fa-f]+)')


def load_funcs():
    rows = []
    with open(os.path.join(ROOT, 're', 'console', 'pc_funcs.csv'), newline='') as f:
        for r in csv.DictReader(f):
            rows.append((int(r['entry'], 16), int(r['body_end'], 16), r['name']))
    rows.sort()
    return rows


def load_hooks():
    out = {}
    with open(os.path.join(ROOT, 'hooks.csv'), newline='', encoding='utf-8', errors='replace') as f:
        for r in csv.DictReader(f):
            try:
                out[int(r['rva'], 16)] = (r.get('name', ''), r.get('confidence', ''), r.get('port', ''))
            except (ValueError, TypeError):
                pass
    return out


def load_slots():
    out = {}
    p = os.path.join(ROOT, 'verify', 'portorigtop_20261109', 'CODEPTR_SLOTS.tsv')
    with open(p, newline='') as f:
        for r in csv.DictReader(f, delimiter='\t'):
            out[int(r['slot'], 16)] = (int(r['target'], 16), r['bind_kind'], r['exe_symbol'])
    return out


def load_map(path):
    base, syms = 0x00010000, []
    pat = re.compile(r'^\s*[0-9A-Fa-f]{4}:[0-9A-Fa-f]{8}\s+(\S+)\s+([0-9A-Fa-f]{8})\s')
    with open(path, errors='replace') as f:
        for ln in f:
            m = re.search(r'Preferred load address is ([0-9A-Fa-f]+)', ln)
            if m:
                base = int(m.group(1), 16)
            m = pat.match(ln)
            if m and int(m.group(2), 16):
                syms.append((int(m.group(2), 16), m.group(1)))
    syms.sort()
    return base, syms


def image_dword(ranges, va):
    for _name, rva, data, _fnv in ranges:
        if rva <= va and va + 4 <= rva + len(data):
            return struct.unpack_from('<I', data, va - rva)[0]
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('log')
    ap.add_argument('--map')
    ap.add_argument('--image')
    ap.add_argument('--tsv')
    a = ap.parse_args()
    funcs = load_funcs()
    entries = [r[0] for r in funcs]
    hooks = load_hooks()
    slots = load_slots()
    base, syms = load_map(a.map) if a.map else (0x00010000, [])
    keys = [s[0] for s in syms]
    ranges = []
    if a.image:
        import mashed_image
        _sha, ranges = mashed_image.parse(open(a.image, 'rb').read())

    def func_of(va):
        i = bisect.bisect_right(entries, va) - 1
        if i >= 0 and funcs[i][0] <= va < max(funcs[i][1], funcs[i][0] + 1):
            e, _end, name = funcs[i]
            h = hooks.get(e, ('', '', ''))
            return '0x%08x %s%s conf=%s port=%s' % (e, h[0] or name, '' if e == va else '+0x%x' % (va - e), h[1] or '-',
                                                   h[2] or '-')
        return 'no pc_funcs entry'

    def sym_of(modoff):
        m = MODOFF.search(modoff or '')
        if not m or not keys:
            return modoff or '-'
        va = base + int(m.group(1), 16)
        i = bisect.bisect_right(keys, va) - 1
        return '%s (%s+0x%x)' % (modoff, syms[i][1], va - syms[i][0]) if i >= 0 else modoff

    rows = []
    for ln in open(a.log, errors='replace'):
        m = LINE.search(ln)
        if not m:
            continue
        kind, va, rest = m.group(1), int(m.group(2), 16), m.group(3)
        sm, vm, rm, em = SLOT.search(rest), VIA.search(rest), RET.search(rest), EIP.search(rest)
        slot = int(sm.group(1), 16) if sm else None
        known = slots.get(slot) if slot is not None else None
        img = image_dword(ranges, slot) if (slot is not None and ranges) else None
        rows.append({
            'kind': kind, 'target': '0x%08x' % va, 'target_func': func_of(va) if va >= 0x00401000 else '-',
            'slot': '0x%08x' % slot if slot is not None else '?',
            'slot_listed': ('yes %s %s' % (known[1] or '-', known[2] or '-')) if known else 'no',
            'image_dword': '0x%08x' % img if img is not None else '-',
            'via': vm.group(1) if vm else '-',
            'site': sym_of(rm.group(2)) if rm else (sym_of(em.group(2)) if em else '-'),
        })
    cols = ['kind', 'target', 'target_func', 'slot', 'slot_listed', 'image_dword', 'via', 'site']
    w = csv.writer(open(a.tsv, 'w', newline='') if a.tsv else sys.stdout, delimiter='\t', lineterminator='\n')
    w.writerow(cols)
    for r in rows:
        w.writerow([r[c] for c in cols])
    print('%d [code-trap] report lines' % len(rows), file=sys.stderr)


if __name__ == '__main__':
    main()
