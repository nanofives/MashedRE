import re
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
P = r'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched'
pe = pefile.PE(P)
IB = pe.OPTIONAL_HEADER.ImageBase
data = open(P, 'rb').read()
t = [s for s in pe.sections if s.Name.startswith(b'.text')][0]
code = data[t.PointerToRawData:t.PointerToRawData + t.Misc_VirtualSize]
va0 = IB + t.VirtualAddress
md = Cs(CS_ARCH_X86, CS_MODE_32)
md.skipdata = True
ins = list(md.disasm(code, va0))
print('instructions', len(ins))
for k, i in enumerate(ins):
    s = i.op_str
    if i.mnemonic == 'imul' and s.endswith(', 0x68'):
        win = ins[k + 1:k + 12]
        base = [w for w in win if '0x68bb' in w.op_str or '0x68bc' in w.op_str]
        if not base:
            continue
        print('0x%08x imul %s  base: %s' % (i.address, s, '; '.join('0x%08x %s %s' % (w.address, w.mnemonic, w.op_str) for w in base)))
        for w in ins[k + 1:k + 60]:
            if w.mnemonic.startswith('mov') and re.search(r'\[e[a-z]{2} \+ 0x(44|54)\], ', w.op_str) and 'esp' not in w.op_str:
                print('     store 0x%08x %s %s' % (w.address, w.mnemonic, w.op_str))
            if w.mnemonic == 'ret':
                break
