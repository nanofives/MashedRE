"""DDKOBJ bulk sizing (DDKOBJ-SPIKE part E).

    py -3.12 re/tools/ddkobj/size_bulk.py <rows.json> <all_build_dir> <out_dir> [--link]

rows.json: {"<rva hex>": [source files]} (the closed-set tool's runnable rows, or the BINDINGS `matched` rows).
For every row: compile its re/match source with the DDK, read its undefined symbols and classify each:
  in-set        another row of this set (or a unit of the all-units variant)
  data          an MGLOBAL / name-suffix address (absolute symbol)
  bind:<kind>   a FUN_x with a BINDINGS.tsv row whose exe symbol the default link defines (kind = matched/bound/...)
  bind-missing  a FUN_x with a BINDINGS row whose exe symbol the default link does not define
  exe-name      a non-FUN name the default link defines (RW names, CRT-free helpers)
  crt           a CRT symbol present in the VS2022 CRT (value caveat: see the sin/cos probe)
  crt-absent    a CRT-looking symbol (leading `_`/`__`) absent from the VS2022 CRT
  unbound       a FUN_x with no row in the set, no BINDINGS row (C0..C3 game code or an unbound library family)
  unknown       anything else
Then (--link) the greatest linkable subset: a row links when every undefined symbol is in-set (and that row links),
data, bind:*, exe-name or crt. Rows that already have an exe wrapper keep the all-units variant's substitution; the
rest are added as wrapperless DDK objects with their MATCH functions renamed `DDK_<rva>` (nothing in the exe calls
them, so this is a link-closure test, not a runtime one). Output: SIZE_BULK.tsv (one row per row) + summary json.
"""
import collections
import concurrent.futures as cf
import csv
import json
import pathlib
import re
import shutil
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import coffedit as CE  # noqa: E402
import ddkobj as D     # noqa: E402

M = D.M
ROOT = D.ROOT


def match_index():
    idx = collections.defaultdict(list)
    for f in sorted((ROOT / "re" / "match").rglob("*.c")):
        try:
            t = f.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for a, _n in D.MATCH_RE.findall(t):
            idx[int(a, 16)].append(f)
    return idx


def main():
    rows = json.loads(pathlib.Path(sys.argv[1]).read_text())
    allb = pathlib.Path(sys.argv[2]).resolve()
    out = pathlib.Path(sys.argv[3]).resolve()
    do_link = "--link" in sys.argv
    objdir = out / "obj"
    if objdir.exists():
        shutil.rmtree(objdir)
    objdir.mkdir(parents=True)
    B = D.load_bindings()
    crt = D.vs_crt_symbols()
    dsyms = D.default_link_symbols()
    dplain = {CE.undecorate(s) for s in dsyms}
    allrep = json.loads((allb / "ddkobj_build.json").read_text())
    wrapped = {}          # rva -> exe name of the all-units variant (decorated)
    for u in allrep["units"]:
        if u["status"] == "ok":
            for d in u["defs"]:
                wrapped[int(d["rva"], 16)] = d["exe"]
    idx = match_index()
    gseen = collections.defaultdict(set)
    for _w, _r, incs in D.exe_wrappers():
        for _s, dd in incs:
            for k, v in dd.items():
                gseen[k].add(v)
    gdefs = {k: next(iter(v)) for k, v in gseen.items() if len(v) == 1 and not next(iter(v)).startswith("@")}
    rvas = sorted(int(k, 16) for k in rows)
    rset = set(rvas)

    def pick_src(a):
        fs = [ROOT / f for f in rows.get("%08x" % a, [])] or idx.get(a, [])
        fs = [pathlib.Path(f) for f in fs if pathlib.Path(f).exists()]
        fs.sort(key=lambda p: ("raw" in p.parts, len(str(p))))
        return fs[0] if fs else None

    def comp(a):
        src = pick_src(a)
        if src is None:
            return a, None, None, "no-source"
        info = D.parse_source(src)
        if info["lang"] != "c":
            return a, src, info, "lang-" + info["lang"]
        r = M.compile_obj(src, info["flags"], info["lang"], info["toolchain"], errors_ok=True)
        if r[0] is None:
            return a, src, info, "ddk-compile-fail"
        dst = objdir / f"bulk_{a:08x}.obj"
        shutil.copyfile(r[0], dst)
        return a, src, info, str(dst)

    with cf.ThreadPoolExecutor(8) as ex:
        res = list(ex.map(comp, rvas))

    recs = {}
    for a, src, info, st in res:
        rec = {"rva": a, "src": str(src.relative_to(ROOT)).replace("\\", "/") if src else "", "status": "ok",
               "undef": [], "wrapped": a in wrapped}
        if not st or not st.endswith(".obj"):
            rec["status"] = st
            recs[a] = rec
            continue
        rec["obj"] = st
        co = CE.Coff.load(st)
        mp = {CE.undecorate(n): r for r, n in info["matches"]}
        defined = {CE.undecorate(s.name) for s in co.syms if s.sec > 0}
        for s in co.syms:
            if s.cls != CE.IMAGE_SYM_CLASS_EXTERNAL or s.sec != 0 or s.name.startswith((".", "@feat", "@comp")):
                continue
            plain = CE.undecorate(s.name)
            if plain in defined:
                continue
            fm = re.match(r"FUN_([0-9a-fA-F]{8})$", plain)
            addr = info["globals"].get(plain)
            if addr is None and not s.is_func:
                mm = re.search(r"_([0-9a-fA-F]{8})$", plain)
                if mm and 0x00400000 <= int(mm.group(1), 16) < 0x00a00000:
                    addr = int(mm.group(1), 16)
            if addr is not None and not s.is_func:
                cls, tgt = "data", addr
            elif fm and int(fm.group(1), 16) in rset:
                cls, tgt = "in-set", int(fm.group(1), 16)
            elif fm and int(fm.group(1), 16) in wrapped:
                cls, tgt = "in-variant", wrapped[int(fm.group(1), 16)]
            elif fm and int(fm.group(1), 16) in B:
                b = B[int(fm.group(1), 16)]
                es = (b.get("exe_symbol") or "").strip()
                cls = ("bind:" + b["kind"]) if es and es in dplain else "bind-missing:" + b["kind"]
                tgt = es
            elif fm:
                cls, tgt = "unbound", None
            elif s.name in crt:
                cls, tgt = ("crt-math" if re.match(r"_+(CI)?(sin|cos|tan|atan2?|asin|acos|sqrt|pow|exp|log|log10|fmod|floor|ceil)$", s.name)
                            else "crt"), None
            elif plain in dplain or s.name in dsyms:
                cls, tgt = "exe-name", None
            elif plain in gdefs and gdefs[plain] in dplain:
                cls, tgt = "wrapper-name", gdefs[plain]
            elif s.name.startswith("_") and (s.name.startswith("__") or plain.startswith("_")):
                cls, tgt = "crt-absent", None
            else:
                cls, tgt = "unknown", None
            rec["undef"].append({"name": s.name, "plain": plain, "class": cls, "tgt": tgt})
        recs[a] = rec

    # greatest linkable subset
    ok = {a for a, r in recs.items() if r["status"] == "ok" and
          all(u["class"] in ("in-set", "in-variant", "wrapper-name", "data", "exe-name", "crt", "crt-math") or u["class"].startswith("bind:")
              for u in r["undef"])}
    changed = True
    while changed:
        changed = False
        for a in list(ok):
            if any(u["class"] == "in-set" and u["tgt"] not in ok for u in recs[a]["undef"]):
                ok.discard(a)
                changed = True
    for a, r in recs.items():
        if r["status"] != "ok":
            r["verdict"] = "fail:" + r["status"]
        elif a in ok:
            r["verdict"] = "linkable"
        else:
            bad = [u["class"] for u in r["undef"] if not (u["class"] in ("in-variant", "wrapper-name", "data", "exe-name", "crt", "crt-math")
                                                          or u["class"].startswith("bind:")
                                                          or (u["class"] == "in-set" and u["tgt"] in ok))]
            r["verdict"] = "blocked:" + ",".join(sorted(set(c.split(":")[0] if c.startswith("bind-missing") else c
                                                            for c in bad)))
    summary = {"rows": len(recs), "verdicts": collections.Counter(r["verdict"].split(":")[0] for r in recs.values()),
               "block_classes": collections.Counter(), "undef_classes": collections.Counter(),
               "top_unbound": collections.Counter(), "top_crt_absent": collections.Counter(),
               "top_unknown": collections.Counter(), "top_bind_missing": collections.Counter(),
               "crt_math": collections.Counter()}
    for r in recs.values():
        if r["verdict"].startswith("blocked:"):
            for c in r["verdict"][8:].split(","):
                summary["block_classes"][c] += 1
        for u in r["undef"]:
            summary["undef_classes"][u["class"].split(":")[0] if not u["class"].startswith("bind") else u["class"]] += 1
            if u["class"] == "unbound":
                summary["top_unbound"][u["plain"]] += 1
            elif u["class"] == "crt-absent":
                summary["top_crt_absent"][u["name"]] += 1
            elif u["class"] == "unknown":
                summary["top_unknown"][u["name"]] += 1
            elif u["class"].startswith("bind-missing"):
                summary["top_bind_missing"][u["plain"] + " " + u["class"]] += 1
            elif u["class"] == "crt-math":
                summary["crt_math"][u["name"]] += 1

    # link: the all-units variant + every linkable wrapperless row
    link = {"attempted": False}
    if do_link:
        add = sorted(a for a in ok if a not in wrapped)
        abs2, thk = {}, {}
        objs = []
        bysrc = collections.OrderedDict()
        for a in add:
            bysrc.setdefault(recs[a]["src"], []).append(a)   # one object per source file (several MATCH functions)
        for srcrel, group in bysrc.items():
            a = group[0]
            r = recs[a]
            co = CE.Coff.load(r["obj"])
            info = D.parse_source(ROOT / r["src"])
            mp = {CE.undecorate(n): rv for rv, n in info["matches"]}
            ucls = {u["name"]: u for u in r["undef"]}
            keep = set()
            for s in co.syms:
                if s.sec > 0 and s.is_func and s.cls in (2, 3):
                    p = CE.undecorate(s.name)
                    if p in mp and mp[p] in ok and mp[p] not in wrapped and recs[mp[p]]["src"] == srcrel:
                        s.name = CE.redecorate(s.name, f"DDK_{mp[p]:08x}")
                        s.cls = 2
                        keep.add(s.sec)
                    elif s.cls == 2:
                        s.name += f"__bulk{a:08x}"
            reach, work = set(keep), list(keep)
            while work:
                for _o, si, _t in co.section(work.pop()).relocs:
                    t = co.slots[si]
                    if t is not None and t.sec > 0 and t.sec not in reach:
                        reach.add(t.sec)
                        work.append(t.sec)
            used = {si for sn in reach for _o, si, _t in co.section(sn).relocs}
            for s in co.syms:
                if s.sec != 0 or s.cls != 2 or s.name.startswith((".", "@feat", "@comp")):
                    continue
                if s.index not in used:
                    s.name = "__ddk_dropped"
                    continue
                if s.name not in ucls:
                    continue
                u = ucls[s.name]
                if u["class"] == "data":
                    nm = f"__ddkabs2_{u['tgt']:08x}"
                    abs2[nm] = u["tgt"]
                    s.name = nm
                elif u["class"] == "in-set":
                    t = u["tgt"]
                    s.name = wrapped.get(t) or CE.redecorate(s.name, f"DDK_{t:08x}")
                elif u["class"] == "in-variant":
                    s.name = u["tgt"]
                elif u["class"].startswith("bind:") or u["class"] == "wrapper-name":
                    s.name = CE.redecorate(s.name, u["tgt"])
            dst = objdir / f"link_{a:08x}.obj"
            co.save(dst)
            objs.append(str(dst))
        a2 = objdir / "bulk_abs.obj"
        CE.build_absolute_obj(a2, abs2)
        lines = [l for l in (allb / "link_ddk.rsp").read_text().splitlines() if l.strip()]
        lines += ['"' + o + '"' for o in objs] + ['"' + str(a2) + '"']
        (out / "link_ddk.rsp").write_text("\n".join(lines) + "\n")
        rc = D.do_link(out)
        log = (out / "link_ddk.log").read_text(errors="replace")
        failed_objs = collections.Counter(re.findall(r"(link_[0-9a-f]{8})\.obj : error LNK\d+", log))
        link = {"attempted": True, "added_wrapperless": len(add), "rc": rc,
                "lnk_errors": len(re.findall(r"error LNK\d+", log)), "objects_with_errors": len(failed_objs),
                "top_errors": re.findall(r"error (LNK\d+: .{0,120})", log)[:15]}
    summary["link"] = link
    with open(out / "SIZE_BULK.tsv", "w", newline="", encoding="utf-8") as fh:
        w = csv.writer(fh, delimiter="\t")
        w.writerow(["rva", "src", "wrapped_in_exe", "verdict", "n_undef", "classes"])
        for a in sorted(recs):
            r = recs[a]
            cc = collections.Counter(u["class"] for u in r["undef"])
            w.writerow([f"0x{a:08x}", r["src"], int(r["wrapped"]), r["verdict"], len(r["undef"]),
                        " ".join(f"{k}={v}" for k, v in sorted(cc.items()))])
    js = {k: (dict(v.most_common(25)) if isinstance(v, collections.Counter) else v) for k, v in summary.items()}
    (out / "SIZE_BULK_summary.json").write_text(json.dumps(js, indent=1))
    print(json.dumps(js, indent=1)[:6000])


if __name__ == "__main__":
    main()
