#!/usr/bin/env python3
# re/tools/veccap/replay_c3.py
#
# Replay-as-C3 (re/CONFIDENCE.md, "Replay clause", 2026-10-11). Replays REAL calls
# recorded in a canonical race by record_race.py, offline in Unicorn, with BOTH
# bodies running in one address space on the exact memory the game had:
#   * original/MASHED.exe mapped at 0x00400000 (the bytes the live game ran);
#   * the port image (port_image.cpp + the registry's TUs, linked /FIXED at
#     0x30000000 with the .asi's per-TU flags) mapped beside it.
# Per call, three-way:
#   fidelity     live result (read by the game's own call) == emulated original
#   equivalence  emulated original == emulated port (return + out buffer)
#   side effects every non-stack byte either body writes (address -> last value)
#   stack        callee-cleaned bytes (ESP after the call) agree
# plus, for both bodies: reads outside the declared state, unmapped accesses,
# imports the port reaches (VirtualQuery is modelled; anything else stops the run).
#
# Branch coverage: every jcc in the ORIGINAL target body (capstone recursive
# descent from the RVA; calls not followed) is scored per side (taken /
# not-taken) over the replayed calls.
#
# FSIN/FCOS gap (README Pilot-2 caveat): CLOSED BY REFUSAL, not by modelling.
# QEMU computes x87 transcendentals through host double maths, not the 80-bit
# microcode, so an emulated FSIN is not the original's FSIN. Any body that can
# reach FSIN/FCOS/FSINCOS/FPTAN/FPATAN/F2XM1/FYL2X/FYL2XP1 is REFUSED: statically
# (target body + its direct-call closure) and dynamically (a tripwire on every
# executed instruction of either body). The tool never emits GREEN for such a body.
#
# Verdicts (log/diff/<rva>_<name>.replay.csv ends in a VERDICT row):
#   REFUSED  transcendental reachable
#   RED      some call whose emulation is faithful has port != original
#            (equivalence, side effects or stack)
#   VOID     the instrument cannot vouch (fidelity miss, unmapped/undeclared read,
#            import stop, emulator error) and nothing is RED
#   HELD     all three legs equal on every call but a clause gate is unmet
#            (< --min-calls distinct calls, or coverage < --min-cov)
#   GREEN    everything equal and every gate met
#
# Usage:
#   py -3.12 re/tools/veccap/replay_c3.py <name> [--min-calls 100] [--min-cov 0.90]
#   py -3.12 re/tools/veccap/replay_c3.py <name> --wrong-port <export>   # mutation control
import argparse
import csv
import json
import os
import struct
import subprocess
import sys
from pathlib import Path

import pefile
from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from capstone.x86 import X86_OP_IMM
from unicorn import (UC_ARCH_X86, UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_UNMAPPED,
                     UC_HOOK_MEM_WRITE, UC_MODE_32, Uc, UcError)
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_EIP, UC_X86_REG_ESP,
                               UC_X86_REG_FPCW, UC_X86_REG_MXCSR)

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE))
from veccap_registry import FUNCS, mashed_root, synth_inputs, n_in, n_out, n_a, has_scalar  # noqa: E402

MASHED_EXE = mashed_root() / 'original' / 'MASHED.exe'
BUNDLES = ROOT / 're' / 'replay' / 'veccap'
SRC = ROOT / 'mashedmod' / 'src' / 'mashed_re'
X87_LIST = ROOT / 'mashedmod' / 'x87_tus.txt'
VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
OUT = HERE / 'out' / 'port_image'
PORT_EXE = OUT / 'port_image.exe'
PORT_MAP = OUT / 'port_image.map'
X87_EXTRA = set()   # --x87-extra: experiment only, never the promotion build


def set_x87_extra(tus):
    """Build a SEPARATE port image with extra TUs compiled /arch:IA32 (x87), to measure
    what adding them to mashedmod/x87_tus.txt would do. Never used for a promotion."""
    global OUT, PORT_EXE, PORT_MAP
    X87_EXTRA.update(tus)
    OUT = HERE / 'out' / ('port_image_x87_' + '_'.join(sorted(tus)))
    PORT_EXE = OUT / 'port_image.exe'
    PORT_MAP = OUT / 'port_image.map'

IMAGE_BASE = 0x00400000
PORT_BASE = 0x30000000
TRAP = 0x7EFD0000          # import trap page (port IAT slots point here)
STUB = 0x7EFE0000          # caller stub: call target ; [fstp dword [RESULT]]
RESULT = 0x7EFE8000        # own page: the stub's store is not part of either write set
STACK_LO, STACK_SZ = 0x7F000000, 0x10000
PAGE = 0x1000

TRANSCENDENTAL = {'fsin', 'fcos', 'fsincos', 'fptan', 'fpatan', 'f2xm1', 'fyl2x', 'fyl2xp1'}
TRANS_BYTES = {0xFE, 0xFF, 0xFB, 0xF2, 0xF3, 0xF0, 0xF1, 0xF9}   # D9 xx
FLOAT_RET = {'f_f', 'f_ptrN', 'f_out_in'}


# ----------------------------------------------------------------------------- images
class Image:
    def __init__(self, path, base):
        self.pe = pefile.PE(str(path))
        self.base = base
        self.mem = self.pe.get_memory_mapped_image(ImageBase=base)
        self.size = (len(self.mem) + PAGE - 1) & ~(PAGE - 1)
        self.ro = []       # [lo, hi) non-writable sections
        self.rw = []
        for s in self.pe.sections:
            lo = base + s.VirtualAddress
            hi = lo + max(s.Misc_VirtualSize, s.SizeOfRawData)
            (self.rw if s.Characteristics & 0x80000000 else self.ro).append((lo, hi))

    def read(self, va, n):
        o = va - self.base
        return self.mem[o:o + n]


def x87_tus():
    out = set()
    for line in X87_LIST.read_text().splitlines():
        t = line.split('#', 1)[0].strip()
        if t:
            out.add(t)
    return out


def build_port_image():
    srcs = sorted({s for c in FUNCS.values() for s in c['sources']})
    deps = [HERE / 'port_image.cpp', X87_LIST] + [SRC / s for s in srcs]
    if PORT_EXE.exists() and PORT_EXE.stat().st_mtime > max(p.stat().st_mtime for p in deps):
        return
    OUT.mkdir(parents=True, exist_ok=True)
    x87 = x87_tus() | X87_EXTRA
    objs, cmds = [], []
    for s in srcs:
        stem = Path(s).stem
        obj = OUT / f'{stem}.obj'
        arch = ' /arch:IA32' if stem in x87 else ''
        cmds.append(f'cl /nologo /c /EHsc /W3 /O2{arch} /Fo"{obj}" "{SRC / s}"')
        objs.append(obj)
    host = OUT / 'port_image.obj'
    cmds.append(f'cl /nologo /c /EHsc /W3 /O2 /Fo"{host}" "{HERE / "port_image.cpp"}"')
    incl = ' '.join(f'/INCLUDE:_{c["export"]}' for c in FUNCS.values())
    cmds.append(f'link /nologo /OUT:"{PORT_EXE}" /MAP:"{PORT_MAP}" "{host}" '
                + ' '.join(f'"{o}"' for o in objs)
                + f' /BASE:0x{PORT_BASE:08x} /FIXED /DYNAMICBASE:NO /OPT:NOREF /OPT:NOICF {incl} user32.lib')
    # CL/_CL_ are prepended to every cl command line: a Git-Bash-launched python
    # inherits them path-mangled (CL=/MP12 -> 'C:/Program Files/Git/MP12'), so drop them.
    env = {k: v for k, v in os.environ.items() if k.upper() not in ('CL', '_CL_')}
    r = subprocess.run(f'call "{VCVARS}" >nul && ' + ' && '.join(cmds), shell=True,
                       capture_output=True, text=True, cwd=str(OUT), env=env)
    if r.returncode != 0:
        sys.exit(f'PORT IMAGE BUILD FAILED:\n{r.stdout[-4000:]}\n{r.stderr[-2000:]}')
    print(f'built {PORT_EXE.relative_to(ROOT)} ({len(srcs)} TUs, x87: '
          f'{sorted(Path(s).stem for s in srcs if Path(s).stem in x87) or "none"})')


def port_symbols():
    syms = {}
    for line in PORT_MAP.read_text(errors='replace').splitlines():
        p = line.split()
        if len(p) >= 3 and ':' in p[0] and len(p[2]) == 8:
            try:
                syms[p[1]] = int(p[2], 16)
            except ValueError:
                pass
    return syms


def port_body(syms, export):
    """The body the installed hook runs. A ShadowAB-wrapped RVA (re/tools/shadow_gen.py)
    exports a wrapper that runs the original AND `<export>_impl` in-process and compares;
    the port under test is `<export>_impl`. Otherwise the export is the body."""
    for name, va in syms.items():
        if name in (f'_{export}_impl', f'{export}_impl') or name.startswith(f'?{export}_impl@'):
            return name, va
    return f'_{export}', syms.get(f'_{export}')


# ----------------------------------------------------------------------------- static
def descend(img, entry, follow_calls=False, limit=20000):
    """Recursive descent over one function body. Returns (insns{addr: insn},
    jccs{addr: (target, fallthrough)}, direct_callees set, indirect jumps list)."""
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    insns, jccs, callees, indirect = {}, {}, set(), []
    work = [entry]
    while work and len(insns) < limit:
        a = work.pop()
        while a not in insns:
            code = img.read(a, 16)
            ins = next(md.disasm(bytes(code), a), None)
            if ins is None:
                indirect.append(('undecodable', a))
                break
            insns[a] = ins
            m = ins.mnemonic
            nxt = a + ins.size
            imm = ins.operands[0].imm if ins.operands and ins.operands[0].type == X86_OP_IMM else None
            if m.startswith('j') and m != 'jmp':
                jccs[a] = (imm, nxt)
                if imm is not None:
                    work.append(imm)
                a = nxt
            elif m == 'jmp':
                if imm is None:
                    indirect.append(('jmp', a))
                else:
                    work.append(imm)
                break
            elif m in ('ret', 'retf', 'int3', 'hlt', 'ud2'):
                break
            elif m == 'call':
                if imm is not None:
                    callees.add(imm)
                a = nxt
            else:
                a = nxt
    return insns, jccs, callees, indirect


def static_transcendentals(img, entry, depth=6):
    """Transcendental opcodes in the body and its direct-call closure."""
    seen, hits, unresolved = set(), [], []
    frontier = [(entry, 0)]
    while frontier:
        fn, d = frontier.pop()
        if fn in seen or not (img.base <= fn < img.base + img.size):
            continue
        seen.add(fn)
        insns, _, callees, indirect = descend(img, fn)
        hits += [(fn, a, i.mnemonic) for a, i in insns.items() if i.mnemonic in TRANSCENDENTAL]
        unresolved += [(fn, k, a) for k, a in indirect]
        for c in callees:
            if d + 1 <= depth:
                frontier.append((c, d + 1))
    return hits, unresolved, seen


def synth_bundle(name, cfg, cw):
    """Registry synthetic inputs as a bundle. No live leg: these calls can show RED
    (port != original) but never count toward the clause's live-call gate."""
    calls = []
    na, ni, no, sc = n_a(cfg), n_in(cfg), n_out(cfg), has_scalar(cfg)
    for v in synth_inputs(cfg):
        bits = [struct.unpack('<I', struct.pack('<f', x))[0] for x in v]
        if cfg['kind'] == 'f_f':
            ptrs = []
        elif cfg['kind'] == 'f_ptrN':
            ptrs = [0x0A001000]
        elif cfg['kind'] == 'v_out_2in':
            ptrs = [0x0A000000, 0x0A001000, 0x0A002000]
        else:
            ptrs = [0x0A000000, 0x0A001000]
        calls.append({'cw': cw, 'mxcsr': 0x1F80, 'ptrs': ptrs, 'in_bits': bits,
                      'out_pre': [0xCCCCCCCC] * no, 'out_live': None, 'ret_live': None,
                      'synthetic': True})
    return {'name': name, 'rva': cfg['rva'], 'kind': cfg['kind'], 'n_in': ni, 'n_out': no,
            'n_a': na, 'scalar': sc, 'calls': calls, 'calls_seen': 0, 'every': 0,
            'scenario': f'SYNTHETIC registry inputs (synth_inputs), cw=0x{cw:04x}, no live leg',
            'recorded_at': 'n/a'}


# ----------------------------------------------------------------------------- one run
def bits_to_bytes(bits):
    return struct.pack(f'<{len(bits)}I', *bits)


class Layout:
    """Where a recorded call's buffers live, and what each body may legitimately read."""

    def __init__(self, b, call):
        k, n_in, n_out, na, sc = b['kind'], b['n_in'], b['n_out'], b['n_a'], b['scalar']
        self.kind = k
        self.bufs = []       # (addr, bytes) written before the run, in order
        self.args = []       # stack args after the return address
        self.out = None      # (addr, n_out)
        ib = call['in_bits']
        if k == 'f_f':
            self.args = [ib[0]]
        elif k == 'f_ptrN':
            p = call['ptrs'][0]
            self.bufs = [(p, bits_to_bytes(ib))]
            self.args = [p]
        elif k == 'v_out_2in':
            po, p1, p2 = call['ptrs']
            nb = n_in - na - sc
            self.bufs = [(po, bits_to_bytes(call['out_pre'])),
                         (p1, bits_to_bytes(ib[:na])), (p2, bits_to_bytes(ib[na:na + nb]))]
            self.args = [po, p1, p2] + ([ib[-1]] if sc else [])
            self.out = (po, n_out)
        else:
            po, pi = call['ptrs']
            self.bufs = [(po, bits_to_bytes(call['out_pre'])), (pi, bits_to_bytes(ib))]
            self.args = [po, pi]
            self.out = (po, n_out)
        self.readable = [(a, a + len(d)) for a, d in self.bufs]


def run_body(mimg, pimg, b, call, lay, entry, regions, jcc_map, imports):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    mapped = []

    def map_range(lo, size):
        for p in range(lo & ~(PAGE - 1), (lo + size + PAGE - 1) & ~(PAGE - 1), PAGE):
            if any(a <= p < a + s for a, s in mapped):
                continue
            uc.mem_map(p, PAGE)
            mapped.append((p, PAGE))

    for img in (mimg, pimg):
        uc.mem_map(img.base, img.size)
        uc.mem_write(img.base, bytes(img.mem))
        mapped.append((img.base, img.size))
    uc.mem_map(STACK_LO, STACK_SZ); mapped.append((STACK_LO, STACK_SZ))
    for pg in (STUB, RESULT, TRAP):
        uc.mem_map(pg, PAGE); mapped.append((pg, PAGE))

    readable = list(lay.readable) + mimg.ro + [(STACK_LO, STACK_LO + STACK_SZ)]
    if regions:
        uc.mem_write(0x007D3FF8, struct.pack('<II', regions['rw_globals'], regions['rw_offset']))
        readable.append((0x007D3FF8, 0x007D4000))
        map_range(regions['slot_addr'], 8)
        uc.mem_write(regions['slot_addr'], struct.pack('<II', regions['lut_root'], regions['lut_inv_root']))
        readable.append((regions['slot_addr'], regions['slot_addr'] + 8))
        for key, root in (('lut_hex', regions['lut_root']), ('lut_inv_hex', regions['lut_inv_root'])):
            data = bytes.fromhex(regions[key])
            map_range(root, len(data))
            uc.mem_write(root, data)
            readable.append((root, root + len(data)))
    for a, d in lay.bufs:
        for lo, hi in ((STACK_LO, STACK_LO + STACK_SZ), (PORT_BASE, PORT_BASE + pimg.size),
                       (TRAP, RESULT + PAGE)):
            if lo <= a < hi:
                raise RuntimeError(f'live buffer 0x{a:08x} collides with a harness region')
        if IMAGE_BASE <= a < IMAGE_BASE + mimg.size:
            continue   # a MASHED global (.data/.bss): already mapped; recorded bytes overlay it below
        map_range(a, len(d))
    for p, s in list(mapped):
        if p not in (mimg.base, pimg.base, STACK_LO, STUB, RESULT, TRAP) and s == PAGE:
            uc.mem_write(p, b'\xcd' * PAGE)   # filler: an undeclared read sees 0xcd, and is flagged
    if regions:   # the filler pass above overwrote region pages: rewrite them
        uc.mem_write(regions['slot_addr'], struct.pack('<II', regions['lut_root'], regions['lut_inv_root']))
        for key, root in (('lut_hex', regions['lut_root']), ('lut_inv_hex', regions['lut_inv_root'])):
            uc.mem_write(root, bytes.fromhex(regions[key]))
    for a, d in lay.bufs:
        uc.mem_write(a, d)
    port_rw = (pimg.base, pimg.base + pimg.size)

    # IAT traps for the port image
    for i, (slot, name) in enumerate(imports):
        uc.mem_write(slot, struct.pack('<I', TRAP + i * 16))
        uc.mem_write(TRAP + i * 16, b'\xcc')

    # stub
    code = b'\xe8' + struct.pack('<i', entry - (STUB + 5))
    if lay.kind in FLOAT_RET:
        code += b'\xd9\x1d' + struct.pack('<I', RESULT)
    stub_end = STUB + len(code)
    uc.mem_write(STUB, code + b'\xf4')

    sp = STACK_LO + STACK_SZ - 0x200
    uc.mem_write(sp, b''.join(struct.pack('<I', v & 0xFFFFFFFF) for v in lay.args))
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_FPCW, call['cw'])
    uc.reg_write(UC_X86_REG_MXCSR, call['mxcsr'] & 0xFFFF)

    st = {'writes': {}, 'port_private_writes': 0, 'bad_reads': [], 'misses': [], 'trans': [],
          'edges': set(), 'import_stop': None, 'vq_calls': 0}
    prev = [None]

    def on_code(u, addr, size, _):
        if TRAP <= addr < TRAP + PAGE:
            idx = (addr - TRAP) // 16
            name = imports[idx][1] if idx < len(imports) else '?'
            if name == 'VirtualQuery':
                esp = u.reg_read(UC_X86_REG_ESP)
                ret, lpa, buf, ln = struct.unpack('<IIII', u.mem_read(esp, 16))
                page = lpa & ~(PAGE - 1)
                reg = next(((s, e) for s, e, _p in u.mem_regions() if s <= page <= e), None)
                if reg:
                    mbi = struct.pack('<7I', page, reg[0], 0x04, reg[1] + 1 - page, 0x1000, 0x04, 0x20000)
                else:
                    mbi = struct.pack('<7I', page, 0, 0x01, PAGE, 0x10000, 0x01, 0)
                u.mem_write(buf, mbi[:min(ln, 28)])
                u.reg_write(UC_X86_REG_EAX, 28)
                u.reg_write(UC_X86_REG_ESP, esp + 16)
                u.reg_write(UC_X86_REG_EIP, ret)
                st['vq_calls'] += 1
                return
            st['import_stop'] = name
            u.emu_stop()
            return
        op = u.mem_read(addr, 2)
        if op[0] == 0xD9 and op[1] in TRANS_BYTES:
            st['trans'].append(addr)
            u.emu_stop()
            return
        if jcc_map is not None:
            p = prev[0]
            if p in jcc_map:
                tgt, ft = jcc_map[p]
                if addr == tgt:
                    st['edges'].add((p, True))
                elif addr == ft:
                    st['edges'].add((p, False))
            prev[0] = addr

    def on_write(u, access, addr, size, value, _):
        if STACK_LO <= addr < STACK_LO + STACK_SZ or RESULT <= addr < RESULT + PAGE:
            return
        if port_rw[0] <= addr < port_rw[1]:
            st['port_private_writes'] += 1
            return
        v = value & ((1 << (8 * size)) - 1)
        for i in range(size):
            st['writes'][addr + i] = (v >> (8 * i)) & 0xFF

    def on_read(u, access, addr, size, value, _):
        if port_rw[0] <= addr < port_rw[1]:
            return
        for lo, hi in readable:
            if lo <= addr and addr + size <= hi:
                return
        # a read of bytes this run itself wrote earlier is not outside state
        if all((addr + i) in st['writes'] for i in range(size)):
            return
        if len(st['bad_reads']) < 8:
            st['bad_reads'].append(addr)
        else:
            st['bad_reads'].append(None)

    def on_unmapped(u, access, addr, size, value, _):
        st['misses'].append(addr)
        return False

    uc.hook_add(UC_HOOK_CODE, on_code)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    uc.hook_add(UC_HOOK_MEM_READ, on_read)
    uc.hook_add(UC_HOOK_MEM_UNMAPPED, on_unmapped)
    err = None
    try:
        uc.emu_start(STUB, stub_end, count=2_000_000)
    except UcError as e:
        err = str(e)
    eip = uc.reg_read(UC_X86_REG_EIP)
    if err is None and eip != stub_end and not st['trans'] and not st['import_stop']:
        err = f'stopped at 0x{eip:08x}'
    st['err'] = err
    st['pop'] = uc.reg_read(UC_X86_REG_ESP) - sp
    st['ret'] = struct.unpack('<I', uc.mem_read(RESULT, 4))[0] if lay.kind in FLOAT_RET else None
    st['out'] = list(struct.unpack(f'<{lay.out[1]}I', uc.mem_read(lay.out[0], lay.out[1] * 4))) \
        if lay.out else None
    return st


# ----------------------------------------------------------------------------- driver
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('name')
    ap.add_argument('--min-calls', type=int, default=100)
    ap.add_argument('--min-cov', type=float, default=0.90)
    ap.add_argument('--wrong-port', default=None,
                    help='mutation control: run THIS port export in place of the right one (must read RED)')
    ap.add_argument('--limit', type=int, default=None)
    ap.add_argument('--x87-extra', default='',
                    help='EXPERIMENT: comma list of TU stems to also compile /arch:IA32 (separate image; '
                         'the verdict is tagged and never counts as promotion evidence)')
    ap.add_argument('--force-cw', type=lambda x: int(x, 0), default=None,
                    help='DIAGNOSTIC: run both bodies under this x87 CW instead of the recorded one. '
                         'The fidelity leg then fails by construction, so the verdict is never GREEN')
    ap.add_argument('--synth', action='store_true',
                    help='replay registry synthetic inputs instead of a recorded bundle (controls only; '
                         'never GREEN: there is no live leg)')
    ap.add_argument('--synth-cw', type=lambda x: int(x, 0), default=0x007F,
                    help='x87 control word for --synth (default 0x007f = the live race CW)')
    ap.add_argument('--out-dir', default=None)
    a = ap.parse_args()

    if a.name.lower().startswith('0x'):
        # static screen only: is this RVA (body + direct-call closure) emulable at all?
        mimg = Image(MASHED_EXE, IMAGE_BASE)
        rva = int(a.name, 16)
        insns, jccs, callees, indirect = descend(mimg, rva)
        hits, unres, closure = static_transcendentals(mimg, rva)
        print(f"0x{rva:08x}: body {len(insns)} insns, {len(jccs)} jcc, closure {len(closure)} fns, "
              f"unresolved indirect {len(unres)}")
        for fn, at, mn in hits:
            print(f"  {mn} at 0x{at:08x} (in 0x{fn:08x})")
        print('REFUSED (transcendental reachable)' if hits else 'emulable: no transcendental in the static closure')
        return 3 if hits else 0

    if a.x87_extra:
        set_x87_extra([t for t in a.x87_extra.split(',') if t])
    cfg = FUNCS[a.name]
    bpath = BUNDLES / f"{cfg['rva']:08x}_{a.name}.json"
    b = synth_bundle(a.name, cfg, a.synth_cw) if a.synth else json.loads(bpath.read_text())
    calls = b['calls'][:a.limit]
    if a.force_cw is not None:
        calls = [dict(c, cw=a.force_cw) for c in calls]
    regions = b.get('regions')

    build_port_image()
    mimg = Image(MASHED_EXE, IMAGE_BASE)
    pimg = Image(PORT_EXE, PORT_BASE)
    syms = port_symbols()
    export = a.wrong_port or cfg['export']
    psym, pentry = port_body(syms, export)
    if pentry is None:
        sys.exit(f'port symbol _{export} not in {PORT_MAP.name}')
    imports = []
    pimg.pe.parse_data_directories()
    for dll in getattr(pimg.pe, 'DIRECTORY_ENTRY_IMPORT', []):
        for imp in dll.imports:
            imports.append((imp.address, (imp.name or b'?').decode()))

    # static: body, jccs, transcendental closure (original AND port)
    insns, jccs, callees, indirect = descend(mimg, cfg['rva'])
    trans_o, unres_o, closure_o = static_transcendentals(mimg, cfg['rva'])
    trans_p, unres_p, _ = static_transcendentals(pimg, pentry)
    print(f"{a.name} 0x{cfg['rva']:08x}: body {len(insns)} insns, {len(jccs)} jcc, "
          f"{len(callees)} direct callees, closure {len(closure_o)} fns; port {psym} @0x{pentry:08x}")

    out_dir = Path(a.out_dir) if a.out_dir else ROOT / 'log' / 'diff'
    if a.wrong_port and not a.out_dir:
        out_dir = ROOT / 'log' / 'replay-c3' / 'negctl'
    out_dir.mkdir(parents=True, exist_ok=True)
    tag = (f'.negctl_{a.wrong_port}' if a.wrong_port else '') + (f'.synth_cw{a.synth_cw:04x}' if a.synth else '') +         (f'.cw{a.force_cw:04x}' if a.force_cw is not None else '') +         (f".x87_{a.x87_extra.replace(',', '_')}" if a.x87_extra else '')
    if a.synth and not a.out_dir:
        out_dir = ROOT / 'log' / 'replay-c3' / 'negctl'
        out_dir.mkdir(parents=True, exist_ok=True)
    csv_path = out_dir / f"{cfg['rva']:08x}_{a.name}{tag}.replay.csv"

    if trans_o or trans_p:
        verdict, summary = 'REFUSED', {'transcendental_original': [hex(x[1]) for x in trans_o],
                                        'transcendental_port': [hex(x[1]) for x in trans_p]}
        with csv_path.open('w', newline='') as f:
            csv.writer(f).writerow(['VERDICT', verdict, 0, json.dumps(summary)])
        print(f'{a.name}: REFUSED (transcendental reachable) {summary} -> {csv_path}')
        return 3

    bad = {'fidelity': 0, 'equivalence': 0, 'side_effects': 0, 'stack': 0,
           'undeclared_reads': 0, 'misses': 0, 'errors': 0, 'import_stops': 0, 'transcendental': 0}
    red_calls = 0
    edges = set()
    examples = []
    port_private = 0
    with csv_path.open('w', newline='') as f:
        w = csv.writer(f)
        w.writerow(['call', 'cw', 'in_bits', 'live_ret', 'orig_ret', 'port_ret',
                    'live_out', 'orig_out', 'port_out', 'fid', 'eq', 'se', 'st', 'notes'])
        for i, c in enumerate(calls):
            lay = Layout(b, c)
            o = run_body(mimg, pimg, b, c, lay, cfg['rva'], regions, jccs, imports)
            r = run_body(mimg, pimg, b, c, lay, pentry, regions, None, imports)
            edges |= o['edges']
            port_private += r['port_private_writes']
            if c.get('synthetic'):   # no live leg: only "the emulation ran cleanly"
                fid = o['err'] is None and not o['trans']
            else:
                fid = (o['err'] is None and not o['trans'] and o['out'] == (c['out_live'] or None)
                       and (c['ret_live'] is None or o['ret'] == c['ret_live']))
            eq = r['err'] is None and r['ret'] == o['ret'] and r['out'] == o['out']
            se = r['writes'] == o['writes']
            stk = r['pop'] == o['pop']
            notes = []
            for side, s in (('orig', o), ('port', r)):
                if s['bad_reads']:
                    bad['undeclared_reads'] += 1
                    notes.append(f"{side}:undeclared_read@{hex(s['bad_reads'][0])}")
                if s['misses']:
                    bad['misses'] += 1
                    notes.append(f"{side}:unmapped@{hex(s['misses'][0])}")
                if s['err']:
                    bad['errors'] += 1
                    notes.append(f"{side}:err={s['err']}")
                if s['import_stop']:
                    bad['import_stops'] += 1
                    notes.append(f"{side}:import={s['import_stop']}")
                if s['trans']:
                    bad['transcendental'] += 1
                    notes.append(f"{side}:transcendental@{hex(s['trans'][0])}")
            bad['fidelity'] += not fid
            bad['equivalence'] += not eq
            bad['side_effects'] += not se
            bad['stack'] += not stk
            if fid and not (eq and se and stk):
                red_calls += 1
                if len(examples) < 3:
                    diff_w = sorted(set(o['writes']) ^ set(r['writes']) |
                                    {k for k in o['writes'] if r['writes'].get(k) != o['writes'][k]})
                    examples.append((i, o['out'], r['out'], o['ret'], r['ret'],
                                     [hex(x) for x in diff_w[:6]], o['pop'], r['pop']))
            h = lambda v: '' if v is None else (f'0x{v:08x}' if isinstance(v, int) else ' '.join(f'{x:08x}' for x in v))
            w.writerow([i, f"0x{c['cw']:04x}", ' '.join(f'{x:08x}' for x in c['in_bits']),
                        h(c['ret_live']), h(o['ret']), h(r['ret']),
                        h(c['out_live'] or None), h(o['out']), h(r['out']),
                        fid, eq, se, stk, ';'.join(notes)])

        n = len(calls)
        distinct = len({tuple(c['in_bits']) for c in calls if not c.get('synthetic')})
        cov_n = len(edges)
        cov_d = 2 * len(jccs)
        cov = cov_n / cov_d if cov_d else 1.0
        uncovered = sorted(f'0x{j:08x}:{"taken" if t else "not-taken"}'
                           for j in jccs for t in (True, False) if (j, t) not in edges)
        instrument_bad = any(bad[k] for k in ('fidelity', 'undeclared_reads', 'misses', 'errors',
                                              'import_stops', 'transcendental'))
        if red_calls:
            verdict = 'RED'
        elif instrument_bad:
            verdict = 'VOID'
        elif distinct < a.min_calls or cov < a.min_cov:
            verdict = 'HELD'
        else:
            verdict = 'GREEN'
        summary = {
            'calls': n, 'distinct_inputs': distinct, 'calls_seen': b['calls_seen'], 'every': b['every'],
            'cw_values': sorted({f"0x{c['cw']:04x}" for c in calls}),
            'jcc': len(jccs), 'jcc_sides_covered': cov_n, 'jcc_sides_total': cov_d,
            'coverage': round(cov, 4), 'uncovered': uncovered, 'failures': bad,
            'red_calls': red_calls, 'port_private_writes': port_private,
            'unresolved_indirect_in_closure': [f'0x{x[2]:08x}' for x in unres_o + unres_p],
            'gates': {'min_calls': a.min_calls, 'min_cov': a.min_cov},
            'bundle': 'synthetic' if a.synth else str(bpath.relative_to(ROOT)),
            'recorded_at': b['recorded_at'],
            'scenario': b['scenario'], 'port_export': export, 'port_symbol': psym,
        }
        w.writerow(['meta', f"scenario={b['scenario']}", f"recorded={b['recorded_at']}",
                    f"calls_seen={b['calls_seen']} every={b['every']}"])
        w.writerow(['VERDICT', verdict, n, json.dumps(summary)])
    print(f"{a.name}: {verdict}  calls={n} distinct={distinct} cw={summary['cw_values']} "
          f"jcc-sides {cov_n}/{cov_d} ({cov:.1%}) failures={bad} red_calls={red_calls} -> {csv_path}")
    if uncovered:
        print(f"  uncovered jcc sides: {uncovered}")
    for ex in examples:
        print(f"  call {ex[0]}: orig_out={ex[1]} port_out={ex[2]} orig_ret={ex[3]} port_ret={ex[4]} "
              f"write-diff={ex[5]} pop {ex[6]}/{ex[7]}")
    return 0 if verdict == 'GREEN' else 1


if __name__ == '__main__':
    sys.exit(main())
