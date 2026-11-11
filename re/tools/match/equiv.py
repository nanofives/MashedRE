"""Tool-decided equivalence of a near-match body (EQUIV-CHECK, L-122 direction, 2026-11-11).

    py -3.12 re/tools/match/equiv.py re/match/s09/0045a3a0.c [--only 0x0045a3a0] [--json] [--trace]

Input: a `// MATCH:` source (compiled with the recorded DDK compiler through match.py) and the original's bytes
(original/MASHED.exe.unpatched). Output per function:

    EQUIV [classes]                          the two bodies compute the same observable behaviour
    EQUIV-IF-CALLERS-IGNORE(<regs>) [...]    same, except the listed registers at return (caller scan attached)
    NOT-PROVEN <kind>: <detail>              the first difference the rules cannot explain

Sound before complete. The verdict is a proof by symbolic product execution, not a byte pattern:

* Both bodies are executed symbolically from one shared entry state (registers, stack, memory are symbols). Every
  value is a hash-consed term in a canonical form. Every rewrite the canonical form applies is an identity of
  32-bit machine arithmetic, so two equal terms always denote equal values.
* The two control-flow graphs are explored as a product from the entry pair. A region runs on each side until a
  conditional branch, an indirect jump or a return. At a conditional branch the two conditions must be equal or
  complementary (then the successors pair crosswise). At a return every observable must be equal.
* Observables: the sequence of calls (target, every stack argument the callee reads, every register it reads,
  memory, x87 state), memory at every call and return, EAX/ECX/EDX/ST0/EBX/ESI/EDI/EBP/ESP at every return,
  the return address and `ret` pop count. Loop heads are joined with an equality partition (optimistic value
  numbering, refined until it is inductive).
* A difference in any observable, any instruction the model does not cover, and any construct whose effect is
  not modelled exactly gives NOT-PROVEN.

The named classes (re/tools/match/equiv_rules.md has the exact conditions) are reported by an attribution pass that
runs after the proof: ENC, SIB, REG, ORDER, LAYOUT, COMM, plus FRAME (the proof needed the private-frame
assumption A1) and VALUE-FLOW (proven, but not one of the six syntactic classes, e.g. a value kept in a register on
one side and reloaded from its stack slot on the other). Attribution never changes a verdict.
"""
from __future__ import annotations

import argparse
import bisect
import difflib
import json
import pathlib
import re
import struct
import sys

import capstone
from capstone import x86 as X

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match  # noqa: E402

M32 = 0xFFFFFFFF
MASK = {1: 1, 8: 0xFF, 16: 0xFFFF, 32: M32, 64: (1 << 64) - 1, 80: (1 << 80) - 1}
CALLEE_SAVED = ("ebx", "esi", "edi", "ebp")
RELAX_ICALL = False
LAST_PCS: dict = {}   # pcs executed by the last proof, per side (mutation harness)   # --relax-icall: assume A4' (a call through a pointer reads no register); reported separately
GPR = ("eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi")
SUB = {}
for _r in "abcd":
    SUB[f"e{_r}x"] = (f"e{_r}x", 0, 32)
    SUB[f"{_r}x"] = (f"e{_r}x", 0, 16)
    SUB[f"{_r}l"] = (f"e{_r}x", 0, 8)
    SUB[f"{_r}h"] = (f"e{_r}x", 8, 8)
for _r in ("si", "di", "bp", "sp"):
    SUB[f"e{_r}"] = (f"e{_r}", 0, 32)
    SUB[_r] = (f"e{_r}", 0, 16)


class NotProven(Exception):
    def __init__(self, kind, detail=""):
        super().__init__(f"{kind}: {detail}")
        self.kind = kind
        self.detail = detail


class Fallback(Exception):
    """The private-frame model (A1) does not apply (a stack address escapes); rerun in strict mode."""


# ====================================================================== terms
class T:
    __slots__ = ("op", "w", "a", "id")

    def __repr__(self):
        return show(self)

    def __lt__(self, o):
        return self.id < o.id


_TAB: dict = {}


def reset_terms():
    LAST_PCS.clear()
    _TAB.clear()
    _HAS.clear()


def mk(op, w, *a):
    key = (op, w, a)
    t = _TAB.get(key)
    if t is None:
        t = T()
        t.op, t.w, t.a, t.id = op, w, a, len(_TAB)
        _TAB[key] = t
    return t


def C(v, w=32):
    return mk("c", w, v & MASK[w])


def S(name, w=32):
    return mk("s", w, name)


def isc(t):
    return t.op == "c"


def show(t, depth=0):
    if not isinstance(t, T):
        return repr(t)
    if depth > 6:
        return "..."
    if t.op == "c":
        return hex(t.a[0]) if t.w != 1 else str(t.a[0])
    if t.op == "s":
        return str(t.a[0])
    if t.op == "lin":
        parts = [(show(x, depth + 1) if c == 1 else f"{show(x, depth + 1)}*{c if c < 0x80000000 else c - (1 << 32)}")
                 for x, c in t.a[0]]
        if t.a[1]:
            k = t.a[1]
            parts.append(hex(k) if k < 0x80000000 else "-" + hex((1 << 32) - k))
        return "(" + "+".join(parts) + ")"
    return f"{t.op}{t.w}(" + ",".join(show(x, depth + 1) for x in t.a) + ")"


def to_lin(t):
    if t.w != 32:
        return {t: 1}, 0
    if t.op == "c":
        return {}, t.a[0]
    if t.op == "lin":
        return dict(t.a[0]), t.a[1]
    return {t: 1}, 0


def from_lin(d, k):
    d = {x: c & M32 for x, c in d.items() if c & M32}
    k &= M32
    if not d:
        return C(k)
    if len(d) == 1 and k == 0:
        (x, c), = d.items()
        if c == 1:
            return x
    return mk("lin", 32, tuple(sorted(d.items(), key=lambda p: p[0].id)), k)


def add(a, b):
    if a.w != 32 or b.w != 32:
        w = a.w
        return ext(add(zx(a, 32), zx(b, 32)), 0, w)
    da, ka = to_lin(a)
    db, kb = to_lin(b)
    for x, c in db.items():
        da[x] = da.get(x, 0) + c
    return from_lin(da, ka + kb)


def mulc(a, k):
    if a.w != 32:
        return ext(mulc(zx(a, 32), k), 0, a.w)
    d, c = to_lin(a)
    return from_lin({x: v * k for x, v in d.items()}, c * k)


def sub(a, b):
    if a.w != 32:
        return ext(add(zx(a, 32), mulc(zx(b, 32), -1)), 0, a.w)
    return add(a, mulc(b, -1))


def zx(t, w):
    if t.w == w:
        return t
    if t.w > w:
        return ext(t, 0, w)
    if t.op == "c":
        return C(t.a[0], w)
    if t.op == "zx":
        return zx(t.a[0], w)
    return mk("zx", w, t)


def sx(t, w):
    if t.w == w:
        return t
    if t.op == "c":
        v = t.a[0]
        if v >> (t.w - 1):
            v -= 1 << t.w
        return C(v, w)
    return mk("sx", w, t)


def ext(t, lo, w):
    if lo == 0 and w == t.w:
        return t
    if t.op == "c":
        return C(t.a[0] >> lo, w)
    if t.op == "zx":
        inner = t.a[0]
        if lo + w <= inner.w:
            return ext(inner, lo, w)
        if lo >= inner.w:
            return C(0, w)
    if t.op == "sx":
        inner = t.a[0]
        if lo + w <= inner.w:
            return ext(inner, lo, w)
    if t.op == "ins":
        base, v, vlo = t.a
        if lo == vlo and w == v.w:
            return v
        if lo >= vlo + v.w or lo + w <= vlo:
            return ext(base, lo, w)
        if vlo <= lo and lo + w <= vlo + v.w:
            return ext(v, lo - vlo, w)
    if t.op == "ext":
        return ext(t.a[0], t.a[1] + lo, w)
    if t.op == "cat" and t.w == 64:
        if lo + w <= 32:
            return ext(t.a[0], lo, w)
        if lo >= 32:
            return ext(t.a[1], lo - 32, w)
    if t.op == "lin" and lo == 0:
        # low bits of a sum depend only on the low bits of the addends; keep as is (canonical enough)
        pass
    return mk("ext", w, t, lo)


def cat64(lo32, hi32):
    if lo32.op == "ext" and hi32.op == "ext" and lo32.a[0] is hi32.a[0] and lo32.a[1] == 0 and hi32.a[1] == 32 \
            and lo32.a[0].w == 64:
        return lo32.a[0]
    return mk("cat", 64, lo32, hi32)


def ins(base, v, lo):
    """base (32-bit) with bits [lo, lo+v.w) replaced by v."""
    if v.w == base.w:
        return v
    if v.op == "ext" and v.a[0] is base and v.a[1] == lo:
        return base
    if base.op == "c" and v.op == "c":
        m = MASK[v.w] << lo
        return C((base.a[0] & ~m) | (v.a[0] << lo), base.w)
    if base.op == "ins" and base.a[2] == lo and base.a[1].w == v.w:
        return ins(base.a[0], v, lo)
    if base.op == "c" and base.a[0] == 0 and lo == 0:
        return zx(v, base.w)
    if base.op == "zx" and lo == 0 and base.a[0].w <= v.w:
        return zx(v, base.w)
    return mk("ins", base.w, base, v, lo)


COMMUTATIVE = {"and", "or", "xor", "mul", "imulhi", "mulhi"}


def bop(op, w, a, b):
    """and / or / xor with flattening, constant folding and sorted operands (COMM)."""
    if op in ("and", "or", "xor"):
        items = []
        for x in (a, b):
            if x.op == op and x.w == w:
                items.extend(x.a)
            else:
                items.append(x)
        k = None
        rest = []
        for x in items:
            if x.op == "c":
                v = x.a[0]
                if k is None:
                    k = v
                elif op == "and":
                    k &= v
                elif op == "or":
                    k |= v
                else:
                    k ^= v
            else:
                rest.append(x)
        if op == "xor":
            cnt = {}
            for x in rest:
                cnt[x] = cnt.get(x, 0) + 1
            rest = [x for x, n in cnt.items() if n % 2]
        else:
            rest = list(dict.fromkeys(rest))
        if k is not None:
            if op == "and" and k == 0:
                return C(0, w)
            if op == "or" and k == MASK[w]:
                return C(MASK[w], w)
            if (op == "and" and k == MASK[w]) or (op in ("or", "xor") and k == 0):
                k = None
        if not rest:
            return C(k or 0, w) if k is not None else C(MASK[w] if op == "and" else 0, w)
        args = sorted(rest, key=lambda t: t.id)
        if k is not None:
            args.append(C(k, w))
        if len(args) == 1:
            return args[0]
        return mk(op, w, *args)
    if op == "mul":
        if a.op == "c" and w == 32:
            return mulc(b, a.a[0])
        if b.op == "c" and w == 32:
            return mulc(a, b.a[0])
        if a.op == "c" and b.op == "c":
            return C(a.a[0] * b.a[0], w)
    if op in COMMUTATIVE:
        a, b = sorted((a, b), key=lambda t: t.id)
    return mk(op, w, a, b)


def shift(op, w, a, cnt):
    """shl / shr / sar / rol / ror; cnt is an 8-bit term already masked to 5 bits when constant."""
    if cnt.op == "c":
        k = cnt.a[0] & 31
        if k == 0:
            return a
        if a.op == "c":
            v = a.a[0]
            if op == "shl":
                return C(v << k, w)
            if op == "shr":
                return C(v >> k, w)
            if op == "sar":
                if v >> (w - 1):
                    v -= 1 << w
                return C(v >> k, w)
            if op == "rol":
                k %= w
                return C((v << k) | (v >> (w - k)), w)
            if op == "ror":
                k %= w
                return C((v >> k) | (v << (w - k)), w)
        if op == "shl":
            return mulc(a, 1 << k) if w == 32 else ext(mulc(zx(a, 32), 1 << k), 0, w)
        return mk(op, w, a, C(k, 8))
    return mk(op, w, a, cnt)


# ---------------------------------------------------------------- predicates
NEG_CC = {"e": "ne", "ne": "e", "b": "ae", "ae": "b", "be": "a", "a": "be", "l": "ge", "ge": "l", "le": "g", "g": "le",
          "s": "ns", "ns": "s", "o": "no", "no": "o", "p": "np", "np": "p"}
TRUE, FALSE = None, None


def P(rel, a, b):
    if a.op == "c" and b.op == "c":
        x, y, w = a.a[0], b.a[0], a.w
        sx_ = x - (1 << w) if x >> (w - 1) else x
        sy_ = y - (1 << w) if y >> (w - 1) else y
        v = {"eq": x == y, "ne": x != y, "ult": x < y, "ule": x <= y, "slt": sx_ < sy_, "sle": sx_ <= sy_}[rel]
        return C(int(v), 1)
    if rel in ("eq", "ne"):
        if a is b:
            return C(int(rel == "eq"), 1)
        # eq(x - y, 0) == eq(x, y)
        if b.op == "c" and b.a[0] == 0 and a.op == "lin" and a.w == 32:
            d, k = to_lin(a)
            neg = {x: c for x, c in d.items() if c == M32}
            pos = {x: c for x, c in d.items() if c == 1}
            if len(neg) == 1 and len(pos) + len(neg) == len(d):
                (y, _), = neg.items()
                return P(rel, from_lin(pos, k), y)
        a, b = sorted((a, b), key=lambda t: t.id)
    if rel in ("ule",) and b.op == "c" and b.a[0] == MASK[b.w]:
        return C(1, 1)
    if rel == "ult" and b.op == "c" and b.a[0] == 0:
        return C(0, 1)
    if rel == "ule" and a.op == "c" and a.a[0] == 0:
        return C(1, 1)
    return mk("P", 1, rel, a, b)


def Pcc(cc, F):
    return mk("P", 1, "cc", cc, F)


def negate(p):
    if p.op == "c":
        return C(1 - p.a[0], 1)
    rel, a, b = p.a
    if rel == "cc":
        return Pcc(NEG_CC[a], b)
    return {"eq": lambda: P("ne", a, b), "ne": lambda: P("eq", a, b), "slt": lambda: P("sle", b, a),
            "sle": lambda: P("slt", b, a), "ult": lambda: P("ule", b, a), "ule": lambda: P("ult", b, a)}[rel]()


def zero(w):
    return C(0, w)


def pred(cc, F):
    """Condition `cc` over flags term F, in canonical form."""
    kind = F.a[0]
    if kind == "sub":
        a, b = F.a[1], F.a[2]
        m = {"e": ("eq", a, b), "ne": ("ne", a, b), "b": ("ult", a, b), "ae": ("ule", b, a), "be": ("ule", a, b),
             "a": ("ult", b, a), "l": ("slt", a, b), "ge": ("sle", b, a), "le": ("sle", a, b), "g": ("slt", b, a)}
        if cc in m:
            return P(*m[cc])
        if cc in ("s", "ns"):
            r = sub(a, b)
            return P("slt", r, zero(r.w)) if cc == "s" else P("sle", zero(r.w), r)
        return Pcc(cc, F)
    if kind == "logic":
        r = F.a[1]
        z = zero(r.w)
        m = {"e": ("eq", r, z), "ne": ("ne", r, z), "s": ("slt", r, z), "l": ("slt", r, z), "ns": ("sle", z, r),
             "ge": ("sle", z, r), "le": ("sle", r, z), "g": ("slt", z, r), "be": ("eq", r, z), "a": ("ne", r, z)}
        if cc in m:
            return P(*m[cc])
        if cc in ("b", "o"):
            return C(0, 1)
        if cc in ("ae", "no"):
            return C(1, 1)
        return Pcc(cc, mk("F", 0, "res", r))
    if kind in ("add", "incdec", "neg", "shres", "res"):
        r = F.a[-1] if kind != "res" else F.a[1]
        if kind == "add":
            r = add(F.a[1], F.a[2])
        if kind == "neg":
            r = sub(zero(F.a[1].w), F.a[1])
            if cc == "b":
                return P("ne", F.a[1], zero(F.a[1].w))
            if cc == "ae":
                return P("eq", F.a[1], zero(F.a[1].w))
        z = zero(r.w)
        if cc == "e":
            return P("eq", r, z)
        if cc == "ne":
            return P("ne", r, z)
        if cc == "s":
            return P("slt", r, z)
        if cc == "ns":
            return P("sle", z, r)
        if kind == "incdec" and cc in ("b", "ae"):
            return pred(cc, F.a[1])
        if kind in ("res", "shres", "incdec") and cc in ("p", "np"):
            return Pcc(cc, mk("F", 0, "res", r))
        return Pcc(cc, F)
    if kind == "keepzs":         # rol/ror: ZF/SF/PF untouched, CF/OF changed
        if cc in ("e", "ne", "s", "ns", "p", "np"):
            return pred(cc, F.a[1])
        return Pcc(cc, F)
    return Pcc(cc, F)


def F_(kind, *a):
    return mk("F", 0, kind, *a)


# ---------------------------------------------------------------- memory
MEM0 = None


def disjoint(a, sa, b, sb):
    da, ka = to_lin(a)
    db, kb = to_lin(b)
    if da != db:
        return False
    diff = (kb - ka) & M32
    return diff >= sa and ((1 << 32) - diff) >= sb


def same_addr(a, b):
    return a is b


def contained(a, sa, b, sb):
    """load [a, a+sa) inside store [b, b+sb): returns byte offset or None."""
    da, ka = to_lin(a)
    db, kb = to_lin(b)
    if da != db:
        return None
    off = (ka - kb) & M32
    if off + sa <= sb:
        return off
    return None


def store(m, a, v, sz):
    if m.op == "st":
        pm, a2, v2, sz2 = m.a
        if a2 is a and sz2 == sz:
            return store(pm, a, v, sz)
        if disjoint(a, sz, a2, sz2) and a.id < a2.id:
            return mk("st", 0, store(pm, a, v, sz), a2, v2, sz2)
    return mk("st", 0, m, a, v, sz)


def load(m, a, sz, ctx=None):
    w = 8 * sz
    if ctx is not None:
        k = ctx.ro_fold(a, sz)
        if k is not None:
            return k
    cur = m
    while cur.op == "st":
        pm, a2, v2, sz2 = cur.a
        if a2 is a and sz2 == sz:
            return v2
        off = contained(a, sz, a2, sz2)
        if off is not None:
            return ext(v2, 8 * off, w)
        if disjoint(a, sz, a2, sz2):
            cur = pm
            continue
        break
    return mk("ld", w, cur, a)


_HAS: dict = {}


def has_sym(t, name, memo=None):
    if memo is None:
        memo = _HAS
    if not isinstance(t, T):
        if isinstance(t, tuple):
            return any(has_sym(x, name, memo) for x in t)
        return False
    r = memo.get((t.id, name))
    if r is not None:
        return r
    if t.op == "s":
        r = t.a[0] == name
    else:
        r = any(has_sym(x, name, memo) for x in t.a if isinstance(x, (T, tuple)))
    memo[(t.id, name)] = r
    return r


# ====================================================================== code providers
_MD = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
_MD.detail = True

RO_FLAGS = 0x80000000  # IMAGE_SCN_MEM_WRITE


def coff_full(path):
    """match.coff plus section characteristics and symbol storage class."""
    secs, syms = match.coff(path)
    d = pathlib.Path(path).read_bytes()
    _, nsec, _, _, _, opt, _ = struct.unpack_from("<HHIIIHH", d, 0)
    for i in range(nsec):
        o = 20 + opt + 40 * i
        secs[i]["chars"] = struct.unpack_from("<I", d, o + 36)[0]
    return secs, syms


class Image:
    def __init__(self, exe=None):
        p = match.pe()
        self.pe = p
        self.base = p.OPTIONAL_HEADER.ImageBase
        self.secs = []
        for s in p.sections:
            lo = self.base + s.VirtualAddress
            hi = lo + max(s.Misc_VirtualSize, s.SizeOfRawData)
            self.secs.append((lo, hi, bool(s.Characteristics & 0x80000000), s.Name.rstrip(b"\0").decode()))
        self.lo = min(s[0] for s in self.secs)
        self.hi = self.base + p.OPTIONAL_HEADER.SizeOfImage
        self.data = p.get_memory_mapped_image()
        d = p.OPTIONAL_HEADER.DATA_DIRECTORY[12]          # IMAGE_DIRECTORY_ENTRY_IAT
        self.iat_lo, self.iat_hi = self.base + d.VirtualAddress, self.base + d.VirtualAddress + d.Size
        self.iat = {}
        try:
            p.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
            for e in getattr(p, "DIRECTORY_ENTRY_IMPORT", []):
                for imp in e.imports:
                    self.iat[imp.address] = (e.dll.decode(), (imp.name or b"").decode())
        except Exception:
            pass

    def bytes(self, va, n):
        o = va - self.base
        if o < 0 or o + n > len(self.data):
            return None
        return self.data[o:o + n]

    def readonly(self, va, n):
        if self.iat_lo <= va + n and va < self.iat_hi:
            return False
        for lo, hi, w, name in self.secs:
            if lo <= va and va + n <= hi:
                return not w and name != ".idata"
        return False

    def in_image(self, va):
        return self.lo <= va < self.hi


import pefile  # noqa: E402

_IMG = None


def image():
    global _IMG
    if _IMG is None:
        _IMG = Image()
    return _IMG


class Ins:
    """A decoded instruction with operand resolution done once per side."""
    __slots__ = ("cs", "addr", "size", "mn", "ops", "relocs", "text", "next")


class Side:
    name = "?"

    def decode(self, pc):
        c = self._cache.get(pc)
        if c is None:
            c = self._decode(pc)
            self._cache[pc] = c
        return c


class ObjSide(Side):
    """Our side: one COMDAT function symbol of a compiled object (offset 0 = function start)."""

    def __init__(self, name, secs, syms, sym, addr, resolver):
        self.name = name
        self._cache = {}
        fns = [s for s in syms if s["sec"] > 0 and s["type"] == 0x20]
        cands = [s for s in fns if s["name"] == sym] or [s for s in fns if undecorate(s["name"]) == undecorate(sym)]
        if not cands:
            cands = [s for s in syms if s["sec"] > 0 and s["type"] == 0x20 and (s["name"] == sym or sym in s["name"])]
        if not cands:
            raise NotProven("no-symbol", sym)
        s = cands[0]
        self.sym = s
        self.secs, self.syms = secs, syms
        self.secidx = s["sec"]
        sec = secs[s["sec"] - 1]
        self.start = s["value"]
        later = sorted(x["value"] for x in syms if x["sec"] == s["sec"] and x["type"] == 0x20 and x["value"] > self.start)
        self.end = later[0] if later else len(sec["data"])
        self.code = sec["data"]
        self.by_index = {x["index"]: x for x in syms}
        self.relocs = {off: (self.by_index.get(si, {}), typ) for off, si, typ in sec["relocs"]}
        self.addr = addr           # the original RVA this symbol replaces (self-reference)
        self.resolver = resolver
        self.entry = self.start

    def internal(self, tgt):
        return self.start <= tgt < self.end

    def _decode(self, pc):
        if not (self.start <= pc < self.end):
            raise NotProven("decode", f"{self.name} pc +0x{pc:x} outside the symbol")
        try:
            cs = next(_MD.disasm(self.code[pc:pc + 16], pc))
        except StopIteration:
            raise NotProven("decode", f"{self.name} undecodable bytes at +0x{pc:x}")
        return cs

    def field(self, cs, which):
        """('imm'|'disp') -> resolved term-producer key or None when the field is not relocated."""
        off = cs.imm_offset if which == "imm" else cs.disp_offset
        if not off:
            return None
        r = self.relocs.get(cs.address + off)
        if r is None:
            return None
        return r[0], r[1], struct.unpack_from("<i", self.code, cs.address + off)[0]

    def label(self, pc):
        return f"+0x{pc - self.start:x}"

    def body(self):
        return self.code[self.start:self.end], self.start


class ImageSide(Side):
    def __init__(self, name, addr, size):
        self.name = name
        self._cache = {}
        self.img = image()
        self.start = addr
        self.end = addr + size
        self.entry = addr
        self.addr = addr

    def internal(self, tgt):
        return self.start <= tgt < self.end

    def _decode(self, pc):
        b = self.img.bytes(pc, 16)
        if b is None:
            raise NotProven("decode", f"{self.name} 0x{pc:08x} outside the image")
        try:
            return next(_MD.disasm(b, pc))
        except StopIteration:
            raise NotProven("decode", f"{self.name} undecodable bytes at 0x{pc:08x}")

    def field(self, cs, which):
        return None

    def label(self, pc):
        return f"0x{pc:08x}"

    def body(self):
        return self.img.bytes(self.start, self.end - self.start) or b"", self.start


# ====================================================================== callee summaries
_SUMM: dict = {}
ALLREGS = frozenset(("eax", "ecx", "edx", "ebx", "esi", "edi", "ebp"))


def resolve_thunk(va):
    img = image()
    for _ in range(4):
        b = img.bytes(va, 6)
        if b is None:
            return va
        if b[0] == 0xE9:
            va = (va + 5 + struct.unpack_from("<i", b, 1)[0]) & M32
            continue
        if b[0] == 0xEB:
            va = (va + 2 + struct.unpack_from("<b", b, 1)[0]) & M32
            continue
        break
    return va


def _reg_parts(name):
    r = SUB.get(name)
    if r is None:
        return None, 0
    parent, lo, w = r
    if w == 32:
        return parent, 7
    if w == 16:
        return parent, 3
    return parent, 1 if lo == 0 else 2


_RETPOP = {}


def ret_pop(va):
    """Pop count of every `ret` in the linear disassembly of the Ghidra body; None unless they all agree."""
    va = resolve_thunk(va)
    if va in _RETPOP:
        return _RETPOP[va]
    size = match.ghidra_size(va)
    out = None
    if size:
        pops = {(c.operands[0].imm if c.operands else 0) for c in _MD.disasm(image().bytes(va, size) or b"", va)
                if c.mnemonic in ("ret", "retn")}
        out = next(iter(pops)) if len(pops) == 1 else None
    _RETPOP[va] = out
    return out


UNKNOWN = {"P": None, "N": None, "livein": ALLREGS, "eax_def": False, "noreturn": False, "known": False}
_INPROG: set = set()
_TOP: dict = {}
_FRAMES: list = []
MAX_DEPTH = 40           # call-graph depth followed (deeper: conservative, uncached)
sys.setrecursionlimit(20000)


def _sfail(va, why):
    return dict(UNKNOWN, why=why)


def summary(va, depth=0):
    """Callee facts from the original image: pop count P, stack-argument slots read N, registers read before
    written (live-in), whether EAX is written on every path to a return. None fields = unknown.

    Order-independent: a result that depended on a cycle (a callee still being summarised) or on the depth cut-off
    is conservative for that query and is not cached, so every query of an address gives the same answer whatever
    was asked before it."""
    va = resolve_thunk(va)
    if va in _SUMM:
        return _SUMM[va]
    top = not _INPROG and depth == 0
    if top and va in _TOP:
        return _TOP[va]           # a query that starts with nothing in progress always gives the same answer
    if va in _INPROG or depth > MAX_DEPTH:
        for f in _FRAMES:
            f["taint"] = True
        return dict(UNKNOWN)
    _INPROG.add(va)
    _FRAMES.append({"taint": False})
    try:
        res = _summary_body(va, depth)
    finally:
        _INPROG.discard(va)
        fr = _FRAMES.pop()
    if not fr["taint"]:
        _SUMM[va] = res
    if top:
        _TOP[va] = res
    return res


def _summary_body(va, depth):
    size = match.ghidra_size(va)
    if size is None:
        return _sfail(va, "r2")
    img = image()
    lo, hi = va, va + size
    later = [e for e in match.function_table() if e > va]
    next_entry = min(later) if later else hi
    states = {va: (0, None, {r: 0 for r in ALLREGS})}
    work = [va]
    livein, N, pops, eax_def = set(), 0, set(), True
    pushed = {}            # entry-relative slot -> register pushed while undefined (a save or an allocation)
    unknown_n = False
    ind_assumed = esp_reset = False
    visited = 0
    while work:
        pc = work.pop()
        esp, ebp, dfn = states[pc]
        dfn = dict(dfn)
        recent = set()
        while True:
            visited += 1
            if visited > 20000:
                return _sfail(va, "r3")
            if pc >= next_entry:
                break          # fell into the next function: the call before it does not return
            b = img.bytes(pc, 16)
            if b is None:
                return _sfail(va, "r4")
            try:
                cs = next(_MD.disasm(b, pc))
            except StopIteration:
                return _sfail(va, "r5")
            mn = cs.mnemonic
            try:
                rd, wr = cs.regs_access()
            except Exception:
                return _sfail(va, "r6")
            rdn = [cs.reg_name(r) for r in rd]
            wrn = [cs.reg_name(r) for r in wr]
            ops = cs.operands
            zero_idiom = mn in ("xor", "sub", "sbb") and len(ops) == 2 and ops[0].type == X.X86_OP_REG \
                and ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg
            save = mn == "push" and len(ops) == 1 and ops[0].type == X.X86_OP_REG and \
                _reg_parts(cs.reg_name(ops[0].reg))[0] in ALLREGS and \
                (dfn.get(_reg_parts(cs.reg_name(ops[0].reg))[0], 0) & 7) != 7 and esp is not None
            if save:
                pushed[esp - 4] = _reg_parts(cs.reg_name(ops[0].reg))[0]
            if mn == "push" and esp is not None:
                recent.add(esp - 4)
            if not zero_idiom and not save:
                for r in rdn:
                    p_, m_ = _reg_parts(r)
                    if p_ in ALLREGS and (dfn[p_] & m_) != m_:
                        livein.add(p_)
            # ESP (or a frame-pointer EBP) copied as a value: the argument area may be reached through the copy
            if mn not in ("push", "pop", "call", "ret", "retn", "leave", "add", "sub"):
                for op in ops:
                    if op.type == X.X86_OP_REG:
                        rn = cs.reg_name(op.reg)
                        if rn == "esp" and not (mn == "mov" and cs.reg_name(ops[0].reg) == "ebp" and
                                                ops[0].type == X.X86_OP_REG):
                            unknown_n = unknown_n or (pc if "pc" in dir() else -1)
                        if rn == "ebp" and ebp is not None and not (mn == "mov" and ops[0].type == X.X86_OP_REG and
                                                                     cs.reg_name(ops[0].reg) in ("esp", "ebp")):
                            unknown_n = unknown_n or (pc if "pc" in dir() else -1)
            # stack argument reads
            for op in ops:
                if op.type == X.X86_OP_MEM and op.mem.base:
                    bn = cs.reg_name(op.mem.base)
                    base_off = esp if bn == "esp" else (ebp if bn == "ebp" else None)
                    if bn in ("esp", "ebp"):
                        if base_off is None or op.mem.index:
                            if bn == "esp" or ebp is None:
                                unknown_n = unknown_n or (pc if "pc" in dir() else -1)
                            continue
                        off = base_off + op.mem.disp
                        if mn == "lea":
                            if off >= 4:
                                unknown_n = unknown_n or (pc if "pc" in dir() else -1)
                            elif any(off <= k < off + 64 for k in pushed):
                                livein.update(pushed.values())     # its address escapes: count it read
                        elif off >= 4:
                            N = max(N, (off - 4 + max(op.size, 4) + 3) // 4)
                        else:
                            for k in list(pushed):
                                if off < k + 4 and k < off + op.size:
                                    if op.access & capstone.CS_AC_READ and not (mn == "mov" and op is ops[0]):
                                        livein.add(pushed[k])
                                    del pushed[k]
            for r in wrn:
                p_, m_ = _reg_parts(r)
                if p_ in ALLREGS:
                    dfn[p_] |= m_
            # esp / ebp tracking
            nxt = pc + cs.size
            if mn == "push":
                esp = esp - (2 if ops and ops[0].size == 2 else 4) if esp is not None else None
            elif mn == "pop":
                if esp is not None and esp in pushed:
                    r0 = pushed.pop(esp)
                    if not (ops[0].type == X.X86_OP_REG and _reg_parts(cs.reg_name(ops[0].reg))[0] == r0):
                        livein.add(r0)
                if ops[0].type == X.X86_OP_REG and cs.reg_name(ops[0].reg) == "ebp":
                    ebp = None
                esp = esp + 4 if esp is not None else None
            elif mn in ("sub", "add") and ops[0].type == X.X86_OP_REG and cs.reg_name(ops[0].reg) == "esp":
                if ops[1].type == X.X86_OP_IMM and esp is not None:
                    esp = esp + (ops[1].imm if mn == "add" else -ops[1].imm)
                else:
                    esp = None
            elif mn == "mov" and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and \
                    cs.reg_name(ops[0].reg) == "ebp" and cs.reg_name(ops[1].reg) == "esp":
                ebp = esp
            elif mn == "mov" and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and \
                    cs.reg_name(ops[0].reg) == "esp" and cs.reg_name(ops[1].reg) == "ebp":
                esp = ebp
                esp_reset = True
            elif mn == "leave":
                esp = ebp + 4 if ebp is not None else None
                ebp = None
                esp_reset = True
            elif "esp" in wrn and mn not in ("call", "ret"):
                esp = None
                esp_reset = True
            elif "ebp" in wrn:
                ebp = None
            if mn == "call":
                tgt = ops[0].imm if ops[0].type == X.X86_OP_IMM else None
                if tgt is not None:
                    tgt = resolve_thunk(tgt)
                    s2 = summary(tgt, depth + 1)
                    for r in s2["livein"]:
                        if (dfn[r] & 7) != 7:
                            livein.add(r)
                    nargs = s2.get("N")
                    for k, r in pushed.items():
                        # A5: a callee whose argument count is unknown (varargs) reads only the slots pushed in
                        # this block before the call
                        if esp is not None and k >= esp and (k < esp + 4 * nargs if nargs is not None else k in recent):
                            livein.add(r)
                    if s2.get("noreturn"):
                        break
                    pp = s2["P"] if s2.get("P") is not None else ret_pop(tgt)
                else:
                    for r in (() if RELAX_ICALL else ("ecx", "edx")):
                        if (dfn[r] & 7) != 7:
                            livein.add(r)
                    for k, r in pushed.items():
                        if esp is None or (k >= esp and k in recent):
                            livein.add(r)
                    pp = 0                 # A4: a function-pointer call is cdecl; checked by the balance at ret
                    ind_assumed = True
                for r in ("eax", "ecx", "edx"):
                    dfn[r] = 7
                recent = set()
                esp = esp + pp if (esp is not None and pp is not None) else None
            if mn in ("ret", "retn"):
                pops.add(ops[0].imm if ops else 0)
                if (dfn["eax"] & 7) != 7:
                    eax_def = False
                if esp != 0:
                    if esp is not None or ind_assumed:
                        return _sfail(va, "r7")
                break
            if mn in ("int3", "hlt", "ud2"):
                break
            is_jmp = mn == "jmp"
            is_jcc = cs.group(capstone.CS_GRP_JUMP) and not is_jmp
            if is_jmp or is_jcc:
                if ops[0].type != X.X86_OP_IMM:
                    m_ = ops[0].mem if ops[0].type == X.X86_OP_MEM else None
                    if is_jmp and not (m_ is not None and not m_.base and m_.index and m_.scale == 4
                                       and lo <= (m_.disp & M32) < hi + 0x400):
                        # indirect tail jump: A4 (a function-pointer target pops nothing, reads no register
                        # unless --relax-icall is off: then ECX / EDX count as read)
                        for r in (() if RELAX_ICALL else ("ecx", "edx")):
                            if (dfn[r] & 7) != 7:
                                livein.add(r)
                        ind_assumed = True
                        pops.add(0)
                        eax_def = False
                        if esp != 0:
                            return _sfail(va, "r8b")
                        break
                    if not is_jmp:
                        return _sfail(va, "r8")
                    # jump table: every entry up to the first one outside the body (over-approximation)
                    k = m_.disp & M32
                    while True:
                        raw = img.bytes(k, 4)
                        e = struct.unpack("<I", raw)[0] if raw else 0
                        if not (lo <= e < hi):
                            break
                        st = (esp, ebp, dict(dfn))
                        if e not in states:
                            states[e] = st
                            work.append(e)
                        else:
                            o = states[e]
                            if o[0] != esp:
                                return _sfail(va, "r9")
                            merged = {r: o[2][r] & dfn[r] for r in ALLREGS}
                            if merged != o[2]:
                                states[e] = (esp, ebp if o[1] == ebp else None, merged)
                                work.append(e)
                        k += 4
                    break
                tgt = ops[0].imm
                if not (lo <= tgt < hi):
                    # tail call
                    s2 = summary(tgt, depth + 1)
                    for r in s2["livein"]:
                        if (dfn[r] & 7) != 7:
                            livein.add(r)
                    if s2.get("P") is not None:
                        pops.add(s2["P"])
                    if not s2.get("eax_def"):
                        eax_def = False
                    if is_jmp:
                        break
                else:
                    st = (esp, ebp, dict(dfn))
                    if tgt not in states:
                        states[tgt] = st
                        work.append(tgt)
                    else:
                        o = states[tgt]
                        merged = {r: o[2][r] & dfn[r] for r in ALLREGS}
                        if o[0] != esp:
                            return _sfail(va, "r9")
                        if merged != o[2] or o[1] != ebp:
                            states[tgt] = (esp, ebp if o[1] == ebp else None, merged)
                            work.append(tgt)
                    if is_jmp:
                        break
            if nxt in states and not (is_jmp):
                o = states[nxt]
                merged = {r: o[2][r] & dfn[r] for r in ALLREGS}
                if o[0] != esp:
                    return _sfail(va, "r10")
                if merged == o[2] and o[1] == ebp:
                    break
                states[nxt] = (esp, ebp if o[1] == ebp else None, merged)
                dfn = merged
            pc = nxt
    if ind_assumed and esp_reset:
        unknown_n = unknown_n or (pc if "pc" in dir() else -1)
    res = {"n_why": unknown_n, "P": (next(iter(pops)) if len(pops) == 1 else None), "N": None if unknown_n else N,
           "livein": frozenset(livein), "eax_def": eax_def and bool(pops), "noreturn": not pops, "known": len(pops) <= 1}
    if len(pops) > 1:
        res["P"] = None
    return res


# ====================================================================== the engine
X87_STORE = {"fst", "fstp", "fist", "fistp", "fisttp", "fnstsw", "fstsw", "fnstcw", "fstcw", "fnstenv", "fnsave",
             "fbstp", "fstenv", "fsave"}
CC_OF = {"jo": "o", "jno": "no", "jb": "b", "jae": "ae", "je": "e", "jne": "ne", "jbe": "be", "ja": "a", "js": "s",
         "jns": "ns", "jp": "p", "jnp": "np", "jl": "l", "jge": "ge", "jle": "le", "jg": "g"}
SETCC = {"set" + k[1:]: v for k, v in CC_OF.items()}
CMOVCC = {"cmov" + k[1:]: v for k, v in CC_OF.items()}


class Region:
    __slots__ = ("events", "texts", "x87", "jmps", "end", "pcs", "pops", "paired")


class State:
    """One side's machine state. r: full registers; f: flags; x: x87; m: memory chain; s: private stack
    {offset: (term, size)} (default mode only)."""
    __slots__ = ("r", "f", "x", "m", "s", "xf", "cl")

    def copy(self):
        n = State()
        n.r, n.f, n.x, n.m, n.s, n.xf, n.cl = dict(self.r), self.f, self.x, self.m, dict(self.s), self.xf, self.cl
        return n


class Ctx:
    def __init__(self, A, B, strict, resolver, trace=False, popsol=None, popsol_unique=True):
        self.sides = (A, B)
        self.popsol = popsol            # A6: original-side call pc -> pop count (one solution of pin_pops)
        self.popsol_unique = popsol_unique
        self.strict = strict
        self.res = resolver
        self.trace = trace
        self.esp0 = S("esp0")
        self.mem0 = mk("M0", 0)
        self.used = set()           # rule tags observed during the proof
        self.relax_icall = RELAX_ICALL
        self.maxcall = {}
        self.maxcall_side = -(1 << 31)
        self.esp_reset = {A.name: esp_reset_any(A), B.name: esp_reset_any(B)}
        self.retdiff = set()        # registers that differ at some return
        self.retdiff_detail = {}
        self.pairs_seen = []
        self._pins = {}

    # ------------------------------------------------------------ constants
    def ro_fold(self, a, sz):
        d, k = to_lin(a)
        if not d:
            img = image()
            if img.readonly(k, sz):
                b = img.bytes(k, sz)
                if b is not None:
                    return C(int.from_bytes(b, "little"), 8 * sz)
            return None
        if len(d) == 1:
            (t, c), = d.items()
            if c == 1 and t.op == "od":
                secidx, off = t.a
                sec = self.res.obj_secs[secidx - 1]
                if not (sec["chars"] & RO_FLAGS):
                    o = off + k
                    if 0 <= o and o + sz <= len(sec["data"]):
                        return C(int.from_bytes(sec["data"][o:o + sz], "little"), 8 * sz)
        return None

    # ------------------------------------------------------------ state
    def entry_state(self):
        st = State()
        st.r = {r: S(r + "_in") for r in GPR}
        st.r["esp"] = self.esp0
        st.f = F_("entry")
        st.x = mk("X0", 0)
        st.m = self.mem0
        st.s = {}
        st.xf = None
        st.cl = None          # highest ESP offset of any call so far on this path: frame bytes below it may hold
        return st             # a callee's leftovers; bytes at or above it still hold the entry contents

    def stack_off(self, a, st=None):
        """Default mode: offset k when a == esp0 + k and the slot is private; None when the access goes to memory
        (no esp0, or an exposed frame slot). A variable offset esp0 + k + x exposes the frame from k (A1': the
        indexed object starts at k and lies below esp0)."""
        d, k = to_lin(a)
        if self.esp0 in d:
            ks = k - (1 << 32) if k >= 0x80000000 else k
            if d[self.esp0] != 1:
                raise Fallback("stack address scaled")
            if len(d) == 1:
                if st is not None and st.xf is not None and st.xf <= ks < 0:
                    return None
                return ks
            if any(has_sym(t, "esp0") for t in d if t is not self.esp0):
                raise Fallback("stack address inside a non-linear term")
            if st is None:
                raise Fallback("stack access with a variable offset")
            self.expose(st, ks, "variable-offset frame access")
            return None
        if a.op != "lin" and has_sym(a, "esp0"):
            raise Fallback("stack address inside a non-linear term")
        return None

    def esp_offsets(self, t, out=None, memo=None):
        """Every esp0 + k linear subterm of t (k signed); raises Fallback on any other use of esp0."""
        out = [] if out is None else out
        memo = set() if memo is None else memo
        if not isinstance(t, T):
            if isinstance(t, tuple):
                for x in t:
                    self.esp_offsets(x, out, memo)
            return out
        if t.id in memo or not has_sym(t, "esp0"):
            return out
        memo.add(t.id)
        if t is self.esp0:
            out.append(0)
            return out
        if t.op == "lin":
            d, k = to_lin(t)
            if self.esp0 in d:
                if d[self.esp0] != 1 or any(has_sym(x, "esp0") for x in d if x is not self.esp0):
                    raise Fallback("stack address in a non-linear term")
                out.append(k - (1 << 32) if k >= 0x80000000 else k)
                return out
        if t.op in ("ld", "st", "M0", "Mc", "call", "X", "Xc", "ret", "reta", "retc", "retd", "argc", "x87o",
                    "x87sw", "x87"):
            # a value loaded from memory or produced by a callee: any stack address inside it was exposed when
            # it was stored or passed
            return out
        if t.op == "phi":
            raise Fallback("stack address behind a join")
        raise Fallback(f"stack address inside {t.op}")

    def expose(self, st, k, why):
        """A1': the frame from offset k up to esp0 becomes ordinary memory (an object there may be reached through
        an escaped pointer). Slots below k stay private. An escape into the argument area falls back to strict."""
        if k >= 0:
            raise Fallback(f"argument-area address escapes ({why})")
        if st.xf is not None and st.xf <= k:
            return
        if k < self.maxcall.get(id(st), -(1 << 31)) or k < self.maxcall_side:
            raise Fallback("exposed frame reaches below an earlier call's stack pointer")
        lo = k
        hi = st.xf if st.xf is not None else 0
        moved = sorted((o, e) for o, e in st.s.items() if lo <= o < hi)
        for o, (v, sz) in moved:
            st.m = store(st.m, add(self.esp0, C(o)), v, sz)
            del st.s[o]
        st.xf = lo
        self.used.add("A1-exposed")

    def escape(self, st, vals, why):
        for v in vals:
            for k in self.esp_offsets(v):
                self.expose(st, k, why)

    def sread(self, side, st, off, sz):
        """Private stack read of [off, off+sz)."""
        e = st.s.get(off)
        if e is not None and e[1] == sz:
            return e[0]
        parts = []
        for k, (v, s) in st.s.items():
            if k <= off and off + sz <= k + s:
                return ext(v, 8 * (off - k), 8 * sz)
            if k < off + sz and off < k + s:
                parts.append((k, v, s))
        if not parts:
            return self.sinit(side, off, sz, st)
        if sz == 8:
            return cat64(self.sread(side, st, off, 4), self.sread(side, st, off + 4, 4))
        bs = []
        for i in range(sz):
            o = off + i
            for k, v, s in parts:
                if k <= o < k + s:
                    bs.append(ext(v, 8 * (o - k), 8))
                    break
            else:
                bs.append(self.sinit(side, o, 1, st))
        val = zx(bs[0], 32)
        for i in range(1, sz):
            val = ins(val, bs[i], 8 * i)
        return ext(val, 0, 8 * sz) if sz != 4 else val

    def sinit(self, side, off, sz, st=None):
        """Content of a private stack slot this side has not written: the caller's argument dwords and the frame
        bytes as they were at entry are the same machine state on both sides (shared terms); a frame byte that was
        below ESP at an earlier call may hold that callee's leftovers (this side's own term)."""
        if sz == 8:
            return cat64(self.sinit(side, off, 4, st), self.sinit(side, off + 4, 4, st))
        base = off - (off % 4)
        if base >= 0:
            atom = mk("arg", 32, base)
        elif st is not None and st.cl is not None and base < st.cl:
            atom = mk("undef", 32, side.name, base)
        else:
            atom = mk("below", 32, base)
        if off == base and sz == 4:
            return atom
        if off + sz <= base + 4:
            return ext(atom, 8 * (off - base), 8 * sz)
        bs = [self.sinit(side, off + i, 1, st) for i in range(sz)]
        val = zx(bs[0], 32)
        for i in range(1, sz):
            val = ins(val, bs[i], 8 * i)
        return ext(val, 0, 8 * sz) if sz != 4 else val

    def swrite(self, st, off, v, sz):
        new = {}
        for k, (val, s) in st.s.items():
            if k + s <= off or off + sz <= k:
                new[k] = (val, s)
            else:
                for i in range(s):
                    o = k + i
                    if not (off <= o < off + sz):
                        new[o] = (ext(val, 8 * i, 8), 1)
        new[off] = (v, sz)
        st.s = new

    def memread(self, side, st, a, sz):
        if not self.strict:
            k = self.stack_off(a, st)
            if k is not None:
                if st.xf is not None and k < st.xf < k + sz:
                    raise Fallback("access straddles the exposed frame boundary")
                return self.sread(side, st, k, sz)
            if st.xf is not None:
                self._exposed_initial(side, st, a, sz)
        return load(st.m, a, sz, self)

    def _exposed_initial(self, side, st, a, sz):
        pass

    def memwrite(self, side, st, a, v, sz):
        if not self.strict:
            k = self.stack_off(a, st)
            if k is not None:
                if st.xf is not None and k < st.xf < k + sz:
                    raise Fallback("access straddles the exposed frame boundary")
                self.swrite(st, k, v, sz)
                return
            if has_sym(v, "esp0"):
                self.escape(st, [v], "stored to memory")
        st.m = store(st.m, a, v, sz)

    # ------------------------------------------------------------ registers
    @staticmethod
    def rget(st, name):
        p, lo, w = SUB[name]
        v = st.r[p]
        return v if w == 32 else ext(v, lo, w)

    @staticmethod
    def rset(st, name, v):
        p, lo, w = SUB[name]
        if w == 32:
            st.r[p] = v
        else:
            assert v.w == w, (name, v.w)
            st.r[p] = ins(st.r[p], v, lo)

    # ------------------------------------------------------------ operands
    def reloc_value(self, side, cs, which, raw):
        """Value of an immediate / displacement field. Our relocated fields resolve through the resolver;
        the original's absolute operands are plain constants."""
        r = side.field(cs, which)
        if r is None:
            return C(raw)
        symrec, typ, field = r
        return self.res.resolve(side, symrec, field, typ)

    def addr(self, side, st, cs, op):
        mem = op.mem
        if mem.segment and cs.reg_name(mem.segment) == "fs":
            base = mk("fs", 32)
        else:
            base = C(0)
            if mem.segment and cs.reg_name(mem.segment) not in ("ds", "ss", "es"):
                raise NotProven("unsupported", f"segment {cs.reg_name(mem.segment)} in {cs.mnemonic} {cs.op_str}")
        a = base
        if mem.base:
            a = add(a, self.rget(st, cs.reg_name(mem.base)))
        if mem.index:
            a = add(a, mulc(self.rget(st, cs.reg_name(mem.index)), mem.scale))
        if mem.disp or cs.disp_offset:
            a = add(a, self.reloc_value(side, cs, "disp", mem.disp & M32))
        return a

    def read(self, side, st, cs, op, w=None):
        if op.type == X.X86_OP_REG:
            return self.rget(st, cs.reg_name(op.reg))
        if op.type == X.X86_OP_IMM:
            w = w or op.size * 8
            v = self.reloc_value(side, cs, "imm", op.imm & M32)
            if v.op == "c":
                return C(v.a[0], w)
            if w != 32:
                raise NotProven("unsupported", "relocated narrow immediate")
            return v
        if op.type == X.X86_OP_MEM:
            return self.memread(side, st, self.addr(side, st, cs, op), op.size)
        raise NotProven("unsupported", f"operand type {op.type}")

    def write(self, side, st, cs, op, v):
        if op.type == X.X86_OP_REG:
            self.rset(st, cs.reg_name(op.reg), v)
        elif op.type == X.X86_OP_MEM:
            self.memwrite(side, st, self.addr(side, st, cs, op), v, op.size)
        else:
            raise NotProven("unsupported", "write to a non-lvalue")

    def push(self, side, st, v, sz=4):
        st.r["esp"] = add(st.r["esp"], C(-sz))
        self.memwrite(side, st, st.r["esp"], v, sz)

    def pop(self, side, st, sz=4):
        v = self.memread(side, st, st.r["esp"], sz)
        st.r["esp"] = add(st.r["esp"], C(sz))
        return v

    # ------------------------------------------------------------ calls
    def esp_off(self, st):
        d, k = to_lin(st.r["esp"])
        if d != {self.esp0: 1}:
            raise NotProven("esp-unknown", f"esp = {show(st.r['esp'])}")
        return k - (1 << 32) if k >= 0x80000000 else k

    def cleanup(self, side, cs):
        """Bytes released right after a call by `add esp, imm` or `pop ecx/edx`, skipping instructions that touch
        neither ESP nor control flow. None when the next ESP change is anything else."""
        pc = cs.address + cs.size
        n = 0
        for _ in range(12):
            try:
                c = side.decode(pc)
            except NotProven:
                break
            mn = c.mnemonic
            if mn == "add" and c.operands[0].type == X.X86_OP_REG and c.reg_name(c.operands[0].reg) == "esp" \
                    and c.operands[1].type == X.X86_OP_IMM:
                return (n + c.operands[1].imm) // 4
            if mn == "pop" and c.operands[0].type == X.X86_OP_REG and c.reg_name(c.operands[0].reg) in ("ecx", "edx"):
                n += 4
                pc += c.size
                continue
            if n:
                return n // 4
            rd, wr = c.regs_access()
            names = {c.reg_name(r) for r in rd} | {c.reg_name(r) for r in wr}
            if "esp" in names or "sp" in names or c.group(capstone.CS_GRP_JUMP) or c.group(capstone.CS_GRP_CALL) \
                    or c.group(capstone.CS_GRP_RET) or c.mnemonic in ("push", "pop"):
                return None
            pc += c.size
        return n // 4 if n else None

    def pushcount(self, side, region):
        """A5: the number of pushes right before the call in this region (instructions that touch neither ESP nor
        control flow may sit between them). None when the region start is reached first."""
        n = 0
        for pc in reversed(region.pcs[:-1]):
            c = side.decode(pc)
            mn = c.mnemonic
            if mn == "push":
                n += 1
                continue
            if mn == "call" or mn in ("ret", "retn"):
                return n
            if mn == "sub" and c.operands[0].type == X.X86_OP_REG and c.reg_name(c.operands[0].reg) == "esp"                     and c.operands[1].type == X.X86_OP_IMM:
                return n + (c.operands[1].imm + 3) // 4       # space for arguments stored with mov [esp+k]
            rd, wr = c.regs_access()
            if any(c.reg_name(r) in ("esp", "sp") for r in wr):
                return n
        return None

    def do_call(self, side, st, tgt, cs, region):
        """Model a call: build the observable key, then the post-call state."""
        info = self.res.callee(tgt)
        if info.get("chkstk"):
            sz = st.r["eax"]
            if sz.op != "c":
                raise NotProven("unsupported", "__chkstk with a variable size")
            st.r["esp"] = add(st.r["esp"], C(-sz.a[0]))
            region.events.append(("chkstk", sz))
            region.pops.append(None)
            return
        if info.get("indirect"):
            # A4: a function-pointer call pops nothing (cdecl); a wrong guess shows up as an unbalanced stack at
            # the return, which is checked on both sides, so it is refused when either side resets ESP from
            # another register. Arguments: the slots this side's own cleanup releases (the original's cleanup
            # covers at least the callee's arguments); both sides must agree on that count.
            if self.esp_reset[side.name]:
                raise NotProven("indirect-call-esp-reset", f"{side.name}: ESP restored from a register in a function "
                                f"with a call through {show(tgt)}")
            livein = frozenset() if self.relax_icall else frozenset(("ecx", "edx"))
            pin = None
            if side is self.sides[1]:
                # A6, the original: the pop count is one solution of its own stack equations (pin_pops); the
                # proof is run once per solution (check_sides)
                if self.popsol is not None:
                    pin = self.popsol.get(cs.address)
                    if pin is not None:
                        self.used.add("A6-pinned-pop" if self.popsol_unique else "A6-pop-solution")
            else:
                # A6, ours: the pop count of the original's call paired with this one (same index in the same
                # region). The targets are then compared as equal terms, so both sides call the same function
                # and the copied count is that function's real one whenever the original's is.
                k = len(region.events)
                if region.paired is not None and k < len(region.paired) and region.paired[k] is not None:
                    pin = region.paired[k]
                    self.used.add("A6-paired-pop")
            if pin is not None:
                n = pin // 4 if pin else self.cleanup(side, cs)
                info = {"P": pin, "N": n, "livein": livein, "eax_def": False, "noreturn": False, "pin": pin}
            else:
                self.used.add("A4-indirect")
                n = self.cleanup(side, cs)
                info = {"P": 0, "N": n, "livein": livein, "eax_def": False, "noreturn": False, "pin": 0}
        P_, N_, livein = info["P"], info["N"], info["livein"]
        regargs = tuple((r, st.r[r]) for r in sorted(livein))
        if "pin" in info:
            regargs = regargs + (("pop", info["pin"]),)      # a call through a pointer: its pop count is observed
        if self.strict:
            args = (st.r["esp"],)
        else:
            k = self.esp_off(st)
            self.maxcall_side = max(self.maxcall_side, k)       # deepest-up call ESP seen (both sides, all paths)
            if N_ is None:
                n = self.cleanup(side, cs)
                if n is None:
                    n = self.pushcount(side, region)
                    if n is not None:
                        self.used.add("A5-arity")
                if n is None:
                    if k >= 0:
                        raise NotProven("esp-unknown", "call with esp at or above entry")
                    n = (-k) // 4
                    self.used.add("call-args-all")
            else:
                n = N_
            args = tuple(self.sread(side, st, k + 4 * i, 4) if not (st.xf is not None and st.xf <= k + 4 * i < 0)
                         else load(st.m, add(self.esp0, C(k + 4 * i)), 4, self) for i in range(n))
            esc = [v for v in args + tuple(v for _, v in regargs) if has_sym(v, "esp0")]
            if esc:
                self.escape(st, esc, "passed to a callee")
                args = tuple(self.sread(side, st, k + 4 * i, 4) if not (st.xf is not None and st.xf <= k + 4 * i < 0)
                             else load(st.m, add(self.esp0, C(k + 4 * i)), 4, self) for i in range(n))
        key = mk("call", 0, tgt, args, regargs, st.m, st.x)
        region.events.append(key)
        region.pops.append(info.get("pin"))
        if info.get("noreturn"):
            region.end = ("halt",)
            return "halt"
        if P_ is None and tgt.op == "c":
            P_ = ret_pop(tgt.a[0])          # every `ret` in the callee's body agrees
        if P_ is None:
            raise NotProven("callee-pop-unknown", f"callee {show(tgt)}")
        eb, cb, db = st.r["eax"], st.r["ecx"], st.r["edx"]
        st.r["eax"] = mk("ret", 32, key) if info.get("eax_def") else mk("reta", 32, key, eb)
        st.r["ecx"] = mk("retc", 32, key, cb)
        st.r["edx"] = mk("retd", 32, key, db)
        st.f = F_("call", side.name, key)
        st.x = mk("Xc", 0, key)
        st.m = mk("Mc", 0, key)
        if not self.strict:
            k = self.esp_off(st)
            st.s = {o: e for o, e in st.s.items() if o >= k}       # the callee's frame below esp is garbage
            st.cl = k if st.cl is None else max(st.cl, k)
            for i in range(len(args)):
                if st.xf is not None and st.xf <= k + 4 * i < 0:
                    continue
                old = self.sread(side, st, k + 4 * i, 4)
                self.swrite(st, k + 4 * i, mk("argc", 32, key, i, old), 4)
        st.r["esp"] = add(st.r["esp"], C(P_))
        return None

    def pins(self, side):
        """A6 pop counts of this side's calls through a pointer (pin_pops), computed once per side."""
        p = self._pins.get(side.name)
        if p is None:
            if self.esp_reset[side.name]:
                p = {"why": "esp reset"}
            else:
                p = pin_pops(side, self.pin_callee)
            self._pins[side.name] = p
        return p

    def pin_callee(self, side, cs):
        """pin_pops callee facts: ("pop", P), ("halt",), ("chkstk",), ("var",) for a call through a pointer, or
        None when the callee cannot be resolved. Uses the same resolution as do_call."""
        op = cs.operands[0]
        try:
            if op.type == X.X86_OP_IMM:
                tgt = self.target(side, None, cs, op)
                info = self.res.callee(tgt)
                if info.get("chkstk"):
                    return ("chkstk",)
                if info.get("noreturn"):
                    return ("halt",)
                P_ = info.get("P")
                if P_ is None and tgt.op == "c":
                    P_ = ret_pop(tgt.a[0])
                return ("pop", P_) if P_ is not None else None
            if op.type == X.X86_OP_MEM and not op.mem.base and not op.mem.index:
                a = self.reloc_value(side, cs, "disp", op.mem.disp & M32)
                if a.op == "c" and a.a[0] in self.res.import_pop:
                    return ("pop", self.res.import_pop[a.a[0]])
        except NotProven:
            return None
        return ("var",)

    # ------------------------------------------------------------ one instruction
    def step(self, side, st, cs, region):
        """Executes cs on st. Returns None to continue, or a terminator tuple."""
        mn = cs.mnemonic
        ops = cs.operands
        rd = lambda i, w=None: self.read(side, st, cs, ops[i], w)  # noqa: E731
        wr = lambda i, v: self.write(side, st, cs, ops[i], v)  # noqa: E731
        w = ops[0].size * 8 if ops else 32
        if cs.prefix[0] in (0xF0,):
            raise NotProven("unsupported", f"lock prefix: {mn} {cs.op_str}")
        if mn.startswith("f") or mn in ("wait",):
            return self.x87(side, st, cs, region)
        if mn in ("nop", "cld", "fwait"):
            return None
        if mn == "mov":
            v = rd(1, w)
            wr(0, v)
        elif mn in ("movzx", "movsx"):
            v = rd(1)
            wr(0, zx(v, w) if mn == "movzx" else sx(v, w))
        elif mn == "lea":
            a = self.addr(side, st, cs, ops[1])
            wr(0, a if w == 32 else ext(a, 0, w))
        elif mn == "xchg":
            a, b = rd(0), rd(1)
            wr(0, b)
            wr(1, a)
        elif mn == "push":
            if ops[0].type == X.X86_OP_IMM:
                self.push(side, st, rd(0, 32), 4)
            else:
                v = rd(0)
                self.push(side, st, v, v.w // 8)
        elif mn == "pop":
            v = self.pop(side, st, ops[0].size)
            wr(0, v)
        elif mn in ("add", "sub", "adc", "sbb", "cmp"):
            a, b = rd(0), rd(1, w)
            if mn in ("adc", "sbb"):
                cf = zx(pred("b", st.f), w)
                if mn == "sbb" and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg:
                    r = sub(zero(w), cf)
                else:
                    r = add(add(a, b), cf) if mn == "adc" else sub(sub(a, b), cf)
                st.f = F_("opaque", mn, a, b, st.f)
                wr(0, r)
                return None
            if mn == "add":
                r = add(a, b)
                x, y = sorted((a, b), key=lambda t: t.id)
                st.f = F_("add", x, y)
                wr(0, r)
            else:
                if mn == "sub" and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg:
                    st.f = F_("logic", zero(w))
                    wr(0, zero(w))
                    return None
                st.f = F_("sub", a, b)
                if mn == "sub":
                    wr(0, sub(a, b))
        elif mn in ("and", "or", "xor", "test"):
            a, b = rd(0), rd(1, w)
            if mn == "xor" and ops[0].type == X.X86_OP_REG and ops[1].type == X.X86_OP_REG and ops[0].reg == ops[1].reg:
                r = zero(w)
            else:
                r = bop("and" if mn == "test" else mn, w, a, b)
            st.f = F_("logic", r)
            if mn != "test":
                wr(0, r)
        elif mn == "not":
            wr(0, bop("xor", w, rd(0), C(MASK[w], w)))
        elif mn == "neg":
            a = rd(0)
            st.f = F_("neg", a)
            wr(0, sub(zero(w), a))
        elif mn in ("inc", "dec"):
            a = rd(0)
            r = add(a, C(1 if mn == "inc" else -1, w))
            st.f = F_("incdec", st.f, r)
            wr(0, r)
        elif mn in ("shl", "sal", "shr", "sar", "rol", "ror"):
            a = rd(0)
            cnt = C(1, 8) if len(ops) == 1 else (C(ops[1].imm & 31, 8) if ops[1].type == X.X86_OP_IMM
                                                 else bop("and", 8, rd(1), C(31, 8)))
            op = "shl" if mn == "sal" else mn
            r = shift(op, w, a, cnt)
            if cnt.op == "c" and cnt.a[0] & 31 == 0:
                pass
            elif cnt.op == "c":
                st.f = F_("keepzs", st.f, op, a, cnt) if op in ("rol", "ror") else F_("shres", op, a, cnt, r)
            else:
                st.f = F_("opaque", op, a, cnt, st.f)
            wr(0, r)
        elif mn in ("shld", "shrd"):
            a, b = rd(0), rd(1)
            cnt = C(ops[2].imm & 31, 8) if ops[2].type == X.X86_OP_IMM else bop("and", 8, rd(2), C(31, 8))
            r = mk(mn, w, a, b, cnt)
            st.f = F_("opaque", mn, a, b, cnt, st.f)
            wr(0, r)
        elif mn == "imul":
            if len(ops) == 1:
                a, b = self.rget(st, {8: "al", 16: "ax", 32: "eax"}[w]), rd(0)
                lo_ = bop("mul", w, a, b)
                hi_ = bop("imulhi", w, a, b)
                if w == 8:
                    self.rset(st, "ax", ins(zx(lo_, 16), hi_, 8))
                else:
                    self.rset(st, {16: "ax", 32: "eax"}[w], lo_)
                    self.rset(st, {16: "dx", 32: "edx"}[w], hi_)
                x, y = sorted((a, b), key=lambda t: t.id)
                st.f = F_("opaque", "imul1", x, y)
            else:
                a, b = (rd(0), rd(1, w)) if len(ops) == 2 else (rd(1), rd(2, w))
                r = bop("mul", w, a, b)
                x, y = sorted((a, b), key=lambda t: t.id)
                st.f = F_("opaque", "imul", x, y)
                wr(0, r)
        elif mn == "mul":
            a, b = self.rget(st, {8: "al", 16: "ax", 32: "eax"}[w]), rd(0)
            lo_ = bop("mul", w, a, b)
            hi_ = bop("mulhi", w, a, b)
            if w == 8:
                self.rset(st, "ax", ins(zx(lo_, 16), hi_, 8))
            else:
                self.rset(st, {16: "ax", 32: "eax"}[w], lo_)
                self.rset(st, {16: "dx", 32: "edx"}[w], hi_)
            x, y = sorted((a, b), key=lambda t: t.id)
            st.f = F_("opaque", "mul", x, y)
        elif mn in ("div", "idiv"):
            if w != 32:
                raise NotProven("unsupported", f"{mn} {w}-bit")
            d_ = rd(0)
            hi, lo = st.r["edx"], st.r["eax"]
            st.r["eax"] = mk(mn + "q", 32, hi, lo, d_)
            st.r["edx"] = mk(mn + "r", 32, hi, lo, d_)
            st.f = F_("undef", side.name, side.label(cs.address))
        elif mn == "cdq":
            st.r["edx"] = shift("sar", 32, st.r["eax"], C(31, 8))
        elif mn == "cwde":
            st.r["eax"] = sx(self.rget(st, "ax"), 32)
        elif mn == "cbw":
            self.rset(st, "ax", sx(self.rget(st, "al"), 16))
        elif mn == "bswap":
            wr(0, mk("bswap", 32, rd(0)))
        elif mn in SETCC:
            wr(0, zx(pred(SETCC[mn], st.f), 8))
        elif mn in CMOVCC:
            p = pred(CMOVCC[mn], st.f)
            a, b = rd(0), rd(1)
            wr(0, b if p.op == "c" and p.a[0] else (a if p.op == "c" else mk("ite", w, p, b, a)))
        elif mn == "sahf":
            st.f = F_("sahf", self.rget(st, "ah"))
        elif mn == "lahf":
            self.rset(st, "ah", mk("lahf", 8, st.f))
        elif mn == "leave":
            st.r["esp"] = st.r["ebp"]
            st.r["ebp"] = self.pop(side, st)
        elif mn.startswith("rep ") or mn in ("movsd", "movsb", "movsw", "stosd", "stosb", "stosw"):
            self.strop(side, st, cs)
        elif mn == "call":
            return ("call",)
        elif mn in ("ret", "retn"):
            return ("ret", ops[0].imm if ops else 0)
        elif mn == "jmp":
            return ("jmp",)
        elif mn in CC_OF:
            return ("jcc", CC_OF[mn])
        elif mn in ("int3", "hlt", "ud2"):
            return ("halt",)
        elif mn in ("stmxcsr", "ldmxcsr") and ops and ops[0].type == X.X86_OP_MEM and ops[0].size == 4:
            # MXCSR is part of the floating-point state term st.x (which already stands for the whole FP state:
            # entry symbol, then a function of the previous state at every FP instruction and call). stmxcsr
            # stores a function of that term; ldmxcsr makes the term a function of the old term and the loaded
            # word. Equal terms still mean equal values, so this adds no assumption. They are not x87
            # instructions, so the x87 region rule does not apply to them.
            a = self.addr(side, st, cs, ops[0])
            if mn == "stmxcsr":
                self.memwrite(side, st, a, mk("mxcsr", 32, st.x), 4)
            else:
                st.x = mk("Xmx", 0, st.x, self.memread(side, st, a, 4))
        else:
            raise NotProven("unsupported", f"instruction {mn} {cs.op_str}")
        return None

    def strop(self, side, st, cs):
        mn = cs.mnemonic
        rep = mn.startswith("rep ")
        base = mn.split()[-1]
        if base not in ("movsd", "movsb", "movsw", "stosd", "stosb", "stosw") or mn.startswith("repne"):
            raise NotProven("unsupported", f"string op {mn}")
        sz = {"d": 4, "b": 1, "w": 2}[base[-1]]
        if rep:
            n = st.r["ecx"]
            if n.op != "c":
                raise NotProven("unsupported", f"{mn} with a variable count")
            n = n.a[0]
            if n > 512:
                raise NotProven("unsupported", f"{mn} count {n}")
        else:
            n = 1
        for _ in range(n):
            if base.startswith("movs"):
                v = self.memread(side, st, st.r["esi"], sz)
                self.memwrite(side, st, st.r["edi"], v, sz)
                st.r["esi"] = add(st.r["esi"], C(sz))
            else:
                v = self.rget(st, {4: "eax", 2: "ax", 1: "al"}[sz])
                self.memwrite(side, st, st.r["edi"], v, sz)
            st.r["edi"] = add(st.r["edi"], C(sz))
        if rep:
            st.r["ecx"] = C(0)

    def x87(self, side, st, cs, region):
        mn = cs.mnemonic
        region.x87 = True
        desc = []
        memval = None
        storeop = None
        for i, op in enumerate(cs.operands):
            if op.type == X.X86_OP_REG:
                desc.append(cs.reg_name(op.reg))
            elif op.type == X.X86_OP_MEM:
                a = self.addr(side, st, cs, op)
                desc.append(f"m{op.size}")
                if mn in X87_STORE:
                    storeop = (a, op.size)
                else:
                    memval = self.memread(side, st, a, op.size)
            elif op.type == X.X86_OP_IMM:
                desc.append(hex(op.imm))
        before = st.x
        if mn in ("fnstsw", "fstsw") and cs.operands and cs.operands[0].type == X.X86_OP_REG:
            self.rset(st, "ax", mk("x87sw", 16, before))
        elif storeop is not None:
            a, sz = storeop
            self.memwrite(side, st, a, mk("x87o", 8 * sz, before, mn), sz)
        if mn in ("fcomi", "fcomip", "fucomi", "fucomip"):
            st.f = F_("x87", before, mn, tuple(desc))
        st.x = mk("X", 0, before, mn, tuple(desc), memval)
        return None

    # ------------------------------------------------------------ regions
    def run_region(self, side, st, pc, paired=None):
        reg = Region()
        reg.events, reg.texts, reg.x87, reg.jmps, reg.end, reg.pcs = [], [], False, 0, None, []
        reg.pops, reg.paired = [], paired
        seen = set()
        while True:
            if pc in seen:
                raise NotProven("loop-without-branch", f"{side.name} {side.label(pc)}")
            seen.add(pc)
            reg.pcs.append(pc)
            cs = side.decode(pc)
            nxt = pc + cs.size
            if self.trace:
                print(f"    {side.name} {side.label(pc)}: {cs.mnemonic} {cs.op_str}")
            try:
                t = self.step(side, st, cs, reg)
            except (KeyError, AssertionError, IndexError, TypeError) as e:
                raise NotProven("unsupported", f"{side.name} {side.label(pc)}: {cs.mnemonic} {cs.op_str} ({type(e).__name__})")
            if t is None:
                reg.texts.append(self.text(side, cs))
                pc = nxt
                continue
            kind = t[0]
            if kind == "call":
                op = cs.operands[0]
                tgt = self.target(side, st, cs, op)
                reg.texts.append("call " + show(tgt))
                h = self.do_call(side, st, tgt, cs, reg)
                if h == "halt":
                    return reg
                pc = nxt
                continue
            if kind == "jmp":
                op = cs.operands[0]
                if op.type == X.X86_OP_IMM and side.field(cs, "imm") is None and side.internal(op.imm):
                    reg.jmps += 1
                    pc = op.imm
                    continue
                if op.type == X.X86_OP_IMM:
                    # tail call: call + return
                    # tail call = `call tgt ; ret P(tgt)`: the callee's arguments start above our return address
                    tgt = self.target(side, st, cs, op)
                    reg.texts.append("call " + show(tgt))
                    reg.texts.append("ret")
                    esp_before = st.r["esp"]
                    st.r["esp"] = add(esp_before, C(4))
                    h = self.do_call(side, st, tgt, cs, reg)
                    if h == "halt":
                        return reg
                    st.r["esp"] = esp_before
                    reg.end = ("ret", self.res.callee(tgt)["P"])
                    return reg
                t = self.read(side, st, cs, op)
                k = self.static_target(side, t)
                if k is not None:
                    reg.jmps += 1
                    self.used.add("switch")
                    pc = k
                    continue
                reg.end = ("ind", t, cs)
                return reg
            if kind == "jcc":
                p = pred(t[1], st.f)
                tgt = cs.operands[0].imm
                if not side.internal(tgt):
                    raise NotProven("unsupported", f"{side.name} conditional jump out of the function at {side.label(pc)}")
                if p.op == "c" and (tgt if p.a[0] else nxt) not in seen:
                    reg.jmps += 1
                    pc = tgt if p.a[0] else nxt
                    continue
                reg.end = ("jcc", p, tgt, nxt)
                return reg
            if kind == "ret":
                reg.texts.append(f"ret {t[1]}")
                reg.end = ("ret", t[1])
                return reg
            if kind == "halt":
                reg.end = ("halt",)
                return reg

    def static_target(self, side, t):
        """A jump-table load whose index folded to a constant: the entry it reads."""
        if isinstance(side, ObjSide):
            if t.op != "ld":
                return None
            d, k = to_lin(t.a[1])
            if len(d) != 1:
                return None
            (lab, c), = d.items()
            if lab.op != "label" or c != 1:
                return None
            off = (lab.a[0] + k) & M32
            r = side.relocs.get(off)
            if r is None or r[0].get("sec") != side.secidx:
                return None
            tgt = r[0]["value"] + struct.unpack_from("<i", side.code, off)[0]
            return tgt if side.internal(tgt) else None
        if t.op == "c" and side.internal(t.a[0]):
            return t.a[0]
        return None

    def target(self, side, st, cs, op):
        if op.type == X.X86_OP_IMM:
            r = side.field(cs, "imm")
            if r is not None:
                symrec, typ, field = r
                t = self.res.resolve(side, symrec, field, typ)
                return C(resolve_thunk(t.a[0])) if t.op == "c" else t
            if isinstance(side, ObjSide):
                if side.internal(op.imm):
                    if op.imm == side.start:
                        return C(side.addr)
                    raise NotProven("unsupported", "call to an internal label")
            return C(resolve_thunk(op.imm & M32))
        v = self.read(side, st, cs, op)
        return v

    def text(self, side, cs):
        """Encoding-independent text of one instruction (relocations resolved), for the x87 region rule."""
        parts = []
        for i, op in enumerate(cs.operands):
            if op.type == X.X86_OP_REG:
                parts.append(cs.reg_name(op.reg))
            elif op.type == X.X86_OP_IMM:
                v = self.reloc_value(side, cs, "imm", op.imm & M32)
                parts.append(show(v))
            else:
                mem = op.mem
                seg = (cs.reg_name(mem.segment) + ":") if mem.segment else ""
                b = cs.reg_name(mem.base) if mem.base else ""
                ix = (cs.reg_name(mem.index) + f"*{mem.scale}") if mem.index else ""
                d = self.reloc_value(side, cs, "disp", mem.disp & M32) if (mem.disp or cs.disp_offset) else None
                if d is not None and d.op == "c":
                    k = d.a[0]
                    rb = image().bytes(k, op.size) if image().readonly(k, op.size) else None
                    ds = f"const:{int.from_bytes(rb, 'little'):0{2 * op.size}x}" if rb is not None and not b and not ix else hex(k)
                elif d is not None and d.op == "lin" or (d is not None and d.op == "od"):
                    v = self.ro_fold(d, op.size)
                    ds = f"const:{v.a[0]:0{2 * op.size}x}" if v is not None and not b and not ix else show(d)
                else:
                    ds = ""
                parts.append(f"{seg}m{op.size}[" + "+".join(x for x in (b, ix, ds) if x) + "]")
        return cs.mnemonic + " " + ",".join(parts)


X87_STRICT = False   # --x87-strict: a region with any x87 instruction must be identical text on both sides


def _is_x87(t):
    m = t.split(" ", 1)[0]
    return m.startswith("f") or m == "wait"


def x87_rule(ta, tb, here):
    """x87 exclusion. The x87 instructions of the region must be the same instructions with the same operands in the
    same order, and every other instruction must stay between the same two x87 instructions (segments compared as
    mnemonic multisets: register renaming and reordering of integer code inside a segment are left to the proof).
    Calls are segment boundaries too. --x87-strict: the whole region must be identical text."""
    def fail(i, why):
        xa = ta[i] if i < len(ta) else "(end)"
        xb = tb[i] if i < len(tb) else "(end)"
        raise NotProven("x87-region", f"region {here} {why} at instruction {i}: {xa} | {xb}")
    if X87_STRICT:
        i = next((i for i, (x, y) in enumerate(zip(ta, tb)) if x != y), min(len(ta), len(tb)))
        fail(i, "differs")
    def split(ts):
        segs, cur, bars = [], [], []
        for t in ts:
            if _is_x87(t) or t.startswith("call "):
                segs.append(sorted(x.split(" ", 1)[0] for x in cur))
                bars.append(t)
                cur = []
            else:
                cur.append(t)
        segs.append(sorted(x.split(" ", 1)[0] for x in cur))
        return segs, bars
    sa, ba = split(ta)
    sb, bb = split(tb)
    if ba != bb:
        i = next((i for i, (x, y) in enumerate(zip(ba, bb)) if x != y), min(len(ba), len(bb)))
        raise NotProven("x87-region", f"region {here} x87 instruction {i} differs: "
                        f"{ba[i] if i < len(ba) else '(end)'} | {bb[i] if i < len(bb) else '(end)'}")
    for i, (x, y) in enumerate(zip(sa, sb)):
        if x != y:
            raise NotProven("x87-region", f"region {here}: integer code moved across x87 instruction {i} "
                            f"({ba[i - 1] if i else '(region start)'}): {x} | {y}")


def esp_reset_any(side):
    """True when the linear disassembly of the body writes ESP other than by push/pop/call/ret/add/sub imm."""
    if isinstance(side, ObjSide):
        code, base = side.code[side.start:side.end], side.start
    else:
        code, base = image().bytes(side.start, side.end - side.start) or b"", side.start
    for c in _MD.disasm(code, base):
        mn = c.mnemonic
        if mn in ("push", "pop", "call", "ret", "retn", "int3", "nop"):
            continue
        if mn in ("add", "sub") and c.operands and c.operands[0].type == X.X86_OP_REG and \
                c.reg_name(c.operands[0].reg) == "esp":
            if c.operands[1].type == X.X86_OP_IMM:
                continue
            return True
        try:
            _, wr = c.regs_access()
        except Exception:
            return True
        if any(c.reg_name(r) in ("esp", "sp") for r in wr):
            return True
    return False


def _table_targets(side, cs):
    """Entries of a `jmp [idx*4 + table]` jump table that land inside the body, or None when cs is not one."""
    op = cs.operands[0]
    if op.type != X.X86_OP_MEM or op.mem.base or not op.mem.index or op.mem.scale != 4:
        return None
    out = []
    if isinstance(side, ObjSide):
        r = side.field(cs, "disp")
        if r is None or r[0].get("sec") != side.secidx:
            return None
        off = r[0]["value"] + r[2]
        while True:
            e = side.relocs.get(off)
            if e is None or e[0].get("sec") != side.secidx:
                break
            t = e[0]["value"] + struct.unpack_from("<i", side.code, off)[0]
            if not side.internal(t):
                break
            out.append(t)
            off += 4
        return out or None
    k = op.mem.disp & M32
    while True:
        raw = image().bytes(k, 4)
        e = struct.unpack("<I", raw)[0] if raw else 0
        if not side.internal(e):
            break
        out.append(e)
        k += 4
    return out or None


SOLUTION_SPACE_MAX = 1 << 16    # pin_pops: largest product of free-pop ranges that is enumerated
POP_SOLUTIONS_MAX = 32          # largest number of original-side pop solutions proved one by one


def pin_pops(side, callee_info):
    """A6: the pop count of every call through a pointer, solved from the body's own stack discipline.

    ESP is tracked as an entry-relative linear form `k + sum c_i * P_i`, with one unknown P_i per call site whose
    callee is not known (callee_info returns ("var",)). Every other ESP change is concrete. The equations are the
    facts every MSVC frame-pointer-omitted body keeps: one ESP offset per instruction whatever path reaches it (a
    join), and ESP at the entry offset at every `ret` and every tail jump. A P_i is pinned only when the system
    determines it uniquely (its reduced row has no free unknown), so a pinned value is the callee's real pop count
    for every execution on which the body keeps that discipline. Returns {"pins": {pc: P}, "free": [pc...]} or
    {"why": reason} when the walk cannot follow the body (an ESP write that is not push/pop/add/sub imm, an
    unresolved callee, an inconsistent system)."""
    from fractions import Fraction
    forms, eqs, sites = {}, [], set()
    work = [(side.entry, 0, ())]
    steps = 0
    while work:
        pc, k, d = work.pop()
        eax_imm = None
        while True:
            steps += 1
            if steps > 200000:
                return {"why": "body too large"}
            form = (k, d)
            if not side.internal(pc):
                return {"why": f"walk left the body at {side.label(pc)}"}
            if pc in forms:
                if forms[pc] != form:
                    k0, d0 = forms[pc]
                    dd = dict(d)
                    for v, c in d0:
                        dd[v] = dd.get(v, 0) - c
                    eqs.append((k - k0, dd))
                break
            forms[pc] = form
            try:
                cs = side.decode(pc)
            except NotProven as e:
                return {"why": f"decode {e.detail}"}
            mn, ops = cs.mnemonic, cs.operands
            nxt = pc + cs.size
            if mn == "push":
                k -= 4 if ops[0].type == X.X86_OP_IMM else ops[0].size
            elif mn == "pop":
                k += ops[0].size
            elif mn in ("add", "sub") and ops and ops[0].type == X.X86_OP_REG and cs.reg_name(ops[0].reg) == "esp":
                if ops[1].type != X.X86_OP_IMM:
                    return {"why": f"esp write {side.label(pc)}"}
                imm = ops[1].imm & M32
                imm = imm - (1 << 32) if imm >= 0x80000000 else imm
                k += imm if mn == "add" else -imm
            elif mn == "call":
                ci = callee_info(side, cs)
                if ci is None:
                    return {"why": f"callee {side.label(pc)}"}
                if ci[0] == "halt":
                    break
                if ci[0] == "chkstk":
                    if eax_imm is None:
                        return {"why": f"__chkstk size {side.label(pc)}"}
                    k -= eax_imm
                elif ci[0] == "pop":
                    k += ci[1]
                else:
                    sites.add(pc)
                    dd = dict(d)
                    dd[pc] = dd.get(pc, 0) + 1
                    d = tuple(sorted(dd.items()))
                eax_imm = None
                pc = nxt
                continue
            elif mn in ("ret", "retn"):
                eqs.append((k, dict(d)))
                break
            elif mn in ("int3", "hlt", "ud2"):
                break
            elif mn == "jmp":
                op = ops[0]
                if op.type == X.X86_OP_IMM and side.field(cs, "imm") is None and side.internal(op.imm):
                    pc = op.imm
                    continue
                tt = _table_targets(side, cs) if op.type == X.X86_OP_MEM else None
                if tt:
                    for t in tt:
                        work.append((t, k, d))
                    break
                if op.type == X.X86_OP_MEM and op.mem.index:
                    return {"why": f"unread jump table {side.label(pc)}"}
                eqs.append((k, dict(d)))          # tail jump: ESP at the entry offset
                break
            elif cs.group(capstone.CS_GRP_JUMP):
                if ops[0].type != X.X86_OP_IMM or not side.internal(ops[0].imm):
                    return {"why": f"conditional jump out of the body {side.label(pc)}"}
                work.append((ops[0].imm, k, d))
            else:
                try:
                    _, wr = cs.regs_access()
                except Exception:
                    return {"why": f"regs {side.label(pc)}"}
                wrn = {cs.reg_name(r) for r in wr}
                if "esp" in wrn or "sp" in wrn:
                    return {"why": f"esp write {side.label(pc)}"}
                if "eax" in wrn or "ax" in wrn or "al" in wrn or "ah" in wrn:
                    eax_imm = (ops[1].imm & M32) if (mn == "mov" and ops[0].type == X.X86_OP_REG and
                                                      cs.reg_name(ops[0].reg) == "eax" and
                                                      ops[1].type == X.X86_OP_IMM) else None
            pc = nxt
    if not sites:
        return {"pins": {}, "free": [], "solutions": [{}]}
    vs = sorted(sites)
    callk = {v: forms[v][0] for v in vs}           # constant part of the ESP form at each site
    col = {v: i for i, v in enumerate(vs)}
    rows = []
    for k, dd in eqs:
        r = [Fraction(0)] * (len(vs) + 1)
        for v, c in dd.items():
            if c:
                r[col[v]] += c
        r[-1] = Fraction(-k)
        rows.append(r)
    # reduced row echelon form
    piv, ri = [], 0
    for ci in range(len(vs)):
        p = next((i for i in range(ri, len(rows)) if rows[i][ci] != 0), None)
        if p is None:
            continue
        rows[ri], rows[p] = rows[p], rows[ri]
        f = rows[ri][ci]
        rows[ri] = [x / f for x in rows[ri]]
        for i in range(len(rows)):
            if i != ri and rows[i][ci] != 0:
                g = rows[i][ci]
                rows[i] = [a - g * b for a, b in zip(rows[i], rows[ri])]
        piv.append(ci)
        ri += 1
    for r in rows[ri:]:
        if r[-1] != 0:
            return {"why": "inconsistent stack equations"}
    pins = {}
    for i, ci in enumerate(piv):
        r = rows[i]
        if any(r[j] != 0 for j in range(len(vs)) if j != ci):
            continue
        val = r[-1]
        if val.denominator != 1 or val < 0 or int(val) % 4:
            return {"why": f"pop {val} at {side.label(vs[ci])}"}
        pins[vs[ci]] = int(val)
    free = [v for v in vs if v not in pins]
    # every solution of the system: each pop a multiple of 4 in [0, -k] (k = the constant part of the ESP offset
    # at the call; the callee pops only slots inside this frame, and the other pops only raise ESP), and ESP at or
    # below the entry offset at every instruction. A free pop is any free-column value; pivots follow from them.
    fcols = [j for j in range(len(vs)) if j not in piv]           # the unknowns the system leaves free
    rng = [range(0, max(0, -callk[vs[j]]) + 1, 4) for j in fcols]
    total = 1
    for r_ in rng:
        total *= len(r_)
    if total > SOLUTION_SPACE_MAX:
        return {"why": f"{len(free)} free pops, {total} candidate assignments", "pins": pins, "free": free}
    sols = []
    import itertools
    for combo in itertools.product(*rng):
        val = dict(zip(fcols, combo))
        full = {}
        ok = True
        for i, ci in enumerate(piv):
            x = rows[i][-1] - sum(rows[i][j] * val[j] for j in fcols)
            if x.denominator != 1 or x < 0 or int(x) % 4:
                ok = False
                break
            full[vs[ci]] = int(x)
        if not ok:
            continue
        for j, c in zip(fcols, combo):
            full[vs[j]] = c
        if any(full[v] > -callk[v] for v in vs):
            continue
        if any(k + sum(c * full[v] for v, c in d) > 0 for k, d in forms.values()):
            continue
        sols.append(full)
    if not sols:
        return {"why": "no solution of the stack equations"}
    return {"pins": pins, "free": free, "solutions": sols}


def strip_frame(m, esp0):
    """Memory after return without the stores into this function's own frame (esp0 + k, k < 0): that storage is
    dead once the function returns (A1'), and an escaped pointer to it cannot be used after the return."""
    stores = []
    cur = m
    while cur.op == "st":
        pm, a, v, sz = cur.a
        stores.append((a, v, sz))
        cur = pm
    out = cur
    keep = []
    for a, v, sz in reversed(stores):
        d, k = to_lin(a)
        if d == {esp0: 1} and (k >= 0x80000000):
            continue
        keep.append((a, v, sz))
    for a, v, sz in keep:
        out = store(out, a, v, sz)
    return out


# ====================================================================== product exploration
class Explorer:
    MAX_VISITS = 4000

    def __init__(self, ctx):
        self.ctx = ctx
        self.arrivals = {}     # pair -> {edge: (stA, stB)}
        self.entry = {}        # pair -> (stA, stB, partition-signature)
        self.sticky = {}       # pair -> set of locations already widened to phi
        self.part = {}         # pair -> partition (dict loc -> class key)
        self.prevval = {}      # pair -> loc -> fixed value last used

    @staticmethod
    def locs(ctx, side, st):
        out = {("r", r): v for r, v in st.r.items()}
        out[("f",)] = st.f
        out[("x",)] = st.x
        out[("m",)] = st.m
        out[("xf",)] = None
        if not ctx.strict:
            for k, (v, s) in st.s.items():
                for o in range(k - (k % 4), k + s, 4):
                    out[("s", o)] = None
        return out

    def flatten(self, side, st, keys):
        out = {}
        for key in keys:
            if key[0] == "r":
                out[key] = st.r[key[1]]
            elif key[0] == "f":
                out[key] = st.f
            elif key[0] == "x":
                out[key] = st.x
            elif key[0] == "m":
                out[key] = st.m
            elif key[0] == "xf":
                out[key] = mk("noxf", 0) if st.xf is None else C(st.xf)
            else:
                out[key] = self.ctx.sread(side, st, key[1], 4)
        return out

    def merged_entry(self, pair):
        ctx = self.ctx
        arr = self.arrivals[pair]
        edges = sorted(arr.keys(), key=repr)
        A, B = ctx.sides
        keysA, keysB = set(), set()
        for e in edges:
            sa, sb = arr[e]
            keysA |= set(self.locs(ctx, A, sa))
            keysB |= set(self.locs(ctx, B, sb))
        flatA = [self.flatten(A, arr[e][0], keysA) for e in edges]
        flatB = [self.flatten(B, arr[e][1], keysB) for e in edges]
        locs = [("A",) + k for k in sorted(keysA, key=repr)] + [("B",) + k for k in sorted(keysB, key=repr)]
        vec = {}
        for l in locs:
            src = flatA if l[0] == "A" else flatB
            vec[l] = tuple(src[i][l[1:]] for i in range(len(edges)))
        prev = self.part.get(pair, {})
        sticky = self.sticky.setdefault(pair, set())
        prevval = self.prevval.setdefault(pair, {})
        groups = {}
        for l in locs:
            key = (prev.get(l, ("new",)), vec[l])
            groups.setdefault(key, []).append(l)
        part, val = {}, {}
        for key, members in groups.items():
            mem_t = tuple(sorted(members, key=repr))
            v = vec[members[0]]
            for l in members:
                part[l] = mem_t
            if len(set(v)) == 1 and not (set(members) & sticky) and \
                    all(prevval.get(l, v[0]) is v[0] for l in members):
                val[mem_t] = v[0]
                for l in members:
                    prevval[l] = v[0]
            else:
                if not ctx.strict and members[0][1] not in ("m", "x", "f") and                         any(ctx.esp_offsets(x) for x in set(v)):
                    raise Fallback(f"a stack address is joined at {pair}")
                kind = members[0][1]
                w = 32 if kind in ("r", "s") else 0
                val[mem_t] = mk("phi", w, pair, mem_t)
                sticky |= set(members)
        self.part[pair] = part
        sa, sb = State(), State()
        for st, side, keys in ((sa, "A", keysA), (sb, "B", keysB)):
            st.r, st.s, st.xf = {}, {}, None
            cls_ = [(arr[e][0] if side == "A" else arr[e][1]).cl for e in edges]
            cls_ = [c for c in cls_ if c is not None]
            st.cl = max(cls_) if cls_ else None
            for k in keys:
                v = val[part[(side,) + k]]
                if k[0] == "r":
                    st.r[k[1]] = v
                elif k[0] == "f":
                    st.f = v
                elif k[0] == "x":
                    st.x = v
                elif k[0] == "m":
                    st.m = v
                elif k[0] == "xf":
                    if v.op == "phi":
                        raise Fallback("the exposed frame differs between the paths into a join")
                    st.xf = None if v.op == "noxf" else (v.a[0] - (1 << 32) if v.a[0] >= 0x80000000 else v.a[0])
                else:
                    st.s[k[1]] = (v, 4)
        sig = tuple(sorted(((l, val[part[l]].id) for l in locs), key=repr)) + (("cl", sa.cl, sb.cl),)
        return sa, sb, sig

    def run(self):
        ctx = self.ctx
        A, B = ctx.sides
        start = (A.entry, B.entry)
        self.arrivals[start] = {"entry": (ctx.entry_state(), ctx.entry_state())}
        work = [start]
        visits = 0
        rets = 0
        while work:
            pair = work.pop()
            sa, sb, sig = self.merged_entry(pair)
            if self.entry.get(pair) == sig:
                continue
            self.entry[pair] = sig
            visits += 1
            if visits > self.MAX_VISITS:
                raise NotProven("too-many-iterations", f"{visits} pair visits")
            ctx.pairs_seen.append(pair)
            if ctx.trace:
                print(f"  pair {A.label(pair[0])} / {B.label(pair[1])}")
            rb = ctx.run_region(B, sb, pair[1])           # the original first: ours copies its pop counts
            ra = ctx.run_region(A, sa, pair[0], rb.pops)
            LAST_PCS.setdefault("A", set()).update(ra.pcs)
            LAST_PCS.setdefault("B", set()).update(rb.pcs)
            here = f"{A.name} {A.label(pair[0])} / {B.name} {B.label(pair[1])}"
            if ra.jmps or rb.jmps:
                ctx.used.add("jmp-threading")
            # calls in order
            if len(ra.events) != len(rb.events):
                raise NotProven("call-sequence", f"region {here}: {len(ra.events)} vs {len(rb.events)} calls")
            for ea, eb in zip(ra.events, rb.events):
                if ea is not eb:
                    raise NotProven(*self.why_call(ea, eb, here))
            if (ra.x87 or rb.x87) and ra.texts != rb.texts:
                x87_rule(ra.texts, rb.texts, here)
                ctx.used.add("x87-barrier")
            ka, kb = ra.end[0], rb.end[0]
            if ka != kb:
                raise NotProven("cfg-mismatch", f"region {here}: {ka} vs {kb}")
            if ka == "jcc":
                pa, pb = ra.end[1], rb.end[1]
                if pa is pb and pa.op == "c":
                    succ = [((ra.end[2], rb.end[2]), "t")] if pa.a[0] else [((ra.end[3], rb.end[3]), "f")]
                elif pa is pb:
                    succ = [((ra.end[2], rb.end[2]), "t"), ((ra.end[3], rb.end[3]), "f")]
                elif pa is negate(pb):
                    ctx.used.add("polarity")
                    succ = [((ra.end[2], rb.end[3]), "t"), ((ra.end[3], rb.end[2]), "f")]
                else:
                    raise NotProven("branch-condition", f"region {here}: {show(pa)} | {show(pb)}")
                for nxt, tag in succ:
                    self.arrivals.setdefault(nxt, {})[(pair, tag)] = (sa.copy(), sb.copy())
                    work.append(nxt)
            elif ka in ("ret", "tailret"):
                rets += 1
                self.check_ret(sa, sb, ra, rb, here)
            elif ka == "halt":
                pass
            elif ka == "ind":
                for nxt in self.switch_pairs(ra, rb, here):
                    self.arrivals.setdefault(nxt, {})[(pair, ("sw",) + nxt)] = (sa.copy(), sb.copy())
                    work.append(nxt)
        if not rets:
            ctx.used.add("no-return")

    def switch_pairs(self, ra, rb, here):
        """Jump table: ours `jmp [label + idx*4]` (entries = our same-section DIR32 relocations), the original's
        `jmp [K + idx*4]` with K inside its own body. Same index term on both sides; the table length is ours (the
        compiler's bound check precedes it on every path, and the paths are paired branch by branch)."""
        ctx = self.ctx
        A, B = ctx.sides
        ta, tb = ra.end[1], rb.end[1]
        if ta.op != "ld" or tb.op != "ld":
            raise NotProven("indirect-jump", f"region {here}: jmp {show(ta)} | {show(tb)}")
        da, ka = to_lin(ta.a[1])
        db, kb = to_lin(tb.a[1])
        labs = [t for t in da if t.op == "label" and da[t] == 1]
        if len(labs) != 1:
            raise NotProven("indirect-jump", f"region {here}: ours jmp {show(ta)}")
        lab = labs[0]
        ia = {t: c for t, c in da.items() if t is not lab}
        if ia != db or set(ia.values()) != {4}:
            raise NotProven("switch-index", f"region {here}: {show(ta.a[1])} | {show(tb.a[1])}")
        base_a = (lab.a[0] + ka) & M32
        base_b = kb
        if not (B.start <= base_b < B.end + 0x400):
            raise NotProven("switch-table", f"region {here}: original table at 0x{base_b:08x}")
        out = []
        i = 0
        img = image()
        while True:
            r = A.relocs.get(base_a + 4 * i)
            if r is None or r[0].get("sec") != A.secidx:
                break
            ea = r[0]["value"] + struct.unpack_from("<i", A.code, base_a + 4 * i)[0]
            raw = img.bytes(base_b + 4 * i, 4)
            eb = struct.unpack("<I", raw)[0] if raw else None
            if eb is None or not B.internal(eb) or not A.internal(ea):
                raise NotProven("switch-table", f"region {here}: entry {i}: ours +0x{ea:x} orig {eb}")
            out.append((ea, eb))
            i += 1
        if not out:
            raise NotProven("switch-table", f"region {here}: no relocated entries at ours +0x{base_a:x}")
        ctx.used.add("switch")
        return list(dict.fromkeys(out))

    def why_call(self, ea, eb, here):
        if isinstance(ea, tuple) or isinstance(eb, tuple):
            return "call-sequence", f"region {here}: {ea} | {eb}"
        ta, aa, ra_, ma, xa = ea.a
        tb, ab, rb_, mb, xb = eb.a
        if ta is not tb:
            return "call-target", f"region {here}: {show(ta)} | {show(tb)}"
        if aa != ab:
            i = next((i for i, (x, y) in enumerate(zip(aa, ab)) if x is not y), min(len(aa), len(ab)))
            xa_ = show(aa[i]) if i < len(aa) else "(none)"
            xb_ = show(ab[i]) if i < len(ab) else "(none)"
            return "call-args", f"region {here} call {show(ta)} arg {i}: {xa_} | {xb_}"
        if ra_ != rb_:
            return "call-regargs", f"region {here} call {show(ta)}: {ra_} | {rb_}"
        if ma is not mb:
            return "call-memory", f"region {here} call {show(ta)}: memory differs ({show(ma)} | {show(mb)})"
        return "call-x87", f"region {here} call {show(ta)}"

    def check_ret(self, sa, sb, ra, rb, here):
        ctx = self.ctx
        if ra.end[1] != rb.end[1]:
            raise NotProven("ret-pop", f"region {here}: ret {ra.end[1]} vs {rb.end[1]}")
        if ra.end[0] == "ret":
            if not ctx.strict:
                for side, st in ((ctx.sides[0], sa), (ctx.sides[1], sb)):
                    k = ctx.esp_off(st)
                    if k != 0:
                        raise NotProven("ret-esp", f"{side.name}: esp = esp0{k:+d} at ret")
                    ra_ = ctx.sread(side, st, 0, 4)
                    if ra_ is not ctx.sinit(side, 0, 4):
                        raise NotProven("ret-address", f"{side.name}: return address overwritten")
            else:
                if sa.r["esp"] is not sb.r["esp"]:
                    raise NotProven("ret-esp", f"region {here}")
                if load(sa.m, sa.r["esp"], 4, ctx) is not load(sb.m, sb.r["esp"], 4, ctx):
                    raise NotProven("ret-address", f"region {here}")
        else:
            if sa.r["esp"] is not sb.r["esp"]:
                raise NotProven("ret-esp", f"region {here} (tail call)")
        ma, mb = (sa.m, sb.m) if ctx.strict else (strip_frame(sa.m, ctx.esp0), strip_frame(sb.m, ctx.esp0))
        if ma is not mb:
            raise NotProven("ret-memory", f"region {here}: {show(ma)} | {show(mb)}")
        if sa.x is not sb.x:
            raise NotProven("ret-x87", f"region {here}")
        for r in CALLEE_SAVED:
            if sa.r[r] is not sb.r[r]:
                raise NotProven("ret-callee-saved", f"region {here}: {r} {show(sa.r[r])} | {show(sb.r[r])}")
        for r in ("eax", "edx", "ecx"):
            if not ctx.strict and (ctx.esp_offsets(sa.r[r]) or ctx.esp_offsets(sb.r[r])):
                raise Fallback("a stack address is returned")
            if sa.r[r] is not sb.r[r]:
                ctx.retdiff.add(r)
                ctx.retdiff_detail.setdefault(r, (here, show(sa.r[r])[:240], show(sb.r[r])[:240]))


# ====================================================================== relocation resolution
SYMMAP_PATH = pathlib.Path(__file__).resolve().parent / "equiv_symmap.tsv"
_SYMMAP = None


def symmap():
    global _SYMMAP
    if _SYMMAP is None:
        _SYMMAP = {}
        if SYMMAP_PATH.exists():
            for ln in SYMMAP_PATH.read_text().splitlines():
                if ln.startswith("#") or not ln.strip():
                    continue
                name, addr = ln.split("\t")[:2]
                _SYMMAP[name] = int(addr, 16)
    return _SYMMAP


def undecorate(n):
    return n.lstrip("_@").split("@")[0]


RE_G = re.compile(r"MGLOBALA?\(\s*[^,]+?,\s*(\w+)\s*,\s*(?:[^,;\n]+,\s*)?(0x[0-9a-fA-F]+)\s*\)")


class Resolver:
    """Resolves our object's relocations to the original's addresses (relocheck rules) and supplies callee
    facts. Own read-only data used as a value resolves only when the aligned original operand holds equal bytes."""

    def __init__(self, text, secs, syms, addr):
        self.gl = {name: int(a, 16) for name, a in RE_G.findall(text)}
        self.obj_secs = secs
        self.syms = syms
        self.addr = addr
        self.odata_map = {}     # (secidx, offset) -> original address (verified bytes)
        self.import_pop = {}    # IAT address -> pop count from our decoration
        self.unresolved = set()

    def sym_address(self, name):
        u = undecorate(name)
        if u in self.gl:
            return self.gl[u]
        m = re.fullmatch(r"(?:thunk_FUN|FUN|DAT|PTR|UNK|BYTE|WORD|DWORD|QWORD|FLOAT|DOUBLE|s|u|PTR_FUN|PTR_DAT)_([0-9a-fA-F]{8})", u)
        if m:
            return int(m.group(1), 16)
        sm = symmap()
        if name in sm:
            return sm[name]
        return None

    def resolve(self, side, symrec, field, typ):
        """Target of a relocated field = S + A (A = the signed field content), for DIR32 and REL32 alike."""
        name = symrec.get("name", "?")
        addend = field
        if symrec.get("sec") == side.secidx:
            v = symrec["value"] + addend
            if v == side.start:
                return C(side.addr)
            return mk("label", 32, v)
        a = self.sym_address(name)
        if a is not None:
            if name.startswith("__imp_"):
                m = re.search(r"@(\d+)$", name)
                self.import_pop[(a + addend) & M32] = int(m.group(1)) if m else 0
            return C(a + addend)
        if symrec.get("sec", 0) > 0:
            key = (symrec["sec"], symrec["value"] + addend)
            if key in self.odata_map:
                return C(self.odata_map[key])
            return add(mk("od", 32, symrec["sec"], symrec["value"]), C(addend))
        self.unresolved.add(name)
        return add(mk("unres", 32, name), C(addend))

    def callee(self, tgt):
        if tgt.op == "c":
            va = tgt.a[0]
            if va == self.addr:
                s = summary(va)
                return s
            sm = symmap()
            if any(va == v and undecorate(k) in ("chkstk", "alloca_probe") for k, v in sm.items()):
                return {"chkstk": True}
            return summary(va)
        d, _k = to_lin(tgt)
        if any(t.op == "unres" for t in d):
            raise NotProven("unresolved-callee", show(tgt))
        # indirect: an import through the IAT
        if tgt.op == "ld" and tgt.a[1].op == "c" and tgt.a[1].a[0] in self.import_pop:
            n = self.import_pop[tgt.a[1].a[0]]
            return {"P": n, "N": n // 4, "livein": frozenset(), "eax_def": False, "noreturn": False, "known": True}
        if tgt.op == "c":
            return summary(tgt.a[0])
        return {"indirect": True}


def verify_odata(text, A, B, res):
    """Map our own read-only data symbols used as values to the original's addresses: align the two linear
    disassemblies, and accept an aligned operand only when the original holds the same bytes."""
    ours = list(_MD.disasm(A.code[A.start:A.end], A.start))
    img = image()
    theirs = list(_MD.disasm(img.bytes(B.start, B.end - B.start) or b"", B.start))
    ka = [c.mnemonic for c in ours]
    kb = [c.mnemonic for c in theirs]
    sm = difflib.SequenceMatcher(None, ka, kb, autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            continue
        for k in range(i2 - i1):
            ca, cb = ours[i1 + k], theirs[j1 + k]
            for which in ("imm", "disp"):
                off = ca.imm_offset if which == "imm" else ca.disp_offset
                if not off:
                    continue
                r = A.relocs.get(ca.address + off)
                if r is None or r[0].get("sec", 0) <= 0 or r[0].get("sec") == A.secidx:
                    continue
                if res.sym_address(r[0].get("name", "")) is not None:
                    continue
                offb = cb.imm_offset if which == "imm" else cb.disp_offset
                if not offb:
                    continue
                raw = struct.unpack_from("<i", A.code, ca.address + off)[0]
                key = (r[0]["sec"], r[0]["value"] + raw)
                theirs_addr = struct.unpack_from("<I", img.bytes(cb.address + offb, 4))[0]
                sec = res.obj_secs[key[0] - 1]
                nxt = sorted(x["value"] for x in res.syms if x["sec"] == key[0] and x["value"] > key[1])
                end = min(nxt[0] if nxt else len(sec["data"]), key[1] + 64)
                ob = sec["data"][key[1]:end]
                if not ob or sec["chars"] & RO_FLAGS:
                    continue
                tb = img.bytes(theirs_addr, len(ob))
                if tb == ob and img.readonly(theirs_addr, len(ob)):
                    res.odata_map[key] = theirs_addr


# ====================================================================== attribution
REGN = re.compile(r"\b(e?[abcd]x|[abcd][lh]|e?[sd]i|e?bp)\b")


def attribute(A, B, ctx_used, frame_needed):
    """Post-proof labels: which named classes the difference consists of."""
    img = image()
    ours = list(_MD.disasm(*A.body()))
    theirs = list(_MD.disasm(*B.body()))

    def key(c, side):
        ops = []
        for op in c.operands:
            if op.type == X.X86_OP_REG:
                ops.append(c.reg_name(op.reg))
            elif op.type == X.X86_OP_IMM:
                if c.group(capstone.CS_GRP_JUMP) or c.group(capstone.CS_GRP_CALL):
                    ops.append("<t>")
                else:
                    reloc = isinstance(side, ObjSide) and c.imm_offset and (c.address + c.imm_offset) in side.relocs
                    ops.append("<a>" if reloc or (isinstance(side, ImageSide) and img.in_image(op.imm & M32)) else hex(op.imm & M32))
            else:
                m = op.mem
                reloc = isinstance(side, ObjSide) and c.disp_offset and (c.address + c.disp_offset) in side.relocs
                d = "<a>" if reloc or (isinstance(side, ImageSide) and img.in_image(m.disp & M32)) else hex(m.disp & M32)
                ops.append(f"[{c.reg_name(m.base) if m.base else ''}+{c.reg_name(m.index) + '*' + str(m.scale) if m.index else ''}+{d}]")
        return c.mnemonic + " " + ",".join(ops)

    def pad(c):
        return c.mnemonic in ("nop", "int3") or (c.mnemonic in ("lea", "mov") and len(c.operands) == 2 and
                                                 c.operands[0].type == X.X86_OP_REG and
                                                 ((c.operands[1].type == X.X86_OP_REG and c.operands[1].reg == c.operands[0].reg) or
                                                  (c.operands[1].type == X.X86_OP_MEM and c.operands[1].mem.base == c.operands[0].reg
                                                   and not c.operands[1].mem.index and c.operands[1].mem.disp == 0)))

    def trim(lst):
        lst = list(lst)
        while lst and (pad(lst[-1]) or lst[-1].mnemonic == "int3"):
            lst.pop()
        return lst
    ours, theirs = trim(ours), trim(theirs)
    oa = [c for c in ours if not pad(c)]
    ob = [c for c in theirs if not pad(c)]
    classes = set()
    pa = [bytes(c.bytes) for c in ours if pad(c)]
    pb = [bytes(c.bytes) for c in theirs if pad(c)]
    if pa != pb:
        classes.add("ENC")
    ka = [key(c, A) for c in oa]
    kb = [key(c, B) for c in ob]
    # trailing padding after the last ret
    sm = difflib.SequenceMatcher(None, ka, kb, autojunk=False)
    sigma, sigma_bad = {}, False
    residue = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                ca, cb = oa[i1 + k], ob[j1 + k]
                ba = bytes(ca.bytes)
                bb = bytes(cb.bytes)
                jumpish = ca.group(capstone.CS_GRP_JUMP) or ca.group(capstone.CS_GRP_CALL)
                if len(ba) != len(bb) or (not jumpish and not A.relocs_in(ca) and ba != bb):
                    classes.add("ENC")
            continue
        residue.append((tag, i1, i2, j1, j2))
    # moved instructions: a deleted run on one side that is inserted elsewhere on the other
    dels = [(i1, i2) for t, i1, i2, j1, j2 in residue if t == "delete"]
    inss = [(j1, j2) for t, i1, i2, j1, j2 in residue if t == "insert"]
    moved = set()
    for i1, i2 in dels:
        for j1, j2 in inss:
            if ka[i1:i2] == kb[j1:j2] and (j1, j2) not in moved:
                moved.add((j1, j2))
                moved.add(("a", i1, i2))
                cf = any(x.split()[0] in ("ret", "jmp") or x.split()[0].startswith("j") for x in ka[i1:i2])
                classes.add("LAYOUT" if cf or i2 - i1 >= 3 else "ORDER")
                break
    residue = [r for r in residue if not ((r[0] == "delete" and ("a", r[1], r[2]) in moved) or
                                          (r[0] == "insert" and (r[3], r[4]) in moved))]
    for tag, i1, i2, j1, j2 in residue:
        xa, xb = ka[i1:i2], kb[j1:j2]
        if xa and xb and len(xa) == len(xb) and _comm(xa, xb):
            classes.add("COMM")
            continue
        if xa and xb and len(xa) == len(xb):
            if all(_sib_swap(a, b) or a == b for a, b in zip(xa, xb)):
                classes.add("SIB")
                continue
            fa = [REGN.sub("R", a) for a in xa]
            fb = [REGN.sub("R", b) for b in xb]
            if fa == fb:
                for a, b in zip(xa, xb):
                    for ra, rb in zip(REGN.findall(a), REGN.findall(b)):
                        ra, rb = SUB[ra][0], SUB[rb][0]
                        if sigma.get(ra, rb) != rb:
                            sigma_bad = True
                        sigma[ra] = rb
                classes.add("REG")
                continue
            if sorted(xa) == sorted(xb):
                classes.add("ORDER")
                continue
            if sorted(fa) == sorted(fb):
                classes.add("ORDER")
                classes.add("REG")
                continue
            if _comm(xa, xb):
                classes.add("COMM")
                continue
        if tag in ("delete", "insert") or (tag == "replace"):
            seq = xa or xb
            other = kb if xa else ka
            if seq and all(s.split()[0] in ("jmp",) or s.split()[0].startswith("j") for s in seq):
                classes.add("LAYOUT")
                continue
            if tag != "replace" and _moved(seq, other):
                classes.add("LAYOUT")
                continue
        classes.add("VALUE-FLOW")
    if sigma and (sigma_bad or len(set(sigma.values())) != len(sigma)):
        classes.discard("REG")
        classes.add("VALUE-FLOW")
    if "polarity" in ctx_used:
        classes.add("LAYOUT")
    if frame_needed:
        classes.add("FRAME")
    order = ["ENC", "SIB", "REG", "ORDER", "LAYOUT", "COMM", "FRAME", "VALUE-FLOW"]
    return [c for c in order if c in classes], sigma


def _modrm_only(ca, cb):
    return None


def _sib_swap(a, b):
    ma = re.findall(r"\[([^+\]]*)\+([^*+\]]*)\*1\+", a)
    mb = re.findall(r"\[([^+\]]*)\+([^*+\]]*)\*1\+", b)
    if not ma or not mb:
        return False
    return re.sub(r"\[[^\]]*\]", "[]", a) == re.sub(r"\[[^\]]*\]", "[]", b) and \
        all(set(x) == set(y) for x, y in zip(ma, mb))


COMM_MN = {"add", "imul", "and", "or", "xor"}


def _comm(xa, xb):
    """`mov r, X ; op r, Y` against `mov r, Y ; op r, X` (op commutative), or one commutative op with swapped
    register operands."""
    if len(xa) == 2 and len(xb) == 2:
        (m1, a1), (m2, a2) = [s.split(" ", 1) for s in xa]
        (n1, b1), (n2, b2) = [s.split(" ", 1) for s in xb]
        if m1 == n1 == "mov" and m2 == n2 and m2 in COMM_MN:
            r1, x1 = a1.split(",", 1)
            r2, y1 = a2.split(",", 1)
            s1, x2 = b1.split(",", 1)
            s2, y2 = b2.split(",", 1)
            return r1 == r2 == s1 == s2 and x1 == y2 and y1 == x2
    return False


def _moved(seq, other):
    n = len(seq)
    return any(other[i:i + n] == seq for i in range(len(other) - n + 1))


# ====================================================================== callers (for EQUIV-IF-CALLERS-IGNORE)
_CALLERS = {}


def callers(va):
    if va in _CALLERS:
        return _CALLERS[va]
    img = image()
    text = [s for s in img.secs if s[3] == ".text"]
    out = []
    for lo, hi, _, _ in text:
        data = img.bytes(lo, hi - lo) or b""
        for m in re.finditer(b"\xE8", data):
            i = m.start()
            if i + 5 > len(data):
                continue
            tgt = (lo + i + 5 + struct.unpack_from("<i", data, i + 1)[0]) & M32
            if tgt == va or (img.in_image(tgt) and resolve_thunk(tgt) == va and tgt != va):
                out.append(lo + i)
    _CALLERS[va] = out
    return out


def caller_reads(site, reg, depth=0, seen=None):
    """'ignores' | 'reads' | 'unknown': does the caller read `reg` after the call at `site` before writing it?"""
    img = image()
    seen = seen if seen is not None else set()
    pc = site + 5
    for _ in range(40):
        if pc in seen:
            return "ignores"
        seen.add(pc)
        b = img.bytes(pc, 16)
        if b is None:
            return "unknown"
        try:
            cs = next(_MD.disasm(b, pc))
        except StopIteration:
            return "unknown"
        rd, wr = cs.regs_access()
        rdp = {SUB.get(cs.reg_name(r), (None,))[0] for r in rd}
        wrfull = {cs.reg_name(r) for r in wr}
        mn = cs.mnemonic
        zero_idiom = mn in ("xor", "sub") and len(cs.operands) == 2 and cs.operands[0].type == X.X86_OP_REG and \
            cs.operands[1].type == X.X86_OP_REG and cs.operands[0].reg == cs.operands[1].reg
        if reg in rdp and not zero_idiom and mn not in ("call",):
            return "reads"
        if mn == "call":
            if cs.operands[0].type == X.X86_OP_IMM:
                s = summary(cs.operands[0].imm)
                if reg in s["livein"]:
                    return "reads"
            else:
                if reg in ("ecx", "edx"):
                    return "unknown"
            if reg in ("eax", "ecx", "edx"):
                return "ignores"
        if reg in wrfull:
            return "ignores"
        if mn in ("ret", "retn"):
            return "unknown"
        if cs.group(capstone.CS_GRP_JUMP):
            op = cs.operands[0]
            if op.type != X.X86_OP_IMM or depth > 4:
                return "unknown"
            if mn == "jmp":
                pc = op.imm
                continue
            r1 = caller_reads(op.imm - 5, reg, depth + 1, set(seen))
            if r1 != "ignores":
                return r1
        pc += cs.size
    return "unknown"


# ====================================================================== driver
def _relocs_in(self, c):
    return any((c.address + k) in self.relocs for k in range(c.size))


ObjSide.relocs_in = _relocs_in


def check_function(src_text, secs, syms, addr, sym, trace=False, size=None, orig_side=None):
    """Returns a verdict dict for one function."""
    reset_terms()
    size = size or match.ghidra_size(addr)
    if size is None:
        return {"addr": addr, "verdict": "NOT-PROVEN", "kind": "no-size", "detail": "no Ghidra size"}
    res = Resolver(src_text, secs, syms, addr)
    A = ObjSide("ours", secs, syms, sym, addr, res)
    B = orig_side or ImageSide("orig", addr, size)
    out = {"addr": addr, "sym": sym}
    try:
        verify_odata(src_text, A, B, res)
    except Exception as e:  # alignment is best effort; unresolved operands stay opaque
        out["odata_note"] = str(e)[:80]
    try:
        n, bad = match.byte_compare(addr, secs, syms, A.sym["name"])
        out["bytes_bad"] = len(bad)
    except BaseException:  # noqa: BLE001
        out["bytes_bad"] = None
    return check_sides(A, B, res, addr, out, trace)


def check_sides(A, B, res, addr, out=None, trace=False):
    """The proof for two code providers (ours, original). Used by check_function and by the synthetic tests.

    A6: the original's calls through a pointer get their pop counts from pin_pops. With one solution the proof
    runs once. With several (the stack equations do not separate some pops), it runs once per solution, up to
    POP_SOLUTIONS_MAX, and the verdict is the weakest of the runs. The real counts are one of the solutions, so
    the verdict holds for them."""
    out = {"addr": addr} if out is None else out
    if isinstance(A, ObjSide):
        # resolve our import relocations first: Resolver.import_pop (IAT address -> pop count from our decoration)
        # is what makes the original's `call [IAT]` an import call, and the original now runs first (A6)
        for off, (symrec, typ) in sorted(A.relocs.items()):
            if symrec.get("name", "").startswith("__imp_"):
                try:
                    res.resolve(A, symrec, struct.unpack_from("<i", A.code, off)[0], typ)
                except NotProven:
                    pass
    pp = Ctx(A, B, False, res).pins(B)
    sols = pp.get("solutions")
    if not sols:
        sols = [None]                     # no solution set (pp["why"]): A4 for every call through a pointer
        if pp.get("why"):
            out["pop_why"] = pp["why"]
    elif len(sols) > POP_SOLUTIONS_MAX:
        out.update(verdict="NOT-PROVEN", kind="callee-pop-ambiguous",
                   detail=f"{len(sols)} solutions of the original's stack equations for {len(pp['free'])} calls "
                          f"through a pointer ({', '.join(B.label(v) for v in pp['free'])}); limit {POP_SOLUTIONS_MAX}")
        return out
    runs = []
    for i, sol in enumerate(sols):
        o = _check_once(A, B, res, addr, dict(out), trace, sol, len(sols) == 1)
        if o["verdict"] == "NOT-PROVEN":
            if len(sols) > 1:
                o["detail"] = f"pop solution {i + 1}/{len(sols)} {{{', '.join(f'{B.label(k)}:{v}' for k, v in sorted(sol.items()))}}}: " + o["detail"]
            return o
        runs.append(o)
    if len(runs) == 1:
        return runs[0]
    best = max(runs, key=lambda o: len(o.get("regs", ())))
    for o in runs:
        if not set(o.get("regs", ())) <= set(best.get("regs", ())):
            best = dict(best, verdict="NOT-PROVEN", kind="callee-pop-solutions",
                        detail="return registers differ between pop solutions")
            return best
    best["pop_solutions"] = len(runs)
    return best


def _check_once(A, B, res, addr, out, trace, popsol, popsol_unique):
    results = {}
    for strict in (False, True):
        reset_keep = None
        ctx = Ctx(A, B, strict, res, trace, popsol, popsol_unique)
        try:
            Explorer(ctx).run()
            results[strict] = ("EQUIV", ctx)
        except Fallback as e:
            results[strict] = ("FALLBACK", str(e))
            if strict:
                results[strict] = ("NOT-PROVEN", NotProven("internal", "fallback in strict mode"))
        except NotProven as e:
            results[strict] = ("NOT-PROVEN", e)
        except RecursionError:
            results[strict] = ("NOT-PROVEN", NotProven("internal", "recursion"))
        if not strict and results[False][0] == "EQUIV":
            # strict rerun only for attribution of FRAME
            continue
        if not strict and results[False][0] == "NOT-PROVEN":
            break
    d, s = results[False], results.get(True)
    if d[0] == "EQUIV":
        ctx = d[1]
        frame_needed = not (s and s[0] == "EQUIV")
        mode = "frame"
    elif d[0] == "FALLBACK" and s and s[0] == "EQUIV":
        ctx = s[1]
        frame_needed = False
        mode = "strict"
    else:
        e = d[1] if d[0] == "NOT-PROVEN" else (s[1] if s else NotProven("internal", "?"))
        if d[0] == "FALLBACK":
            out["fallback"] = d[1]
        out.update(verdict="NOT-PROVEN", kind=e.kind, detail=e.detail)
        return out
    classes, sigma = attribute(A, B, ctx.used, frame_needed)
    if isinstance(A, ObjSide):
        loc = sorted({r[0]["name"] for r in A.relocs.values() if r[0].get("type") == 0x20 and r[0].get("sec", 0) > 0
                      and r[0].get("sec") != A.secidx})
        if loc:
            out["local_callees"] = loc     # bodies this source defines and calls: an admission must cover them too
    out["classes"] = classes
    out["mode"] = mode
    out["rules"] = sorted(ctx.used)
    if res.unresolved:
        out["unresolved_unused"] = sorted(res.unresolved)
    if ctx.retdiff:
        regs = sorted(ctx.retdiff, key=["eax", "edx", "ecx"].index)
        cl = callers(addr)
        rows = []
        for site in cl:
            rows.append((site, {r: caller_reads(site, r) for r in regs}))
        out["retdiff"] = ctx.retdiff_detail
        out.update(verdict="EQUIV-IF-CALLERS-IGNORE", regs=regs,
                   callers=[{"site": f"0x{s:08x}", **v} for s, v in rows],
                   callers_all_ignore=all(all(x == "ignores" for x in v.values()) for _, v in rows))
    else:
        out["verdict"] = "EQUIV"
    return out


def check_source(path, only=None, trace=False):
    path = pathlib.Path(path)
    text = path.read_text()
    anns = match.parse_annotations(text)
    lang = match.file_lang(text)
    tc = match.file_toolchain(text)
    fl = match.file_flags(text) or match.DEFAULT_FLAGS
    obj = match.compile_obj(path, fl, lang, tc)
    secs, syms = coff_full(obj)
    outs = []
    for addr, sym in anns:
        if only is not None and addr not in (only if isinstance(only, set) else {only}):
            continue
        try:
            outs.append(check_function(text, secs, syms, addr, sym, trace))
        except NotProven as e:
            outs.append({"addr": addr, "sym": sym, "verdict": "NOT-PROVEN", "kind": e.kind, "detail": e.detail})
    return outs


def fmt(r):
    a = f"0x{r['addr']:08x}"
    if r["verdict"] == "NOT-PROVEN":
        extra = f" (fallback: {r['fallback']})" if r.get("fallback") else ""
        return f"{a}\tNOT-PROVEN\t{r['kind']}: {r['detail']}{extra}"
    cls = ",".join(r["classes"]) or "BYTE-IDENTICAL"
    if r.get("pop_solutions"):
        cls += f"; {r['pop_solutions']} pop solutions"
    if r["verdict"] == "EQUIV":
        return f"{a}\tEQUIV\t[{cls}]"
    cl = r["callers"]
    ign = sum(1 for c in cl if all(v == "ignores" for k, v in c.items() if k != "site"))
    return (f"{a}\tEQUIV-IF-CALLERS-IGNORE({','.join(x.upper() for x in r['regs'])})\t[{cls}]\t"
            f"callers {len(cl)}, scan says ignore: {ign}/{len(cl)}: " +
            " ".join(f"{c['site']}:" + "/".join(c[x] for x in r["regs"]) for c in cl[:30]))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src", nargs="+")
    ap.add_argument("--only", default="")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--trace", action="store_true")
    ap.add_argument("--relax-icall", action="store_true")
    ap.add_argument("--x87-strict", action="store_true")
    a = ap.parse_args()
    global RELAX_ICALL, X87_STRICT
    RELAX_ICALL = a.relax_icall
    X87_STRICT = a.x87_strict
    only = {int(x, 16) for x in a.only.split(",")} if a.only else None
    allres = []
    for s in a.src:
        try:
            rs = check_source(s, only, a.trace)
        except SystemExit as e:
            print(f"{s}\tERROR\t{str(e).splitlines()[0][:160]}")
            continue
        for r in rs:
            allres.append(r)
            if not a.json:
                print(fmt(r))
    if a.json:
        print(json.dumps(allres, indent=1, default=str))
    return 0


if __name__ == "__main__":
    sys.exit(main())
