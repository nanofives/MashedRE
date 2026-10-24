# audiolink: direct call/jmp rel32 sites to each VA in .text, plus any dword equal to the VA anywhere in the image
# (function-pointer tables). Usage: caller_scan.py VA [VA ...]
import struct, sys
d = open(r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched', 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]; n = struct.unpack_from('<H', d, pe+6)[0]; osz = struct.unpack_from('<H', d, pe+20)[0]
base = struct.unpack_from('<I', d, pe+52)[0]; s0 = pe+24+osz
secs = [(d[s0+i*40:s0+i*40+8].rstrip(b'\0').decode(),) + struct.unpack_from('<IIII', d, s0+i*40+8) for i in range(n)]
_, tvs, tva, trs, tro = secs[0]
for arg in sys.argv[1:]:
    t = int(arg, 16); calls = []
    for i in range(tro, tro + trs - 5):
        if d[i] in (0xe8, 0xe9):
            src = base + tva + (i - tro)
            if src + 5 + struct.unpack_from('<i', d, i+1)[0] == t: calls.append(f'{"call" if d[i] == 0xe8 else "jmp"}@{src:08x}')
    ptrs = []
    pat = struct.pack('<I', t)
    for nm, vs, va, rs, ro in secs:
        j = ro
        while True:
            j = d.find(pat, j, ro + rs)
            if j < 0: break
            ptrs.append(f'{nm}@{base + va + j - ro:08x}'); j += 1
    print(f'{t:#010x}: {", ".join(calls) or "no direct calls"} | ptr refs: {", ".join(ptrs) or "none"}')
