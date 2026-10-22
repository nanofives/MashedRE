r"""rpct pre-gate: from a MASHED_ELIM_TRACE csv, count the race rows at the elimination zoom (required zoom == 10.0, 0x41200000)
where the stand-in race_pct (pct<i>, FillRaceCamCars) and the slot (o_pct<i>, 0x008a96ec + i*0x30c, what 0x00408ad0 loads) would
pick a different victim. EliminationCheck (Race/RaceCamera.cpp) takes the pair from MostSeparatedPair; the trace has no y, so
the pair is not rebuilt: every active+alive pair is checked, which bounds the count from above.
Victim law (RaceCamera.cpp EliminationCheck): pa > 80 && pb < 20 -> pa -= 100, else pb > 80 && pa < 20 -> pb -= 100; the car
with the lower value is the victim (pa < pb -> a, else b).
usage: py -3.12 pregate.py <elim.csv> [...]"""
import csv, sys


def victim(pa, pb):
    if pa > 80.0 and pb < 20.0:
        pa -= 100.0
    elif pb > 80.0 and pa < 20.0:
        pb -= 100.0
    return 0 if pa < pb else 1


for path in sys.argv[1:]:
    rows = list(csv.DictReader(open(path, newline='')))
    z10 = [r for r in rows if r['zoom_hex'] == '41200000' and float(r['countdown']) <= 0.0]
    n_rows = n_any = 0
    pairs = {}
    first = None
    for r in z10:
        alive = [i for i in range(4) if r[f'act{i}'] == '1' and r[f'alive{i}'] == '1']
        if len(alive) < 2:
            continue
        n_rows += 1
        hit = False
        for x in range(len(alive)):
            for y in range(x + 1, len(alive)):
                a, b = alive[x], alive[y]
                v_s = victim(float(r[f'pct{a}']), float(r[f'pct{b}']))
                v_o = victim(float(r[f'o_pct{a}']), float(r[f'o_pct{b}']))
                if v_s != v_o:
                    hit = True
                    pairs[(a, b)] = pairs.get((a, b), 0) + 1
        if hit:
            n_any += 1
            if first is None:
                first = r
    print(f"{path}: rows={len(rows)} zoom10_racing={len(z10)} with>=2 alive={n_rows} "
          f"rows where some alive pair flips victim={n_any} per_pair={dict(sorted(pairs.items()))}")
    if first is not None:
        print("  first flip row n=%s round=%s " % (first['n'], first['round']) +
              " ".join(f"c{i}:alive={first[f'alive{i}']},pct={float(first[f'pct{i}']):.3f},o_pct={float(first[f'o_pct{i}']):.3f}"
                       for i in range(4)))
