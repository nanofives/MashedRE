"""MATCH-ARITY-TRIAGE-1: for ARITY rows of callee_arity_lint whose offending source is #included by a mashedmod
wrapper, disassemble the original call sites inside the caller and count pushes / cleanup, plus the callee's
ret imm and highest stack-argument slot it addresses (linear esp-tracking scan).
usage: py -3.12 triage.py <repo_root> <lint_full.md> <out.tsv>"""
import re, sys, pathlib, collections
import pefile, capstone

root = pathlib.Path(sys.argv[1]); full = pathlib.Path(sys.argv[2]); out = pathlib.Path(sys.argv[3])
EXE = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")

inc = collections.defaultdict(set)
for p in (root / "mashedmod" / "src").rglob("*"):
    if p.suffix.lower() not in (".cpp", ".c", ".h", ".inc"):
        continue
    for m in re.finditer(r'#\s*include\s*"([^"]*re/match/[^"]+\.c)"', p.read_text(errors="replace")):
        rel = "re/match/" + m.group(1).split("re/match/", 1)[1]
        inc[rel].add(p.relative_to(root).as_posix())

secs = []
cur = None
for ln in full.read_text(encoding="utf-8").splitlines():
    m = re.match(r"## ARITY 0x([0-9a-f]{8})\s+\((.*)\)", ln)
    if m:
        ref = re.search(r"REF (\S+):(\d+) \S+ (.*) n=(\d+)", m.group(2))
        cur = dict(callee=int(m.group(1), 16), ref=ref.group(1) if ref else "", refline=ref.group(2) if ref else "",
                   refproto=ref.group(3) if ref else "", refn=int(ref.group(4)) if ref else None,
                   c4="c4=match" in m.group(2), ents=[])
        secs.append(cur); continue
    if ln.startswith("## "):
        cur = None; continue
    m = re.match(r"- (\S+):(\d+) (def|decl) (.*) n=(\d+)", ln)
    if cur is not None and m:
        cur["ents"].append(dict(src=m.group(1), line=m.group(2), kind=m.group(3), proto=m.group(4), n=int(m.group(5))))

pe = pefile.PE(str(EXE), fast_load=True); base = pe.OPTIONAL_HEADER.ImageBase
text = [s for s in pe.sections if s.Name.rstrip(b"\0") == b".text"][0]
tva = base + text.VirtualAddress; tdata = text.get_data()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
funcs = {}
for ln in (root / "re/console/pc_funcs.csv").read_text().splitlines()[1:]:
    r = ln.split(",")
    funcs[int(r[0], 16)] = int(r[1], 16)   # body_end is the last byte (inclusive)


def insns(a, b):
    return list(md.disasm(tdata[a - tva:b + 1 - tva], a))


def callee_ret(c):
    end = funcs.get(c)
    if not end:
        return "?"
    rets = sorted({(i.op_str or "0") for i in insns(c, end) if i.mnemonic == "ret"})
    return "/".join(rets) or "none"


ARG = re.compile(r"\[(esp|ebp) \+ (0x[0-9a-f]+|\d+)\]")


def callee_maxarg(c, deep=True):
    end = funcs.get(c)
    if not end:
        return "?"
    d = 0; ebp = None; mx = 0; at = {}; pre = 0; hit = ""; term = False
    for i in insns(c, end):
        if term:
            d = at.get(i.address, pre)
        term = False
        for mm in (ARG.finditer(i.op_str) if i.mnemonic != "lea" else ()):
            dd = d if mm.group(1) == "esp" else ebp
            if dd is None:
                continue
            off = int(mm.group(2), 0) - dd - 4
            if off >= 0 and off // 4 + 1 > mx:
                mx = off // 4 + 1; hit = f"{i.address:#010x} {i.mnemonic} {i.op_str}"
        m = i.mnemonic
        if m == "push":
            d += 4
        elif m == "pop":
            d -= 4
        elif m == "sub" and i.op_str.startswith("esp, "):
            d += int(i.op_str[5:], 0)
        elif m == "add" and i.op_str.startswith("esp, "):
            d -= int(i.op_str[5:], 0)
        elif m == "mov" and i.op_str == "ebp, esp":
            ebp = d
        elif m.startswith("j") and i.op_str.startswith("0x"):
            at.setdefault(int(i.op_str, 16), d)
        if m in ("ret", "jmp"):
            term = True
        if m != "pop":
            pre = d
    tj = [(i.address, int(i.op_str, 16)) for i in insns(c, end) if i.mnemonic == "jmp" and i.op_str.startswith("0x") and not (c <= int(i.op_str, 16) <= end)]
    tjs = ",".join(f"{a:#010x}->{b:#010x}(reads {callee_maxarg(b, False).split(' TAIL')[0] if deep and b in funcs else '?'})" for a, b in tj)
    return f"{mx}" + (f" @ {hit}" if hit else "") + (f" TAILJMP-OUT {tjs}" if tj else "")


def defined_in(src):
    t = (root / src).read_text(errors="replace")
    ds = set()
    for m in re.finditer(r"^[^\n;#/]*?\bFUN_([0-9a-f]{8})\s*\([^;{]*\)\s*\{", t, re.M):
        ds.add(int(m.group(1), 16))
    for m in re.finditer(r"^[^\n;#/]*?\bFUN_([0-9a-f]{8})\s*\([^;{]*\)\s*\n\s*\{", t, re.M):
        ds.add(int(m.group(1), 16))
    for m in re.finditer(r"MATCH: 0x([0-9a-f]{8})", t):
        ds.add(int(m.group(1), 16))
    fm = re.search(r"([0-9a-f]{8})\.c$", src)
    if fm:
        ds.add(int(fm.group(1), 16))
    return {d for d in ds if d in funcs}


def call_sites(caller, callee):
    L = insns(caller, funcs[caller]); res = []
    for k, i in enumerate(L):
        if i.mnemonic in ("call", "jmp") and i.op_str.startswith("0x") and int(i.op_str, 16) == callee:
            if i.mnemonic == "jmp":
                res.append(dict(at=i.address, pushes=["TAILJMP"], stop="", after="", ecx=False, rel="")); continue
            pushes = []; j = k - 1
            while j >= 0 and L[j].mnemonic not in ("call", "ret") and not L[j].mnemonic.startswith("j"):
                if L[j].mnemonic == "push" or (L[j].mnemonic == "sub" and L[j].op_str.startswith("esp")):
                    pushes.append(f"{L[j].address:#010x} {L[j].mnemonic} {L[j].op_str}")
                j -= 1
            stop = f"{L[j].address:#010x} {L[j].mnemonic}" if j >= 0 else "entry"
            if j < 0:   # drop the frame prologue: sub esp / callee-saved pushes before the first other push
                pr = list(reversed(pushes)); q = 0
                while q < len(pr) and (pr[q].endswith(("push ebx", "push ebp", "push esi", "push edi")) or " sub esp" in pr[q]):
                    q += 1
                pushes = list(reversed(pr[q:])); stop = "entry(prologue dropped)"
            npush = sum(1 for p in pushes if " push " in p) + sum(int(p.split("sub esp, ")[1], 0) // 4 for p in pushes if " sub esp, " in p)
            depth = 0; rel = "end"
            for q in L[k + 1:]:
                if depth <= -4 * npush:
                    rel = f"released@{q.address:#010x}"; break
                if q.mnemonic == "call":
                    rel = f"pending-at-call@{q.address:#010x}->{q.op_str}"
                    if q.op_str.startswith("0x") and int(q.op_str, 16) in funcs:
                        nm = callee_maxarg(int(q.op_str, 16), False).split(" TAIL")[0].split()[0]
                        rel += f" (own pushes {depth // 4}, next callee reads {nm}: " + (
                            "EXTRAS-READ-BY-NEXT" if int(nm) > depth // 4 else "extras-not-read-by-next") + ")"
                    break
                if q.mnemonic in ("ret", "jmp") or q.mnemonic.startswith("j"):
                    rel = f"pending-at-{q.mnemonic}@{q.address:#010x}"; break
                if q.mnemonic == "push": depth += 4
                elif q.mnemonic == "pop": depth -= 4
                elif q.mnemonic == "add" and q.op_str.startswith("esp, "): depth -= int(q.op_str[5:], 0)
                elif q.mnemonic == "sub" and q.op_str.startswith("esp, "): depth += int(q.op_str[5:], 0)
                elif q.mnemonic == "lea" and q.op_str.startswith("esp, [esp + "): depth -= int(q.op_str[12:-1], 0)
            if npush == 0:
                rel = "n/a"
            after = L[k + 1] if k + 1 < len(L) else None
            clean = f"{after.address:#010x} {after.mnemonic} {after.op_str}" if after else ""
            ecx = any(L[x].op_str.startswith(("ecx,", "edx,")) for x in range(max(j, 0), k) if L[x].mnemonic in ("mov", "lea"))
            res.append(dict(at=i.address, pushes=list(reversed(pushes)), stop=stop, after=clean, ecx=ecx, rel=rel))
    return res


def dwords(proto, srctext):
    m = re.search(r"\((.*)\)\s*$", proto)
    inner = m.group(1).strip() if m else ""
    if inner in ("", "void"):
        return 0
    tot = 0
    for p in inner.split(","):
        p = p.strip()
        if "*" in p or p in ("int", "uint", "float", "char", "short", "undefined4", "unsignedint", "unsignedchar",
                             "HINSTANCE", "undefined", "undefined1", "undefined2", "byte", "ushort", "long"):
            tot += 1; continue
        if p == "double":
            tot += 2; continue
        sm = re.search(r"typedef\s+struct\s*\w*\s*\{([^}]*)\}\s*" + re.escape(p) + r"\s*;", srctext)
        if not sm:
            sm = re.search(r"struct\s+" + re.escape(p) + r"\s*\{([^}]*)\}", srctext)
        if sm:
            body = sm.group(1); b = 0
            for decl in body.split(";"):
                decl = decl.strip()
                if not decl:
                    continue
                t = decl.split()[0]
                names = decl[len(t):].split(",")
                for nm in names:
                    arr = re.search(r"\[(\d+)\]", nm); k = int(arr.group(1)) if arr else 1
                    sz = 8 if t == "double" else 2 if t in ("short", "ushort", "uint16_t") else 1 if t in ("char", "unsigned", "uchar", "byte", "uint8_t", "RwUInt8", "unsignedchar") else 4
                    if t == "unsigned" and "char" in nm:
                        sz = 1
                    b += sz * k
            tot += (b + 3) // 4; continue
        if re.search(r"RGBA|Rgba", p):
            tot += 1; continue
        if re.search(r"V2|v2", p):
            tot += 2; continue
        return None
    return tot


rows = []
for s in secs:
    if s["refn"] is None:
        continue
    for e in s["ents"]:
        if e["n"] == s["refn"] or e["src"] == s["ref"] or e["src"] not in inc:
            continue
        txt = (root / e["src"]).read_text(errors="replace")
        name = f"FUN_{s['callee']:08x}"
        ncall = len(re.findall(r"(?<![\w*)])\b" + name + r"\s*\(", txt)) - 1
        e["use"] = f"name_calls_in_src={ncall}"
        sites = [(c, x) for c in sorted(defined_in(e["src"])) for x in call_sites(c, s["callee"])]
        rows.append((s, e, sites))

def verdict(S, C, R, M, after, ret):
    if ret not in ("0", "none", "?"):
        return "CHECK-stdcall"
    if S is None or C is None or R is None:
        return "MANUAL"
    if S == C == R:
        return "SAME-LAYOUT"
    if S == C and M <= R < C:
        return "CALLER-EXTRA-UNREAD"
    if S == C and R < M:
        return "CALLER-RIGHT"
    if S == C and C < R and M > S:
        return "SITE-SHORT"
    if S == C and C < R:
        return "SITE-SHORT-UNREAD"
    if S == R:
        return "CALLEE-RIGHT"
    return "MANUAL"


with out.open("w", encoding="utf-8") as f:
    f.write("callee\tref\trefn\tc4ref\tcallee_ret\tcallee_maxarg\tsrc\tline\tproto\tn\twrappers\tcaller\tcall_at"
            "\tnpush\tpushes\tstop\tafter\tregarg\tuse\tC_dw\tR_dw\tS_dw\tcleanup_ok\tverdict\trelease\n")
    for s, e, sites in rows:
        mx = callee_maxarg(s["callee"]); M = int(mx.split()[0]) if mx[0].isdigit() else None
        ret = callee_ret(s["callee"])
        C = dwords(e["proto"], (root / e["src"]).read_text(errors="replace"))
        R = dwords(s["refproto"], (root / s["ref"]).read_text(errors="replace"))
        head = (f"{s['callee']:#010x}\t{s['ref']}:{s['refline']} {s['refproto']}\t{s['refn']}\t{int(s['c4'])}"
                f"\t{ret}\t{mx}\t{e['src']}\t{e['line']}\t{e['proto']}"
                f"\t{e['n']}\t{';'.join(sorted(inc[e['src']]))}")
        if not sites:
            v = "NO-CALL" if e["use"].endswith(("=0", "=-1")) else "MANUAL-NOSITE"
            f.write(f"{head}\t\tNO_SITE\t\t\t\t\t\t{e['use']}\t{C}\t{R}\t\t\t{v}\n")
        for c, x in sites:
            if x["pushes"] == ["TAILJMP"]:
                f.write(f"{head}\t{c:#010x}\t{x['at']:#010x}\t\tTAILJMP\t\t\t0\t{e['use']}\t{C}\t{R}\t\t\tMANUAL-TAILJMP\n")
                continue
            S = 0
            for p in x["pushes"]:
                if " push " in p:
                    S += 1
                elif " sub esp, " in p:
                    S += int(p.split("sub esp, ")[1], 0) // 4
            am = re.search(r"add esp, (0x[0-9a-f]+|\d+)$", x["after"])
            ck = "" if not am else ("yes" if int(am.group(1), 0) // 4 == S else f"no({int(am.group(1), 0) // 4})")
            v = verdict(S, C, R, M if M is not None else -1, x["after"], ret)
            if x["ecx"] and v not in ("SAME-LAYOUT",):
                v += "+REGARG?"
            f.write(f"{head}\t{c:#010x}\t{x['at']:#010x}\t{S}\t{' | '.join(x['pushes'])}\t{x['stop']}\t{x['after']}"
                    f"\t{int(x['ecx'])}\t{e['use']}\t{C}\t{R}\t{S}\t{ck}\t{v}\t{x['rel']}\n")
print(len(rows), "offending (callee,source) pairs in exe sources;", sum(len(r[2]) for r in rows), "call sites;",
      sum(1 for r in rows if not r[2]), "pairs with no direct call site")
