"""RO-67F0B8-CALLER: resolve the raw stack dwords of a detorig.py watch hit (--watch-stack N) into return addresses.

A dword counts as a return address when it lies in MASHED.exe's code section and the bytes just before it decode
(capstone, over original/MASHED.exe.unpatched) as a CALL that ends exactly at it. For a direct CALL rel32 the target is
printed, and the chain check asks that each direct call's target contains the previous frame's return address
(target <= previous return < next function start, by the hooks.csv RVA list).

usage: py -3.12 resolve_stack.py <watch.jsonl> [--exe <Mashed>/original/MASHED.exe.unpatched] [--hooks hooks.csv]
"""
import argparse
import bisect
import csv
import json
import struct

import capstone


def load_pe(path):
    d = open(path, 'rb').read()
    pe = struct.unpack_from('<I', d, 0x3c)[0]
    n = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    secs = []
    for i in range(n):
        o = pe + 24 + opt + 40 * i
        name = d[o:o + 8].rstrip(b'\0').decode('latin1')
        vsz, va, rsz, rp, ch = struct.unpack_from('<IIII', d, o + 8) + (struct.unpack_from('<I', d, o + 36)[0],)
        secs.append((name, 0x400000 + va, max(vsz, rsz), rp, rsz, ch))
    return d, secs


def code_bytes(d, secs, va, n):
    for name, sva, size, rp, rsz, ch in secs:
        if ch & 0x20 and sva <= va < sva + rsz:
            off = rp + (va - sva)
            return d[off:off + n]
    return None


def call_ending_at(md, d, secs, ret):
    """The CALL instruction that ends at ret, or None."""
    for ln in (5, 6, 2, 3, 7, 4):   # e8 rel32, ff 15 [m32], ff d0 (reg), ff 50 xx / ff 14 xx, ff 90 xx xx xx xx ...
        b = code_bytes(d, secs, ret - ln, ln)
        if not b or len(b) != ln:
            continue
        ins = list(md.disasm(b, ret - ln))
        if len(ins) == 1 and ins[0].size == ln and ins[0].mnemonic == 'call':
            return ins[0]
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('watch')
    ap.add_argument('--exe', default='C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched')
    ap.add_argument('--hooks', default='hooks.csv')
    a = ap.parse_args()
    d, secs = load_pe(a.exe)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    starts = []
    try:
        with open(a.hooks, newline='', encoding='utf-8', errors='replace') as f:
            for row in csv.reader(f):
                if row and not row[0].startswith('#') and row[0] != 'rva':
                    try:
                        starts.append(int(row[0], 16))
                    except ValueError:
                        pass
    except OSError:
        pass
    starts = sorted(set(starts))

    def fn_of(va):
        i = bisect.bisect_right(starts, va) - 1
        return starts[i] if i >= 0 else None

    for line in open(a.watch):
        p = json.loads(line)
        print(f"hit n={p.get('n')} tick={p.get('tick')} rec={p.get('rec')} pc={p['pc']} values={p.get('values')} tid={p.get('tid')}")
        print(f"  accurate bt: {p.get('bt')}")
        st = p.get('stack')
        if not st:
            print('  (no stack: run without --watch-stack)')
            continue
        print(f"  esp={st['esp']} ebp={st['ebp']}")
        print(f"  fuzzy bt: {st['fuzzy']}")
        esp = int(st['esp'], 16)
        prev = int(p['pc'], 16)
        print(f"  frame 0: pc {p['pc']} in function {hex(fn_of(prev)) if fn_of(prev) else '?'}")
        for k, w in enumerate(st['words']):
            try:
                v = int(w, 16)
            except ValueError:
                continue
            ins = call_ending_at(md, d, secs, v)
            if ins is None:
                continue
            tgt = ''
            chain = ''
            if ins.op_str.startswith('0x'):
                t = int(ins.op_str, 16)
                tgt = f' -> target {hex(t)}'
                nxt = starts[bisect.bisect_right(starts, t)] if bisect.bisect_right(starts, t) < len(starts) else None
                chain = ' chain OK' if (t <= prev and (nxt is None or prev < nxt)) else ' (target does not contain previous frame)'
            print(f"  [esp+0x{4 * k:03x}] = {hex(v)}  {ins.mnemonic} {ins.op_str} at {hex(ins.address)} in {hex(fn_of(v)) if fn_of(v) else '?'}{tgt}{chain}")
            if chain == ' chain OK':
                prev = v


if __name__ == '__main__':
    main()
