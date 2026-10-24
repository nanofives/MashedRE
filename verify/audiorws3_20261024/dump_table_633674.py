import struct
from pathlib import Path
d=Path(r"C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched").read_bytes()
pe=struct.unpack_from("<I",d,0x3C)[0]; n=struct.unpack_from("<H",d,pe+6)[0]; osz=struct.unpack_from("<H",d,pe+20)[0]
base=struct.unpack_from("<I",d,pe+24+28)[0]; s0=pe+24+osz; secs=[]
for i in range(n):
    s=s0+i*40; name=d[s:s+8].rstrip(b'\0').decode(); vsz,va,rsz,ro=struct.unpack_from("<IIII",d,s+8); secs.append((name,va,vsz,rsz,ro))
def off(v):
    r=v-base
    for nm,va,vsz,rsz,ro in secs:
        if va<=r<va+max(vsz,rsz): return (ro+(r-va)) if r-va<rsz else None, nm
for nm,va,vsz,rsz,ro in secs: print(nm,hex(va+base),hex(vsz),hex(rsz))
o,nm=off(0x633674); print("table in",nm)
ptrs=struct.unpack_from("<9I",d,o)
for i,p in enumerate(ptrs):
    po,pn=off(p); print(i,hex(p),pn, d[po:po+16].hex() if po else "BSS")
print("around:", d[o-16:o+48].hex())
