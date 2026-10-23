# Disassemble the slice-1 audiolink functions from original/MASHED.exe.unpatched (and flag any byte
# difference against the patched MASHED.exe in the same range). Linear sweep from the entry; stops at
# the first RET whose following byte is padding (0xCC / 0x90) or at the next listed entry, cap 0x300 bytes.
import struct, sys
from pathlib import Path
import capstone
MAIN = Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/original")
RVAS = [0x005b1160,0x005aefa0,0x005b4060,0x005af200,0x005b10a0,0x005b10e0,0x005b3580,0x005b35a0,0x005b3670,
        0x005b36b0,0x005ac540,0x005ade60,0x005aded0,0x005ae590,0x005aeda0,0x005b0700,0x005b73b0,0x005b9410,
        0x005baf40,0x005bb5b0,0x005bc450,0x005bcb80,0x005bf660,0x005bfcc0,0x005c7500,0x005c75b0,0x005c9380,
        0x005c9770,0x005baf00]
def load(p):
    d = p.read_bytes(); pe = struct.unpack_from("<I", d, 0x3C)[0]
    n = struct.unpack_from("<H", d, pe+6)[0]; osz = struct.unpack_from("<H", d, pe+20)[0]
    base = struct.unpack_from("<I", d, pe+24+28)[0]; s0 = pe+24+osz; secs=[]
    for i in range(n):
        s = s0+i*40; vsz, va, rsz, ro = struct.unpack_from("<IIII", d, s+8); secs.append((va, max(vsz,rsz), ro))
    def off(v):
        r = v-base
        for va, sz, ro in secs:
            if va <= r < va+sz: return ro+(r-va)
    return d, off
du, offu = load(MAIN/"MASHED.exe.unpatched"); dp, offp = load(MAIN/"MASHED.exe")
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
starts = sorted(RVAS)
for va in RVAS:
    nxt = min([s for s in starts if s > va] + [va+0x300]); cap = min(nxt, va+0x300)
    o = offu(va); code = du[o:o+(cap-va)]; end = cap
    lines=[]
    for ins in md.disasm(code, va):
        lines.append(f"  {ins.address:08x}: {ins.bytes.hex():<20} {ins.mnemonic} {ins.op_str}")
        if ins.mnemonic.startswith("ret"):
            nb = du[o+(ins.address+ins.size-va)] if ins.address+ins.size < cap else 0xCC
            if nb in (0xCC, 0x90): end = ins.address+ins.size; break
    po = offp(va); patched = dp[po:po+(end-va)] != du[o:o+(end-va)]
    print(f"== {va:#010x} .. {end:#010x}  ({end-va} bytes){'  PATCHED-DIFFERS' if patched else ''}")
    print("\n".join(lines)); print()
