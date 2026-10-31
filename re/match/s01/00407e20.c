/* 0x00407e20 (1895 B). Best 76.6%, residual (b). Found: one V3 local reused as the subtract vector and the normalize input, nn / corner copies as scalars (frame 0x34 now matches), db0 takes a third unused arg (n). Open: (1) the original stores d.z and reloads it for the dot product (fstp + fld/fmul), ours keeps it in st0 (inline Dot/Sub helpers and operand orders did not change it); (2) loop1 negates prev (fchs) inside the loop and the original keeps idx as the cursor home (writing it that way moved the else block to the end, 72-73%). */
// FLAGS: /O2 /GS /Ob1
typedef struct { float x, y, z; } V3;
struct TrackPos {
    int idx;        /* 0x00 */
    float t;        /* 0x04 */
    float frac;     /* 0x08 */
    int code;       /* 0x0c */
    int f10;        /* 0x10 */
    float dist;     /* 0x14 */
    float d18;      /* 0x18 */
    float d1c;      /* 0x1c */
    float d20;      /* 0x20 */
    float d24;      /* 0x24 */
};
V3 *FUN_00426cb0(int i);
V3 *FUN_00426cc0(int i);
V3 *FUN_00426d00(int i, int k);
int FUN_00407be0(int v, int start);
int FUN_00407cd0(int v, int start);
int FUN_00407d70(int i);
int FUN_00407db0(int i, int v, int n);
float FUN_004c39b0(V3 *out, V3 *in);
int FUN_004a2cbd(const char *fmt, ...);

// MATCH: 0x00407e20 FUN_00407e20
void FUN_00407e20(int unused, int idx, int grp, V3 *pos, struct TrackPos *out, float lim)
{
    V3 *p, *n, *q;
    V3 d, nrm;
    float nx, ny, nz, cx, cy, cz;
    float dot, prev, a, t, dist, s;
    float ax, az, bx, bz;
    int cur, nxt, nxt2, found, cnt, code, total, off;

    p = FUN_00426cb0(idx);
    n = FUN_00426cc0(idx);
    d.x = pos->x - p->x;
    d.y = pos->y - p->y;
    d.z = pos->z - p->z;
    dot = n->z * d.z + n->y * d.y + d.x * n->x;
    if (dot <= 0.0f) {
        if (idx == 0 && grp == 0 && FUN_00407be0(0, 0) == -10) {
            cur = 0;
            t = 0.0f;
            out->d20 = 0.0f;
            out->d24 = 0.0f;
            out->d18 = 0.0f;
            out->d1c = 0.0f;
            code = 0;
            dist = 0.0f;
            goto done;
        }
        cur = idx;
        found = 0;
        cnt = 0;
        do {
            prev = dot;
            cur = FUN_00407be0(cur, grp);
            p = FUN_00426cb0(cur);
            n = FUN_00426cc0(cur);
            d.x = pos->x - p->x;
            d.y = pos->y - p->y;
            d.z = pos->z - p->z;
            dot = n->z * d.z + n->y * d.y + d.x * n->x;
            if (dot >= 0.0f)
                found = 1;
            cnt++;
        } while (cnt <= 10 && !found);
    } else {
        found = 0;
        cnt = 0;
        cur = FUN_00407be0(idx, grp);
        if (cur == -10) {
            cur = idx;
            goto body;
        }
        while (!found) {
            cur = FUN_00407cd0(cur, grp);
        body:
            nxt = FUN_00407cd0(cur, grp);
            if (nxt >= 0x200)
                FUN_004a2cbd("i is too big!!\n");
            p = FUN_00426cb0(nxt);
            n = FUN_00426cc0(nxt);
            d.x = pos->x - p->x;
            d.y = pos->y - p->y;
            d.z = pos->z - p->z;
            prev = n->z * d.z + n->y * d.y + d.x * n->x;
            if (prev <= 0.0f)
                found = 1;
            else
                dot = -prev;
            cnt++;
            if (cnt > 10)
                found = 1;
        }
    }
    a = -prev;
    if (dot < 0.0f)
        dot = -dot;
    if (a < 0.0f)
        a = -a;
    t = dot / (a + dot) + cur;
    if (t >= FUN_00407d70(grp))
        t = t - FUN_00407d70(grp);
    dist = 0.0f;
    code = 1;
    nxt = FUN_00407cd0(cur, grp);
    nxt2 = FUN_00407cd0(nxt, grp);

    p = FUN_00426cb0(cur);
    ax = p->x;
    az = p->z;
    p = FUN_00426cb0(nxt);
    bx = p->x;
    bz = p->z;
    d.z = bx - ax;
    d.y = 0.0f;
    d.x = -(bz - az);
    FUN_004c39b0(&nrm, &d);
    d.x = pos->x - ax;
    d.z = pos->z - az;
    out->d20 = nrm.z * d.z + nrm.x * d.x;

    FUN_00426cb0(nxt);
    p = FUN_00426cb0(nxt2);
    ax = p->x;
    az = p->z;
    d.z = bx - ax;
    d.y = 0.0f;
    d.x = -(bz - az);
    FUN_004c39b0(&nrm, &d);
    d.x = pos->x - ax;
    d.z = pos->z - az;
    out->d24 = nrm.z * d.z + nrm.x * d.x;

    p = FUN_00426d00(cur, 2);
    ax = p->x;
    az = p->z;
    p = FUN_00426d00(nxt, 2);
    d.z = p->x - ax;
    d.y = 0.0f;
    d.x = -(p->z - az);
    FUN_004c39b0(&nrm, &d);
    d.x = pos->x - ax;
    d.z = pos->z - az;
    out->d18 = nrm.z * d.z + nrm.x * d.x;
    if (out->d18 < lim) {
        dist = out->d18 - lim;
        code = 2;
    }

    p = FUN_00426d00(cur, 0);
    ax = p->x;
    az = p->z;
    p = FUN_00426d00(nxt, 0);
    d.z = p->x - ax;
    d.x = -(p->z - az);
    FUN_004c39b0(&nrm, &d);
    d.x = ax - pos->x;
    d.z = az - pos->z;
    out->d1c = nrm.z * d.z + nrm.x * d.x;
    if (out->d1c < lim) {
        dist = out->d1c - lim;
        code = 3;
    }

    q = FUN_00426cc0(cur);
    nx = q->x;
    ny = q->y;
    nz = q->z;
    q = FUN_00426d00(cur, 0);
    cx = q->x;
    cy = q->y;
    cz = q->z;
    q = FUN_00426d00(cur, 3);
    d.x = q->x - cx;
    d.y = q->y - cy;
    d.z = q->z - cz;
    nrm.x = ny * d.z - nz * d.y;
    nrm.y = nz * d.x - nx * d.z;
    nrm.z = nx * d.y - ny * d.x;
    d.x = pos->x - cx;
    d.y = pos->y - cy;
    d.z = pos->z - cz;
    s = nrm.y * d.y + nrm.z * d.z + nrm.x * d.x;
    if (s < -10.0f && s < dist) {
        dist = s;
        code = 5;
    }

    q = FUN_00426d00(cur, 2);
    cx = q->x;
    cy = q->y;
    cz = q->z;
    q = FUN_00426d00(cur, 1);
    d.x = q->x - cx;
    d.y = q->y - cy;
    d.z = q->z - cz;
    nrm.x = ny * d.z - nz * d.y;
    nrm.y = nz * d.x - nx * d.z;
    nrm.z = nx * d.y - ny * d.x;
    d.x = pos->x - cx;
    d.y = pos->y - cy;
    d.z = pos->z - cz;
    s = nrm.y * d.y + nrm.z * d.z + nrm.x * d.x;
    if (s < 0.0f && s < dist) {
        code = 4;
        dist = s;
    }
done:
    out->dist = dist;
    out->idx = cur;
    out->code = code;
    out->t = t;
    total = FUN_00407d70(grp);
    off = FUN_00407db0(grp, cur, total);
    out->frac = ((out->t - (int)out->t) + off) / total * 100.0f;
}
