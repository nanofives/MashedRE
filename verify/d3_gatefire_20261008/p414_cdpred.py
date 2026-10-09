import numpy as np
M=0xffffffff
ring=[0x9a319039]
for i in range(1,31): ring.append((ring[-1]*0x41c64e6d+0x3039)&M)
p,q=3,0
def nxt():
    global p,q
    ring[p]=(ring[p]+ring[q])&M; u=ring[p]; p+=1; u>>=1
    if p>=31: p=0; q+=1; return u
    q+=1
    if q>=31: q=0
    return u
for _ in range(0x136): nxt()
f=np.float32
k=f(4.656612873e-10)
out=[]
for n in range(16):
    u=nxt()
    # port law: float arithmetic
    port=f(f(f(4)*f(u&0x7fffffff))*k)+f(1)
    exact=1+4*(u&0x7fffffff)*2**-31   # x87 extended approx
    out.append((n,hex(u),float(port),int(port),exact,int(exact)))
for o in out: print(o)
