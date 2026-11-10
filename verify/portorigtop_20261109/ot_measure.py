"""PORT-OT-MEASURE (L-111 (a)): per row of the ORIG_TOP route, is RwGlobals usable and is its initialised data seeded?

  py -3.12 verify/portorigtop_20261109/ot_measure.py lists                 # the two row sets -> <data>/ot_rows.json
  py -3.12 verify/portorigtop_20261109/ot_measure.py probe --exe E --root R [--secs 30]   # under mashed_lock: one
        knob-ON boot (MASHED_ORIG_TOP=1), reads the exe's own memory -> <data>/probe.json
  py -3.12 verify/portorigtop_20261109/ot_measure.py verdict               # -> verify/portorigtop_20261109/OT_MEASURE.tsv

<data> = D:/MashedWT/PORT-OT-MEASURE_data (or --data).

Row sets: the rows of RESULT_PORT-ORIGTOP-SIZE that are runnable once every binding family is bound (1,207), and of
those
  RWG   the 72 whose matched body names DAT_007d3ff8 (RwGlobals). Per row: the offsets it uses off RwGlobals and
        whether each is a call slot (`(code **)(DAT_007d3ff8 + off)`) or a data read;
  SEED  the 123 that read a global whose bytes are nonzero in MASHED.exe's .data / .rdata. Per row: those addresses.
Probe: MASHED_ORIG_TOP=1, MASHED_DET_FRAMES=<n>. The probe reads the process memory with ReadProcessMemory at several
times: the dword at 0x007d3ff8, the dword at every used offset off it, and the bytes at every SEED address. It only
reads, and it kills only the PID it spawned.
Verdict per row:
  RWG   OK                  RwGlobals is non-null and every call slot the row uses is non-null, at the last sample
        needs binding       RwGlobals null, or a call slot it uses is null (the slots are named)
  SEED  OK                  every address is inside a seed (Compat/PortStateSeed.inc, L-99, applied at exe WinMain entry
                            before the ORIG_TOP fork; Match/PortTop_RdataSeed.inc, the knob path), or the exe holds
                            the original bytes there at the first sample anyway
        needs seed rule     some address is not seeded and the exe does not hold the original bytes there
A row in both sets gets both columns. Static facts plus one boot: the gates of each swap stay the authority.
"""
from __future__ import annotations

import collections
import csv
import ctypes
import json
import os
import pathlib
import re
import subprocess
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "re" / "tools"))
import origtop_size as O          # noqa: E402
import port_closed_sets as P      # noqa: E402

RWG = 0x007d3ff8
SLOT = re.compile(r"DAT_007d3ff8\s*\+\s*(0x[0-9a-fA-F]+|\d+)")
CALLSLOT = re.compile(r"\(\s*code\s*\*\*\s*\)\s*\(\s*DAT_007d3ff8\s*\+\s*(0x[0-9a-fA-F]+|\d+)\s*\)")


def opt(argv, k, d=None):
    return argv[argv.index(k) + 1] if k in argv else d


def final_set():
    """The 1,207 rows of RESULT_PORT-ORIGTOP-SIZE (same classes, edges and fixpoint as origtop_size.py)."""
    ent, bounds = O.load_bounds()
    fset = set(ent)
    dedge = O.direct_edges()
    redge, _n = O.reloc_edges(O.EXE_DEF, ent, bounds)
    edges = collections.defaultdict(set)
    for src in (dedge, redge):
        for a, cs in src.items():
            edges[a] |= {c for c in cs if c in fset and c != a}
    H = P.load_hooks()
    files, by_rva = P.load_matched()
    B = O.load_bindings()
    plat = set()
    with open(ROOT / "re" / "console" / "match" / "platform_candidates.csv", newline="") as fh:
        for r in csv.DictReader(fh):
            if r["signal"] == "platform-api":
                plat.add(int(r["pc_va"], 16))

    def cls(a):
        k = B.get(a)
        if k in O.BIND_SAT:
            return "bind:" + k
        r = H.get(a)
        if k == "matched":
            return "match"
        if a in plat:
            return "fam:d3d-dshow"
        if r is None:
            return "norow"
        fam = O.family_of(r)
        if r["confidence"] == "C4" and a in by_rva:
            if fam and fam != "rwphysics":
                return "fam:" + fam
            return "match"
        if fam:
            return "fam:" + fam
        if r["confidence"] == "C4":
            return "game-c4-hand" if (r.get("exe_file") or "").strip() else "game-c4"
        return "game-c0c3"
    C = {a: cls(a) for a in fset}
    reach, st = {O.TOP}, [O.TOP]
    while st:
        a = st.pop()
        if C[a].startswith("bind:") or C[a].startswith("fam:"):
            continue
        for c in edges.get(a, ()):
            if c not in reach:
                reach.add(c)
                st.append(c)
    sat = {"fam:rt2d", "fam:lua", "fam:d3d-dshow", "fam:rwphysics", "fam:crt", "fam:rwcore"}
    ok = {a for a in reach if C[a] == "match"}
    changed = True
    while changed:
        changed = False
        for a in list(ok):
            for c in edges.get(a, ()):
                if c not in reach:
                    continue
                k = C[c]
                if (k == "match" and c in ok) or k.startswith("bind:") or k in sat:
                    continue
                ok.discard(a)
                changed = True
                break
    return ok, H, files, by_rva


def rwg_offsets_from_binary(a, end):
    """(call_slots, data_offs) of RwGlobals uses in the ORIGINAL function [a, end): a register loaded from [0x007d3ff8]
    and then used as a memory base. The matched sources spell these as `DAT_007d3ff8->member` or `(DAT_007d3ff8 + off)`,
    so the binary is the one reading that gives every offset."""
    import capstone
    import pefile
    if not hasattr(rwg_offsets_from_binary, "img"):
        pe = pefile.PE(str(O.EXE_DEF), fast_load=True)
        rwg_offsets_from_binary.img = pe.get_memory_mapped_image()
        rwg_offsets_from_binary.base = pe.OPTIONAL_HEADER.ImageBase
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        md.detail = True
        rwg_offsets_from_binary.md = md
    img, base, md = rwg_offsets_from_binary.img, rwg_offsets_from_binary.base, rwg_offsets_from_binary.md
    X = capstone.x86
    calls, datas = set(), set()
    # register state: "R" = RwGlobals, ("M", addr) = the value of a module-offset global at addr (0x007dxxxx),
    # ("RM", addr) = RwGlobals + that module offset. A use is recorded as an int (plain offset) or "m<addr>+<disp>".
    st = {}

    def absmem(op):
        return op.type == X.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0

    def use(op, is_call):
        b, i = st.get(op.mem.base), st.get(op.mem.index) if op.mem.index else None
        d = op.mem.disp & 0xffffffff
        key = None
        if b == "R" and i is None:
            key = d
        elif isinstance(b, tuple) and b[0] == "RM" and i is None:
            key = "m%08x+0x%x" % (b[1], d)
        elif (b == "R" and isinstance(i, tuple) and i[0] == "M") or (i == "R" and isinstance(b, tuple) and b[0] == "M"):
            key = "m%08x+0x%x" % ((i if b == "R" else b)[1], d)
        if key is not None:
            (calls if is_call else datas).add(key)
    # pc_funcs.csv body_end is the address of the LAST instruction, so disassemble a little past it and stop there
    for ins in md.disasm(img[a - base:end + 16 - base], a):
        if ins.address > end:
            break
        ops = ins.operands
        mn = ins.mnemonic
        for op in ops:
            if op.type == X.X86_OP_MEM and not absmem(op):
                use(op, mn in ("call", "jmp"))
        if mn == "mov" and len(ops) == 2 and ops[0].type == X.X86_OP_REG:
            dst, src = ops[0].reg, ops[1]
            if absmem(src) and (src.mem.disp & 0xffffffff) == RWG:
                st[dst] = "R"
            elif absmem(src) and 0x007d0000 <= (src.mem.disp & 0xffffffff) < 0x007e0000:
                st[dst] = ("M", src.mem.disp & 0xffffffff)
            elif src.type == X.X86_OP_REG and src.reg in st:
                st[dst] = st[src.reg]
            else:
                st.pop(dst, None)
            continue
        if mn == "add" and len(ops) == 2 and ops[0].type == X.X86_OP_REG:
            dst, src = ops[0].reg, ops[1]
            cur = st.get(dst)
            other = None
            if absmem(src) and (src.mem.disp & 0xffffffff) == RWG:
                other = "R"
            elif absmem(src) and 0x007d0000 <= (src.mem.disp & 0xffffffff) < 0x007e0000:
                other = ("M", src.mem.disp & 0xffffffff)
            elif src.type == X.X86_OP_REG:
                other = st.get(src.reg)
            if cur == "R" and isinstance(other, tuple) and other[0] == "M":
                st[dst] = ("RM", other[1])
            elif other == "R" and isinstance(cur, tuple) and cur[0] == "M":
                st[dst] = ("RM", cur[1])
            elif not (src.type == X.X86_OP_IMM):
                st.pop(dst, None)
            continue
        if ops and ops[0].type == X.X86_OP_REG and mn not in ("cmp", "test", "push", "call"):
            st.pop(ops[0].reg, None)
        if mn == "call":
            for r in (X.X86_REG_EAX, X.X86_REG_ECX, X.X86_REG_EDX):
                st.pop(r, None)
    key = lambda k: (isinstance(k, str), str(k))
    return sorted(calls, key=key), sorted(datas - calls, key=key)


def lists(data):
    final, H, files, by_rva = final_set()
    _ent, bounds = O.load_bounds()
    pe = P.PE()
    raw = pe.raw
    rows = {}
    for a in sorted(final):
        f = (by_rva.get(a) or [None])[0]
        if not f:
            continue
        txt, bodies = files[f]
        body = bodies.get(a, "")
        rec = {"rva": "0x%08x" % a, "name": (H.get(a) or {}).get("name", "?"),
               "subsystem": (H.get(a) or {}).get("subsystem", "?"), "file": f}
        if "DAT_007d3ff8" in body:
            calls, datas = rwg_offsets_from_binary(a, bounds[a])
            rec["rwg_call_slots"] = calls
            rec["rwg_data_offs"] = datas
            rec["rwg_text_offs"] = sorted({int(x, 0) for x in SLOT.findall(body)})   # cross-check only
        seeds = []
        for n, ga, sz in P.globals_of(txt, body):
            sec, kind = pe.classify(ga, sz)
            if sec in (".data", ".rdata") and kind == "nonzero":
                for name, va, vs, rs, pr in pe.secs:
                    if va <= ga < va + max(vs, rs):
                        off = ga - va
                        b = raw[pr + off: pr + min(off + sz, rs)]
                        seeds.append({"name": n, "addr": "0x%08x" % ga, "size": sz, "sec": sec, "orig": b.hex()})
                        break
        if seeds:
            rec["seed"] = seeds
        if "rwg_call_slots" in rec or seeds:
            rows[rec["rva"]] = rec
    out = pathlib.Path(data) / "ot_rows.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    json.dump(rows, open(out, "w"), indent=1)
    print("rows %d: RWG %d, SEED %d -> %s" % (len(rows), sum("rwg_call_slots" in r for r in rows.values()),
                                               sum("seed" in r for r in rows.values()), out))


# ------------------------------------------------------------------ probe (inside mashed_lock)
PROCESS_VM_READ, PROCESS_QUERY_INFORMATION = 0x0010, 0x0400
k32 = ctypes.WinDLL("kernel32", use_last_error=True) if os.name == "nt" else None


def rpm(h, addr, n):
    buf = ctypes.create_string_buffer(n)
    got = ctypes.c_size_t(0)
    if not k32.ReadProcessMemory(h, ctypes.c_void_p(addr), buf, n, ctypes.byref(got)) or got.value != n:
        return None
    return buf.raw


def probe(argv, data):
    rows = json.load(open(pathlib.Path(data) / "ot_rows.json"))
    exe, root = opt(argv, "--exe"), opt(argv, "--root")
    secs = float(opt(argv, "--secs", "30"))
    offs = sorted({o for r in rows.values() for o in r.get("rwg_call_slots", []) + r.get("rwg_data_offs", [])},
                  key=lambda o: (isinstance(o, str), str(o)))
    addrs = sorted({(int(s["addr"], 16), s["size"]) for r in rows.values() for s in r.get("seed", [])})
    env = dict(os.environ, MASHED_ROOT=root, MASHED_ORIG_TOP="1", MASHED_DET_FRAMES=opt(argv, "--frames", "3000"))
    p = subprocess.Popen([exe], cwd=root, env=env)
    print("spawned pid", p.pid, flush=True)
    h = k32.OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, False, p.pid)
    samples = []
    t0 = time.time()
    marks = [0.5, 1.5, 3, 6, 10, 15, 22, secs]
    for m in marks:
        while time.time() - t0 < m and p.poll() is None:
            time.sleep(0.05)
        if p.poll() is not None:
            break
        s = {"t": round(time.time() - t0, 2)}
        g = rpm(h, RWG, 4)
        s["rwg"] = int.from_bytes(g, "little") if g else None
        if s["rwg"]:
            s["slots"], s["mods"] = {}, {}
            for o in offs:
                if isinstance(o, str):          # "m<addr>+0x<disp>": RwGlobals + [addr] + disp
                    ma, d = o[1:].split("+")
                    mv = rpm(h, int(ma, 16), 4)
                    m = int.from_bytes(mv, "little") if mv else None
                    s["mods"][ma] = m
                    v = rpm(h, s["rwg"] + m + int(d, 16), 4) if m is not None else None
                else:
                    v = rpm(h, s["rwg"] + o, 4)
                s["slots"][str(o)] = int.from_bytes(v, "little") if v else None
        s["seed"] = {}
        for a, n in addrs:
            v = rpm(h, a, n)
            s["seed"]["0x%08x" % a] = v.hex() if v is not None else None
        samples.append(s)
        print("sample t=%.1f rwg=%s" % (s["t"], hex(s["rwg"]) if s["rwg"] else s["rwg"]), flush=True)
    try:
        rc = p.wait(timeout=max(5.0, secs + 30 - (time.time() - t0)))
    except subprocess.TimeoutExpired:
        p.kill()
        rc = p.wait()
        print("killed own pid", p.pid)
    k32.CloseHandle(h)
    json.dump({"exe": exe, "root": root, "rc": rc, "samples": samples}, open(pathlib.Path(data) / "probe.json", "w"),
              indent=1)
    print("probe rc=%d samples=%d -> %s" % (rc, len(samples), pathlib.Path(data) / "probe.json"))


# ------------------------------------------------------------------ verdict
def seed_ranges():
    rng = [(a, a + n) for a, n in P.parse_seed()]
    p = ROOT / "mashedmod" / "src" / "mashed_re" / "Match" / "PortTop_RdataSeed.inc"
    if p.exists():
        for m in re.finditer(r"\{\s*0x([0-9a-fA-F]+)u,\s*(\d+),", p.read_text(encoding="utf-8")):
            a = int(m.group(1), 16)
            rng.append((a, a + int(m.group(2))))
    return rng


def covered(rng, a, n):
    need = set(range(a, a + n))
    for lo, hi in rng:
        need -= set(range(max(lo, a), min(hi, a + n)))
        if not need:
            return True
    return not need


def verdict(data):
    rows = json.load(open(pathlib.Path(data) / "ot_rows.json"))
    pr = json.load(open(pathlib.Path(data) / "probe.json"))
    ss = pr["samples"]
    last = ss[-1] if ss else {}
    first = ss[0] if ss else {}
    rng = seed_ranges()
    out = ["rva\tname\tsubsystem\trwg_verdict\trwg_detail\tseed_verdict\tseed_detail"]
    cnt = collections.Counter()
    for k in sorted(rows):
        r = rows[k]
        rv = rd = sv = sd = ""
        if "rwg_call_slots" in r:
            g = last.get("rwg")
            if not g:
                rv, rd = "needs binding", "RwGlobals 0x007d3ff8 = 0 at every sample"
            else:
                sl = last.get("slots") or {}
                fmt = lambda o: ("+0x%x" % o) if isinstance(o, int) else ("+[0x%s]+%s" % tuple(o[1:].split("+")))
                val = lambda o: sl.get(str(o))
                miss = [o for o in r["rwg_call_slots"] if not val(o)]
                zero = [o for o in r["rwg_data_offs"] if not val(o)]
                rd = "RwGlobals=0x%08x; call slots %s; data %s" % (
                    g, " ".join("%s=%s" % (fmt(o), hex(val(o) or 0)) for o in r["rwg_call_slots"]) or "-",
                    " ".join("%s=%s" % (fmt(o), hex(val(o) or 0)) for o in r["rwg_data_offs"]) or "-")
                mods = last.get("mods") or {}
                unreg = sorted({o[1:].split("+")[0] for o in r["rwg_call_slots"] + r["rwg_data_offs"]
                                if isinstance(o, str) and not mods.get(o[1:].split("+")[0])})
                rv = "needs binding" if (miss or zero or unreg) else "OK"
                if unreg:
                    rd += "; module offset 0 (not registered): " + " ".join("[0x%s]" % m for m in unreg)
                if miss:
                    rd += "; null call slot: " + " ".join(fmt(o) for o in miss)
                if zero:
                    rd += "; zero data field: " + " ".join(fmt(o) for o in zero)
            cnt["rwg " + rv] += 1
        if "seed" in r:
            bad, how = [], []
            for s in r["seed"]:
                a, n = int(s["addr"], 16), s["size"]
                if covered(rng, a, n):
                    how.append("%s seeded" % s["addr"])
                    continue
                # any sample: the first samples come before PortStateSeed_Apply (every address reads 0 there)
                seen = [x.get("seed", {}).get(s["addr"]) for x in ss]
                got = seen[-1] if seen else None
                # prefix: one address can be read at two sizes by different rows, and the probe keeps the longer read
                if any(v is not None and v[:len(s["orig"])] == s["orig"] for v in seen):
                    how.append("%s exe holds original bytes" % s["addr"])
                else:
                    bad.append("%s %s(%dB, %s) exe=%s orig=%s" % (s["addr"], s["name"], n, s["sec"],
                                                                   (got or "unreadable")[:16], s["orig"][:16]))
            sv = "needs seed rule" if bad else "OK"
            sd = "; ".join(bad) if bad else "; ".join(how)
            cnt["seed " + sv] += 1
        out.append("\t".join([k, r["name"], r["subsystem"], rv, rd, sv, sd]))
    dst = HERE / "OT_MEASURE.tsv"
    dst.write_text("\n".join(out) + "\n", encoding="utf-8")
    by_sub = collections.Counter((rows[k]["subsystem"], v) for k in rows for v in
                                 ([("rwg:" + l.split("\t")[3])] if False else []))
    print(dict(cnt))
    print("->", dst)


def main(argv):
    data = opt(argv, "--data", "D:/MashedWT/PORT-OT-MEASURE_data")
    if argv and argv[0] == "lists":
        lists(data)
    elif argv and argv[0] == "probe":
        probe(argv, data)
    elif argv and argv[0] == "verdict":
        verdict(data)
    else:
        print(__doc__)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
