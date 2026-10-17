"""Warzone pontoon / deck draws per capfix frame: the boat (texture battle_platform3, BOAT.DFF's only texture) and untextured
3D draws whose world AABB lies inside the platform area (DECK.DFF / TOWER.DFF are untextured), original vs each arm.
    py -3.12 boat_rows.py <arm> [<arm> ...]      (arm = tracks/<arm>/11_Warzone/stand/d1r2)"""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
CAP = r'C:\Users\maria\Desktop\Proyectos\Mashed\verify\capfix_20261016\11_Warzone'
ROWS = (584, 624, 659)
AREA = (155.0, -4.0, 40.0, 195.0, 8.0, 80.0)   # x0 y0 z0 x1 y1 z1 around the platform at rest (173.5, -1.4, 58.4)


def draws(path):
    d = json.load(open(path))
    return [e for e in d.get('events', []) if e.get('e') == 'd']


def inside(bb):
    return (bb[0] >= AREA[0] and bb[3] <= AREA[3] and bb[1] >= AREA[1] and bb[4] <= AREA[4]
            and bb[2] >= AREA[2] and bb[5] <= AREA[5])


def pick(evs):
    boat = sorted((e['tri'], tuple(round(x, 2) for x in e['bb'])) for e in evs if e.get('tex') == 'battle_platform3')
    unt = sorted((e['tri'], tuple(round(x, 2) for x in e['bb'])) for e in evs
                 if e.get('tex') in ('-', None, '') and e.get('pt') == 4 and 'bb' in e and inside(e['bb']))
    return boat, unt


def main():
    out = []
    for k in ROWS:
        o = os.path.join(CAP, f'orig_row_{k:05d}.draws.json')
        ob, ou = pick(draws(o))
        out.append(f'row {k} ORIG boat {ob} untextured-in-area {ou}')
        for arm in sys.argv[1:]:
            p = os.path.join(HERE, 'tracks', arm, '11_Warzone', 'stand', 'd1r2', f'stand_row_{k:05d}.draws.json')
            if not os.path.exists(p):
                out.append(f'row {k} {arm}: missing'); continue
            sb, su = pick(draws(p))
            same_tri = [b[0] for b in ob] == [b[0] for b in sb]
            maxd = 0.0
            if same_tri:
                for (_, a), (_, b) in zip(ob, sb):
                    maxd = max(maxd, max(abs(x - y) for x, y in zip(a, b)))
            out.append(f'row {k} {arm:6s} boat {sb} untextured-in-area {su} | boat tris equal {same_tri}, '
                       f'max AABB delta {maxd:.3f}; untextured-in-area count orig {len(ou)} arm {len(su)}')
    txt = '\n'.join(out)
    print(txt)
    open(os.path.join(HERE, 'arms', 'BOAT_ROWS.txt'), 'w').write(txt + '\n')


if __name__ == '__main__':
    main()
