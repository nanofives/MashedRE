# Heap/import stand-ins, first target 0x004943d0 (2026-10-11)

Harness: re/frida/js/standins.js. Registry entry: `renderer_scalar_dtor`. Port: mashedmod/src/mashed_re/Render/RendererScalarDtor.cpp.

| file | run | verdict |
|---|---|---|
| selfbaseline.csv | `run_diff.py renderer_scalar_dtor --self-baseline` (reimpl arm = original) | GREEN 6/6 |
| port.csv | `run_diff.py renderer_scalar_dtor` | GREEN 6/6 |
| negctl_skipfree.csv | `MASHED_NEGCTL=dtor4943d0_skipfree` | RED 4/6 (the 4 vectors with flags bit 0 set) |
| negctl_wrongfree.csv | `MASHED_NEGCTL=dtor4943d0_wrongfree` (frees via RwGlobals+0x10c) | RED 4/6 (`free:rwfree:OBJ` vs `free:_free:OBJ`) |
| negctl_extraimport.csv | `MASHED_NEGCTL=dtor4943d0_extraimport` (extra CloseHandle(0x1234) via exe IAT 0x005cc088) | RED 6/6 |
| verify_hook_install_renderer_scalar_dtor.txt | `run_verify_hook.py renderer_scalar_dtor` (inline JMP live) | PASS, reimpl fired 2/2, observations equal path1's |

First port run (getenv inside the body) read RED 6/6: the .asi CRT's env lock showed up as an extra
EnterCriticalSection/LeaveCriticalSection pair through the .asi IAT. Fixed by reading the control switch at DLL load.
