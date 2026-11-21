"""Show the differing bytes of one DIFFERENT unit: ddk exe symbol body vs the original at the rva.
usage (worktree root): py -3.12 verify/bindbulk_20261121/tools/diffbytes.py 0x004744a0"""
import csv, struct, sys
rva = sys.argv[1].lower()
row = [r for r in csv.DictReader(open('mashedmod/build/ddkobj/BYTECHECK.tsv'), delimiter='\t') if r['rva'].lower() == rva][0]
def va_to_off(data, va, imagebase=None):
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    nsec = struct.unpack_from('<H', data, pe + 6)[0]
    opt = struct.unpack_from('<H', data, pe + 20)[0]
    base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
    so = pe + 24 + opt
    for i in range(nsec):
        n, vs, vaddr, rs, rp = struct.unpack_from('<8sIIII', data, so + 40 * i)
        if vaddr <= va < vaddr + max(vs, rs):
            return rp + (va - vaddr), base
    raise SystemExit('va not in a section')
orig = open('original/MASHED.exe.unpatched', 'rb').read()
ddk = open('mashedmod/build/ddkobj/mashed_re_ddk.exe', 'rb').read()
size = int(row['size'])
o_off, o_base = va_to_off(orig, int(rva, 16) - 0x400000)
d_off, d_base = va_to_off(ddk, int(row['exe_va'], 16))
a, b = orig[o_off:o_off + size], ddk[d_off:d_off + size]
print(row['rva'], row['exe_symbol'], 'size', size, 'first_diff', row['first_diff'])
idx = [i for i in range(size) if a[i] != b[i]]
print('differing offsets', [hex(i) for i in idx])
lo = max(0, idx[0] - 12); hi = min(size, idx[-1] + 12)
print('orig', a[lo:hi].hex(' ')); print('ddk ', b[lo:hi].hex(' ')); print('window from +%#x' % lo)
