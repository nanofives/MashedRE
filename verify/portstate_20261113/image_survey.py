"""Read-only survey of original/MASHED.exe.unpatched for the PORTSTATE-IMAGE design.
Usage: py -3.12 verify/portstate_20261113/image_survey.py <MASHED.exe.unpatched> <PortStateSeed.inc> <PortTop_RdataSeed.inc> <PortTop_DInputSeed.inc> <PortTopCodePtr.inc>
"""
import hashlib, re, struct, sys

exe, seed_inc, rdata_inc, dinput_inc, cp_inc = sys.argv[1:6]
b = open(exe, 'rb').read()
print('file', exe, 'size', len(b), 'sha256', hashlib.sha256(b).hexdigest().upper())
e_lfanew = struct.unpack_from('<I', b, 0x3c)[0]
assert b[e_lfanew:e_lfanew + 4] == b'PE\0\0'
fh = e_lfanew + 4
machine, nsec, tds, _, _, opt_size, chars = struct.unpack_from('<HHIIIHH', b, fh)
opt = fh + 20
magic = struct.unpack_from('<H', b, opt)[0]
image_base = struct.unpack_from('<I', b, opt + 28)[0]
sec_align, file_align = struct.unpack_from('<II', b, opt + 32)
size_image = struct.unpack_from('<I', b, opt + 56)[0]
n_dd = struct.unpack_from('<I', b, opt + 92)[0]
print('e_lfanew 0x%x machine 0x%x nsec %d timestamp 0x%08x characteristics 0x%04x (RELOCS_STRIPPED=%d)'
      % (e_lfanew, machine, nsec, tds, chars, chars & 1))
print('optional magic 0x%x ImageBase 0x%08x SectionAlignment 0x%x FileAlignment 0x%x SizeOfImage 0x%x'
      % (magic, image_base, sec_align, file_align, size_image))
dd_names = ['EXPORT', 'IMPORT', 'RESOURCE', 'EXCEPTION', 'SECURITY', 'BASERELOC', 'DEBUG', 'ARCH', 'GLOBALPTR',
            'TLS', 'LOAD_CONFIG', 'BOUND_IMPORT', 'IAT', 'DELAY_IMPORT', 'CLR', 'RES']
dds = {}
for i in range(n_dd):
    rva, sz = struct.unpack_from('<II', b, opt + 96 + 8 * i)
    dds[dd_names[i]] = (rva, sz)
    if sz:
        print('  datadir %-12s rva 0x%06x size 0x%x  (VA 0x%08x) [header field at file 0x%x]'
              % (dd_names[i], rva, sz, image_base + rva, opt + 96 + 8 * i))
sh = opt + opt_size
secs = []
for i in range(nsec):
    name = b[sh + 40 * i: sh + 40 * i + 8].rstrip(b'\0').decode()
    vsize, va, rsize, rptr = struct.unpack_from('<IIII', b, sh + 40 * i + 8)
    ch = struct.unpack_from('<I', b, sh + 40 * i + 36)[0]
    secs.append((name, va, vsize, rptr, rsize, ch))
    fb = min(vsize, rsize)
    print('  section %-8s hdr@file 0x%x  VA 0x%08x..0x%08x vsize 0x%06x raw 0x%06x@0x%06x file-backed 0x%06x '
          'BSS 0x%06x chars 0x%08x'
          % (name, sh + 40 * i, image_base + va, image_base + va + vsize, vsize, rsize, rptr, fb,
             max(0, vsize - rsize), ch))

def sec(name):
    return [s for s in secs if s[0] == name][0]

def va2off(va):
    for name, sva, vs, rp, rs, _ in secs:
        a = image_base + sva
        if a <= va < a + min(vs, rs):
            return rp + va - a
    return None

text = sec('.text')
tlo, thi = image_base + text[1], image_base + text[1] + text[2]
print('.text range 0x%08x..0x%08x' % (tlo, thi))

# Base relocations
rel_rva, rel_sz = dds.get('BASERELOC', (0, 0))
highlow = []
if rel_sz:
    off = va2off(image_base + rel_rva)
    end = off + rel_sz
    while off < end:
        page, bsz = struct.unpack_from('<II', b, off)
        if bsz == 0:
            break
        for j in range((bsz - 8) // 2):
            ent = struct.unpack_from('<H', b, off + 8 + 2 * j)[0]
            t, o = ent >> 12, ent & 0xfff
            if t == 3:
                highlow.append(image_base + page + o)
            elif t != 0:
                print('  reloc type', t)
        off += bsz
print('base relocations HIGHLOW:', len(highlow))

# Pointers in file-backed .rdata/.data
for sname in ('.rdata', '.data'):
    name, va, vs, rp, rs, _ = sec(sname)
    lo = image_base + va
    fb = min(vs, rs)
    rel_in = [r for r in highlow if lo <= r < lo + fb]
    def tgt(r):
        return struct.unpack_from('<I', b, va2off(r))[0]
    rel_text = [r for r in rel_in if tlo <= tgt(r) < thi]
    rel_data = [r for r in rel_in if not (tlo <= tgt(r) < thi)]
    # unrelocated scan: aligned dwords whose value lands in .text
    scan = []
    for o in range(0, fb - 3, 4):
        v = struct.unpack_from('<I', b, rp + o)[0]
        if tlo <= v < thi:
            scan.append(lo + o)
    rel_set = set(rel_text)
    print('%s file-backed 0x%x: relocs %d (into .text %d, elsewhere %d); aligned-dword scan into .text %d, '
          'scan-only (no reloc) %d, reloc-only (not in scan) %d'
          % (sname, fb, len(rel_in), len(rel_text), len(rel_data), len(scan), len(set(scan) - rel_set),
             len(rel_set - set(scan))))
    if sname == '.data':
        bss_hi = lo + vs
        print('   .data BSS tail 0x%08x..0x%08x' % (lo + fb, bss_hi))
    # distinct targets
    tg = sorted({tgt(r) for r in rel_text})
    print('   distinct .text targets via relocs: %d' % len(tg))
    globals()['rel_text_' + sname[1:]] = rel_text

all_code_slots = set(rel_text_rdata) | set(rel_text_data)

# Seed equality: every seed entry's bytes vs the file image
def parse_portstate(path):
    t = open(path).read()
    data_m = re.search(r'kPortStateSeedData\[\d+\] = \{(.*?)\};', t, re.S)
    data = bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})\b', re.sub(r'//[^\n]*', '', data_m.group(1))))
    ents = [(int(a, 16), int(n), int(o)) for a, n, o in re.findall(r'\{ 0x([0-9a-f]+)u,\s*(\d+)u,\s*(\d+)u \}', t)]
    return [(a, data[o:o + n]) for a, n, o in ents]

def parse_strtab(path):
    out = []
    for a, n, s in re.findall(r'\{ 0x([0-9a-f]+)u, (\d+), "((?:[^"\\]|\\.)*)" \}', open(path).read()):
        bs = bytes(int(h, 16) for h in re.findall(r'\\x([0-9a-f]{2})', s))
        assert len(bs) == int(n), (a, n, len(bs))
        out.append((int(a, 16), bs))
    return out

def parse_cpseed(path):
    out = []
    t = open(path).read()
    for a, n, s in re.findall(r'CP_SEED\(0x([0-9a-fA-F]+)u?,\s*(\d+)u?,\s*"((?:[^"\\]|\\.)*)"\)', t):
        bs = bytes(int(h, 16) for h in re.findall(r'\\x([0-9a-f]{2})', s))
        out.append((int(a, 16), bs))
    fix = [int(x, 16) for x in re.findall(r'CP_FIX\(0x([0-9a-fA-F]+)', t)]
    return out, fix

def classify(addr, n):
    for name, va, vs, rp, rs, _ in secs:
        lo = image_base + va
        if lo <= addr and addr + n <= lo + vs:
            return name, 'file' if addr + n <= lo + min(vs, rs) else ('bss' if addr >= lo + min(vs, rs) else 'straddle')
    return None, None

for label, ents in (('PortStateSeed.inc', parse_portstate(seed_inc)),
                    ('PortTop_RdataSeed.inc', parse_strtab(rdata_inc)),
                    ('PortTop_DInputSeed.inc', parse_strtab(dinput_inc))):
    eq = neq = nfile = 0
    where = {}
    cp_overlap = 0
    for a, bs in ents:
        s, k = classify(a, len(bs))
        where[(s, k)] = where.get((s, k), 0) + 1
        o = va2off(a)
        if o is not None and k == 'file' and b[o:o + len(bs)] == bs:
            eq += 1
        else:
            neq += 1
            print('   MISMATCH/unbacked', label, hex(a), len(bs), s, k)
        if any(a <= c < a + len(bs) or c < a < c + 4 for c in all_code_slots):
            cp_overlap += 1
    print('%s: entries %d bytes %d equal-to-file %d other %d sections %s entries overlapping a reloc code slot %d'
          % (label, len(ents), sum(len(x) for _, x in ents), eq, neq, where, cp_overlap))

cps, fixes = parse_cpseed(cp_inc)
print('PortTopCodePtr.inc: CP_FIX slots %d (in reloc code-slot set %d), CP_SEED blocks %d'
      % (len(fixes), len(set(fixes) & all_code_slots), len(cps)))
print('CP_FIX slots not in reloc set:', [hex(x) for x in sorted(set(fixes) - all_code_slots)][:20])
eqc = sum(1 for a, bs in cps if va2off(a) is not None and b[va2off(a):va2off(a)+len(bs)] == bs)
print('PortTopCodePtr.inc CP_SEED blocks equal-to-file %d of %d, bytes %d' % (eqc, len(cps), sum(len(x) for _, x in cps)))
print('PortTopCodePtr.inc CP_FIX slots whose image dword lies in .text: %d of %d' % (sum(1 for s in fixes if tlo <= struct.unpack_from('<I', b, va2off(s))[0] < thi), len(fixes)))
