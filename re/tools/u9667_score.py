# U-9667 offline scorer: run the original 0x0042ac90 (MenuEntryGet) body and the standalone's
# ItemActionCode (mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp:432) over the ORIGINAL's live
# menu tables (re/analysis/u9667_menu_tables/dump_*.json, from re/frida/u9667_menu_table_dump.py).
#
# (a) orig_meg     - transcription of the bytes of 0x0042ac90 (disasm of original/MASHED.exe.unpatched):
#       0x0042aca5 xor eax,eax / 0x0042aca7 cmp ecx,-1 / je 0x0042acd2      cursor -1 skips the item walk
#       0x0042acac inc ecx                                                  cursor+1 passes
#       0x0042acb0 cmp [edx+eax*4],0xff040000 / je 0x0042accd              word at eax already T4?
#       0x0042acc0 mov esi,[edx+eax*4+4] / inc eax / cmp esi,0xff040000 / jne 0x0042acc0
#       0x0042accd inc eax / 0x0042acce dec ecx / jne 0x0042acb0           (loop while ecx != 0)
#       0x0042acd2 mov ecx,[edx+eax*4] / cmp ecx,0xff050000 / je 0x0042acfd
#       0x0042ace0 cmp ecx,0xff140000 / je / 0x0042ace8 cmp ecx,0xff060000 / je 0x0042acfd
#       0x0042acf0 mov ecx,[edx+eax*4+4] / inc eax / cmp ecx,0xff050000 / jne 0x0042ace0
#       0x0042acfd cmp [edx+eax*4],0xff060000 / jne 0x0042ad0a / or eax,-1 / ret
#       0x0042ad0a mov eax,[edx+eax*4+4] / ret
# (b) item_action_code - transcription of ItemActionCode (MenuNavSM.cpp:432-457).
#
# The bytes are cross-checked against the 16 vectors of re/frida/negctl_fixtures/z_fix_u9660.py,
# whose run_diff fingerprints (FNV-1a 32 over the 4 LE bytes of the return) are in
# re/analysis/negctl/reearn_20261011/evidence/fix_diff_menu_entry_get.csv.
#
# Usage: py -3.12 re/tools/u9667_score.py [--out re/analysis/u9667_menu_tables/score.json]
import argparse, csv, json, re, struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
OUTDIR = ROOT / "re" / "analysis" / "u9667_menu_tables"
M = 0xFFFFFFFF
T4, T5, T6, T7, T14 = 0xFF040000, 0xFF050000, 0xFF060000, 0xFF070000, 0xFF140000


class OOB(Exception):
    pass


def W(words, i):
    if i < 0 or i >= len(words):
        raise OOB(i)
    return words[i]


def orig_meg(words, cursor):
    """Returns (ret, trace). trace: max word index read."""
    rd = []
    def w(i):
        rd.append(i); return W(words, i)
    eax = 0
    if (cursor & M) != M:
        ecx = (cursor + 1) & M
        while True:
            if w(eax) != T4:
                while True:
                    esi = w(eax + 1); eax += 1
                    if esi == T4:
                        break
            eax += 1
            ecx = (ecx - 1) & M
            if ecx == 0:
                break
    ecx = w(eax)
    if ecx != T5:
        while True:
            if ecx == T14 or ecx == T6:
                break
            ecx = w(eax + 1); eax += 1
            if ecx == T5:
                break
    if w(eax) == T6:
        return M, max(rd)
    return w(eax + 1), max(rd)


def item_action_code(words, item_index):
    """Returns (ret, how). how in {'neg-index', 'T7', 'T6', 'key', ...}."""
    if item_index < 0:
        return M, "neg-index"
    idx, item, in_target = 0, -1, False
    while True:
        k = W(words, idx)
        if k == T7:
            return M, "T7"
        if k == T4:
            item += 1
            in_target = (item == item_index)
            idx += 2
            continue
        if in_target:
            if k == T6:
                return M, "T6"
            if k in (T5, T14):
                return W(words, idx + 1), "key"
            idx += 2
            continue
        idx += 1


def item_action_code_wordwise(words, item_index):
    """Ablation for cause (2): ItemActionCode with one-word steps everywhere."""
    if item_index < 0:
        return M, "neg-index"
    idx, item, in_target = 0, -1, False
    while True:
        k = W(words, idx)
        if k == T7:
            return M, "T7"
        if k == T4:
            item += 1
            in_target = (item == item_index)
            idx += 1
            continue
        if in_target:
            if k == T6:
                return M, "T6"
            if k in (T5, T14):
                return W(words, idx + 1), "key"
        idx += 1


def classify(words, cursor, o, s, s_how):
    if o == s:
        return None
    if cursor == -1:
        return "1"
    if s_how == "T7":
        return "3"
    try:
        sw, _ = item_action_code_wordwise(words, cursor)
        if sw == o:
            return "2"
    except OOB:
        pass
    return "other"


def fnv1a(v):
    h = 0x811C9DC5
    for b in struct.pack("<I", v & M):
        h = ((h ^ b) * 0x01000193) & M
    return h


def selftest():
    """Rebuild the 16 z_fix_u9660 vectors and check orig_meg against the run_diff fingerprints."""
    src = (ROOT / "re/frida/negctl_fixtures/z_fix_u9660.py").read_text(encoding="utf-8")
    ns = {"FIXES": {}}
    exec(compile(src, "z_fix_u9660.py", "exec"), ns)
    tests = ns["_TESTS"]
    fp = {}
    with open(ROOT / "re/analysis/negctl/reearn_20261011/evidence/fix_diff_menu_entry_get.csv") as f:
        for r in csv.DictReader(f):
            fp[int(r["idx"])] = int(r["original_bits"], 16)
    res = []
    for i, t in enumerate(tests):
        seed = t["seed"]
        slot = next(x["val"] for x in seed if x["addr"] == 0x0067E9F8)
        cur = next(x["val"] for x in seed if x["addr"] == 0x0067ED40 + slot * 0x40)
        cur = cur - (1 << 32) if cur & 0x80000000 else cur
        base = slot * 0x100
        mem = [0xA5A5A5A5] * 64       # scratch_fill 0xA5
        for x in seed:
            a = x["addr"]
            if isinstance(a, dict) and base <= a["scratch"] < base + 0x100:
                mem[(a["scratch"] - base) // 4] = x["val"]
        try:
            v, _ = orig_meg(mem, cur)
            ok = fnv1a(v) == fp[i]
        except OOB:
            v, ok = None, False
        res.append({"idx": i, "cursor": cur, "ret": v if v is None else hex(v), "fp_match": ok})
    return res


def standalone_tables():
    src = (ROOT / "mashedmod/src/mashed_re/Frontend/MenuNavSM.cpp").read_text(encoding="utf-8")
    out = {}
    for m in re.finditer(r"const std::uint32_t kT(\d+)\[\] = \{([^}]*)\}", src):
        out[int(m.group(1))] = [int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]+)u", m.group(2))]
    return out


def file_tables():
    import pefile
    pe = pefile.PE(str(ROOT / "original/MASHED.exe.unpatched"), fast_load=True)
    img = pe.get_memory_mapped_image()
    ptrs = struct.unpack("<34I", img[0x1F7638:0x1F7638 + 34 * 4])     # PTR_005f7638
    tabs = {}
    for sid, p in enumerate(ptrs):
        if not p:
            continue
        o, ws = p - 0x400000, []
        while len(ws) < 512:
            w = struct.unpack("<I", img[o + 4 * len(ws):o + 4 * len(ws) + 4])[0]
            ws.append(w)
            if w == T7:
                break
        tabs[sid] = (p, ws)
    return ptrs, tabs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(OUTDIR / "score.json"))
    a = ap.parse_args()

    st = selftest()
    st_ok = sum(r["fp_match"] for r in st)
    print(f"selftest: orig_meg vs z_fix_u9660 run_diff fingerprints {st_ok}/{len(st)}")

    # live tables, keyed by pointer; live cursors seen per pointer; live 0x0042ac90 calls
    live, screens, cursors, calls, reached = {}, {}, {}, [], {}
    ptrs_live = None
    for f in sorted(OUTDIR.glob("dump_*.json")):
        r = json.loads(f.read_text(encoding="utf-8"))
        ptrs_live = r.get("ptrs_live", ptrs_live)
        for d in r["dumps"]:
            for s in d["slots"]:
                t = s["table"]
                if not t or not s["table_ptr"]:
                    continue
                p = s["table_ptr"]
                ws = t["words"] + t["tail"]
                if p in live and live[p]["words"] != t["words"]:
                    print(f"WARN table {hex(p)} words differ between dumps ({f.name})")
                live.setdefault(p, {"words": t["words"], "tail": t["tail"], "term_index": t["term_index"],
                                    "cap_hit": t["cap_hit"]})
                if s["slot"] == d["depth"]:
                    cursors.setdefault(p, set()).add(s["cursor"])
                    reached.setdefault(p, set()).add(f"{r['lane']}:{f.stem}")
            if d.get("screen") is not None:
                top = d["slots"][d["depth"]]
                screens[top["table_ptr"]] = d["screen"]
        for c in r.get("meg_calls", []):
            c = dict(c); c["src"] = f.stem; calls.append(c)

    fptrs, ftabs = file_tables()
    ptr2sid = {p: sid for sid, p in enumerate(fptrs) if p}
    sa = standalone_tables()

    rows, totals = [], {"1": 0, "2": 0, "3": 0, "other": 0, "agree": 0, "oob": 0}
    fire = []
    for p in sorted(live, key=lambda q: ptr2sid.get(q, 99)):
        L = live[p]
        words = L["words"] + L["tail"]   # tail lets a walk that ignores 0xff070000 continue
        sid = ptr2sid.get(p)
        n_items = sum(1 for w in L["words"] if w == T4)
        tested = sorted(set(range(n_items)) | {-1} | cursors.get(p, set()))
        cnt = {"1": 0, "2": 0, "3": 0, "other": 0, "agree": 0, "oob": 0}
        per = []
        for c in tested:
            try:
                o, maxrd = orig_meg(words, c)
                past_t7 = maxrd > L["term_index"] >= 0
            except OOB:
                cnt["oob"] += 1; per.append({"cursor": c, "orig": "OOB"}); continue
            s, how = item_action_code(L["words"], c)
            k = classify(L["words"], c, o, s, how)
            cnt[k or "agree"] += 1
            e = {"cursor": c, "orig": hex(o), "iac": hex(s), "iac_how": how, "cause": k,
                 "orig_read_past_T7": past_t7, "live_cursor": c in cursors.get(p, set())}
            per.append(e)
            if k:
                fire.append({"screen": sid, "ptr": hex(p), **e})
        for k2 in cnt:
            totals[k2] += cnt[k2]
        rows.append({
            "screen": sid, "ptr": hex(p), "words_to_T7": len(L["words"]), "cap_hit": L["cap_hit"],
            "items": n_items, "live_cursors": sorted(cursors.get(p, set())),
            "reached_by": sorted(reached.get(p, set())),
            "live_eq_file": (sid in ftabs and ftabs[sid][1] == L["words"]),
            "live_eq_standalone_kT": (sid in sa and sa[sid] == L["words"]),
            "counts": cnt, "per_cursor": per,
        })

    # every live 0x0042ac90 call: does the transcription reproduce its return, what would IAC return
    call_rows = []
    for c in calls:
        p = c["table_ptr"]; L = live.get(p)
        cur = c["cursor"]
        o = orig_meg(L["words"] + L["tail"], cur)[0] if L else None
        s = item_action_code(L["words"], cur) if L else (None, None)
        call_rows.append({"src": c["src"], "depth": c["depth"], "screen": ptr2sid.get(p), "ptr": hex(p),
                          "cursor": cur, "live_ret": hex(c["ret"]),
                          "orig_meg": None if o is None else hex(o), "orig_meg_matches_live": o == c["ret"],
                          "iac": None if s[0] is None else hex(s[0]), "iac_how": s[1],
                          "iac_matches_live": s[0] == c["ret"]})

    out = {"selftest": st, "selftest_ok": f"{st_ok}/{len(st)}",
           "ptrs_live_eq_file": (ptrs_live is not None and list(ptrs_live) == list(fptrs)),
           "screens_not_dumped": [sid for sid, p in enumerate(fptrs) if p and p not in live],
           "totals": totals, "fires": fire, "live_calls": call_rows, "tables": rows}
    Path(a.out).write_text(json.dumps(out, indent=1), encoding="utf-8")

    print(f"PTR_005f7638 live == file: {out['ptrs_live_eq_file']}; screens not dumped: {out['screens_not_dumped']}")
    print(f"{'scr':>3} {'ptr':>9} {'words':>5} {'items':>5} {'live_cur':<16} {'=file':<5} {'=kT':<5} "
          f"{'tested':>6} {'agree':>5} {'(1)':>3} {'(2)':>3} {'(3)':>3} {'oth':>3} {'oob':>3}")
    for r in rows:
        c = r["counts"]
        print(f"{str(r['screen']):>3} {r['ptr']:>9} {r['words_to_T7']:>5} {r['items']:>5} "
              f"{str(r['live_cursors']):<16} {str(r['live_eq_file']):<5} {str(r['live_eq_standalone_kT']):<5} "
              f"{len(r['per_cursor']):>6} {c['agree']:>5} {c['1']:>3} {c['2']:>3} {c['3']:>3} {c['other']:>3} {c['oob']:>3}")
    print("totals", totals)
    print("live 0x0042ac90 calls:")
    for c in call_rows:
        print("  ", c)
    print("fires:")
    for f in fire:
        print("  ", f)


if __name__ == "__main__":
    main()
