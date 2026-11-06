"""Probe: can a windowed D3D9 HAL device be created right now? Prints OK or the HRESULT."""
import ctypes
from ctypes import wintypes as W
import datetime

u32 = ctypes.WinDLL('user32', use_last_error=True)
d3d9 = ctypes.WinDLL('d3d9')
u32.CreateWindowExW.restype = W.HWND
u32.CreateWindowExW.argtypes = [W.DWORD, W.LPCWSTR, W.LPCWSTR, W.DWORD, ctypes.c_int, ctypes.c_int, ctypes.c_int,
                                ctypes.c_int, W.HWND, W.HMENU, W.HINSTANCE, W.LPVOID]
hwnd = u32.CreateWindowExW(0, 'STATIC', 'd3dprobe', 0, 0, 0, 640, 480, None, None, None, None)
d3d9.Direct3DCreate9.restype = ctypes.c_void_p
d3d = d3d9.Direct3DCreate9(32)


class PP(ctypes.Structure):
    _fields_ = [('w', W.UINT), ('h', W.UINT), ('fmt', W.UINT), ('cnt', W.UINT), ('ms', W.UINT), ('msq', W.DWORD),
                ('swap', W.UINT), ('hwnd', W.HWND), ('windowed', W.BOOL), ('autods', W.BOOL), ('dsfmt', W.UINT),
                ('flags', W.DWORD), ('rate', W.UINT), ('interval', W.UINT)]


def vfn(obj, idx, restype, *argtypes):
    vt = ctypes.cast(ctypes.c_void_p(obj), ctypes.POINTER(ctypes.POINTER(ctypes.c_void_p)))[0]
    return ctypes.WINFUNCTYPE(restype, ctypes.c_void_p, *argtypes)(vt[idx])


ts = datetime.datetime.now().strftime('%H:%M:%S')
if not d3d:
    print(ts, 'FAIL Direct3DCreate9 NULL')
else:
    pp = PP(640, 480, 0, 1, 0, 0, 1, hwnd, 1, 0, 0, 0, 0, 0)
    dev = ctypes.c_void_p()
    cd = vfn(d3d, 16, ctypes.c_long, W.UINT, W.UINT, W.HWND, W.DWORD, ctypes.POINTER(PP), ctypes.POINTER(ctypes.c_void_p))
    hr = cd(d3d, 0, 1, hwnd, 0x20, ctypes.byref(pp), ctypes.byref(dev))
    if hr == 0:
        print(ts, 'OK CreateDevice')
        vfn(dev.value, 2, ctypes.c_ulong)(dev.value)
    else:
        print(ts, 'FAIL CreateDevice hr=0x%08x' % (hr & 0xffffffff))
    vfn(d3d, 2, ctypes.c_ulong)(d3d)
u32.DestroyWindow(hwnd)
