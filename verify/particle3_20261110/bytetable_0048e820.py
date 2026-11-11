"""PORT-OT-PARTICLE-3: full byte-diff table of re/match/s15/0048e820.c against MASHED.exe.unpatched 0x0048e820 (L-115
order). Whole obj symbol (code + switch table) compared byte k to byte k, relocation sites masked. Each differing
byte is classified: it must sit in an instruction whose bytes equal the original's once ESI and EDI are exchanged in
our instruction (ModRM reg / r/m field, or the +r register in the opcode byte).
usage: py -3.12 verify/particle3_20261110/bytetable_0048e820.py
"""
import pathlib, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / 're' / 'tools' / 'match'))
import capstone
import match as M

ADDR = 0x0048e820
SRC = pathlib.Path(__file__).resolve().parents[2] / 're' / 'match' / 's15' / '0048e820.c'
text = SRC.read_text(encoding='utf-8')
flags = text.split('// FLAGS:', 1)[1].split('\n', 1)[0].strip()
obj = M.compile_obj(SRC, flags, toolchain=M.file_toolchain(text))
secs, syms = M.coff(obj)
mine, relocs, full, local = M.function_from_obj(secs, syms, 'FUN_0048e820')
while mine and mine[-1] in (0xCC, 0x90):
    mine = mine[:-1]
orig = M.original_bytes(ADDR, len(mine))
masked = {r + k for r in relocs for k in range(4)}
# local relocations (switch table entries, internal jumps): the original holds the absolute address
for site, tgt in local.items():
    for k in range(4):
        masked.add(site + k)
bad = [i for i in range(len(mine)) if i not in masked and mine[i] != orig[i]]

cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
code_end = min(local.values()) if local else len(mine)   # first switch-table target offset bounds the code
ins_m = list(cs.disasm(mine, 0))
ins_o = list(cs.disasm(orig, 0))
by_off_m = {i.address: i for i in ins_m}
by_off_o = {i.address: i for i in ins_o}

def swap(s):
    return s.replace('esi', '\0').replace('edi', 'esi').replace('\0', 'edi')

def containing(ins_list, off):
    for i in ins_list:
        if i.address <= off < i.address + i.size:
            return i
    return None

rows, unexplained = [], 0
seen = set()
for b in bad:
    im, io = containing(ins_m, b), containing(ins_o, b)
    if im is None or io is None or im.address in seen:
        continue
    seen.add(im.address)
    same_len = im.size == io.size and im.address == io.address
    nb = [k for k in range(im.size) if im.address + k in bad]
    # every differing byte must be a 6 <-> 7 exchange (ESI <-> EDI) in one 3-bit register field: bits 0-2 (ModRM r/m,
    # or the +r of a one-byte opcode such as BE/BF, 4E/4F) or bits 3-5 (ModRM reg), with all other bits equal
    def regswap(x, y):
        d = x ^ y
        if d == 1 and {x & 7, y & 7} == {6, 7}:
            return True
        if d == 8 and {(x >> 3) & 7, (y >> 3) & 7} == {6, 7}:
            return True
        if d == 9 and {x & 7, y & 7} == {6, 7} and {(x >> 3) & 7, (y >> 3) & 7} == {6, 7}:
            return True
        return False
    ok = same_len and all(regswap(mine[im.address + k], orig[im.address + k]) for k in nb) and         swap(im.mnemonic) == io.mnemonic
    unexplained += 0 if ok else 1
    rows.append((im.address, bytes(im.bytes).hex(' '), '%s %s' % (im.mnemonic, im.op_str),
                 bytes(io.bytes).hex(' '), '%s %s' % (io.mnemonic, io.op_str), len(nb),
                 'ESI<->EDI' if ok else 'UNEXPLAINED'))

print('symbol %s: %d B (code + switch table), %d relocation-masked bytes, %d differing bytes in %d instructions, '
      '%d unexplained' % (full, len(mine), len(masked), len(bad), len(rows), unexplained))
print('| # | off | our bytes | ours | original bytes | original | diff bytes | class |')
print('|---|---|---|---|---|---|---|---|')
for n, r in enumerate(rows, 1):
    print('| %d | +0x%02x | `%s` | %s | `%s` | %s | %d | %s |' % ((n,) + r))
outside = [b for b in bad if containing(ins_m, b) is None]
print('differing bytes outside any instruction (table / pad): %d' % len(outside))
