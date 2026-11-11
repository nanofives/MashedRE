"""Minimal i386 COFF object editor for the DDKOBJ variant (DDKOBJ-SPIKE, 2026-11-11).

Reads an MSVC .obj (regular COFF, not /bigobj), lets the caller rename symbols, append symbols and repoint
relocations, and writes the object back. The symbol table and the string table are rebuilt; section data and
relocation entries keep their file offsets, so only the tail of the file (symtab + strtab) moves.
"""
from __future__ import annotations

import pathlib
import struct

IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_CLASS_STATIC = 3
IMAGE_SYM_ABSOLUTE = -1
DTYPE_FUNCTION = 0x20
REL_I386_DIR32 = 0x0006
REL_I386_DIR32NB = 0x0007
REL_I386_REL32 = 0x0014


class Sym:
    __slots__ = ("index", "name", "value", "sec", "type", "cls", "aux")

    def __init__(self, index, name, value, sec, typ, cls, aux):
        self.index, self.name, self.value, self.sec, self.type, self.cls, self.aux = index, name, value, sec, typ, cls, aux

    @property
    def is_func(self):
        return (self.type & 0x30) == DTYPE_FUNCTION

    @property
    def defined(self):
        return self.sec != 0

    def __repr__(self):
        return f"Sym({self.index},{self.name!r},sec={self.sec},cls={self.cls},type={self.type:#x})"


class Section:
    __slots__ = ("index", "name", "hdr_off", "size", "rawptr", "relptr", "nrel", "flags", "data", "relocs")


class Coff:
    def __init__(self, data: bytes):
        self.d = bytearray(data)
        machine, nsec, self.timestamp, symptr, nsym, opt, self.chars = struct.unpack_from("<HHIIIHH", self.d, 0)
        if machine == 0 and nsec == 0xFFFF:
            raise ValueError("bigobj COFF not supported")
        if machine != 0x14C:
            raise ValueError(f"not i386 COFF (machine {machine:#x})")
        self.symptr, self.opt = symptr, opt
        strtab = symptr + nsym * 18
        strsize = struct.unpack_from("<I", self.d, strtab)[0] if strtab + 4 <= len(self.d) else 4
        self._strtab = bytes(self.d[strtab:strtab + strsize])

        def sname(raw):
            if raw[:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", raw, 4)[0]
                return self._strtab[off:self._strtab.index(b"\0", off)].decode("latin-1")
            return raw.rstrip(b"\0").decode("latin-1")

        self.sections: list[Section] = []
        for i in range(nsec):
            o = 20 + opt + 40 * i
            s = Section()
            s.index = i + 1
            s.hdr_off = o
            raw = bytes(self.d[o:o + 8])
            if raw[:1] == b"/":
                s.name = self._strtab[int(raw[1:].rstrip(b"\0")):].split(b"\0", 1)[0].decode("latin-1")
            else:
                s.name = raw.rstrip(b"\0").decode("latin-1")
            s.size, s.rawptr, s.relptr, _ln, s.nrel, _nl, s.flags = struct.unpack_from("<IIIIHHI", self.d, o + 16)
            s.data = bytes(self.d[s.rawptr:s.rawptr + s.size]) if s.rawptr else b""
            s.relocs = [list(struct.unpack_from("<IIH", self.d, s.relptr + 10 * k)) for k in range(s.nrel)]
            self.sections.append(s)
        self.syms: list[Sym] = []
        self.slots: list[Sym | None] = []   # one entry per symbol-table slot (aux slots = None)
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            name = sname(bytes(self.d[o:o + 8]))
            value, sec, typ, cls, naux = struct.unpack_from("<IhHBB", self.d, o + 8)
            aux = bytes(self.d[o + 18:o + 18 + 18 * naux])
            s = Sym(i, name, value, sec, typ, cls, aux)
            self.syms.append(s)
            self.slots.append(s)
            self.slots.extend([None] * naux)
            i += 1 + naux
        tail_end = strtab + strsize
        if tail_end != len(self.d):
            # symtab+strtab must be the file tail for the in-place rewrite below
            raise ValueError(f"symbol/string table is not the file tail ({tail_end} != {len(self.d)})")

    @classmethod
    def load(cls, path) -> "Coff":
        return cls(pathlib.Path(path).read_bytes())

    def by_name(self, name):
        return [s for s in self.syms if s.name == name]

    def section(self, idx) -> Section:
        return self.sections[idx - 1]

    def add_symbol(self, name, value=0, sec=0, typ=0, cls=IMAGE_SYM_CLASS_EXTERNAL) -> Sym:
        idx = len(self.slots)
        s = Sym(idx, name, value, sec, typ, cls, b"")
        self.syms.append(s)
        self.slots.append(s)
        return s

    def repoint(self, old_index, new_index, skip_sections=()):
        n = 0
        for s in self.sections:
            if s.index in skip_sections:
                continue
            for r in s.relocs:
                if r[1] == old_index:
                    r[1] = new_index
                    n += 1
        return n

    def drectve(self) -> str:
        for s in self.sections:
            if s.name == ".drectve":
                return s.data.decode("latin-1")
        return ""

    def save(self, path):
        d = self.d[:self.symptr]
        # relocations in place
        for s in self.sections:
            for k, (va, si, typ) in enumerate(s.relocs):
                struct.pack_into("<IIH", d, s.relptr + 10 * k, va, si, typ)
        strtab = bytearray(b"\0\0\0\0")
        symtab = bytearray()
        for s in self.syms:
            nb = s.name.encode("latin-1")
            if len(nb) <= 8:
                raw = nb.ljust(8, b"\0")
            else:
                raw = b"\0\0\0\0" + struct.pack("<I", len(strtab))
                strtab += nb + b"\0"
            naux = len(s.aux) // 18
            symtab += raw + struct.pack("<IhHBB", s.value & 0xFFFFFFFF, s.sec, s.type, s.cls, naux) + s.aux
        struct.pack_into("<I", strtab, 0, len(strtab))
        nsym = len(symtab) // 18
        struct.pack_into("<I", d, 12, nsym)
        out = bytes(d) + bytes(symtab) + bytes(strtab)
        pathlib.Path(path).write_bytes(out)


def build_absolute_obj(path, symbols: dict[str, int]):
    """A COFF object with no sections whose symbols are absolute (section -1) externals: name -> value."""
    strtab = bytearray(b"\0\0\0\0")
    symtab = bytearray()
    # @feat.00 = 1: the object holds no exception handlers, so it stays SafeSEH-compatible
    feats = {"@feat.00": 1}
    for name, val in list(feats.items()) + sorted(symbols.items()):
        nb = name.encode()
        if len(nb) <= 8:
            raw = nb.ljust(8, b"\0")
        else:
            raw = b"\0\0\0\0" + struct.pack("<I", len(strtab))
            strtab += nb + b"\0"
        cls = 3 if name == "@feat.00" else IMAGE_SYM_CLASS_EXTERNAL
        symtab += raw + struct.pack("<IhHBB", val & 0xFFFFFFFF, IMAGE_SYM_ABSOLUTE, 0, cls, 0)
    struct.pack_into("<I", strtab, 0, len(strtab))
    hdr = struct.pack("<HHIIIHH", 0x14C, 0, 0, 20, len(symtab) // 18, 0, 0)
    pathlib.Path(path).write_bytes(hdr + bytes(symtab) + bytes(strtab))


def build_thunk_obj(path, targets: dict[str, int]):
    """One .text section of 8-byte thunks `push imm32 ; ret` (68 xx xx xx xx C3 CC CC), one external per target.
    No register is touched and the stack is balanced, so any calling convention passes through. The immediate is the
    original address (the exe is fixed-base and maps the original range), so the section carries no relocations."""
    items = sorted(targets.items())
    code = bytearray()
    for _n, a in items:
        code += b"\x68" + struct.pack("<I", a) + b"\xC3\xCC\xCC"
    strtab = bytearray(b"\0\0\0\0")
    symtab = bytearray()

    def sym(name, value, sec, typ, cls, aux=b""):
        nonlocal strtab, symtab
        nb = name.encode()
        if len(nb) <= 8:
            raw = nb.ljust(8, b"\0")
        else:
            raw = b"\0\0\0\0" + struct.pack("<I", len(strtab))
            strtab += nb + b"\0"
        symtab += raw + struct.pack("<IhHBB", value, sec, typ, cls, len(aux) // 18) + aux

    sym("@feat.00", 1, IMAGE_SYM_ABSOLUTE, 0, IMAGE_SYM_CLASS_STATIC)
    sym(".text", 0, 1, 0, IMAGE_SYM_CLASS_STATIC, struct.pack("<IHHIHBB", len(code), 0, 0, 0, 0, 0, 0).ljust(18, b"\0"))
    for k, (n, _a) in enumerate(items):
        sym(n, 8 * k, 1, DTYPE_FUNCTION, IMAGE_SYM_CLASS_EXTERNAL)
    struct.pack_into("<I", strtab, 0, len(strtab))
    hdr_size = 20 + 40
    rawptr = hdr_size
    symptr = rawptr + len(code)
    flags = 0x60000020 | 0x00400000   # CODE | EXECUTE | READ | ALIGN_8BYTES
    sec = b".text\0\0\0" + struct.pack("<IIIIIIHHI", 0, 0, len(code), rawptr, 0, 0, 0, 0, flags)
    hdr = struct.pack("<HHIIIHH", 0x14C, 1, 0, symptr, len(symtab) // 18, 0, 0)
    pathlib.Path(path).write_bytes(hdr + sec + bytes(code) + bytes(symtab) + bytes(strtab))


def build_data_obj(path, items: dict[str, bytes]):
    """One read-only .rdata section holding each blob (4-byte aligned), one external symbol per blob."""
    data = bytearray()
    offs = {}
    for n, b in sorted(items.items()):
        while len(data) % 4:
            data += b"\0"
        offs[n] = len(data)
        data += b
    strtab = bytearray(b"\0\0\0\0")
    symtab = bytearray()

    def sym(name, value, sec, typ, cls, aux=b""):
        nonlocal strtab, symtab
        nb = name.encode()
        if len(nb) <= 8:
            raw = nb.ljust(8, b"\0")
        else:
            raw = b"\0\0\0\0" + struct.pack("<I", len(strtab))
            strtab += nb + b"\0"
        symtab += raw + struct.pack("<IhHBB", value, sec, typ, cls, len(aux) // 18) + aux

    sym("@feat.00", 1, IMAGE_SYM_ABSOLUTE, 0, IMAGE_SYM_CLASS_STATIC)
    sym(".rdata", 0, 1, 0, IMAGE_SYM_CLASS_STATIC, struct.pack("<IHHIHBB", len(data), 0, 0, 0, 0, 0, 0).ljust(18, b"\0"))
    for n, o in offs.items():
        sym(n, o, 1, 0, IMAGE_SYM_CLASS_EXTERNAL)
    struct.pack_into("<I", strtab, 0, len(strtab))
    rawptr = 20 + 40
    symptr = rawptr + len(data)
    flags = 0x40000040 | 0x00300000   # INITIALIZED_DATA | READ | ALIGN_4BYTES
    sec = b".rdata\0\0" + struct.pack("<IIIIIIHHI", 0, 0, len(data), rawptr, 0, 0, 0, 0, flags)
    hdr = struct.pack("<HHIIIHH", 0x14C, 1, 0, symptr, len(symtab) // 18, 0, 0)
    pathlib.Path(path).write_bytes(hdr + sec + bytes(data) + bytes(symtab) + bytes(strtab))


def undecorate(name: str) -> str:
    n = name
    if n.startswith("_") or n.startswith("@"):
        n = n[1:]
    at = n.rfind("@")
    if at > 0 and n[at + 1:].isdigit():
        n = n[:at]
    return n


def redecorate(decorated: str, new_plain: str) -> str:
    """Apply decorated's convention (_x, _x@N, @x@N) to new_plain."""
    pre = decorated[0] if decorated[:1] in ("_", "@") else ""
    at = decorated.rfind("@")
    suf = decorated[at:] if at > 0 and decorated[at + 1:].isdigit() else ""
    return pre + new_plain + suf
