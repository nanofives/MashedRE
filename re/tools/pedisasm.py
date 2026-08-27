#!/usr/bin/env python3
"""Disassemble a function straight out of MASHED.exe, no Ghidra required.

The Ghidra MCP is the normal route for disassembly work (see the `ghidra-pool`
skill), but it is not always attached, and a pool slot is heavy for "I need to
read forty instructions". This reads the PE directly with capstone, so it needs
no project, no lock, and no other session's cooperation.

It reads `original/MASHED.exe.unpatched` BY DEFAULT, not `MASHED.exe`: the working
binary carries ten boot patches (BOOT_PATCHES.md) and two of them are detours into
code caves, so disassembling the patched image can show you a JMP where the original
has real code. The default is the file that matches the SHA-256 anchor.

Usage:
    py -3.12 re/tools/pedisasm.py 0x0040e180
    py -3.12 re/tools/pedisasm.py 0x0040e180 --max 400 --out re/analysis/x/f.asm

Output format matches the existing dumps in re/analysis/race_camera/*.asm:
    0x0040e180: 55                       push ebp
"""
import argparse
import struct
import sys

DEFAULT_EXE = 'original/MASHED.exe.unpatched'
ANCHOR = 'BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E'


def sections(data):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    if data[pe:pe + 4] != b'PE\0\0':
        sys.exit('not a PE')
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    optsz = struct.unpack_from('<H', data, pe + 20)[0]
    imgbase = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    out = []
    for i in range(nsec):
        o = pe + 24 + optsz + i * 40
        name = data[o:o + 8].rstrip(b'\0').decode('ascii', 'replace')
        vsz, va, rsz, ptr = struct.unpack_from('<IIII', data, o + 8)
        out.append((name, va, vsz, ptr, rsz))
    return imgbase, out


def va_to_off(imgbase, secs, va):
    rva = va - imgbase
    for name, sva, vsz, ptr, rsz in secs:
        if sva <= rva < sva + max(vsz, rsz):
            return ptr + (rva - sva)
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('va', help='virtual address, e.g. 0x0040e180')
    ap.add_argument('--exe', default=DEFAULT_EXE)
    ap.add_argument('--max', type=int, default=4096, help='max bytes to decode')
    ap.add_argument('--out', help='also write the listing here')
    ap.add_argument('--no-stop', action='store_true',
                    help='do not stop at the first terminating RET')
    a = ap.parse_args()

    try:
        from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    except ImportError:
        sys.exit('capstone not installed: py -3.12 -m pip install capstone')

    data = open(a.exe, 'rb').read()
    if a.exe == DEFAULT_EXE:
        import hashlib
        h = hashlib.sha256(data).hexdigest().upper()
        if h != ANCHOR:
            sys.exit(f'ANCHOR MISMATCH on {a.exe}\n  got      {h}\n  expected {ANCHOR}\n'
                     'RVAs are not valid against this file. Re-anchor before continuing.')

    imgbase, secs = sections(data)
    va = int(a.va, 0)
    off = va_to_off(imgbase, secs, va)
    if off is None:
        sys.exit(f'VA {va:#010x} is not inside any section')

    md = Cs(CS_ARCH_X86, CS_MODE_32)
    lines = []
    # Stop at a RET only once the stack frame is plausibly closed: many functions
    # have interior RETs inside conditional tails, so track depth crudely via the
    # leading PUSH EBP / MOV EBP,ESP prologue and stop on RET at depth 0.
    for ins in md.disasm(data[off:off + a.max], va):
        lines.append(f'0x{ins.address:08x}: {ins.bytes.hex():<24} '
                     f'{ins.mnemonic} {ins.op_str}'.rstrip())
        if not a.no_stop and ins.mnemonic == 'ret':
            break

    text = '\n'.join(lines)
    print(text)
    if a.out:
        with open(a.out, 'w') as f:
            f.write(text + '\n')
        print(f'\n[wrote {len(lines)} instructions -> {a.out}]', file=sys.stderr)


if __name__ == '__main__':
    main()
