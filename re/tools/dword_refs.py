# Every absolute-dword occurrence of each target VA in a PE image, with the .text instruction
# that carries it (capstone, decoded from 1..7 bytes before the dword). Finds pointer installs
# (push offset fn, mov [g], fn) that a direct-call scan misses.
# Usage: py -3.12 re/tools/dword_refs.py original/MASHED.exe.unpatched 0x004991f0 [0x... ...]
import sys,struct,capstone,pefile
pe=pefile.PE(sys.argv[1]); data=pe.__data__
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
def off2va(o):
    for s in pe.sections:
        if s.PointerToRawData<=o<s.PointerToRawData+s.SizeOfRawData:
            return 0x400000+s.VirtualAddress+o-s.PointerToRawData, s.Name.rstrip(b'\0').decode()
    return None,None
for t in sys.argv[2:]:
    tv=int(t,16); pat=struct.pack('<I',tv); i=0; print('###',t)
    while True:
        i=data.find(pat,i)
        if i<0: break
        va,sec=off2va(i)
        line=''
        if sec=='.text':
            for k in range(1,8):
                ins=next(md.disasm(data[i-k:i-k+15],va-k),None)
                if ins and ins.address+ins.size>=va+4 and ins.address<va:
                    line=f'{hex(ins.address)} {ins.mnemonic} {ins.op_str}'; break
        print(' ',hex(va),sec,line)
        i+=1
