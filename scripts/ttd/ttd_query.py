#!/usr/bin/env python3
r"""Headless TTD trace query via dbgeng.dll — no GUI, no AppX activation, no MCP.

The WinDbg *GUI* (DbgX.Shell.exe) can't be scripted on this machine: it re-activates
as a packaged Store app and command-line args don't survive AppX activation. But the
replay engine is just dbgeng.dll, which we can drive directly from Python: open a
.run trace, run dx/WinDbg commands, capture all engine output via OpenLogFile (so no
COM output-callback object is needed), print it.

Uses the 64-bit engine (tools\ttd_amd64\dbgeng.dll) replaying the x86 MASHED trace via
amd64\ttd\wow64\ — standard cross-bitness TTD replay. Run setup via:
    pwsh scripts\ttd\setup_recorder.ps1   (x86 recorder)
    + the amd64 engine is copied to tools\ttd_amd64\ (see README).

Usage:
    py -3.12 scripts/ttd/ttd_query.py <trace.run> -c "dx ..." [-c "..."]
    py -3.12 scripts/ttd/ttd_query.py <trace.run> -f commands.txt
"""
import argparse, ctypes, os, sys, tempfile, uuid
from ctypes import (POINTER, byref, c_long, c_void_p, c_ulong, c_uint64,
                    c_char_p, c_wchar_p, Structure, c_ubyte, c_ushort, c_uint32)

ENGINE_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                          "..", "..", "tools", "ttd_amd64"))
S_OK = 0

class GUID(Structure):
    _fields_ = [("Data1", c_uint32), ("Data2", c_ushort), ("Data3", c_ushort),
                ("Data4", c_ubyte * 8)]
    def __init__(self, s):
        ctypes.memmove(byref(self), uuid.UUID(s).bytes_le, 16)

IID_IDebugClient  = GUID("27fe5639-8407-4f47-8364-ee118fb08ac8")
IID_IDebugClient4 = GUID("ca83c3de-5089-4cf8-93c8-d892387f2a5e")
IID_IDebugControl = GUID("5182e668-105e-416e-ad92-24ef800424ba")

def _vmethod(iface, index, restype, *argtypes):
    """Bind COM vtable slot `index` on interface pointer `iface`."""
    vt = ctypes.cast(iface, POINTER(c_void_p))[0]            # *iface = vtable ptr
    fn = ctypes.cast(vt, POINTER(c_void_p))[index]           # vtable[index]
    return ctypes.WINFUNCTYPE(restype, c_void_p, *argtypes)(fn)

def _hr(name, hr):
    if hr & 0x80000000:
        raise OSError(f"{name} failed hr=0x{hr & 0xffffffff:08x}")
    return hr

def _qi(iface, iid, name):
    out = c_void_p()
    _hr(name, _vmethod(iface, 0, c_long, POINTER(GUID), POINTER(c_void_p))(
        iface, byref(iid), byref(out)))
    if not out.value:
        raise OSError(f"{name}: null interface")
    return out

def query(trace, cmds):
    trace = os.path.abspath(trace)
    if not os.path.exists(trace):
        sys.exit(f"trace not found: {trace}")
    if not os.path.exists(os.path.join(ENGINE_DIR, "dbgeng.dll")):
        sys.exit(f"engine missing: {ENGINE_DIR}\\dbgeng.dll (see scripts\\ttd\\README.md)")

    os.add_dll_directory(ENGINE_DIR)
    ttd_sub = os.path.join(ENGINE_DIR, "ttd")
    if os.path.isdir(ttd_sub):
        os.add_dll_directory(ttd_sub)
    dbgeng = ctypes.WinDLL(os.path.join(ENGINE_DIR, "dbgeng.dll"))

    DebugCreate = dbgeng.DebugCreate
    DebugCreate.argtypes = [POINTER(GUID), POINTER(c_void_p)]
    DebugCreate.restype = c_long
    client = c_void_p()
    _hr("DebugCreate", DebugCreate(byref(IID_IDebugClient), byref(client)))

    control = _qi(client, IID_IDebugControl, "QI(IDebugControl)")
    client4 = _qi(client, IID_IDebugClient4, "QI(IDebugClient4)")

    logf = tempfile.NamedTemporaryFile(delete=False, suffix=".ttdlog"); logf.close()
    logpath = logf.name
    # IDebugControl::OpenLogFile (slot 8): HRESULT(PCSTR File, BOOL Append)
    _hr("OpenLogFile", _vmethod(control, 8, c_long, c_char_p, c_long)(
        control, logpath.encode(), 0))
    # IDebugClient4::OpenDumpFileWide (slot 60): HRESULT(PCWSTR File, ULONG64 Handle)
    _hr("OpenDumpFileWide", _vmethod(client4, 60, c_long, c_wchar_p, c_uint64)(
        client4, trace, 0))
    # IDebugControl::WaitForEvent (slot 93): HRESULT(ULONG Flags, ULONG Timeout) — loads trace
    _vmethod(control, 93, c_long, c_ulong, c_ulong)(control, 0, 0xFFFFFFFF)

    # IDebugControl::Execute (slot 66): HRESULT(ULONG OutCtl, PCSTR Cmd, ULONG Flags)
    Execute = _vmethod(control, 66, c_long, c_ulong, c_char_p, c_ulong)
    for cmd in cmds:
        Execute(control, 0, cmd.encode(), 0)   # DEBUG_OUTCTL_THIS_CLIENT, DEBUG_EXECUTE_DEFAULT

    _vmethod(control, 9, c_long)(control)       # CloseLogFile (slot 9)
    with open(logpath, "r", errors="replace") as fh:
        text = fh.read()
    os.unlink(logpath)
    return text

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("trace")
    ap.add_argument("-c", action="append", default=[], dest="cmds")
    ap.add_argument("-f", dest="cmdfile")
    a = ap.parse_args()
    cmds = list(a.cmds)
    if a.cmdfile:
        with open(a.cmdfile) as fh:
            cmds += [ln.rstrip("\n") for ln in fh if ln.strip()]
    if not cmds:
        cmds = ["dx @$cursession.TTD.Calls(0x4c3b30).Count()"]
    sys.stdout.write(query(a.trace, cmds))

if __name__ == "__main__":
    main()
