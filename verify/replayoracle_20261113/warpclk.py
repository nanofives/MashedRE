import struct, glob, os, json, collections, sys
BASE = 0x005ea000
A = int(sys.argv[1], 16) if len(sys.argv) > 1 else 0x0068bc0c
rows = []
for m in glob.glob('D:/MashedWT/*_data/**/meta.json', recursive=True):
    d = os.path.dirname(m)
    f = os.path.join(d, 'ckpt00.data.bin')
    if not os.path.exists(f):
        continue
    v = struct.unpack_from('<I', open(f, 'rb').read(), A - BASE)[0]
    try:
        j = json.load(open(m))
        samp = dict((s[0], s[1]) for s in j['warp']['pins']['norm']['sample'])
        boot = bool(j.get('boot')) or bool((j.get('cfg') or {}).get('boot'))
    except Exception:
        continue
    rows.append((samp.get('7f0ff0'), samp.get('771978'), v, boot, d.replace(os.sep, '/')))
rows.sort(key=lambda r: (r[0] is None, r[0] or 0))
by = collections.defaultdict(collections.Counter)
for r in rows:
    by[(r[0], r[3])][r[2]] += 1
for k in sorted(by, key=lambda k: (k[0] is None, k[0] or 0)):
    print('7f0ff0@warp=%s boot=%s  values=%s' % (k[0], k[1], dict(by[k])))
