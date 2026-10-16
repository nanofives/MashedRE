"""carlod live witness for FUN_00420870's per-part visibility (PREREG_carlod.md section 1).

Original side only. Input: rtex's --matfx captures (log/rtex/orig_matfx2_<track>/trace.csv.matfx.json,
every 100 trace rows, per car record: flags +0x294, cached part-0x13 position +0x258, the camera
object's frame position, and for every part atomic its render byte atomic+2). Checks, per car record:

  1. the switch at 0x00420a33..0x00420b16 (model() below, transcribed from the decomp in
     evidence/decomp_fun_00420870.txt) with some state S in {0,1,2} reproduces every part's render
     bit (atomic+2 & 4, set/cleared by FUN_004b5240 @ 0x004b524f / 0x004b5256);
  2. the state S implied by the fit equals the distance rule read off FUN_00446520: S = 1 when
     d2 < 144.0 (fld [ebp-0x1ac]; fcomp [0x005ce1d0] = 144.0; test ah,5; jp -> S 2, 0x00446965 /
     0x00446975), else 2, with d2 = |rec+0x258 - camera frame pos|^2 (FUN_0041f120 @ 0x0041f120).

    py -3.12 vis_check.py [track ...]        (default: training arctic city)
"""
import json
import sys

BASE = r"C:\Users\maria\Desktop\Proyectos\Mashed\log\rtex\orig_matfx2_%s\trace.csv.matfx.json"


def model(S, F, part):
    """FUN_00420870 @ 0x00420870: switch(part) at 0x00420a33, state S = *(rec+0x298), flags F =
    *(rec+0x294). Returns the bool handed to FUN_004b5240 (0x00420b16)."""
    f10, f20, f40, f80 = bool(F & 0x10), bool(F & 0x20), bool(F & 0x40), bool(F & 0x80)
    f400, f800 = bool(F & 0x400), bool(F & 0x800)
    special = False                      # parts 0x17 / 0x32 skip the 0x400 mask (0x00420b98)
    if part in (0, 1, 2, 3, 8, 9, 0x10, 0x12, 0x14): v = S == 0
    elif part in (4, 6): v = S == 0 and not f10
    elif part in (5, 7): v = S == 0 and f10
    elif part in (0xa, 0xc): v = S == 0 and not f20
    elif part in (0xb, 0xd): v = S == 0 and f20
    elif part == 0x11: v = f800 and not f20
    elif part == 0x15: v = f40 and not f10
    elif part == 0x17: v = S == 0 and f400; special = True
    elif part == 0x18: v = f80 and not f20
    elif part in (0x21, 0x22, 0x23, 0x24, 0x29, 0x2a): v = S == 1
    elif part in (0x25, 0x27): v = S == 1 and not f10
    elif part in (0x26, 0x28): v = S == 1 and f10
    elif part in (0x2b, 0x2d): v = S == 1 and not f20
    elif part in (0x2c, 0x2e): v = S == 1 and f20
    elif part in (0x31, 0x47): v = S == 1
    elif part == 0x32: v = S == 1 and f400; special = True
    elif part == 0x46: v = S == 0
    elif part == 0x64: v = S == 2 and not (F & 0x30)
    elif part == 0x65: v = S == 2 and f10 and not f20
    elif part == 0x66: v = S == 2 and not f10 and f20
    elif part == 0x67: v = S == 2 and f10 and f20
    else: v = False                      # default: bVar11 stays false (0x00420a2e)
    return v if special else (v and not f400)


def main(tracks):
    tot_cars = fit_ok = rule_ok = both = 0
    for tr in tracks:
        snaps = json.load(open(BASE % tr))
        for sn in snaps:
            if 's' not in sn:
                continue
            for ci, c in enumerate(sn['s']['cars']):
                parts = c['parts']
                pos, cam = c.get('pos258'), c.get('p29c_pos')
                if not parts or not pos or not cam:
                    continue
                F = c['flags']
                fits = sorted((sum(1 for p in parts if bool(p['rflags'] & 4) != model(S, F, p['p'])), S)
                              for S in (0, 1, 2))
                sfit = fits[0][1] if fits[0][0] == 0 else -1
                d2 = (pos[2] - cam[2]) ** 2 + (pos[0] - cam[0]) ** 2 + (pos[1] - cam[1]) ** 2
                srule = 1 if d2 < 144.0 else 2
                tot_cars += 1
                fit_ok += sfit >= 0
                rule_ok += (fits[[s for _, s in fits].index(srule)][0] == 0)
                both += (sfit == srule)
                vis = sorted(p['p'] for p in parts if p['rflags'] & 4)
                print("%-8s row %4d ba8c %d car%d F=%#05x d2=%9.2f rule=S%d fit=S%s mism(best)=%d vis=%s%s" % (
                    tr, sn['row'], sn['ba8c'], ci, F, d2, srule, sfit if sfit >= 0 else '?', fits[0][0],
                    ','.join('%x' % p for p in vis), '' if sfit == srule else '   <-- no exact fit'))
    print("car records %d: switch fits a state exactly on %d, rule's S fits exactly on %d, both agree on %d"
          % (tot_cars, fit_ok, rule_ok, both))


if __name__ == "__main__":
    main(sys.argv[1:] or ["training", "arctic", "city"])
