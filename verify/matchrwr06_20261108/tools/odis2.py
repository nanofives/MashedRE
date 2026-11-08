"""dis.py 0xVA [maxbytes] : capstone disasm from VA until a ret followed by cc/90 padding up to 16-align (or maxbytes)."""
import sys, pathlib
sys.path.insert(0, 're/tools/match')
import capstone, match
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for a in sys.argv[1:]:
    addr = int(a, 16); mx = 4000
    b = match.original_bytes(addr, mx)
    tgt = set()
    for ins in md.disasm(b, addr):
        if ins.mnemonic.startswith('j'):
            try: tgt.add(int(ins.op_str, 16))
            except: pass
        print(f"  {ins.address:08x}  {ins.bytes.hex():16} {ins.mnemonic} {ins.op_str}")
        if ins.mnemonic == 'ret' and not any(t > ins.address for t in tgt):
            end = ins.address + ins.size
            print(f"; END 0x{end:08x} size {end-addr}  next bytes {b[end-addr:end-addr+8].hex()}")
            break
    print()
