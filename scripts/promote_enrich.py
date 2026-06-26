#!/usr/bin/env py -3.12
"""promote_enrich.py — full mechanical enrichment of the first-party C2 pool.

Extends promote_subdivide.py with EIGHT more classification dimensions, all
computed with NO Ghidra and NO running game (collision-free), in one pass over a
single disassembly. Every dimension is a binary-disasm or hooks.csv/CSV-join fact
(NO-GUESSING; heuristic fields carry a confidence/maybe flag).

DIMENSIONS
  1 signature   inferred calling convention + arg count/kind + return; mapped to a
                diff_template.js GENERIC scalar arg_type when one fits -> 'runnable
                today via path1' vs 'needs bespoke arg_type / booted observe'.
  2 promote-order  first-party call DAG: Tarjan SCC (recursion cycles flagged) +
                keystone ranking (C1 callees by #C2 parents they block) + waves.
  3 rw-orch     fraction of resolved callees that are library/RW-named.
  4 xbox-twin   join re/console/match/xbuild_match_v2.csv (scaffold=ok-asc = clean
                second static witness -> easier to author).
  5 fp          uses x87/SSE float ops (bit-identity gotcha: build x87, inline asm).
  6 imports     IAT/thunk fingerprint -> COM/D3D9/DSound/DInput/DShow vs Win32.
  7 clone       normalized body hash -> identical-shape clusters (batch one spec).
  8 globals     .data/.rdata globals each fn touches -> hot globals + shared-state
                connected components (promote together under one scenario).

OUTPUT (re/analysis/recon_c2/, enrich_* prefix; never touches shared frontier files):
  enrich_all.tsv                every clean first-party C2 with all columns
  enrich_runnable_today.tsv     signature maps to a generic scalar arg_type now
  enrich_needs_argtype.tsv      callee-clean but signature needs a bespoke arg_type
  enrich_keystones.tsv          C1 callees ranked by #C2 parents unblocked (C1->C2 first)
  enrich_clone_clusters.tsv     identical-body clusters (>=2 members)
  enrich_hot_globals.tsv        globals touched by the most C2 functions
  enrich_state_components.tsv   shared-global connected components

Run:  py -3.12 scripts/promote_enrich.py
"""
import csv
import os
import re
import struct
import sys
from collections import Counter, defaultdict

import pefile
import capstone

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "scripts"))
import promote_frontier as PF

OUTDIR = os.path.join(ROOT, "re", "analysis", "recon_c2")
XBUILD = os.path.join(ROOT, "re", "console", "match", "xbuild_match_v2.csv")
EXE = PF.EXE

PROMOTABLE = {"C2", "C3", "C4"}
LIBMARK = re.compile(r"fiddb|vs2003|library match|__crt|\bseh\b|heap_init|mtinit|"
                     r"msvcrt|crt-band|lever3", re.I)
GLOBAL_LO, GLOBAL_HI = 0x005EA000, 0x00915000      # writable .data
RDATA_LO, RDATA_HI = 0x005CC000, 0x005E9044        # read-only consts
FPU_MN = re.compile(r"^f(ld|st|add|sub|mul|div|com|ild|ist|chs|abs|sqrt|"
                    r"xch|prem|patan|sin|cos|ptan|2xm1|yl2x|rndint|nstcw|ldcw)")
SSE_MN = re.compile(r"(ss|sd|ps|pd)$|^(movups|movaps|movss|movsd|cvt|unpck|shuf)")
HEX_RE = re.compile(r"0x[0-9a-f]+")
RWNAME_RE = re.compile(r"^(Rw|Rp|Rt|_rw|_rp)")


def conf_of(hooks, va):
    r = hooks.get(va)
    return (r.get("confidence") or "").strip() if r else ""


# ---------------------------------------------------------------- dim 6: imports
def build_import_maps():
    pe = pefile.PE(EXE)
    pe.parse_data_directories(
        directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
    iat = {}                       # IAT slot VA -> (dll, name)
    for entry in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
        dll = entry.dll.decode("latin1").lower()
        for imp in entry.imports:
            if imp.address:
                nm = imp.name.decode("latin1") if imp.name else f"ord{imp.ordinal}"
                iat[imp.address] = (dll, nm)
    # thunk map: scan .text for `jmp dword ptr [iatslot]`  (FF 25 disp32)
    text = next(s for s in pe.sections if s.Name.rstrip(b"\x00") == b".text")
    base = pe.OPTIONAL_HEADER.ImageBase
    tva = base + text.VirtualAddress
    data = text.get_data()
    thunk = {}
    i = 0
    while True:
        j = data.find(b"\xff\x25", i)
        if j < 0:
            break
        slot = struct.unpack_from("<I", data, j + 2)[0]
        if slot in iat:
            thunk[tva + j] = iat[slot]
        i = j + 2
    return iat, thunk


def classify_dll(dll):
    if dll.startswith("d3d9") or dll.startswith("d3dx"):
        return "D3D9"
    if dll.startswith("dsound"):
        return "DSound"
    if dll.startswith("dinput"):
        return "DInput"
    if dll.startswith("ddraw"):
        return "DDraw"
    if dll in ("quartz.dll", "strmiids.dll", "ole32.dll", "oleaut32.dll"):
        return "COM/DShow"
    if dll.startswith("winmm"):
        return "WinMM"
    return "Win32"


# ---------------------------------------------------------------- dim 1: signature
def infer_sig(insns, derefs_arg):
    """Heuristic 32-bit signature. Returns dict(conv, nargs, float_arg, ret,
    arg_type_today, runnable_today, sig_conf)."""
    if not insns:
        return dict(conv="?", nargs=0, float_arg=False, ret="void",
                    arg_type_today=None, runnable_today=False, sig_conf="none")
    ebp_frame = (len(insns) >= 2 and insns[0].mnemonic == "push"
                 and insns[0].op_str == "ebp" and insns[1].mnemonic == "mov"
                 and insns[1].op_str.replace(" ", "") == "ebp,esp")
    arg_offs, float_arg = set(), False
    ecx_in = edx_in = wrote_ecx = wrote_edx = False
    eax_written = st0_live = False
    ret_imm = False
    for ins in insns:
        mn, op = ins.mnemonic, ins.op_str
        # arg slots: ebp+N (N>=8) when framed; else esp+N at shallow depth
        if ebp_frame:
            for m in re.finditer(r"ebp \+ (0x[0-9a-f]+|\d+)", op):
                off = int(m.group(1), 16) if m.group(1).startswith("0x") else int(m.group(1))
                if off >= 8:
                    arg_offs.add(off)
                    if mn.startswith("f"):
                        float_arg = True
        else:
            for m in re.finditer(r"esp \+ (0x[0-9a-f]+|\d+)", op):
                off = int(m.group(1), 16) if m.group(1).startswith("0x") else int(m.group(1))
                if 4 <= off <= 0x40:
                    arg_offs.add(off)
                    if mn.startswith("f"):
                        float_arg = True
        # register-arg detection (fastcall/thiscall): reg used as ptr base before write
        if "[ecx" in op and not wrote_ecx:
            ecx_in = True
        if "[edx" in op and not wrote_edx:
            edx_in = True
        if mn == "mov" and op.startswith("ecx,"):
            wrote_ecx = True
        if mn == "mov" and op.startswith("edx,"):
            wrote_edx = True
        if op.startswith("eax") and mn in ("mov", "lea", "add", "sub", "xor", "or",
                                            "and", "imul", "movzx", "movsx", "pop", "inc", "dec"):
            eax_written = True
        if mn in ("fld", "fild", "fadd", "fmul", "fsub", "fdiv", "fchs", "fabs"):
            st0_live = True
        if mn in ("fstp", "fst") and "[" in op:
            st0_live = False   # result stored out, not returned in st0
        if ins.bytes and ins.bytes[0] == 0xC2:   # ret imm16
            ret_imm = True
    nargs = len(arg_offs)
    if ecx_in or edx_in:
        conv = "fastcall/thiscall"
    elif ret_imm:
        conv = "stdcall"
    else:
        conv = "cdecl"
    ret = "float" if (st0_live and not eax_written) else ("int" if eax_written else "void")
    # map to a GENERIC scalar arg_type the harness supports today
    at = None
    if not (ecx_in or edx_in) and not float_arg and ret in ("int", "void"):
        if not derefs_arg:
            if nargs == 0:
                at = "none"
            elif nargs == 1:
                at = "int_scalar"
            elif nargs == 2:
                at = "int_pair"
        else:
            # single deref'd-pointer arg, int return: the ptr_arg_int_get
            # arg_type (diff_template.js 2026-06-26 meta-action) feeds a seeded
            # scratch buffer and observes the int return. Unblocks ~195 cdecl/1/
            # int getters that int_scalar can't touch (random-int deref => AV).
            # Restricted to nargs==1 + int return (the validated shape); deeper
            # double-deref getters that fault are dropped by the flake-tolerant
            # collector, never falsely promoted.
            if nargs == 1 and ret == "int":
                at = "ptr_arg_int_get"
    runnable = at is not None
    sig_conf = "low" if not ebp_frame else "med"   # esp-arg counting is less reliable
    return dict(conv=conv, nargs=nargs, float_arg=float_arg, ret=ret,
                arg_type_today=at, runnable_today=runnable, sig_conf=sig_conf)


# ---------------------------------------------------------------- dim 2: SCC + order
def tarjan_scc(nodes, succ):
    index = {}
    low = {}
    onstack = {}
    stack = []
    out = []
    counter = [0]
    # iterative Tarjan
    for root in nodes:
        if root in index:
            continue
        work = [(root, iter(succ.get(root, ())))]
        index[root] = low[root] = counter[0]; counter[0] += 1
        stack.append(root); onstack[root] = True
        while work:
            node, it = work[-1]
            advanced = False
            for w in it:
                if w not in nodes_set:
                    continue
                if w not in index:
                    index[w] = low[w] = counter[0]; counter[0] += 1
                    stack.append(w); onstack[w] = True
                    work.append((w, iter(succ.get(w, ()))))
                    advanced = True
                    break
                elif onstack.get(w):
                    low[node] = min(low[node], index[w])
            if advanced:
                continue
            if low[node] == index[node]:
                comp = []
                while True:
                    w = stack.pop(); onstack[w] = False; comp.append(w)
                    if w == node:
                        break
                out.append(comp)
            work.pop()
            if work:
                parent = work[-1][0]
                low[parent] = min(low[parent], low[node])
    return out


nodes_set = set()  # filled in main (used by tarjan_scc closure)


def main():
    a = PF.analyze()
    hooks = a["hooks"]
    size_of, has_call = a["size_of"], a["has_call"]
    direct_callees, callers = a["direct_callees"], a["callers"]
    code, tva = a["code"], a["text_va"]

    # universe: clean first-party C2 (same filter as promote_subdivide)
    universe = []
    for va, row in hooks.items():
        if (row.get("confidence") or "").strip() != "C2":
            continue
        if not PF.is_first_party(row):
            continue
        if va not in size_of:
            continue
        if LIBMARK.search(row.get("notes") or ""):
            continue
        universe.append(va)
    universe.sort()

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = False
    iat, thunk = build_import_maps()

    # xbox twin join
    twin = {}
    if os.path.exists(XBUILD):
        with open(XBUILD, newline="", encoding="utf-8", errors="replace") as f:
            for r in csv.DictReader(f):
                try:
                    pv = int(r["pc_va"], 16)
                except (ValueError, KeyError):
                    continue
                twin[pv] = (r.get("scaffold", ""), r.get("tier", ""))

    rows = []
    clone_groups = defaultdict(list)
    global_users = defaultdict(set)     # global addr -> {va}
    fn_globals = {}

    for va in universe:
        sz = size_of.get(va, 0)
        off = va - tva
        insns = list(md.disasm(code[off:off + sz], va))
        ms = [(i.mnemonic, i.op_str) for i in insns]

        # STATE / deref-arg (reuse promote_classify heuristic)
        derefs_arg = any("[eax" in o or "[ecx" in o or "[edx" in o for _, o in ms)
        reads_global = any(("0x6" in o or "0x7" in o or "0x8" in o or "0x9" in o)
                           and "[" in o for _, o in ms)

        # dim 1: signature
        sig = infer_sig(insns, derefs_arg)

        # dim 5: fp
        fp = any(FPU_MN.match(m) or SSE_MN.search(m) for m, _ in ms)

        # dim 6: imports
        imps = set()
        for ins in insns:
            if ins.mnemonic in ("call", "jmp"):
                op = ins.op_str
                mm = re.search(r"\[(0x[0-9a-f]+)\]", op)
                if mm:                                  # call dword ptr [IAT]
                    slot = int(mm.group(1), 16)
                    if slot in iat:
                        imps.add(classify_dll(iat[slot][0]))
                elif op.startswith("0x"):               # call thunk
                    t = int(op, 16)
                    if t in thunk:
                        imps.add(classify_dll(thunk[t][0]))
        com_d3d = bool(imps & {"D3D9", "DSound", "DInput", "DDraw", "COM/DShow"})

        # dim 3: rw orchestrator
        callees = direct_callees.get(va, set())
        lib_callees = 0
        for c in callees:
            cr = hooks.get(c)
            nm = (cr or {}).get("name", "")
            if (cr and "third-party-library" in (cr.get("subsystem") or "")) \
                    or RWNAME_RE.match(nm):
                lib_callees += 1
        rw_frac = (lib_callees / len(callees)) if callees else 0.0
        rw_orch = len(callees) >= 2 and rw_frac >= 0.5

        # dim 7: clone hash (normalized body) — only meaningful for >=4 insns
        norm = " ".join(m + " " + HEX_RE.sub("#", o) for m, o in ms)
        clone_key = None
        if len(ms) >= 4:
            clone_key = (len(ms), hash(norm))
            clone_groups[clone_key].append(va)

        # dim 8: globals touched
        gset = set()
        for _, o in ms:
            for m in HEX_RE.findall(o):
                v = int(m, 16)
                if GLOBAL_LO <= v < GLOBAL_HI or RDATA_LO <= v < RDATA_HI:
                    gset.add(v)
        fn_globals[va] = gset
        for g in gset:
            global_users[g].add(va)

        # dim 4: twin
        sc, tier = twin.get(va, ("", ""))
        clean_twin = (sc == "ok-asc")

        rows.append(dict(
            va=va, sub=(hooks[va].get("subsystem") or ""),
            name=(hooks[va].get("name") or ""), size=sz,
            n_callees=len(callees), conv=sig["conv"], nargs=sig["nargs"],
            ret=sig["ret"], fp=int(fp), arg_type_today=sig["arg_type_today"] or "-",
            runnable_today=int(sig["runnable_today"]), sig_conf=sig["sig_conf"],
            imports=("+".join(sorted(imps)) or "-"), com_d3d=int(com_d3d),
            rw_orch=int(rw_orch), rw_frac=round(rw_frac, 2),
            twin=("ok-asc" if clean_twin else (sc or "-")),
            derefs_arg=int(derefs_arg), reads_global=int(reads_global),
            n_globals=len(gset), clone_key=clone_key,
        ))

    # ---- dim 2: SCC + keystones over the WHOLE first-party graph ----
    global nodes_set
    fp_starts = [v for v, r in hooks.items()
                 if v in size_of and PF.is_first_party(r)]
    nodes_set = set(fp_starts)
    succ = {v: [c for c in direct_callees.get(v, ()) if c in nodes_set] for v in fp_starts}
    sccs = tarjan_scc(fp_starts, succ)
    cyclic = set()
    for comp in sccs:
        if len(comp) > 1:
            cyclic |= set(comp)
    # keystones: C1 functions ranked by # of C2 parents they directly block
    keystone = Counter()
    for va in universe:
        for c in direct_callees.get(va, set()):
            if conf_of(hooks, c) not in PROMOTABLE:      # blocking callee (<C2)
                keystone[c] += 1
    # annotate cyclic onto rows
    cyc_va = set(r["va"] for r in rows) & cyclic
    for r in rows:
        r["cyclic"] = int(r["va"] in cyclic)

    # ===================== writes =====================
    os.makedirs(OUTDIR, exist_ok=True)
    cols = ["va", "sub", "name", "size", "n_callees", "conv", "nargs", "ret",
            "arg_type_today", "runnable_today", "sig_conf", "fp", "imports",
            "com_d3d", "rw_orch", "rw_frac", "twin", "derefs_arg", "reads_global",
            "n_globals", "cyclic"]

    def w(path, subset, columns=cols):
        with open(path, "w", encoding="utf-8", newline="") as f:
            wr = csv.writer(f, delimiter="\t")
            wr.writerow(["rva" if c == "va" else c for c in columns])
            for r in subset:
                wr.writerow([f"{r['va']:08x}" if c == "va" else r.get(c, "")
                             for c in columns])

    w(os.path.join(OUTDIR, "enrich_all.tsv"), rows)
    runnable = sorted((r for r in rows if r["runnable_today"]),
                      key=lambda r: (r["nargs"], r["size"]))
    w(os.path.join(OUTDIR, "enrich_runnable_today.tsv"), runnable)
    # callee-clean (no blocking callee) but signature needs a bespoke arg_type
    def callee_clean(va):
        return all(conf_of(hooks, c) in PROMOTABLE for c in direct_callees.get(va, set()))
    needs_at = sorted((r for r in rows if not r["runnable_today"] and callee_clean(r["va"])
                       and not r["com_d3d"]),
                      key=lambda r: r["size"])
    w(os.path.join(OUTDIR, "enrich_needs_argtype.tsv"), needs_at)

    # keystones
    ks_path = os.path.join(OUTDIR, "enrich_keystones.tsv")
    with open(ks_path, "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f, delimiter="\t")
        wr.writerow(["rva", "now_conf", "parents_blocked", "is_leaf", "cyclic",
                     "subsystem", "name"])
        for c, n in keystone.most_common():
            cr = hooks.get(c, {})
            wr.writerow([f"{c:08x}", conf_of(hooks, c) or "?unmapped", n,
                         int(not has_call.get(c, True)), int(c in cyclic),
                         cr.get("subsystem", ""), cr.get("name", "")])

    # clone clusters
    cl_path = os.path.join(OUTDIR, "enrich_clone_clusters.tsv")
    clusters = [(k, vs) for k, vs in clone_groups.items() if len(vs) >= 2]
    clusters.sort(key=lambda kv: -len(kv[1]))
    with open(cl_path, "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f, delimiter="\t")
        wr.writerow(["cluster", "n_insns", "n_members", "member_rvas"])
        for i, (k, vs) in enumerate(clusters):
            wr.writerow([i, k[0], len(vs), " ".join(f"{v:08x}" for v in sorted(vs))])

    # hot globals (touched by most C2 fns)
    hg_path = os.path.join(OUTDIR, "enrich_hot_globals.tsv")
    with open(hg_path, "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f, delimiter="\t")
        wr.writerow(["global", "n_c2_users", "region"])
        for g, users in sorted(global_users.items(), key=lambda kv: -len(kv[1]))[:200]:
            region = "data" if GLOBAL_LO <= g < GLOBAL_HI else "rdata"
            wr.writerow([f"0x{g:08x}", len(users), region])

    # shared-global connected components (edge = share >=2 globals)
    # build via union-find over fns that share globals (cap: only globals with
    # 2..40 users to avoid hairball from ubiquitous globals)
    parent = {r["va"]: r["va"] for r in rows}
    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]; x = parent[x]
        return x
    def union(x, y):
        rx, ry = find(x), find(y)
        if rx != ry:
            parent[rx] = ry
    shared = defaultdict(int)
    for g, users in global_users.items():
        if 2 <= len(users) <= 40:
            ul = sorted(users)
            for i in range(len(ul)):
                for j in range(i + 1, len(ul)):
                    shared[(ul[i], ul[j])] += 1
    for (x, y), c in shared.items():
        if c >= 2:
            union(x, y)
    comps = defaultdict(list)
    for r in rows:
        comps[find(r["va"])].append(r["va"])
    comp_path = os.path.join(OUTDIR, "enrich_state_components.tsv")
    big = sorted((v for v in comps.values() if len(v) >= 3), key=lambda v: -len(v))
    with open(comp_path, "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f, delimiter="\t")
        wr.writerow(["component", "n_members", "subsystems", "member_rvas"])
        for i, vs in enumerate(big):
            subs = Counter(hooks[v].get("subsystem", "") for v in vs)
            wr.writerow([i, len(vs), ",".join(f"{s}:{n}" for s, n in subs.most_common(3)),
                         " ".join(f"{v:08x}" for v in sorted(vs))])

    # ===================== summary =====================
    n = len(rows)
    print(f"clean first-party C2 enriched: {n}\noutput: {OUTDIR}\n")
    print("DIM1 signature -> generic arg_type runnable via path1 TODAY:")
    print(f"  runnable_today (0-2 int args, scalar/void ret, no arg-deref): {len(runnable)}")
    print(f"    by arg_type: " + ", ".join(f"{at}={c}" for at, c in
          Counter(r['arg_type_today'] for r in runnable).most_common()))
    print(f"  callee-clean but needs a bespoke arg_type: {len(needs_at)}")
    print(f"\nDIM2 graph: first-party nodes={len(fp_starts)}, "
          f"SCC cycles touching C2={len(cyc_va)}; keystone C1 blockers={len(keystone)}")
    print("  top keystones (C1->C2 mechanical unblocks N C2 parents):")
    for c, k in keystone.most_common(8):
        print(f"    {c:08x}  blocks {k:3d}  ({conf_of(hooks,c) or '?'}, "
              f"{'leaf' if not has_call.get(c,True) else 'non-leaf'})")
    print(f"\nDIM3 rw-orchestrators (>=50% lib callees): {sum(r['rw_orch'] for r in rows)}")
    print(f"DIM4 with clean ok-asc xbox twin: {sum(1 for r in rows if r['twin']=='ok-asc')}")
    print(f"DIM5 fp/x87-using (bit-identity care): {sum(r['fp'] for r in rows)}")
    print(f"DIM6 touch COM/D3D/DSound/DInput imports: {sum(r['com_d3d'] for r in rows)}")
    imp_ct = Counter()
    for r in rows:
        if r["imports"] != "-":
            for t in r["imports"].split("+"):
                imp_ct[t] += 1
    print("  imports breakdown: " + ", ".join(f"{k}={v}" for k, v in imp_ct.most_common()))
    print(f"DIM7 clone clusters (>=2 identical bodies): {len(clusters)} "
          f"covering {sum(len(vs) for _,vs in clusters)} fns")
    print(f"DIM8 hot globals (top user count): " +
          ", ".join(f"0x{g:08x}:{len(u)}" for g, u in
                    sorted(global_users.items(), key=lambda kv: -len(kv[1]))[:5]))
    print(f"     shared-state components (>=3 fns): {len(big)}")


if __name__ == "__main__":
    sys.exit(main())
