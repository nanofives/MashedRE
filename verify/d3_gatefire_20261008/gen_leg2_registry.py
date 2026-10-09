"""Emit the four leg-2 hooks_registry entries (cache_setter_observe) and splice them in after
'sub_state_roll_countdown_obs'. Read-only on the image; writes re/frida/hooks_registry.py once."""
import struct, pefile
pe = pefile.PE("original/MASHED.exe.unpatched"); ib = pe.OPTIONAL_HEADER.ImageBase
def fb(x): return struct.unpack('<I', struct.pack('<f', x))[0]
H = lambda a: '0x%08x' % a
ENT, ROW, OUT, CAM = 0x008964c0 + 20 * 0xd8, 0x008964c0 + 21 * 0xd8, 0x008964c0 + 22 * 0xd8, 0x00897fe0
def seed(a, v): return {'addr': H(a), 'val': v}
T = 0x5f8d50
rows = [struct.unpack('<12I', pe.get_data(T + 4 + 0x30 * k - ib, 0x30)) for k in range(5)]
# --- 0x00441b30 ---
v441b30 = []
for r in rows:
    s = [seed(ENT + 4 * i, 0xAAAAAAAA) for i in range(0x36)] + [seed(ROW + 4 * i, r[i]) for i in range(12)]
    v441b30.append({'seed': s, 'args': [ENT, ROW], 'obs': [H(ENT + 4 * i) for i in range(0x36)]})
# --- 0x00442600 ---
F = (0, 4, 8, 0x3c, 0x40, 0x44, 0xa4, 0xac, 0xb0, 0xb4, 0xc4, 0xcc, 0xd0, 0xd4)
v442600 = []
for trk, n in ((0x1e, 7), (0x02, 8), (0x1a, 15), (0x25, 17), (0x00, 10)):
    s = [seed(0x0089898c, 0), seed(0x00644158, trk), seed(0x00639d78, 0)]
    obs = [H(a) for a in (0x00898994, 0x0089898c, 0x008964a0, 0x0089649c, 0x008964a4, 0x0089899c, 0x00898998)]
    obs += [H(0x008964c0 + k * 0xd8 + o) for k in range(n) for o in F]
    v442600.append({'seed': s, 'args': [], 'obs': obs})
# --- 0x00441c80 ---
CARP = lambda k: 0x00881ef8 + k * 0xd04
cars = [(1.0, 2.0, 3.0), (-4.0, 0.5, 9.0), (7.25, 1.5, -2.0), (0.125, 3.0, 11.0)]
def carseed():
    s = []
    for k, p in enumerate(cars):
        s += [seed(0x00881f48 + k * 0xd04, 0)] + [seed(CARP(k) + 4 * i, fb(p[i])) for i in range(3)]
    return s
v441c80 = []
for c7, c8 in ((1, 0), (1, 1), (1, 2), (1, 3), (0, 0)):
    s = carseed() + [seed(CAM + 0x1c, c7), seed(CAM + 0x20, c8)] + [seed(OUT + 4 * i, 0xAAAAAAAA) for i in range(3)]
    v441c80.append({'seed': s, 'args': [OUT, CAM], 'obs': [H(OUT + 4 * i) for i in range(3)]})
# --- 0x00445aa0 ---
tgt = (-0.15, 3.54, 4.46)
def ent(pos, vel, a8=0):
    d = {0x3c: pos[0], 0x40: pos[1], 0x44: pos[2], 0x8c: vel[0], 0x90: vel[1], 0x94: vel[2], 0xa4: 12.0,
         0xac: 5.0, 0xb0: 180.0, 0xb4: -15.0, 0xb8: 0.0, 0xbc: 0.0, 0xc0: 1.0}
    s = [seed(ENT + o, fb(v)) for o, v in d.items()] + [seed(ENT + 0xa8, a8), seed(ENT + 0xc8, 0xAAAAAAAA)]
    s += [seed(ENT + 0x4c + 4 * i, 0xAAAAAAAA) for i in range(16)]
    return s
def camseed(flag, t=tgt, c7=1, c8=0):
    return [seed(CAM, flag)] + [seed(CAM + 4 + 4 * i, fb(t[i])) for i in range(3)] + \
           [seed(CAM + 0x10 + 4 * i, fb((0.5, 2.0, 6.0)[i])) for i in range(3)] + [seed(CAM + 0x1c, c7), seed(CAM + 0x20, c8)]
def common(inp0=0):
    return [seed(0x007f1008, 50), seed(0x007f101c, 700)] + \
           [seed(0x007f1042 + k * 0x4c, (inp0 if k == 0 else 0)) for k in range(8)] + carseed()
OBS = [H(ENT + o) for o in (0x3c, 0x40, 0x44, 0x8c, 0x90, 0x94, 0xa4, 0xa8, 0xac, 0xb0, 0xb4, 0xb8, 0xbc, 0xc0, 0xc8)]
# +0x68/+0x78/+0x88 are RwMatrix pad words: RwMatrixRotate's postconcat copies them from an
# uninitialised stack temp (orig 3/3/6 vs stack junk on the reimpl side, same callee): NOT observed.
OBS += [H(ENT + 0x4c + 4 * i) for i in range(16) if (0x4c + 4 * i) not in (0x68, 0x78, 0x88)] + [H(CAM)]
v445aa0 = [
    {'seed': ent((1.47, 9.7, 32.1), (0.0, -1.0, -3.0)) + camseed(1) + common(), 'args': [ENT, CAM], 'obs': OBS},
    {'seed': ent((tgt[0] + 0.01, 3.6, tgt[2] + 0.01), (0.0, 0.0, -0.05)) + camseed(1) + common(), 'args': [ENT, CAM], 'obs': OBS},
    {'seed': ent((tgt[0], 4.0, tgt[2] + 3.0), (0.2, 0.0, -1.0)) + camseed(1) + common(), 'args': [ENT, CAM], 'obs': OBS},
    {'seed': ent((cars[0][0] + 3.0, 2.0, cars[0][2] + 4.0), (0.0, 0.0, 0.0)) + camseed(0) + common(), 'args': [ENT, CAM], 'obs': OBS},
    {'seed': ent((1.47, 9.7, 32.1), (0.0, 0.0, -2.0)) + camseed(1) + common(inp0=1), 'args': [ENT, CAM], 'obs': OBS},
]
def entry(key, rva, export, sig_args, tests, comment):
    return ("\n    # %s\n    '%s': {\n        'rva':            0x%08x,\n        'export':         '%s',\n"
            "        'signature':      {'ret': 'void', 'args': %r},\n        'arg_type':       'cache_setter_observe',\n"
            "        'lut_root_delta': 0,\n        'path1_tests':    %r,\n        'path2_tests':    [],\n    },\n"
            % (comment, key, rva, export, sig_args, tests))
txt = ""
txt += entry('cam_entry_build_obs', 0x00441b30, 'CamEntryBuild441b30', ['uint32', 'uint32'], v441b30,
             "0x00441b30 (PREREG_LEG2.md G-PATH): Training table rows 0..4 into scratch entry 20; every entry dword sentinel-seeded and observed.")
txt += entry('cam_car_midpoint_obs', 0x00441c80, 'CamCarMidpoint441c80', ['uint32', 'uint32'], v441c80,
             "0x00441c80: four seeded car positions (set index 0); cam[7]=1 pins the pair to cam[8]; the last vector runs FUN_0040e180's live pick on both sides.")
txt += entry('cam_entries_init_obs', 0x00442600, 'CamEntriesInit442600', [], v442600,
             "0x00442600: non-repeatable body (guard 0x0089898c reseeded to 0 per vector); five track ids. 0x00897fc0 (one ring draw, not restorable between sides) is NOT observed: named coverage gap.")
txt += entry('cam_type0_step_obs', 0x00445aa0, 'CamEntryType0Step445aa0', ['uint32', 'uint32'], v445aa0,
             "0x00445aa0: far approach / site-(a) release / near (blend + lim) / flag-0 car-midpoint arm (sets +0xa8, LAB same frame) / site-(b) input byte.")
p = "re/frida/hooks_registry.py"; s = open(p, encoding='utf-8', newline='').read()
nl = '\r\n' if '\r\n' in s else '\n'
anchor = "    'vehicle_physics_state_reset_obs': {"
assert s.count(anchor) == 1 and "'cam_type0_step_obs'" not in s
s = s.replace(anchor, txt.replace('\n', nl).lstrip(nl) + nl + anchor)
open(p, 'w', encoding='utf-8', newline='').write(s)
print("spliced 4 entries")
