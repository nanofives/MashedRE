import sys, struct, re, capstone
dll, mapf = sys.argv[1], sys.argv[2]
names = sys.argv[3:]
d = open(dll,'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
nsec = struct.unpack_from('<H', d, pe+6)[0]; opt = struct.unpack_from('<H', d, pe+20)[0]
base = struct.unpack_from('<I', d, pe+24+28)[0]
secs=[]; o=pe+24+opt
for i in range(nsec):
    vs,va,rs,ra = struct.unpack_from('<IIII', d, o+8); secs.append((va,vs,ra,rs)); o+=40
def off(v):
    r=v-base
    for va,vs,ra,rs in secs:
        if va<=r<va+max(vs,rs): return r-va+ra
syms={}
for l in open(mapf, encoding='latin-1'):
    m=re.match(r'\s*\w+:\w+\s+(\S+)\s+([0-9a-f]{8})\s', l)
    if m: syms[m.group(1)]=int(m.group(2),16)
rev={v:k for k,v in syms.items()}
md=capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for n in names:
    a=syms[n]; fo=off(a); print('==',n,hex(a))
    for ins in md.disasm(d[fo:fo+0x1000], a):
        t=''
        if ins.mnemonic in ('call','jmp') and ins.op_str.startswith('0x'):
            t='  ; '+rev.get(int(ins.op_str,16),'')
        print(f'  {ins.address:08x} {ins.mnemonic} {ins.op_str}{t}')
        if ins.mnemonic in ("int3",) or (ins.mnemonic=="ret" and len(sys.argv)<0): break
