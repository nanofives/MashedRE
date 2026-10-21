"""u9779: every instruction in MASHED.exe.unpatched .text whose bytes contain an absolute dword address, by byte search
then capstone decode backwards (tries start offsets 1..10 before the match, keeps decodes that cover the match).
usage: absref.py <va> [--writes]"""
import sys, struct, capstone
B = open(r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched', 'rb').read()
pe = struct.unpack_from('<I', B, 0x3c)[0]; ns = struct.unpack_from('<H', B, pe + 6)[0]; oh = struct.unpack_from('<H', B, pe + 20)[0]
ib = struct.unpack_from('<I', B, pe + 24 + 28)[0]; st = pe + 24 + oh
secs = []
for i in range(ns):
    name = B[st + i * 40:st + i * 40 + 8].rstrip(b'\0')
    vs, vaddr, rs, rp = struct.unpack_from('<IIII', B, st + i * 40 + 8)
    secs.append((name, vaddr + ib, rs, rp))
va = int(sys.argv[1], 16); pat = struct.pack('<I', va)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = False
for name, sva, rs, rp in secs:
    if name != b'.text': continue
    data = B[rp:rp + rs]; i = 0
    while True:
        j = data.find(pat, i)
        if j < 0: break
        i = j + 1; best = None
        for back in range(1, 11):
            s = j - back
            if s < 0: continue
            ins = next(md.disasm(data[s:s + 16], sva + s), None)
            if ins and ins.address + ins.size >= sva + j + 4 and ins.address <= sva + j:
                best = ins
                if ins.address + ins.size == sva + j + 4 or ins.size >= back + 4: break
        if best is None: print(f'{sva + j:08x}  ?'); continue
        txt = f'{best.mnemonic} {best.op_str}'
        w = ('[0x%x]' % va) in best.op_str.split(',')[0] and best.mnemonic not in ('cmp', 'push', 'test', 'fld', 'fild')
        if '--writes' in sys.argv and not w: continue
        print(f'{best.address:08x}  {txt}' + ('   <- WRITE' if w else ''))
