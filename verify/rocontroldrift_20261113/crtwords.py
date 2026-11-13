"""RO-CONTROL-DRIFT: the CRT-startup words of block 0x00773c00 and the MASK:os toggle 0x006147d8 in every checkpoint-0
dump under D:/MashedWT/*_data (any depth), with the dump's mtime.

  0x00773c40  260-byte buffer GetModuleFileNameA(NULL, 0x00773c40, 0x104) fills at 0x004abeb0 (exe path)
  0x00773d60  dword GetProcAddress(GetModuleHandleA("kernel32.dll"), "InitializeCriticalSectionAndSpinCount") at 0x004ae2d8
  0x006147d8  MASK:os row of re/frida/warp_normalize.tsv

    py -3.12 verify/rocontroldrift_20261113/crtwords.py --out verify/rocontroldrift_20261113/CRTWORDS.tsv
"""
import argparse, collections, datetime, glob, os, pathlib, struct

LO = 0x005EA000


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    a = ap.parse_args()
    rows = []
    for f in glob.glob("D:/MashedWT/*_data/**/ckpt00.data.bin", recursive=True):
        try:
            with open(f, "rb") as h:
                h.seek(0x00773C40 - LO); path = h.read(0x104).split(b"\0")[0].decode("latin1")
                h.seek(0x00773D60 - LO); k = struct.unpack("<I", h.read(4))[0]
                h.seek(0x006147D8 - LO); t = struct.unpack("<I", h.read(4))[0]
        except OSError:
            continue
        rows.append((os.path.getmtime(f), f.replace("\\", "/"), path, k, t))
    rows.sort()
    with open(a.out, "w", encoding="utf-8") as o:
        o.write("mtime\tdump\texe_path_0x00773c40\tdword_0x00773d60\tdword_0x006147d8\n")
        for m, f, p, k, t in rows:
            o.write(f"{datetime.datetime.fromtimestamp(m):%Y-%m-%d %H:%M}\t{f}\t{p}\t{k:#010x}\t{t}\n")
    # summary: per value of 0x00773d60, the mtime span; per exe-path root, the count
    span = collections.OrderedDict()
    for m, f, p, k, t in rows:
        span.setdefault(k, []).append(m)
    for k, v in span.items():
        print(f"0x00773d60={k:#010x}: {len(v)} dumps, {datetime.datetime.fromtimestamp(min(v)):%Y-%m-%d %H:%M} .. {datetime.datetime.fromtimestamp(max(v)):%Y-%m-%d %H:%M}")
    roots = collections.Counter(p.rsplit("\\original\\", 1)[0] for _, _, p, _, _ in rows)
    for r, n in roots.most_common():
        print(f"root {r}: {n}")
    print("0x006147d8 values:", collections.Counter(t for *_, t in rows))
    print("dumps", len(rows))


if __name__ == "__main__":
    main()
