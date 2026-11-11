"""BIND-CRT-STDIO: fopen / fclose / fread / fwrite / _vsnprintf / _ctime64 / _tzset bound to the exe CRT
(Match/PortTop_CrtStdio.cpp), and fputs by code-level identity. No-chain convention: bindings_rows.apply.
usage: py -3.12 verify/portorigtop_20261109/update_bindings_bindcrtstdio.py
"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from bindings_rows import apply

T = 'BIND-CRT-STDIO: '
EV = ' (verify/portorigtop_20261109/RESULT_BIND-CRT-STDIO.md)'
ROWS = [
    (0x004a4541, 'fopen', 'MashedCrt_fopen_004a4541', 'std::fopen on PortTop_Fs_DiskPath; write modes return NULL',
     'libid identical-unlisted fopen.obj/wfopen.obj, narrow by its callers; fix_fopen contract kept (NULL, never garbage)'),
    (0x004a4254, 'fputs', 'MashedCrt_fputs_004a4254', 'std::fputs',
     'libid DIFFERS (prologue / arg-load order, 0.82); code: lock, strlen, _stbuf, __fwrite_lk, _ftbuf, (written == len) ? 0 : -1 = VC7 fputs'),
    (0x004a4368, 'fclose', 'MashedCrt_fclose_004a4368', 'std::fclose', 'libid fclose.obj identical'),
    (0x004a49cf, 'fread', 'MashedCrt_fread_004a49cf', 'std::fread',
     'libid identical-ambiguous with _fwrite; code: calls __fread_lk 0x004a48e6'),
    (0x004a4b22, 'fwrite', 'MashedCrt_fwrite_004a4b22', 'std::fwrite',
     'libid identical-ambiguous with _fread; code: calls __fwrite_lk 0x004a4a1b'),
    (0x004a3b84, '_vsnprintf', 'MashedCrt_vsnprintf_004a3b84', '_vsnprintf', 'libid vsnprint.obj identical-unlisted'),
    (0x004a43b9, '_ctime64', 'MashedCrt_ctime64_004a43b9', '_ctime64',
     'libid identical-unlisted x4 (ctime/ctime64/wctime/wctime64); code: localtime 0x004a9858 reads a 64-bit time, char* callers'),
    (0x004aa00c, '_tzset', 'MashedCrt_tzset_004aa00c', '_tzset + mirror of _timezone/_daylight/_dstbias into 0x00616670/74/78',
     'libid tzset.obj identical; the matched _ftime64 0x004a43d2 reads _timezone 0x00616670 directly'),
]
FIXED = {rva: (name, 'crt', sym + ' (Match/PortTop_CrtStdio.cpp) -> ' + tgt, T + idn + '; FILE and time-zone state owned by the exe CRT' + EV, 5, 0)
         for rva, name, sym, tgt, idn in ROWS}
if __name__ == '__main__':
    apply(T, FIXED, {})
