"""Installers of MASHED.exe functions that are reached through a stored pointer (vtable slot, callback argument,
function-pointer global, function table).

Ported from SimGolf re/tools/pointer_callers.py (2026-10-11). The C2->C3 caller half (re/CONFIDENCE.md L23/L25) is
checked with Ghidra's getCallingFunctions (DecompPC.java `callers`, re/tools/caller_screen.py). That set holds every
function that has *a reference* to the entry, so a `push offset fn` Ghidra recognised already counts. It never holds:
  * a function whose only reference is a slot of a table in .rdata/.data (vtables, callback tables): the referencing
    address is data, so there is no containing function;
  * the code that installs or dispatches through that table (a constructor's `mov [ecx], vtable`, a `call [global]`);
  * an operand Ghidra never turned into a reference.
This tool finds those mechanically from the raw image bytes; Ghidra's reference database is used only as the witness
for the precision gate (--precision), never as an input.

  1. every aligned or unaligned dword in the mapped image equal to a function entry is a candidate reference;
  2. a candidate inside a function's code counts only when it is the immediate or displacement of an instruction
     decoded linearly from the function start (capstone). The instruction classifies it:
        push imm32 + the next `call` in the function        -> rw_register | callback_arg   (callee recorded)
        mov [mem], imm32                                    -> store_mem
        mov reg, imm32 / lea reg, [disp32]                  -> load_reg
        anything else (cmp, test, ...)                      -> not an installer; counted, never emitted
  3. a candidate in data is a TABLE slot. Walking back through dwords that point into code (and, for a NULL-led table,
     through NULL slots, at most 0x400 bytes) the table start is the nearest address that some instruction holds as an
     operand. Every such instruction is an installer site. Its class names the row kind:
        call/jmp [start] or mov reg, [start] on slot 0      -> ptr_call / ptr_load   (a global function pointer)
        mov dword ptr [reg], start  (displacement 0)        -> vtable                (constructor storing a vptr)
        anything else                                       -> fn_table
     A slot whose table no instruction references is emitted as kind data_orphan with no installer site.
     Unaligned data hits are dropped: every installer-bearing data row of the 2026-10-11 run was 4-aligned.
  4. a candidate in .text outside every function is emitted as text_orphan (undiscovered code; see undiscovered.py
     in SimGolf); no installer is claimed.

rw_register: the callee is RwPluginRegistryAddPlugin 0x004d7de0 (callee of RwEngineRegisterPlugin 0x004c2d90 at
0x004c2dab and of RpAtomicRegisterPlugin 0x004e7d40 at 0x004e7d5e), a wrapper of it (body <= 0x40 bytes, calls a
member), or a function whose name matches REG_NAME.

An installer is evidence that the function is REACHABLE, not that it runs, and not that the installer calls it.
Whether an installer satisfies the caller half is a rubric question for re/CONFIDENCE.md; this tool does not answer it.

    py -3.12 re/tools/pointer_callers.py 0x<rva>                 # every reference, with kind / slot / installer
    py -3.12 re/tools/pointer_callers.py --all [-o out.tsv]      # one row per (target, installer site), every function
    py -3.12 re/tools/pointer_callers.py --precision             # gate: rows vs Ghidra's reference database
    py -3.12 re/tools/pointer_callers.py --refresh               # re-export the Ghidra listing (pool slot, read-only)

Inputs: original/MASHED.exe.unpatched (SHA-256 anchor checked), and the RefGraphPC.java export (functions.tsv,
refs.tsv) cached under log/refgraph/. --refresh rebuilds the cache through decomp_pc.py's pool acquire/release.
"""
import argparse, bisect, csv, hashlib, io, pathlib, re, struct, subprocess, sys

import capstone
from capstone import x86 as cx
import pefile

ROOT = pathlib.Path(__file__).resolve().parents[2]
ANCHOR = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
RW_ADD_PLUGIN = 0x004d7de0
REG_NAME = re.compile(r"Register(Plugin|PluginStream|Interpolation)|SetStream\w*CallBack|PluginAttach", re.I)
ANON = re.compile(r"^(FUN_|LAB_|thunk_FUN_|sub_)")
INSTALLER_KINDS = ("rw_register", "callback_arg", "store_mem", "load_reg", "ptr_call", "ptr_load", "vtable", "fn_table")


def _main_repo():
    """The worktree has no original/ (never junction it in, KNOWN_ISSUES WORKTREE-SYMLINK-WIPE): use the main repo's."""
    for c in (ROOT, *ROOT.parents):
        if (c / "original" / "MASHED.exe.unpatched").exists():
            return c
    raise SystemExit("original/MASHED.exe.unpatched not found above " + str(ROOT))


def refgraph_dir():
    return _main_repo() / "log" / "refgraph"


def refresh():
    sys.path.insert(0, str(ROOT / "re" / "tools"))
    import decomp_pc
    out = refgraph_dir()
    out.mkdir(parents=True, exist_ok=True)
    name, idx = decomp_pc.acquire_slot()
    try:
        cmd = [str(decomp_pc.GH), str(decomp_pc.POOL_DIR), name, "-process", decomp_pc.PROGRAM, "-readOnly",
               "-noanalysis", "-scriptPath", str(decomp_pc.SCRIPTS), "-postScript", "RefGraphPC.java", str(out)]
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=900)
        print("\n".join(l for l in r.stdout.splitlines() if "RefGraphPC" in l))
    finally:
        decomp_pc.release_slot(idx)


def load_hooks():
    raw = io.open(ROOT / "hooks.csv", encoding="utf-8", newline="").read()
    h = {}
    for r in csv.reader(io.StringIO(raw, newline="")):
        if r and re.fullmatch(r"[0-9a-fA-F]{8}", r[0].strip()):
            h[int(r[0], 16)] = {"name": r[1], "conf": r[3]}
    return h


def qualifies(h, fn):
    """caller_screen.py's rule: C2+ counts, so does a hooks.csv name that is not FUN_/LAB_/thunk_/sub_."""
    ci = h.get(fn)
    return bool(ci) and (ci["conf"] in ("C2", "C3", "C4") or not ANON.match(ci["name"]))


class Image:
    def __init__(self):
        exe = _main_repo() / "original" / "MASHED.exe.unpatched"
        blob = exe.read_bytes()
        if hashlib.sha256(blob).hexdigest().upper() != ANCHOR:
            raise SystemExit(f"{exe}: SHA-256 does not match the version anchor; RVAs are invalid")
        pe = pefile.PE(data=blob, fast_load=True)
        self.base = pe.OPTIONAL_HEADER.ImageBase
        self.img = pe.get_memory_mapped_image()
        self.text = [(self.base + s.VirtualAddress, self.base + s.VirtualAddress + s.Misc_VirtualSize)
                     for s in pe.sections if s.Characteristics & 0x20000000]
        g = refgraph_dir()
        if not (g / "functions.tsv").exists():
            raise SystemExit(f"{g}/functions.tsv missing: run with --refresh")
        self.names, self.ranges, self.size = {}, [], {}
        for r in csv.DictReader(io.open(g / "functions.tsv", encoding="utf-8"), delimiter="\t"):
            e = int(r["entry"], 16)
            self.names[e] = r["name"]
            tot = 0
            for rg in r["ranges"].split(";"):
                if "-" in rg:
                    lo, hi = (int(x, 16) for x in rg.split("-"))
                    self.ranges.append((lo, hi, e))
                    tot += hi - lo
            self.size[e] = tot
        self.ranges.sort()
        self._starts = [x[0] for x in self.ranges]
        self._dec = {}
        self._fn_ins = {}

    # --- geometry -------------------------------------------------------------------------------------------
    def function_at(self, va):
        i = bisect.bisect_right(self._starts, va) - 1
        if i >= 0 and self.ranges[i][0] <= va < self.ranges[i][1]:
            return self.ranges[i][2]
        return None

    def in_text(self, v):
        return any(lo <= v < hi for lo, hi in self.text)

    def dword(self, va):
        o = va - self.base
        return struct.unpack_from("<I", self.img, o)[0] if 0 <= o <= len(self.img) - 4 else None

    def find_all(self, value):
        needle, out = struct.pack("<I", value), []
        i = self.img.find(needle)
        while i >= 0:
            out.append(self.base + i)
            i = self.img.find(needle, i + 1)
        return out

    # --- decoding -------------------------------------------------------------------------------------------
    def _decode_range(self, lo, hi, entry):
        if (lo, hi) in self._dec:
            return self._dec[(lo, hi)]
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        md.detail = True
        md.skipdata = True
        ops, ins_list = {}, []
        for ins in md.disasm(self.img[lo - self.base:hi - self.base], lo):
            if ins.id == 0:
                continue
            tgt = None
            if ins.mnemonic == "call":
                op = ins.operands[0]
                tgt = op.imm if op.type == cx.X86_OP_IMM else "indirect"
            ins_list.append((ins.address, ins.mnemonic, tgt))
            for off in (ins.imm_offset, ins.disp_offset):
                if off:
                    ops[ins.address + off] = (ins.address, self._classify(ins, off))
        self._dec[(lo, hi)] = ops
        self._fn_ins.setdefault(entry, []).extend(ins_list)
        self._fn_ins[entry].sort()
        return ops

    @staticmethod
    def _classify(ins, off):
        """What the instruction does with the dword at ins.address+off."""
        m = ins.mnemonic
        ops = ins.operands
        is_imm = off == ins.imm_offset
        if m == "push" and is_imm:
            return "push"
        if m == "mov" and len(ops) == 2:
            d, s = ops
            if is_imm and d.type == cx.X86_OP_MEM:
                vptr = d.mem.base != 0 and d.mem.index == 0 and d.mem.disp == 0
                return "store_vptr" if vptr else "store_mem"
            if is_imm and d.type == cx.X86_OP_REG:
                return "load_reg"
            if not is_imm and s.type == cx.X86_OP_MEM:
                return "read_mem"
        if m == "lea" and not is_imm:
            return "load_reg"
        if m in ("call", "jmp") and not is_imm:
            return "call_mem"
        return "other:" + m

    def code_ref(self, va):
        """(function entry, instruction address, class) when `va` is an operand of a decoded instruction, else None."""
        i = bisect.bisect_right(self._starts, va) - 1
        if i < 0 or not (self.ranges[i][0] <= va < self.ranges[i][1]):
            return None
        lo, hi, entry = self.ranges[i]
        hit = self._decode_range(lo, hi, entry).get(va)
        return (entry, hit[0], hit[1]) if hit else None

    def next_call(self, entry, after):
        lst = self._fn_ins.get(entry, [])
        j = bisect.bisect_right(lst, (after, "~", None))
        for a, m, t in lst[j:j + 24]:
            if m == "call":
                return t
            if m in ("ret", "retn"):
                return None
        return None


def rw_register_set(im, hooks):
    """0x004d7de0 plus its short wrappers (fixpoint), plus anything REG_NAME names."""
    out = {RW_ADD_PLUGIN}
    out |= {e for e, n in im.names.items() if REG_NAME.search(n)}
    out |= {e for e, r in hooks.items() if REG_NAME.search(r["name"])}
    changed = True
    while changed:
        changed = False
        for e, sz in im.size.items():
            if e in out or sz > 0x40:
                continue
            for lo, hi, en in im.ranges:
                if en == e:
                    im._decode_range(lo, hi, en)
            if any(m == "call" and t in out for _, m, t in im._fn_ins.get(e, [])):
                out.add(e)
                changed = True
    return out


def references(im, target, regset):
    """[(kind, site, installer_fn, detail, ref_va)] for the function entry `target`. Non-installer operand uses are
    returned with kind 'operand:<class>' so the caller can count them; they are never written to the TSV."""
    out = []
    for ref in im.find_all(target):
        owner = im.function_at(ref)
        if owner is not None:
            cr = im.code_ref(ref)
            if cr is None:
                continue                                    # bytes inside code that are not an operand
            fn, site, cls = cr
            if cls == "push":
                callee = im.next_call(fn, site)
                kind = "rw_register" if callee in regset else "callback_arg"
                det = "push; next call " + (f"0x{callee:08x}" if isinstance(callee, int) else str(callee))
                out.append((kind, site, fn, det, ref))
            elif cls in ("store_mem", "store_vptr"):
                out.append(("store_mem", site, fn, "mov [mem], imm", ref))
            elif cls == "load_reg":
                out.append(("load_reg", site, fn, "mov/lea reg, imm", ref))
            else:
                out.append(("operand:" + cls, site, fn, "", ref))
            continue
        if im.in_text(ref):
            out.append(("text_orphan", None, None, "in .text outside every function", ref))
            continue
        if ref % 4:
            continue        # MSVC aligns every data pointer; all 63 unaligned hits on 2026-10-11 were orphans (noise)
        # data slot: walk back to the nearest address an instruction holds as an operand
        start, users = ref, []
        while im.dword(start) is not None and im.in_text(im.dword(start)):
            users = [u for u in map(im.code_ref, im.find_all(start)) if u is not None]
            if users:
                break
            start -= 4
        if not users and im.dword(start) == 0:
            probe = start
            while ref - probe < 0x400 and im.dword(probe) is not None \
                    and (im.dword(probe) == 0 or im.in_text(im.dword(probe))):
                users = [u for u in map(im.code_ref, im.find_all(probe)) if u is not None]
                if users:
                    start = probe
                    break
                probe -= 4
        if not users:
            out.append(("data_orphan", None, None, "no instruction references any slot at or before it", ref))
            continue
        slot = (ref - start) // 4
        vt = any(c == "store_vptr" for _, _, c in users)
        for fn, site, cls in users:
            if slot == 0 and cls == "call_mem":
                kind = "ptr_call"
            elif slot == 0 and cls == "read_mem":
                kind = "ptr_load"
            elif vt:
                kind = "vtable"
            else:
                kind = "fn_table"
            out.append((kind, site, fn, f"slot {slot} (+0x{slot * 4:x}) of 0x{start:08x}; {cls}", ref))
    return out


def all_rows(im, hooks):
    regset = rw_register_set(im, hooks)
    rows, skipped = [], 0
    for t in sorted(im.names):
        for kind, site, fn, det, ref in references(im, t, regset):
            if kind.startswith("operand:"):
                skipped += 1
                continue
            rows.append((t, site, kind, fn, det, ref))
    return rows, skipped


TABLE_START = re.compile(r"slot (\d+) \S+ of 0x([0-9a-f]{8})")


def boundaries(im, rows):
    """{(target, site, ref): verdict} for every row: 'n/a' for code kinds, 'resolved' or 'suspect@0x<p>' for tables.

    Mashed has no RTTI, so the end of one vtable and the start of the next are visible only through code that
    references the start. A table start that NO code references (an abstract base built __declspec(novtable), a vptr
    store the optimiser removed) is invisible, and the walk-back in references() runs straight across it, crediting
    the earlier table's installers with the later table's slots. The test: a derived vtable repeats the inherited
    prefix of its base, so a hidden start shows up as a repeat. Position p in (start, ref] is a suspected boundary
    when its first two dwords are non-NULL, DISTINCT, and equal the first two dwords of any table start in `rows`
    (this table's own included). Distinct, because a shared empty virtual fills consecutive slots inside one vtable:
    on 2026-10-11 the pair (0x0045b350, 0x0045b350) RwInitNullStub x2 at slot 18 of 0x005cf9e8 / 0x005cfd70 /
    0x005d0eb0 / 0x005d0dc0 matched a referenced table that starts with the same two stubs, and flagged 168 rows. A 'suspect' row must not be used as a caller (re/CONFIDENCE.md installer clause). 'resolved'
    means this test found no hidden start; a hidden table whose prefix matches no referenced table is not
    detectable here.
    """
    starts = set()
    for t, site, kind, fn, det, ref in rows:
        m = TABLE_START.search(det)
        if m:
            starts.add(int(m.group(2), 16))
    prefixes = {(im.dword(s), im.dword(s + 4)) for s in starts}
    out = {}
    for t, site, kind, fn, det, ref in rows:
        m = TABLE_START.search(det)
        if not m:
            out[(t, site, ref)] = "n/a"
            continue
        start, verdict = int(m.group(2), 16), "resolved"
        for p in range(start + 4, ref + 1, 4):
            pair = (im.dword(p), im.dword(p + 4))
            if pair[0] and pair[1] and pair[0] != pair[1] and pair in prefixes:
                verdict = f"suspect@0x{p:08x}"
                break
        out[(t, site, ref)] = verdict
    return out


def write_tsv(rows, hooks, im, path):
    lvl = lambda a: hooks.get(a, {}).get("conf", "-") if a is not None else "-"
    nm = lambda a: (hooks.get(a, {}).get("name") or im.names.get(a, "?")) if a is not None else "-"
    hx = lambda a: f"0x{a:08x}" if a is not None else "-"
    with io.open(path, "w", encoding="utf-8", newline="") as f:
        f.write("target_rva\tinstaller_site_rva\tkind\ttarget_level\ttarget_name\tinstaller_fn\tinstaller_level"
                "\tinstaller_name\tref_va\tdetail\tboundary\n")
        bd = boundaries(im, rows)
        for t, site, kind, fn, det, ref in rows:
            f.write(f"{hx(t)}\t{hx(site)}\t{kind}\t{lvl(t)}\t{nm(t)}\t{hx(fn)}\t{lvl(fn)}\t{nm(fn)}\t{hx(ref)}"
                    f"\t{det}\t{bd[(t, site, ref)]}\n")


FLOW = re.compile(r"CALL|JUMP|FALL_THROUGH|FLOW|TERMINATOR")


def load_refs():
    """Ghidra's reference database: {(from, to)} and {to: {types}} -- the witness, never an input."""
    pairs, types = set(), {}
    for r in csv.DictReader(io.open(refgraph_dir() / "refs.tsv", encoding="utf-8"), delimiter="	"):
        fr, to = int(r["from"], 16), int(r["to"], 16)
        pairs.add((fr, to))
        types.setdefault(to, set()).add(r["type"])
    return pairs, types


def precision(im, rows):
    """Each emitted installer row is a mechanical claim. Ghidra confirms it when its reference database holds the
    same edge: code kinds need (site -> target); table kinds need (slot -> target) AND (site -> table start).
    Held-out set: targets Ghidra already holds a non-flow reference to (its own analysis already found a stored
    pointer, so the true installer set is independently known). Rows on other targets are out of sample."""
    pairs, types = load_refs()
    held = {t for t, ty in types.items() if any(not FLOW.search(x) for x in ty)}
    stats, misses = {}, []
    for t, site, kind, fn, det, ref in rows:
        if kind not in INSTALLER_KINDS or t not in held:
            continue
        if kind in ("rw_register", "callback_arg", "store_mem", "load_reg"):
            ok = (site, t) in pairs
        else:
            start = int(re.search(r"of 0x([0-9a-f]{8})", det).group(1), 16)
            ok = (ref, t) in pairs and (site, start) in pairs
        s = stats.setdefault(kind, [0, 0])
        s[0] += 1
        s[1] += ok
        if not ok:
            misses.append((t, site, kind, det))
    return stats, misses


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("addr", nargs="?")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("-o", "--out")
    ap.add_argument("--precision", action="store_true")
    ap.add_argument("--refresh", action="store_true")
    a = ap.parse_args()
    if a.refresh:
        refresh()
        return 0
    im, hooks = Image(), load_hooks()
    if a.addr:
        t = int(a.addr, 16)
        regset = rw_register_set(im, hooks)
        refs = references(im, t, regset)
        if not refs:
            print("no stored pointer to this function")
        for kind, site, fn, det, ref in refs:
            who = f"0x{fn:08x} {hooks.get(fn, {}).get('name') or im.names.get(fn, '?')}" if fn else "none"
            print(f"0x{ref:08x} {kind:13} site {'0x%08x' % site if site else '-':10} {det}; installer: {who}")
        return 0
    rows, skipped = all_rows(im, hooks)
    if a.all:
        out = pathlib.Path(a.out) if a.out else ROOT / "re" / "analysis" / "pointer_callers.tsv"
        write_tsv(rows, hooks, im, out)
        print(f"{len(rows)} rows ({len({r[0] for r in rows})} targets) -> {out}; {skipped} non-installer operand uses")
    if a.precision:
        stats, misses = precision(im, rows)
        tot = sum(s[0] for s in stats.values()); ok = sum(s[1] for s in stats.values())
        for k, (n, c) in sorted(stats.items()):
            print(f"  {k:13} {c:5}/{n:<5} {100 * c / max(n, 1):6.2f}%")
        print(f"  {'ALL':13} {ok:5}/{tot:<5} {100 * ok / max(tot, 1):6.2f}%")
        for m in misses[:40]:
            print("  miss", " ".join(f"0x{x:08x}" if isinstance(x, int) else str(x) for x in m))
    return 0


if __name__ == "__main__":
    sys.exit(main())
