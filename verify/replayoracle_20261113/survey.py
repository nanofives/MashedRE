import struct, glob, os, json, datetime, sys
BASE = 0x005ea000
A = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x0068bc0c
rows = []
for m in glob.glob('D:/MashedWT/*_data/**/meta.json', recursive=True):
    d = os.path.dirname(m)
    f = os.path.join(d, 'ckpt00.data.bin')
    if not os.path.exists(f):
        continue
    b = open(f, 'rb').read()
    v = struct.unpack_from('<I', b, A - BASE)[0]
    try:
        j = json.load(open(m))
        exe = j.get('exe', '')
        sc = j.get('scenario') or (j.get('cfg') or {}).get('scen')
        hooks = j.get('hooks')
        prov = bool(j.get('prov_cfg'))
    except Exception:
        exe = sc = hooks = prov = '?'
    root = exe.split(os.sep)[2] if exe.count(os.sep) > 2 else exe
    rows.append((os.path.getmtime(m), d.replace(os.sep, '/'), v, root, json.dumps(sc, separators=(',', ':')), hooks, prov))
rows.sort()
for r in rows:
    print(datetime.datetime.fromtimestamp(r[0]).strftime('%m-%d_%H:%M'), r[2], r[1], r[3], r[4], 'hooks=%s' % r[5], 'prov=%s' % r[6])
