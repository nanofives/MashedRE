# orig_law_check.py <orig_frame.json>
# Per text call of a recheck_frame_probe.py capture:
#  (a) camera projection of each submitted glyph vertex: p = local*ltm, p' = p*view, (p'.x/p'.z*W, p'.y/p'.z*H)
#  (b) byte-law prediction with NO camera: n = pen + s'*(local - (0, c8)), s' = scale*0.0708 (0x00427eae fmul
#      [0x005cd5fc]), composition from FUN_00554940 (FUN_00552df0(pen) -> FUN_00552da0(s',s') ->
#      FUN_00552df0(0,-[font+8]), all pre-concat), pixel = (n.x*W, (1 - n.y)*H)
#  (c) the closed form the standalone uses: top = y + cell_h*(0.025/0.0708 + c8 - 1), cell_h = scale*0.0708*H
# Prints per call: args, [0x67d830]/[0x67d834], c8, quad top from (a), (b), (c), and max |a-b| over all verts.
import json, struct, sys
F = json.load(open(sys.argv[1]))["frame"]
cam = F["cam"]; W, H = cam["ras_w"], cam["ras_h"]
def mul(p, m):
    x, y, z = p
    return (x*m[0] + y*m[4] + z*m[8] + m[12], x*m[1] + y*m[5] + z*m[9] + m[13], x*m[2] + y*m[6] + z*m[10] + m[14])
f32 = lambda v: struct.unpack("<f", struct.pack("<f", v))[0]
K0708 = f32(0.0708)
print(f"camera proj {cam['proj']} raster {W}x{H} view diag {cam['view'][0]:.6f} {cam['view'][5]:.6f} {cam['view'][10]:.6f} "
      f"row3 {cam['view'][12]:.6f} {cam['view'][13]:.6f} {cam['view'][14]:.6f}")
worst = 0.0
for ci, c in enumerate(F["calls"]):
    pi = c.get("pen")
    pen = F["pens"][pi] if pi is not None else None
    subs = [s for s in F["submits"] if s["call"] == ci]
    if pen is None: print(f"call {ci} {c['fn']} ret {c['ret']}: no pen"); continue
    s = pen["scale"]; sp = f32(s*K0708); c8 = pen["c8"]; px, py = pen["pen"]
    tops_a, tops_b, dmax = [], [], 0.0
    for sb in subs:
        for q in range(0, len(sb["verts"]) - 3, 4):
            ya, yb = [], []
            for v in sb["verts"][q:q+4]:
                p = mul(mul(v[:3], sb["ltm"]), cam["view"]); a = (p[0]/p[2]*W, p[1]/p[2]*H)
                nx = px + sp*v[0]; ny = py + sp*(v[1] - c8); b = (nx*W, (1 - ny)*H)
                dmax = max(dmax, abs(a[0]-b[0]), abs(a[1]-b[1])); ya.append(a[1]); yb.append(b[1])
            tops_a.append(min(ya)); tops_b.append(min(yb))
    worst = max(worst, dmax)
    cell_h = s*K0708*H
    top_c = pen["y"] + cell_h*(0.025/0.0708 + c8 - 1.0)
    ta = f"{min(tops_a):.4f}" if tops_a else "n/a"; tb = f"{min(tops_b):.4f}" if tops_b else "n/a"
    print(f"call {ci:2d} fn {c['fn']} ret {c['ret']} id 0x{int(c['raw'][0],16):x} y {pen['y']:.4f} s {s:.4f} al {pen['align']} "
          f"k30 {pen['k'][2]} k34 {pen['k'][3]} c8 {c8:.6f}({pen['c8hex']}) pen ({px:.6f},{py:.6f}) glyphs {len(tops_a)} "
          f"top cam {ta} bytes {tb} closed {top_c:.4f} | max|cam-bytes| {dmax:.5f}")
print(f"WORST max|cam-bytes| over all vertices: {worst:.6f} px")
