import json, sys, math
import numpy as np
f32 = np.float32
fx = json.load(open(sys.argv[1]))
a = [x for x in fx if x.get("n")][len(fx)//2]
L = [f32(v) for v in a["light_ltm"]]
lup = L[4:7]; lat = L[8:11]; lpos = L[12:15]; lr = L[0:3]
def dot(p, q): return f32(f32(f32(p[2]*q[2]) + f32(p[0]*q[0])) + f32(p[1]*q[1]))
def cl(v): return f32(-1) if v < -1 else (f32(1) if v > 1 else v)
def norm(v):
    l = f32(math.sqrt(f32(f32(f32(v[2]*v[2]) + f32(v[1]*v[1])) + f32(v[0]*v[0]))))
    return [f32(v[0]/l), f32(v[1]/l), f32(v[2]/l)]
def inv_ortho(m):  # rows r,u,a,p
    r, u, at, p = m[0:3], m[4:7], m[8:11], m[12:15]
    R = [[r[0], u[0], at[0]], [r[1], u[1], at[1]], [r[2], u[2], at[2]]]
    pos = [f32(-(p[0]*R[0][k] + p[1]*R[1][k] + p[2]*R[2][k])) for k in range(3)]
    return R, pos
def xform(v, m):  # row vector * rows
    return [f32(v[0]*m[0+k] + v[1]*m[4+k] + v[2]*m[8+k] + m[12+k]) for k in range(3)]
def tri(O, D, V0, V1, V2):
    e1 = [f32(V1[k]-V0[k]) for k in range(3)]; e2 = [f32(V2[k]-V0[k]) for k in range(3)]
    p = [f32(D[1]*e2[2]-D[2]*e2[1]), f32(D[2]*e2[0]-e2[2]*D[0]), f32(e2[1]*D[0]-D[1]*e2[0])]
    det = f32(f32(f32(p[2]*e1[2]) + f32(p[0]*e1[0])) + f32(p[1]*e1[1]))
    if not det > f32(1e-8): return None
    f15 = f32(det * f32(-9.999999747378752e-06)); f7 = f32(det - f15)
    s = [f32(O[k]-V0[k]) for k in range(3)]
    u = f32(f32(f32(s[2]*p[2]) + f32(s[0]*p[0])) + f32(s[1]*p[1]))
    if u < f15 or u > f7: return None
    q = [f32(s[1]*e1[2]-s[2]*e1[1]), f32(s[2]*e1[0]-s[0]*e1[2]), f32(s[0]*e1[1]-s[1]*e1[0])]
    v = f32(f32(f32(D[2]*q[2]) + f32(q[0]*D[0])) + f32(D[1]*q[1]))
    if not (v >= f15 and f32(u+v) <= f7): return None
    t = f32(f32(f32(q[2]*e2[2]) + f32(q[0]*e2[0])) + f32(q[1]*e2[1]))
    if not (t >= f15 and t <= f7): return None
    return f32(t/det)
for o in a["objs"]:
    if o["st"] not in (1, 2) or not o["flag"]: continue
    M = [f32(v) for v in o["ltm"]]; start = M[12:15]
    dl = [f32(f32(2.0)*lat[k]) for k in range(3)]
    end = [f32(dl[k]+start[k]) for k in range(3)]
    D = dl
    V = o["cache_v"]; n = [f32(v) for v in o["cache"][0:3]]
    t = tri(start, D, V[0], V[1], V[2])
    hit = [f32(f32(f32(n[0]*f32(0.02)) + f32(f32(end[0]-start[0])*t)) + start[0]),
           f32(f32(f32(f32(n[1]*f32(0.02)) + f32(0.01)) + f32(f32(end[1]-start[1])*t)) + start[1]),
           f32(f32(f32(n[2]*f32(0.02)) + f32(f32(end[2]-start[2])*t)) + start[2])]
    # FUN_0045c030
    e = [f32(V[0][k]-V[1][k]) for k in range(3)]
    right = norm(e)
    c = [f32(e[2]*n[1]-e[1]*n[2]), f32(e[0]*n[2]-e[2]*n[0]), f32(e[1]*n[0]-e[0]*n[1])]
    up = norm(c); at = n
    P = [f32(lup[k]+hit[k]) for k in range(3)]
    w = [f32(V[0][k]-P[k]) for k in range(3)]
    r1 = cl(dot(n, w)); r2 = cl(dot(n, lat))
    if r2 == 0: r2 = f32(0.01)
    ratio = f32(r1/r2)
    Q = [f32(f32(lat[0]*ratio)+P[0]), f32(f32(ratio*lat[1])+P[1]), f32(f32(ratio*lat[2])+P[2])]
    Dv = [f32(Q[k]-hit[k]) for k in range(3)]
    right = norm(Dv)
    up = [f32(at[1]*right[2]-right[1]*at[2]), f32(at[2]*right[0]-at[0]*right[2]), f32(at[0]*right[1]-at[1]*right[0])]
    b = o["bbox"]
    corners = [(b[3],b[4],b[5]),(b[0],b[4],b[5]),(b[0],b[1],b[5]),(b[3],b[1],b[5]),(b[3],b[4],b[2]),(b[0],b[4],b[2]),(b[0],b[1],b[2]),(b[3],b[1],b[2])]
    R, ip = inv_ortho(L)
    xs, ys = [], []
    for cc in corners:
        wv = xform([f32(x) for x in cc], M)
        lv = [f32(wv[0]*R[0][k] + wv[1]*R[1][k] + wv[2]*R[2][k] + ip[k]) for k in range(3)]
        xs.append(lv[0]); ys.append(lv[1])
    spy = f32(max(ys) - min(ys)); spx = f32(max(xs)-min(xs))
    cosv = f32(f32(f32(lat[2]*right[2]) + f32(lat[0]*right[0])) + f32(lat[1]*right[1]))
    tn = f32(math.tan(math.acos(float(cosv))))
    sx = f32(spy/tn); sy = f32(spx*f32(1.4))
    rs = [f32(right[k]*sx) for k in range(3)]; us = [f32(up[k]*sy) for k in range(3)]
    k1 = f32(sx*f32(0.1))
    pos = [f32(hit[k] + f32(k1*rs[k])) for k in range(3)]
    m = o["shm"]
    print(f"obj {o['i']}: t={t}")
    print("  port r", [round(float(x),5) for x in rs], "u", [round(float(x),5) for x in us], "a", [round(float(x),5) for x in at], "p", [round(float(x),5) for x in pos])
    print("  orig r", [round(x,5) for x in m[0:3]], "u", [round(x,5) for x in m[4:7]], "a", [round(x,5) for x in m[8:11]], "p", [round(x,5) for x in m[12:15]])
print("batch:", [ (round(s["m"][12],4), round(s["m"][13],4), round(s["m"][14],4), hex(s["c"])) for s in a["shad"]])
