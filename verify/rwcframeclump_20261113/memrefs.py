# Absolute-dword memory references to each target VA in .text, keeping only decodes whose operand is the
# memory form [target] (dword_refs.py takes the first decode from 1..7 bytes back, which can be an
# immediate-form misdecode such as "or eax, imm32" when the byte before the dword is the modrm 0x0d).
# Also prints the read/write direction from capstone's operand access (owners: owner.py).
# Usage: py -3.12 memrefs.py <pe> 0x... [0x...]
import sys, struct, capstone, pefile
from capstone import x86
pe = pefile.PE(sys.argv[1]); data = pe.__data__
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
def off2va(o):
    for s in pe.sections:
        if s.PointerToRawData <= o < s.PointerToRawData + s.SizeOfRawData:
            return 0x400000 + s.VirtualAddress + o - s.PointerToRawData, s.Name.rstrip(b'\0').decode()
    return None, None
for t in sys.argv[2:]:
    tv = int(t, 16); pat = struct.pack('<I', tv); i = 0; print('###', t)
    while True:
        i = data.find(pat, i)
        if i < 0: break
        va, sec = off2va(i)
        if sec == '.text':
            cands = []
            for k in range(1, 8):
                ins = next(md.disasm(data[i - k:i - k + 15], va - k), None)
                if not ins or ins.address + ins.size < va + 4 or ins.address >= va: continue
                for op in ins.operands:
                    if op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and (op.mem.disp & 0xffffffff) == tv:
                        acc = ('R' if op.access & capstone.CS_AC_READ else '') + ('W' if op.access & capstone.CS_AC_WRITE else '')
                        cands.append(f'{ins.address:#010x} {acc or "?":2} {ins.mnemonic} {ins.op_str}')
            print(' ', hex(va), ' | '.join(cands) if cands else 'NO-MEM-DECODE')
        i += 1
