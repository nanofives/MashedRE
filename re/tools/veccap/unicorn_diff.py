#!/usr/bin/env python3
# re/tools/veccap/unicorn_diff.py
#
# Registry-driven emulator differential (generalized 2026-07-16). Executes the
# ORIGINAL machine code of every registry function inside Unicorn (QEMU x86) —
# no game process, no Frida — and bit-compares results (return f32 + out
# buffers) against the live-captured ground truth from capture_vectors.py.
#
# Per-kind caller stubs round ST0 to f32 the way the game's callers do
# (fstp m32), avoiding host-side 80-bit register readout:
#   f_f      : push bits          ; call ; fstp [RESULT]
#   f_ptrN   : push ARGBUF        ; call ; fstp [RESULT]
#   f_out_in : push ARGBUF, OUTBUF; call ; fstp [RESULT]   (cdecl, right-to-left)
#
# Degenerate-flagged vectors are skipped (the originals call RW error-record
# plumbing on that path, which touches live-game state; counted, not hidden).
import json
import struct
import sys
from pathlib import Path

import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import UC_X86_REG_ESP

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE))
from veccap_registry import FUNCS, STATIC_READS, is_degenerate  # noqa: E402

MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
IMAGE_BASE = 0x00400000

SCRATCH   = 0x0F000000
STUB      = SCRATCH
RESULT    = SCRATCH + 0x100
ARGBUF    = SCRATCH + 0x200
OUTBUF    = SCRATCH + 0x300
STACK_TOP = 0x0E800000


def build_uc(capture):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    pe = pefile.PE(str(MASHED_EXE), fast_load=True)
    size_img = (pe.OPTIONAL_HEADER.SizeOfImage + 0xFFF) & ~0xFFF
    uc.mem_map(IMAGE_BASE, size_img)
    uc.mem_write(IMAGE_BASE, pe.header)
    for s in pe.sections:
        data = s.get_data()
        if data:
            uc.mem_write(IMAGE_BASE + s.VirtualAddress, data)

    r = capture['regions']

    def map_page(addr, size):
        # map each 4 KB page independently so a span that partially overlaps an
        # existing mapping (the two LUTs share pages) still gets its free pages.
        base = addr & ~0xFFF
        end = (addr + size + 0xFFF) & ~0xFFF
        for p in range(base, end, 0x1000):
            try:
                uc.mem_map(p, 0x1000)
            except Exception:
                pass  # already mapped — fine

    uc.mem_write(0x007D3FF8, struct.pack('<II', r['rw_globals'], r['rw_offset']))
    map_page(r['slot_addr'], 8)
    uc.mem_write(r['slot_addr'], struct.pack('<II', r['lut_root'], r['lut_inv_root']))
    for key, root in (('lut_hex', r['lut_root']), ('lut_inv_hex', r['lut_inv_root'])):
        data = bytes.fromhex(r[key])
        map_page(root, len(data))
        uc.mem_write(root, data)
    # STATIC_READS live inside the image (already mapped from the file)

    uc.mem_map(STACK_TOP - 0x10000, 0x10000)
    uc.mem_map(SCRATCH, 0x10000)
    return uc


def build_stub(uc, rva):
    """Static caller stub: `call target ; fstp dword [RESULT]`. Args are placed
    directly on the stack per vector (cdecl reads them at [ESP+4]...), so the
    stub never self-modifies — Unicorn/QEMU caches translated blocks and would
    otherwise reuse a stale immediate."""
    code = b'\xe8' + struct.pack('<i', rva - (STUB + 5))   # call rel32
    code += b'\xd9\x1d' + struct.pack('<I', RESULT)        # fstp dword [RESULT]
    end = STUB + len(code)
    code += b'\x90'
    uc.mem_write(STUB, code)
    return end


def main():
    capture = json.loads((HERE / 'out' / 'vectors.json').read_text())

    # threshold for the degenerate flag — same source as the packer (exe file)
    pe = pefile.PE(str(MASHED_EXE), fast_load=True)
    threshold = struct.unpack(
        '<f', pe.get_data(STATIC_READS[0]['addr'] - IMAGE_BASE, 4))[0]

    total_fail = 0
    for name, fd in capture['funcs'].items():
        cfg = FUNCS[name]
        uc = build_uc(capture)
        stub_end = build_stub(uc, fd['rva'])
        kind = fd['kind']
        fail = skipped = shown = 0
        for i, rec in enumerate(fd['vectors']):
            floats = [struct.unpack('<f', struct.pack('<I', b))[0] for b in rec['v_bits']]
            if is_degenerate(cfg, floats, threshold):
                skipped += 1
                continue
            # place cdecl args on the stack: after `call` pushes the return
            # address, the callee reads arg1 at [ESP+4] == pre-call [ESP+0].
            esp = STACK_TOP - 0x100
            if kind == 'f_f':
                uc.mem_write(esp, struct.pack('<I', rec['v_bits'][0]))       # float bits
            elif kind == 'f_ptrN':
                uc.mem_write(ARGBUF, struct.pack(f"<{fd['n_in']}I", *rec['v_bits']))
                uc.mem_write(esp, struct.pack('<I', ARGBUF))
            else:  # f_out_in: cdecl (out, in) -> [ESP]=out, [ESP+4]=in
                uc.mem_write(ARGBUF, struct.pack(f"<{fd['n_in']}I", *rec['v_bits']))
                uc.mem_write(OUTBUF, b'\xcc' * (fd['n_out'] * 4))
                uc.mem_write(esp, struct.pack('<II', OUTBUF, ARGBUF))
            uc.reg_write(UC_X86_REG_ESP, esp)
            uc.emu_start(STUB, stub_end, timeout=2_000_000, count=1_000_000)
            got_ret = struct.unpack('<I', uc.mem_read(RESULT, 4))[0]
            got_out = list(struct.unpack(f"<{fd['n_out']}I",
                                         uc.mem_read(OUTBUF, fd['n_out'] * 4))) if fd['n_out'] else []
            if got_ret != rec['ret_bits'] or got_out != rec['out_bits']:
                fail += 1
                if shown < 3:
                    print(f"MISMATCH {name}#{i} expected_ret=0x{rec['ret_bits']:08x} "
                          f"got=0x{got_ret:08x} out_exp={rec['out_bits']} out_got={got_out}")
                    shown += 1
        verdict = 'FAIL' if fail else 'PASS'
        print(f"{name:<16} vectors={len(fd['vectors'])} skipped_degenerate={skipped} "
              f"mismatches={fail}  {verdict}")
        total_fail += fail
    print(f'TOTAL: {"FAIL" if total_fail else "PASS"}')
    return 1 if total_fail else 0


if __name__ == '__main__':
    sys.exit(main())
