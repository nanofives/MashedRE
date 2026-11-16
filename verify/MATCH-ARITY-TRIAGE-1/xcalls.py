"""py -3.12 xcalls.py <target_hex>...  every direct call/jmp to target in .text with the 4 instructions before it"""
import sys, pefile, capstone
pe = pefile.PE(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", fast_load=True)
t = [s for s in pe.sections if s.Name.rstrip(b"\0") == b".text"][0]
va = 0x400000 + t.VirtualAddress; d = t.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for a in sys.argv[1:]:
    tgt = int(a, 16); n = 0
    for off in range(len(d) - 5):
        if d[off] in (0xE8, 0xE9) and (va + off + 5 + int.from_bytes(d[off + 1:off + 5], "little", signed=True)) & 0xffffffff == tgt:
            at = va + off
            prev = list(md.disasm(d[off - 24:off + 5], at - 24))
            if not prev or prev[-1].address != at:
                continue
            n += 1
            print(f"{tgt:#010x} {'call' if d[off] == 0xE8 else 'jmp'} @ {at:#010x}: " + " ; ".join(f"{i.address:#010x} {i.mnemonic} {i.op_str}" for i in prev[-6:-1]))
    print(f"{tgt:#010x}: {n} sites")
