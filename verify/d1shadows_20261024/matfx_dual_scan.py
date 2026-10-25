import sys, struct, collections
b = open(sys.argv[1], 'rb').read()
cnt = collections.Counter()
off = 0
while True:
    i = b.find(struct.pack('<I', 0x120), off)
    if i < 0: break
    off = i + 4
    if i % 4: continue
    sz, ver = struct.unpack_from('<II', b, i + 4)
    if sz > 4096 or (ver & 0xFFFF) not in (0xFFFF,) and ver >> 16 not in range(0x0000, 0x4000):
        pass
    if sz < 8 or sz > 2048: continue
    d = b[i + 12:i + 12 + sz]
    e1, e2 = struct.unpack_from('<II', d, 0)
    # first effect payload
    p = 4
    desc = [hex(e1)]
    for k in range(2):
        if p + 4 > len(d): break
        et = struct.unpack_from('<I', d, p)[0]; p += 4
        if et == 4:
            sb, db, ht = struct.unpack_from('<III', d, p); p += 12
            tex = ''
            if ht:
                # texture chunk
                tsz = struct.unpack_from('<I', d, p + 4)[0]
                tb = d[p + 12:p + 12 + tsz]
                # struct (12+4) then string chunk
                q = 12 + 4
                ssz = struct.unpack_from('<I', tb, q + 4)[0]
                tex = tb[q + 12:q + 12 + ssz].split(b'\0')[0].decode('latin1')
                p += 12 + tsz
            desc.append(f'dual src={sb} dst={db} tex={tex}')
        elif et == 0:
            desc.append('none')
        else:
            desc.append(f'type{et}'); break
    cnt[' | '.join(desc)] += 1
for k, v in cnt.most_common():
    print(v, k)

