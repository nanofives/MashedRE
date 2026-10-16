"""List every instruction in MASHED.exe (.unpatched anchor) whose memory displacement is a course
clump slot (course + 0x10110 + 4*i, i < 0x40) or an Mts count/record base, inside an address range.
Static witness only. Usage: clumpref.py <lo> <hi> [--table]"""
import sys, struct
from pathlib import Path
import capstone, pefile

EXE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
SLOT_TABLE = 0x005f33f8
NAMES = {}

def load():
    pe = pefile.PE(str(EXE))
    base = pe.OPTIONAL_HEADER.ImageBase
    img = pe.get_memory_mapped_image()
    return base, img

def slot_owner(base, img):
    # map handler entry -> (track, slot)
    own = {}
    for i in range(13):
        r = img[SLOT_TABLE - base + i*0x48: SLOT_TABLE - base + (i+1)*0x48]
        name = r[:16].split(b'\0')[0].decode()
        for k, p in enumerate(struct.unpack_from('<13I', r, 20)):
            if p and p != 0x0045b350:
                own.setdefault(p, []).append(f"{name}.s{k}")
    return own

def main():
    lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
    base, img = load()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
    code = bytes(img[lo-base:hi-base])
    hits = []
    # linear sweep from lo, resync on bad byte by stepping one byte (memory: capstone sweeps stop at bad bytes)
    off = 0
    while off < len(code):
        got = False
        for ins in md.disasm(code[off:], lo+off):
            got = True
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_MEM:
                    d = op.mem.disp & 0xffffffff
                    if 0x10110 <= d < 0x10210 and (d-0x10110) % 4 == 0:
                        hits.append((ins.address, f"clump[{(d-0x10110)//4}]", ins.mnemonic+' '+ins.op_str))
                    elif 0x10000 <= d < 0x10040 and d % 4 == 0:
                        hits.append((ins.address, f"mtscount[{(d-0x10000)//4}]", ins.mnemonic+' '+ins.op_str))
                elif op.type == capstone.x86.X86_OP_IMM and ins.mnemonic in ('add','lea','mov') and 0x10110 <= (op.imm & 0xffffffff) < 0x10210:
                    hits.append((ins.address, f"clump-imm[{((op.imm&0xffffffff)-0x10110)//4}]", ins.mnemonic+' '+ins.op_str))
            off = ins.address + ins.size - lo
        if not got:
            off += 1
    own = slot_owner(base, img)
    starts = sorted(own)
    import bisect
    for a, what, txt in hits:
        j = bisect.bisect_right(starts, a) - 1
        fn = starts[j] if j >= 0 else 0
        print(f"0x{a:08x}  {what:16s} {txt:40s} after-slot-fn 0x{fn:08x} {','.join(own.get(fn, []))}")

if __name__ == '__main__':
    main()
