"""x87pc24_trinormal_model.py <values_off.json> <values_on.json>

PORT-X87-PC24 arm A4b scoring. Replays the ORIGINAL's 0x0046c5f0 instruction stream (MASHED.exe.unpatched
0x0046c5f0..0x0046c6c1, transcribed below with each instruction's address) on the inputs the exe recorded, at
x87 PC_24 (CW 0x007f, the original's) and at PC_53 (0x027f), and compares each model with the exe's knob-OFF and
knob-ON outputs call by call.

Arithmetic model. Every input is a float32 load (exact). PC_24: each FSUB / FMUL / FADD / FDIVR result is rounded
to a 24-bit significand, round to nearest even; all values here are normal floats, so that equals IEEE float32
arithmetic (numpy float32). PC_53: each result rounded to a 53-bit significand (float64), and FSTP / FST to a
dword rounds to float32. FUN_004c3b30 (0x0046c685) receives the dword stored at 0x0046c66f and returns the RW LUT
sqrt, reproduced from Math/RwLutTables.cpp (table fitted 4096/4096 to the original's live tables); FLD of a dword is
exact, so its ST0 result is the same at both PCs.
Register args (0x0046c5f3..0x0046c618): EAX = a, EDX and ECX the two other points, ESI = out. The probe records
the port's (apex, p1, p2). Both EDX / ECX assignments are scored and printed; the wrong one matches nothing.
"""
import json, struct, sys
import numpy as np

F32 = np.float32


def bits(f):
    return struct.unpack('<I', struct.pack('<f', float(f)))[0]


def fbits(b):
    return F32(struct.unpack('<f', struct.pack('<I', b & 0xffffffff))[0])


SQ = []
for i in range(0x1000):
    xb = ((0x7f if i & 0x800 else 0x80) << 23) | ((i & 0x7ff) << 12)
    s = np.sqrt(fbits(xb))   # float32 sqrt, correctly rounded
    SQ.append((bits(s) - (((xb + 0x800) >> 1) & 0x3fc00000)) & 0xffffffff)


def lut_sqrt(x32):
    b = bits(x32)
    if b == 0:
        return F32(0.0)
    bi = (b + 0x800) & 0xffffffff
    return fbits(SQ[(bi >> 12) & 0xfff] + ((bi >> 1) & 0x3fc00000))


def run(a, edx, ecx, pc24):
    R = (lambda v: F32(v)) if pc24 else (lambda v: np.float64(v))   # rounding of each arithmetic result
    L = lambda v: np.float64(F32(v)) if not pc24 else F32(v)          # dword load
    st = []                                                           # st[0] = ST0
    m = {}                                                            # [esp+k] dwords
    def fld(v): st.insert(0, v)
    def stp(k): m[k] = F32(st.pop(0))
    fld(L(a[0])); st[0] = R(st[0] - L(edx[0]))        # 0x46c5f3 / f5
    fld(L(a[1])); st[0] = R(st[0] - L(edx[1]))        # 0x46c5f7 / fa
    fld(L(a[2])); st[0] = R(st[0] - L(edx[2]))        # 0x46c5fd / 600
    fld(L(a[0])); st[0] = R(st[0] - L(ecx[0])); stp(0xc)   # 0x46c603..607
    fld(L(a[1])); st[0] = R(st[0] - L(ecx[1])); stp(0x8)   # 0x46c60b..611
    fld(L(a[2])); st[0] = R(st[0] - L(ecx[2]))        # 0x46c615 / 618
    fld(st[0])                                        # 0x46c61b fld st(0)
    st[0] = R(st[0] * st[3])                          # 0x46c61d fmul st(0),st(3)  (d8 cb)
    fld(L(m[0x8])); st[0] = R(st[0] * st[3])          # 0x46c61f / 623
    st[1] = R(st[1] - st[0]); st.pop(0)               # 0x46c625 fsubp st(1),st(0) (de e9)
    stp(0x0)                                          # 0x46c627
    fld(L(m[0xc])); st[0] = R(st[0] * st[2])          # 0x46c62b / 62f
    st[0], st[1] = st[1], st[0]                       # 0x46c631 fxch st(1)
    st[0] = R(st[0] * st[4])                          # 0x46c633 fmul st(0),st(4)  (d8 cc)
    st[1] = R(st[1] - st[0]); st.pop(0)               # 0x46c635
    stp(0x4)                                          # 0x46c637
    st.pop(0)                                         # 0x46c63b fstp st(0)
    fld(L(m[0x8])); st[0] = R(st[0] * st[2])          # 0x46c63d / 641
    fld(L(m[0xc])); st[0] = R(st[0] * st[2])          # 0x46c643 / 647
    st[1] = R(st[1] - st[0]); st.pop(0)               # 0x46c649
    stp(0x8)                                          # 0x46c64b
    st.pop(0); st.pop(0)                              # 0x46c64f / 651
    fld(L(m[0x8])); st[0] = R(st[0] * L(m[0x8]))      # 0x46c653 / 657
    fld(L(m[0x4])); st[0] = R(st[0] * L(m[0x4]))      # 0x46c65b / 65f
    st[1] = R(st[1] + st[0]); st.pop(0)               # 0x46c663 faddp
    fld(L(m[0x0])); st[0] = R(st[0] * L(m[0x0]))      # 0x46c665 / 669
    st[1] = R(st[1] + st[0]); st.pop(0)               # 0x46c66d faddp
    m[0xc] = F32(st[0])                               # 0x46c66f fst
    if st[0] < 0:                                     # 0x46c673 fcomp 0.0, jne (C0 = below) -> no write
        return None
    st.pop(0)
    s = lut_sqrt(m[0xc])                              # 0x46c680..685
    if s == 0:                                        # 0x46c68a fcom 0.0, jnp: equal -> no write
        return None
    inv = R(np.float64(1.0) / np.float64(s)) if not pc24 else F32(F32(1.0) / s)   # 0x46c69a fdivr [1.0]
    o0 = F32(R(L(m[0x0]) * inv))                      # 0x46c6a0..6a6
    o1 = F32(R(L(m[0x4]) * inv))                      # 0x46c6a8..6ae
    o2 = F32(R(inv * L(m[0x8])))                      # 0x46c6b1..6b5
    return o0, o1, o2


def unhex(h, n):
    return [fbits(struct.unpack('<I', bytes.fromhex(h[8 * i:8 * i + 8]))[0]) for i in range(n)]


def main():
    off = json.load(open(sys.argv[1]))['calls']['tri']
    on = json.load(open(sys.argv[2]))['calls']['tri']
    n = min(len(off), len(on))
    best = None
    for name, pick in (('EDX=p1,ECX=p2', lambda p: (p[1], p[2])), ('EDX=p2,ECX=p1', lambda p: (p[2], p[1]))):
        sc = {'O24=off': 0, 'O24=on': 0, 'O53=off': 0, 'O53=on': 0, 'nowrite': 0}
        for i in range(n):
            ins, o_off = off[i].split(':'); o_on = on[i].split(':')[1]
            pts = [unhex(ins[24 * k:24 * k + 24], 3) for k in range(3)]   # apex, p1, p2
            edx, ecx = pick(pts)
            r24 = run(pts[0], edx, ecx, True); r53 = run(pts[0], edx, ecx, False)
            if r24 is None:
                sc['nowrite'] += 1; continue
            h24 = ''.join(struct.pack('<f', float(v)).hex() for v in r24)
            h53 = ''.join(struct.pack('<f', float(v)).hex() for v in r53)
            sc['O24=off'] += h24 == o_off; sc['O24=on'] += h24 == o_on
            sc['O53=off'] += h53 == o_off; sc['O53=on'] += h53 == o_on
        print(name, 'calls', n, sc)
        if best is None or sc['O53=off'] + sc['O24=on'] > best[1]:
            best = (name, sc['O53=off'] + sc['O24=on'])
    print('assignment used (max O53=off + O24=on):', best[0])


if __name__ == '__main__':
    main()
