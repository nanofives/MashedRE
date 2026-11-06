"""Print the FUN_004046a0 seed records of MATCH-T13-SEED oracle json files (one row per call)."""
import json, struct, sys


def f32(u):
    return struct.unpack('<f', struct.pack('<I', u & 0xffffffff))[0]


for p in sys.argv[1:]:
    d = json.load(open(p))
    print('#', p.replace('\\', '/').split('/')[-1], 'run', d.get('run'), 'err', d.get('err'))
    print('call atSeg segRet1 state rule course d360_raw d360 timer timer_raw tbl_floats(nonzero)')
    for i, r in enumerate(d.get('seed10', [])):
        tb = bytes.fromhex(r.get('tbl', ''))
        fl = []
        for o in range(0, len(tb) - 3, 4):
            u = struct.unpack_from('<I', tb, o)[0]
            v = f32(u)
            if u and 1e-3 < abs(v) < 1e4:
                fl.append('+%02x=%.7g(%08x)' % (o, v, u))
        print(i, r['atSeg'], r['segRet1'], r['state'], r['rule'], hex(r.get('course', -1)),
              '%08x' % r.get('d360raw', 0), '%.7g' % f32(r.get('d360raw', 0)), '%.7g' % r['timer'],
              '%08x' % r.get('timerRaw', 0), ' '.join(fl))
