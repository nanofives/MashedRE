"""Offline cross-references for MASHED.exe: per-function facts from Ghidra (one headless dump of every
function's name, calling convention, prototype, thunk target, body ranges and references-to-entry) joined
with the anchored bytes (capstone over `original/MASHED.exe.unpatched`). Ported from SimGolf
`re/tools/xref.py`; the Ghidra side is `re/tools/ghidra_scripts/FuncFactsPC.java`.

    py -3.12 re/tools/xref.py 0x004a4541              # one function: callers, callees, strings
    py -3.12 re/tools/xref.py --string 0x005cd663     # functions that reference an address in the image
    py -3.12 re/tools/xref.py --rebuild-facts         # re-dump the Ghidra facts (read-only pool slot)

Caches: `log/ghidra_funcfacts.jsonl` (Ghidra dump; only changes if the master project is re-analysed) and
`log/xref_mashed.json` (call graph). The Ghidra run acquires a pool slot through `scripts/ghidra_pool.sh`,
opens the clone `-readOnly -noanalysis`, and releases the slot; nothing is written to any project.
Addresses are absolute VAs (image base 0x00400000), as everywhere in this repo."""
import argparse, json, pathlib, subprocess, sys, tempfile

import capstone
import pefile

ROOT = pathlib.Path(__file__).resolve().parents[2]


def _main_repo():
    """Directory holding `original/` and `mashed_pool/`. A worktree (.worktrees/<name>/) has neither, so walk up."""
    for p in (ROOT, *ROOT.parents):
        if (p / "original" / "MASHED.exe.unpatched").is_file():
            return p
    raise SystemExit("original/MASHED.exe.unpatched not found above " + str(ROOT))


MAIN = _main_repo()
EXE = MAIN / "original" / "MASHED.exe.unpatched"
ANCHOR_SHA256 = "bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e"
FACTS = ROOT / "log" / "ghidra_funcfacts.jsonl"
CACHE = ROOT / "log" / "xref_mashed.json"
GH = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\TD5RE\ghidra\ghidra_12.0.3_PUBLIC\support\analyzeHeadless.bat")
SCRIPTS = pathlib.Path(__file__).resolve().parent / "ghidra_scripts"


def _bash():
    # `bash` on PATH is WSL here and cannot open Windows paths (memory: bash-on-path-is-wsl-not-git-bash).
    for c in (r"C:\Program Files\Git\bin\bash.exe", r"C:\Program Files (x86)\Git\bin\bash.exe"):
        if pathlib.Path(c).exists():
            return c
    return "bash"


def check_anchor():
    import hashlib
    h = hashlib.sha256(EXE.read_bytes()).hexdigest()
    if h != ANCHOR_SHA256:
        raise SystemExit(f"{EXE} SHA-256 {h} != anchor {ANCHOR_SHA256}: RVAs are invalid, re-anchor first")


_PE = None


def load_pe():
    global _PE
    if _PE is None:
        pe = pefile.PE(str(EXE), fast_load=True)
        pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"]])
        _PE = pe
    return _PE


def base():
    return load_pe().OPTIONAL_HEADER.ImageBase


def iat():
    """IAT slot VA -> (dll, symbol)."""
    pe = load_pe()
    return {i.address: (e.dll.decode(), (i.name or b"#%d" % i.ordinal).decode())
            for e in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []) for i in e.imports}


def section_of(va):
    """(name, writable, file_backed) for an image VA, or None outside every section.
    file_backed is False past SizeOfRawData (BSS tail: no bytes in the file, zero at load)."""
    pe = load_pe()
    rva = va - base()
    for s in pe.sections:
        lo, vs = s.VirtualAddress, max(s.Misc_VirtualSize, s.SizeOfRawData)
        if lo <= rva < lo + vs:
            name = s.Name.rstrip(b"\0").decode(errors="replace")
            return name, bool(s.Characteristics & 0x80000000), (rva - lo) < s.SizeOfRawData
    return None


def read_bytes(va, n):
    """File bytes at va, or None when any of them lies outside the file-backed part of a section."""
    sec = section_of(va)
    if not sec or not sec[2] or not (section_of(va + n - 1) or (None, None, False))[2]:
        return None
    try:
        b = load_pe().get_data(va - base(), n)
    except Exception:
        return None
    return b if len(b) == n else None


def read_cstr(va, maxlen=200):
    b = read_bytes(va, 1)
    if b is None:
        return None
    try:
        data = load_pe().get_data(va - base(), maxlen)
    except Exception:
        return None
    end = data.find(b"\0")
    if end < 4:
        return None
    s = data[:end]
    if all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return s.decode("latin-1")
    return None


# ---------------------------------------------------------------- Ghidra facts (headless, read-only)
def hooks_rvas():
    """Every hooks.csv RVA: FuncFactsPC defines a transient function at each one Ghidra has no function for."""
    import csv
    out = []
    for r in csv.DictReader(open(ROOT / "hooks.csv", encoding="utf-8", errors="replace", newline="")):
        v = (r.get("rva") or "").strip()
        if v and not v.startswith("#") and all(c in "0123456789abcdefABCDEF" for c in v):
            out.append(v)
    return out


def dump_facts():
    """Acquire a pool slot, run FuncFactsPC.java on the clone (-readOnly -noanalysis), release the slot."""
    pool_sh = str(MAIN / "scripts" / "ghidra_pool.sh").replace("\\", "/")
    res = subprocess.run([_bash(), pool_sh, "acquire"], capture_output=True, text=True, timeout=600)
    lines = [l.strip() for l in res.stdout.splitlines() if l.strip()]
    if res.returncode != 0 or not lines or not lines[-1].startswith("Mashed_pool"):
        sys.stderr.write(res.stdout + res.stderr)
        raise SystemExit("pool acquire failed")
    slot = lines[-1]
    try:
        tmp = pathlib.Path(tempfile.mkdtemp(prefix="funcfacts_"))
        out, create = tmp / "facts.jsonl", tmp / "create.txt"
        create.write_text("\n".join(hooks_rvas()) + "\n", encoding="utf-8")
        cmd = [str(GH), str(MAIN / "mashed_pool"), slot, "-process", "MASHED.exe", "-noanalysis", "-readOnly",
               "-scriptPath", str(SCRIPTS), "-postScript", "FuncFactsPC.java", str(out), str(create)]
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=3600)
        if not out.exists():
            sys.stderr.write(r.stdout[-4000:] + r.stderr[-4000:])
            raise SystemExit("FuncFactsPC produced no output")
    finally:
        subprocess.run([_bash(), pool_sh, "release", slot[len("Mashed_pool"):]],
                       capture_output=True, text=True, timeout=300)
    FACTS.parent.mkdir(exist_ok=True)
    body = out.read_text(encoding="utf-8")
    FACTS.write_text(json.dumps({"slot": slot, "program": "MASHED.exe"}) + "\n" + body, encoding="utf-8")
    sys.stderr.write(f"[xref] {slot}: {body.count(chr(10))} functions -> {FACTS}\n")


def facts(rebuild=False):
    """entry VA -> {name, cc, proto, purge, thunk, ranges[(lo, hi_exclusive)], refs[(from, type, from_fn)]}."""
    if rebuild or not FACTS.exists():
        dump_facts()
    lines = FACTS.read_text(encoding="utf-8").splitlines()
    meta = json.loads(lines[0])
    out = {}
    for l in lines[1:]:
        if not l.strip():
            continue
        f = json.loads(l)
        e = int(f["entry"], 16)
        t = f["thunk"]
        out[e] = {
            "name": f["name"], "created": bool(f.get("created")), "cc": f["cc"], "proto": f["proto"], "purge": f["purge"],
            "thunk": t if (t is None or t.startswith("EXTERNAL:")) else int(t, 16),
            "ranges": [(int(a, 16), int(b, 16) + 1) for a, b in f["ranges"]],
            "refs": [(int(a, 16) if all(c in "0123456789abcdef" for c in a) else a, ty,
                      int(fn, 16) if fn else None) for a, ty, fn in f["refs"]],
        }
    return meta, out


# ---------------------------------------------------------------- call graph (capstone over the anchored bytes)
def disasm(ranges):
    """Instructions over every body range; also returns [(stop_va, range_end)] where capstone stopped early."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    pe, b = load_pe(), base()
    insns, short = [], []
    for lo, hi in ranges:
        try:
            got = list(md.disasm(pe.get_data(lo - b, hi - lo), lo))
        except Exception:
            got = []
        insns += got
        end = (got[-1].address + got[-1].size) if got else lo
        if end < hi:
            short.append((end, hi))
    return insns, short


def build(fx):
    entries = set(fx)
    img_lo, img_hi = base(), base() + load_pe().OPTIONAL_HEADER.SizeOfImage
    idx = {}
    for va, f in fx.items():
        insns, _ = disasm(f["ranges"])
        inside = lambda t: any(lo <= t < hi for lo, hi in f["ranges"])
        calls, strs, data = set(), set(), set()
        for ins in insns:
            if ins.mnemonic in ("call", "jmp") and ins.operands and ins.operands[0].type == capstone.x86.X86_OP_IMM:
                t = ins.operands[0].imm & 0xFFFFFFFF
                if t in entries and (ins.mnemonic == "call" or not inside(t)):
                    calls.add(t)
                continue
            for op in ins.operands:
                v = None
                if op.type == capstone.x86.X86_OP_IMM:
                    v = op.imm & 0xFFFFFFFF
                elif op.type == capstone.x86.X86_OP_MEM and not op.mem.base and not op.mem.index:
                    v = op.mem.disp & 0xFFFFFFFF
                if v is None or not (img_lo <= v < img_hi) or v in entries:
                    continue
                (strs if read_cstr(v) is not None else data).add(v)
        idx[va] = {"calls": sorted(calls), "strings": sorted(strs), "data": sorted(data)}
    callers = {}
    for va, f in idx.items():
        for t in f["calls"]:
            callers.setdefault(t, []).append(va)
    for va in idx:
        idx[va]["callers"] = sorted(callers.get(va, []))
    CACHE.parent.mkdir(exist_ok=True)
    CACHE.write_text(json.dumps({str(k): v for k, v in idx.items()}))
    return idx


def load(rebuild=False):
    _, fx = facts()
    if rebuild or not CACHE.exists() or CACHE.stat().st_mtime < FACTS.stat().st_mtime:
        return build(fx)
    return {int(k): v for k, v in json.loads(CACHE.read_text()).items()}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("addr", nargs="?")
    ap.add_argument("--string")
    ap.add_argument("--rebuild", action="store_true", help="rebuild the capstone call graph")
    ap.add_argument("--rebuild-facts", action="store_true", help="re-dump the Ghidra facts (pool slot)")
    a = ap.parse_args()
    check_anchor()
    if a.rebuild_facts:
        dump_facts()
    _, fx = facts()
    idx = load(a.rebuild or a.rebuild_facts)
    if a.string:
        s = int(a.string, 16)
        print(repr(read_cstr(s)))
        for va, f in sorted(idx.items()):
            if s in f["strings"] or s in f["data"]:
                print(f"  0x{va:08x} {fx[va]['name']}")
        return
    if not a.addr:
        return
    va = int(a.addr, 16)
    if va not in fx:
        raise SystemExit(f"0x{va:08x} is not a Ghidra function entry")
    f, g = idx[va], fx[va]
    print(f"0x{va:08x} {g['name']}  {g['proto']}  ranges " +
          ", ".join(f"0x{lo:08x}-0x{hi - 1:08x}" for lo, hi in g["ranges"]))
    print("  callers: " + (", ".join(f"0x{c:08x} {fx[c]['name']}" for c in f["callers"]) or "none"))
    print("  callees: " + (", ".join(f"0x{c:08x} {fx[c]['name']}" for c in f["calls"]) or "none"))
    for s in f["strings"]:
        print(f"  string 0x{s:08x}: {read_cstr(s)!r}")
    if f["data"]:
        print("  data: " + " ".join(f"0x{d:08x}" for d in f["data"][:30]))


if __name__ == "__main__":
    main()
