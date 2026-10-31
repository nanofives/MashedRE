"""COFF object / archive reader and MASHED.exe image access for library-object identity.

Adapted from SimGolf's re/tools/match.py (coff, function_from_obj) and log/c3/m36e_lib_match.py (archive members),
reduced to what a byte-level comparison needs. MASHED.exe has no base relocations, so masking is driven by the
object's relocations only.
"""
import pathlib
import struct

ROOT = pathlib.Path(__file__).resolve().parents[3]
ANCHOR = ROOT / "original" / "MASHED.exe.unpatched"
ANCHOR_SHA256 = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
IMAGE_BASE = 0x400000

# i386 relocation types
REL_DIR32, REL_DIR32NB, REL_SECTION, REL_SECREL, REL_REL32 = 0x06, 0x07, 0x0A, 0x0B, 0x14


class Image:
    """Section-mapped view of the anchored PE (read as a file, never run)."""

    def __init__(self, path=None):
        path = pathlib.Path(path or ROOT / "original" / "MASHED.exe.unpatched")
        if not path.exists():  # a worktree junctions original/ from the main checkout
            path = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
        self.data = path.read_bytes()
        import hashlib
        h = hashlib.sha256(self.data).hexdigest().upper()
        if h != ANCHOR_SHA256:
            raise SystemExit(f"anchor mismatch: {path} sha256 {h}")
        pe = struct.unpack_from("<I", self.data, 0x3c)[0]
        nsec = struct.unpack_from("<H", self.data, pe + 6)[0]
        opt = struct.unpack_from("<H", self.data, pe + 20)[0]
        self.base = struct.unpack_from("<I", self.data, pe + 24 + 28)[0]
        self.sections = []
        for i in range(nsec):
            o = pe + 24 + opt + 40 * i
            name = self.data[o:o + 8].rstrip(b"\0").decode()
            vsize, va, rsize, raw = struct.unpack_from("<IIII", self.data, o + 8)
            self.sections.append((name, self.base + va, vsize, raw, rsize))
        t = [s for s in self.sections if s[0] == ".text"][0]
        self.text_va, self.text_size = t[1], t[2]
        self.text = self.read(t[1], t[2])

    def read(self, va, n):
        for name, sva, vsize, raw, rsize in self.sections:
            if sva <= va < sva + max(vsize, rsize):
                off = va - sva
                b = self.data[raw + off: raw + min(off + n, rsize)]
                return b + bytes(n - len(b))
        raise ValueError(f"va 0x{va:08x} outside the image")

    def string_at(self, va, limit=256):
        b = self.read(va, limit)
        return b.split(b"\0")[0]


def _name(data, raw8, strtab):
    if raw8[:4] == b"\0\0\0\0":
        off = struct.unpack_from("<I", raw8, 4)[0]
        end = data.index(b"\0", strtab + off)
        return data[strtab + off:end].decode("latin-1")
    return raw8.rstrip(b"\0").decode("latin-1")


class Obj:
    """One COFF i386 object: sections (with relocations), symbols (index -> dict), functions."""

    def __init__(self, data, name="?"):
        self.name = name
        self.data = data
        mach, nsec, _, symptr, nsym, opt, _ = struct.unpack_from("<HHIIIHH", data, 0)
        if mach != 0x14c:
            raise ValueError(f"{name}: machine 0x{mach:x}")
        strtab = symptr + 18 * nsym
        self.sections = []
        for i in range(nsec):
            o = 20 + opt + 40 * i
            nm = data[o:o + 8]
            if nm[:1] == b"/":
                off = int(nm[1:].rstrip(b"\0"))
                nm = data[strtab + off:data.index(b"\0", strtab + off)]
            vsize, va, size, praw, preloc, _, nrel, _, ch = struct.unpack_from("<IIIIIIHHI", data, o + 8)
            relocs = []
            for k in range(nrel):
                rva, si, ty = struct.unpack_from("<IIH", data, preloc + 10 * k)
                relocs.append((rva, si, ty))
            body = data[praw:praw + size] if praw else bytes(size)
            self.sections.append({"name": nm.rstrip(b"\0").decode("latin-1"), "data": body, "chars": ch,
                                  "relocs": relocs, "index": i + 1})
        self.symbols = {}
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            nm = _name(data, data[o:o + 8], strtab)
            val, sec, ty, cls, naux = struct.unpack_from("<IhHBB", data, o + 8)
            self.symbols[i] = {"name": nm, "value": val, "sec": sec, "type": ty, "cls": cls, "idx": i}
            i += 1 + naux
        self.comp_id = None
        for s in self.symbols.values():
            if s["name"] == "@comp.id":
                self.comp_id = (s["value"] >> 16, s["value"] & 0xffff)

    def functions(self):
        """[{name, sec, off, size, static}] for every function symbol; size runs to the next function symbol of the
        same section (or the section end), trailing int3 fill trimmed."""
        out = []
        bysec = {}
        for s in self.symbols.values():
            if s["sec"] > 0 and (s["type"] & 0xf0) == 0x20 and s["cls"] in (2, 3):
                sec = self.sections[s["sec"] - 1]
                if sec["chars"] & 0x20:
                    bysec.setdefault(s["sec"], []).append(s)
        for secno, syms in bysec.items():
            sec = self.sections[secno - 1]
            syms.sort(key=lambda s: s["value"])
            for k, s in enumerate(syms):
                end = syms[k + 1]["value"] if k + 1 < len(syms) else len(sec["data"])
                body = sec["data"][s["value"]:end]
                n = len(body.rstrip(b"\xcc\x90"))   # int3 / nop fill between functions
                out.append({"name": s["name"], "sec": secno, "off": s["value"], "size": n, "raw_end": end,
                            "static": s["cls"] == 3, "obj": self.name})
        return out


def archive_members(blob):
    """(member name, bytes) for every member of a COFF archive (.lib)."""
    if blob[:8] != b"!<arch>\n":
        raise ValueError("not an archive")
    o, longnames = 8, b""
    while o + 60 <= len(blob):
        hdr = blob[o:o + 60]
        name = hdr[:16].decode("latin-1").strip()
        size = int(hdr[48:58].decode().strip())
        data = blob[o + 60:o + 60 + size]
        if name == "//":
            longnames = data
        elif name not in ("/", ""):
            if name.startswith("/") and name[1:].isdigit():
                off = int(name[1:])
                end = longnames.find(b"\n", off)
                end2 = longnames.find(b"\0", off)
                ends = [e for e in (end, end2) if e >= 0]
                name = longnames[off:min(ends)].decode("latin-1").rstrip("/")
            else:
                name = name.rstrip("/")
            yield name, data
        o += 60 + size + (size & 1)
