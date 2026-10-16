"""camribbon G-FRAME-AFTER / G-FRAME-BEFORE-WORSE (PREREG_camribbon_addendum2.md section B2).

    py -3.12 frame_score.py <orig_dir> <after_dir> <before_dir> <out_dir> K [K ...] [--ctl 60]

orig_dir holds orig_row_<K>.bmp, after_dir / before_dir hold stand_row_<R>.bmp. Prints, per K, the
re/tools/imgdiff.py mean-abs numbers and writes <out_dir>/sbs3_row_<K>.jpg = original | before | after.
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "d1r2_20261016"))
import score_d1r2 as S  # noqa: E402
from PIL import Image  # noqa: E402


def main():
    a = sys.argv[1:]
    ctl = 60
    if "--ctl" in a:
        i = a.index("--ctl"); ctl = int(a[i + 1]); del a[i:i + 2]
    od, ad, bd, out = Path(a[0]), Path(a[1]), Path(a[2]), Path(a[3])
    ks = [int(k) for k in a[4:]]
    out.mkdir(parents=True, exist_ok=True)
    ok_after = ok_worse = True
    for k in ks:
        o = od / f"orig_row_{k:05d}.bmp"
        d = {}
        for off in (0, -ctl, ctl):
            d[("after", off)] = S.imgdiff_mean(o, ad / f"stand_row_{k + off:05d}.bmp")[0]
        d[("before", 0)] = S.imgdiff_mean(o, bd / f"stand_row_{k:05d}.bmp")[0]
        sharp = d[("after", 0)] < d[("after", -ctl)] and d[("after", 0)] < d[("after", ctl)]
        worse = d[("before", 0)] > d[("after", 0)]
        ok_after &= sharp
        ok_worse &= worse
        print(f"K={k}: orig@K vs after@K {d[('after', 0)]:.2f}  after@K-{ctl} {d[('after', -ctl)]:.2f}  "
              f"after@K+{ctl} {d[('after', ctl)]:.2f}  =>  sharp minimum: {'PASS' if sharp else 'FAIL'}")
        print(f"       orig@K vs before@K {d[('before', 0)]:.2f}  (after {d[('after', 0)]:.2f})  =>  "
              f"before worse: {'PASS' if worse else 'FAIL'}")
        io = Image.open(o).convert("RGB")
        ib = Image.open(bd / f"stand_row_{k:05d}.bmp").convert("RGB").resize(io.size)
        ia = Image.open(ad / f"stand_row_{k:05d}.bmp").convert("RGB").resize(io.size)
        sbs = Image.new("RGB", (io.width * 3, io.height))
        sbs.paste(io, (0, 0)); sbs.paste(ib, (io.width, 0)); sbs.paste(ia, (io.width * 2, 0))
        sbs = sbs.resize((sbs.width // 2, sbs.height // 2))
        p = out / f"sbs3_row_{k:05d}.jpg"
        sbs.save(p, quality=85)
        print(f"       side-by-side (original | before | after) -> {p}")
    print(f"G-FRAME-AFTER: {'PASS' if ok_after else 'FAIL'}   G-FRAME-BEFORE-WORSE: {'PASS' if ok_worse else 'FAIL'}")
    return 0 if (ok_after and ok_worse) else 1


if __name__ == "__main__":
    sys.exit(main())
