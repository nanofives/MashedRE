# PREREG D4-WSB-C4-BROAD-A, addendum 2: 0x0055abb0 declared-width mask on its 0x00565200 call-out

Committed before arm Y1. Written after the long census racediff of `0x0055abb0` (`C4_HOLD=400`, A4 calls 300-30000,
ON PID 16228, OFF PID 3952) returned HOLD `BEHAVIOR-DIFF`, 6 calls each side. That HOLD stands as recorded.

## What differed

OFF `[c565200,h0,h1,S+34] eax=S+34` x3, `[c565200,h0,h1,S+54] eax=S+54` x3. ON `[c565200,h0,18,S+34]` x2,
`[c565200,h0,a,S+34]` x1, `[c565200,h0,a,S+54]` x3. Only argument 2 (index 1) of the `0x00565200` call differs.

The original builds it as `mov dx, word ptr [ecx+0x20]` (`0x0055abb7`) and pushes all of EDX (`push edx`,
`0x0055abe6`), so bits 16-31 are the caller's EDX. The callee reads that argument as `mov ecx,[esp+8]` /
`and ecx, 0xffff` (`0x00565200`, `0x00565208`): only bits 0-15 reach any use. The port passes the zero-extended
`ushort` (`RwpSolverBroadphase3.cpp:522`, `:525`). The OFF value is a full dword that the tool named as a heap
pointer (`h1`), so the low 16 bits cannot be read back from the recorded samples.

This is the case of the v6 mask for `0x005646c0` arg 2 (`verify/wsbbvh_20261027/make_tool2.py` item 1).

## Arm Y1

`C4_TOOL=c4_racediff_v10.py` (`make_tool5.py`), `C4_ARGW=565200:1:ffff`, census, `C4_HOLD=400`, A4 calls 300-30000,
hook installed alone. No other option.

## Rule

`0x0055abb0` is promoted only on Y1 `C4-CLEAN`. Any other outcome keeps it C3. Re-run only for both-sides-empty or a
U-9677 crash.
