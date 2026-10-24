# audiolink slice 7 arm T: read the original's 0x00633b48 table (512 B) and 0x005d757c (4 B) at the menu, compare with
# the file. Spawns its own MASHED (explicit PID, killed at the end; sandbox must be disabled).
import os, struct, subprocess, sys, time, frida
ORIG = r'C:/Users/maria/Desktop/Proyectos/Mashed/original'
d = open(os.path.join(ORIG, 'MASHED.exe.unpatched'), 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]; n = struct.unpack_from('<H', d, pe+6)[0]; osz = struct.unpack_from('<H', d, pe+20)[0]
base = struct.unpack_from('<I', d, pe+52)[0]; s0 = pe+24+osz
def fbytes(v, ln):
    r = v - base
    for i in range(n):
        vs, va, rs, ro = struct.unpack_from('<IIII', d, s0+i*40+8)
        if va <= r < va + rs: return d[ro + r - va: ro + r - va + ln]
env = {**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1', 'MASHED_MUTE': '1'}
p = subprocess.Popen([os.path.join(ORIG, 'MASHED.exe')], cwd=ORIG, env=env, creationflags=0x00000200 | 0x00000008)
print('pid', p.pid)
try:
    time.sleep(20)
    s = frida.get_local_device().attach(p.pid)
    sc = s.create_script("rpc.exports.rd = (a, l) => Array.from(new Uint8Array(ptr(a).readByteArray(l)));")
    sc.load()
    ok = True
    for va, ln in ((0x00633b48, 512), (0x005d757c, 4)):
        live = bytes(sc.exports_sync.rd(va, ln)); f = fbytes(va, ln)
        eq = live == f; ok &= eq
        diff = [i for i in range(ln) if live[i] != f[i]][:8]
        print(f'{va:#010x} len {ln}: {"EQUAL" if eq else "DIFFERENT"}' + ('' if eq else f' first diffs at {diff}'))
    s.detach()
    print('T', 'HOLDS' if ok else 'FAILS')
finally:
    p.kill(); print('killed', p.pid)
