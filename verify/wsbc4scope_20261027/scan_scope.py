"""D4-WSB-C4-SCOPE static store scan for the BROAD and SOLV bands.

  py -3.12 verify/wsbc4scope_20261027/scan_scope.py [--detail] > verify/wsbc4scope_20261027/scope_stores.tsv

Reads ONLY original/MASHED.exe.unpatched of the main checkout (SHA-256 checked against the anchor).
For every hooks.csv row at C3 or C4 in BROAD 0x00558000-0x0055efff and SOLV 0x00566000-0x0056ffff:

- Body = recursive descent from the entry (jcc both ways, jmp imm followed, ret ends a path). A jmp imm
  outside [entry, next hooks.csv rva) is recorded as a tail jump and not followed. Indirect jmp ends the path
  and is recorded. Stricter than the linear sweep of verify/wsbbvh_20261027/scan_blindspot.py: bytes after a
  ret that no path reaches (padding, another function) are not scanned.
- ESP delta d = entry_esp - esp per path: push/pop (+-4), add/sub esp imm, `and esp, imm` (flagged ALIGN),
  `mov esp, ebp`/`leave` from a tracked ebp, the stack probe 0x004a3440 (= sub esp, eax with eax from the last
  `mov eax, imm`), and the callee's own `ret N` for every direct call (callee walked for its ret immediates).
- Register provenance per path: entry regs are in_<reg>; `mov r, [esp+X]` on an entry slot k>=4 gives pK
  (param_K, K = slot/4); `mov r, [b+disp]` gives *(prov(b)+disp); `lea`/`add`/`sub` imm keep the base;
  `mov ebp, esp` / `lea r, [esp+X]` give a stack address; any other write to r gives `?`; a call kills
  eax/ecx/edx.
- Every write-access memory operand (capstone CS_AC_WRITE, plus x87 fst/fstp/fist/fistp/fisttp/fbstp/fnst*),
  and every string store (stos/movs, destination edi), is classified:
    local   stack, entry offset < 0 (own frame)
    arg     stack, entry offset >= 4 (an entry stack slot = the caller's frame)
    retaddr stack, entry offset 0..3
    global  absolute address
    ptr     anything else, printed as [prov(base)+disp(+index*scale)]
  `local` is the only class inside the function's own frame.

Columns: band rva conf status subsys wrapper wfile c3_evidence insns rets d_ok calls tail indirect
         n_local n_arg n_global n_ptr n_str stores_outside_frame
--detail prints, per row, every non-local store with its address and operand.
"""
import bisect, csv, hashlib, re, sys
from pathlib import Path
import pefile, capstone
from capstone import x86_const as X

WT = Path(__file__).resolve().parents[2]
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
EXE = MAIN / "original" / "MASHED.exe.unpatched"
ANCHOR = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
assert hashlib.sha256(EXE.read_bytes()).hexdigest().upper() == ANCHOR, "anchor mismatch"
pe = pefile.PE(str(EXE), fast_load=True)
text = next(s for s in pe.sections if s.Name.startswith(b".text"))
TB = 0x400000 + text.VirtualAddress; CODE = text.get_data(); TE = TB + len(CODE)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
PROBE = 0x004a3440
X87W = {"fst", "fstp", "fist", "fistp", "fisttp", "fbstp", "fnstcw", "fstcw", "fnstsw", "fstsw",
        "fnstenv", "fstenv", "fnsave", "fsave"}
R32 = {X.X86_REG_EAX: "eax", X.X86_REG_EBX: "ebx", X.X86_REG_ECX: "ecx", X.X86_REG_EDX: "edx",
       X.X86_REG_ESI: "esi", X.X86_REG_EDI: "edi", X.X86_REG_EBP: "ebp"}
SUB = {}  # 16/8-bit alias -> 32-bit name
for full, parts in (("eax", "ax al ah"), ("ebx", "bx bl bh"), ("ecx", "cx cl ch"), ("edx", "dx dl dh"),
                    ("esi", "si"), ("edi", "di"), ("ebp", "bp")):
    for p in parts.split(): SUB[p] = full
    SUB[full] = full

_cache = {}
def ins_at(pc):
    if pc in _cache: return _cache[pc]
    i = next(md.disasm(CODE[pc - TB:pc - TB + 16], pc), None) if TB <= pc < TE else None
    _cache[pc] = i; return i

_pop = {}
def callee_pop(t, depth=0):
    """Set of `ret N` immediates reachable from t without following calls (None if none found)."""
    if t in _pop: return _pop[t]
    _pop[t] = None
    seen, work, rets = set(), [t], set()
    while work and len(seen) < 6000:
        pc = work.pop()
        while pc not in seen:
            seen.add(pc); i = ins_at(pc)
            if i is None: break
            m = i.mnemonic
            if m in ("ret", "retn"):
                rets.add(i.operands[0].imm if i.operands else 0); break
            if m == "jmp":
                if i.operands[0].type == X.X86_OP_IMM: pc = i.operands[0].imm; continue
                break
            if m.startswith("j") or m in ("loop", "jecxz"): work.append(i.operands[0].imm)
            pc = i.address + i.size
    _pop[t] = rets or None
    return _pop[t]

def reg_name(r):
    n = md.reg_name(r) if r else ""
    return SUB.get(n, n)

def prov_str(p):
    if isinstance(p, tuple): return f"stk{p[1]:+#x}"
    return p

def classify(op, d, regs, align):
    """-> (class, text). Stack offsets are entry-relative (entry_esp + off)."""
    mm = op.mem; base = reg_name(mm.base); idx = reg_name(mm.index)
    if base == "esp" or (base in regs and isinstance(regs[base], tuple)):
        off = (mm.disp - d) if base == "esp" else (regs[base][1] + mm.disp)
        if idx:
            return ("ptr", f"[stack{off:+#x}+{idx}*{mm.scale}] (indexed stack)")
        sz = op.size or 4
        u = " ALIGN" if (align and base == "esp") else ""
        if off + sz <= 0 and not (align and base == "esp" and off + sz > -8):
            return ("local", f"stk{off:+#x}{u}")
        if off >= 4: return ("arg", f"arg slot entry{off:+#x} (param_{off // 4}){u}")
        if align and base == "esp": return ("ptr", f"[stk{off:+#x}] ALIGN-ambiguous")
        return ("retaddr", f"stk{off:+#x}")
    if not base and not idx:
        return ("global", f"[{mm.disp & 0xffffffff:#010x}]")
    b = prov_str(regs.get(base, "?")) if base else ""
    s = f"[{b}({base})" if base else "["
    if mm.disp: s += f"{mm.disp:+#x}"
    if idx: s += f"+{prov_str(regs.get(idx, '?'))}({idx})*{mm.scale}"
    return ("ptr", s + "]")

def walk(entry, hi):
    init = {n: f"in_{n}" for n in R32.values()}
    work = [(entry, 0, init, False, None)]
    seen = {}; stores = []; calls = []; rets = []; tails = []; indirect = []; conflicts = []; n = 0
    while work:
        pc, d, regs, align, eaxv = work.pop()
        regs = dict(regs)
        while True:
            if pc in seen:
                if seen[pc] != d: conflicts.append((pc, seen[pc], d))
                break
            seen[pc] = d; i = ins_at(pc); n += 1
            if i is None: indirect.append((pc, "UNDECODABLE")); break
            m = i.mnemonic; nxt = i.address + i.size
            # --- stores
            core = m.split()[-1]
            is_str = core in ("stosb", "stosw", "stosd", "movsb", "movsw") or (core == "movsd" and "xmm" not in i.op_str)
            if is_str:
                stores.append((pc, "str", f"{m} -> edi={prov_str(regs.get('edi', '?'))} ecx={prov_str(regs.get('ecx', '?'))}"))
            for op in ([] if is_str else i.operands):
                if op.type == X.X86_OP_MEM and ((op.access & capstone.CS_AC_WRITE) or m in X87W) and m != "lea":
                    c, t = classify(op, d, regs, align)
                    stores.append((pc, c, f"{t}  ({m} {i.op_str})"))
            # --- register provenance
            ops = i.operands
            def setr(o, v):
                if o.type == X.X86_OP_REG:
                    nm = reg_name(o.reg)
                    if nm in R32.values():
                        regs[nm] = v if md.reg_name(o.reg) == nm else "?"
            handled = False
            if m == "mov" and len(ops) == 2 and ops[0].type == X.X86_OP_REG:
                src = ops[1]
                if src.type == X.X86_OP_MEM:
                    b = reg_name(src.mem.base)
                    if b == "esp" and not src.mem.index:
                        off = src.mem.disp - d
                        setr(ops[0], f"p{off // 4}" if off >= 4 and off % 4 == 0 else f"*stk{off:+#x}")
                    elif b in regs and not src.mem.index:
                        setr(ops[0], f"*({prov_str(regs[b])}{src.mem.disp:+#x})")
                    elif not b and not src.mem.index:
                        setr(ops[0], f"*[{src.mem.disp & 0xffffffff:#x}]")
                    else: setr(ops[0], "?")
                elif src.type == X.X86_OP_REG:
                    s = reg_name(src.reg)
                    if s == "esp": setr(ops[0], ("stk", -d))
                    else: setr(ops[0], regs.get(s, "?"))
                elif src.type == X.X86_OP_IMM:
                    setr(ops[0], f"#{src.imm & 0xffffffff:#x}")
                    if reg_name(ops[0].reg) == "eax": eaxv = src.imm
                handled = True
                if reg_name(ops[0].reg) == "esp":  # mov esp, ebp
                    s = reg_name(src.reg) if src.type == X.X86_OP_REG else ""
                    if isinstance(regs.get(s), tuple): d = -regs[s][1]; align = False
                    else: indirect.append((pc, "esp from untracked reg"))
            elif m == "lea" and ops[0].type == X.X86_OP_REG:
                mm = ops[1].mem; b = reg_name(mm.base)
                if mm.index: setr(ops[0], "?")
                elif b == "esp": setr(ops[0], ("stk", mm.disp - d))
                elif isinstance(regs.get(b), tuple): setr(ops[0], ("stk", regs[b][1] + mm.disp))
                elif b in regs: setr(ops[0], f"{prov_str(regs[b])}{mm.disp:+#x}")
                else: setr(ops[0], "?")
                handled = True
            elif m in ("add", "sub") and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_IMM:
                nm = reg_name(ops[0].reg); k = ops[1].imm if m == "add" else -ops[1].imm
                if nm == "esp": d -= k
                elif isinstance(regs.get(nm), tuple): regs[nm] = ("stk", regs[nm][1] + k)
                elif nm in regs: regs[nm] = f"{prov_str(regs[nm])}{k:+#x}" if regs[nm] != "?" else "?"
                handled = True
            elif m == "and" and ops[0].type == X.X86_OP_REG and reg_name(ops[0].reg) == "esp":
                align = True; handled = True
            elif m == "leave":
                if isinstance(regs.get("ebp"), tuple): d = -regs["ebp"][1] - 4; align = False
                else: indirect.append((pc, "leave with untracked ebp"))
                regs["ebp"] = "?"; handled = True
            if m in ("push", "pushfd", "pushal", "pushad"):
                d += 32 if m in ("pushal", "pushad") else 4
            elif m in ("pop", "popfd", "popal", "popad"):
                d -= 32 if m in ("popal", "popad") else 4
                if m == "pop" and ops[0].type == X.X86_OP_REG:
                    regs[reg_name(ops[0].reg)] = "?"
            if not handled and m not in ("push", "pop"):
                try:
                    _, wr = i.regs_access()
                    for r in wr:
                        nm = reg_name(r)
                        if nm in regs: regs[nm] = "?"
                        if nm == "esp" and m not in ("call", "ret", "retn", "leave"): indirect.append((pc, f"esp written by {m}"))
                except capstone.CsError:
                    pass
            # --- control flow
            if m == "call":
                if ops[0].type == X.X86_OP_IMM:
                    t = ops[0].imm; calls.append((pc, f"{t:#010x}"))
                    if t == PROBE:
                        d += (eaxv or 0)
                        if eaxv is None: indirect.append((pc, "probe with unknown eax"))
                    else:
                        pp = callee_pop(t)
                        if pp is None: indirect.append((pc, f"callee {t:#x} no ret found"))
                        elif len(pp) > 1: indirect.append((pc, f"callee {t:#x} rets {sorted(pp)}"))
                        else: d -= next(iter(pp))
                else:
                    calls.append((pc, f"indirect {i.op_str}"))
                    indirect.append((pc, f"indirect call {i.op_str} (pop unknown)"))
                for r in ("eax", "ecx", "edx"): regs[r] = "?"
                eaxv = None
            if m in ("ret", "retn"):
                rets.append((pc, d)); break
            if m == "jmp":
                if ops[0].type == X.X86_OP_IMM:
                    t = ops[0].imm
                    if entry <= t < hi: pc = t; continue
                    tails.append((pc, f"{t:#010x}")); break
                indirect.append((pc, f"indirect jmp {i.op_str}")); break
            if m.startswith("j") or m in ("loop", "loope", "loopne", "jecxz"):
                work.append((ops[0].imm, d, dict(regs), align, eaxv))
            pc = nxt
    return dict(n=n, stores=stores, calls=calls, rets=rets, tails=tails, indirect=indirect,
                conflicts=conflicts, lo=min(seen), hi=max(seen))

# --- wrappers in the port source (this worktree)
src_txt = {p: p.read_text(encoding="utf-8", errors="replace") for p in (WT / "mashedmod" / "src" / "mashed_re").rglob("*.cpp")}
def wrapper_of(rva):
    hits = []
    for p, t in src_txt.items():
        for m in re.finditer(r"ShadowAB::(Run|RunTracked|RunRegion)\(\s*\w+\s*,\s*(?:&)?(\w+)", t):
            if rva in m.group(2).lower():
                region = ""
                if m.group(1) == "RunRegion":
                    tail = t[m.end():m.end() + 200].split(";")[0]
                    region = " region=" + re.sub(r"\s+", " ", tail.strip(", ")).split(", param")[0]
                hits.append((m.group(1) + region, p.name))
    return hits

def main():
    detail = "--detail" in sys.argv
    rows = [r for r in csv.reader(open(WT / "hooks.csv", encoding="utf-8", errors="replace"))
            if r and re.fullmatch(r"[0-9a-fA-F]{8}", r[0])]
    rv = sorted({int(r[0], 16) for r in rows})
    BANDS = [("BROAD", 0x00558000, 0x0055f000), ("SOLV", 0x00566000, 0x00570000)]
    if not detail:
        print("band\trva\tconf\tstatus\tsubsys\twrapper\twfile\tc3_evidence\tinsns\trets\td_ok\tcalls\ttail\tindirect"
              "\tn_local\tn_arg\tn_global\tn_ptr\tn_str\tstores_outside_frame")
    for r in sorted(rows, key=lambda r: int(r[0], 16)):
        a = int(r[0], 16)
        band = next((b for b, lo, hi in BANDS if lo <= a < hi), None)
        if band is None or r[3] not in ("C3", "C4"): continue
        k = bisect.bisect_right(rv, a); hi = rv[k] if k < len(rv) else a + 0x1000
        w = walk(a, hi)
        cnt = {c: 0 for c in ("local", "arg", "retaddr", "global", "ptr", "str")}
        for _, c, _t in w["stores"]: cnt[c] += 1
        wr = wrapper_of(r[0].lower())
        wrap = ";".join(x[0] for x in wr) or "none"; wf = ";".join(sorted({x[1] for x in wr}))
        d_ok = all(dd == 0 for _, dd in w["rets"]) and not w["conflicts"]
        outside = cnt["arg"] + cnt["retaddr"] + cnt["global"] + cnt["ptr"] + cnt["str"]
        if detail:
            print(f"== {band} 0x{r[0].lower()} {r[3]}/{r[4]} {r[2]} wrapper={wrap} ({wf}) body {w['lo']:#x}..{w['hi']:#x} "
                  f"{w['n']} insns rets={[(hex(p), dd) for p, dd in w['rets']]} conflicts={len(w['conflicts'])}")
            for pc, c, t in w["stores"]:
                if c != "local": print(f"   {pc:08x} {c:7s} {t}")
            for pc, t in w["calls"]: print(f"   {pc:08x} call    {t}")
            for pc, t in w["tails"]: print(f"   {pc:08x} tailjmp {t}")
            for pc, t in w["indirect"]: print(f"   {pc:08x} NOTE    {t}")
            continue
        ev = (r[6] + " | " + r[7])[:70].replace("\t", " ")
        print(f"{band}\t0x{r[0].lower()}\t{r[3]}\t{r[4]}\t{r[2][:28]}\t{wrap}\t{wf}\t{ev}\t{w['n']}\t{len(w['rets'])}\t"
              f"{'yes' if d_ok else 'NO'}\t{len(w['calls'])}\t{len(w['tails'])}\t{len(w['indirect'])}\t"
              f"{cnt['local']}\t{cnt['arg']}\t{cnt['global']}\t{cnt['ptr'] + cnt['retaddr']}\t{cnt['str']}\t{outside}")

if __name__ == "__main__":
    main()
