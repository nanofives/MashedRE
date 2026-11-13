import struct
d = open(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched", "rb").read()
end = d.index(b"Rich", 0x80)
key = struct.unpack_from("<I", d, end + 4)[0]
i = end - 4
vals = []
while True:
    v = struct.unpack_from("<I", d, i)[0] ^ key
    if v == 0x536E6144:  # DanS
        break
    vals.append(v); i -= 4
vals.reverse()
vals = vals[3:]  # skip padding
for j in range(0, len(vals), 2):
    comp, cnt = vals[j], vals[j + 1]
    print(f"prodid 0x{comp >> 16:02x} build {comp & 0xffff:5d} count {cnt}")
