#!/usr/bin/env python3
r"""extract_loadicon_asset.py - one-time extractor for the loading-disc texture (D5-ICON-ASSET, L-64 (b)).

MASHED.exe carries the "loadicon" spinning-disc texture as the PE resource RWTEXDICTIONARY/404
(FUN_004921d0: FUN_004997b0(0x194,"RWTEXDICTIONARY") -> FUN_004c5c00(dict,"LoadIcon")). The standalone used to
read it at boot with LoadLibraryExA("original\\MASHED.exe", LOAD_LIBRARY_AS_DATAFILE), so the LAN release had to
ship the original exe. This tool copies the resource bytes, unchanged, into mashedmod/assets/loadicon_404.txd.
mashedmod/assets/mashed_re.rc embeds that file back into mashed_re.exe under the same type and id, and
exe_main.cpp LoadLoadIcon reads it from the exe's own module with the same decode.

Source: original/MASHED.exe.unpatched (the SHA-256 anchor) when present, else original/MASHED.exe. Resources are
not touched by any of the ten boot patches (all are .text / code caves), so either file gives the same bytes.

The resource directory is walked by hand (no pefile dependency). --check cross-reads the resource through the
Win32 loader (LoadLibraryExW AS_DATAFILE + FindResourceW), the call the old exe path made, and compares.

  py -3.12 re/tools/extract_loadicon_asset.py [--exe <MASHED.exe>] [--out <file>] [--check]
"""
import argparse
import hashlib
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, "..", ".."))
ANCHOR_SHA256 = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"
RES_TYPE = "RWTEXDICTIONARY"
RES_ID = 404
RT_DIR = 2  # IMAGE_DIRECTORY_ENTRY_RESOURCE


def sha256(b):
    return hashlib.sha256(b).hexdigest().upper()


def sections(pe, off_nt):
    nsec, = struct.unpack_from("<H", pe, off_nt + 6)
    opt_size, = struct.unpack_from("<H", pe, off_nt + 20)
    off = off_nt + 24 + opt_size
    out = []
    for i in range(nsec):
        vsize, va, rsize, rptr = struct.unpack_from("<IIII", pe, off + 40 * i + 8)
        out.append((va, max(vsize, rsize), rptr))
    return out


def rva_to_off(secs, rva):
    for va, size, rptr in secs:
        if va <= rva < va + size:
            return rptr + (rva - va)
    raise ValueError("rva 0x%x outside every section" % rva)


def read_resource(pe):
    if pe[:2] != b"MZ":
        raise ValueError("not an MZ image")
    off_nt, = struct.unpack_from("<I", pe, 0x3c)
    if pe[off_nt:off_nt + 4] != b"PE\0\0":
        raise ValueError("no PE signature")
    magic, = struct.unpack_from("<H", pe, off_nt + 24)
    if magic != 0x10b:
        raise ValueError("not PE32 (magic 0x%x)" % magic)
    dd = off_nt + 24 + 96                      # PE32 data directories
    rsrc_rva, rsrc_size = struct.unpack_from("<II", pe, dd + 8 * RT_DIR)
    secs = sections(pe, off_nt)
    base = rva_to_off(secs, rsrc_rva)

    def entries(dir_off):
        n_named, n_id = struct.unpack_from("<HH", pe, base + dir_off + 12)
        for i in range(n_named + n_id):
            name, target = struct.unpack_from("<II", pe, base + dir_off + 16 + 8 * i)
            if name & 0x80000000:
                so = base + (name & 0x7fffffff)
                ln, = struct.unpack_from("<H", pe, so)
                key = pe[so + 2:so + 2 + 2 * ln].decode("utf-16-le")
            else:
                key = name
            yield key, target

    def find(dir_off, key):
        for k, t in entries(dir_off):
            if (isinstance(k, str) and isinstance(key, str) and k.upper() == key.upper()) or k == key:
                return t
        return None

    t = find(0, RES_TYPE)
    if t is None or not t & 0x80000000:
        raise ValueError("resource type %s not found" % RES_TYPE)
    t = find(t & 0x7fffffff, RES_ID)
    if t is None or not t & 0x80000000:
        raise ValueError("resource %s/%d not found" % (RES_TYPE, RES_ID))
    langs = list(entries(t & 0x7fffffff))
    if len(langs) != 1:
        raise ValueError("expected one language entry, got %d" % len(langs))
    lang, leaf = langs[0]
    if leaf & 0x80000000:
        raise ValueError("language entry is a directory")
    data_rva, size = struct.unpack_from("<II", pe, base + leaf)
    off = rva_to_off(secs, data_rva)
    return pe[off:off + size], lang, data_rva


def win32_read(path):
    import ctypes
    from ctypes import wintypes
    k32 = ctypes.WinDLL("kernel32", use_last_error=True)
    k32.LoadLibraryExW.restype = wintypes.HMODULE
    k32.LoadLibraryExW.argtypes = [wintypes.LPCWSTR, wintypes.HANDLE, wintypes.DWORD]
    k32.FindResourceW.restype = ctypes.c_void_p
    k32.FindResourceW.argtypes = [wintypes.HMODULE, ctypes.c_void_p, wintypes.LPCWSTR]
    k32.LoadResource.restype = ctypes.c_void_p
    k32.LoadResource.argtypes = [wintypes.HMODULE, ctypes.c_void_p]
    k32.LockResource.restype = ctypes.c_void_p
    k32.LockResource.argtypes = [ctypes.c_void_p]
    k32.SizeofResource.restype = wintypes.DWORD
    k32.SizeofResource.argtypes = [wintypes.HMODULE, ctypes.c_void_p]
    k32.FreeLibrary.argtypes = [wintypes.HMODULE]
    h = k32.LoadLibraryExW(path, None, 0x2)    # LOAD_LIBRARY_AS_DATAFILE
    if not h:
        raise OSError("LoadLibraryExW failed: %d" % ctypes.get_last_error())
    try:
        hr = k32.FindResourceW(h, ctypes.c_void_p(RES_ID), RES_TYPE)
        if not hr:
            raise OSError("FindResourceW failed: %d" % ctypes.get_last_error())
        p = k32.LockResource(k32.LoadResource(h, hr))
        return ctypes.string_at(p, k32.SizeofResource(h, hr))
    finally:
        k32.FreeLibrary(h)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", help="MASHED.exe to read (default original/MASHED.exe.unpatched, else original/MASHED.exe)")
    ap.add_argument("--out", default=os.path.join(REPO, "mashedmod", "assets", "loadicon_404.txd"))
    ap.add_argument("--check", action="store_true", help="cross-read through the Win32 resource loader")
    a = ap.parse_args()

    exe = a.exe
    if not exe:
        exe = os.path.join(REPO, "original", "MASHED.exe.unpatched")
        if not os.path.isfile(exe):
            exe = os.path.join(REPO, "original", "MASHED.exe")
    with open(exe, "rb") as f:
        pe = f.read()
    h = sha256(pe)
    print("source  %s  %d bytes  sha256 %s%s" % (exe, len(pe), h, "  (anchor)" if h == ANCHOR_SHA256 else ""))
    data, lang, rva = read_resource(pe)
    print("resource %s/%d lang %s at rva 0x%08x, %d bytes, sha256 %s" % (RES_TYPE, RES_ID, lang, rva, len(data),
                                                                          sha256(data)))
    w, hgt, depth, stride = struct.unpack_from("<IIII", data, 0x2c)
    print("header  w=%d h=%d depth=%d stride=%d" % (w, hgt, depth, stride))
    if a.check:
        via = win32_read(os.path.abspath(exe))
        if via != data:
            sys.exit("[!] Win32 loader read differs (%d bytes, sha256 %s)" % (len(via), sha256(via)))
        print("check   Win32 LoadLibraryExW/FindResourceW read is byte-identical")
    os.makedirs(os.path.dirname(os.path.abspath(a.out)), exist_ok=True)
    with open(a.out, "wb") as f:
        f.write(data)
    print("[+] wrote %s" % a.out)


if __name__ == "__main__":
    main()
