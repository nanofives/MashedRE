r"""RO-EXE-TICK-TRACE: offline check of one trace directory (exe MASHED_REPLAY_TRACE or original detorig.py --boot).

usage: py -3.12 trace_verify.py <dir> [<dir> ...]   prints one OK / FAIL line per directory, exit 1 on any FAIL.

Checks: meta.json has the keys detorig_ticks.py / detorig_report.py read; ticks.bin holds meta.ticks.total records of
recw dwords; record k is tick k + 1 (boot mode); for every checkpoint taken at a tick boundary (exe side: field "tick"),
the dump re-hashed (FNV-1a per block over dwords, heap rule [0x00995000, 0x30000000) low 2 bits clear -> 0) equals that
tick's record, and the raw words equal the dump. Also prints each file's SHA-256 (for the RESULT).
"""
import hashlib, json, struct, sys
from pathlib import Path

DATA_LO = 0x005ea000


def fnv_blocks(words, bw):
    out = []
    for b in range(0, len(words), bw):
        h = 2166136261
        for v in words[b:b + bw]:
            if 0x00995000 <= v < 0x30000000 and not (v & 3):
                v = 0
            h = ((h ^ v) * 16777619) & 0xffffffff
        out.append(h)
    return out


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check(d):
    d = Path(d)
    errs = []
    m = json.loads((d / "meta.json").read_text())
    for k in ("ticks", "ckpt", "race", "cfg"):
        if k not in m:
            errs.append(f"meta.json has no {k}")
    t = m["ticks"]
    recw, nblk, block = t["recw"], t["nblk"], t["block"]
    raw = (d / t["file"]).read_bytes()
    if len(raw) % (recw * 4):
        errs.append(f"ticks.bin size {len(raw)} not a multiple of {recw * 4}")
    n = len(raw) // (recw * 4)
    if n != t["total"]:
        errs.append(f"records {n} != meta total {t['total']}")
    recs = {}
    for i in range(n):
        r = struct.unpack_from(f"<{recw}I", raw, i * recw * 4)
        if r[0] != i + 1:
            errs.append(f"record {i} tick {r[0]}, expected {i + 1}")
            break
        recs[r[0]] = r
    raw_va = [va for lo, hi in t.get("raw") or [] for va in range(lo, hi, 4)]
    if 2 + nblk + len(raw_va) != recw:
        errs.append(f"recw {recw} != 2 + {nblk} + {len(raw_va)}")
    nck = 0
    for c in m["ckpt"]:
        b = (d / c["file"]).read_bytes()
        words = struct.unpack(f"<{len(b) // 4}I", b)
        if c.get("tick") is None or c["k"] == 100 or c["tick"] not in recs:
            continue
        r = recs[c["tick"]]
        hb = fnv_blocks(words, block // 4)
        bad = [i for i in range(nblk) if hb[i] != r[2 + i]]
        if bad:
            errs.append(f"ckpt k {c['k']} tick {c['tick']}: {len(bad)} block hashes differ, first block {bad[0]} "
                        f"(0x{DATA_LO + bad[0] * block:08x})")
        rb = [i for i, va in enumerate(raw_va) if words[(va - DATA_LO) // 4] != r[2 + nblk + i]]
        if rb:
            errs.append(f"ckpt k {c['k']} tick {c['tick']}: {len(rb)} raw words differ, first 0x{raw_va[rb[0]]:08x}")
        nck += 1
    files = ["meta.json", t["file"]] + [c["file"] for c in m["ckpt"]]
    print(f"{'FAIL' if errs else 'OK'} {d}: records {n} x {recw} dwords, {nck} checkpoints re-hashed, side {m.get('side', 'original')}, "
          f"race n {m['race'].get('n')} tick {m['race'].get('tick')}, selftest {m.get('selftest')}")
    for e in errs[:10]:
        print("   ", e)
    for f in files:
        print(f"    sha256 {f} {sha(d / f)}")
    return not errs


if __name__ == "__main__":
    ok = all([check(a) for a in sys.argv[1:]])
    sys.exit(0 if ok else 1)
