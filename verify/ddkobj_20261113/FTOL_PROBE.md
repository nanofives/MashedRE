# FTOL probe (DDKOBJ-CRTMATH)

Probe: re/tools/ddkobj/mathprobe/ftolprobe.c, built by build_ftol.bat (DDK cl 13.00.9176 + libc __ftol, VS2022 /arch:IA32, VS2022 SSE2). 4126 inputs per mode (4096 xorshift doubles of magnitude 2^-8..2^31 plus 26 edge values and 4 NaN/Inf), x87 PC_24 and PC_53. Forms: (int)double, (int)float, (int)(d*3+0.25).

## DDK vs VS2022 /arch:IA32

| form | PC | calls | DDK vs vs_ia32 differ | first differing input |
|---|---|---|---|---|
| (int)double | PC_24 | 4126 | 8 | -2147483649.0 ddk=2147483647 vs_ia32=-2147483648 |
| (int)float | PC_24 | 4126 | 7 | 4294967296.0 ddk=0 vs_ia32=-2147483648 |
| (int)(d*3+.25) | PC_24 | 4126 | 7 | 4294967296.0 ddk=0 vs_ia32=-2147483648 |
| (int)double | PC_53 | 4126 | 8 | -2147483649.0 ddk=2147483647 vs_ia32=-2147483648 |
| (int)float | PC_53 | 4126 | 7 | 4294967296.0 ddk=0 vs_ia32=-2147483648 |
| (int)(d*3+.25) | PC_53 | 4126 | 12 | 2147483647.0 ddk=2147483645 vs_ia32=-2147483648 |

## DDK vs VS2022 SSE2

| form | PC | calls | DDK vs vs_sse2 differ | first differing input |
|---|---|---|---|---|
| (int)double | PC_24 | 4126 | 8 | -2147483649.0 ddk=2147483647 vs_sse2=-2147483648 |
| (int)float | PC_24 | 4126 | 7 | 4294967296.0 ddk=0 vs_sse2=-2147483648 |
| (int)(d*3+.25) | PC_24 | 4126 | 480 | -7381434.0 ddk=-22144302 vs_sse2=-22144301 |
| (int)double | PC_53 | 4126 | 8 | -2147483649.0 ddk=2147483647 vs_sse2=-2147483648 |
| (int)float | PC_53 | 4126 | 7 | 4294967296.0 ddk=0 vs_sse2=-2147483648 |
| (int)(d*3+.25) | PC_53 | 4126 | 12 | 2147483647.0 ddk=2147483645 vs_sse2=-2147483648 |

## Where the (int)double and /arch:IA32 differences are

```
PC_24 differ in range: 0 differ out of int32 range / NaN / Inf: 8
    -2147483649.0 ddk 2147483647 vs -2147483648
    4294967296.0 ddk 0 vs -2147483648
    1e+30 ddk 0 vs -2147483648
    -1e+30 ddk 0 vs -2147483648
    -inf ddk 0 vs -2147483648
    nan ddk 0 vs -2147483648
    nan ddk 0 vs -2147483648
    inf ddk 0 vs -2147483648
PC_53 differ in range: 0 differ out of int32 range / NaN / Inf: 8
    -2147483649.0 ddk 2147483647 vs -2147483648
    4294967296.0 ddk 0 vs -2147483648
    1e+30 ddk 0 vs -2147483648
    -1e+30 ddk 0 vs -2147483648
    -inf ddk 0 vs -2147483648
    nan ddk 0 vs -2147483648
    nan ddk 0 vs -2147483648
    inf ddk 0 vs -2147483648
```
