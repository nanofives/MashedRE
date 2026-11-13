# First-divergence report of the replay oracle (RO-FIRST-DIVERGENCE, 2026-11-13).
#
# Reads two tick-trace run directories (detorig.py --boot / mashed_re_dev.asi MASHED_REPLAY_ASI on the original,
# MASHED_REPLAY_TRACE on the exe: the same meta.json / ticks.bin / ckptNN.data.bin format) and reports where they first
# differ:
#   1. record level: the first ticks.bin record (tick c) at which a block hash or a raw word differs, with blocks whose
#      whole 1 KB is masked (--masks) or ignored (--ignore) left out. The detorig_ticks.py verdict (IDENTICAL-FOR-N over
#      NEW blocks) is printed beside it, because the two answer different questions: NEW blocks need the block to be
#      equal first, so a block that differs from record 0 never ends IDENTICAL-FOR-N.
#   2. dword level, when both runs carry full-image dumps at tick c - 1 and tick c (detorig.py --ckpt-ticks, exe
#      MASHED_REPLAY_TRACE_CKPT; tick 0 = before the first tick): every dword of the divergent blocks that differs at
#      tick c, both values at c - 1 and c, its class (MASK / RULE row, PTR pair, value), the file initializer of the
#      original's image (.data raw bytes, or BSS = 0), and the static referrers (functions with an absolute-operand
#      reference to the dword, read / write from the instruction, named from the Ghidra facts and hooks.csv).
#      Each dump is re-hashed against its own tick record first (the dump at tick u is taken after record u).
#   3. writers, when a run carries watch.jsonl (detorig.py --watch ... --watch-from install, exe
#      MASHED_REPLAY_TRACE_WATCH): the hits on each reported dword in the frame between record c - 1 and record c
#      (hit tick == c - 1), the last one being the last writer. Original pcs are named from the Ghidra facts, exe pcs
#      from mashed_re.map (--exe-map).
# When the dumps or the watch are missing, the report prints the re-run that produces them (--plan).
#
#   py -3.12 re/frida/detorig_firstdiv.py DIR_REF DIR_RUN [--masks a.tsv,b.tsv] [--exe-map mashed_re.map]
#        [--json OUT] [--md OUT] [--max-dwords 64] [--tick C]
import argparse, bisect, csv, json, struct, sys
from pathlib import Path

import numpy as np

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "re" / "tools"))
from detorig import DATA_LO, is_ptr, is_ptr_pair, load_norm, region_of  # noqa: E402

DEF_MASKS = f"{ROOT / 're/frida/warp_normalize.tsv'},{ROOT / 're/frida/replay_masks.tsv'}"


def load_run(d):
    d = Path(d)
    m = json.loads((d / "meta.json").read_text())
    t = m.get("ticks")
    if not t:
        raise SystemExit(f"{d}: no ticks in meta.json")
    raw = np.fromfile(d / t["file"], dtype="<u4")
    recw = t["recw"]
    n = raw.size // recw
    recs = raw[: n * recw].reshape(n, recw)
    dumps = {}
    exe = m.get("side") == "exe"
    for c in m.get("ckpt") or []:
        # tick-boundary dumps only: exe k != 100 (k 100 is inside the warp), original k >= 200 (detorig.py --ckpt-ticks;
        # its other checkpoints are taken at clock-call counts inside a frame)
        if (c.get("k") == 100) if exe else (c.get("k", 0) < 200):
            continue
        if c.get("tick") is not None and c.get("file") and (d / c["file"]).exists():
            dumps.setdefault(int(c["tick"]), d / c["file"])
    return {"dir": d, "meta": m, "t": t, "recs": recs, "dumps": dumps, "side": m.get("side", "original")}


def block_hashes(words, bw, nblk):
    """FNV-1a per block over the dump's dwords with the heap rule (detorig.js CModule / PortTop_ReplayTrace.cpp)."""
    v = words.astype(np.uint64)
    v = np.where((v >= 0x00995000) & (v < 0x30000000) & ((v & 3) == 0), 0, v).astype(np.uint32)
    pad = nblk * bw - v.size
    m = np.concatenate([v, np.zeros(max(pad, 0), np.uint32)]).reshape(nblk, bw)
    lens = np.full(nblk, bw)
    if pad > 0:
        lens[-1] = bw - pad
    h = np.full(nblk, 2166136261, dtype=np.uint64)
    for i in range(bw):
        act = i < lens
        nh = ((h ^ m[:, i].astype(np.uint64)) * 16777619) & 0xFFFFFFFF
        h = np.where(act, nh, h)
    return h.astype(np.uint32)


def read_dump(p):
    return np.fromfile(p, dtype="<u4")


def verify_dump(run, tick):
    """Dump at tick u against record u (index u - 1). Returns (status, mismatching block indexes)."""
    if tick not in run["dumps"]:
        return "missing", []
    if tick == 0:
        return "no-record (tick 0)", []
    i = tick - 1
    if i >= len(run["recs"]):
        return "no-record", []
    t = run["t"]
    w = read_dump(run["dumps"][tick])
    h = block_hashes(w, t["block"] // 4, t["nblk"])
    rec = run["recs"][i]
    bad = [int(b) for b in np.nonzero(h != rec[2: 2 + t["nblk"]])[0]]
    return ("OK" if not bad else f"MISMATCH {len(bad)} blocks"), bad


class Statics:
    """Static facts about a data dword of MASHED.exe: file initializer, absolute-operand referrers (capstone over .text,
    the scan of re/tools/gen_replay_masks.py, here with read / write access), function names (Ghidra facts, hooks.csv)."""

    def __init__(self):
        import xref
        self.xref = xref
        pe = xref.load_pe()
        self.pe = pe
        self.base = pe.OPTIONAL_HEADER.ImageBase
        self.secs = [(self.base + s.VirtualAddress, s.Misc_VirtualSize, s.SizeOfRawData, s.get_data(), s.Name.rstrip(b"\0").decode())
                     for s in pe.sections]
        facts = Path(xref.FACTS)
        if not facts.exists():
            facts = xref.MAIN / "log" / "ghidra_funcfacts.jsonl"
        self.funcs, self.fname = [], {}
        for line in open(facts):
            r = json.loads(line)
            if "entry" in r:
                e = int(r["entry"], 16)
                self.fname[e] = r.get("name") or ""
                for lo, hi in r["ranges"]:
                    self.funcs.append((int(lo, 16), int(hi, 16), e))
        self.funcs.sort()
        self.starts = [f[0] for f in self.funcs]
        self.hooks = {}
        for r in csv.DictReader(l for l in open(ROOT / "hooks.csv", encoding="utf-8") if not l.startswith("#")):
            try:
                self.hooks[int(r["rva"], 16)] = (r.get("name") or r.get("class_method") or "", r.get("subsystem") or "", r.get("status") or "")
            except (ValueError, KeyError, TypeError):
                pass
        self._refs = None

    def owner(self, va):
        i = bisect.bisect_right(self.starts, va) - 1
        while i >= 0:
            if self.funcs[i][0] <= va <= self.funcs[i][1]:
                return self.funcs[i][2]
            if va - self.funcs[i][0] > 0x40000:
                break
            i -= 1
        return None

    def fn_label(self, e):
        if e is None:
            return "no-function"
        h = self.hooks.get(e)
        nm = (h[0] if h and h[0] else self.fname.get(e, "")) or f"FUN_{e:08x}"
        sub = h[1] if h else "no-hooks-row"
        return f"0x{e:08x} {nm} [{sub}]"

    def pc_label(self, pc):
        return self.fn_label(self.owner(pc))

    def init_value(self, va):
        for lo, vsz, rsz, data, name in self.secs:
            if lo <= va < lo + max(vsz, rsz):
                off = va - lo
                if off + 4 <= rsz:
                    return name, struct.unpack_from("<I", data, off)[0]
                return name + " (bss)", 0
        return "outside", None

    def refs(self):
        if self._refs is not None:
            return self._refs
        cache = ROOT / "log" / "firstdiv_datarefs.json"
        exe = Path(self.xref.EXE)
        key = f"{exe.stat().st_size}:{int(exe.stat().st_mtime)}"
        if cache.exists():
            c = json.loads(cache.read_text())
            if c.get("key") == key:
                self._refs = {int(k): v for k, v in c["refs"].items()}
                return self._refs
        import capstone
        text = next(s for s in self.pe.sections if s.Name.rstrip(b"\0") == b".text")
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        md.detail = True
        md.skipdata = True
        refs = {}
        for ins in md.disasm(text.get_data(), self.base + text.VirtualAddress):
            if ins.id == 0:
                continue
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                    d = op.mem.disp & 0xffffffff
                    if DATA_LO <= d < 0x00915008:
                        acc = "w" if op.access & capstone.CS_AC_WRITE else "r"
                        refs.setdefault(d & ~3, []).append([ins.address, acc, op.size])
        cache.parent.mkdir(parents=True, exist_ok=True)
        cache.write_text(json.dumps({"key": key, "refs": {str(k): v for k, v in refs.items()}}))
        self._refs = refs
        return refs

    def referrers(self, va):
        out = {"w": {}, "r": {}}
        # refs are keyed by disp & ~3, so a byte / word access inside the dword is included. An access that starts
        # in the dword below and spans into this one (a qword / 10-byte operand) is not.
        for pc, acc, size in self.refs().get(va & ~3, []):
            out[acc].setdefault(self.fn_label(self.owner(pc)), []).append(f"0x{pc:08x}")
        return out


class ExeMap:
    def __init__(self, path):
        self.addr, self.name = [], []
        if not path or not Path(path).exists():
            return
        import re
        rx = re.compile(r"^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+([0-9a-fA-F]{8})\s")
        rows = []
        for line in open(path, encoding="latin-1"):
            m = rx.match(line)
            if m:
                rows.append((int(m.group(2), 16), m.group(1)))
        rows.sort()
        self.addr = [r[0] for r in rows]
        self.name = [r[1] for r in rows]
        pref = None
        for line in open(path, encoding="latin-1"):
            if "Preferred load address is" in line:
                pref = int(line.split()[-1], 16)
                break
        rows = [r for r in rows if pref is None or r[0] >= pref]   # absolute symbols (0x0...) left out
        self.lo = rows[0][0] if rows else None
        self.hi = rows[-1][0] if rows else None

    def in_image(self, v):
        """v lies between the exe's load address and its highest public symbol, outside MASHED.exe's own image range
        0x00400000..0x00995000 (the exe keeps the original's globals at their original addresses there)."""
        return self.lo is not None and self.lo <= v <= self.hi and not (0x00400000 <= v < 0x00995000)

    def label(self, pc):
        if not self.addr:
            return "no map"
        i = bisect.bisect_right(self.addr, pc) - 1
        return f"{self.name[i]}+0x{pc - self.addr[i]:x}" if i >= 0 else "below map"


def load_watch(run):
    p = run["dir"] / "watch.jsonl"
    hits = []
    if p.exists():
        for line in open(p):
            line = line.strip()
            if line:
                try:
                    hits.append(json.loads(line))
                except json.JSONDecodeError:
                    pass
    return hits


def attribute(hits, run):
    """[(va or None, hit)]. Exe hits carry their slot (DR6). Original (detorig.js) hits carry no slot: with one watched
    va it is that va; with two, a hit goes to the va whose value (h['values'], read after the store) changed since the
    previous hit, to both when both changed, and to None (same-value write, slot not known) when neither did. The value
    before the first hit is taken from the run's tick-0 dump."""
    w = run["meta"].get("cfg", {}).get("watch") or []
    out = []
    prev = None
    if len(w) > 1 and 0 in run["dumps"]:
        d0 = read_dump(run["dumps"][0])
        prev = [int(d0[(v - DATA_LO) // 4]) for v in w]
    for h in hits:
        if h.get("kind") != "watch":
            continue
        if "va" in h:
            out.append((int(h["va"], 16), h)); continue
        if len(w) == 1:
            out.append((w[0], h)); continue
        vals = [int(x, 16) for x in h.get("values") or []]
        if len(vals) != len(w) or prev is None:
            out.append((None, h)); prev = vals or prev; continue
        ch = [k for k in range(len(w)) if vals[k] != prev[k]]
        for k in ch:
            out.append((w[k], dict(h, value=hex(vals[k]), slot_by="value-change")))
        if not ch:
            out.append((None, dict(h, slot_by="same-value write, slot not known")))
        prev = vals
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("ref", help="reference run (the original)")
    ap.add_argument("run", help="compared run (the exe)")
    ap.add_argument("--masks", default=DEF_MASKS, help="mask / normalisation files, comma-separated ('' = none)")
    ap.add_argument("--ignore", default="", help="LO-HI hex VA ranges left out at block level")
    ap.add_argument("--exe-map", default="", help="mashed_re.map of the compared exe (names exe pcs of watch hits)")
    ap.add_argument("--tick", type=int, default=0, help="report this tick instead of the first divergent one")
    ap.add_argument("--max-dwords", type=int, default=64)
    ap.add_argument("--new-only", action="store_true", help="with --tick: only the blocks that first become NEW at that record")
    ap.add_argument("--no-statics", action="store_true", help="skip the file-initializer / referrer columns")
    ap.add_argument("--json", default="")
    ap.add_argument("--md", default="")
    a = ap.parse_args()

    A, B = load_run(a.ref), load_run(a.run)
    ta, tb = A["t"], B["t"]
    assert ta["recw"] == tb["recw"] and ta["block"] == tb["block"] and ta["nblk"] == tb["nblk"], "different record layout"
    blk, nblk = ta["block"], ta["nblk"]
    n = min(len(A["recs"]), len(B["recs"]))
    ticks_a, ticks_b = A["recs"][:n, 0], B["recs"][:n, 0]
    tick_align = bool(np.all(ticks_a == ticks_b))
    mk = load_norm(a.masks)["mask"] if a.masks else {}
    va0 = lambda j: DATA_LO + j * blk
    ign = [(int(x, 16), int(y, 16)) for x, y in (r.split("-") for r in a.ignore.split(",") if r)]
    full_masked = np.array([all(va0(j) + 4 * w in mk for w in range(blk // 4)) for j in range(nblk)])
    ignored = np.array([any(lo < va0(j) + blk and va0(j) < hi for lo, hi in ign) for j in range(nblk)])
    skip = full_masked | ignored
    hd = A["recs"][:n, 2: 2 + nblk] != B["recs"][:n, 2: 2 + nblk]
    em = ExeMap(a.exe_map)

    def dword_class(va, x, y):
        # IMGPTR: the reference value is an address inside MASHED.exe's image and the run's value an address inside
        # the exe's own image (from --exe-map), or the other way round: each side points into its own code or data,
        # so the two values cannot be equal. Needs the map; without it such a dword is VALUE.
        imgp = (0x00400000 <= x < 0x00995000 and em.in_image(y)) or (0x00400000 <= y < 0x00995000 and em.in_image(x))
        return mk.get(va) or ("PTR" if is_ptr_pair(x, y) else "IMGPTR" if imgp and x != y else "VALUE")

    # A block that differs at every record (detorig_ticks.py CONSTANT) is EXPLAINED, and not counted, when at the
    # earliest tick both runs dumped every dword that differs in it is a MASK / RULE row, a PTR pair or an IMGPTR pair.
    # Run 44: only ticks >= 1 count. A tick-0 dump (boot entry / trace init) has no record, and on the original side it
    # comes before the boot writes, so explaining at tick 0 hid record-0 VALUE dwords (0x00604604 with the image ON).
    always = hd.all(axis=0) & ~skip if n else np.zeros(nblk, bool)
    common = sorted(t for t in set(A["dumps"]) & set(B["dumps"]) if t >= 1)
    explained, expl_tick = [], None
    if common and always.any():
        expl_tick = common[0]
        xa, xb = read_dump(A["dumps"][expl_tick]), read_dump(B["dumps"][expl_tick])
        bw = blk // 4
        for j in np.nonzero(always)[0]:
            lo = int(j) * bw
            ws = np.nonzero(xa[lo: lo + bw] != xb[lo: lo + bw])[0]
            if all(dword_class(DATA_LO + 4 * (lo + int(w)), int(xa[lo + w]), int(xb[lo + w])) != "VALUE" for w in ws):
                explained.append(int(j))
    skip = skip.copy()
    skip[explained] = True
    hd_counted = hd & ~skip[None, :]
    raw_va = []
    for lo, hi in ta.get("raw") or []:
        raw_va += list(range(lo, hi, 4))
    ra, rb = A["recs"][:n, 2 + nblk:], B["recs"][:n, 2 + nblk:]
    raw_cnt = np.zeros(n, dtype=int)
    for k, v in enumerate(raw_va):
        if v in mk:
            continue
        col = np.nonzero(ra[:, k] != rb[:, k])[0]
        for i in col:
            if not is_ptr_pair(int(ra[i, k]), int(rb[i, k])):
                raw_cnt[i] += 1
    rows_div = np.nonzero(hd_counted.any(axis=1) | (raw_cnt > 0))[0]
    first_rec = int(rows_div[0]) if rows_div.size else None
    # detorig_ticks.py's NEW-block verdict, for reference
    seen_eq = np.zeros(nblk, bool)
    first_new = {}
    for i in range(n):
        new = np.nonzero(hd[i] & seen_eq & ~skip)[0]
        for j in new:
            first_new.setdefault(int(j), i)
        seen_eq |= ~hd[i]
    ident = min(first_new.values()) if first_new else n

    rep = {"ref": str(A["dir"]), "run": str(B["dir"]), "sides": [A["side"], B["side"]], "records": n, "block": blk,
           "tick_counters_aligned": tick_align, "masks": a.masks, "ignore": a.ignore,
           "blocks_fully_masked": int(full_masked.sum()),
           "constant_blocks_explained": [hex(va0(j)) for j in explained], "explained_at_tick": expl_tick,
           "identical_for_n_new_blocks": ident,
           "first_new_block": (lambda j: [hex(va0(j)), first_new[j]])(min(first_new, key=lambda j: (first_new[j], j))) if first_new else None,
           "first_divergent_record": first_rec}
    out = []
    P = out.append
    P(f"# First divergence: {A['dir'].name} ({A['side']}) vs {B['dir'].name} ({B['side']})")
    P("")
    P(f"- records compared: {n}, block {blk} B, {nblk} blocks, {len(raw_va)} raw words; tick counters aligned: {tick_align}")
    P(f"- masks: {a.masks or 'none'} ({int(full_masked.sum())} blocks wholly masked); ignore: {a.ignore or 'none'}")
    P(f"- blocks differing at every record: {int(always.sum())}; explained by mask / rule / PTR / IMGPTR dwords at tick "
      f"{expl_tick} (both runs dumped): {len(explained)} {[hex(va0(j)) for j in explained[:8]]}")
    P(f"- detorig_ticks.py verdict (NEW blocks): IDENTICAL-FOR-N {ident} of {n}"
      + (f", first NEW block {rep['first_new_block'][0]} at record {rep['first_new_block'][1]}" if first_new else ""))
    if first_rec is None and not a.tick:
        P(f"- **no divergence**: every counted block and raw word is equal at all {n} records")
        rep["verdict"] = f"IDENTICAL {n}"
        return finish(a, rep, out)
    r = (a.tick - 1) if a.tick else first_rec
    c = int(A["recs"][r, 0])
    blocks = [int(j) for j in np.nonzero(hd_counted[r])[0]]
    if a.new_only:   # only blocks equal at some earlier record (the detorig_ticks.py NEW sense)
        blocks = [j for j in blocks if first_new.get(j) == r]
    rep.update({"record": r, "tick": c, "tick_run": int(B["recs"][r, 0]), "blocks_at_record": [hex(va0(j)) for j in blocks],
                "raw_words_at_record": int(raw_cnt[r]),
                "blocks_differing_per_record_first16": [int(x) for x in hd_counted.sum(axis=1)[:16]]})
    P(f"- **first divergent record {first_rec}** (tick {int(A['recs'][first_rec, 0])})" if first_rec is not None else "- no divergent record")
    if a.tick:
        P(f"- reporting tick {c} (record {r}) as asked")
    P(f"- at record {r} (tick {c}): {len(blocks)} counted blocks differ, {int(raw_cnt[r])} raw words (non-PTR, unmasked)")
    P(f"- counted blocks differing at records 0..15: {rep['blocks_differing_per_record_first16']}")
    P("")
    # dumps
    st = {}
    for X, nm in ((A, "ref"), (B, "run")):
        for u in (c - 1, c):
            st[(nm, u)] = verify_dump(X, u) if u >= 0 else ("n/a", [])
    rep["dumps"] = {f"{nm}@{u}": s[0] for (nm, u), s in st.items()}
    P("## Dumps")
    for (nm, u), s in st.items():
        X = A if nm == "ref" else B
        P(f"- {nm} tick {u}: {s[0]}" + (f" ({X['dumps'][u].name})" if u in X["dumps"] else "")
          + (f", blocks whose dump differs from the record: {[hex(va0(j)) for j in s[1][:12]]}" if s[1] else ""))
    rep["dump_record_mismatch"] = {f"{nm}@{u}": [hex(va0(j)) for j in s[1]] for (nm, u), s in st.items() if s[1]}
    have = all(u in X["dumps"] for X in (A, B) for u in (c - 1, c) if u >= 0)
    plan = []
    if not have:
        plan.append(f"ref (original): detorig.py --boot ... --ckpt-ticks {max(c - 1, 0)},{c}")
        plan.append(f"run (exe): MASHED_REPLAY_TRACE_CKPT={max(c - 1, 0)},{c}")
    S = None if a.no_statics else Statics()
    if have:
        da = {u: read_dump(A["dumps"][u]) for u in (c - 1, c) if u >= 0}
        db = {u: read_dump(B["dumps"][u]) for u in (c - 1, c) if u >= 0}
        dw = []
        for j in blocks:
            lo = j * (blk // 4)
            for w in range(lo, min(lo + blk // 4, da[c].size)):
                x, y = int(da[c][w]), int(db[c][w])
                if x == y:
                    continue
                va = DATA_LO + 4 * w
                cls = dword_class(va, x, y)
                dw.append((va, cls, w, x, y))
        # raw words of the record that are not in a counted block
        cnt = {}
        for _, cls, *_ in dw:
            cnt[cls] = cnt.get(cls, 0) + 1
        rep["dword_classes"] = cnt
        P("")
        P(f"## Dwords differing at tick {c} in the {len(blocks)} blocks: {len(dw)} ({', '.join(f'{k} {v}' for k, v in sorted(cnt.items()))})")
        val = [d for d in dw if d[1] == "VALUE"]
        rep["value_dwords"] = len(val)
        P("")
        P(f"| va | ref t{c - 1} | ref t{c} | run t{c - 1} | run t{c} | new at t{c} | file init | static writers | region |")
        P("|---|---|---|---|---|---|---|---|---|")
        rows = []
        for va, cls, w, x, y in val[: a.max_dwords]:
            pa = int(da[c - 1][w]) if c - 1 in da else None
            pb = int(db[c - 1][w]) if c - 1 in db else None
            new = (pa == pb) if pa is not None and pb is not None else None
            sec, iv = S.init_value(va) if S else ("", None)
            wr = S.referrers(va)["w"] if S else {}
            rd = S.referrers(va)["r"] if S else {}
            row = {"va": hex(va), "ref_prev": pa, "ref": x, "run_prev": pb, "run": y, "new_this_tick": new,
                   "section": sec, "file_init": iv, "static_writers": wr, "static_readers": rd, "region": region_of(va)}
            rows.append(row)
            f8 = lambda v: "-" if v is None else f"{v:08x}"
            wl = "; ".join(f"{k} @{','.join(v[:3])}" for k, v in list(wr.items())[:3]) or "none (abs)"
            P(f"| 0x{va:08x} | {f8(pa)} | {f8(x)} | {f8(pb)} | {f8(y)} | {new} | {sec} {f8(iv)} | {wl} | {region_of(va)} |")
        if len(val) > a.max_dwords:
            P(f"| ... {len(val) - a.max_dwords} more VALUE dwords (--max-dwords) | | | | | | | | |")
        rep["dwords"] = rows
        # census by init: which side kept the file initializer
        if S:
            # FILEINIT-MISSING: the reference holds the file initializer of MASHED.exe and the run does not (in the exe:
            # its zero-filled image pad, exe_main.cpp B17). BOOT-WRITE: the run holds the initializer and the reference
            # does not (written by the original between its load and tick c). OTHER: neither holds it.
            cat = {}
            for va, cls, w, x, y in val:
                sec, iv = S.init_value(va)
                k = "FILEINIT-MISSING" if x == iv else "BOOT-WRITE" if y == iv else "OTHER"
                k2 = (k, sec)
                e = cat.setdefault(k2, {"n": 0, "first": va, "run_zero": 0})
                e["n"] += 1
                e["run_zero"] += (y == 0)
            rep["value_census"] = [{"cat": k, "section": s, "dwords": e["n"], "first": hex(e["first"]), "run_zero": e["run_zero"]}
                                   for (k, s), e in sorted(cat.items())]
            P("")
            P("| category | section | dwords | run value 0 | first va |")
            P("|---|---|---|---|---|")
            for r_ in rep["value_census"]:
                P(f"| {r_['cat']} | {r_['section']} | {r_['dwords']} | {r_['run_zero']} | {r_['first']} |")
        first_val = val[0][0] if val else None
        rep["first_value_dword"] = hex(first_val) if first_val else None
    # writers
    wa, wb = load_watch(A), load_watch(B)
    P("")
    P("## Writers (watch.jsonl)")
    targets = [int(x["va"], 16) for x in rep.get("dwords", [])[:4]]
    if not wa and not wb:
        P("- no watch.jsonl in either run")
        if targets or blocks:
            vs = ",".join(hex(t) for t in (targets[:2] or [va0(blocks[0])]))
            plan.append(f"ref (original): detorig.py --boot ... --watch {vs} --watch-from install")
            plan.append(f"run (exe): MASHED_REPLAY_TRACE_WATCH={vs}")
    wrep = {}
    for X, hits, nm in ((A, wa, "ref"), (B, wb, "run")):
        for v, h in attribute(hits, X):
            wrep.setdefault((nm, v), []).append(h)
        # a watched va with no hit at all is reported too (no writer from the arming on)
        for v in (X["meta"].get("cfg", {}).get("watch") or []) + [int(x, 16) for x in (X["meta"].get("watch") or {}).get("va", [])]:
            wrep.setdefault((nm, v), [])
    rep["writers"] = {}
    for (nm, v), hs in sorted(wrep.items(), key=lambda kv: (kv[0][1] or 0, kv[0][0])):
        side_exe = (A if nm == "ref" else B)["side"] == "exe"
        lab = (lambda pc: em.label(pc)) if side_exe else (lambda pc: S.pc_label(pc) if S else hex(pc))
        # The frame between record r - 1 and record r (tick c = r + 1): with the hit's record count (field rec: records
        # written before the store, exe and detorig.js since RO-FIRST-DIVERGENCE) it is rec == r, exact. Without it the
        # tick counter at the store stands in (tick == c - 1); a store at the tick boundary before record r then counts
        # in the next frame (the RONEG self-test's planted +1 at 0x007f0ff0 is one).
        if any(h.get("rec") is not None for h in hs):
            frame = [h for h in hs if h.get("rec") is not None and int(h["rec"]) == r]
            upto = [h for h in hs if h.get("rec") is not None and int(h["rec"]) <= r]
            rule = "rec == %d" % r
        else:
            frame = [h for h in hs if h.get("tick") is not None and int(h["tick"]) == c - 1]
            upto = [h for h in hs if h.get("tick") is not None and int(h["tick"]) <= c - 1]
            rule = "tick == %d" % (c - 1)
        last = (frame or upto or [None])[-1]
        key = f"{nm}:{hex(v) if v is not None else '?'}"
        rep["writers"][key] = {"hits": len(hs), "in_frame": len(frame), "frame_rule": rule, "last": last,
                               "last_label": lab(int(last["pc"], 16)) if last else None,
                               "frame_pcs": sorted({h["pc"] for h in frame})}
        P(f"- {key}: {len(hs)} hits, {len(frame)} in the frame tick {c - 1} -> {c} ({rule}); last writer "
          + (f"pc {last['pc']} ({lab(int(last['pc'], 16))}) at tick {last.get('tick')} value {last.get('value', (last.get('values') or ['?'])[0])}"
             + (f", bt {last.get('bt', '')[:120]}" if last.get("bt") else "") if last else "none at or before that frame"))
    if plan:
        P("")
        P("## Re-run plan (missing evidence)")
        for p in plan:
            P(f"- {p}")
    rep["plan"] = plan
    return finish(a, rep, out)


def finish(a, rep, out):
    txt = "\n".join(out) + "\n"
    sys.stdout.write(txt)
    if a.md:
        Path(a.md).write_text(txt, encoding="utf-8")
    if a.json:
        Path(a.json).write_text(json.dumps(rep, indent=1, default=str))
    return 0


if __name__ == "__main__":
    sys.exit(main())
