#!/usr/bin/env python3
"""Mode 7 M3: summarize a scenario_launch.py --cb-probe capture (<out>.cb.tsv) or an exe trace in the same format.

PK rows: each FUN_00458920 call (arm, car, pickup, grant, witnesses).
S11 rows: FUN_00486630 calls: gate d24 <= _DAT_005cc9bc (read from the anchored .rdata), whether the
car record +0x144..+0x14c changed, ring cursor movement.
ST rows: pickup-pool state transitions with frames / summed timer step since the entry's last transition.
Usage: py -3.12 re/tools/pu_cb_report.py <cb.tsv> [more ...]
"""
import collections, struct, sys
import pefile

pe = pefile.PE("original/MASHED.exe.unpatched")
C9BC = struct.unpack("<f", pe.get_data(0x005cc9bc - 0x400000, 4))[0]
f32 = lambda h: struct.unpack("<f", struct.pack("<I", int(h, 16)))[0]

def ent(h):
    b = bytes.fromhex(h)
    return dict(t18=struct.unpack_from("<f", b, 0x18)[0], t1c=struct.unpack_from("<f", b, 0x1c)[0],
                st=struct.unpack_from("<i", b, 0x20)[0], ty=struct.unpack_from("<i", b, 0x24)[0],
                v44=["%08x" % x for x in struct.unpack_from("<3I", b, 0x44)])

def summary(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip() and not l.startswith("#")]
    print(f"== {path}: {len(rows)} rows; _DAT_005cc9bc = {C9BC!r}")
    for r in rows:
        if r[0] != "PK": continue
        ei, eo = ent(r[17]), ent(r[20])
        w = r[24] if len(r) > 24 else ""
        arm = "collect" if "c010" in w else "bump" if "8570" in w else ("vanish" if r[17] != r[20] else "no-op")
        print(f"  PK n={r[1]} frame={r[2]} slots a={r[3]} b={r[4]} type_a={r[5]} type_b={r[6]} fd0={r[7]} ba8c={r[8]} "
              f"car+0xa8 {r[9] or '-'} -> {r[22] if len(r) > 22 else '?'}; a5d0/4={r[11]}/{r[12]} 9dc4={r[13]} free_particles={r[14]} "
              f"ring {r[15]} -> {r[21] if len(r) > 21 else '?'} pool_idx={r[16]}")
        print(f"     arm={arm} entry in st={ei['st']} t18={ei['t18']} t1c={ei['t1c']:.6g} ty={ei['ty']}  out st={eo['st']} "
              f"t1c={eo['t1c']:.6g} v44={eo['v44']}  witnesses=[{w}]")
    s11 = [r for r in rows if r[0] == "S11"]
    if s11:
        gate = sum(1 for r in s11 if r[6] and f32(r[6]) <= C9BC)
        changed = sum(1 for r in s11 if len(r) > 13 and r[8] != r[13])
        ringmv = sum(1 for r in s11 if len(r) > 14 and r[11] != r[14])
        by = collections.Counter((r[3], r[4], r[5]) for r in s11)
        d24 = collections.Counter(r[6] for r in s11)
        d78 = collections.Counter(r[7] for r in s11)
        print(f"  S11 calls={len(s11)} gate(d24<=c9bc)={gate} rec+0x144 changed={changed} ring moved={ringmv}")
        print(f"     by (a,b,type_b): {dict(by.most_common(6))}")
        print(f"     d24 values: {[(k, f32(k) if k else None, n) for k, n in d24.most_common(5)]}  d78: {dict(d78.most_common(4))}")
        print(f"     frames {s11[0][2]}..{s11[-1][2]}")
    st = [r for r in rows if r[0] == "ST"]
    for r in st:
        print(f"  ST frame={r[2]} idx={r[3]} {r[4]}->{r[5]} t18={f32(r[6]):.6g} t1c={f32(r[7]):.6g} type={r[8]} "
              f"frames_since={r[9]} dtsum_since={r[10]}")


# ---- G-M3-LIVE relation checks (PREREG_M3.md section 3), exe trace ----
def u32b(b, o): return struct.unpack_from("<I", b, o)[0]

def respawn_ticks(t18_bits, dt_bits=0x3c888888):
    import numpy as np
    t = np.uint32(t18_bits).view(np.float32); dt = np.uint32(dt_bits).view(np.float32); k = 0
    while not (t <= np.float32(0)):
        t = np.float32(t - dt); k += 1
        if k > 10**6: return -1
    return k

def check(path):
    rows = [l.rstrip("\n").split("\t") for l in open(path) if l.strip() and not l.startswith("#")]
    pk = [r for r in rows if r[0] == "PK"]; st = [r for r in rows if r[0] == "ST"]
    pf = [r for r in rows if r[0] == "PF"]; cnt = [r for r in rows if r[0] == "CNT"]
    fails = collections.Counter(); arms = collections.Counter()
    one = struct.unpack("<I", struct.pack("<f", 1.0))[0]
    for r in pk:
        ei, eo, w = bytes.fromhex(r[17]), bytes.fromhex(r[20]), r[24]
        st_in, st_out = struct.unpack_from("<i", ei, 0x20)[0], struct.unpack_from("<i", eo, 0x20)[0]
        if "c010" in w:
            arms["collect"] += 1
            ty = struct.unpack_from("<i", ei, 0x24)[0]
            g = [x for x in w.split("|") if x.startswith("c010:")][0].split(":")
            ok = (st_in == 1 and st_out == 2 and u32b(eo, 0x1c) == u32b(ei, 0x18) and int(g[2]) == ty
                  and r[22] != "" and int(r[22]) == ty and int(g[1]) == int(r[6], 16) - 0xff00)
            if not ok: fails["i_collect"] += 1
        elif "8570" in w:
            arms["bump"] += 1
            ok = (st_out == 1 and eo[0x44:0x50] == struct.pack("<3I", 0, 0x3d23d70a, 0) and eo[0x1c:0x20] == ei[0x1c:0x20])
            if not ok: fails["ii_bump"] += 1
        elif ei != eo:
            arms["vanish"] += 1
            exp = struct.unpack("<I", struct.pack("<f", struct.unpack("<f", ei[0x18:0x1c])[0] - 1.0))[0]
            if not (st_out == 2 and u32b(eo, 0x1c) == exp): fails["iii_vanish"] += 1
        else:
            arms["no-op"] += 1
    resp = []
    for r in st:
        if r[4] == "2" and r[5] == "1":
            k = respawn_ticks(int(r[6], 16))
            resp.append((int(r[2]), int(r[3]), int(r[9]), k))
            if int(r[9]) != k: fails["iv_respawn"] += 1
    last = cnt[-1] if cnt else None
    unported = int(last[8]) if last and len(last) > 8 else -1
    if unported != 0: fails["v_unported"] += 1
    print(f"== G-M3-LIVE check {path}")
    for r in rows:
        if r[0] == "SEED": print(f"  SEED tick={r[2]} entries={r[3]} t18:type = {r[4:]}")
    print(f"  PK calls={len(pk)} arms={dict(arms)} ST transitions={len(st)} respawns={len(resp)} PF rows={len(pf)}")
    if last: print(f"  last CNT: tick={last[2]} grants={last[3]} bursts={last[4]} sfx={last[5]} pf_collects={last[6]} callbacks={last[7]} unported={unported}")
    print(f"  respawn (tick, idx, frames_since, predicted): {resp[:12]}")
    print(f"  relation failures: {dict(fails) or 'none'}")
    for r in pk[:40]:
        w = r[24]
        arm = "collect" if "c010" in w else "bump" if "8570" in w else ("vanish" if r[17] != r[20] else "no-op")
        print(f"    PK tick={r[2]} car={int(r[6],16)-0xff00 if int(r[6],16)>=0xff00 else r[6]} pool={r[16]} type={struct.unpack_from('<i', bytes.fromhex(r[17]), 0x24)[0]} arm={arm} held_before={r[9]} after={r[22]}")
    for r in pf[:20]:
        print(f"    PF tick={r[2]} car={r[3]} type={r[4]}")

if __name__ == "__main__":
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    for path in args:
        if "--check" in sys.argv: check(path)
        else: summary(path)
