"""G-P1 byte compare: port course block vs the original's live block (PREREG_PATHPROG.md section 4)."""
import struct, sys
REG = [("dist", 0x0, 0x6000), ("angle", 0x6000, 0xc1c0), ("nodes", 0xc1c0, 0x159c0),
       ("route", 0x159c0, 0x161c0), ("safe", 0x161c0, 0x16240), ("hdr", 0x16240, 0x16264)]
FIELDS = ["dir.x","dir.y","dir.z","ctr.x","ctr.y","ctr.z"] + [f"c{k}.{a}" for k in range(4) for a in "xyz"] + ["+48"]
def main(o, p):
    O = open(o, "rb").read(); P = open(p, "rb").read()
    allok = True
    for name, a, e in REG:
        diffs = [i for i in range(a, e, 4) if O[i:i+4] != P[i:i+4]]
        print(f"{name:6s} [{a:#07x},{e:#07x}) diff dwords = {len(diffs)}")
        for i in diffs[:6]:
            ov, pv = struct.unpack_from("<I", O, i)[0], struct.unpack_from("<I", P, i)[0]
            of, pf = struct.unpack_from("<f", O, i)[0], struct.unpack_from("<f", P, i)[0]
            where = ""
            if name == "nodes":
                r, f = divmod(i - 0xc1c0, 0x4c); where = f" node {r} {FIELDS[f//4]}"
            print(f"   +{i:#07x}{where}: orig {ov:08x} ({of!r})  port {pv:08x} ({pf!r})  ulp {pv-ov:+d}")
        allok &= not diffs
    print("G-P1", "PASS" if allok else "RED")
if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
