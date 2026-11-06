import numpy as np
M=0xffffffff
r=[0x9a319039]
for i in range(1,31): r.append((r[-1]*0x41c64e6d+0x3039)&M)
p,q=3,0
def nxt():
    global p,q
    r[p]=(r[p]+r[q])&M; u=r[p]>>1; p+=1
    if p>=31: p=0; q+=1; return u
    q+=1
    if q>=31: q=0
    return u
for _ in range(0x136): nxt()
f32=np.float32
def cd(u):
    v=f32(4.0)*f32(u&0x7fffffff)*f32(4.656612873e-10)+f32(1.0)
    return int(v)
seq=[cd(nxt()) for _ in range(3000)]
print('K=0 draws',seq[:4])
hits=[k for k in range(len(seq)-3) if seq[k:k+4]==[3,3,3,3]]
print('first K with 3,3,3,3:',hits[:10])
