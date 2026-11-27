"""RO-AUDIOINIT-45D460-REACH-6: one dword from tick-trace image dumps (ckptNN.data.bin, base = meta.json lo, else
detorig.DATA_LO 0x005ea000). usage: py -3.12 dword_at.py 0x<addr> <dump> [<dump> ...]"""
import json, os, struct, sys
addr = int(sys.argv[1], 16)
for p in sys.argv[2:]:
    m = json.load(open(os.path.join(os.path.dirname(p), 'meta.json')))
    lo = m.get('lo') or m.get('data_lo') or 0x005ea000
    lo = int(lo, 16) if isinstance(lo, str) else lo
    with open(p, 'rb') as f:
        f.seek(addr - lo)
        b = f.read(4)
    v = struct.unpack('<I', b)[0]
    print('%s  lo=0x%08x  [0x%08x] = 0x%08x (signed %d)' % (p, lo, addr, v, struct.unpack('<i', b)[0]))
