# audiolink: find every .text instruction whose immediate or displacement is one of the given VAs (byte search for the
# little-endian dword, then decode at each start offset 1..8 bytes back that yields an instruction covering the dword).
# Also prints the enclosing function start guess (back to the previous CC/90 padding run). Usage: xref_scan.py VA [VA ...]
import struct, sys, capstone
d = open(r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched', 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]; n = struct.unpack_from('<H', d, pe+6)[0]; osz = struct.unpack_from('<H', d, pe+20)[0]
base = struct.unpack_from('<I', d, pe+52)[0]; s0 = pe+24+osz
vs, va, rs, ro = struct.unpack_from('<IIII', d, s0+8)          # .text = first section
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def fstart(o):
    k = o
    while k > ro and not (d[k-1] in (0xcc, 0x90) and d[k-2] in (0xcc, 0x90, 0xc3)): k -= 1
    return base + va + (k - ro)
for arg in sys.argv[1:]:
    t = int(arg, 16); pat = struct.pack('<I', t); i = ro; hits = 0
    print(f'== {t:#010x}')
    while True:
        i = d.find(pat, i, ro + rs)
        if i < 0: break
        best = None
        for back in range(1, 9):
            st = i - back
            ins = next(md.disasm(d[st:st+16], base + va + (st - ro)), None)
            if ins and st + ins.size >= i + 4 and st <= i:
                best = ins; break
        if best:
            hits += 1
            print(f'  {best.address:08x}: {best.mnemonic} {best.op_str}   (fn ~{fstart(i):08x})')
        i += 1
    print(f'  {hits} hits')
