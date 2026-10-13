# List the original's settings-widget draws (caller 0x430xxx/0x431xxx) in frame 0
# of a menu_draw_burst capture, decoded with drawlist_diff's own normalizer.
import json, sys
sys.path.insert(0, r"C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\scr19fix\re\tools")
import drawlist_diff as dd
d = json.load(open(sys.argv[1]))
k = list(d)[0]
for i, rec in enumerate(d[k]):
    r = dd.normalize(rec, 1.0)
    rets = r['rets']
    if len(rets) > 1 and rets[1][:5] in ('0x430', '0x431', '0x432'):
        print(f"{k}[{i}] x={r['x']:.4f} y={r['y']:.4f} w={r['w']:.2f} h={r['h']:.2f} col={r['colors'][0]:08x} {rets[0]},{rets[1]}")
