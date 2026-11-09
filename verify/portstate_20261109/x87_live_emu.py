"""x87_live_emu.py --root <root> --img <dir from x87_port_images.py> --out <json> --rvas 0x..,0x.. [--every-json cw.json]

PORT-X87-PC24-AUDIT arm 3 (PREREG_X87AUDIT.md section 4): live-seeded emulation. Boots <root>/original/MASHED.exe
(hooks OFF), drives the record_race.py Quick-Battle recipe, and at sampled entries of each --rvas function holds the
game thread while Unicorn replays that call from the live memory: every page the replay touches is read from the
paused process on demand, nothing is written back. Variants on the same state (all compared to O):

  O    original bytes at the live CW          O53  original at PC_53      O24  original at PC_24
  X53  port body (/arch:IA32 image) PC_53     X24  port x87 body PC_24    S    port body (/arch:SSE2) live CW, MXCSR 0x1fa0

Compared: every byte any variant wrote outside [esp0-128K, esp0+4+4*nargs) and outside the port images, plus the return
value by the matched C signature (int: eax, float/double: ST0 as 80-bit and as f32). REFUSED: an x87 transcendental,
code fetched outside the original and port images (imports), an unreadable page, or the instruction cap.
"""
import argparse, json, os, queue, re, struct, sys, threading, time
from pathlib import Path
import frida, pefile
from unicorn import (Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_MEM_UNMAPPED, UC_HOOK_MEM_WRITE, UC_HOOK_CODE,
                     UC_MEM_FETCH_UNMAPPED, UC_PROT_ALL)
from unicorn.x86_const import *

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
ap = argparse.ArgumentParser()
ap.add_argument('--root', required=True)
ap.add_argument('--img', default='D:/MashedWT/PORT-X87-PC24-AUDIT_data/img')
ap.add_argument('--out', required=True)
ap.add_argument('--rvas', required=True)
ap.add_argument('--max', type=int, default=24, help='samples per RVA')
ap.add_argument('--every-json', default='', help='arm-1 cw json: spread samples over the run by call counts')
ap.add_argument('--race-s', type=float, default=45)
ap.add_argument('--late', default='', help='hot RVAs: attached only after --late-after s of race, every call, '
                'detached after --max samples (CLAUDE.md hot-path rule)')
ap.add_argument('--late-after', type=float, default=10)
a = ap.parse_args()

PAGE = 0x1000
STUB, RESULT = 0x2ff00000, 0x2ff00800
TRANS = {0xFE, 0xFF, 0xFB, 0xF2, 0xF3, 0xF1, 0xF9, 0xF0}
INV = [l.split('\t') for l in (HERE / 'x87_inventory.tsv').read_text().splitlines()[1:]]
NAMES = {p[1]: p[2] for p in (l.split('\t') for l in (HERE / 'x87_names.tsv').read_text().splitlines()) if len(p) >= 3}
LATE = [int(x, 16) for x in a.late.split(',') if x]
RVAS = [int(x, 16) for x in a.rvas.split(',') if x and int(x, 16) not in LATE] + LATE


def signature(rva):
    """Return type and arg count from the matched source: the definition after '// MATCH: 0x<rva>'."""
    for tu, sect, src, r, how in INV:
        if r and int(r, 16) == rva and how.startswith('match:'):
            t = (REPO / src).read_text(errors='replace')
            m = re.search(r're/match/([a-z0-9_]+)/((?:[a-z0-9]+_)?%08x)\.c' % rva, t)
            if not m:
                continue
            c = (REPO / 're' / 'match' / m.group(1) / (m.group(2) + '.c')).read_text(errors='replace')
            d = re.search(r'^\s*([\w\s\*]+?)\s*\b(?:__cdecl|__stdcall|__fastcall|__thiscall)?\s*FUN_%08x\s*\(([^)]*)\)\s*\{'
                          % rva, c, re.M)
            if not d:
                return ('?', 0)
            ret = d.group(1).split()[-1] if d.group(1).split() else '?'
            if '*' in d.group(1):
                ret = 'ptr'
            args = [x for x in d.group(2).split(',') if x.strip() and x.strip() != 'void']
            return (ret, len(args))
    return ('?', 0)


class Img:
    def __init__(self, exe, mp, base):
        self.pe = pefile.PE(str(exe))
        self.mem = bytes(self.pe.get_memory_mapped_image(ImageBase=base))
        self.base = base
        self.size = (len(self.mem) + PAGE - 1) & ~(PAGE - 1)
        self.sym = {}
        for l in Path(mp).read_text(errors='replace').splitlines():
            p = l.split()
            if len(p) >= 4 and ':' in p[0] and len(p[2]) == 8:
                try:
                    self.sym.setdefault(p[1], int(p[2], 16))
                except ValueError:
                    pass

    def addr(self, rva):
        nm = NAMES.get('0x%08x' % rva) or 'FUN_%08x' % rva
        return self.sym.get('_' + nm) or self.sym.get(nm)

    def run_init(self):
        """Run the C++ dynamic initializers (__xc_a..__xc_z) once: the MGLOBAL references (`static T &n =
        *(T *)a`) are stored by them, and the image is never loaded natively. Image-only Unicorn; an initializer that
        touches anything else is skipped and counted."""
        lo, hi = self.sym.get('___xc_a'), self.sym.get('___xc_z')
        uc = Uc(UC_ARCH_X86, UC_MODE_32)
        uc.mem_map(self.base, self.size, UC_PROT_ALL)
        uc.mem_write(self.base, self.mem)
        stk, ret = 0x2fe00000, 0x2fef0000
        uc.mem_map(stk, 0x100000, UC_PROT_ALL)
        uc.mem_write(ret, b'\xf4')
        ok = bad = 0
        for p in range(lo, hi, 4):
            fn = struct.unpack('<I', bytes(uc.mem_read(p, 4)))[0]
            if not fn:
                continue
            sp = stk + 0xff000
            uc.mem_write(sp, struct.pack('<I', ret))
            uc.reg_write(UC_X86_REG_ESP, sp)
            try:
                uc.emu_start(fn, ret, count=200000)
                ok += 1
            except UcError:
                bad += 1
        self.mem = bytes(uc.mem_read(self.base, self.size))
        return ok, bad


IMG = {'X': Img(Path(a.img) / 'X' / 'port_X.exe', Path(a.img) / 'X' / 'port_X.map', 0x30000000),
       'S': Img(Path(a.img) / 'S' / 'port_S.exe', Path(a.img) / 'S' / 'port_S.map', 0x34000000)}
INIT = {k: v.run_init() for k, v in IMG.items()}
print('port image initializers (ok, failed):', INIT, flush=True)
_ope = pefile.PE(str(Path(a.root) / 'original' / 'MASHED.exe'))
ORIG_LO, ORIG_HI = 0x00400000, 0x00400000 + _ope.OPTIONAL_HEADER.SizeOfImage
_omem = bytes(_ope.get_memory_mapped_image())
# Non-writable pages of the original image come from the file on disk (the booted, patched MASHED.exe): the live
# code pages carry Frida's inline hooks, so a replay from them would jump into the trampolines. Writable pages are live.
ORIG_RO = {}
for _s in _ope.sections:
    if not (_s.Characteristics & 0x80000000):
        _lo = (0x00400000 + _s.VirtualAddress) & ~0xfff
        for _p in range(_lo, 0x00400000 + _s.VirtualAddress + _s.Misc_VirtualSize, 0x1000):
            _o = _p - 0x00400000
            ORIG_RO[_p] = _omem[_o:_o + 0x1000].ljust(0x1000, b'\0')
PORT_RANGES = [(i.base, i.base + i.size) for i in IMG.values()]


class Bridge:
    def __init__(self):
        self.q = queue.Queue()
        self.ev = threading.Event()
        self.reply = None
        self.script = None
        self.live = {}

    def on_message(self, m, data):
        if m['type'] != 'send':
            print('msg', m, flush=True); return
        p = m['payload']
        if p.get('type') == 'enter':
            self.q.put(p)
        elif p.get('type') == 'page':
            self.reply = (p['ok'], data)
            self.ev.set()
        elif p.get('type') == 'leave':
            self.live[p['key']] = (bytes(data) if data is not None else b'', p.get('st0'), p.get('st0f'))

    def read(self, va):
        self.ev.clear()
        self.script.post({'type': 'cmd', 'op': 'read', 'va': va, 'len': PAGE})
        if not self.ev.wait(20):
            return None
        ok, data = self.reply
        return bytes(data) if ok and data is not None else None

    def done(self, rd=None, st0=False, key=''):
        self.script.post({'type': 'cmd', 'op': 'done', 'rd': rd or [], 'st0': st0, 'key': key})


import ctypes
# x87 transcendentals run on THIS CPU (QEMU's soft x87 is not bit-exact for them; replay_c3.py refuses them for that
# reason). Python is 64-bit; the x87 unit works the same in long mode. Thunk(rcx=in[2 tbyte, 16 B apart],
# rdx=out[2 tbyte], r8=&{cw, restore cw}): fldcw; fld in1?; fld in0; <op>; fstp out0; fstp out1?; fldcw restore; ret.
TOPS = {0xFE: (1, 1), 0xFF: (1, 1), 0xF0: (1, 1), 0xF2: (1, 2), 0xFB: (1, 2), 0xF3: (2, 1), 0xF1: (2, 1), 0xF9: (2, 1)}
_k32 = ctypes.windll.kernel32
_k32.VirtualAlloc.restype = ctypes.c_void_p
_k32.VirtualAlloc.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_ulong, ctypes.c_ulong]
_THUNK = {}
for _op, (_ni, _no) in TOPS.items():
    _c = bytes([0x41, 0xD9, 0x28]) + (bytes([0xDB, 0x69, 0x10]) if _ni == 2 else b'') + bytes([0xDB, 0x29, 0xD9, _op,
                                                                                               0xDB, 0x3A]) + \
        (bytes([0xDB, 0x7A, 0x10]) if _no == 2 else b'') + bytes([0x41, 0xD9, 0x68, 0x02, 0xC3])
    _m = _k32.VirtualAlloc(None, 4096, 0x3000, 0x40)
    ctypes.memmove(_m, _c, len(_c))
    _THUNK[_op] = ctypes.CFUNCTYPE(None, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p)(_m)


def native_trans(op, ins, cw):
    i = ctypes.create_string_buffer(b''.join(x.ljust(16, b'\0') for x in ins).ljust(32, b'\0'), 32)
    o = ctypes.create_string_buffer(32)
    w = ctypes.create_string_buffer(struct.pack('<HH', cw, 0x027f), 4)
    _THUNK[op](ctypes.addressof(i), ctypes.addressof(o), ctypes.addressof(w))
    return [o.raw[0:10], o.raw[16:26]]


def run_variant(code_va, regs, cw, mxcsr, cache, br, esp0, nargs, retaddr):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    for im in IMG.values():
        uc.mem_map(im.base, im.size, UC_PROT_ALL)
        uc.mem_write(im.base, im.mem)
    uc.mem_map(STUB, PAGE, UC_PROT_ALL)
    st = {'writes': set(), 'refused': None, 'n': 0}
    lo_ex, hi_ex = esp0 - 0x20000, esp0 + 4 + 4 * nargs

    def on_unmapped(u, access, addr, size, value, _):
        page = addr & ~(PAGE - 1)
        if access == UC_MEM_FETCH_UNMAPPED and not (ORIG_LO <= addr < ORIG_HI):
            st['refused'] = st['refused'] or 'fetch outside images @%08x' % addr
            return False
        d = ORIG_RO.get(page) or cache.get(page)
        if d is None and page not in cache:
            d = br.read(page)
            cache[page] = d
        if d is None:
            st['refused'] = st['refused'] or 'unreadable page %08x @%08x' % (page, u.reg_read(UC_X86_REG_EIP))
            return False
        u.mem_map(page, PAGE, UC_PROT_ALL)
        u.mem_write(page, d)
        return True

    def on_write(u, access, addr, size, value, _):
        if lo_ex <= addr < hi_ex or STUB <= addr < STUB + PAGE:
            return
        for lo, hi in PORT_RANGES:
            if lo <= addr < hi:
                return
        for i in range(size):
            st['writes'].add(addr + i)

    def on_code(u, addr, size, _):
        st['n'] += 1
        if size >= 2:
            b = u.mem_read(addr, 2)
            if b[0] == 0xD9 and b[1] in TRANS and not (STUB <= addr < STUB + PAGE):
                st['trans'] = (addr, b[1])
                u.emu_stop()

    uc.hook_add(UC_HOOK_MEM_UNMAPPED, on_unmapped)
    uc.hook_add(UC_HOOK_MEM_WRITE, on_write)
    uc.hook_add(UC_HOOK_CODE, on_code)
    for k, r in (('eax', UC_X86_REG_EAX), ('ebx', UC_X86_REG_EBX), ('ecx', UC_X86_REG_ECX), ('edx', UC_X86_REG_EDX),
                 ('esi', UC_X86_REG_ESI), ('edi', UC_X86_REG_EDI), ('ebp', UC_X86_REG_EBP), ('esp', UC_X86_REG_ESP)):
        uc.reg_write(r, regs[k])
    uc.reg_write(UC_X86_REG_FPCW, cw)
    uc.reg_write(UC_X86_REG_MXCSR, mxcsr)
    sw0 = uc.reg_read(UC_X86_REG_FPSW)
    pc = code_va
    st['n_trans'] = 0
    while True:
        st['trans'] = None
        try:
            uc.emu_start(pc, retaddr, count=3_000_000)
        except UcError as e:
            st['refused'] = st['refused'] or 'uc error %s @%08x' % (e, uc.reg_read(UC_X86_REG_EIP))
        if not st['trans'] or st['refused']:
            break
        addr, op = st['trans']
        ni, no = TOPS[op]
        R = RESULT
        pop = b''.join(bytes([0xDB, 0x3D]) + struct.pack('<I', R + 16 * k) for k in range(ni)) + b'\xf4'
        uc.mem_write(STUB, pop)
        uc.emu_start(STUB, STUB + len(pop) - 1)
        ins = [bytes(uc.mem_read(R + 16 * k, 10)) for k in range(ni)]
        outs = native_trans(op, ins, uc.reg_read(UC_X86_REG_FPCW))
        for k in range(no):
            uc.mem_write(R + 16 * k, outs[k])
        push = b''.join(bytes([0xDB, 0x2D]) + struct.pack('<I', R + 16 * k) for k in reversed(range(no))) + b'\xf4'
        uc.mem_write(STUB, push)
        uc.emu_start(STUB, STUB + len(push) - 1)
        st['n_trans'] += 1
        pc = addr + 2
    if not st['refused'] and uc.reg_read(UC_X86_REG_EIP) != retaddr:
        st['refused'] = 'did not return (eip %08x, %d insns)' % (uc.reg_read(UC_X86_REG_EIP), st['n'])
    out = {'uc': uc, 'st': st, 'eax': uc.reg_read(UC_X86_REG_EAX), 'edx': uc.reg_read(UC_X86_REG_EDX),
           'esp': uc.reg_read(UC_X86_REG_ESP), 'st0': None, 'st0f': None}
    if not st['refused'] and ((uc.reg_read(UC_X86_REG_FPSW) >> 11) & 7) != ((sw0 >> 11) & 7):
        code = bytes([0xD9, 0xC0, 0xD9, 0x1D]) + struct.pack('<I', RESULT + 16) + bytes([0xDB, 0x3D]) + \
            struct.pack('<I', RESULT) + b'\xf4'
        uc.mem_write(STUB, code)
        try:
            uc.emu_start(STUB, STUB + len(code) - 1)
            out['st0'] = bytes(uc.mem_read(RESULT, 10)).hex()
            out['st0f'] = bytes(uc.mem_read(RESULT + 16, 4)).hex()
        except UcError as e:
            st['refused'] = 'st0 read %s' % e
    return out


def byte_at(v, addr, cache):
    try:
        return bytes(v['uc'].mem_read(addr, 1))[0]
    except UcError:
        d = cache.get(addr & ~(PAGE - 1))
        return d[addr & (PAGE - 1)] if d else None


AGENT = r'''
'use strict';
let T=[], KEEP=[], cwGet=null, mxGet=null, st0Get=null, ST0BUF=null;
function hex(b){ return Array.prototype.map.call(new Uint8Array(b), function(x){ return ('0'+x.toString(16)).slice(-2); }).join(''); }
rpc.exports={
  arm:function(list){
    const st=Memory.alloc(Process.pageSize); KEEP.push(st);
    st.writeByteArray([0x50,0xD9,0x3C,0x24,0x58,0x0F,0xB7,0xC0,0xC3]);
    st.add(16).writeByteArray([0x50,0x0F,0xAE,0x1C,0x24,0x58,0xC3]);
    Memory.protect(st,Process.pageSize,'rwx');
    cwGet=new NativeFunction(st,'uint',[]); mxGet=new NativeFunction(st.add(16),'uint',[]);
    // ST0 at onLeave: mov eax,[esp+4]; fld st0; fstp tbyte [eax]; fst dword [eax+16]; ret (ST0 left intact)
    st.add(32).writeByteArray([0x8B,0x44,0x24,0x04, 0xD9,0xC0, 0xDB,0x38, 0xD9,0x50,0x10, 0xC3]);
    st0Get=new NativeFunction(st.add(32),'void',['pointer']); ST0BUF=Memory.alloc(64); KEEP.push(ST0BUF);
    const res=[];
    list.forEach(function(e,i){
      const t={rva:e[0], every:e[1], max:e[2], n:0, taken:0, L:null}; T.push(t);
      try{
        t.L=Interceptor.attach(ptr(e[0]),{onEnter:function(){
          t.n++;
          if(t.taken>=t.max || (t.n % t.every)!==0) return;
          t.taken++;
          const c=this.context;
          send({type:'enter', i:i, rva:t.rva, n:t.n, cw:cwGet(), mx:mxGet(), tid:Process.getCurrentThreadId(),
                regs:{eax:c.eax.toUInt32(), ebx:c.ebx.toUInt32(), ecx:c.ecx.toUInt32(), edx:c.edx.toUInt32(),
                      esi:c.esi.toUInt32(), edi:c.edi.toUInt32(), ebp:c.ebp.toUInt32(), esp:c.esp.toUInt32()}});
          while(true){
            let m=null; recv('cmd', function(x){ m=x; }).wait();
            if(m.op==='read'){ let b=null; try{ b=ptr(m.va).readByteArray(m.len); }catch(err){ b=null; }
              send({type:'page', va:m.va, ok:b!==null}, b); }
            else { this.rd=m.rd; this.st0=m.st0; this.key=m.key; break; }
          }
          if(t.taken>=t.max){ const L=t.L; setTimeout(function(){ try{ L.detach(); }catch(err){} },3000); }
        }, onLeave:function(){
          if(!this.key) return;
          let tot=0; this.rd.forEach(function(r){ tot+=r[1]; });
          const u=new Uint8Array(tot); let o=0;
          this.rd.forEach(function(r){ try{ u.set(new Uint8Array(ptr(r[0]).readByteArray(r[1])),o); }catch(err){} o+=r[1]; });
          let s0=null, s0f=null;
          if(this.st0){ st0Get(ST0BUF); s0=hex(ST0BUF.readByteArray(10)); s0f=hex(ST0BUF.add(16).readByteArray(4)); }
          send({type:'leave', key:this.key, st0:s0, st0f:s0f}, u.buffer);
        }});
        res.push(1);
      }catch(err){ res.push(String(err)); }
    });
    return res;
  },
  counts:function(){ return T.map(function(t){ return [t.rva,t.n,t.taken]; }); }
};
'''


def main():
    sig = {r: signature(r) for r in RVAS}
    every = {r: 1 for r in RVAS}
    if a.every_json:
        d = json.load(open(a.every_json))
        last = d['snaps'][-1]['c']
        for i, k in enumerate(d['targets']):
            r = int(k.split(':')[0], 16)
            if r in every:
                tot = last[i][0] + last[i][1] + last[i][2]
                every[r] = max(1, tot // (a.max + 1))
    res = {'argv': sys.argv, 'sig': {'0x%08x' % r: sig[r] for r in RVAS}, 'every': {'0x%08x' % r: every[r] for r in RVAS},
           'samples': []}
    print('signatures', res['sig'], flush=True)
    print('every', res['every'], flush=True)
    for r in RVAS:
        for t in 'XS':
            if IMG[t].addr(r) is None:
                print(f'WARNING {r:08x} not in port image {t}', flush=True)
    dev = frida.get_local_device()
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith('MASHED_')}
    env.update({'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'})
    exe = Path(a.root) / 'original' / 'MASHED.exe'
    pid = dev.spawn(str(exe), cwd=str(exe.parent), env=env)
    print('spawned pid', pid, flush=True)
    res['pid'] = pid
    br = Bridge()
    stop = threading.Event()

    def worker():
        while not stop.is_set():
            try:
                p = br.q.get(timeout=0.5)
            except queue.Empty:
                continue
            t0 = time.time()
            rva = p['rva']
            regs = p['regs']
            cache = {}
            esp0 = regs['esp']
            V = {}
            cw = p['cw']
            ret, nargs = sig[rva]
            err = None
            try:
                ra = br.read(esp0 & ~(PAGE - 1))
                cache[esp0 & ~(PAGE - 1)] = ra
                retaddr = struct.unpack_from('<I', ra, esp0 & (PAGE - 1))[0] if ra and (esp0 & (PAGE - 1)) <= PAGE - 4 \
                    else None
                if retaddr is None:
                    d2 = br.read((esp0 + 4) & ~(PAGE - 1)); cache[(esp0 + 4) & ~(PAGE - 1)] = d2
                    retaddr = struct.unpack('<I', (ra + d2)[esp0 & (PAGE - 1):(esp0 & (PAGE - 1)) + 4])[0]
                cw24, cw53 = (cw & ~0x300), (cw & ~0x300) | 0x200
                plan = [('O', rva, cw, p['mx']), ('O53', rva, cw53, p['mx']), ('O24', rva, cw24, p['mx'])]
                xa, sa = IMG['X'].addr(rva), IMG['S'].addr(rva)
                if xa:
                    plan += [('X53', xa, cw53, 0x1fa0), ('X24', xa, cw24, 0x1fa0)]
                if sa:
                    plan += [('S', sa, cw, 0x1fa0)]
                for name, va, c, mx in plan:
                    V[name] = run_variant(va, regs, c, mx, cache, br, esp0, nargs, retaddr)
            except Exception as ex:
                err = repr(ex)
            addrs = sorted(set().union(*[v['st']['writes'] for v in V.values()])) if V else []
            rd = []
            for x in addrs[:65536]:
                if rd and rd[-1][0] + rd[-1][1] == x:
                    rd[-1][1] += 1
                else:
                    rd.append([x, 1])
            key = '%08x:%d' % (rva, p['n'])
            br.done(rd, ret in ('float', 'double', 'float10', '?'), key)
            rec = {'key': key, 'live_n': min(len(addrs), 65536), 'err': err, 'rva': '0x%08x' % rva, 'n': p['n'], 'cw': cw, 'mx': p['mx'], 'tid': p['tid'],
                   'secs': round(time.time() - t0, 2), 'pages': len(cache)}
            refused = {k: v['st']['refused'] for k, v in V.items() if v['st']['refused']}
            if refused:
                rec['refused'] = refused
            rec['n_writes'] = len(addrs)
            dig = {}
            for k, v in V.items():
                if v['st']['refused']:
                    continue
                mem = bytes((byte_at(v, x, cache) or 0) for x in addrs)
                retv = {'eax': v['eax']} if ret not in ('void', 'float', 'double', 'float10') else {}
                if ret in ('float', 'double', 'float10', '?'):
                    retv['st0'] = v['st0']; retv['st0f'] = v['st0f']
                dig[k] = (mem, retv, v['esp'])
            if 'O' in dig:
                o = dig['O']
                rec['o_mem'] = o[0][:65536].hex()
                rec['o_ret'] = o[1]
                rec['eq'] = {}
                rec['eqf'] = {}
                for k, d in dig.items():
                    rec['eq'][k] = d[0] == o[0] and d[1] == o[1] and d[2] == o[2]
                    r1 = {kk: vv for kk, vv in d[1].items() if kk != 'st0'}
                    r0 = {kk: vv for kk, vv in o[1].items() if kk != 'st0'}
                    rec['eqf'][k] = d[0] == o[0] and r1 == r0 and d[2] == o[2]
                    if not rec['eq'][k]:
                        diffs = [(hex(addrs[j]), o[0][j], d[0][j]) for j in range(len(addrs)) if o[0][j] != d[0][j]][:6]
                        rec.setdefault('diff', {})[k] = {'mem': diffs, 'ret_o': o[1], 'ret_v': d[1],
                                                         'esp': (o[2], d[2])}
            res['samples'].append(rec)
            print(rec['rva'], 'n=%d cw=%04x' % (rec['n'], cw), 'pages', rec['pages'], 'writes', rec['n_writes'],
                  'eq', rec.get('eq'), ('REFUSED ' + str(refused)) if refused else '', rec['secs'], 's', flush=True)

    th = threading.Thread(target=worker, daemon=True)
    try:
        sess = dev.attach(pid)
        scr = sess.create_script(AGENT)
        br.script = scr
        scr.on('message', br.on_message)
        scr.load()
        att = scr.exports_sync.arm([[r, every[r], a.max] for r in RVAS if r not in LATE])
        res['attach'] = att
        print('attach', att, flush=True)
        th.start()
        nav = sess.create_script((REPO / 're' / 'frida' / 'nav_agent.js').read_text(encoding='utf-8'))
        nav.load(); N = nav.exports_sync; N.init()
        dev.resume(pid)

        def wait(pred, t):
            end = time.time() + t
            while time.time() < end:
                if pred():
                    return True
                time.sleep(0.1)
            return False

        def press(c, ms=180):
            N.press(c, ms); time.sleep(ms / 1000.0 + 0.3)

        def confirm_to(target, tries=6):
            for _ in range(tries):
                if N.depth() >= target:
                    return True
                press(4)
                if wait(lambda: N.depth() >= target, 2.0):
                    return True
            return N.depth() >= target

        if not wait(lambda: N.phase() == 3 and N.depth() >= 1, 90):
            raise SystemExit('never reached title')
        time.sleep(1.0)
        confirm_to(2); time.sleep(0.4); press(4); time.sleep(0.8)
        confirm_to(3)
        N.setsel(1); time.sleep(0.3)
        confirm_to(4, 4); confirm_to(5, 4)
        press(4); time.sleep(1.5)
        for _ in range(5):
            if N.phase() != 3:
                break
            press(4); time.sleep(1.5)
        print('phase after confirm', N.phase(), flush=True)
        if LATE:
            time.sleep(a.late_after)
            res['attach_late'] = scr.exports_sync.arm([[r, 1, a.max] for r in LATE])
            print('late attach', res['attach_late'], flush=True)
        end = time.time() + a.race_s
        while time.time() < end:
            time.sleep(1)
        res['counts'] = scr.exports_sync.counts()
    finally:
        stop.set()
        time.sleep(1.0)
        nl = 0
        for rec in res['samples']:
            lv = br.live.get(rec.get('key'))
            if lv is None or 'o_mem' not in rec:
                continue
            nl += 1
            mem_eq = lv[0].hex() == rec['o_mem'][:2 * rec['live_n']]
            r_eq = ('st0f' not in rec['o_ret']) or rec['o_ret'].get('st0f') == lv[2]
            rec['live_eq'] = bool(mem_eq and r_eq)
            rec['live_st0'] = lv[1]; rec['live_st0f'] = lv[2]
            if not mem_eq:
                lb, ob = lv[0], bytes.fromhex(rec['o_mem'])
                rec['live_diff'] = [j for j in range(min(len(lb), len(ob))) if lb[j] != ob[j]][:8]
            del rec['o_mem']
        for rec in res['samples']:
            rec.pop('o_mem', None)
        print('live legs', nl, 'equal', sum(1 for r in res['samples'] if r.get('live_eq')), flush=True)
        Path(a.out).write_text(json.dumps(res))
        print('->', a.out, flush=True)
        try:
            dev.kill(pid); print('killed own pid', pid, flush=True)
        except Exception as e:
            print('kill', e, flush=True)


if __name__ == '__main__':
    main()
